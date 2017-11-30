#include "SliceAreaDetector.h"
#include <QBuffer>
#include <QDataStream>
#include <QFileInfo>

#include "QuickView.h"

IEROM_NAMESPACE_START

SliceAreaDetector::SliceAreaDetector(QString rawFilePath, int sliceWidth,
                                     QString templateFilePath, QString detectedAreaFileName)
{
    this->rawFilePath = rawFilePath;
    this->sliceWidth = sliceWidth;
    this->sliceRightEdgeTemplate.name = templateFilePath;
    //this->sharedMemoryKeyName = sharedMemoryKeyName;
    this->detectedAreaFileName = detectedAreaFileName;

    this->sliceAreaPosition.setX(-1);
    this->sliceAreaPosition.setY(-1);
    this->isVerbose = true;

    this->params.maxIllumination = 0xFF;
    this->params.minIllumination = 0x67;
    this->params.maxPixelValue = 0xFF;
}

int SliceAreaDetector::sumOfAbsoluteDifferences(
        itk::ImageLinearConstIteratorWithIndex<ImageType> searchIt,
        itk::ImageLinearConstIteratorWithIndex<ImageType> templateIt)
{
    searchIt.SetDirection(0);
    templateIt.SetDirection(0);

    PixelType searchPixelValue, templatePixelValue;
    int sad = 0;

    for(searchIt.GoToBegin(), templateIt.GoToBegin(); !searchIt.IsAtEnd();
        searchIt.NextLine(), templateIt.NextLine())
    {
        for(searchIt.GoToBeginOfLine(); !searchIt.IsAtEndOfLine(); ++searchIt, ++templateIt)
        {
            searchPixelValue = searchIt.Get();
            templatePixelValue = templateIt.Get();
            sad += qAbs((int)searchPixelValue-(int)templatePixelValue);
        }
        //QCoreApplication::processEvents();
    }
    return sad;
}

#define kReLightedFileSave  false

void SliceAreaDetector::reLight(int direction,
                                   itk::ImageLinearConstIteratorWithIndex<ImageType> inputIt,
                                   itk::ImageLinearIteratorWithIndex<ImageType> outputIt)
{
    inputIt.SetDirection(direction);
    outputIt.SetDirection(direction);

    for(inputIt.GoToBegin(), outputIt.GoToBegin(); !inputIt.IsAtEnd();
        inputIt.NextLine(), outputIt.NextLine())
    {
        QVector<PixelType> pixelsInLine;
        double sumOfALine = 0;
        while( !inputIt.IsAtEndOfLine() )
        {
            sumOfALine += inputIt.Get();
            pixelsInLine << inputIt.Get();  // it.Set() doesn't exist in the Const Iterator
            ++inputIt;
        }
        //qSort(pixelsInLine.begin(), pixelsInLine.end());
        //PixelType Median = pixelsInLine[pixelsInLine.size()/2];
        PixelType average = sumOfALine/pixelsInLine.size();

//        bool bIntensityError = false;

//        if(Median < nERROR_THRESHOLD)
//            bIntensityError = true;

        for(inputIt.GoToBeginOfLine(); !inputIt.IsAtEndOfLine(); ++inputIt, ++outputIt)
        {
            PixelType pixelValue = inputIt.Get();
//            if(bIntensityError == true)
//            {
//                if(PixelValue < Median*nERROR_FACTOR) // (1-0.2)
//                {
//                    outputIt.Set(PixelValue); // use the original value
//                    continue;
//                }
//            }
            double adjustPixelValue = (pixelValue/(double)average) * this->params.maxIllumination;
//            pixelValue = (adjustPixelValue > (double)nMAX_PIXEL_VALUE)
            if(adjustPixelValue > (double)this->params.maxIllumination)
                pixelValue = (PixelType)this->params.maxIllumination;
            else if(adjustPixelValue < (double)this->params.minIllumination)
                pixelValue = (PixelType)0;
            else
                pixelValue = (PixelType)adjustPixelValue;

//            pixelValue = (adjustPixelValue > (double)kIllumination)
//                        ? (PixelType)kIllumination : (PixelType)adjustPixelValue;
            outputIt.Set(pixelValue);
        }
        //QCoreApplication::processEvents();
    }
}

