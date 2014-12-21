#include "prologserver.h"
#include "prologthread.h"

PrologServer::PrologServer(QObject *parent) :
    QTcpServer(parent)
{
}

void PrologServer::startServer(QString prologPath)
{
    int port = 8766;

    if(!this->listen(QHostAddress::Any, port))
        qDebug() << "Could not start server on port " << port;
    else
        qDebug() << "Listening to port" << port << "...";

    _prologPath = prologPath;

    return;
}

void PrologServer::incomingConnection(qintptr socketId)
{
    qDebug() << socketId << " Connecting...";

    PrologThread *thread = new PrologThread(socketId, _prologPath, this);
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
    return;
}
