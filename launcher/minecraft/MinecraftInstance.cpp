// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2022 Sefa Eyeoglu <contact@scrumplex.net>
 *  Copyright (C) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
 *  Copyright (C) 2023 TheKodeToad <TheKodeToad@proton.me>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "MinecraftInstance.h"
#include "Application.h"
#include "BuildConfig.h"
#include "minecraft/launch/CreateGameFolders.h"
#include "minecraft/launch/ExtractNatives.h"
#include "minecraft/launch/PrintInstanceInfo.h"
#include "settings/Setting.h"
#include "settings/SettingsObject.h"

#include "FileSystem.h"
#include "MMCTime.h"
#include "java/JavaVersion.h"
#include "pathmatcher/MultiMatcher.h"
#include "pathmatcher/RegexpMatcher.h"

#include "launch/LaunchTask.h"
#include "launch/steps/CheckJava.h"
#include "launch/steps/LookupServerAddress.h"
#include "launch/steps/PostLaunchCommand.h"
#include "launch/steps/PreLaunchCommand.h"
#include "launch/steps/QuitAfterGameStop.h"
#include "launch/steps/TextPrint.h"
#include "launch/steps/Update.h"

#include "minecraft/launch/ClaimAccount.h"
#include "minecraft/launch/LauncherPartLaunch.h"
#include "minecraft/launch/ModMinecraftJar.h"
#include "minecraft/launch/ReconstructAssets.h"
#include "minecraft/launch/ScanModFolders.h"
#include "minecraft/launch/VerifyJavaInstall.h"

#include "java/JavaUtils.h"

#include "meta/Index.h"
#include "meta/VersionList.h"

#include "icons/IconList.h"

#include "mod/ModFolderModel.h"
#include "mod/ResourcePackFolderModel.h"
#include "mod/ShaderPackFolderModel.h"
#include "mod/TexturePackFolderModel.h"

#include "WorldList.h"

#include "AssetsUtils.h"
#include "MinecraftLoad.h"
#include "MinecraftUpdate.h"
#include "PackProfile.h"
#include "minecraft/gameoptions/GameOptions.h"
#include "minecraft/update/FoldersTask.h"

#include "tools/BaseProfiler.h"

#include <QActionGroup>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#ifdef Q_OS_LINUX
#include "MangoHud.h"
#endif

#define IBUS "@im=ibus"

class OrSetting : public Setting {
    Q_OBJECT
   public:
    OrSetting(QString id, std::shared_ptr<Setting> a, std::shared_ptr<Setting> b) : Setting({ id }, false), m_a(a), m_b(b) {}
    virtual QVariant get() const
    {
        bool a = m_a->get().toBool();
        bool b = m_b->get().toBool();
        return a || b;
    }
    virtual void reset() {}
    virtual void set(QVariant value) {}

   private:
    std::shared_ptr<Setting> m_a;
    std::shared_ptr<Setting> m_b;
};

MinecraftInstance::MinecraftInstance(SettingsObjectPtr globalSettings, SettingsObjectPtr settings, const QString& rootDir)
    : BaseInstance(globalSettings, settings, rootDir)
{
    m_components.reset(new PackProfile(this));
}

void MinecraftInstance::saveNow()
{
    m_components->saveNow();
}

