#include "MinecraftLoad.h"
#include "MinecraftInstance.h"
#include "PackProfile.h"

MinecraftLoad::MinecraftLoad(MinecraftInstance* inst, QObject* parent) : Task(parent), m_inst(inst) {}

void MinecraftLoad::executeTask()
{

    auto components = m_inst->getPackProfile();
    m_task = components->loadOffline();
    if (!m_task) {
        emitFailed(tr("Could not load the local Minecraft component configuration."));
        return;
    }
    connect(m_task.get(), &Task::succeeded, this, &MinecraftLoad::subtaskSucceeded);
    connect(m_task.get(), &Task::failed, this, &MinecraftLoad::subtaskFailed);
    connect(m_task.get(), &Task::aborted, this, [this] { subtaskFailed(tr("Aborted")); });
    connect(m_task.get(), &Task::progress, this, &MinecraftLoad::progress);
    connect(m_task.get(), &Task::stepProgress, this, &MinecraftLoad::propagateStepProgress);
    connect(m_task.get(), &Task::status, this, &MinecraftLoad::setStatus);
    m_task->start();
}

void MinecraftLoad::subtaskSucceeded()
{
    if (isFinished()) {
        qCritical() << "MinecraftLoad: Subtask" << sender() << "succeeded, but work was already done!";
        return;
    }
    emitSucceeded();
}

void MinecraftLoad::subtaskFailed(QString error)
{
    if (isFinished()) {
        qCritical() << "MinecraftLoad: Subtask" << sender() << "failed, but work was already done!";
        return;
    }
    emitFailed(error);
}
