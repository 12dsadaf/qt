#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>

namespace Ui {
class loginView;
}

class loginView : public QWidget
{
    Q_OBJECT

public:
    explicit loginView(QWidget *parent = nullptr);
    ~loginView();

private slots:
    void on_btSignIn_clicked();

signals:
    void loginSuccess();
    void loginFailed();

private:
    Ui::loginView *ui;
};

#endif // LOGINVIEW_H