void MinecraftInstance::loadSpecificSettings()
{
    if (isSpecificSettingsLoaded())
        return;

    auto javaOverride = m_settings->registerSetting("OverrideJava", false);
    auto locationOverride = m_settings->registerSetting("OverrideJavaLocation", false);
    auto argsOverride = m_settings->registerSetting("OverrideJavaArgs", false);

    auto javaOrLocation = std::make_shared<OrSetting>("JavaOrLocationOverride", javaOverride, locationOverride);
    auto javaOrArgs = std::make_shared<OrSetting>("JavaOrArgsOverride", javaOverride, argsOverride);

    if (auto global_settings = globalSettings()) {
        m_settings->registerOverride(global_settings->getSetting("JavaPath"), javaOrLocation);
        m_settings->registerOverride(global_settings->getSetting("JvmArgs"), javaOrArgs);
        m_settings->registerOverride(global_settings->getSetting("IgnoreJavaCompatibility"), javaOrLocation);

        m_settings->registerPassthrough(global_settings->getSetting("JavaSignature"), javaOrLocation);
        m_settings->registerPassthrough(global_settings->getSetting("JavaArchitecture"), javaOrLocation);
        m_settings->registerPassthrough(global_settings->getSetting("JavaRealArchitecture"), javaOrLocation);
        m_settings->registerPassthrough(global_settings->getSetting("JavaVersion"), javaOrLocation);
        m_settings->registerPassthrough(global_settings->getSetting("JavaVendor"), javaOrLocation);

        auto windowSetting = m_settings->registerSetting("OverrideWindow", false);
        m_settings->registerOverride(global_settings->getSetting("LaunchMaximized"), windowSetting);
        m_settings->registerOverride(global_settings->getSetting("MinecraftWinWidth"), windowSetting);
        m_settings->registerOverride(global_settings->getSetting("MinecraftWinHeight"), windowSetting);

        auto memorySetting = m_settings->registerSetting("OverrideMemory", false);
        m_settings->registerOverride(global_settings->getSetting("MinMemAlloc"), memorySetting);
        m_settings->registerOverride(global_settings->getSetting("MaxMemAlloc"), memorySetting);
        m_settings->registerOverride(global_settings->getSetting("PermGen"), memorySetting);

        auto nativeLibraryWorkaroundsOverride = m_settings->registerSetting("OverrideNativeWorkarounds", false);
        m_settings->registerOverride(global_settings->getSetting("UseNativeOpenAL"), nativeLibraryWorkaroundsOverride);
        m_settings->registerOverride(global_settings->getSetting("CustomOpenALPath"), nativeLibraryWorkaroundsOverride);
        m_settings->registerOverride(global_settings->getSetting("UseNativeGLFW"), nativeLibraryWorkaroundsOverride);
        m_settings->registerOverride(global_settings->getSetting("CustomGLFWPath"), nativeLibraryWorkaroundsOverride);

        auto performanceOverride = m_settings->registerSetting("OverridePerformance", false);
        m_settings->registerOverride(global_settings->getSetting("EnableFeralGamemode"), performanceOverride);
        m_settings->registerOverride(global_settings->getSetting("EnableMangoHud"), performanceOverride);
        m_settings->registerOverride(global_settings->getSetting("UseDiscreteGpu"), performanceOverride);

        auto miscellaneousOverride = m_settings->registerSetting("OverrideMiscellaneous", false);
        m_settings->registerOverride(global_settings->getSetting("CloseAfterLaunch"), miscellaneousOverride);
        m_settings->registerOverride(global_settings->getSetting("QuitAfterGameStop"), miscellaneousOverride);

        auto legacySettings = m_settings->registerSetting("OverrideLegacySettings", false);
        m_settings->registerOverride(global_settings->getSetting("OnlineFixes"), legacySettings);

        auto envSetting = m_settings->registerSetting("OverrideEnv", false);
        m_settings->registerOverride(global_settings->getSetting("Env"), envSetting);

        m_settings->set("InstanceType", "OneSix");
    }

    m_settings->registerSetting("JoinServerOnLaunch", false);
    m_settings->registerSetting("JoinServerOnLaunchAddress", "");

    m_settings->registerSetting("UseAccountForInstance", false);
    m_settings->registerSetting("InstanceAccountId", "");

    m_settings->registerSetting("ExportName", "");
    m_settings->registerSetting("ExportVersion", "1.0.0");
    m_settings->registerSetting("ExportSummary", "");
    m_settings->registerSetting("ExportAuthor", "");
    m_settings->registerSetting("ExportOptionalFiles", true);
    m_settings->registerSetting("InitializedLaunchConfigurations", QStringList());

    qDebug() << "Instance-type specific settings were loaded!";

    setSpecificSettingsLoaded(true);

    updateRuntimeContext();
}

void MinecraftInstance::updateRuntimeContext()
{
    m_runtimeContext.updateFromInstanceSettings(m_settings);
}

QString MinecraftInstance::typeName() const
{
    return "Minecraft";
}

std::shared_ptr<PackProfile> MinecraftInstance::getPackProfile() const
{
    return m_components;
}

QSet<QString> MinecraftInstance::traits() const
{
    auto components = getPackProfile();
    if (!components) {
        return { "version-incomplete" };
    }
    auto profile = components->getProfile();
    if (!profile) {
        return { "version-incomplete" };
    }
    return profile->getTraits();
}

void MinecraftInstance::populateLaunchMenu(QMenu* menu)
{
    QAction* normalLaunchOffline = menu->addAction(tr("Launch &Offline"));
    normalLaunchOffline->setShortcut(QKeySequence::Open);
    QAction* normalLaunchOnline = menu->addAction(tr("Launch &Online"));
    normalLaunchOnline->setShortcut(QKeySequence(tr("Ctrl+Shift+L")));
    QAction* normalLaunchDemo = menu->addAction(tr("Launch &Demo"));
    normalLaunchDemo->setShortcut(QKeySequence(tr("Ctrl+Alt+O")));

    normalLaunchDemo->setEnabled(supportsDemo());

    connect(normalLaunchOffline, &QAction::triggered, [this] { APPLICATION->launch(shared_from_this(), false, false); });
    connect(normalLaunchOnline, &QAction::triggered, [this] { APPLICATION->launch(shared_from_this(), true, false); });
    connect(normalLaunchDemo, &QAction::triggered, [this] { APPLICATION->launch(shared_from_this(), false, true); });

    QString profilersTitle = tr("Profilers");
    menu->addSeparator()->setText(profilersTitle);

    auto profilers = new QActionGroup(menu);
    profilers->setExclusive(true);
    connect(profilers, &QActionGroup::triggered, [this](QAction* action) {
        settings()->set("Profiler", action->data());
        emit profilerChanged();
    });

    QAction* noProfilerAction = menu->addAction(tr("&No Profiler"));
    noProfilerAction->setData("");
    noProfilerAction->setCheckable(true);
    noProfilerAction->setChecked(true);
    profilers->addAction(noProfilerAction);

    for (auto profiler = APPLICATION->profilers().begin(); profiler != APPLICATION->profilers().end(); profiler++) {
        QAction* profilerAction = menu->addAction(profiler.value()->name());
        profilers->addAction(profilerAction);
        profilerAction->setData(profiler.key());
        profilerAction->setCheckable(true);
        profilerAction->setChecked(settings()->get("Profiler").toString() == profiler.key());

        QString error;
        profilerAction->setEnabled(profiler.value()->check(&error));
    }
}

QString MinecraftInstance::gameRoot() const
{
    QFileInfo mcDir(FS::PathCombine(instanceRoot(), "minecraft"));
    QFileInfo dotMCDir(FS::PathCombine(instanceRoot(), ".minecraft"));

    if (mcDir.exists() && !dotMCDir.exists())
        return mcDir.filePath();
    else
        return dotMCDir.filePath();
}

QString MinecraftInstance::binRoot() const
{
    return FS::PathCombine(gameRoot(), "bin");
}

