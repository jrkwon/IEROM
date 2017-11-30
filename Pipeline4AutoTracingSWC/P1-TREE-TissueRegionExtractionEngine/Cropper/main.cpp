#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "itkImage.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkJPEGImageIOFactory.h"

#define TissueWidth 2400

const unsigned int Dimension =2;
typedef unsigned char PixelType;
typedef itk::Image<PixelType,Dimension> ImageType;
typedef itk::ImageRegion<Dimension> RegionType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::ExtractImageFilter<ImageType,ImageType> ExtractFilterType;

static ImageType::Pointer extract(const ImageType::Pointer image, int index);
static ImageType::Pointer image_reader(QString filepath);
static bool image_writer(const ImageType::Pointer image, QString outpath);

int main(int argc, char *argv[])
{
  const QString in_path = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Images/WholeMouseBrainVasculature2008-01-23/00000";
  const QString out_path = "E:/CroppedVasculature/00000";

  itk::JPEGImageIOFactory::RegisterOneFactory();

  const QString csv_file = "E:/CropIndices/CropIndices_00000.csv";
  QFile file(csv_file);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      qDebug() << "Could not open the file for reading";
      return false;
    }
  QTextStream in(&file);

  QString line;
  QStringList list;
  QString in_filename;
  QString out_filename;
   do
     {
       line = in.readLine();
       list = line.split(',');
       in_filename = in_path + "/" +list[0]+".jpg";
       out_filename = out_path + "/" +list[0]+"_"+list[1]+".jpg";

       ImageType::Pointer in_image = image_reader(in_filename);
       if(in_image.IsNull()) {
           qDebug() << in_filename << " COULD NOT BE READ" << endl;
           return EXIT_FAILURE;
         }

       ImageType::Pointer cropped_image = extract(in_image,list[1].toInt());
       if(cropped_image.IsNull()) {
           qDebug() << "Failed to extract image at x_inx:" << list[1] << "from the following file: ";
           qDebug() << in_filename << endl;
           return EXIT_FAILURE;
         }

       bool write_done = image_writer(cropped_image,out_filename);
       if(write_done == false) {
           qDebug() << "Failed to write the extracted image at: ";
           qDebug() << out_filename << endl;
           return EXIT_FAILURE;
         }
     } while (!line.isNull());

  file.close();
  qDebug() << "Program Finished!";
  return EXIT_SUCCESS;
}

static ImageType::Pointer extract(const ImageType::Pointer image, int index)
{
  RegionType region = image->GetLargestPossibleRegion();
  RegionType::IndexType start;
  RegionType::SizeType size;
  start[0] = index;
  start[1] = region.GetIndex()[1];
  size[0] = TissueWidth;
  size[1] = region.GetSize()[1];
  region.SetSize(size);
  region.SetIndex(start);

  ExtractFilterType::Pointer extractfilter = ExtractFilterType::New();
  extractfilter->SetExtractionRegion(region);
  extractfilter->SetInput(image);
  extractfilter->SetDirectionCollapseToIdentity();
  try{
    extractfilter->Update();
  }
  catch(itk::ExceptionObject &e){
    std::cerr << e.GetDescription() << std::endl;
    qDebug() << "Extract Filter Update failed";
    return NULL;
  }
  return extractfilter->GetOutput();
}

static ImageType::Pointer image_reader(QString filepath)
{
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filepath.toStdString());
  try{
    reader->Update();
  }
  catch(itk::ExceptionObject &e){
    qDebug() << "Error while reading file " << filepath <<endl;
    qDebug() << e.GetDescription();
    return NULL;
  }
  return reader->GetOutput();
}

static bool  image_writer(const ImageType::Pointer image, QString filepath)
{
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(filepath.toStdString());
  writer->SetInput(image);
  try
  {
    writer->Update();
  }
  catch(itk::ExceptionObject &e){
    qDebug() << "Error while writing file " << filepath <<endl;
    qDebug() << e.GetDescription();
    return false;
  }
  return true;
}
