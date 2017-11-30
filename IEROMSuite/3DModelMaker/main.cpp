#include <QCoreApplication>
#include <QFileInfo>
#include "model3dmaker.h"


// ---------------------------------------
// argv
// 1. directory path which contains a set of 256 jpeg files
// 2. specify type of format desired for making the output volume files:
//    0: STL file format
//    1: TIFF file format
//    2: VTK file format
// 3. destination directory path for storing the output volume files

#define nArgIndex 4

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < nArgIndex)
    {
        std::cout << ">>> 3D Model Maker by Akanksha Ashwini <<<" << std::endl;
        std::cout << std::endl;
        std::cout << "Creates Volumes of 256^3 for the given unit volume specification" << std::endl;

        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName)<< std::endl;// << " ImageFileName ShrinkedImageFileName[-v]" << std::endl;
        return -1;
    }
    QString SourceDirPath = args[1];
    int modelformatSel = args[2].toInt();
    QString DestinationDirPath = args[3];

    ierom::Model3DMaker modelMaker(SourceDirPath, DestinationDirPath);

    switch(modelformatSel){
        case ierom::STL:
            modelMaker.CreateStlMeshes();
            break;
        case ierom::TIFF:
            modelMaker.CreateMultiPageTiffs();
            break;
        case ierom::VTK:
            modelMaker.CreateVtkFiles();
            break;
        default:
            break;
    }

    return true;
}