QString MinecraftInstance::getNativePath() const
{
    QDir natives_dir(FS::PathCombine(instanceRoot(), "natives/"));
    return natives_dir.absolutePath();
}

QString MinecraftInstance::getLocalLibraryPath() const
{
    QDir libraries_dir(FS::PathCombine(instanceRoot(), "libraries/"));
    return libraries_dir.absolutePath();
}

bool MinecraftInstance::supportsDemo() const
{
    Version instance_ver{ getPackProfile()->getComponentVersion("net.minecraft") };

    return instance_ver >= Version("1.3.1");
}

QString MinecraftInstance::jarModsDir() const
{
    QDir jarmods_dir(FS::PathCombine(instanceRoot(), "jarmods/"));
    return jarmods_dir.absolutePath();
}

QString MinecraftInstance::modsRoot() const
{
    return FS::PathCombine(gameRoot(), "mods");
}

QString MinecraftInstance::modsCacheLocation() const
{
    return FS::PathCombine(instanceRoot(), "mods.cache");
}

QString MinecraftInstance::coreModsDir() const
{
    return FS::PathCombine(gameRoot(), "coremods");
}

QString MinecraftInstance::nilModsDir() const
{
    return FS::PathCombine(gameRoot(), "nilmods");
}

QString MinecraftInstance::resourcePacksDir() const
{
    return FS::PathCombine(gameRoot(), "resourcepacks");
}

QString MinecraftInstance::texturePacksDir() const
{
    return FS::PathCombine(gameRoot(), "texturepacks");
}

QString MinecraftInstance::shaderPacksDir() const
{
    return FS::PathCombine(gameRoot(), "shaderpacks");
}

QString MinecraftInstance::instanceConfigFolder() const
{
    return FS::PathCombine(gameRoot(), "config");
}

QString MinecraftInstance::libDir() const
{
    return FS::PathCombine(gameRoot(), "lib");
}

QString MinecraftInstance::worldDir() const
{
    return FS::PathCombine(gameRoot(), "saves");
}

QString MinecraftInstance::resourcesDir() const
{
    return FS::PathCombine(gameRoot(), "resources");
}

QDir MinecraftInstance::librariesPath() const
{
    return QDir::current().absoluteFilePath("libraries");
}

QDir MinecraftInstance::jarmodsPath() const
{
    return QDir(jarModsDir());
}

QDir MinecraftInstance::versionsPath() const
{
    return QDir::current().absoluteFilePath("versions");
}

QStringList MinecraftInstance::getClassPath()
{
    QStringList jars, nativeJars;
    auto profile = m_components->getProfile();
    profile->getLibraryFiles(runtimeContext(), jars, nativeJars, getLocalLibraryPath(), binRoot());
    return jars;
}

QString MinecraftInstance::getMainClass() const
{
    auto profile = m_components->getProfile();
    return profile->getMainClass();
}

QStringList MinecraftInstance::getNativeJars()
{
    QStringList jars, nativeJars;
    auto profile = m_components->getProfile();
    profile->getLibraryFiles(runtimeContext(), jars, nativeJars, getLocalLibraryPath(), binRoot());
    return nativeJars;
}

QStringList MinecraftInstance::extraArguments()
{
    auto list = BaseInstance::extraArguments();
    auto version = getPackProfile();
    if (!version)
        return list;
    auto jarMods = getJarMods();
    if (!jarMods.isEmpty()) {
        list.append({ "-Dfml.ignoreInvalidMinecraftCertificates=true", "-Dfml.ignorePatchDiscrepancies=true" });
    }
    auto addn = m_components->getProfile()->getAddnJvmArguments();
    if (!addn.isEmpty()) {
        list.append(addn);
    }
    auto agents = m_components->getProfile()->getAgents();
    for (auto agent : agents) {
        QStringList jar, temp1, temp2, temp3;
        agent->library()->getApplicableFiles(runtimeContext(), jar, temp1, temp2, temp3, getLocalLibraryPath());
        list.append("-javaagent:" + jar[0] + (agent->argument().isEmpty() ? "" : "=" + agent->argument()));
    }

    {
        QString openALPath;
        QString glfwPath;

        if (settings()->get("UseNativeOpenAL").toBool()) {
            openALPath = APPLICATION->m_detectedOpenALPath;
            auto customPath = settings()->get("CustomOpenALPath").toString();
            if (!customPath.isEmpty())
                openALPath = customPath;
        }
        if (settings()->get("UseNativeGLFW").toBool()) {
            glfwPath = APPLICATION->m_detectedGLFWPath;
            auto customPath = settings()->get("CustomGLFWPath").toString();
            if (!customPath.isEmpty())
                glfwPath = customPath;
        }

        QFileInfo openALInfo(openALPath);
        QFileInfo glfwInfo(glfwPath);

        if (!openALPath.isEmpty() && openALInfo.exists())
            list.append("-Dorg.lwjgl.openal.libname=" + openALInfo.absoluteFilePath());
        if (!glfwPath.isEmpty() && glfwInfo.exists())
            list.append("-Dorg.lwjgl.glfw.libname=" + glfwInfo.absoluteFilePath());
    }

    return list;
}

