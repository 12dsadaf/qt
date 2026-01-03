#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// 核心：添加代理模型头文件（Qt 5/6通用）
#include <QSortFilterProxyModel>
// 新增：Qt版本兼容宏（自动适配Qt 5/6）
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#define SET_FILTER_REGEXP(proxy, regex) proxy->setFilterRegularExpression(regex)
#else
#define SET_FILTER_REGEXP(proxy, regex) proxy->setFilterRegExp(regex)
#endif

#include "DataManager.h"
#include "TaskModel.h"
#include "ReminderManager.h"
#include "ExportTool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 原有槽函数（名称不变）
    void on_btnAddTask_clicked();
    void on_btnDeleteTask_clicked();
    void on_btnFilterPriority_clicked();
    void on_btnFilterCompleted_clicked();
    void on_btnExport_clicked();
    void on_btnRefresh_clicked();
    void onReminderTriggered(const QString& taskName, const QDateTime& deadline);

private:
    Ui::MainWindow *ui;
    DataManager* m_dataMgr;
    TaskModel* m_taskModel;
    // 筛选代理模型（核心修复）
    QSortFilterProxyModel* m_proxyModel;
    ReminderManager* m_reminderMgr;
    ExportTool* m_exportTool;

    // 弹出添加/编辑任务对话框
    void showTaskDialog(const Task& task = Task());
};

#endif // MAINWINDOW_H
