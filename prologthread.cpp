#include "prologthread.h"

PrologThread::PrologThread(qintptr socketId, QString prologPath, QObject *parent) :
    QThread(parent), _socketId(socketId), _prologPath(prologPath), _cmdSize(0)
{
    // We initialize client's prolog environemt
    _process = new QProcess;
    QObject::connect(this, SIGNAL(started()), this, SLOT(startProcess()));
    QObject::connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOut()));
    QObject::connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(readErrOut()));
    QObject::connect(_process, SIGNAL(started()), this, SLOT(processStarted()));
    QObject::connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
    _process->setProcessChannelMode(QProcess::MergedChannels);
     _process->setProgram(_prologPath);
     _process->setProcessEnvironment(QProcessEnvironment::systemEnvironment());
     _process->setWorkingDirectory(QDir::homePath());
}

void PrologThread::run()
{
    // thread starts here
    qDebug() << " [Server] Prolog Thread started";

    _socket = new QTcpSocket;

    // We set the id of the socket descriptor
    if(!_socket->setSocketDescriptor(_socketId))
    {
        emit error(_socket->error());
        return;
    }

    /* We connect the socket signals to our class signals
     * We make use  of Qt::DirectConnection because it's multithreaded
     */
    QObject::connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    QObject::connect(_socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << "[Thread] Client(" << _socketId << ") connected";

     this->exec();
}

void PrologThread::readyRead()
{
    // Read all data available in the socket
    QDataStream in(_socket);

    if (_cmdSize == 0) // Si on ne connaît pas encore la taille du message, on essaie de la récupérer
    {
        if (_socket->bytesAvailable() < (int)sizeof(quint16)) // On n'a pas reçu la taille du message en entier
             return;

        in >> _cmdSize; // Si on a reçu la taille du message en entier, on la récupère
    }

    // Si on connaît la taille du message, on vérifie si on a reçu le message en entier
    if (_socket->bytesAvailable() < _cmdSize) // Si on n'a pas encore tout reçu, on arrête la méthode
        return;


    // Si ces lignes s'exécutent, c'est qu'on a reçu tout le message : on peut le récupérer !
    QString command;
    in >> command;
    if(command.right(2)  == "\r\n")
        command = command.left(command.size()-2);

    if(_process->processId() != 0)
    {
//        If needed, we can compare command in case we want to specifically do something.
        if(command.left(7) == "consult")
        {
            QString fileContent = command.right(command.size()-7);
            QFile file("/Users/blecam/prolog-consult.pl");
             if (file.open( QIODevice::Truncate|QIODevice::ReadWrite)) {
                    file.reset();
                    file.write(fileContent.toStdString().c_str(), fileContent.size());
                    QString absPath = file.fileName();
                    QString consultCmd = "consult(\'"+absPath+"\'). \r\n";
                    _process->write(consultCmd.toStdString().c_str());
              }
             else {
                 qDebug() << "Error creating a temporary file";
             }
        }
        else{
             command.append("\r\n");
            _process->write(command.toStdString().c_str());
        }
        if(!_process->waitForBytesWritten())
        {
        }
    }
    _cmdSize = 0;
    return;
}

void PrologThread::disconnected()
{
    qDebug() << "[Thread] " << _socketId << " Disconnected";

    _process->kill();
    _socket->deleteLater();
    terminate();
}

void PrologThread::startProcess()
{
    _process->start(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

void PrologThread::processStarted()
{
    qDebug() << "[Prolog] Environment successfully started";
    sendData(QByteArray( "[Prolog] Environment successfully started"));
}

void PrologThread::processError(QProcess::ProcessError error)
{
    qDebug() << "[PrologThread] Error: "<< error;
    sendStr("[PrologThread] Error: "+QString::number(error));
}

void PrologThread::readStdOut()
{
    QByteArray output = _process->readAllStandardOutput();
//    qDebug() << "[Prolog] " << output;
    sendData(output);
}

void PrologThread::readErrOut()
{
    QByteArray errOutput = _process->readAllStandardError();
//    qDebug() << "[Prolog] " << errOutput;

    sendData(errOutput);
}


void PrologThread::sendData(QByteArray results)
{
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);

    QString toSend(results);
    qDebug() << toSend;
    out << (quint16) 0;
    out << toSend;
    out.device()->seek(0);
    out << (quint16) (packet.size() - sizeof(quint16));

    _socket->write(packet);
    _socket->flush();
}

void PrologThread::sendStr(QString str)
{
    str.replace("\n", " ");
    sendData(QByteArray(str.toStdString().c_str()));
}