QStringList MinecraftInstance::javaArguments()
{
    QStringList args;

    args.append(extraArguments());

#ifdef Q_OS_MAC
    args << "-Xdock:icon=icon.png";
    args << QString("-Xdock:name=\"%1\"").arg(windowTitle());
#endif
    auto traits_ = traits();

#ifdef Q_OS_MAC
    if (traits_.contains("FirstThreadOnMacOS")) {
        args << QString("-XstartOnFirstThread");
    }
#endif

#ifdef Q_OS_WIN32
    args << QString(
        "-XX:HeapDumpPath=MojangTricksIntelDriversForPerformance_javaw.exe_"
        "minecraft.exe.heapdump");
#endif

    int min = settings()->get("MinMemAlloc").toInt();
    int max = settings()->get("MaxMemAlloc").toInt();
    if (min < max) {
        args << QString("-Xms%1m").arg(min);
        args << QString("-Xmx%1m").arg(max);
    } else {
        args << QString("-Xms%1m").arg(max);
        args << QString("-Xmx%1m").arg(min);
    }

    JavaVersion javaVersion = getJavaVersion();
    if (javaVersion.requiresPermGen()) {
        auto permgen = settings()->get("PermGen").toInt();
        if (permgen != 64) {
            args << QString("-XX:PermSize=%1m").arg(permgen);
        }
    }

    args << "-Duser.language=en";

    if (javaVersion.isModular() && shouldApplyOnlineFixes())

        args << "--add-opens"
             << "java.base/java.net=ALL-UNNAMED";

    return args;
}

QString MinecraftInstance::getLauncher()
{

    if (traits().contains("legacyLaunch") || traits().contains("alphaLaunch"))
        return "legacy";

    return "standard";
}

bool MinecraftInstance::shouldApplyOnlineFixes()
{
    return traits().contains("legacyServices") && settings()->get("OnlineFixes").toBool();
}

QMap<QString, QString> MinecraftInstance::getVariables()
{
    QMap<QString, QString> out;
    out.insert("INST_NAME", name());
    out.insert("INST_ID", id());
    out.insert("INST_DIR", QDir::toNativeSeparators(QDir(instanceRoot()).absolutePath()));
    out.insert("INST_MC_DIR", QDir::toNativeSeparators(QDir(gameRoot()).absolutePath()));
    out.insert("INST_JAVA", settings()->get("JavaPath").toString());
    out.insert("INST_JAVA_ARGS", javaArguments().join(' '));
    out.insert("NO_COLOR", "1");
    return out;
}

QProcessEnvironment MinecraftInstance::createEnvironment()
{

    QProcessEnvironment env = CleanEnviroment();

    auto variables = getVariables();
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        env.insert(it.key(), it.value());
    }

    auto insertEnv = [&env](QMap<QString, QVariant> envMap) {
        if (envMap.isEmpty())
            return;

        for (auto iter = envMap.begin(); iter != envMap.end(); iter++)
            env.insert(iter.key(), iter.value().toString());
    };

    bool overrideEnv = settings()->get("OverrideEnv").toBool();

    if (!overrideEnv)
        insertEnv(APPLICATION->settings()->get("Env").toMap());
    else
        insertEnv(settings()->get("Env").toMap());
    return env;
}

QProcessEnvironment MinecraftInstance::createLaunchEnvironment()
{

    QProcessEnvironment env = createEnvironment();

#ifdef Q_OS_LINUX
    if (settings()->get("EnableMangoHud").toBool() && APPLICATION->capabilities() & Application::SupportsMangoHud) {
        QStringList preloadList;
        if (auto value = env.value("LD_PRELOAD"); !value.isEmpty())
            preloadList = value.split(QLatin1String(":"));

        auto mangoHudLibString = MangoHud::getLibraryString();
        if (!mangoHudLibString.isEmpty()) {
            QFileInfo mangoHudLib(mangoHudLibString);
            QString libPath = mangoHudLib.absolutePath();
            auto appendLib = [libPath, &preloadList](QString fileName) {
                if (QFileInfo(FS::PathCombine(libPath, fileName)).exists())
                    preloadList << FS::PathCombine(libPath, fileName);
            };

            appendLib("libMangoHud_dlsym.so");
            appendLib("libMangoHud_opengl.so");
            appendLib(mangoHudLib.fileName());
        }

        env.insert("LD_PRELOAD", preloadList.join(QLatin1String(":")));
        env.insert("MANGOHUD", "1");
    }

    if (settings()->get("UseDiscreteGpu").toBool()) {

        env.insert("DRI_PRIME", "1");

        env.insert("__NV_PRIME_RENDER_OFFLOAD", "1");
        env.insert("__VK_LAYER_NV_optimus", "NVIDIA_only");
        env.insert("__GLX_VENDOR_LIBRARY_NAME", "nvidia");
    }
#endif
    return env;
}

static QString replaceTokensIn(QString text, QMap<QString, QString> with)
{

    QString result;
    QRegularExpression token_regexp("\\$\\{(.+)\\}", QRegularExpression::InvertedGreedinessOption);
    QStringList list;
    QRegularExpressionMatchIterator i = token_regexp.globalMatch(text);
    int lastCapturedEnd = 0;
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        result.append(text.mid(lastCapturedEnd, match.capturedStart()));
        QString key = match.captured(1);
        auto iter = with.find(key);
        if (iter != with.end()) {
            result.append(*iter);
        }
        lastCapturedEnd = match.capturedEnd();
    }
    result.append(text.mid(lastCapturedEnd));
    return result;
}

