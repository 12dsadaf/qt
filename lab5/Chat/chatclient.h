#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
class chatClient : public QObject
{
    Q_OBJECT
public:
    explicit chatClient(QObject *parent = nullptr);

signals:
    void connected();
    void messageReceiverd(const QString &text);
    void jsonReceived(const QJsonObject &docObj);

private:
    QTcpSocket * m_clientSocket;

public slots:
    void onReadyRead();
    void sendMessage(const QString &text,const QString &type="message");
    void connectToServer(const QHostAddress &address,quint16 port);
    void disconnectToServer();

};

#endif // CHATCLIENT_H
