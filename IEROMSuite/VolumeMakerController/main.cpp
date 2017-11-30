#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
//#include <QImage>
#include <QTextStream>
#include <QBuffer>
#include <QElapsedTimer>

#include <iostream>

#include "volumemakercontroller.h"
#include "Settings.h"
#include "Metadata.h"
#include "Mission.h"

#define nArguments 2
//////////////////////////////////////////////////////////////////////////////
/// \brief main
/// \param argc
/// \param argv[1]: Source Directory
/// \param argv[2]: X co-ordinate as pixel value
/// \param argv[3]: Y co-ordinate as pixel value
/// \param argv[4]: Z co-ordinate
///
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    ierom::settings::Settings settings;
//    ierom::Metadata metadata;
//    ierom::Mission mission;

    if(args.size() < nArguments) {
        std::cout << ">>> IEROM Volume Maker Controller"
                  << settings.processInfo.volumemaker.version.major << "."
                  << settings.processInfo.volumemaker.version.minor <<" <<<" << std::endl;
        std::cout << "Takes x,y,z co-ordinate inputs from the GUI and pass them to Unit Volume Maker." << std::endl;

        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName ) << "Source Directory" << std::endl;
        return false;
    }

    QString SourceDirPath = args[1];
    QString DestinationDirPath = args[2];
    ierom::VolumeMakerController volMakerController;

    //source directory where the whole mouse brain vasculature images are present
    volMakerController.sourceDirectory
            = SourceDirPath; //+ QString("\\%1").arg(settings.processInfo.volumemaker.imageDirectoryName);

    //destination directory for the unit volume subdirectories
    volMakerController.destinationDirectory
            = DestinationDirPath;//SourceDirPath + QString("\\%1").arg(settings.processInfo.volumemaker.unitVolsDirectoryName);

    //Volume Maker process path
    QString VolMakerProcessName = QString("%1/%2%3%4/%5%6")
                                    .arg(settings.suiteDir)
                                    .arg(settings.processInfo.pathPrefix)
                                    .arg(settings.processInfo.volumemaker.name)
                                    .arg(settings.processInfo.partialPath)
                                    .arg(settings.processInfo.volumemaker.name)
                                    .arg(settings.processInfo.processExt);

    qDebug() << VolMakerProcessName;

    volMakerController.InitializeDimensions(volMakerController.sourceDirectory);

    std::cout << ">>> IEROM Volume Maker Controller ver 1.0 <<<" << std::endl;
    std::cout << "Inovokes a Volume Maker method to create unit volumes of 256*256*256" << std::endl;
    std::cout << "Src: " << qPrintable(volMakerController.sourceDirectory) << std::endl;
    std::cout << "Dst: " << qPrintable(volMakerController.destinationDirectory) << std::endl;
    //std::cout << "# of files in Src: " << zFiles << std::endl;

    //use this when we want to create unit volumes for the complete data set
    int w = volMakerController.getImgWidth();
    int h = volMakerController.getImgHeight();
    int d = volMakerController.getImgDepth();

    int uX=0; int uY=0; int uZ=0;

    uZ = 0;//for(uZ=0; uZ<d; ++uZ)
    {
        for(uX=0; uX<w; ++uX)
        {
            for(uY=0; uY<h; ++uY)
            {
                QStringList volMakerArgs;
                volMakerArgs << volMakerController.sourceDirectory
                             << QString::number(uX)
                             << QString::number(uY)
                             << QString::number(uZ)
                             << volMakerController.destinationDirectory << "-v";

                QDir tempchk(volMakerController.destinationDirectory +
                        QString("/Vol_%1_%2_%3").arg(QString::number(uX)).arg(QString::number(uY)).arg(QString::number(uZ)));

                if(tempchk.exists())
                    continue;
                else{
                    //call the volumemaker process for all x, y, z coordinates
                    QElapsedTimer timer;
                    timer.start();
                    volMakerController.process.start(VolMakerProcessName, volMakerArgs);
                    volMakerController.process.waitForFinished(-1);
                    float elapsedInSec = timer.elapsed() / 1000.0;

                    if(volMakerController.isProcessError)
                        break;

                    std::cout << "Vol_" <<uX<<"_"<<uY<<"_"<<uZ
                              << std::endl
                              << "\t elpased: " << elapsedInSec << " sec" << std::endl;
                }

            }
        }
    }


    return true; //a.exec();
}