QStringList MinecraftInstance::processMinecraftArgs(AuthSessionPtr session, MinecraftServerTargetPtr serverToJoin) const
{
    auto profile = m_components->getProfile();
    QString args_pattern = profile->getMinecraftArguments();
    for (auto tweaker : profile->getTweakers()) {
        args_pattern += " --tweakClass " + tweaker;
    }

    if (serverToJoin && !serverToJoin->address.isEmpty()) {
        if (profile->hasTrait("feature:is_quick_play_multiplayer")) {
            args_pattern += " --quickPlayMultiplayer " + serverToJoin->address + ':' + QString::number(serverToJoin->port);
        } else {
            args_pattern += " --server " + serverToJoin->address;
            args_pattern += " --port " + QString::number(serverToJoin->port);
        }
    }

    QMap<QString, QString> token_mapping;

    if (session) {

        token_mapping["auth_session"] = session->session;
        token_mapping["auth_access_token"] = session->access_token;
        token_mapping["auth_player_name"] = session->player_name;
        token_mapping["auth_uuid"] = session->uuid;
        token_mapping["user_properties"] = session->serializeUserProperties();
        token_mapping["user_type"] = session->user_type;
        if (session->demo) {
            args_pattern += " --demo";
        }
    }

    token_mapping["profile_name"] = name();
    token_mapping["version_name"] = profile->getMinecraftVersion();
    token_mapping["version_type"] = profile->getMinecraftVersionType();

    QString absRootDir = QDir(gameRoot()).absolutePath();
    token_mapping["game_directory"] = absRootDir;
    QString absAssetsDir = QDir("assets/").absolutePath();
    auto assets = profile->getMinecraftAssets();
    token_mapping["game_assets"] = AssetsUtils::getAssetsDir(assets->id, resourcesDir()).absolutePath();

    token_mapping["assets_root"] = absAssetsDir;
    token_mapping["assets_index_name"] = assets->id;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList parts = args_pattern.split(' ', Qt::SkipEmptyParts);
#else
    QStringList parts = args_pattern.split(' ', QString::SkipEmptyParts);
#endif
    for (int i = 0; i < parts.length(); i++) {
        parts[i] = replaceTokensIn(parts[i], token_mapping);
    }
    return parts;
}

QString MinecraftInstance::createLaunchScript(AuthSessionPtr session, MinecraftServerTargetPtr serverToJoin)
{
    QString launchScript;

    if (!m_components)
        return QString();
    auto profile = m_components->getProfile();
    if (!profile)
        return QString();

    auto mainClass = getMainClass();
    if (!mainClass.isEmpty()) {
        launchScript += "mainClass " + mainClass + "\n";
    }
    auto appletClass = profile->getAppletClass();
    if (!appletClass.isEmpty()) {
        launchScript += "appletClass " + appletClass + "\n";
    }

    if (serverToJoin && !serverToJoin->address.isEmpty()) {
        launchScript += "serverAddress " + serverToJoin->address + "\n";
        launchScript += "serverPort " + QString::number(serverToJoin->port) + "\n";
    }

    for (auto param : processMinecraftArgs(session, nullptr  
                                           )) {
        launchScript += "param " + param + "\n";
    }

    {
        QString windowParams;
        if (settings()->get("LaunchMaximized").toBool())
            windowParams = "maximized";
        else
            windowParams =
                QString("%1x%2").arg(settings()->get("MinecraftWinWidth").toInt()).arg(settings()->get("MinecraftWinHeight").toInt());
        launchScript += "windowTitle " + windowTitle() + "\n";
        launchScript += "windowParams " + windowParams + "\n";
    }

    {
        launchScript += "launcherBrand " + BuildConfig.LAUNCHER_NAME + "\n";
        launchScript += "launcherVersion " + BuildConfig.printableVersionString() + "\n";
    }

    {
        launchScript += "instanceName " + name() + "\n";
        launchScript += "instanceIconKey " + name() + "\n";
        launchScript += "instanceIconPath icon.png\n";   
    }

    if (session) {
        launchScript += "userName " + session->player_name + "\n";
        launchScript += "sessionId " + session->session + "\n";
    }

    for (auto trait : profile->getTraits()) {
        launchScript += "traits " + trait + "\n";
    }

    if (shouldApplyOnlineFixes())
        launchScript += "onlineFixes true\n";

    launchScript += "launcher " + getLauncher() + "\n";

    return launchScript;
}

