#include "RegisterWindow.h"
#include <QCryptographicHash>


RegisterWindow::RegisterWindow(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle("用户注册");
    this->resize(350, 250);

    // 创建控件
    leUsername = new QLineEdit(this);
    lePassword = new QLineEdit(this);
    leConfirmPwd = new QLineEdit(this);
    lePassword->setEchoMode(QLineEdit::Password);
    leConfirmPwd->setEchoMode(QLineEdit::Password);

    QPushButton *btnRegister = new QPushButton("注册", this);
    QPushButton *btnCancel = new QPushButton("取消", this);

    // 布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("用户名：", leUsername);
    formLayout->addRow("密码：", lePassword);
    formLayout->addRow("确认密码：", leConfirmPwd);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnRegister);
    btnLayout->addSpacing(20);
    btnLayout->addWidget(btnCancel);
    btnLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(btnLayout);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // 信号槽绑定
    connect(btnRegister, &QPushButton::clicked, this, &RegisterWindow::on_btnRegister_clicked);
    connect(btnCancel, &QPushButton::clicked, this, &RegisterWindow::on_btnCancel_clicked);

    m_dataMgr = DataManager::getInstance();
}

RegisterWindow::~RegisterWindow()
{
    delete leUsername;
    delete lePassword;
    delete leConfirmPwd;
}

QString RegisterWindow::encryptPassword(const QString &password)
{
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(password.toUtf8());
    return hash.result().toHex();
}

void RegisterWindow::on_btnRegister_clicked()
{
    QString username = leUsername->text().trimmed();
    QString password = lePassword->text().trimmed();
    QString confirmPwd = leConfirmPwd->text().trimmed();

    if (username.isEmpty() || password.isEmpty() || confirmPwd.isEmpty()) {
        QMessageBox::warning(this, "警告", "所有字段不能为空！");
        return;
    }

    if (password != confirmPwd) {
        QMessageBox::warning(this, "警告", "两次输入的密码不一致！");
        lePassword->clear();
        leConfirmPwd->clear();
        return;
    }

    if (m_dataMgr->isUsernameExists(username)) {
        QMessageBox::warning(this, "警告", "用户名已存在！");
        leUsername->clear();
        return;
    }

    QString encryptedPwd = encryptPassword(password);
    if (m_dataMgr->addUser(username, encryptedPwd)) {
        QMessageBox::information(this, "成功", "注册成功！请返回登录！");
        this->accept();
    } else {
        QMessageBox::critical(this, "失败", "注册失败，请重试！");
    }
}

void RegisterWindow::on_btnCancel_clicked()
{
    this->reject();
}
