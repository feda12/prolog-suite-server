#include <QCoreApplication>
#include "prologserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PrologServer server;
    server.startServer("/usr/local/bin/swipl");

    return a.exec();
}
