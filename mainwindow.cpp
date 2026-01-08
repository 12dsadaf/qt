#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    
{
    ui->setupUi(this);
    // 1. 初始化数据库
    m_dataMgr = DataManager::getInstance();
    m_dataMgr->initDatabase();

    // 2. 初始化自定义Model（关键修改：设置用户ID筛选）
    m_taskModel = new TaskModel(this, m_dataMgr->getInstance()->m_db);
    int currentUserId = m_dataMgr->getCurrentUserId(); // 获取登录后的用户ID
    m_taskModel->setFilterUserId(currentUserId); // 强制筛选当前用户的任务

    // 3. 初始化筛选代理模型
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_taskModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    ui->tableView->setModel(m_proxyModel);


    // 4. TableView样式设置
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnHidden(TaskModel::Id, true); // 隐藏ID列

    // 5. 初始化其他模块
    m_reminderMgr = new ReminderManager(this);
    m_exportTool = new ExportTool(this);

    // 6. 绑定提醒信号
    connect(m_reminderMgr, &ReminderManager::reminderTriggered,
            this, &MainWindow::onReminderTriggered);


}

MainWindow::~MainWindow()
{
    delete ui;
}

// 添加任务
void MainWindow::on_btnAddTask_clicked()
{
    showTaskDialog();
}

// 删除任务
void MainWindow::on_btnDeleteTask_clicked()
{
    QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::warning(this, "提示", "请选择要删除的任务！");
        return;
    }

    QModelIndex proxyIndex = selectedIndexes.first();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    int taskId = m_taskModel->data(m_taskModel->index(sourceIndex.row(), TaskModel::Id)).toInt();
    int currentUserId = m_dataMgr->getCurrentUserId(); // 获取当前用户ID

    if (QMessageBox::question(this, "确认", "是否删除该任务？") == QMessageBox::Yes) {
        // 调用带userId的deleteTask（修复第73行错误）
        if (m_dataMgr->deleteTask(taskId, currentUserId)) {
            m_taskModel->select();
            QMessageBox::information(this, "成功", "任务删除成功！");
        } else {
            QMessageBox::critical(this, "失败", "任务删除失败！");
        }
    }
}

// 按优先级筛选（核心修复：兼容Qt 5/6）
void MainWindow::on_btnFilterPriority_clicked()
{
    // 1. 获取选中的优先级（1=低，2=中，3=高）
    int priority = ui->cbxPriority->currentIndex() + 1;
    // 2. 重置之前的筛选
    SET_FILTER_REGEXP(m_proxyModel, "");
    // 3. 设置筛选列：优先级列
    m_proxyModel->setFilterKeyColumn(TaskModel::Priority);
    // 4. 设置筛选条件（匹配数字）
    SET_FILTER_REGEXP(m_proxyModel, QString::number(priority));
}

// 按完成状态筛选（核心修复：兼容Qt 5/6）
void MainWindow::on_btnFilterCompleted_clicked()
{
    // （保留步骤1的代码）
    bool isCompleted = ui->cbxCompleted->isChecked();
    SET_FILTER_REGEXP(m_proxyModel, "");
    m_proxyModel->setFilterKeyColumn(TaskModel::IsCompleted);
    if (isCompleted) {
        SET_FILTER_REGEXP(m_proxyModel, "已完成");
    } else {
        SET_FILTER_REGEXP(m_proxyModel, "未完成");
    }

    // 新增：筛选后检查结果数量，为空则提示
    int filteredCount = m_proxyModel->rowCount();
    if (filteredCount == 0) {
        QString tip = isCompleted ? "暂无已完成的任务" : "暂无未完成的任务";
        QMessageBox::information(this, "筛选结果", tip);
    }
}

