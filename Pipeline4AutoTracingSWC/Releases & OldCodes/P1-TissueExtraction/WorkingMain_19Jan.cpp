//Qt Headers
#include <QCoreApplication>
#include "QDirIterator"
#include "QDebug"

//Generic C++ headers
#include <iostream>
#include <cmath>

//------------------------------------//
//  ITK-VTK-OpenCV Headers
//------------------------------------//
////IO functions
#include "itkImage.h"
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


//------------------------------//
//  Function Declarations
//------------------------------//
ImageType::Pointer extract(ImageType::Pointer input, RegionType region);
ImageType::Pointer extract_hline(int ystart, ImageType::Pointer image, RegionType::SizeType size);
ImageType::Pointer doFilter(ImageType::Pointer input, int indexNo);
unsigned int find_peak(ImageType::Pointer line,RegionType::SizeType size);
bool right_edge(ImageType::Pointer image_in, unsigned int* inx);
bool left_edge(ImageType::Pointer image_in,unsigned int* left_index);


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
    qDebug() << "Extract Filter output failed";
    return NULL;
  }
  return extractfilter->GetOutput();
}

//-------------------------------------------------------------------------------------------------//
// Function : Count #of non-zero pixels in a given region/slice of input image
//-------------------------------------------------------------------------------------------------//
unsigned int nz_pixelcnt(ImageType::Pointer input, RegionType slice){
  ImageRegionIterType it(input,slice);
  unsigned int zeros = 0;
  while(!it.IsAtEnd()){
      if(it.Get() == 0) zeros++;
      ++it;
    }
  return zeros;
}

//-------------------------------------------------------------------------------------------------//
// Function : Write name of image that failed to crop into a fail_logs file
//-------------------------------------------------------------------------------------------------//


//-----------------------//
// Main Application
//-----------------------//
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  QString inPath = "C:/Users/BI2S_STUDENT/Documents/IEROM/Winter2016/Qt/TissueExtraction/Test_Data/In";
  QString outPath = "C:/Users/BI2S_STUDENT/Documents/IEROM/Winter2016/Qt/TissueExtraction/Test_Data/Out";

  QDirIterator dir_iter(inPath,QStringList() << "*.jpg", QDir::Files);
  itk::JPEGImageIOFactory::RegisterOneFactory();

  QString next;
  QString inFileName;
  QString edge;

  while(dir_iter.hasNext()){
      next = dir_iter.next();
      inFileName = dir_iter.fileInfo().completeBaseName();

      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName(dir_iter.fileInfo().absoluteFilePath().toStdString());
      try{
        reader->Update();
      }
      catch(itk::ExceptionObject& e){
        std::cerr << e.GetDescription() << std::endl;
        return EXIT_FAILURE;
      }

      ImageType::Pointer  image_in = ImageType::New();
      image_in = reader->GetOutput();
      image_in->DisconnectPipeline();

      unsigned int inx;

      edge = "_r";
      int start_index;
      if(!right_edge(image_in,&inx)) {
          edge = "_l";
          image_in->DisconnectPipeline();
          if(!left_edge(image_in,&inx)){
              return EXIT_FAILURE;
            }
          start_index =  inx;
        }
      else {
          start_index =  inx - TissueWidth;
        }

      RegionType::SizeType full_size = image_in->GetLargestPossibleRegion().GetSize();
      RegionType::IndexType start;
      RegionType::SizeType size;
      RegionType desiredRegion;
      size[0] = TissueWidth;
      size[1] = full_size[1];
      start[0] = start_index;
      start[1] = 0;
      desiredRegion.SetIndex(start);
      desiredRegion.SetSize(size);
      image_in->DisconnectPipeline();

      ImageType::Pointer image_out = ImageType::New();
      image_out = extract(image_in,desiredRegion);

      qDebug() << image_out->GetLargestPossibleRegion().GetSize()[0];
      qDebug() << image_out->GetLargestPossibleRegion().GetSize()[1];

      image_out->DisconnectPipeline();

      QString outfile = outPath + "/" + inFileName +edge+QString::number(inx)+".jpg";

      WriterType::Pointer  writer  = WriterType::New();
      writer->SetFileName(outfile.toStdString());
      writer->SetInput(image_out);
      try
          {
          writer->Update();
          }
        catch( itk::ExceptionObject& e )
          {
          std::cerr << "Error: " << e.GetDescription() << std::endl;
          return EXIT_FAILURE;
          }

      image_in->DisconnectPipeline();
      image_out->DisconnectPipeline();

      reader = NULL;
      writer = NULL;
      image_in = NULL;
      image_out = NULL;
    }
  return a.exec();
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

  ImageType::SizeType full_size = (image_gauss->GetLargestPossibleRegion()).GetSize();
  RegionType::SizeType size;
  size[0] = full_size[0];
  size[1] = 1;
  ImageType::Pointer line1 = extract_hline(full_size[1]/4    , image_gauss, size);
  unsigned int inx1 = find_peak(line1,size);
  //qDebug() << "Inx1 = "<<inx1;
  ImageType::Pointer line2 = extract_hline(full_size[1]/2    , image_gauss, size);
  unsigned int inx2 = find_peak(line2,size);
  //qDebug() << "Inx2 = "<<inx2;
  ImageType::Pointer line3 = extract_hline(3*full_size[1]/4, image_gauss, size);
  unsigned int inx3 = find_peak(line3,size);
  //qDebug() << "Inx3 = "<<inx3;
  if((inx1>4096)||(inx2>4096)||(inx3>4096)){
      qDebug() << "find_peak method exited with error";
      return false;
    }
  *inx = int((inx1+inx2+inx3)/3);
  if( (std::abs(inx1-inx2) > 5) || (std::abs(inx2-inx3) > 5) || (std::abs(inx1-inx3) > 5) ){
      qDebug() << "Warning:: Low Confidence, indices far apart";
      return false; //Based on results in shaded unclear right edges
    }
  if(*inx < TissueWidth){
      qDebug() << "Right edge index < Tissue Width :: Trying to find left Edge...";
      return false;
    }
  //qDebug() << "Right Index Found is " << inx;
  gradientfilter = NULL;
  image_gauss = NULL;
  return true;
}

