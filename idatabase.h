#ifndef IDATABASE_H
#define IDATABASE_H

#include <QObject>
#include <qtsql>
#include <QSqlDatabase>
#include <QDataWidgetMapper>
class IDataBase : public QObject
{
    Q_OBJECT
public:

    static IDataBase&getInstance()
    {
        static IDataBase instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    QString userLogin(QString username,QString password);


private:
     explicit IDataBase(QObject *parent = nullptr);
    IDataBase(IDataBase const&)       = delete;
    void operator=(IDataBase const&)  = delete;
    void initDataBase();
    QSqlDatabase database;


signals:

public:
    bool initPatientModel();
    int addNewPatient();
    bool searchPatient(QString filter);
    bool deleteCurrentPatient();
    bool submitPatientEdit();
    void reverPatientEdit();
    QItemSelectionModel *thePatientSelection;
    QSqlTableModel *patientTabModel;

private:


};

#endif // IDATABASE_H
