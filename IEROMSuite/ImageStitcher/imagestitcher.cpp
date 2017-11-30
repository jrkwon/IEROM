#include "imagestitcher.h"
#include <QDirIterator>
#include <QThread>
#include <QtConcurrent/QtConcurrent>



IEROM_NAMESPACE_START

ImageStitcher::ImageStitcher(QString SourceDirectory)
{
    QDir source(SourceDirectory);
    this->SourceDir = source.absolutePath();
    if(source.cdUp()){
        if(!source.cd("StitchedImages")) source.mkdir("StitchedImages") ;
        this->DestinationDir = source.absolutePath();
    }

    if(source.cdUp() && source.cd(SourceDir)){
       if(source.cd("Filler"))this->filler_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00000")) this->col0_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00001")) this->col1_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00002")) this->col2_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00003")) this->col3_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00004")) this->col4_dirPath = source.absolutePath();
       if(source.cdUp() && source.cd("00005")) this->col5_dirPath = source.absolutePath();
    }
}
/////////////////////////////////////////////////////////////////////////////////
/// Itk Image Reader
/// \brief ImageStitcher::readFile
/// \param filePath
/// \return itk image reader output
///
ImageType::ConstPointer ImageStitcher::readFile(QString filePath)
{
    itk::JPEGImageIOFactory::RegisterOneFactory();

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filePath.toStdString());
    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & err )
    {
        std::cerr << "ExceptionObject Caught !!!" << std::endl;
        std::cerr << err << std::endl;
        return NULL;
    }

    return reader->GetOutput();
}

///////////////////////////////////////////////////////////////////////////////////////////////
///Inverse the Intensity of the Clean Vascular Data
///\brief ImageStitcher::InverseIntensity
/// \param imagefilePath
/// \return image with inversed intensity output
///
ImageType::ConstPointer ImageStitcher::InverseIntensity(QString imagefilePath)
{
    ImageType::ConstPointer image = readFile(imagefilePath);
    InvertIntensityFilterType::Pointer invertIntensityFilter = InvertIntensityFilterType::New();
    invertIntensityFilter->SetInput(image);
    invertIntensityFilter->SetMaximum(Maximum);
    invertIntensityFilter->Update();

    return invertIntensityFilter->GetOutput();
}
////////////////////////////////////////////////////////////////////////////////////////
/// \brief ImageStitcher::BinaryImageFilter
/// \param image
/// \return binary output image
///

ImageType::ConstPointer ImageStitcher::BinaryImageFilter(ImageType::ConstPointer image)
{
    BinaryThresholdFilterType::Pointer BinaryImageFilter = BinaryThresholdFilterType::New();
    BinaryImageFilter->SetInput(image);
    BinaryImageFilter->SetLowerThreshold(1);
    BinaryImageFilter->SetUpperThreshold(70);
    BinaryImageFilter->SetInsideValue(255); //white
    BinaryImageFilter->SetOutsideValue(0); // black
    BinaryImageFilter->Update();

    return BinaryImageFilter->GetOutput();
}


//////////////////////////////////////////////////////////////////////////
///Flip the Images which are marked as right edged
///
ImageType::ConstPointer ImageStitcher::FlipImagetoLeft(ImageType::ConstPointer image)
{
    itk::FixedArray<bool, 2> flipAxes;
    flipAxes[0] = true;
    flipAxes[1] = false;
    FlipImageFilterType::Pointer FlipImageFilter = FlipImageFilterType::New();
    FlipImageFilter->SetInput(image);
    FlipImageFilter->SetFlipAxes(flipAxes);
    FlipImageFilter->Update();

    return FlipImageFilter->GetOutput();
}


/////////////////////////////////////////////////////////////////////////////////
///
///
QString ImageStitcher::CreateEmptyImage()
{
    //create an empty image for all "not found" substitution
    QDir dir(col0_dirPath);
    QString firstfile = dir.filePath("20080415_135412_x148.0967y31.1286z10.5830_t0.001000_v17.4763_929N.jpg");
    ImageType::ConstPointer firstImage = readFile(firstfile);
    ImageType::RegionType region = firstImage->GetLargestPossibleRegion();

    ImageType::Pointer EmptyImage = ImageType::New();
    EmptyImage->SetRegions(region);
    EmptyImage->Allocate();

    ConstIteratorType in(firstImage,region);
    IteratorType out(EmptyImage,region);
    in.GoToBegin();
    out.GoToBegin();

    while(!in.IsAtEnd())
    {
            out.Set(pixelVal);
            ++in;++out;
    }

     //Save the image
     WriterType::Pointer writer = WriterType::New();
     QDir fillerdir(filler_dirPath);
     QString fillerFile = fillerdir.absolutePath().append("/filler.jpg");
     writer->SetFileName(fillerFile.toStdString());
     writer->SetInput(EmptyImage);
     writer->Update();

     return fillerFile;
}

