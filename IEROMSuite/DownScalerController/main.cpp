#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

#include "downscalercontroller.h"
#include "Settings.h"
#include "Metadata.h"
#include "Mission.h"

//////////////////////////////////////////////////////////////////////////////
/// \brief main
/// \param argc
/// \param argv[1]: a source directory where DownScalerController takes command
/// \return
///

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    ierom::settings::Settings settings;
    ierom::Metadata metadata;
    ierom::Mission mission;

    if(args.size() < 2) {
        qDebug() << ">>> IEROM Down Scaler Controller "
                  << settings.processInfo.detector.version.major << "."
                  << settings.processInfo.detector.version.minor
                  << " <<<" << "\n";
        qDebug() << "Down scales or subsamples IEROM merged data." << "\n";

//        QFileInfo pathInfo(args[0]);
//        QString fileName(pathInfo.completeBaseName());
//        std::cout << qPrintable(fileName )<< " ColumnNumber" << std::endl;
        return false;
    }
    QString SourceDirPath = args[1];
    ierom::DownScalerController scalerController;
    scalerController.SourceDir = SourceDirPath;

    // create the destination directory name using ID name from Metatdata
    scalerController.DestinationDir
            = QString("%1").arg(mission.processedImageDirectory);
    if(!QDir(scalerController.DestinationDir).exists())
        QDir().mkdir(scalerController.DestinationDir);

    //////////////////
    /// Detector full file path
    QString scalerProcessName = QString("%1/%2%3%4/%5%6")
                                    .arg(settings.suiteDir)
                                    .arg(settings.processInfo.pathPrefix)
                                    .arg(settings.processInfo.downscaler.name)
                                    .arg(settings.processInfo.partialPath)
                                    .arg(settings.processInfo.downscaler.name)
                                    .arg(settings.processInfo.processExt);

    qDebug() << scalerProcessName;

    QDirIterator dir_iter(scalerController.SourceDir,QStringList() << "*.jpg", QDir::Files);
    QString next;
    QString inFileName;
    int i =0;

    while(dir_iter.hasNext()){
        next = dir_iter.next();
        inFileName = dir_iter.fileInfo().completeBaseName();

        if(i%2==0){
        QString imgFileName = dir_iter.fileInfo().absoluteFilePath();

        //Template file path
        QString templateFilePath = QString("%1/%2")
                .arg(scalerController.destinationDir)
                .arg(settings.inFileName);


        QString outFileName = templateFilePath.append(".jpg");

        QStringList detectorArgs;
        detectorArgs << imgFileName
                     << outFileName  << "-v";

        QElapsedTimer timer;
        timer.start();
        scalerController.process.start(scalerProcessName, detectorArgs);
        scalerController.process.waitForFinished();
        //float elapsedInSec = timer.elapsed() / 1000.0;

        if(scalerController.isProcessError)
            break;
        }
        i++;
    }


    return a.exec();
}

