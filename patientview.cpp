#include "patientview.h"
#include "ui_patientview.h"
#include "idatabase.h"
PatientView::PatientView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PatientView)
{
    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);

    IDataBase &iDatabase= IDataBase::getInstance();
    if(iDatabase.initPatientModel()){
        ui->tableView->setModel(iDatabase.patientTabModel);
        ui->tableView->setSelectionModel(iDatabase.thePatientSelection);
    }
}

PatientView::~PatientView()
{
    delete ui;
}

void PatientView::on_btAdd_clicked()
{
    int currentRow=IDataBase::getInstance().addNewPatient();
    emit goPatientEditView(currentRow);
}


void PatientView::on_btSearch_clicked()
{
    QString filter=QString ("name like '%%1%' ").arg(ui->txtSearch->text());
    IDataBase::getInstance().searchPatient(filter);
}


void PatientView::on_btDelete_clicked()
{
    IDataBase::getInstance().deleteCurrentPatient();
}


void PatientView::on_btAlter_clicked()
{
    QModelIndex curIndex=IDataBase::getInstance().thePatientSelection->currentIndex();


    emit goPatientEditView(curIndex.row());
}

