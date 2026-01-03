#include "DataManager.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>

// 单例实例化
DataManager* DataManager::getInstance()
{
    static DataManager instance;
    return &instance;
}

// 构造函数：初始化数据库连接
DataManager::DataManager(QObject *parent) : QObject(parent)
{
    // 加载SQLite驱动
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("D:/forQtProject/identifier.sqlite");  // 数据库文件（本地存储）
}

DataManager::~DataManager()
{
    if (m_db.isOpen()) {
        m_db.close();  // 关闭数据库连接
    }
}

// 1. 初始化数据库：修改 task 表，新增 user_id 外键
bool DataManager::initDatabase()
{
    if (!m_db.open()) {
        qDebug() << "数据库打开失败：" << m_db.lastError().text();
        return false;
    }

    // 1.1 创建用户表（原有代码不变）
    QString createUserTableSql = R"(
        CREATE TABLE IF NOT EXISTS user (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        )
    )";

    // 1.2 修改任务表：新增 user_id 字段（外键关联 user.id）
    QString createTaskTableSql = R"(
        CREATE TABLE IF NOT EXISTS task (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            priority INTEGER NOT NULL CHECK(priority BETWEEN 1 AND 3),
            deadline TEXT NOT NULL,
            is_completed INTEGER NOT NULL DEFAULT 0,
            user_id INTEGER NOT NULL, -- 关联用户ID
            FOREIGN KEY(user_id) REFERENCES user(id) ON DELETE CASCADE -- 外键约束：用户删除时任务同步删除
        )
    )";

    QSqlQuery query;
    if (!query.exec(createUserTableSql)) {
        qDebug() << "创建用户表失败：" << query.lastError().text();
        return false;
    }
    if (!query.exec(createTaskTableSql)) {
        qDebug() << "创建任务表失败：" << query.lastError().text();
        return false;
    }

    return true;
}

QList<Task> DataManager::getTasks(int userId) {
    QList<Task> tasks;
    QSqlQuery query;
    // 增加 WHERE user_id = ? 条件，仅查询当前用户的任务
    query.prepare("SELECT id, name, category, priority, deadline, is_completed, user_id FROM task WHERE user_id = ?");
    query.addBindValue(userId); // 绑定当前登录用户ID
    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.id = query.value(0).toInt();
            task.name = query.value(1).toString();
            task.category = query.value(2).toString();
            task.priority = query.value(3).toInt();
            task.deadline = QDateTime::fromString(query.value(4).toString(), Qt::ISODate);
            task.isCompleted = query.value(5).toBool();
            task.userId = query.value(6).toInt();
            tasks.append(task);
        }
    } else {
        qDebug() << "查询任务失败：" << query.lastError().text();
    }
    return tasks;
}


// 注册用户：插入用户名和加密后的密码
bool DataManager::addUser(const QString &username, const QString &encryptedPwd)
{
    if (!m_db.isOpen() || username.isEmpty() || encryptedPwd.isEmpty()) {
        return false;
    }

    QString sql = "INSERT INTO user (username, password) VALUES (?, ?)";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(username);
    query.addBindValue(encryptedPwd);

    return query.exec();
}

// 验证登录：匹配用户名和加密密码
bool DataManager::verifyUser(const QString &username, const QString &encryptedPwd)
{
    if (!m_db.isOpen() || username.isEmpty() || encryptedPwd.isEmpty()) {
        return false;
    }

    QString sql = "SELECT id FROM user WHERE username = ? AND password = ? LIMIT 1";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(username);
    query.addBindValue(encryptedPwd);

    return query.exec() && query.next();
}

// 检查用户名是否已存在（核心修复函数）
bool DataManager::isUsernameExists(const QString &username)
{
    if (!m_db.isOpen() || username.trimmed().isEmpty()) {
        return false;
    }

    QString sql = "SELECT id FROM user WHERE username = ? LIMIT 1";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(username.trimmed());

    return query.exec() && query.next();
}

// 通过用户名获取用户ID
int DataManager::getUserIdByUsername(const QString &username)
{
    if (!m_db.isOpen() || username.isEmpty()) {
        return -1;
    }

    QString sql = "SELECT id FROM user WHERE username = ? LIMIT 1";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value("id").toInt();
    }
    return -1;
}

// 2. 实现用户ID管理接口
void DataManager::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
}

int DataManager::getCurrentUserId() const
{
    return m_currentUserId;
}


