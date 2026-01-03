#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QSqlTableModel>
#include <QColor>

class TaskModel : public QSqlTableModel
{
    Q_OBJECT
public:
    // 仅声明构造函数（带默认参数，无花括号{}！）
    explicit TaskModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    enum Columns {
        Id = 0,
        Name,
        Category,
        Priority,
        Deadline,
        IsCompleted,
        UserId // 新增的用户ID列
    };

    // 新增：设置当前筛选的用户ID
    void setFilterUserId(int userId);

private:
    int m_filterUserId = -1; // 存储当前登录用户ID，默认-1（未登录
};

#endif // TASKMODEL_H
