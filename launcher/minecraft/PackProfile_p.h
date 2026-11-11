#pragma once

#include <QList>
#include <QMap>
#include <QTimer>
#include <map>
#include "Component.h"

class MinecraftInstance;
using ComponentContainer = QList<ComponentPtr>;
using ComponentIndex = QMap<QString, ComponentPtr>;

struct PackProfileData {

    MinecraftInstance* m_instance;

    std::shared_ptr<LaunchProfile> m_profile;

    ComponentContainer components;
    ComponentIndex componentIndex;
    bool dirty = false;
    bool downloadResources = true;
    QTimer m_saveTimer;
    Task::Ptr m_updateTask;
    bool loaded = false;
    bool interactionDisabled = true;
};