QStringList MinecraftInstance::verboseDescription(AuthSessionPtr session, MinecraftServerTargetPtr serverToJoin)
{
    QStringList out;
    out << "Main Class:"
        << "  " + getMainClass() << "";
    out << "Native path:"
        << "  " + getNativePath() << "";

    auto profile = m_components->getProfile();

    auto alltraits = traits();
    if (alltraits.size()) {
        out << "Traits:";
        for (auto trait : alltraits) {
            out << "traits " + trait;
        }
        out << "";
    }

    auto settings = this->settings();
    bool nativeOpenAL = settings->get("UseNativeOpenAL").toBool();
    bool nativeGLFW = settings->get("UseNativeGLFW").toBool();
    if (nativeOpenAL || nativeGLFW) {
        if (nativeOpenAL)
            out << "Using system OpenAL.";
        if (nativeGLFW)
            out << "Using system GLFW.";
        out << "";
    }

    {
        out << "Libraries:";
        QStringList jars, nativeJars;
        profile->getLibraryFiles(runtimeContext(), jars, nativeJars, getLocalLibraryPath(), binRoot());
        auto printLibFile = [&](const QString& path) {
            QFileInfo info(path);
            if (info.exists()) {
                out << "  " + path;
            } else {
                out << "  " + path + " (missing)";
            }
        };
        for (auto file : jars) {
            printLibFile(file);
        }
        out << "";
        out << "Native libraries:";
        for (auto file : nativeJars) {
            printLibFile(file);
        }
        out << "";
    }

    auto printModList = [&](const QString& label, ModFolderModel& model) {
        if (model.size()) {
            out << QString("%1:").arg(label);
            auto modList = model.allMods();
            std::sort(modList.begin(), modList.end(), [](auto a, auto b) {
                auto aName = a->fileinfo().completeBaseName();
                auto bName = b->fileinfo().completeBaseName();
                return aName.localeAwareCompare(bName) < 0;
            });
            for (auto mod : modList) {
                if (mod->type() == ResourceType::FOLDER) {
                    out << u8"  [🖿] " + mod->fileinfo().completeBaseName() + " (folder)";
                    continue;
                }

                if (mod->enabled()) {
                    out << u8"  [✔] " + mod->fileinfo().completeBaseName();
                } else {
                    out << u8"  [✘] " + mod->fileinfo().completeBaseName() + " (disabled)";
                }
            }
            out << "";
        }
    };

    printModList("Mods", *(loaderModList().get()));
    printModList("Core Mods", *(coreModList().get()));

    auto& jarMods = profile->getJarMods();
    if (jarMods.size()) {
        out << "Jar Mods:";
        for (auto& jarmod : jarMods) {
            auto displayname = jarmod->displayName(runtimeContext());
            auto realname = jarmod->filename(runtimeContext());
            if (displayname != realname) {
                out << "  " + displayname + " (" + realname + ")";
            } else {
                out << "  " + realname;
            }
        }
        out << "";
    }

    auto params = processMinecraftArgs(nullptr, serverToJoin);
    out << "Params:";
    out << "  " + params.join(' ');
    out << "";

    QString windowParams;
    if (settings->get("LaunchMaximized").toBool()) {
        out << "Window size: max (if available)";
    } else {
        auto width = settings->get("MinecraftWinWidth").toInt();
        auto height = settings->get("MinecraftWinHeight").toInt();
        out << "Window size: " + QString::number(width) + " x " + QString::number(height);
    }
    out << "";
    out << "Launcher: " + getLauncher();
    out << "";
    return out;
}

QMap<QString, QString> MinecraftInstance::createCensorFilterFromSession(AuthSessionPtr session)
{
    if (!session) {
        return QMap<QString, QString>();
    }
    auto& sessionRef = *session.get();
    QMap<QString, QString> filter;
    auto addToFilter = [&filter](QString key, QString value) {
        if (key.trimmed().size()) {
            filter[key] = value;
        }
    };
    if (sessionRef.session != "-") {
        addToFilter(sessionRef.session, tr("<SESSION ID>"));
    }
    if (sessionRef.access_token != "0") {
        addToFilter(sessionRef.access_token, tr("<ACCESS TOKEN>"));
    }
    addToFilter(sessionRef.uuid, tr("<PROFILE ID>"));

    return filter;
}

MessageLevel::Enum MinecraftInstance::guessLevel(const QString& line, MessageLevel::Enum level)
{
    QRegularExpression re("\\[(?<timestamp>[0-9:]+)\\] \\[[^/]+/(?<level>[^\\]]+)\\]");
    auto match = re.match(line);
    if (match.hasMatch()) {

        QString timestamp = match.captured("timestamp");
        QString levelStr = match.captured("level");
        if (levelStr == "INFO")
            level = MessageLevel::Message;
        if (levelStr == "WARN")
            level = MessageLevel::Warning;
        if (levelStr == "ERROR")
            level = MessageLevel::Error;
        if (levelStr == "FATAL")
            level = MessageLevel::Fatal;
        if (levelStr == "TRACE" || levelStr == "DEBUG")
            level = MessageLevel::Debug;
    } else {

        if (line.contains("[INFO]") || line.contains("[CONFIG]") || line.contains("[FINE]") || line.contains("[FINER]") ||
            line.contains("[FINEST]"))
            level = MessageLevel::Message;
        if (line.contains("[SEVERE]") || line.contains("[STDERR]"))
            level = MessageLevel::Error;
        if (line.contains("[WARNING]"))
            level = MessageLevel::Warning;
        if (line.contains("[DEBUG]"))
            level = MessageLevel::Debug;
    }
    if (line.contains("overwriting existing"))
        return MessageLevel::Fatal;

    static const QString javaSymbol = "([a-zA-Z_$][a-zA-Z\\d_$]*\\.)+[a-zA-Z_$][a-zA-Z\\d_$]*";
    if (line.contains("Exception in thread") || line.contains(QRegularExpression("\\s+at " + javaSymbol)) ||
        line.contains(QRegularExpression("Caused by: " + javaSymbol)) ||
        line.contains(QRegularExpression("([a-zA-Z_$][a-zA-Z\\d_$]*\\.)+[a-zA-Z_$]?[a-zA-Z\\d_$]*(Exception|Error|Throwable)")) ||
        line.contains(QRegularExpression("... \\d+ more$")))
        return MessageLevel::Error;
    return level;
}

IPathMatcher::Ptr MinecraftInstance::getLogFileMatcher()
{
    auto combined = std::make_shared<MultiMatcher>();
    combined->add(std::make_shared<RegexpMatcher>(".*\\.log(\\.[0-9]*)?(\\.gz)?$"));
    combined->add(std::make_shared<RegexpMatcher>("crash-.*\\.txt"));
    combined->add(std::make_shared<RegexpMatcher>("IDMap dump.*\\.txt$"));
    combined->add(std::make_shared<RegexpMatcher>("ModLoader\\.txt(\\..*)?$"));
    return combined;
}

QString MinecraftInstance::getLogFileRoot()
{
    return gameRoot();
}

