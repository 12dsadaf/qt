#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "DataManager.h"

// 完全移除 Ui 命名空间和 ui 指针
class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override; // 显式 override 确保继承正确

private slots:
    void on_btnLogin_clicked();
    void on_btnRegister_clicked();
    void on_btnCancel_clicked();

private:
    // 仅保留控件和数据管理指针，无 ui
    QLineEdit *leUsername;
    QLineEdit *lePassword;
    DataManager *m_dataMgr;

    QString encryptPassword(const QString &password);
};

#endif // LOGINWINDOW_H
