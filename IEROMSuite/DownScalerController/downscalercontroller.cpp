#include "downscalercontroller.h"
#include "downscaler.h"
#include <QFile>

IEROM_NAMESPACE_START
DownScalerController::DownScalerController()
{
    isProcessError = isFinished = false;
    //startX = -1;

    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(updateErrorOutput()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutput()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));
}

void DownScalerController::updateErrorOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cerr << qPrintable(QString::fromLocal8Bit(errorArray));
}

void DownScalerController::updateOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cout << qPrintable(QString::fromLocal8Bit(errorArray));
}

void DownScalerController::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        qDebug() << "Detector crashes.";
        isProcessError = true;
    }

    isFinished = true;
}

void DownScalerController::processError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart)
    {
        qDebug() << "Detector not found.";
    }
    isProcessError = isFinished = true;
}


IEROM_NAMESPACE_END
