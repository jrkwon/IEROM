#include <QCoreApplication>
#include <QDebug>
#include <iostream>

#include "Mission.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ierom::Mission mission;

    qDebug("Raw Image Directory: \n\t%s", qPrintable(mission.rawImageDirectory));
    qDebug("Processed Image Directory: \n\t%s", qPrintable(mission.processedImageDirectory));

    return true; //a.exec();
}
