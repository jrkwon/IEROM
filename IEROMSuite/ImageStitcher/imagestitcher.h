#ifndef IMAGESTITCHER_H
#define IMAGESTITCHER_H

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
#include "itkInvertIntensityImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageIterator.h"
#include "itkFlipImageFilter.h"
#include "itkTileImageFilter.h"

const unsigned int Dimension  = 2;
const unsigned char pixelVal  = 0;
/****************************************
 *
 *        **********
 *        *        *
 *        *        *
 *        *        *
 *        *        *
 * ********        *********
 * Outside  Inside  Outside
 * */

const unsigned int Maximum    = 255;
const unsigned int insideVal  = 255;
const unsigned int outsideVal = 0;
const unsigned int lowerThreshold = 0;
const unsigned int upperThreshold = 70;
/*****************************************/

typedef unsigned char                       PixelType;
typedef itk::Image<PixelType, Dimension>    ImageType;
typedef itk::ImageFileReader<ImageType>     ReaderType;
typedef itk::ImageFileWriter<ImageType>     WriterType;
typedef itk::InvertIntensityImageFilter<ImageType> InvertIntensityFilterType;
typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> BinaryThresholdFilterType;
typedef itk::FlipImageFilter<ImageType>     FlipImageFilterType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
typedef itk::ImageRegionIterator<ImageType> IteratorType;
typedef itk::TileImageFilter< ImageType, ImageType > TileFilterType;


IEROM_NAMESPACE_START
class ImageStitcher
{
public:
    ImageStitcher(QString SourceDirectory);
    Result StitchImagesAndSaveIt();

private:
    ImageType::ConstPointer readFile(QString filePath);
    ImageType::ConstPointer InverseIntensity(QString dirpath);
    ImageType::ConstPointer BinaryImageFilter(ImageType::ConstPointer image);
    ImageType::ConstPointer FlipImagetoLeft(ImageType::ConstPointer image);

    QString getFileforSameZCoordinate(QString col_path, QString z);
    QString CreateEmptyImage();
    QString getZCoordinate(QString file);


private:
    QString SourceDir;
    QString DestinationDir;
    QString col0_dirPath;
    QString col1_dirPath;
    QString col2_dirPath;
    QString col3_dirPath;
    QString col4_dirPath;
    QString col5_dirPath;
    QString filler_dirPath;
    QString fillerFile;

};
IEROM_NAMESPACE_END

#endif // IMAGESTITCHER_H
