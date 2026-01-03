#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>


// 任务结构体（存储任务核心信息）
struct Task {
    int id;             // 唯一ID（自增）
    QString name;       // 任务名称
    QString category;   // 分类（如：工作/学习/生活）
    int priority;       // 优先级（1-3：低/中/高）
    QDateTime deadline; // 截止时间
    bool isCompleted;   // 是否完成
    int userId;         // 关联的用户ID（新增）
};

class DataManager : public QObject
{
    Q_OBJECT
public:
    static DataManager* getInstance();  // 单例模式（全局唯一）
    bool initDatabase();                // 初始化数据库（创建表）
    bool addTask(const Task& task);     // 添加任务
    bool deleteTask(int taskId);        // 删除任务
    bool updateTask(const Task& task);  // 更新任务（如修改状态/名称）
    QList<Task> getTasks(int userId);         // 获取所有任务
    QList<Task> filterTasksByPriority(int priority); // 按优先级筛选
    QList<Task> filterTasksByCompletion(bool isCompleted); // 按完成状态筛选
    QSqlDatabase m_db;  // SQLite数据库对象
    // 用户相关接口
    bool addUser(const QString &username, const QString &encryptedPwd); // 注册用户
    bool verifyUser(const QString &username, const QString &encryptedPwd); // 验证登录
    bool isUsernameExists(const QString &username); // 检查用户名是否已存在
    // 新增：设置/获取当前登录用户ID（全局唯一）
    void setCurrentUserId(int userId);
    int getCurrentUserId() const;

    // 任务接口扩展：所有接口新增 userId 参数
    bool addTask(const Task& task, int userId);     // 绑定用户ID
    bool deleteTask(int taskId, int userId);        // 仅删除当前用户的任务
    bool updateTask(const Task& task, int userId);  // 仅更新当前用户的任务
    QList<Task> getAllTasks(int userId);            // 仅查询当前用户的任务
    QList<Task> filterTasksByPriority(int priority, int userId); // 带用户筛选
    QList<Task> filterTasksByCompletion(bool isCompleted, int userId); // 带用户筛选

    // 新增：通过用户名获取用户ID（登录成功后调用）
    int getUserIdByUsername(const QString& username);

private:
    DataManager(QObject *parent = nullptr);  // 私有构造（单例）
    int m_currentUserId = -1; // 当前登录用户ID（默认-1：未登录）
    ~DataManager();

};

#endif // DATAMANAGER_H
