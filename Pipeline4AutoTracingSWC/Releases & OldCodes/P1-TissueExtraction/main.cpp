//Qt Headers
#include <QCoreApplication>
#include "QDirIterator"
#include "QDebug"

//Generic C++ headers
#include <iostream>
#include <cstdlib>

//------------------------------------//
//  ITK-VTK-OpenCV Headers
//------------------------------------//
////IO functions
#include "itkImage.h"
#include "itkImageRegion.h"
#include "itkIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkImageDuplicator.h"
#include "itkJPEGImageIOFactory.h"

////Filters
#include "itkExtractImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkSobelEdgeDetectionImageFilter.h"
////Computes the Magnitude of the Gradient of an image by convolution with the first derivative of a Gaussian.
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkHistogram.h"
#include "itkGradientRecursiveGaussianImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
//#include "itkVectorMagnitudeImageFilter.h"
#include "itkTriangleThresholdImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageLinearConstIteratorWithIndex.h"

////VTK & Glue Functions
#include "QuickView.h"

////OpenCV & Glue Functions
#include "itkOpenCVImageBridge.h"
#include "cv.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"


//Defines
#define TissueWidth 2400

//----------------------------------//
//            Typedefs                //
//---------------------------------//

//Data Structures
const unsigned int                                       Dimension = 2;
typedef unsigned char                                 PixelType;

// Double type for GradientRecursiveGaussianImageFilter
typedef double                                                                           DoublePixelType;
typedef itk::Image<PixelType,Dimension>                                 ImageType;
typedef itk::Image< DoublePixelType, Dimension >                  DoubleImageType;
typedef itk::ImageRegion<Dimension>                                     RegionType;
typedef itk::CovariantVector< DoublePixelType, Dimension >  CovPixelType;
typedef itk::Image< CovPixelType, Dimension >                       CovImageType;

// Allows to select the X or Y output images
typedef itk::VectorIndexSelectionCastImageFilter< CovImageType, DoubleImageType > IndexSelectionType;

//IO and Creation
typedef itk::ImageFileReader<ImageType>   ReaderType;
typedef itk::ImageFileWriter<ImageType>    WriterType;

//Filters
typedef itk::ExtractImageFilter<ImageType,ImageType> ExtractFilterType;
typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<ImageType,ImageType> GaussianGradientFilterType;
typedef itk::GradientRecursiveGaussianImageFilter< ImageType, CovImageType > VFilterType;
typedef itk::RescaleIntensityImageFilter< DoubleImageType, ImageType > RescalerType;
typedef itk::TriangleThresholdImageFilter<ImageType, ImageType > TriangleFilterType;
typedef itk::FlatStructuringElement<Dimension> StructuringElementType;
typedef itk::BinaryErodeImageFilter <ImageType, ImageType, StructuringElementType> BinaryErodeImageFilterType;

//Iterators
typedef itk::ImageLinearConstIteratorWithIndex<ImageType>    ConstIteratorType;
typedef itk::ImageLinearIteratorWithIndex<ImageType>             IteratorType;
typedef itk::ImageRegionConstIterator<ImageType> ImageRegionIterType;
typedef itk::ImageLinearConstIteratorWithIndex<ImageType> ConstLineIt;


//------------------------------//
//  Function Declarations
//------------------------------//
ImageType::Pointer extract(ImageType::Pointer input, RegionType region);
ImageType::Pointer doFilter(ImageType::Pointer input, int indexNo);
unsigned int find_peak(ImageType::Pointer line);
unsigned int find_edge(ImageType::Pointer line);
QVector<int> find_devs(ImageType::Pointer image);
bool right_edge(ImageType::Pointer image_in, unsigned int* inx);
bool left_edge(ImageType::Pointer image_in,unsigned int* left_index);

//Inline Fucntion to fiund absolute difference
inline int absdiff(int x, int y){
  if(x>y) return x-y;
  else return y-x;
}


