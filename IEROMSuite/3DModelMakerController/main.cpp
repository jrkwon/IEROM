#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QBuffer>
#include <QElapsedTimer>
#include <iostream>

#include "model3dmakercontroller.h"
#include "Settings.h"
#include "Metadata.h"
#include "Mission.h"

#define nArguments 2
//////////////////////////////////////////////////////////////////////////////
/// \brief main
/// \param argc
/// \param argv[1]: Source Directory containing all Unit Volumes
/// \param argv[2]: Format type: STL / TIFF / VTK
///
///
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    ierom::settings::Settings settings;
    ierom::Mission mission;

    if(args.size() < nArguments) {
        std::cout << ">>> IEROM 3D Model Maker Controller"
                  << settings.processInfo.modelMaker.version.major << "."
                  << settings.processInfo.modelMaker.version.minor <<" <<<" << std::endl;


        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName ) << "Source Directory" << std::endl;
        return false;
    }

    QString SourceDirPath = args[1];
    QString FormatType    = args[2];

    ierom::Model3DMakerController modelMakerController;

    //source directory where the Unit Volume directories for a particular resolution are present
    //ex: D:/KESM-DATA/Unit-Volumes/Unit-Volumes_9600x12000
    QDir source_dir(SourceDirPath);
    modelMakerController.sourceDirectory
            = source_dir.absolutePath(); //+ QString("\\%1").arg(settings.processInfo.modelMaker.unitVolsDirectoryName);

    QStringList sourceResolution = source_dir.dirName().split("_");
    QString resolution = sourceResolution.at(1);

    QString DestDirTypeLabel;
    int nFormatType;

    if(FormatType.contains("STL",Qt::CaseInsensitive)){
        nFormatType = 0;
        DestDirTypeLabel = settings.processInfo.modelMaker.stlsDirectoryName;
    }

    else if(FormatType.contains("TIFF",Qt::CaseInsensitive)){
        nFormatType = 1;
        DestDirTypeLabel = settings.processInfo.modelMaker.tiffsDirectoryName;
    }

    else if(FormatType.contains("VTK",Qt::CaseInsensitive)){
        nFormatType = 2;
        DestDirTypeLabel = settings.processInfo.modelMaker.vtksDirectoryName;
    }



    QDir kesmDataDir(mission.processedImageDirectory);

    //destination directory for the unit volume subdirectories
    modelMakerController.destinationDirectory
            = kesmDataDir.absolutePath() + QString("/%1_%2").arg(DestDirTypeLabel).arg(resolution);

    QDir dir(modelMakerController.destinationDirectory);
    if(!dir.exists())
        dir.mkdir(".");

    //Volume Maker process path
    QString ModelMakerProcessName = QString("%1/%2%3%4/%5%6")
                                    .arg(settings.suiteDir)
                                    .arg(settings.processInfo.pathPrefix)
                                    .arg(settings.processInfo.modelMaker.name)
                                    .arg(settings.processInfo.partialPath)
                                    .arg(settings.processInfo.modelMaker.name)
                                    .arg(settings.processInfo.processExt);

    qDebug() << ModelMakerProcessName;

    std::cout << ">>> IEROM 3D Model Maker Controller ver 1.0 <<<" << std::endl;
    std::cout << "Inovokes a 3D Model Maker method" << std::endl;
    std::cout << "Src: " << qPrintable(modelMakerController.sourceDirectory) << std::endl;
    std::cout << "Dst: " << qPrintable(modelMakerController.destinationDirectory) << std::endl;
    //std::cout << "# of files in Src: " << zFiles << std::endl;

    QDirIterator UnitVolumeDirs(modelMakerController.sourceDirectory, QStringList()<< "*",
                                QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    QString nextdir;
    //QString volDir;
    QString volLabel;

    while(UnitVolumeDirs.hasNext()){
        nextdir = UnitVolumeDirs.next();
        //volDir  = UnitVolumeDirs.filePath();
        volLabel = UnitVolumeDirs.fileInfo().fileName();

        QStringList modelMakerArgs;
        modelMakerArgs << UnitVolumeDirs.filePath()
                       << QString::number(nFormatType)
                       << modelMakerController.destinationDirectory << "-v";

        QFile file(modelMakerController.destinationDirectory + QString("/%1.stl").arg(volLabel));
        if(file.exists())
            continue;

        else{
            QElapsedTimer timer;
            timer.start();
            modelMakerController.process.start(ModelMakerProcessName, modelMakerArgs);
            modelMakerController.process.waitForFinished(-1);
            float elapsedInSec = timer.elapsed() / 1000.0;

            if(modelMakerController.isProcessError)
                break;

            std::cout << "created" << volLabel.toStdString()
                      << std::endl
                      << "\t elpased: " << elapsedInSec << " sec" << std::endl;
        }
    }



    return true; //a.exec();
}

