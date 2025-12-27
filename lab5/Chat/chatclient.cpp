#include "chatclient.h"
#include "QJsonObject"
#include <QDataStream>
#include <QJsonDocument>
chatClient::chatClient(QObject *parent)
    : QObject{parent}
{
    m_clientSocket = new QTcpSocket(this);
    connect(m_clientSocket,&QTcpSocket::readyRead,this,&chatClient::onReadyRead);
    connect(m_clientSocket,&QTcpSocket::connected,this,&chatClient::connected);
}




void chatClient::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_clientSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    for(;;){
        socketStream.startTransaction();
        socketStream>>jsonData;
        if(socketStream.commitTransaction()){
            emit messageReceiverd(QString::fromUtf8(jsonData));

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) { // and is a JSON object
                    // emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                    emit jsonReceived(jsonDoc.object()); // parse the JSON
                }

            }


        }else{
            break;
        }
    }
}

void chatClient::sendMessage(const QString &text, const QString &type)
{
    if(m_clientSocket->state()!=QAbstractSocket::ConnectedState)
        return;
    if(!text.isEmpty()){
        QDataStream clientStream(m_clientSocket);
        clientStream.setVersion(QDataStream::Qt_5_12);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        clientStream<<QJsonDocument(message).toJson();
    }
}

void chatClient::connectToServer(const QHostAddress &address, quint16 post)
{
    m_clientSocket->connectToHost(address,post);
}

void chatClient::disconnectToServer()
{
    m_clientSocket->disconnectFromHost();
}
