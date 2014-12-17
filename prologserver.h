#ifndef PROLOGSERVER_H
#define PROLOGSERVER_H

#include <QTcpServer>

class PrologServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit PrologServer(QObject *parent = 0);

    void startServer(QString prologPath);

protected:
    void incomingConnection(qintptr socketId);

private:
    QString _prologPath;
};

#endif // PROLOGSERVER_H
