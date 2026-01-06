#include "TaskModel.h"

TaskModel::TaskModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db)
{
    setTable("task");
    setEditStrategy(QSqlTableModel::OnFieldChange);
    setHeaderData(Id, Qt::Horizontal, "ID");
    setHeaderData(Name, Qt::Horizontal, "任务名称");
    setHeaderData(Category, Qt::Horizontal, "分类");
    setHeaderData(Priority, Qt::Horizontal, "优先级");
    setHeaderData(Deadline, Qt::Horizontal, "截止时间");
    setHeaderData(IsCompleted, Qt::Horizontal, "完成状态");
}

// 自定义单元格显示：优化UI体验（5天内可选择性实现，核心功能不影响）

// 新增：设置用户ID筛选，强制查询当前用户的任务
void TaskModel::setFilterUserId(int userId)
{
    m_filterUserId = userId;
    if (userId != -1) {
        // 关键：添加 SQL 筛选条件，仅查询当前用户的任务
        setFilter(QString("user_id = %1").arg(userId));
    } else {
        setFilter(""); // 未登录时清空筛选（实际登录前不会加载该模型）
    }
    select(); // 重新加载数据（应用筛选条件）
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();

    // 1. 完成状态：显示“已完成”/“未完成”（替代数据库的0/1）
    if (index.column() == IsCompleted && role == Qt::DisplayRole) {
        int isCompleted = QSqlTableModel::data(index, Qt::DisplayRole).toInt();
        return isCompleted ? "已完成" : "未完成";
    }

    // 2. 优先级标色：高优先级（3）红色，中（2）黄色，低（1）绿色
    if (index.column() == Priority && role == Qt::ForegroundRole) {
        int priority = QSqlTableModel::data(index, Qt::DisplayRole).toInt();
        switch (priority) {
        case 3: return QColor(Qt::red);    // 高优先级
        case 2: return QColor(Qt::darkYellow); // 中优先级
        case 1: return QColor(Qt::green);  // 低优先级
        default: return QColor(Qt::black);
        }
    }

    // 其他情况按默认逻辑处理
    return QSqlTableModel::data(index, role);
}
