#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <qtcpsocket.h>

class serverWorker : public QObject
{
    Q_OBJECT
public:
    explicit serverWorker(QObject *parent =nullptr);
    virtual bool setSocketDesctiptor(qintptr socketDesctiptor);

    QString userName();
    void setUserName(QString name);

signals:
    void logMessage(const QString &msg);
    void jsonReceived(serverWorker *sender,const QJsonObject &docObj);
    void disconnectFromClient();

private:
    QTcpSocket * m_serverSocket;
    QString m_userName;


public slots:
    void onReadyRead();
    void sendMessage(const QString &text,const QString &type="message");
    void sendJson(const QJsonObject &json);
};

#endif // SERVERWORKER_H
