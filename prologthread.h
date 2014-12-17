#ifndef PROLOGTHREAD_H
#define PROLOGTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QTemporaryFile>

/* We want to be able to listen to multiple clients at once.
 * Thus, we create a thread for each connection that will
 * communicate with one and only one client.
 * Actions will be performed in the thread.
*/

class PrologThread : public QThread
{
    Q_OBJECT
public:
    explicit PrologThread(qintptr socketId, QString prologPath, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError sockErr);

public slots:
    void readyRead();
    void disconnected();

private slots:
    void readStdOut();
    void readErrOut();
    void startProcess();
    void processStarted();
    void processError(QProcess::ProcessError error);

    void sendData(QByteArray results);
    void sendStr(QString str);
private:
    QTcpSocket *_socket;
    qintptr _socketId;
    QProcess *_process;
    QString _prologPath;
    quint16 _cmdSize;
};

#endif // PROLOGTHREAD_H
