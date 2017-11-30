#ifndef SLICEAREADETECTORCONTROLLER_H
#define SLICEAREADETECTORCONTROLLER_H

#include "Settings.h"
#include "Metadata.h"

#include <QObject>
#include <QProcess>
#include <QSharedMemory>

IEROM_NAMESPACE_START

class DetectorController : public QObject
{
    Q_OBJECT
public:
    DetectorController();

public slots:
    void updateOutput();
    void updateErrorOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

public:
    /////////////////////
    //QPoint loadFromSharedMemory();
    QPoint loadSliceAreaPosition(QString fileName);

public:
    QProcess process;
    bool isFinished;
    bool isProcessError;

    //////////////////////
    /// Data section
    int columnNumber;
    QString destinationDirectory;
    QString templateFilePath;

    QSharedMemory sharedMemory;
};

IEROM_NAMESPACE_END

#endif // SLICEAREADETECTORCONTROLLER_H
