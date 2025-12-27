#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_chatServer =new chatServer(this);

    connect(m_chatServer,&chatServer::logMessage,this,&MainWindow::logMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{

    if (m_chatServer->isListening()){
        m_chatServer->stopServer();
         ui->startButton->setText("启动服务器");
        logMessage("服务器已经停止");

    }
    else{

        if(!m_chatServer->listen(QHostAddress::Any,1967)){
            QMessageBox::critical(this,"报错","无法启动服务器");
            return ;
        }
        ui->startButton->setText("停止服务器");
        logMessage("服务器已经启动");

    }


}

void MainWindow::logMessage(const QString &msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}



