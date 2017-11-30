#include "downscaler.h"

IEROM_NAMESPACE_START

DownScaler::DownScaler(QString InputFileName, QString OutputFileName)
{
    this->shrinkfactor = 2;
    this->InputImgFile = InputFileName;
    this->ShrinkedImgFile = OutputFileName;
}

ImageType::ConstPointer DownScaler::readFile(QString filePath)
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

ImageType::ConstPointer DownScaler::shrinkImage(ImageType::ConstPointer imageFile)
{
    itk::JPEGImageIOFactory::RegisterOneFactory();

    ShrinkImageFilterType::Pointer shrinkFilter = ShrinkImageFilterType::New();
    shrinkFilter->SetInput(imageFile);
    //shrink the first and second dimensions by a factor of 2 i.e. 50% reduction
    shrinkFilter->SetShrinkFactors(shrinkfactor);
    shrinkFilter->Update();

    return shrinkFilter->GetOutput();
}

void DownScaler::writeFile(ImageType::ConstPointer outImgFile, QString outFilePath)
{
    itk::JPEGImageIOFactory::RegisterOneFactory();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outFilePath.toStdString());
    writer->SetInput(outImgFile);
    writer->Update();
}

Result DownScaler::shrinkImageAndSaveIt(){

    itk::JPEGImageIOFactory::RegisterOneFactory();

    ImageType::ConstPointer Image = readFile(InputImgFile);
    ImageType::ConstPointer ShrinkedImage = shrinkImage(Image);
    writeFile(ShrinkedImage, ShrinkedImgFile);
    return Success;

}
IEROM_NAMESPACE_END
