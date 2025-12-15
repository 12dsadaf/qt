#ifndef MASTERVIEW_H
#define MASTERVIEW_H

#include "departmentview.h"
#include "doctorview.h"
#include "loginview.h"
#include "patientview.h"
#include "welcomeview.h"
#include "patienteditview.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class MasterView;
}
QT_END_NAMESPACE

class MasterView : public QWidget
{
    Q_OBJECT

public:
    MasterView(QWidget *parent = nullptr);
    ~MasterView();

public slots:
    void goLoginView();
    void goWelcome();
    void goDoctorView();
    void goDepartmentView();
    void goPatientEditView(int num);
    void goPatientView();
    void goPreviousView();
    void pushWidgetToStackView(QWidget *widget);

private slots:
    void on_btBack_clicked();

    void on_stackedWidget_currentChanged(int arg1);

    void on_btlogout_clicked();

private:
    Ui::MasterView *ui;

    WelcomeView *welcomeView;
    DoctorView *doctorView;
    PatientView *patientView;
    DepartmentView *departmentView;
    loginView *loginview;
    PatientEditView *patientEditView;

};
#endif // MASTERVIEW_H
