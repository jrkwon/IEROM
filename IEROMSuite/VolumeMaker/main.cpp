#include <QCoreApplication>
#include <QFileInfo>
#include "volumemaker.h"


// ---------------------------------------
// argv
// 1. Input image file path name
// 2.

#define nArgIndex 5

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < nArgIndex)
    {
        std::cout << ">>> KESM Volume Maker by Akanksha Ashwini <<<" << std::endl;
        std::cout << std::endl;
        std::cout << "Creates Volumes of 256^3 from the selected location" << std::endl;
        std::cout << "Takes x,y,z points and Source and Destination Directory as Input" << std::endl;

        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName)<< std::endl;// << " ImageFileName ShrinkedImageFileName[-v]" << std::endl;
        return -1;
    }
    QString SourceDirPath = args[1];
    int uX = args[2].toInt();
    int uY = args[3].toInt();
    int uZ = args[4].toInt();

    QString DestinationDirPath = args[5];

    ierom::VolumeMaker createVol(SourceDirPath, uX, uY, uZ, DestinationDirPath);
    createVol.CreateVolumesAndSaveIt();

    return true;
}