ImageType::ConstPointer SliceAreaDetector::readFile(QString filePath)
{
    // new for itk 4.8
    itk::JPEGImageIOFactory::RegisterOneFactory();

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filePath.toStdString().c_str());

    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << "ExceptionObject caught !" << std::endl;
        std::cerr << err << std::endl;
        return NULL;
    }

    return reader->GetOutput();
}

Result SliceAreaDetector::getSliceAreaPositionAndSaveIt()
{
    //    if(QFile::exists(strSrcImage) != true)
    //    {
    //        qDebug() << "Error: " << qPrintable(fileName) << "does not exist.";
    //    }

    //////////////////////////////////////////////////////////////////////////
    // read the template image file & get template image width and height
    ImageType::ConstPointer templateImage = readFile(this->sliceRightEdgeTemplate.name);
    ImageType::SizeType templateImageSize = templateImage->GetLargestPossibleRegion().GetSize();
    this->sliceRightEdgeTemplate.width = templateImageSize[0];
    this->sliceRightEdgeTemplate.height = templateImageSize[1];

    //////////////////////////////////////////////////
    // TODO: Normalize intensity level of the template image
    //

    ///////////////////////////////////////////
    // Read the image to detect a slice region
    QFileInfo fiRaw(this->rawFilePath);
    //QFileInfo fiTemplate(this->sliceRightEdgeTemplate.name);
    //displayMessage(QString("File: %1, Template: %2").arg(fiRaw.fileName()).arg(fiTemplate.fileName()));
    displayMessage(QString("--- : File: %1").arg(fiRaw.fileName()));

    ImageType::ConstPointer inputImage = readFile(this->rawFilePath);

    ImageType::SizeType imageSize = inputImage->GetLargestPossibleRegion().GetSize();
    int imageWidth = imageSize[0];

    ImageType::SizeType size;
    size[0] = this->sliceRightEdgeTemplate.width;
    size[1] = this->sliceRightEdgeTemplate.height;
    ImageType::IndexType start;

    int minSad, minSadX;
    int sad;
    int rightEdgeX;
    uint x, y;

    minSadX = 0;
    minSad = -1;

    for(y = 0, x = imageWidth-size[0]; x > sliceWidth-size[0]/2; x--)
    {
        ///////////////////////////////////////
        // read a region from the search image
        start[0] = x, start[1] = y;

        ImageType::RegionType desiredRegion;
        desiredRegion.SetSize(size);
        desiredRegion.SetIndex(start);
        FilterType::Pointer filter = FilterType::New();
        filter->SetRegionOfInterest(desiredRegion);
        filter->SetInput(inputImage);

        try
        {
            filter->Update();
        }
        catch( itk::ExceptionObject & err )
        {
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
            return Fail;
        }

        //////////////////////////////////////////////////
        // Normalize intensity level of the region
        //////////////////////////////////////////////////
        // 1. prepare the output
        ImageType::Pointer outputImage = ImageType::New();
        outputImage->SetRegions( filter->GetOutput()->GetRequestedRegion() );
        //outputImage->CopyInformation(pReader->GetOutput());
        outputImage->Allocate();
        // 2. fill buffer with initial value
        ImageType::PixelType  initialValue = this->params.maxPixelValue;
        outputImage->FillBuffer( initialValue );
        // 3. re-light
        ConstIteratorType inputItX(filter->GetOutput(), filter->GetOutput()->GetRequestedRegion());
        IteratorType      outputItX(outputImage, filter->GetOutput()->GetRequestedRegion());
        reLight(0, inputItX, outputItX);

        //ConstIteratorType inputItY(outputImage, filter->GetOutput()->GetRequestedRegion());
        //IteratorType      outputItY(outputImage, filter->GetOutput()->GetRequestedRegion());
        //doRelight(1, inputItY, outputItY);
        /////////////////////////////////////



#if kReLightedFileSave
        ////////////////////
        // relighting debug
        WriterType::Pointer writer = WriterType::New();
        static int i = 0;
        QString tempFilename = QString("%1.jpg").arg(i++);
        writer->SetFileName(tempFilename.toStdString().c_str());
        //writer->SetInput(filter->GetOutput());
        writer->SetInput(outputImage );

        try
        {
            writer->Update();
        }
        catch ( itk::ExceptionObject &err)
        {
            std::cerr << "ExceptionObject caught !" << std::endl;
            std::cerr << err << std::endl;
            return Fail;
        }
#endif


        //////////////////////////////////////////
        // Calculate Sum of Absolute Differences
        //////////////////////////////////////////
        //ConstIteratorType searchIt(filter->GetOutput(), filter->GetOutput()->GetRequestedRegion());
        ConstIteratorType searchIt(outputImage, outputImage->GetRequestedRegion());
        ConstIteratorType templateIt(templateImage, templateImage->GetRequestedRegion());

        sad = sumOfAbsoluteDifferences(searchIt, templateIt);
        if((minSad == -1) || (sad < minSad))
        {
            minSad = sad;
            minSadX = x;
            //            qDebug() << "minSad: " << minSad << "minSadX: " << minSadX;
        }

        showProgress(imageWidth-size[0], sliceWidth-size[0]/2, x);
    }
    // to guarantee "100%" displayed.
    showProgress(imageWidth-size[0], sliceWidth-size[0]/2, sliceWidth-size[0]/2);
    std::cout << std::endl;

    rightEdgeX = minSadX +  this->sliceRightEdgeTemplate.width/2;
    int startX = rightEdgeX - this->sliceWidth;

    //displayMessage(QString("startX: %1").arg(startX));

    sliceAreaPosition.setX(startX);
    // TODO: registration for y axis may be required.
    sliceAreaPosition.setY(0); // always 0 for now.

    saveSliceAreaPosition();

    return Success;
}

