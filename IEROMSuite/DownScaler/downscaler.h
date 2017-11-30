#ifndef DOWNSCALER_H
#define DOWNSCALER_H


///////////////////////////
// Qt Headers
#include <QString>
///////////////////////////
//IEROM Headers
#include "Settings.h"
///////////////////////////
// itk Headers
#include "itkImage.h"
#include "itkJPEGImageIOFactory.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkShrinkImageFilter.h"


const unsigned int Dimension = 2;

typedef unsigned char                       PixelType;
typedef itk::Image<PixelType, Dimension>    ImageType;
typedef itk::ImageFileReader<ImageType>     ReaderType;
typedef itk::ImageFileWriter<ImageType>     WriterType;
typedef itk::ShrinkImageFilter <ImageType, ImageType>       ShrinkImageFilterType;


IEROM_NAMESPACE_START

class DownScaler
{
public:
    DownScaler(QString InputFile,  QString OutputFile);
    Result shrinkImageAndSaveIt();


private:
    ImageType::ConstPointer readFile(QString filePath);
    ImageType::ConstPointer shrinkImage(ImageType::ConstPointer imageFile);
    void writeFile(ImageType::ConstPointer outImgFile, QString outFileName);

private:
    int shrinkfactor;
    QString InputImgFile;
    QString ShrinkedImgFile;

};
IEROM_NAMESPACE_END

#endif // DOWNSCALER_H