//------------------------------------------------------------------------------------------------------//
// Function : Extract and return a requested "<Region>" from the "<Input>" image
//------------------------------------------------------------------------------------------------------//
ImageType::Pointer extract(ImageType::Pointer input, RegionType region){

  ExtractFilterType::Pointer extractfilter = ExtractFilterType::New();
  extractfilter->SetExtractionRegion(region);
  extractfilter->SetInput(input);
  extractfilter->SetDirectionCollapseToIdentity();
  try{
    extractfilter->Update();
  }
  catch(itk::ExceptionObject &e){
    std::cerr << e.GetDescription() << std::endl;
    //qDebug() << "Extract Filter output failed";
    return NULL;
  }
  return extractfilter->GetOutput();
}

//-----------------------//
// Main Application
//-----------------------//
int main(int argc, char *argv[])
{
  //QCoreApplication a(argc, argv);

  //QString inPath = "C:/Users/BI2S_STUDENT/Documents/IEROM/Winter2016/Qt/TissueExtraction/Test_Data/in3";
  QString inPath = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Images/WholeMouseBrainVasculature2008-01-23/00000";
  QString outPath = "C:/Users/BI2S_STUDENT/Documents/IEROM/Winter2016/Qt/TissueExtraction/Test_Data/Out";

  QDirIterator dir_iter(inPath,QStringList() << "*.jpg", QDir::Files);
  itk::JPEGImageIOFactory::RegisterOneFactory();

  QString next;
  QString inFileName;
  QString edge;

  QString filename = outPath + "/finalresult_00000.csv";
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
      //qDebug() << "Could not open the file for writing";
      return false;
    }
  QTextStream out(&file);

  while(dir_iter.hasNext()){
      next = dir_iter.next();
      inFileName = dir_iter.fileInfo().completeBaseName();
      out << inFileName;
      out <<",";

      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(dir_iter.fileInfo().absoluteFilePath().toStdString());
      try{
        reader->Update();
      }
      catch(itk::ExceptionObject& e){
        std::cerr << e.GetDescription() << std::endl;
        out << "ReaderFail" << endl;
        file.close();
        return EXIT_FAILURE;
      }

      ImageType::Pointer  image_in = ImageType::New();
      image_in = reader->GetOutput();
      image_in->DisconnectPipeline();

      unsigned int inx = 0;
      edge ="right";
      if(!right_edge(image_in,&inx)) {
          edge = "left";
          ImageType::SizeType full_size = (image_in->GetLargestPossibleRegion()).GetSize();
          RegionType::IndexType start;
          RegionType::SizeType size;
          RegionType leftRegion(start,size);
          size[0] = full_size[0] - TissueWidth;
          start[0] = 0;
          size[1] = full_size[1];
          start[1] = 0;
          leftRegion.SetIndex(start);
          leftRegion.SetSize(size);
          ImageType::Pointer left_in = extract(image_in,leftRegion);
          if(!left_edge(left_in,&inx)){
              out << "LR_EdgeFail" << endl;
              file.close();
              return EXIT_FAILURE;
            }
        }
      else inx = inx - TissueWidth;

      out << edge << ",";
      out << inx << endl;

      reader = NULL;
      image_in = NULL;
    }
  file.close();
  qDebug() << "Program Finished!";
  return EXIT_SUCCESS;
  //return a.exec();
}


