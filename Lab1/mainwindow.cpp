#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QPushButton"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnNum0,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum1,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum2,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum3,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum4,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum5,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum6,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum7,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum8,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnNum9,SIGNAL(clicked(bool)),this,SLOT(btnNumClicked()));
    connect(ui->btnMultiple,SIGNAL(clicked(bool)),this,SLOT(binaryOperatorClicked()));
    connect(ui->btnPlus,SIGNAL(clicked(bool)),this,SLOT(binaryOperatorClicked()));
    connect(ui->btnMinus,SIGNAL(clicked(bool)),this,SLOT(binaryOperatorClicked()));
    connect(ui->btnDivide,SIGNAL(clicked(bool)),this,SLOT(binaryOperatorClicked()));

     connect(ui->btnPercentage,SIGNAL(clicked(bool)),this,SLOT(btnUnaryOperatorClicked()));
     connect(ui->btnInverse,SIGNAL(clicked(bool)),this,SLOT(btnUnaryOperatorClicked()));
      connect(ui->btnSquare,SIGNAL(clicked(bool)),this,SLOT(btnUnaryOperatorClicked()));
      connect(ui->btnSqrt,SIGNAL(clicked(bool)),this,SLOT(btnUnaryOperatorClicked()));
      connect(ui->btnPeriod,SIGNAL(clicked(bool)),this,SLOT(btnUnaryOperatorClicked()));
      connect(ui->btnClearAll,SIGNAL(clicked(bool)),this,SLOT(on_btnCe_clicked()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::calculation(bool *ok)
{

    double result=0;

    if(operands.size()==2 && operators.size()>0){
        double operand1=operands.front().toDouble();
        operands.pop_front();
        double operand2=operands.front().toDouble();
        operands.pop_front();

        QString op=operators.front();
        operators.pop_front();
        if(op=="+"){
            result=operand1+operand2;
        }else if(op=="-"){
            result=operand1-operand2;
        }
        else if(op=="×"){
            result=operand1*operand2;
        }
        else if(op=="÷"){
            result=operand1/operand2;
        }

        operands.push_back(QString::number(result));

         ui->statusbar->showMessage(QString("operands is %1,opcode is %2").arg(operands.size()).arg(operators.size()));
    }
    else
        ui->statusbar->showMessage(QString("operands is %1,opcode is %2").arg(operands.size()).arg(operators.size()));

    return QString::number(result);
}

void MainWindow::btnNumClicked()
{
    QString digit=qobject_cast<QPushButton*>(sender())->text();

    if (digit=="0" && operand=="0")
        digit="";

    if (operand=="0" && digit!="0")
        operand="";


    operand+=digit;

    ui->display->setText(operand);
}



void MainWindow::on_btnPeriod_2_clicked()
{

    if (!operand.contains(".") && operand!="")
        operand+=qobject_cast<QPushButton*>(sender())->text();
      ui->display->setText(operand);
}


void MainWindow::on_btnDel_clicked()
{

    operand=operand.left(operand.length()-1);
    ui->display->setText(operand);
}


void MainWindow::on_btnClear_clicked()
{
    operand.clear();
    ui->display->setText(operand);
}

void MainWindow::on_btnCe_clicked()
{
     operand.clear();
     ui->display->setText(operand);
     operands.clear();
     operators.clear();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<event->key();

    if(event->key()==Qt::Key_0)
        ui->btnNum0->animateClick();
    if(event->key()==Qt::Key_1)
        ui->btnNum1->animateClick();
    if(event->key()==Qt::Key_2)
        ui->btnNum2->animateClick();
    if(event->key()==Qt::Key_3)
        ui->btnNum3->animateClick();
    if(event->key()==Qt::Key_4)
        ui->btnNum4->animateClick();
    if(event->key()==Qt::Key_5)
        ui->btnNum5->animateClick();
    if(event->key()==Qt::Key_6)
        ui->btnNum6->animateClick();
    if(event->key()==Qt::Key_7)
        ui->btnNum7->animateClick();
    if(event->key()==Qt::Key_8)
        ui->btnNum8->animateClick();
    if(event->key()==Qt::Key_9)
        ui->btnNum9->animateClick();
    if(event->key()==Qt::Key_Delete)
        ui->btnDel->animateClick();
    if(event->key()==Qt::Key_Q)
        ui->btnPeriod->animateClick();
    if(event->key()==Qt::Key_W)
        ui->btnPeriod_2->animateClick();
    if(event->key()==Qt::Key_E)
        ui->btnPercentage->animateClick();
    if(event->key()==Qt::Key_R)
        ui->btnClearAll->animateClick();
    if(event->key()==Qt::Key_T)
        ui->btnClear->animateClick();
    if(event->key()==Qt::Key_A)
        ui->btnInverse->animateClick();
    if(event->key()==Qt::Key_S)
        ui->btnSquare->animateClick();
    if(event->key()==Qt::Key_D)
        ui->btnSqrt->animateClick();
    if(event->key()==Qt::Key_F)
        ui->btnDivide->animateClick();
    if(event->key()==Qt::Key_G)
        ui->btnMultiple->animateClick();
    if(event->key()==Qt::Key_H)
        ui->btnMinus->animateClick();
    if(event->key()==Qt::Key_Z)
        ui->btnPlus->animateClick();
    if(event->key()==Qt::Key_X)
        ui->btnEqual->animateClick();

}


void MainWindow::binaryOperatorClicked()
{
    QString opcode=qobject_cast<QPushButton*>(sender())->text();
    qDebug()<<opcode;

    if (operand!="" ){
        operands.push_back(operand);
        operand ="";

        operators.push_back(opcode);



    QString result=calculation();

    ui->display->setText(result);
    }

    else if(operands.size()==1 && operators.size()<1){

        operators.push_back(opcode);

    }

}

void MainWindow::btnUnaryOperatorClicked()
{
    if (operand!=""){
        double result=operand.toDouble();
        operand="";

        QString op=qobject_cast<QPushButton*>(sender())->text();
        qDebug()<<op;

        if(op=="%")
            result /=100.0;
        else if(op=="1/x"){
            result= 1/result;
        }
        else if(op=="x^2"){
            result=result*result;
        }
        else if(op=="√")
            result=sqrt(result);

        else if(op=="±"){
            result=-result;
        }

        operands.push_back(QString::number(result));
        ui->display->setText(QString::number(result));
    }
    else if(operands.size()==1){
        double result=operands.front().toDouble();

        QString op=qobject_cast<QPushButton*>(sender())->text();

        if(op=="%")
            result /=100.0;
        else if(op=="1/x"){
            result= 1/result;
        }
        else if(op=="x^2"){
            result=result*result;
        }
        else if(op=="√")
            result=sqrt(result);
        else if(op=="±"){
            result=-result;
        }

        operands.pop();
        operands.push_back(QString::number(result));
        ui->display->setText(QString::number(result));
    }
}



void MainWindow::on_btnEqual_clicked()
{
    if (operand!=""){
        operands.push_back(operand);
        operand="";

    }



    QString result=calculation();

    ui->display->setText(result);

}

