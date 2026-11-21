/* Copyright 2013-2021 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ReconstructAssets.h"
#include "launch/LaunchTask.h"
#include "minecraft/AssetsUtils.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

bool ReconstructAssets::reconstruct(QString& error) const
{
    const auto instance = std::dynamic_pointer_cast<MinecraftInstance>(m_parent->instance());
    if (!instance) {
        error = tr("Could not access the Minecraft instance while preparing its assets.");
        return false;
    }

    const auto components = instance->getPackProfile();
    if (!components->shouldDownloadResources())
        return true;

    const auto profile = components->getProfile();
    const auto assets = profile->getMinecraftAssets();
    return AssetsUtils::reconstructAssets(assets->id, instance->resourcesDir(), &error);
}

void ReconstructAssets::executeTask()
{
    QString error;
    if (reconstruct(error)) {
        emitSucceeded();
        return;
    }

    m_reconstructionError = error;
    startRecovery();
}

void ReconstructAssets::startRecovery()
{
    if (m_recoveryAttempted || m_aborted) {
        const QString reason = tr("Minecraft asset reconstruction failed: %1").arg(m_reconstructionError);
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
        return;
    }

    m_recoveryAttempted = true;
    const auto instance = std::dynamic_pointer_cast<MinecraftInstance>(m_parent->instance());
    if (!instance) {
        const QString reason = tr("Could not start Minecraft asset recovery because the instance is unavailable.");
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
        return;
    }

    m_updateTask = instance->createUpdateTask(Net::Mode::Online);
    if (!m_updateTask) {
        const QString reason = tr("Could not create the online Minecraft asset recovery task.");
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
        return;
    }

    connect(m_updateTask.get(), &Task::finished, this, &ReconstructAssets::updateFinished);
    connect(m_updateTask.get(), &Task::progress, this, &ReconstructAssets::setProgress);
    connect(m_updateTask.get(), &Task::stepProgress, this, &ReconstructAssets::propagateStepProgress);
    connect(m_updateTask.get(), &Task::status, this, &ReconstructAssets::setStatus);
    connect(m_updateTask.get(), &Task::details, this, &ReconstructAssets::setDetails);

    emit logLine(tr("Minecraft assets could not be reconstructed. Attempting an online asset repair."),
                 MessageLevel::Launcher);
    emit progressReportingRequest();
}

void ReconstructAssets::proceed()
{
    if (m_aborted || !m_updateTask || m_updateTask->isRunning() || m_updateTask->isFinished())
        return;
    m_updateTask->start();
}

void ReconstructAssets::updateFinished()
{
    const auto updateTask = m_updateTask;
    m_updateTask.reset();

    if (!updateTask || !isRunning())
        return;

    if (m_aborted) {
        QString reason = updateTask->failReason();
        if (reason.isEmpty())
            reason = tr("Aborted.");
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
        return;
    }

    if (!updateTask->wasSuccessful()) {
        QString reason = updateTask->failReason();
        if (reason.isEmpty())
            reason = tr("The online Minecraft asset recovery task failed without an error message.");
        emit logLine(reason, MessageLevel::Fatal);
        emitFailed(reason);
        return;
    }

    QString error;
    if (reconstruct(error)) {
        emit logLine(tr("Minecraft assets were repaired successfully."), MessageLevel::Launcher);
        emitSucceeded();
        return;
    }

    const QString reason = tr("Minecraft asset reconstruction still fails after the online repair: %1").arg(error);
    emit logLine(reason, MessageLevel::Fatal);
    emitFailed(reason);
}

bool ReconstructAssets::canAbort() const
{
    return !m_updateTask || m_updateTask->canAbort();
}

bool ReconstructAssets::abort()
{
    if (m_aborted)
        return true;

    m_aborted = true;
    const auto updateTask = m_updateTask;
    if (!updateTask || !updateTask->isRunning()) {
        if (isRunning())
            emitFailed(tr("Aborted."));
        return true;
    }

    if (!updateTask->canAbort()) {
        m_aborted = false;
        return false;
    }
    if (!updateTask->abort()) {
        m_aborted = false;
        return false;
    }
    return true;
}
