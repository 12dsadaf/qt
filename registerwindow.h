#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "DataManager.h"

class RegisterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterWindow(QWidget *parent = nullptr);
    ~RegisterWindow() override;

private slots:
    void on_btnRegister_clicked();
    void on_btnCancel_clicked();

private:
    QLineEdit *leUsername;
    QLineEdit *lePassword;
    QLineEdit *leConfirmPwd;
    DataManager *m_dataMgr;

    QString encryptPassword(const QString &password);
};

#endif // REGISTERWINDOW_H
