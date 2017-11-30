#ifndef VOLUMEMAKERCONTROLLER_H
#define VOLUMEMAKERCONTROLLER_H

#include "Settings.h"
#include "Metadata.h"

#include <QObject>
#include <QProcess>

const int u_width   = 256;
const int u_height  = 256;
const int u_depth   = 256;

IEROM_NAMESPACE_START
class VolumeMakerController : public QObject
{
    Q_OBJECT
public:
    VolumeMakerController();
    void InitializeDimensions(QString ImageDirectory);
    int  getImgWidth();
    int  getImgHeight();
    int  getImgDepth();


public slots:
    void updateOutput();
    void updateErrorOutput();
    void processFinished( QProcess::ExitStatus exitStatus);
    void processError(QProcess::ProcessError error);

public:
    QProcess process;
    bool isFinished;
    bool isProcessError;
    QString sourceDirectory;
    QString destinationDirectory;

private:
    int width;
    int height;
    int depth;


};
IEROM_NAMESPACE_END

#endif // VOLUMEMAKERCONTROLLER_H
