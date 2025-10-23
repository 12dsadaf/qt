#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include "QKeyEvent"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString operand;
    QString opcode;
    QStack<QString> operands;
    QStack<QString> operators;
    QString calculation(bool *ok=NULL);

private slots:

    void btnNumClicked();
    void binaryOperatorClicked();
    void btnUnaryOperatorClicked();
    void on_btnPeriod_2_clicked();
    void on_btnDel_clicked();

    void on_btnClear_clicked();
    void on_btnCe_clicked();

    virtual void keyPressEvent(QKeyEvent *event);

    void on_btnEqual_clicked();


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