QString MinecraftInstance::getStatusbarDescription()
{
    QStringList traits;
    if (hasVersionBroken()) {
        traits.append(tr("broken"));
    }

    QString mcVersion = m_components->getComponentVersion("net.minecraft");
    if (mcVersion.isEmpty()) {

        auto loadTask = m_components->loadOffline();
        if (loadTask && !loadTask->isRunning()) {
            loadTask->start();
        }
        mcVersion = m_components->getComponentVersion("net.minecraft");
    }

    QString description;
    description.append(tr("Minecraft %1").arg(mcVersion));
    if (m_settings->get("ShowGameTime").toBool()) {
        if (lastTimePlayed() > 0 && lastLaunch() > 0) {
            QDateTime lastLaunchTime = QDateTime::fromMSecsSinceEpoch(lastLaunch());
            description.append(
                tr(", last played on %1 for %2")
                    .arg(QLocale().toString(lastLaunchTime, QLocale::ShortFormat))
                    .arg(Time::prettifyDuration(lastTimePlayed(), APPLICATION->settings()->get("ShowGameTimeWithoutDays").toBool())));
        }

        if (totalTimePlayed() > 0) {
            description.append(
                tr(", total played for %1")
                    .arg(Time::prettifyDuration(totalTimePlayed(), APPLICATION->settings()->get("ShowGameTimeWithoutDays").toBool())));
        }
    }
    if (hasCrashed()) {
        description.append(tr(", has crashed."));
    }
    return description;
}

Task::Ptr MinecraftInstance::createUpdateTask(Net::Mode mode)
{
    updateRuntimeContext();
    switch (mode) {
        case Net::Mode::Offline: {
            return Task::Ptr(new MinecraftLoad(this));
        }
        case Net::Mode::Online: {
            return Task::Ptr(new MinecraftUpdate(this));
        }
    }
    return nullptr;
}

