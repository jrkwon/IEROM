/* ---------------------------------------------------------------------------
** FILENAME:
**          main.cpp
**
** DESCRIPTION:
**            Normalization & Selective Normalization
**            Implementation of Method proposed by Jaerock Kwon (ISBI 2011)
**            for normalization of KESM images.
**
** AUTHOR:
**        Shruthi Raghavan
**
** Modified By/On: Shruthi Raghavan/06152016
** -------------------------------------------------------------------------*/
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <iostream>
#include "itkImage.h"
#include "itkJPEGImageIOFactory.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageLinearConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "QuickView.h"

//#define DEBUG_ON
#define BKGND_INTENSITY    120
#define MAX_INTENSITY        255
#define ERROR_THRESHOLD 70
#define FOREGND_FACTOR   0.4


int main(int argc, char *argv[])
{
  //Qt Core Application Stuff
  QCoreApplication a(argc, argv);

  qDebug()<< ">>> Tissue Region Extaction Engine (TREE) for KESM Images <<<" << endl;
  qDebug()<< ">>>     Designed and Implemented by Shruthi Raghavan      <<<" << endl;

  //ITK typedefs
  typedef unsigned char                                                                  PixelType;
  const    unsigned int                                                                     Dimension = 2;
  typedef itk::Image <PixelType, Dimension>                                 ImageType;
  typedef itk::ImageFileReader <ImageType>                                 ReaderType;
  typedef itk::ImageFileWriter <ImageType>                                  WriterType;
  typedef itk::ImageLinearConstIteratorWithIndex<ImageType>    ConstIteratorType;
  typedef itk::ImageLinearIteratorWithIndex<ImageType>             IteratorType;

  //If IO modules are not registered during CMAKE build, we need to register them here
  itk::JPEGImageIOFactory::RegisterOneFactory();

  //Qt Variables
  QStringList                        filters;
  QString                              inPath  = \
      "C:/BI2STools/Data/MouseBrainVasculature08_CroppedImages/00005_crop";
  QString                              outPath  = \
      "C:/BI2STools/Data/MouseBrainVasculature08_Normalized/00005";
  QDirIterator                      dir_iter ( inPath, filters << "*.jpg", QDir::Files);
  QString                              tmp_fpath; // Holds file path during directory iteration
  QString                              outfilename,infilename;
  QStringList                        dummylist;
  QVector<PixelType>          pixel_list; //Dynamic Array of pixels from line read

  //ITK Variables
  ReaderType::Pointer         reader     =  ReaderType::New();
  WriterType::Pointer          writer      = WriterType::New();
  ImageType::Pointer          InImage    = ImageType::New();
  ImageType::Pointer          OutImage = ImageType::New();
  ImageType::Pointer          TmpImage = ImageType::New();
  ImageType::SizeType        size;
  //Other Variables
  unsigned int                     direction=0;
  PixelType                          lineMedian;
  PixelType                          outPixel;
  PixelType                          curr_pixel;
  #ifdef DEBUG_ON
  QuickView viewer;
  #endif

  //Main Loop over all the files in input path given
  while (dir_iter.hasNext())
    {
      tmp_fpath = dir_iter.next();
      /*Code blocks reads in a JPEG File*/
      qDebug() << "Reading... " << tmp_fpath;
      dummylist = tmp_fpath.split("/");
      infilename = dummylist.last();
      outfilename = outPath +"/" + (infilename.split(".jpg")[0]).append("_norm.jpg");
      reader->SetFileName(tmp_fpath.toStdString());
      InImage =  reader->GetOutput();
      try {
      reader->Update();
      }
      catch(itk::ExceptionObject& e) {
        std::cerr << e.GetDescription() << std::endl;
        return EXIT_FAILURE;
      }
      qDebug() << "Read!";

      //qDebug() << "Will write to" << outfilename;

      /*Code Block Extracts region information for ITK iterator*/
      size = InImage->GetLargestPossibleRegion().GetSize();
      std::cout << size << std::endl;

      // After reading the input image, we allocate an output image that of the same
      // size, spacing, and origin.
      OutImage->SetRegions( InImage->GetLargestPossibleRegion() ); //Set Size of image
      OutImage->CopyInformation( InImage ); //Copy meta information from input image
      OutImage->Allocate(); //

      //Temp image
      TmpImage->SetRegions( InImage->GetLargestPossibleRegion() ); //Set Size of image
      TmpImage->CopyInformation( InImage ); //Copy meta information from input image
      TmpImage->Allocate();


      // Next we create the two iterators.  Const iterator walks the input image & non-const
      //iterator walks the output image. The iterators are initialized over the same region.
      ConstIteratorType inputIt  ( InImage   , InImage->GetLargestPossibleRegion() );
      IteratorType          tempIt  ( OutImage, InImage->GetLargestPossibleRegion() );
      IteratorType          outputIt( OutImage, InImage->GetLargestPossibleRegion() );



      //-----------------------------------------------------------------------------------------------------------------------//
      //----------------------------------------------------------------------------------------------------------------------//
        direction = 1;
        //The direction of iteration is set to rows
        inputIt.SetDirection(direction);
        tempIt.SetDirection(direction);

        //Loop to process each line
        for(inputIt.GoToBegin(), tempIt.GoToBegin() ; !inputIt.IsAtEnd(); tempIt.NextLine(), inputIt.NextLine()) {
            /*Set input and output images to beginning of current line*/
           // qDebug() << "Inside the for loop! If you see this, maybe it is okay...";
            inputIt.GoToBeginOfLine();
            tempIt.GoToBeginOfLine();
            while( !inputIt.IsAtEndOfLine()) {
                pixel_list.append(inputIt.Get());
                ++inputIt;
              }
            inputIt.GoToBeginOfLine();
            /*Calculate the scale factot for current row*/
            qSort(pixel_list.begin(), pixel_list.end());
            lineMedian = pixel_list[pixel_list.size()/2];

            /*If median intensity < threshold, keep foreground, scale background*/
            if(lineMedian < (double)ERROR_THRESHOLD) {
            while(!tempIt.IsAtEndOfLine()) {
                curr_pixel =  inputIt.Get();
                if( curr_pixel < (lineMedian*FOREGND_FACTOR))
                  outPixel = curr_pixel;
                else
                  outPixel =  (PixelType)((curr_pixel/(double)lineMedian)*BKGND_INTENSITY);
               // if(outPixel > (double)MAX_INTENSITY) outPixel = (PixelType)MAX_INTENSITY;
                tempIt.Set(outPixel);
                ++inputIt;
                ++tempIt;
                }
            }
            else { //If not, scale all pixels in row
                while(!tempIt.IsAtEndOfLine()) {
                    curr_pixel = inputIt.Get();
                    outPixel =  (PixelType)((curr_pixel/(double)lineMedian)*BKGND_INTENSITY);
                    if(outPixel > (double)MAX_INTENSITY) outPixel = (PixelType)MAX_INTENSITY;
                    tempIt.Set(outPixel);
                    ++tempIt;
                    ++inputIt;
                }
            }
            pixel_list.clear(); //Removes all the elements from the vector and releases the memory used by the vector.
            //for(int i=0;i<pixel_list.size();i++) delete[] &(pixel_list.at(i));
        } //End row column function for loop
        //-----------------------------------------------------------------------------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------------------//

        //-----------------------------------------------------------------------------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------------------//
        direction = 0;
        //The direction of iteration is set to columns
        tempIt.SetDirection(direction);
        outputIt.SetDirection(direction);

        //Loop to process each line
        for(tempIt.GoToBegin(), outputIt.GoToBegin() ; !tempIt.IsAtEnd(); outputIt.NextLine(), tempIt.NextLine()) {
            /*Set input and output images to beginning of current line*/
           // qDebug() << "Inside the for loop! If you see this, maybe it is okay...";
            tempIt.GoToBeginOfLine();
            outputIt.GoToBeginOfLine();
            while( !tempIt.IsAtEndOfLine()) {
                pixel_list.append(tempIt.Get());
                ++tempIt;
              }
            tempIt.GoToBeginOfLine();
            /*Calculate the scale factot for current row*/
            qSort(pixel_list.begin(), pixel_list.end());
            lineMedian = pixel_list[pixel_list.size()/2];

            /*If median intensity < threshold, keep foreground, scale background*/
            if(lineMedian < (double)ERROR_THRESHOLD) {
            while(!outputIt.IsAtEndOfLine()) {
                curr_pixel =  tempIt.Get();
                if( curr_pixel < (lineMedian*FOREGND_FACTOR))
                  outPixel = curr_pixel;
                else
                  outPixel =  (PixelType)((curr_pixel/(double)lineMedian)*BKGND_INTENSITY);
                if(outPixel > (double)MAX_INTENSITY) outPixel = (PixelType)MAX_INTENSITY;
                outputIt.Set(outPixel);
                ++tempIt;
                ++outputIt;
                }
            }
            else { //If not, scale all pixels in row
                while(!outputIt.IsAtEndOfLine()) {
                    curr_pixel = tempIt.Get();
                    outPixel =  (PixelType)((curr_pixel/(double)lineMedian)*BKGND_INTENSITY);
                   // if(outPixel > (double)MAX_INTENSITY) outPixel = (PixelType)MAX_INTENSITY;
                    outputIt.Set(outPixel);
                    ++outputIt;
                    ++tempIt;
                }
            }
            pixel_list.clear(); //Removes all the elements from the vector and releases the memory used by the vector.
            //for(int i=0;i<pixel_list.size();i++) delete[] &(pixel_list.at(i));
        } //End row column function for loop
        //-----------------------------------------------------------------------------------------------------------------------//
        //----------------------------------------------------------------------------------------------------------------------//

      //Write back into output file here
      writer->SetFileName(outfilename.toStdString());
      writer->SetInput(OutImage);

      try
          {
          writer->Update();
          }
        catch( itk::ExceptionObject& e )
          {
          std::cerr << "Error: " << e.GetDescription() << std::endl;
          return EXIT_FAILURE;
          }

      #ifdef DEBUG_ON
      viewer.AddImage<ImageType>(reader->GetOutput()); //GetOutput());
      viewer.Visualize();
      std::cin >> dummy;
      #endif
    } //End while loop for QDir

  return a.exec();
} //End Main
