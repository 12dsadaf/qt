#ifndef REMINDERMANAGER_H
#define REMINDERMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include "DataManager.h"

class ReminderManager : public QObject
{
    Q_OBJECT
public:
    explicit ReminderManager(QObject *parent = nullptr);

signals:
    // 触发提醒时发送信号（主窗口接收后弹出提示框）
    void reminderTriggered(const QString& taskName, const QDateTime& deadline);

private slots:
    void checkDeadlines();  // 定时检查任务截止时间

private:
    QTimer m_timer;         // 定时器（每1分钟检查一次）
    DataManager* m_dataMgr; // 数据管理实例
};

#endif // REMINDERMANAGER_H
