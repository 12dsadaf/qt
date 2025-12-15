#include "masterview.h"
#include "ui_masterview.h"
#include "QDebug"
#include "idatabase.h"
MasterView::MasterView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MasterView)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
    goLoginView();

    IDataBase::getInstance();
}

MasterView::~MasterView()
{
    delete ui;
}

void MasterView::goLoginView()
{
    qDebug()<<"goLoginView";
    loginview =new loginView();
    pushWidgetToStackView(loginview);

    connect(loginview,SIGNAL(loginSuccess()),this,SLOT(goWelcome()));
}

void MasterView::goWelcome()
{
    qDebug()<<"goWelcomeView";
    welcomeView =new WelcomeView();
    pushWidgetToStackView(welcomeView);

     connect(welcomeView,SIGNAL(goDoctorView()),this,SLOT(goDoctorView()));
     connect(welcomeView,SIGNAL(goPatientView()),this,SLOT(goPatientView()));
     connect(welcomeView,SIGNAL(goDepartmentView()),this,SLOT(goDepartmentView()));
}


void MasterView::goDoctorView()
{
    qDebug()<<"goDcotorView";
    doctorView =new DoctorView();
    pushWidgetToStackView(doctorView);
}

void MasterView::goDepartmentView()
{
    qDebug()<<"goDepartmentView";
    departmentView =new DepartmentView();
    pushWidgetToStackView(departmentView);
}

void MasterView::goPatientEditView(int num)
{
    qDebug()<<"goPatientEditView";
    patientEditView =new PatientEditView(this,num);
    pushWidgetToStackView(patientEditView);
    connect(patientEditView,SIGNAL(goPreviousView()),this,SLOT(goPreviousView()));
}


void MasterView::goPatientView()
{
    qDebug()<<"goPatientView";
    patientView =new PatientView(this);
    pushWidgetToStackView(patientView);

    connect(patientView,SIGNAL(goPatientEditView(int)),this,SLOT(goPatientEditView(int)));
}

void MasterView::goPreviousView()
{
   int count =ui->stackedWidget->count();
    if(count>1){
       ui->stackedWidget->setCurrentIndex(count-2);
        ui->lableTitle->setText(ui->stackedWidget->currentWidget()->windowTitle());

       QWidget *widget=ui->stackedWidget->widget(count-1);
        ui->stackedWidget->removeWidget(widget);
       delete widget;
    }
}

void MasterView::pushWidgetToStackView(QWidget *widget)
{
    ui->stackedWidget->addWidget(widget);
    int count =ui->stackedWidget->count();
    ui->stackedWidget->setCurrentIndex(count-1);
    ui->lableTitle->setText(widget->windowTitle());
}



void MasterView::on_btBack_clicked()
{
    goPreviousView();
}


void MasterView::on_stackedWidget_currentChanged(int arg1)
{
    int count =ui->stackedWidget->count();
    if(count>1){
        ui->btBack->setEnabled(true);
    }
    else
        ui->btBack->setEnabled(false);

    QString title=ui->stackedWidget->currentWidget()->windowTitle();

    if(title=="欢迎"){
        ui->btlogout->setEnabled(true);
        ui->btBack->setEnabled(false);


    }
    else
        ui->btlogout->setEnabled(false);

}


void MasterView::on_btlogout_clicked()
{
    goPreviousView();
    ui->btBack->setEnabled(false);
}

