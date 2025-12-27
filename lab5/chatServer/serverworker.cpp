#include "serverworker.h"
#include "QJsonObject"
#include <QDataStream>
#include <QJsonDocument>

serverWorker::serverWorker(QObject *parent) : QObject(parent)
{
    m_serverSocket = new QTcpSocket(this);
    connect(m_serverSocket,&QTcpSocket::readyRead,this,&serverWorker::onReadyRead);
    connect(m_serverSocket,&QTcpSocket::disconnected,this,&serverWorker::disconnectFromClient);
}

bool serverWorker::setSocketDesctiptor(qintptr socketDesctiptor)
{
    return m_serverSocket->setSocketDescriptor(socketDesctiptor);
}

QString serverWorker::userName()
{
    return m_userName;
}

void serverWorker::setUserName(QString name)
{
    m_userName=name;
}



void serverWorker::onReadyRead()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_12);
    for(;;){
        socketStream.startTransaction();
        socketStream>>jsonData;
        if(socketStream.commitTransaction()){
            // emit logMessage(QString::fromUtf8(jsonData));
            // sendMessage("I receieved message");

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject()) { // and is a JSON object
                    emit logMessage(QJsonDocument(jsonDoc).toJson(QJsonDocument::Compact));
                       emit jsonReceived(this, jsonDoc.object()); // parse the JSON
                }

            }

        }else{
            break;
        }
    }
}

void serverWorker::sendMessage(const QString &text, const QString &type)
{
    if(m_serverSocket->state()!=QAbstractSocket::ConnectedState){
        return ;}

    if(!text.isEmpty()){
        QDataStream serverStream(m_serverSocket);
        serverStream.setVersion(QDataStream::Qt_5_12);

        QJsonObject message;
        message["type"] = type;
        message["text"] = text;

        serverStream<<QJsonDocument(message).toJson();
    }

}

void serverWorker::sendJson(const QJsonObject &json)
{
    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);
    emit logMessage(QLatin1String("Sending to ") + userName() + QLatin1String(" - ") +
                    QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}