QString ImageStitcher::getZCoordinate(QString file)
{
    QStringList myStringList = file.split('_');
    QStringList coordinates = myStringList.at(2).split('z');
    return coordinates.at(1);
}

QString ImageStitcher::getFileforSameZCoordinate(QString col_path, QString z)
{
    QDirIterator diriter(col_path,QStringList() << "*.jpg", QDir::Files);
    QString nxt;
    QString fileName;

    //Directory Iterator to iterate over each file
    while(diriter.hasNext()){
        nxt = diriter.next();
        fileName = diriter.fileInfo().completeBaseName();
        QString z_local = getZCoordinate(fileName);
        float z_val = z.toFloat();
        float z_local_val = z_local.toFloat();
        if(z_local_val == z_val)
            return diriter.fileInfo().absoluteFilePath();
        else if(z_local_val > z_val)
            break;
        else
            continue;
    }
    return fillerFile;
}
/////////////////////////////////////////////////////////////////////////////
///Stitch Images : Look for files in the directories and tile them together
///
Result ImageStitcher::StitchImagesAndSaveIt()
{
    this->fillerFile = CreateEmptyImage();
    QDirIterator dir_iter(col4_dirPath,QStringList() << "*.jpg", QDir::Files);
    QString next;
    QString inFileName;

    //Directory Iterator to iterate over each file
    while(dir_iter.hasNext()){
        next = dir_iter.next();
        inFileName = dir_iter.fileInfo().completeBaseName();
        QString z = getZCoordinate(inFileName);

//        QString file_col0 = getFileforSameZCoordinate(col0_dirPath, z);
//        QString file_col1 = getFileforSameZCoordinate(col1_dirPath, z);
//        QString file_col2 = getFileforSameZCoordinate(col2_dirPath, z);
//        QString file_col3 = getFileforSameZCoordinate(col3_dirPath, z);
//        QString file_col5 = getFileforSameZCoordinate(col5_dirPath, z);
        QThreadPool threadpool;

        QFuture<QString> thread0 =
                QtConcurrent::run(&threadpool,getFileforSameZCoordinate, col0_dirPath, z);
        QFuture<QString> thread1 =
                QtConcurrent::run(&threadpool,getFileforSameZCoordinate, col1_dirPath, z);
        QFuture<QString> thread2 =
                QtConcurrent::run(&threadpool,getFileforSameZCoordinate, col2_dirPath, z);
        QFuture<QString> thread3 =
                QtConcurrent::run(&threadpool,getFileforSameZCoordinate, col3_dirPath, z);
        QFuture<QString> thread5 =
                QtConcurrent::run(&threadpool,getFileforSameZCoordinate, col5_dirPath, z);

        thread0.waitForFinished();
        thread1.waitForFinished();
        thread2.waitForFinished();
        thread3.waitForFinished();
        thread5.waitForFinished();

        QString file_col0 = thread0.result();
        QString file_col1 = thread1.result();
        QString file_col2 = thread2.result();
        QString file_col3 = thread3.result();
        QString file_col5 = thread5.result();

        //Read an Image
        ImageType::ConstPointer image0 = readFile(file_col0);
        ImageType::ConstPointer image1 = readFile(file_col1);
        ImageType::ConstPointer image2 = readFile(file_col2);
        ImageType::ConstPointer image3 = readFile(file_col3);
        ImageType::ConstPointer image4 = readFile(dir_iter.fileInfo().absoluteFilePath());
        ImageType::ConstPointer image5 = readFile(file_col5);


        //Stitch the images side-by-side
        TileFilterType::Pointer tileFilter = TileFilterType::New();
        itk::FixedArray< unsigned int, 2 > layout;
        layout[0] = 6; //6 images side-by-side
        layout[1] = 0;

        tileFilter->SetLayout( layout );
        tileFilter->SetInput(0, image5);
        tileFilter->SetInput(1, image4);
        tileFilter->SetInput(2, image3);
        tileFilter->SetInput(3, image2);
        tileFilter->SetInput(4, image1);
        tileFilter->SetInput(5, image0);

        // Set the value of output pixels which are created by mismatched size input images.
        // If the two images are the same height, this will not be used.
        unsigned char fillerValue = pixelVal;
        tileFilter->SetDefaultPixelValue( fillerValue );
        tileFilter->Update();

        ImageType::ConstPointer stitchedImage = BinaryImageFilter(tileFilter->GetOutput());

        //Writing to an Output File
        QString outPath = DestinationDir;
        QString outFileName = outPath.append("/20160414_z_%1").arg(z).append(".jpg");
        QFile file(outFileName);
        if(!file.exists()){
            WriterType::Pointer writer = WriterType::New();
            writer->SetFileName(outFileName.toStdString());
            writer->SetInput(stitchedImage);
            writer->Update();
        }
    }

    return Success;
}



IEROM_NAMESPACE_END
