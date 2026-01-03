#include "ReminderManager.h"
#include <QDateTime>
#include <QDebug>

ReminderManager::ReminderManager(QObject *parent) : QObject(parent)
{
    m_dataMgr = DataManager::getInstance();
    // 定时器配置：每60秒（1分钟）检查一次截止任务
    m_timer.setInterval(60 * 1000);
    connect(&m_timer, &QTimer::timeout, this, &ReminderManager::checkDeadlines);
    m_timer.start();  // 启动定时器
}

// 检查所有未完成任务的截止时间，临近1小时内触发提醒
void ReminderManager::checkDeadlines()
{
    int currentUserId = m_dataMgr->getCurrentUserId(); // 获取当前用户ID
    if (currentUserId == -1) return; // 未登录，不检查

    // 仅查询当前用户的未完成任务
    QList<Task> tasks = m_dataMgr->filterTasksByCompletion(false, currentUserId);
    QDateTime now = QDateTime::currentDateTime();

    foreach (const Task& task, tasks) {
        qint64 secondsLeft = now.secsTo(task.deadline);
        if (secondsLeft > 0 && secondsLeft <= 3600) {
            emit reminderTriggered(task.name, task.deadline); // 仅提醒当前用户的任务
        }
    }
}
