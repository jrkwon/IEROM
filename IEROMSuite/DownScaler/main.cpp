#include <QCoreApplication>
#include "downscaler.h"

// ---------------------------------
// argv
// 1. Input image file path name
// 2. Output file path name where a shrunk image will be saved


//////////////////////////////////////////////////////////////////////////////

#define kLastArgIndex 3

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < kLastArgIndex)
    {
        std::cout << ">>> KESM Down Sampler by Akanksha Ashwini <<<" << std::endl;
        std::cout << std::endl;
        std::cout << "Subsamples the given image data by 50%" << std::endl;
        std::cout << "and saves the output image data in the output file location provided" << std::endl;

//        QFileInfo pathInfo(args[0]);
//        QString fileName(pathInfo.completeBaseName());
//        std::cout << qPrintable(fileName) << " ImageFileName ShrinkedImageFileName[-v]" << std::endl;
        return -1;
    }
    QString In_ImgFileName = args[1];
    QString Out_ImgFileName = args[2];

    ierom::DownScaler downscale(In_ImgFileName, Out_ImgFileName);

    downscale.shrinkImageAndSaveIt();

    return true; //a.exec();
}


