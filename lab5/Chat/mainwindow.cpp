#include "mainwindow.h"
#include "qjsonobject.h"
#include "ui_mainwindow.h"
#include "QHostAddress"
#include <QDataStream>
#include <QJsonDocument>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
     ui->mainframe->setCurrentWidget(ui->loginPage);
    m_chatclient=new chatClient(this);

     // connect(m_chatclient,&chatClient::messageReceiverd,this,&MainWindow::messageReceived);
     connect(m_chatclient,&chatClient::connected,this,&MainWindow::connectToServer);
     connect(m_chatclient, &chatClient::jsonReceived, this, &MainWindow::jsonReceived);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginbutton_clicked()
{

    m_chatclient->connectToServer(QHostAddress(ui->serverEdit->text()),1967);

}


void MainWindow::on_sendButton_clicked()
{
    if(!ui->saylineEdit->text().isEmpty()){
        m_chatclient->sendMessage(ui->saylineEdit->text());
    }
}


void MainWindow::on_loginoutbutton_clicked()
{
    m_chatclient->disconnectToServer();
    ui->mainframe->setCurrentWidget(ui->loginPage);

    for ( auto aItem : ui->UserlistWidget->findItems(ui->usernameEdit->text(), Qt::MatchExactly) ) {
        qDebug("remove");
        ui->UserlistWidget->removeItemWidget(aItem);
        delete aItem;
    }

}

void MainWindow::connectToServer()
{
    ui->mainframe->setCurrentWidget(ui->chatPage);
    m_chatclient->sendMessage(ui->usernameEdit->text(),"login");
}

void MainWindow::messageReceived(const QString &Sender, const QString &text)
{
    ui->roomtextEdit->append(QString("%1 : %2").arg(Sender).arg(text) );
}



void MainWindow::userJoined(const QString &user)
{
    ui->UserlistWidget->addItem(user);
}



void MainWindow::jsonReceived(const QJsonObject &docObj)
{
    const QJsonValue typeVal = docObj.value("type");
    if (typeVal.isNull() || !typeVal.isString())
        return;

    if (typeVal.toString().compare("message", Qt::CaseInsensitive) == 0) {
        const QJsonValue textVal = docObj.value("text");
         const QJsonValue senderVal = docObj.value("sender");
        if (textVal.isNull() || !textVal.isString())
            return;

        if (senderVal.isNull() || !senderVal.isString())
            return;


        messageReceived(senderVal.toString(),textVal.toString());


    } else if (typeVal.toString().compare("newuser", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value("text");
        qDebug()<<usernameVal.toString()+"heelo";
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userJoined(usernameVal.toString());
    }

    else if (typeVal.toString().compare("userdisconnected", Qt::CaseInsensitive) == 0) {
        const QJsonValue usernameVal = docObj.value("username");
        if (usernameVal.isNull() || !usernameVal.isString())
            return;

        userLeft(usernameVal.toString());
    }else if (typeVal.toString().compare("userlist", Qt::CaseInsensitive) == 0) { // user list
        const QJsonValue userlistVal = docObj.value(QLatin1String("userlist"));
        if (userlistVal.isNull() || !userlistVal.isArray())
            return;

        qDebug() << userlistVal.toVariant().toStringList();
        userlistReceived(userlistVal.toVariant().toStringList());
    }

}

void MainWindow::userlistReceived(const QStringList &list)
{
    ui->UserlistWidget->clear();
    ui->UserlistWidget->addItems(list);
}

void MainWindow::userLeft(const QString &user)
{
    for ( auto aItem : ui->UserlistWidget->findItems(user, Qt::MatchExactly) ) {
        qDebug("remove");
        ui->UserlistWidget->removeItemWidget(aItem);
        delete aItem;
    }
}
