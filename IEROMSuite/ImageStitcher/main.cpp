#include <QCoreApplication>
#include <QFileInfo>
#include "imagestitcher.h"

// ---------------------------------------
// argv
// 1. Input image file path name
// 2.

#define nArgIndex 2


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < nArgIndex)
    {
        std::cout << ">>> KESM Image Stitcher by Akanksha Ashwini <<<" << std::endl;
        std::cout << std::endl;
        std::cout << "Stitches the Images from all the columns" << std::endl;
        std::cout << "Takes the Clean Vascular data source directory as input" << std::endl;

        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName)<< std::endl;
        return -1;
    }
    QString SourceDirPath = args[1];


    ierom::ImageStitcher mergeImages(SourceDirPath) ;
    mergeImages.StitchImagesAndSaveIt();

    return true; //return a.exec();
}

