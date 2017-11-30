#ifndef MODEL3DMAKER_H
#define MODEL3DMAKER_H

/////////////////////////////////////
// Qt Headers
#include <QString>
#include <QVector>
/////////////////////////////////////
//IEROM Headers
#include "Settings.h"
/////////////////////////////////////
// itk Headers
#include "itkImage.h"
#include "itkJPEGImageIO.h"
#include "itkTIFFImageIO.h"
#include "itkJPEGImageIOFactory.h"
#include "itkTIFFImageIOFactory.h"
#include "itkImageSeriesReader.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
//////////////////////////////////////////////
// vtk Headers
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingOpenGL);

#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkSTLWriter.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataNormals.h>


///////////////////////////////////////////////
/// Variable Initialization
///
const unsigned int g_nDimension = 3;

typedef unsigned char PixelType;
typedef itk::Image<PixelType, g_nDimension> ImageType;
typedef itk::ImageSeriesReader<ImageType> ReaderType;
typedef itk::NumericSeriesFileNames NameGeneratorType;
typedef itk::ImageFileWriter< ImageType > WriterType;
typedef itk::TIFFImageIO                  TIFFIOType;
typedef itk::ImageToVTKImageFilter<ImageType> FilterType;
typedef itk::InvertIntensityImageFilter<ImageType, ImageType> InvertIntensityFilterType;


///////////////////////////////////////////////////
/// \brief The Model3DMaker class
///
///
IEROM_NAMESPACE_START

enum ModelFormat {STL = 0, TIFF = 1, VTK = 2};

class Model3DMaker
{

public:
    Model3DMaker(QString SourceDirPath, QString DestinationDirPath);
    Result CreateStlMeshes();
    Result CreateMultiPageTiffs();
    Result CreateVtkFiles();

private:
    void convertFileNames();
    ImageType::Pointer readImageFiles();

private:
    QString volLabel;
    QString sourceDir;
    QString destinationDir;
};
IEROM_NAMESPACE_END
#endif // MODEL3DMAKER_H