//-------------------------------------------------------------------------------------------------//
//          Function :
//-------------------------------------------------------------------------------------------------//
ImageType::Pointer extract_hline(int ystart, ImageType::Pointer image, RegionType::SizeType size){
  RegionType::IndexType start;
  start[0] = 0;
  start[1] = ystart;
  RegionType desiredRegion(start, size);
  ExtractFilterType::Pointer extractfilter = ExtractFilterType::New();
  extractfilter->SetExtractionRegion(desiredRegion);
  extractfilter->SetInput(image);
  extractfilter->SetDirectionCollapseToIdentity();
  try{
    extractfilter->Update();
  }
  catch(itk::ExceptionObject &e){
    std::cerr << e.GetDescription() << std::endl;
  }
  return extractfilter->GetOutput();
}

//-------------------------------------------------------------------------------------------------//
//          Function :
//-------------------------------------------------------------------------------------------------//
unsigned int find_peak(ImageType::Pointer line,RegionType::SizeType size){
  unsigned int inx = 0;
  RegionType::IndexType i_inx;
  RegionType region = line->GetLargestPossibleRegion();
  //qDebug() << "[0] = " << region.GetSize()[0] << " , [1] = " << size_new[1];
  i_inx[1] = region.GetIndex()[1];
  for(unsigned int i=size[0]-1 ; i > size[0]/2 ; i--) {
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
      if(next_inx[0] < size[0]/2) {
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
//          Function :
//-------------------------------------------------------------------------------------------------//
int find_lines(ImageType::Pointer inimg){
  cv::Mat img = itk::OpenCVImageBridge::ITKImageToCVMat<ImageType>(inimg);
  std::vector<cv::Vec4i> lines;
  int height = 6000;
  double rho = 1;
  while( (lines.size() == 0) && (height>0) ){
      // HoughLinesP( InputArray image, OutputArray lines,
      // double rho, double theta, int threshold,
      // double minLineLength = 0, double maxLineGap = 0 );
      cv::HoughLinesP(img,lines,rho,CV_PI/180,50,50,100);
      height = height - 1000;
    }
  int max_length = 0;
  QVector<int> lengths;
  int init_size = lines.size();
  for(int cnt=0;cnt<init_size;cnt++){
      if(lines[cnt][0] != lines[cnt][2]) {
          lines.erase(lines.begin()+cnt);
          //std::cerr << "Error: Non-vertical line found";
          //return EXIT_FAILURE;
        }
      else{
        int distance = std::abs(lines[cnt][1]-lines[cnt][3]);
        if(distance >max_length) max_length = distance;
        lengths.append(distance);
        }
    }
  QVector<int> indices;
  int inxl = 0;
  foreach(int length, lengths){
      if(length == max_length) indices.append(inxl);
      inxl++;
    }
  qDebug() <<"Number of indices found are: " << indices.size();
  int min_inx = 4000;
  foreach(int elem, indices){
      if(lines[elem][0] < min_inx) min_inx = lines[elem][0];
    }
  return min_inx;
}

//-------------------------------------------------------------------------------------------------//
//   Function : Find index of the left edge of tissue in the image if possible
//-------------------------------------------------------------------------------------------------//
bool left_edge(ImageType::Pointer image_in,unsigned int* left_index){
  ImageType::Pointer xy_out0 = doFilter(image_in, 0);
  xy_out0->DisconnectPipeline();
  ImageType::Pointer xy_out = doFilter(xy_out0, 1);
  xy_out->DisconnectPipeline();
  xy_out0 = NULL;
  ImageType::SizeType full_size = (xy_out->GetLargestPossibleRegion()).GetSize();
//  QuickView v;
//  v.AddImage<ImageType>(xy_out);
//  v.Visualize();

  RegionType::IndexType start;
  RegionType::SizeType size;
  size[0] = TissueWidth;
  size[1] = full_size[1];
  start[0] = 0;
  start[1] = 0;
  RegionType leftRegion(start,size);

  //*left_index = full_size[0] - TissueWidth + 3;

  TriangleFilterType::Pointer thresholder = TriangleFilterType::New();
  thresholder->SetInsideValue(255);//255
  thresholder->SetOutsideValue(0);//0
  thresholder->SetNumberOfHistogramBins(255);
  thresholder->SetMaskOutput(false);
  thresholder->SetInput(extract(xy_out,leftRegion));
  thresholder->Update();
  xy_out = NULL;

  ImageType::Pointer thresholded = thresholder->GetOutput();
  thresholded->DisconnectPipeline();
  QuickView v;
  v.AddImage<ImageType>(thresholded);
  v.Visualize();

  int min_inx = find_lines(thresholded);
  qDebug() << "Min_Inx from find_lines is :" << min_inx;

  ////Find - <EXTRASTEP>

  RegionType::IndexType sstart;
  RegionType::SizeType ssize;
  RegionType slice;
  ssize[0] = 1;
  ssize[1] = full_size[1];
  sstart[0] = min_inx;
  sstart[1] = 0;
  slice.SetIndex(sstart);
  slice.SetSize(ssize);

  for(int i=0;i<20;i++){
      sstart[0] = min_inx+i-1;
      slice.SetIndex(sstart);
      unsigned int cnt1 = nz_pixelcnt(thresholded,slice);
      //ImageType::Pointer slice1 = extract(threshold_out,slice);
      sstart[0] = min_inx+i;
      slice.SetIndex(sstart);
      unsigned int cnt2 = nz_pixelcnt(thresholded,slice);
      //ImageType::Pointer slice2 = extract(threshold_out,slice);
      sstart[0] = min_inx+i+1;
      slice.SetIndex(sstart);
      unsigned int cnt3 = nz_pixelcnt(thresholded,slice);
      //ImageType::Pointer slice3 = extract(threshold_out,slice);
      if(cnt1<=cnt2 && cnt3<cnt2){ //farthest line for which this is true
          *left_index = min_inx+i;
        }
    }
  if(*left_index > (full_size[0] - TissueWidth)){
      qDebug() << "Left Edge Detection Failed";
      return false;
    }
  thresholder = NULL;
  thresholded = NULL;
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
