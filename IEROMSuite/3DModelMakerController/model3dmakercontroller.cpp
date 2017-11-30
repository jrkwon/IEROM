#include "model3dmakercontroller.h"
#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <iostream>

IEROM_NAMESPACE_START
Model3DMakerController::Model3DMakerController()
{
    isProcessError = isFinished = false;

    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(updateErrorOutput()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutput()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

}

void Model3DMakerController::updateErrorOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cerr << qPrintable(QString::fromLocal8Bit(errorArray));
}

void Model3DMakerController::updateOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cout << qPrintable(QString::fromLocal8Bit(errorArray));
}

void Model3DMakerController::processFinished( QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        qDebug() << "Detector crashes.";
        isProcessError = true;
    }

    isFinished = true;
}

void Model3DMakerController::processError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart)
    {
        qDebug() << "Detector not found.";
    }
    isProcessError = isFinished = true;
}


IEROM_NAMESPACE_END
