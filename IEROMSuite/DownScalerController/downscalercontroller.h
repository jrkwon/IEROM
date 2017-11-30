#ifndef DOWNSCALERCONTROLLER_H
#define DOWNSCALERCONTROLLER_H

#include "Settings.h"
#include "Metadata.h"

#include <QObject>
#include <QProcess>
#include <QSharedMemory>

IEROM_NAMESPACE_START
class DownScalerController : public QObject
{
    Q_OBJECT
public:
    DownScalerController();
public slots:
    void updateOutput();
    void updateErrorOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

public:
    QProcess process;
    bool isFinished;
    bool isProcessError;

    //////////////////////
    /// File Selection from Directory
    QString SourceDir;
    QString DestinationDir;

    QSharedMemory sharedMemory;
};

IEROM_NAMESPACE_END

#endif // DOWNSCALERCONTROLLER_H
