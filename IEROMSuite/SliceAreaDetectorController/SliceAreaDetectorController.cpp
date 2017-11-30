#include "SliceAreaDetectorController.h"
#include <QPoint>
#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <iostream>

IEROM_NAMESPACE_START

DetectorController::DetectorController()
{
    isProcessError = isFinished = false;
    //startX = -1;

    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(updateErrorOutput()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutput()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

}

void DetectorController::updateErrorOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cerr << qPrintable(QString::fromLocal8Bit(errorArray));
}

void DetectorController::updateOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cout << qPrintable(QString::fromLocal8Bit(errorArray));
}

void DetectorController::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        qDebug() << "Detector crashes.";
        isProcessError = true;
    }

    isFinished = true;
}

void DetectorController::processError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart)
    {
        qDebug() << "Detector not found.";
    }
    isProcessError = isFinished = true;
}

QPoint DetectorController::loadSliceAreaPosition(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QPoint(-1, -1);

    QTextStream in(&file);
    int x, y;
    in >> x >> y;
    QPoint sliceAreaPosition(x, y);
    return sliceAreaPosition;
}

///////////////////////////////////////////////////////////////////////////////
//QPoint DetectorController::loadFromSharedMemory()
//{
//    QPoint sliceAreaPosition(-1, -1);
//    if (!sharedMemory.attach()) {
//        std::cout << "Unable to attach to shared memory segment." << std::endl;
//        return sliceAreaPosition;
//    }

//    QBuffer buffer;
//    QDataStream in(&buffer);
//    int x, y;

//    sharedMemory.lock();
//    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
//    buffer.open(QBuffer::ReadOnly);
//    in >> x;
//    in >> y;
//    sliceAreaPosition.setX(x);
//    sliceAreaPosition.setY(y);

//    sharedMemory.unlock();

//    sharedMemory.detach();

//    return sliceAreaPosition;
//}

IEROM_NAMESPACE_END