// 4. 扩展任务接口：所有操作均携带 user_id 筛选
bool DataManager::addTask(const Task &task, int userId)
{
    // 1. 校验：未登录/任务名为空，直接失败
    if (!m_db.isOpen() || m_currentUserId == -1 || task.name.isEmpty()) {
        qDebug() << "新增任务失败：未登录或任务名空";
        return false;
    }

    QString sql = R"(
        INSERT INTO task (name, category, priority, deadline, is_completed, user_id)
        VALUES (?, ?, ?, ?, ?, ?)
    )";

    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(task.name);
    query.addBindValue(task.category);
    query.addBindValue(task.priority);
    query.addBindValue(task.deadline.toString("yyyy-MM-dd HH:mm:ss"));
    query.addBindValue(task.isCompleted ? 1 : 0);
    // 强制绑定「当前登录用户的ID」（而不是task.userId，避免外部传入错误）
    query.addBindValue(m_currentUserId);

    if (!query.exec()) {
        qDebug() << "新增任务SQL失败：" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataManager::deleteTask(int taskId, int userId)
{
    if (!m_db.isOpen() || userId == -1) return false;

    // SQL 条件：仅删除当前用户的任务（task.id = ? AND user_id = ?）
    QString sql = "DELETE FROM task WHERE id = ? AND user_id = ?";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(taskId);
    query.addBindValue(userId);
    return query.exec();
}

bool DataManager::updateTask(const Task &task, int userId)
{
    if (!m_db.isOpen() || userId == -1) return false;

    QString sql = R"(
        UPDATE task SET name=?, category=?, priority=?, deadline=?, is_completed=?
        WHERE id=? AND user_id=?
    )";

    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(task.name);
    query.addBindValue(task.category);
    query.addBindValue(task.priority);
    query.addBindValue(task.deadline.toString("yyyy-MM-dd HH:mm:ss"));
    query.addBindValue(task.isCompleted ? 1 : 0);
    query.addBindValue(task.id);
    query.addBindValue(userId); // 仅更新当前用户的任务

    return query.exec();
}

QList<Task> DataManager::getAllTasks(int userId)
{
    QList<Task> tasks;
    // 1. 先校验：未登录/数据库未打开，直接返回空列表
    if (!m_db.isOpen() || m_currentUserId == -1) {
        qDebug() << "查询失败：未登录或数据库未打开";
        return tasks;
    }

    // 2. SQL强制过滤当前用户的user_id（关键！）
    QString sql = "SELECT * FROM task WHERE user_id = ? ORDER BY priority DESC, deadline ASC";
    QSqlQuery query;
    query.prepare(sql);
    // 绑定「当前登录用户的ID」（而不是任意值）
    query.addBindValue(m_currentUserId);

    if (!query.exec()) {
        qDebug() << "查询任务SQL失败：" << query.lastError().text();
        return tasks;
    }

    // 3. 只读取当前用户的任务
    while (query.next()) {
        Task task;
        task.id = query.value("id").toInt();
        task.name = query.value("name").toString();
        task.category = query.value("category").toString();
        task.priority = query.value("priority").toInt();
        task.deadline = QDateTime::fromString(query.value("deadline").toString(), "yyyy-MM-dd HH:mm:ss");
        task.isCompleted = query.value("is_completed").toInt() == 1;
        task.userId = m_currentUserId; // 强制绑定当前用户ID
        tasks.append(task);
    }

    return tasks;
}

// 按优先级筛选（仅当前用户）
QList<Task> DataManager::filterTasksByPriority(int priority, int userId)
{
    QList<Task> tasks;
    if (!m_db.isOpen() || userId == -1 || priority < 1 || priority > 3) return tasks;

    QString sql = "SELECT * FROM task WHERE priority = ? AND user_id = ? ORDER BY deadline ASC";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(priority);
    query.addBindValue(userId);
    query.exec();

    while (query.next()) {
        // 同 getAllTasks 赋值逻辑，省略重复代码
        Task task;
        task.id = query.value("id").toInt();
        task.name = query.value("name").toString();
        task.category = query.value("category").toString();
        task.priority = query.value("priority").toInt();
        task.deadline = QDateTime::fromString(query.value("deadline").toString(), "yyyy-MM-dd HH:mm:ss");
        task.isCompleted = query.value("is_completed").toInt() == 1;
        task.userId = userId;
        tasks.append(task);
    }
    return tasks;
}

// 按完成状态筛选（仅当前用户）
QList<Task> DataManager::filterTasksByCompletion(bool isCompleted, int userId)
{
    QList<Task> tasks;
    if (!m_db.isOpen() || userId == -1) return tasks;

    QString sql = "SELECT * FROM task WHERE is_completed = ? AND user_id = ? ORDER BY deadline ASC";
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(isCompleted ? 1 : 0);
    query.addBindValue(userId);
    query.exec();

    while (query.next()) {
        // 同 getAllTasks 赋值逻辑，省略重复代码
        Task task;
        task.id = query.value("id").toInt();
        task.name = query.value("name").toString();
        task.category = query.value("category").toString();
        task.priority = query.value("priority").toInt();
        task.deadline = QDateTime::fromString(query.value("deadline").toString(), "yyyy-MM-dd HH:mm:ss");
        task.isCompleted = query.value("is_completed").toInt() == 1;
        task.userId = userId;
        tasks.append(task);
    }
    return tasks;
}