QString MinecraftInstance::launchConfigurationFingerprint()
{

    m_components->saveNow();

    QFile componentsFile(FS::PathCombine(instanceRoot(), "mmc-pack.json"));
    if (!componentsFile.open(QFile::ReadOnly)) {
        qWarning() << "Could not read the component configuration for" << name() << componentsFile.errorString();
        return {};
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(componentsFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        qWarning() << "Could not parse the component configuration for" << name() << parseError.errorString();
        return {};
    }

    const auto root = document.object();
    if (!root.value("components").isArray()) {
        return {};
    }

    QJsonArray configuration;
    const auto components = root.value("components").toArray();
    for (const auto& value : components) {
        if (!value.isObject()) {
            return {};
        }

        const auto source = value.toObject();
        const auto uid = source.value("uid").toString();
        if (uid.isEmpty()) {
            return {};
        }

        QJsonObject component;
        component.insert("uid", uid);
        component.insert("version", source.value("version").toString());
        component.insert("disabled", source.value("disabled").toBool());
        component.insert("dependencyOnly", source.value("dependencyOnly").toBool());
        component.insert("important", source.value("important").toBool());

        QFile patchFile(FS::PathCombine(instanceRoot(), QString("patches/%1.json").arg(uid)));
        if (QFileInfo::exists(patchFile.fileName())) {
            if (!patchFile.open(QFile::ReadOnly)) {
                qWarning() << "Could not read component patch" << patchFile.fileName() << patchFile.errorString();
                return {};
            }
            const auto patchHash = QCryptographicHash::hash(patchFile.readAll(), QCryptographicHash::Sha256).toHex();
            component.insert("patch", QString::fromLatin1(patchHash));
        }
        configuration.append(component);
    }

    QJsonObject fingerprintData;
    fingerprintData.insert("formatVersion", root.value("formatVersion"));
    fingerprintData.insert("DownloadResources", root.value("DownloadResources").toBool(true));
    fingerprintData.insert("components", configuration);
    const auto serialized = QJsonDocument(fingerprintData).toJson(QJsonDocument::Compact);
    return QString::fromLatin1(QCryptographicHash::hash(serialized, QCryptographicHash::Sha256).toHex());
}

bool MinecraftInstance::isCurrentConfigurationInitialized()
{
    const auto fingerprint = launchConfigurationFingerprint();
    if (fingerprint.isEmpty()) {
        return false;
    }
    return settings()->get("InitializedLaunchConfigurations").toStringList().contains(fingerprint);
}

void MinecraftInstance::markCurrentConfigurationInitialized()
{
    const auto fingerprint = launchConfigurationFingerprint();
    if (fingerprint.isEmpty()) {
        qWarning() << "Not recording an initial online launch without a valid component configuration for" << name();
        return;
    }

    auto initialized = settings()->get("InitializedLaunchConfigurations").toStringList();
    if (!initialized.contains(fingerprint)) {
        initialized.append(fingerprint);
        settings()->set("InitializedLaunchConfigurations", initialized);
    }
}

shared_qobject_ptr<LaunchTask> MinecraftInstance::createLaunchTask(AuthSessionPtr session, MinecraftServerTargetPtr serverToJoin)
{
    updateRuntimeContext();

    auto process = LaunchTask::create(std::dynamic_pointer_cast<MinecraftInstance>(shared_from_this()));
    auto pptr = process.get();

    APPLICATION->icons()->saveIcon(iconKey(), FS::PathCombine(gameRoot(), "icon.png"), "PNG");

    {
        process->appendStep(makeShared<TextPrint>(pptr, "Minecraft folder is:\n" + gameRoot() + "\n\n", MessageLevel::Launcher));
    }

    {
        process->appendStep(makeShared<CheckJava>(pptr));
    }

    {
        process->appendStep(makeShared<CreateGameFolders>(pptr));
    }

    if (!serverToJoin && settings()->get("JoinServerOnLaunch").toBool()) {
        QString fullAddress = settings()->get("JoinServerOnLaunchAddress").toString();
        serverToJoin.reset(new MinecraftServerTarget(MinecraftServerTarget::parse(fullAddress)));
    }

    if (serverToJoin && serverToJoin->port == 25565) {

        auto step = makeShared<LookupServerAddress>(pptr);
        step->setLookupAddress(serverToJoin->address);
        step->setOutputAddressPtr(serverToJoin);
        process->appendStep(step);
    }

    if (getPreLaunchCommand().size()) {
        auto step = makeShared<PreLaunchCommand>(pptr);
        step->setWorkingDirectory(gameRoot());
        process->appendStep(step);
    }

    const bool onlineSession = session->status != AuthSession::PlayableOffline;
    const bool initializeConfiguration = !isCurrentConfigurationInitialized();
    if (onlineSession) {
        if (!session->demo) {
            process->appendStep(makeShared<ClaimAccount>(pptr, session));
        }
    }
    const auto updateMode = (onlineSession || initializeConfiguration) ? Net::Mode::Online : Net::Mode::Offline;
    process->appendStep(makeShared<Update>(pptr, updateMode));

    {
        process->appendStep(makeShared<ModMinecraftJar>(pptr));
    }

    {
        process->appendStep(makeShared<ScanModFolders>(pptr));
    }

    {
        process->appendStep(makeShared<PrintInstanceInfo>(pptr, session, serverToJoin));
    }

    {
        process->appendStep(makeShared<ExtractNatives>(pptr));
    }

    {
        process->appendStep(makeShared<ReconstructAssets>(pptr));
    }

    {
        process->appendStep(makeShared<VerifyJavaInstall>(pptr));
    }

    {

        auto step = makeShared<LauncherPartLaunch>(pptr, updateMode == Net::Mode::Online);
        step->setWorkingDirectory(gameRoot());
        step->setAuthSession(session);
        step->setServerToJoin(serverToJoin);
        process->appendStep(step);
    }

    if (getPostExitCommand().size()) {
        auto step = makeShared<PostLaunchCommand>(pptr);
        step->setWorkingDirectory(gameRoot());
        process->appendStep(step);
    }
    if (session) {
        process->setCensorFilter(createCensorFilterFromSession(session));
    }
    if (m_settings->get("QuitAfterGameStop").toBool()) {
        process->appendStep(makeShared<QuitAfterGameStop>(pptr));
    }
    m_launchProcess = process;
    emit launchTaskChanged(m_launchProcess);
    return m_launchProcess;
}

JavaVersion MinecraftInstance::getJavaVersion()
{
    return JavaVersion(settings()->get("JavaVersion").toString());
}

std::shared_ptr<ModFolderModel> MinecraftInstance::loaderModList()
{
    if (!m_loader_mod_list) {
        bool is_indexed = !APPLICATION->settings()->get("ModMetadataDisabled").toBool();
        m_loader_mod_list.reset(new ModFolderModel(modsRoot(), this, is_indexed));
    }
    return m_loader_mod_list;
}

std::shared_ptr<ModFolderModel> MinecraftInstance::coreModList()
{
    if (!m_core_mod_list) {
        bool is_indexed = !APPLICATION->settings()->get("ModMetadataDisabled").toBool();
        m_core_mod_list.reset(new ModFolderModel(coreModsDir(), this, is_indexed));
    }
    return m_core_mod_list;
}

std::shared_ptr<ModFolderModel> MinecraftInstance::nilModList()
{
    if (!m_nil_mod_list) {
        bool is_indexed = !APPLICATION->settings()->get("ModMetadataDisabled").toBool();
        m_nil_mod_list.reset(new ModFolderModel(nilModsDir(), this, is_indexed, false));
    }
    return m_nil_mod_list;
}

std::shared_ptr<ResourcePackFolderModel> MinecraftInstance::resourcePackList()
{
    if (!m_resource_pack_list) {
        m_resource_pack_list.reset(new ResourcePackFolderModel(resourcePacksDir(), this));
    }
    return m_resource_pack_list;
}

std::shared_ptr<TexturePackFolderModel> MinecraftInstance::texturePackList()
{
    if (!m_texture_pack_list) {
        m_texture_pack_list.reset(new TexturePackFolderModel(texturePacksDir(), this));
    }
    return m_texture_pack_list;
}

std::shared_ptr<ShaderPackFolderModel> MinecraftInstance::shaderPackList()
{
    if (!m_shader_pack_list) {
        m_shader_pack_list.reset(new ShaderPackFolderModel(shaderPacksDir(), this));
    }
    return m_shader_pack_list;
}

std::shared_ptr<WorldList> MinecraftInstance::worldList()
{
    if (!m_world_list) {
        m_world_list.reset(new WorldList(worldDir(), this));
    }
    return m_world_list;
}

std::shared_ptr<GameOptions> MinecraftInstance::gameOptionsModel()
{
    if (!m_game_options) {
        m_game_options.reset(new GameOptions(FS::PathCombine(gameRoot(), "options.txt")));
    }
    return m_game_options;
}

QList<Mod*> MinecraftInstance::getJarMods() const
{
    auto profile = m_components->getProfile();
    QList<Mod*> mods;
    for (auto jarmod : profile->getJarMods()) {
        QStringList jar, temp1, temp2, temp3;
        jarmod->getApplicableFiles(runtimeContext(), jar, temp1, temp2, temp3, jarmodsPath().absolutePath());

        mods.push_back(new Mod(QFileInfo(jar[0])));
    }
    return mods;
}

#include "MinecraftInstance.moc"
