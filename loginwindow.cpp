#include "LoginWindow.h"
#include "RegisterWindow.h"
#include <QCryptographicHash>

// 构造函数：完全用代码创建控件，无 ui->setupUi(this)
LoginWindow::LoginWindow(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle("用户登录");
    this->resize(350, 200); // 固定窗口大小

    // 1. 创建控件（直接 new，父对象设为 this，自动管理生命周期）
    leUsername = new QLineEdit(this);
    lePassword = new QLineEdit(this);
    lePassword->setEchoMode(QLineEdit::Password); // 密码隐藏

    QPushButton *btnLogin = new QPushButton("登录", this);
    QPushButton *btnRegister = new QPushButton("注册", this);
    QPushButton *btnCancel = new QPushButton("取消", this);

    // 2. 布局设计（直接绑定到当前窗口，无需 ui 容器）
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("用户名：", leUsername);
    formLayout->addRow("密码：", lePassword);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch(); // 自适应间距
    btnLayout->addWidget(btnLogin);
    btnLayout->addSpacing(20);
    btnLayout->addWidget(btnRegister);
    btnLayout->addSpacing(20);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this); // 布局绑定当前窗口
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(btnLayout);
    mainLayout->setContentsMargins(30, 30, 30, 30); // 内边距

    // 3. 绑定信号槽
    connect(btnLogin, &QPushButton::clicked, this, &LoginWindow::on_btnLogin_clicked);
    connect(btnRegister, &QPushButton::clicked, this, &LoginWindow::on_btnRegister_clicked);
    connect(btnCancel, &QPushButton::clicked, this, &LoginWindow::on_btnCancel_clicked);

    // 4. 初始化数据管理
    m_dataMgr = DataManager::getInstance();
    m_dataMgr->initDatabase();
}

// 析构函数：无 ui 指针，无需 delete ui
LoginWindow::~LoginWindow()
{
    // 可选：Qt 父子关系会自动释放子控件，此处显式释放更清晰
    delete leUsername;
    delete lePassword;
}

// 密码加密（不变）
QString LoginWindow::encryptPassword(const QString &password)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    return hash.result().toHex();
}

// 登录按钮逻辑（不变）
void LoginWindow::on_btnLogin_clicked()
{
    QString username = leUsername->text().trimmed();
    QString password = lePassword->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名和密码不能为空！");
        return;
    }

    // 密码加密
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    QString encryptedPwd = hash.result().toHex();

    // 验证登录 + 设置当前用户ID（核心）
    if (m_dataMgr->verifyUser(username, encryptedPwd)) {
        // 获取当前用户的ID
        int userId = m_dataMgr->getUserIdByUsername(username);
        if (userId == -1) {
            QMessageBox::critical(this, "错误", "无法获取用户信息！");
            return;
        }
        // 强制设置当前用户ID（后续所有任务操作都依赖这个值）
        m_dataMgr->setCurrentUserId(userId);

        QMessageBox::information(this, "成功", "登录成功！");
        this->accept(); // 进入主窗口
    } else {
        QMessageBox::critical(this, "失败", "用户名或密码错误！");
        lePassword->clear();
    }
}

// 注册按钮逻辑（不变）
void LoginWindow::on_btnRegister_clicked()
{
    RegisterWindow regWnd(this);
    regWnd.exec(); // 模态显示注册窗口
}

// 取消按钮逻辑（不变）
void LoginWindow::on_btnCancel_clicked()
{
    this->reject(); // 退出程序
}