//-------------------------------------------------------------------------------------------------//
//   Function : Find index of the right edge of tissue in the image if possible
//-------------------------------------------------------------------------------------------------//
bool right_edge(ImageType::Pointer image_in, unsigned int* inx){
 // ImageType::Pointer image_gauss = ImageType::New();
  GaussianGradientFilterType::Pointer gradientfilter = GaussianGradientFilterType::New();

  gradientfilter->SetInput(image_in);
  try{
    gradientfilter->Update();
  }
  catch(itk::ExceptionObject& e){
    std::cout << " Exception Object Caught:: " << e.GetDescription() << std::endl;
    qDebug() << "Gradient Filter output from find right edge failed";
    return false;
  }
  ImageType::Pointer image_gauss = gradientfilter->GetOutput();
  image_gauss->DisconnectPipeline();

//  QuickView v;
//  v.AddImage<ImageType>(image_gauss);
//  v.Visualize();

//  WriterType::Pointer writer = WriterType::New();
//  writer->SetInput(image_gauss);
//  writer->SetFileName("C:/Users/BI2S_STUDENT/Documents/IEROM/Winter2016/Qt/TissueExtraction/Test_Data/Out/gauss_right_out.jpg");
//  writer->Update();

  RegionType region = (image_gauss->GetLargestPossibleRegion());
  RegionType::IndexType start;
  RegionType::SizeType size;
  start[0] = region.GetIndex()[0];
  int full_height = region.GetSize()[1]/4;
  size[0] = region.GetSize()[0];
  size[1] = 1;
  region.SetSize(size);

  start[1] = full_height/4;
  region.SetIndex(start);
  ImageType::Pointer line1 = extract(image_gauss,region);
  unsigned int inx1 = find_peak(line1);
//  qDebug() << "Inx1 = "<<inx1;

  start[1] = 2*full_height/4;
  region.SetIndex(start);
  ImageType::Pointer line2 =  extract(image_gauss,region);
  unsigned int inx2 = find_peak(line2);
//  qDebug() << "Inx2 = "<<inx2;

  start[1] =3* full_height/4;
  region.SetIndex(start);
  ImageType::Pointer line3 =  extract(image_gauss,region);
  unsigned int inx3 =  find_peak(line3);
//  qDebug() << "Inx3 = "<<inx3;

  if((inx1>4096)||(inx2>4096)||(inx3>4096)){
      //qDebug() << "find_peak method exited with error";
      return false;
    }
  //*inx = int((inx1+inx2+inx3)/3); // See outliers fixed below
  unsigned int diff1 = absdiff(inx1,inx2);
  unsigned int diff2 = absdiff(inx2,inx3);
  unsigned int diff3 = absdiff(inx3,inx1);
//  qDebug() << diff1;
//  qDebug() << diff2;
//  qDebug() << diff3;
  if(diff1>10 || diff2>10 || diff3>10){
      //qDebug() << "Warning:: Low Confidence, indices far apart";
      return false; //Based on results in shaded unclear right edges
    }
  else if( diff1<4 || diff2<4 || diff3<4 ){
      //Assume one outlier is okay. (Maybe artifacts?Tissue?)
      if(diff1 >4 && diff2>4) *inx = (inx1+inx3)/2;
      else if(diff1 >4 && diff3>4) *inx = (inx2+inx3)/2;
      else if(diff2>4 && diff3>4) *inx = (inx1+inx2)/2;
      else *inx =(inx1+inx2+inx3)/3;
    }
  else{
      //qDebug() << "Warning:: Low Confidence, indices far apart";
      return false; //Based on results in shaded unclear right edges
    }
  if(*inx < TissueWidth){
      //qDebug() << "Warning:: Low Confidence, indices far apart";
      return false; //Based on results in shaded unclear right edges
    }
  //qDebug() << "Right Index Found is " << inx;
  gradientfilter = NULL;
  image_gauss = NULL;
  return true;
}

//-------------------------------------------------------------------------------------------------//
//          Function :
//-------------------------------------------------------------------------------------------------//
unsigned int find_peak(ImageType::Pointer line){
  unsigned int inx = 0;
  RegionType::IndexType i_inx;
  RegionType region = line->GetLargestPossibleRegion();
  //qDebug() << "[0] = " << region.GetSize()[0] << " , [1] = " << size_new[1];
  i_inx[1] = region.GetIndex()[1];
  RegionType::SizeType size = region.GetSize();
  for(unsigned int i=size[0]-1 ; i >0 ; i--) {
      i_inx[0] = i;
      if(line->GetPixel(i_inx) != 0){
          inx = i;
          break;
        }
    }

  if(inx == 0){
      qDebug() << "No Pixel set in right half of gradient image";
      return 4097 ;
    }

  PixelType curr_pixel = line->GetPixel(i_inx);
  RegionType::IndexType next_inx;
  next_inx[1] = i_inx[1];
  next_inx[0] = i_inx[0] -1;
  PixelType next_pixel = line->GetPixel(next_inx);
  while(curr_pixel <= next_pixel){
      if(next_inx[0] < 0) {
          qDebug() << "Could not find the peak on the right half";
          return 4097;
        }
      curr_pixel = line->GetPixel(next_inx);
      next_inx[0]--;
      next_pixel = line->GetPixel(next_inx);
    }
  inx = ++next_inx[0];
  return inx;
}

