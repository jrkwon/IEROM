#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QBuffer>
#include <QPoint>
#include <QElapsedTimer>

#include <iostream>

#include "SliceAreaDetectorController.h"
#include "SliceAreaPositionWriter.h"
#include "Settings.h"
#include "Metadata.h"
#include "Mission.h"

// test comments.

//////////////////////////////////////////////////////////////////////////////
/// \brief main
/// \param argc
/// \param argv[1]: a column number where DetectorController takes care of
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
        std::cout << ">>> IEROM Slice Area Dectector Controller "
                  << settings.processInfo.detector.version.major << "."
                  << settings.processInfo.detector.version.minor
                  << " <<<" << std::endl;
        std::cout << "Detects the slice areas of all IEROM raw images in a column." << std::endl;

        QFileInfo pathInfo(args[0]);
        QString fileName(pathInfo.completeBaseName());
        std::cout << qPrintable(fileName )<< " ColumnNumber" << std::endl;
        return false;
    }

    ierom::DetectorController detectorController;
    QString columnNumStr = args[1];
    detectorController.columnNumber = columnNumStr.toInt();

    // create the destination directory name using ID name from Metatdata
    detectorController.destinationDirectory
            = QString("%1/%2").arg(mission.processedImageDirectory).arg(metadata.tissueSample.id);
    if(!QDir(detectorController.destinationDirectory).exists())
        QDir().mkdir(detectorController.destinationDirectory);

    //////////////////
    /// Detector full file path
    QString detectorProcessName = QString("%1/%2%3%4/%5%6")
                                    .arg(settings.suiteDir)
                                    .arg(settings.processInfo.pathPrefix)
                                    .arg(settings.processInfo.detector.name)
                                    .arg(settings.processInfo.partialPath)
                                    .arg(settings.processInfo.detector.name)
                                    .arg(settings.processInfo.processExt);

    qDebug() << detectorProcessName;

    /////////////////
    // Column folder name - padding 0s
    QString formatStr = "%0" + QString("%1d").arg(settings.processInfo.detector.columNumberDigits);
    QString formattedColumnNumber;
    formattedColumnNumber.sprintf(formatStr.toStdString().c_str(), detectorController.columnNumber);

    /////////////////
    // Template file path
    QString templateFilePath = QString("%1/%2%3")
            .arg(detectorController.destinationDirectory)
            .arg(settings.processInfo.detector.templateFileName)
            .arg(metadata.imageFileExtension);

    //qDebug() << templateFilePath;

    /////////////////
    // Make a list of all images in a specified column number
    QString rawImagePathWithColumnNumber
            = QString("%1/%2").arg(mission.rawImageDirectory).arg(formattedColumnNumber);
    QDir rawImageDir(rawImagePathWithColumnNumber);
    QStringList filters;
    filters << QString("*%1").arg(metadata.imageFileExtension);
    rawImageDir.setNameFilters(filters);
    QStringList imageFileList = rawImageDir.entryList(QDir::Files);
    QString imageFileName;

    std::cout << ">>> IEROM Slice Area Dectector Controller ver 1.0 <<<" << std::endl;
    std::cout << "Inovokes a Detector to detect a slice area in a raw image" << std::endl;
    std::cout << "and does the same thing in a column for all images." << std::endl;
    std::cout << "Src: " << qPrintable(mission.rawImageDirectory) << std::endl;
    std::cout << "Dst: " << qPrintable(detectorController.destinationDirectory) << std::endl;
    std::cout << "Col: " << qPrintable(formattedColumnNumber) << std::endl;
    std::cout << "# of files: " << imageFileList.count() << std::endl;

    QPoint sliceAreaPosition;
    QString sliceAreaFileName = settings.suiteDir + "/" + settings.processInfo.globalSharedMemoryKey;

    QString detectedAreaPath = QString("%1/%2")
            .arg(detectorController.destinationDirectory)
            .arg(settings.directoryName.detected);
    if(!QDir(detectedAreaPath).exists())
        QDir().mkdir(detectedAreaPath);
    QString detectedAreaFilePath = detectedAreaPath + "/" + formattedColumnNumber + ".txt";
    QString detectedAreaJsonFilePath = detectedAreaPath + "/" + formattedColumnNumber + ".json";
    ierom::SliceAreaPositionWriter sliceAreaPositionWriter(detectedAreaFilePath, detectedAreaJsonFilePath);

    for (int i = sliceAreaPositionWriter.sliceAreas.size(); i < imageFileList.count(); i++) {
        imageFileName = QString("%1/%2")
                            .arg(rawImagePathWithColumnNumber)
                            .arg(imageFileList[i]);

        QStringList detectorArgs;
        detectorArgs << imageFileName
                     << QString::number(metadata.sectioning.sliceSize.width)
                     << templateFilePath << sliceAreaFileName << "-v";

        QElapsedTimer timer;
        timer.start();
        detectorController.process.start(detectorProcessName, detectorArgs);
        detectorController.process.waitForFinished();
        float elapsedInSec = timer.elapsed() / 1000.0;

        if(detectorController.isProcessError)
            break;

        sliceAreaPosition = detectorController.loadSliceAreaPosition(sliceAreaFileName);
        std::cout << i+1 << "/" << imageFileList.count() << ": " << qPrintable(imageFileList[i])
                  << std::endl
                  << "x = " << sliceAreaPosition.x()
                  << "\t y = " << sliceAreaPosition.y()
                  << "\t elpased: " << elapsedInSec << " sec" << std::endl;

        ierom::SliceAreaInfo sliceAreaInfo;
        sliceAreaInfo.fileName = imageFileList[i];
        sliceAreaInfo.sliceArea = sliceAreaPosition;
        //sliceAreaInfo.valid = (sliceAreaPosition.x() != -1) ? true : false;
        sliceAreaPositionWriter.sliceAreas.append(sliceAreaInfo);

        sliceAreaPositionWriter.write();
    }
//    sliceAreaPositionWriter.write();

    sliceAreaPositionWriter.writeJson();

    return true; //a.exec();
}
