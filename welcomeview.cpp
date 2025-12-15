#include "welcomeview.h"
#include "ui_welcomeview.h"
#include "qdebug.h"

WelcomeView::WelcomeView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WelcomeView)
{
    qDebug()<<"create WelcomeView";
    ui->setupUi(this);
}

WelcomeView::~WelcomeView()
{
     qDebug()<<"destory WelcomeView";
    delete ui;
}

void WelcomeView::on_departManager_clicked()
{
    emit goDepartmentView();
}


void WelcomeView::on_doctorManager_clicked()
{
    emit goDoctorView();
}


void WelcomeView::on_illManager_clicked()
{
    emit goPatientView();
}