//-------------------------------------------------------------------------------------------------//
//             Function: Find normalized sum of pixel values
//-------------------------------------------------------------------------------------------------//
QVector<int> find_devs(ImageType::Pointer image){
  QVector<int> ret;
  ConstLineIt it( image, image->GetRequestedRegion() );
  it.SetDirection(1); //Iterate over columns
  it.GoToBegin();
  int sum = 0;
  int norm_sum = 0;
  int dev = 0;
  while( !it.IsAtEnd() )
    {
      while( !it.IsAtEndOfLine() )
        {
          sum += it.Get();  // it.Set() doesn't exist in the Const Iterator
          ++it;
        }
      norm_sum = sum/12000;
      it.GoToBeginOfLine();
      while( !it.IsAtEndOfLine() ){
          dev += std::abs(it.Get()-norm_sum);
          ++it;
        }
      ret.append(dev);
      dev = 0;
      sum = 0;
      it.NextLine();
    }
  return ret;
}

//-------------------------------------------------------------------------------------------------//
//   Function : Find index of the left edge of tissue in the image if possible
//-------------------------------------------------------------------------------------------------//
bool left_edge(ImageType::Pointer left_in,unsigned int* left_index){
  ImageType::Pointer xy_out0 = doFilter(left_in, 0);
  xy_out0->DisconnectPipeline();
  ImageType::Pointer xy_out = doFilter(xy_out0, 1);
  xy_out->DisconnectPipeline();
  xy_out0 = NULL;

  QVector<int> devs = find_devs(xy_out);
//  QString filename = "values.txt";
//  QFile file(filename);
//  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
//      qDebug() << "Could not open the file for writing";
//      return false;
//    }
//  QTextStream out(&file);
//  QVectorIterator<int> it(devs);
//  while (it.hasNext()){
//        out << it.next() << endl;
//    }
//  file.close();

  *left_index = devs.indexOf(*std::max_element(devs.begin(), devs.end()));
  return true;
}

//-------------------------------------------------------------------------------------------------//
//          Function :
//-------------------------------------------------------------------------------------------------//
ImageType::Pointer doFilter(ImageType::Pointer input, int indexNo)
{
  VFilterType::Pointer filter = VFilterType::New();
  filter->SetInput( input);

  IndexSelectionType::Pointer indexSelectionFilter = IndexSelectionType::New();
  indexSelectionFilter->SetIndex(indexNo);
  indexSelectionFilter->SetInput( filter->GetOutput() );
  //indexSelectionFilter1->Update();

  //Rescale for PixelType Image Output
  RescalerType::Pointer rescaler = RescalerType::New();
  rescaler->SetOutputMinimum( itk::NumericTraits< PixelType >::min() );
  rescaler->SetOutputMaximum( itk::NumericTraits< PixelType >::max() );
  rescaler->SetInput( indexSelectionFilter->GetOutput() );
  rescaler->Update();
  filter = NULL;
  indexSelectionFilter = NULL;

  //ImageType::Pointer out = rescaler->GetOutput();
  return rescaler->GetOutput();
}

////EXTRASTEP
///Following Step seems to be unnecessary without any effect in this case. Check later cases.
/*{
StructuringElementType::RadiusType elementRadius;
elementRadius[0] = 3;
elementRadius[1] = 2;
StructuringElementType structuringElement = StructuringElementType::Box(elementRadius);
BinaryErodeImageFilterType::Pointer erodeFilter = BinaryErodeImageFilterType::New();
erodeFilter->SetInput(thresholded);
erodeFilter->SetKernel(structuringElement);
erodeFilter->SetErodeValue(0); //Value in image to consider as foreground
}*/
