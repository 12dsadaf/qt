#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QObject>
#include <QTcpServer>
#include <serverworker.h>

class chatServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit chatServer(QObject *parent =nullptr);

protected:
    void incomingConnection(qintptr SocketDescriptor) override;
    QVector<serverWorker*> m_clients;

    void broadcast(const QJsonObject &message, serverWorker *exclude);

signals:
    void logMessage(const QString& msg);

public slots:
    void stopServer();
    void jsonReceived(serverWorker *sender,const QJsonObject &docObj);
    void userDisconnected(serverWorker *sender);
};

#endif // CHATSERVER_H
