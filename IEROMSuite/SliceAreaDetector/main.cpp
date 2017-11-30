#include <QCoreApplication>
#include <QFileInfo>
#include "SliceAreaDetector.h"

// ---------------------------------
// argv
// 1. Raw image file path name
// 2. Slice area width
// 3. Template file path name
// 4. Filename where a detected area positin will be saved
// 5. Options
//    -v: [verbose] message displayed

//////////////////////////////////////////////////////////////////////////////
// Note:
// I've got "undefined reference to WinMain@16 error,
// when using "namespace std { }"

#define kLastArgIndex 5

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();

    if(args.size() < kLastArgIndex)
    {
        std::cout << ">>> KESM Slice Area Dectector ver 1.0 by Jaerock Kwon <<<" << std::endl;
        std::cout << std::endl;
        std::cout << "Detects the slice area of an KESM raw image" << std::endl;
        std::cout << "and saves starting x and y positions into the specified" << std::endl;
        std::cout << "shared memory by the key name." << std::endl;


        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName) << " ImageFileName SliceWidth TemplateFilename detectedAreaFileName [-v]" << std::endl;
        return -1;
    }
    QString rawImageFileName = args[1];
    int sliceWidth = args[2].toInt();
    QString templateName = args[3];
    QString detectedAreaFileName = args[4];

    ierom::SliceAreaDetector detector(rawImageFileName, sliceWidth, templateName, detectedAreaFileName);
    bool isVerbose = false;
    if(args.size() >= (kLastArgIndex+1)) {
        isVerbose = (args[kLastArgIndex] == "-v") ? true : false;
    }
    detector.setVerbose(isVerbose);
    detector.getSliceAreaPositionAndSaveIt();

    return true; //a.exec(); // sliceStartPosition.x; //a.exec();
}