void SliceAreaDetector::showProgress(int start, int end, int current)
{
    if(!this->isVerbose)
        return;

//    char verb[] = "-\\|/";
//    static int i = 0;

//    if(verb[i] == '\0')
//        i = 0;
//    std::cout << verb[i++] << "\r";
    int range = qAbs(end - start);
    int progress = qAbs(start-current)/(float)range * 100;

    QString msg = QString("%1%").arg(progress, 3);
    std::cout << "\r" << msg.toStdString().c_str();
}

void SliceAreaDetector::saveSliceAreaPosition()
{
    QFile file(this->detectedAreaFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << this->sliceAreaPosition.x() << " " << this->sliceAreaPosition.y();
}

//void SliceAreaDetector::saveToSharedMemory(QPoint sliceAreaPosition)
//{
//    // -----------------------------------------------------------------------
//    // Note: QSharedMemory doesn't work without manually setting a key in
//    // Windows 10.
//    sharedMemory.setKey(this->sharedMemoryKeyName);

//    if (sharedMemory.isAttached())
//        sharedMemory.detach();

//    QBuffer buffer;
//    buffer.open(QBuffer::ReadWrite);
//    QDataStream out(&buffer);
//    out << sliceAreaPosition.x();
//    out << sliceAreaPosition.y();

//    int size = buffer.size();

//    if (!sharedMemory.create(size)) {
//        qDebug() << "Error: Unable to create shared memory segment.";
//        qDebug() << "       " << sharedMemory.errorString();
//        return;
//    }
//    sharedMemory.lock();
//    char *to = (char*)sharedMemory.data();
//    const char *from = buffer.data().data();
//    memcpy(to, from, qMin(sharedMemory.size(), size));
//    sharedMemory.unlock();

//    QString msg;
//    if(sliceAreaPosition.x() == -1 || sliceAreaPosition.y() == -1)
//        msg = QString("\n\tNo slice area is found.\n");
//    else
//        msg = QString("\n\tx: %1, y: %2\n").arg(sliceAreaPosition.x()).arg(sliceAreaPosition.y());
//    displayMessage(msg);
//}

void SliceAreaDetector::setVerbose(bool isVerbose)
{
    this->isVerbose = isVerbose;
}

void SliceAreaDetector::displayMessage(QString message)
{
    if(isVerbose)
        std::cout << message.toStdString().c_str();
}

IEROM_NAMESPACE_END
