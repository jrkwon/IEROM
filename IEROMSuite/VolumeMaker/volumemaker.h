#ifndef VOLUMEMAKER_H
#define VOLUMEMAKER_H


///////////////////////////
// Qt Headers
#include <QString>
#include <QVector>
///////////////////////////
//IEROM Headers
#include "Settings.h"
///////////////////////////
// itk Headers
#include "itkImage.h"
#include "itkJPEGImageIOFactory.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExtractImageFilter.h"
#include "itkImageIterator.h"

//////////////////////////////////
/// \brief Variable Declarations
///
const int width  = 256; //each split image width
const int height = 256; //each split image height
const int depth  = 256; //number of files in a volume
//unsigned char pixelVal = 120; //background value as checked in Fiji
const unsigned int Dimension = 2;
typedef unsigned char                       PixelType;
typedef itk::Image<PixelType, Dimension>    ImageType;
typedef itk::ImageFileReader<ImageType>     ReaderType;
typedef itk::ImageFileWriter<ImageType>     WriterType;
typedef itk::ExtractImageFilter<ImageType, ImageType> FilterType;
typedef itk::ImageRegionConstIterator<ImageType> ConstIteratorType;
typedef itk::ImageRegionIterator<ImageType> IteratorType;
/////////////////////////////////
/// \brief The VolumeMaker class
///

IEROM_NAMESPACE_START
class VolumeMaker
{
public:
    //VolumeMaker(QString SourcePath, int x, int y, float z, QString DestinationPath);
    VolumeMaker(QString SourcePath, int x, int y, int z, QString DestinationPath);
    //VolumeMaker(QString SourcePath, float x, float y, int z, QString DestinationPath);
    Result CreateVolumesAndSaveIt();

private:
    ImageType::ConstPointer readFile(QString filePath);
    QString SplitImageintoSmallImages(QString imageFileName, QString zPos);
    void CreateIndexFileforZ();
    QString getZCoordinate(QString filename);
    bool checkDirforZ(QString zPos);
    int indexForZ(QString zPos);
    QString GetSelectedFileinDir(QString Directory, QString FileName);
    QString GetDirPathforZ(QString zPos);
    void CreateFillerFile(ImageType::ConstPointer image,
                           ImageType::RegionType region,
                           ImageType::SizeType size);
    void CreateZDirforfiller();

private:
    int x_sel;
    int y_sel;
    int z_sel;
    int res_w;
    int res_h;
    bool isDirCreated;
    bool isfillerCreated;
    bool isArrayFileCreated;

    QString z_lookupFilePath;
    QString fillerFile;
    int max_zVals; //number of files in the source directory
    QVector<QString> zIndexArray;

public:
    QString sourceDir;
    QString destinationDir;
    QString tempDir;



};
IEROM_NAMESPACE_END

#endif // VOLUMEMAKER_H