// 导出CSV
void MainWindow::on_btnExport_clicked()
{
    int currentUserId = m_dataMgr->getCurrentUserId(); // 获取当前用户ID
    QList<Task> userTasks = m_dataMgr->getAllTasks(currentUserId); // 修复第141行错误

    QString filter;
    QStringList formats;
    formats << "Excel文件 (*.xls)" << "PDF文件 (*.pdf)";

    QString filePath = QFileDialog::getSaveFileName(
        this, "选择导出格式", QDir::homePath() + "/任务列表.xls", formats.join(";;"), &filter
        );

    if (filePath.isEmpty()) return;

    // 自动补充后缀
    if (filter.contains("Excel文件") && !filePath.endsWith(".xls")) {
        filePath += ".xls";
    } else if (filter.contains("PDF文件") && !filePath.endsWith(".pdf")) {
        filePath += ".pdf";
    }

    bool exportSuccess = false;
    if (filter.contains("Excel文件")) {
        exportSuccess = m_exportTool->exportToExcel(userTasks, filePath);
    } else if (filter.contains("PDF文件")) {
        exportSuccess = m_exportTool->exportToPdf(userTasks, filePath);
    }

    if (exportSuccess) {
        QMessageBox::information(this, "导出成功", QString("文件已成功导出到：\n%1").arg(filePath));
    } else {
        QMessageBox::critical(this, "导出失败", "文件导出失败！");
    }
}

// 刷新任务列表（重置筛选+重新加载）
void MainWindow::on_btnRefresh_clicked()
{
    int currentUserId = m_dataMgr->getCurrentUserId();
    m_taskModel->setFilterUserId(currentUserId); // 重新应用用户筛选
    SET_FILTER_REGEXP(m_proxyModel, ""); // 重置优先级/完成状态筛选
}

// 接收提醒信号
void MainWindow::onReminderTriggered(const QString &taskName, const QDateTime &deadline)
{
    QString msg = QString("提醒：任务「%1」即将截止！\n截止时间：%2")
                      .arg(taskName)
                      .arg(deadline.toString("yyyy-MM-dd HH:mm"));
    QMessageBox::information(this, "任务提醒", msg);
}

// 任务添加/编辑对话框
void MainWindow::showTaskDialog(const Task &task)
{
    QDialog dialog(this);
    dialog.setWindowTitle(task.id == 0 ? "添加任务" : "编辑任务");
    dialog.resize(400, 300);

    // 创建控件
    QLineEdit* leName = new QLineEdit(task.name);
    QComboBox* cbxCategory = new QComboBox();
    cbxCategory->addItems({"工作", "学习", "生活"});
    cbxCategory->setCurrentText(task.category);

    QComboBox* cbxPriority = new QComboBox();
    cbxPriority->addItems({"低", "中", "高"});
    cbxPriority->setCurrentIndex(task.priority - 1);

    QDateTimeEdit* dteDeadline = new QDateTimeEdit(
        task.deadline.isValid() ? task.deadline : QDateTime::currentDateTime().addDays(1)
        );
    dteDeadline->setCalendarPopup(true);

    QPushButton* btnOk = new QPushButton("确认");
    QPushButton* btnCancel = new QPushButton("取消");

    // 布局
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("任务名称：", leName);
    formLayout->addRow("分类：", cbxCategory);
    formLayout->addRow("优先级：", cbxPriority);
    formLayout->addRow("截止时间：", dteDeadline);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(&dialog);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);

    // 绑定按钮信号
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Task newTask = task;
        newTask.name = leName->text().trimmed();
        newTask.category = cbxCategory->currentText();
        newTask.priority = cbxPriority->currentIndex() + 1;
        newTask.deadline = dteDeadline->dateTime();
        newTask.userId = m_dataMgr->getCurrentUserId(); // 绑定当前用户ID

        if (newTask.name.isEmpty()) {
            QMessageBox::warning(this, "提示", "任务名称不能为空！");
            return;
        }

        bool success = false;
        int currentUserId = m_dataMgr->getCurrentUserId(); // 获取当前用户ID
        if (newTask.id == 0) {
            success = m_dataMgr->addTask(newTask, currentUserId); // 修复第234行错误
        } else {
            success = m_dataMgr->updateTask(newTask, currentUserId); // 修复第236行错误
        }

        if (success) {
            m_taskModel->select();
            QMessageBox::information(this, "成功", task.id == 0 ? "任务添加成功！" : "任务编辑成功！");
        } else {
            QMessageBox::critical(this, "失败", task.id == 0 ? "任务添加失败！" : "任务编辑失败！");
        }
    }
}
