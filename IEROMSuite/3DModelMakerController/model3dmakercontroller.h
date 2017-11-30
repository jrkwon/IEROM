#ifndef MODEL3DMAKERCONTROLLER_H
#define MODEL3DMAKERCONTROLLER_H

#include "Settings.h"
#include "Metadata.h"

#include <QObject>
#include <QProcess>

IEROM_NAMESPACE_START

class Model3DMakerController: public QObject
{
    Q_OBJECT


public:
    Model3DMakerController();

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
};
IEROM_NAMESPACE_END
#endif // MODEL3DMAKERCONTROLLER_H
