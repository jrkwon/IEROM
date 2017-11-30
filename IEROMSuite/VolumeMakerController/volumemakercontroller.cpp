#include "volumemakercontroller.h"
#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <iostream>
#include <QDirIterator>
#include <QImage>

IEROM_NAMESPACE_START
VolumeMakerController::VolumeMakerController()
{
    isProcessError = isFinished = false;
    //startX = -1;
    this->width = 0;
    this->height= 0;
    this->depth = 0;

    connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(updateErrorOutput()));
    connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateOutput()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

}

int VolumeMakerController::getImgWidth()
{
    return width/u_width;
}

int VolumeMakerController::getImgHeight()
{
    return height/u_height;
}

int VolumeMakerController::getImgDepth()
{
    float nd = (float)depth/(float)u_depth;int d_int = (int)nd;float d_dec = nd - d_int;
    int d = d_int + (d_dec > 0.05f ? 1 : 0);
    return d;
}

void VolumeMakerController::InitializeDimensions(QString ImageDirectory)
{
    int arraysize = 0;
    QString firstImage;

    QDirIterator sourceImgDir(ImageDirectory, QStringList() << "*.jpg", QDir::Files);
    QString next;

    while(sourceImgDir.hasNext()){
        next = sourceImgDir.next();
        firstImage = sourceImgDir.filePath();
        ++arraysize;
    }

    QImage imagefile;
    imagefile.load(firstImage);
    width = imagefile.width();
    height = imagefile.height();
    depth  = arraysize;
}

void VolumeMakerController::updateErrorOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cerr << qPrintable(QString::fromLocal8Bit(errorArray));
}

void VolumeMakerController::updateOutput()
{
    QByteArray errorArray = process.readAllStandardError();
    std::cout << qPrintable(QString::fromLocal8Bit(errorArray));
}

void VolumeMakerController::processFinished( QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        qDebug() << "Detector crashes.";
        isProcessError = true;
    }

    isFinished = true;
}

void VolumeMakerController::processError(QProcess::ProcessError error)
{
    if(error == QProcess::FailedToStart)
    {
        qDebug() << "Detector not found.";
    }
    isProcessError = isFinished = true;
}



IEROM_NAMESPACE_END
