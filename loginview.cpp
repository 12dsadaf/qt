#include "loginview.h"
#include "ui_loginview.h"
#include "idatabase.h"
loginView::loginView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::loginView)
{
    ui->setupUi(this);
}

loginView::~loginView()
{
    delete ui;
}

void loginView::on_btSignIn_clicked()
{
    QString status=IDataBase::getInstance().userLogin(ui->loginUsername->text(),ui->loginUserPassword->text());

    if(status=="login ok")
      emit loginSuccess();
}

