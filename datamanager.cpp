#include "DataManager.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>


// ??????
DataManager* DataManager::getInstance()
{
    static DataManager instance;
    return &instance;
}

// ?????????????

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    // ??SQLite??
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("D:/forQtProject/identifier.sqlite");  // ???????????
}

DataManager::~DataManager()



{
    if (m_db.isOpen()) {
        m_db.close();  // ???????
    }
}

// 1. ????????? task ???? user_id ??
bool DataManager::initDatabase()
{
    if (!m_db.open()) {
        qDebug() << "????????" << m_db.lastError().text();
        return false;
    }

    // 1.1 ?????????????
    QString createUserTableSql = R"(
        CREATE TABLE IF NOT EXISTS user (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            password TEXT NOT NULL
        )
    )";

    // 1.2 ???????? user_id ??????? user.id?
    QString createTaskTableSql = R"(
        CREATE TABLE IF NOT EXISTS task (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            priority INTEGER NOT NULL CHECK(priority BETWEEN 1 AND 3),
            deadline TEXT NOT NULL,
            is_completed INTEGER NOT NULL DEFAULT 0,
            user_id INTEGER NOT NULL, -- ????ID
            FOREIGN KEY(user_id) REFERENCES user(id) ON DELETE CASCADE -- ????????????????
        )
    )";

    QSqlQuery query;
    if (!query.exec(createUserTableSql)) {
        qDebug() << "????????" << query.lastError().text();
        return false;
    }
    if (!query.exec(createTaskTableSql)) {
        qDebug() << "????????" << query.lastError().text();
        return false;
    }

    return true;
}

QList<Task> DataManager::getTasks(int userId) {
    QList<Task> tasks;
    QSqlQuery query;
    // ?? WHERE user_id = ? ?????????????
    query.prepare("SELECT id, name, category, priority, deadline, is_completed, user_id FROM task WHERE user_id = ?");
    query.addBindValue(userId); // ????????ID
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
        qDebug() << "???????" << query.lastError().text();
    }
    return tasks;
}


// ?????????????????
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

// ???????????????
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

// ??????????????????
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

// ?????????ID
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

// 2. ????ID????
void DataManager::setCurrentUserId(int userId)
{
    m_currentUserId = userId;
}

int DataManager::getCurrentUserId() const
{
    return m_currentUserId;
}


// 4. ?????????????? user_id ??
bool DataManager::addTask(const Task &task, int userId)
{
    // 1. ??????/??????????
    if (!m_db.isOpen() || m_currentUserId == -1 || task.name.isEmpty()) {
        qDebug() << "???????????????";
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
    // ????????????ID?????task.userId??????????
    query.addBindValue(m_currentUserId);

    if (!query.exec()) {
        qDebug() << "????SQL???" << query.lastError().text();
        return false;
    }
    return true;
}

bool DataManager::deleteTask(int taskId, int userId)
{
    if (!m_db.isOpen() || userId == -1) return false;

    // SQL ??????????????task.id = ? AND user_id = ??
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
    query.addBindValue(userId); // ??????????

    return query.exec();
}

QList<Task> DataManager::getAllTasks(int userId)
{
    QList<Task> tasks;
    // 1. ???????/??????????????
    if (!m_db.isOpen() || m_currentUserId == -1) {
        qDebug() << "???????????????";
        return tasks;
    }

    // 2. SQL?????????user_id?????
    QString sql = "SELECT * FROM task WHERE user_id = ? ORDER BY priority DESC, deadline ASC";
    QSqlQuery query;
    query.prepare(sql);
    // ??????????ID?????????
    query.addBindValue(m_currentUserId);

    if (!query.exec()) {
        qDebug() << "????SQL???" << query.lastError().text();
        return tasks;
    }

    // 3. ??????????
    while (query.next()) {
        Task task;
        task.id = query.value("id").toInt();
        task.name = query.value("name").toString();
        task.category = query.value("category").toString();
        task.priority = query.value("priority").toInt();
        task.deadline = QDateTime::fromString(query.value("deadline").toString(), "yyyy-MM-dd HH:mm:ss");
        task.isCompleted = query.value("is_completed").toInt() == 1;
        task.userId = m_currentUserId; // ????????ID
        tasks.append(task);
    }

    return tasks;
}

// ?????????????
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
        // ? getAllTasks ???????????
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

// ??????????????
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
        // ? getAllTasks ???????????
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
