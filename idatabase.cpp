#include "idatabase.h"
#include "QDebug"
#include "QUuid"
void IDataBase::initDataBase()
{
    database=QSqlDatabase::addDatabase("QSQLITE");
    QString aFile="D:/forQtProject/identifier.sqlite";
    database.setDatabaseName(aFile);

    if(!database.open()){
        qDebug()<< "failed to open  database";

    }else
        qDebug()<<"open  database is ok";
}

bool IDataBase::initPatientModel()
{
    patientTabModel =new QSqlTableModel(this,database);
    patientTabModel->setTable("patient");
    patientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    patientTabModel->setSort(patientTabModel->fieldIndex("name"),Qt::AscendingOrder);

    if(!(patientTabModel->select()))
        return false;

    thePatientSelection=new QItemSelectionModel(patientTabModel);
    return true;
}

int IDataBase::addNewPatient()
{
    patientTabModel->insertRow(patientTabModel->rowCount(),QModelIndex());
    QModelIndex curIndex=patientTabModel->index(patientTabModel->rowCount()-1,1);

    int curRecNo =curIndex.row();
    QSqlRecord curRec=patientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP",QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID",QUuid::createUuid().toString(QUuid::WithoutBraces));

    patientTabModel->setRecord(curRecNo,curRec);

    return curIndex.row();
}

bool IDataBase::searchPatient(QString filter)
{
    patientTabModel->setFilter(filter);
    return patientTabModel->select();
}

bool IDataBase::deleteCurrentPatient()
{
    QModelIndex curIndex=thePatientSelection->currentIndex();
    patientTabModel->removeRow(curIndex.row());
    patientTabModel->submitAll();
    patientTabModel->select();

    return NULL;
}

bool IDataBase::submitPatientEdit()
{
    return patientTabModel->submitAll();
}

void  IDataBase::reverPatientEdit()
{
     patientTabModel->revertAll();
}

QString IDataBase::userLogin(QString username, QString password)
{
    QSqlQuery query;
    query.prepare("select username,password from user where username = :USER");
    query.bindValue(":USER",username);
    query.exec();
    query.first();
    if(query.first()&& query.value("username").isValid()){
        QString passwd=query.value("password").toString();
        if(passwd==password){
             qDebug()<<"login ok";
             return "login ok";
        }else{
            qDebug()<<"wrong password";
            return "wrongPassword";
        }
    }else{
        qDebug()<<"no such user";
        return "wrongUsername";
    }
    return NULL;
}

IDataBase::IDataBase(QObject *parent)
    : QObject{parent}
{
    initDataBase();
}
