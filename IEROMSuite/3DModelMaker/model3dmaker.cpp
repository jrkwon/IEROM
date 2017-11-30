#include "model3dmaker.h"
#include <QDirIterator>

IEROM_NAMESPACE_START
Model3DMaker::Model3DMaker(QString SourceDirPath, QString DestinationDirPath)
{
    QDir s_dir(SourceDirPath);
    QDir d_dir(DestinationDirPath);
    this->sourceDir = s_dir.absolutePath();
    this->volLabel  = s_dir.dirName();
    this->destinationDir = d_dir.absolutePath();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Rename the files in series from 0 to 255
///
///
void Model3DMaker::convertFileNames()
{
    QDirIterator dirit(sourceDir, QStringList() << "*.jpg", QDir::Files);
    QString next;

    int i = 0;
    while(dirit.hasNext()){
        next = dirit.next();
        QString filename = dirit.filePath();
        QFile file(filename);
        file.rename(dirit.fileInfo().absolutePath() + QString("/%1.jpg").arg(QString::number(i)));
        ++i;
    }
}

///Itk function to read image files in a directory
///\return the image file pointer
///
ImageType::Pointer Model3DMaker::readImageFiles()
{
    unsigned int nStartIndex = 0;
    unsigned int nEndIndex   = 255;
    unsigned int nIncrements = 1;

    itk::JPEGImageIOFactory::RegisterOneFactory();
    QString strSeriesFormat = sourceDir + QString("/%d.jpg");

    NameGeneratorType::Pointer pNameGenerator = NameGeneratorType::New();
    pNameGenerator->SetSeriesFormat(strSeriesFormat.toStdString());
    pNameGenerator->SetStartIndex(nStartIndex);
    pNameGenerator->SetEndIndex(nEndIndex);
    pNameGenerator->SetIncrementIndex(nIncrements);

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileNames(pNameGenerator->GetFileNames());
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

Result Model3DMaker::CreateStlMeshes()
{
    //int nIsoSurface = 100;  //9600x12000
    int nIsoSurface = 120;  //4800x6000
    //int nIsoSurface = 130;  //2400x3000
    //int nIsoSurface = 150;  //1200x1500
    //int nIsoSurface = 160;  //600x750
    //int nIsoSurface = 180;   //300x375

    convertFileNames();

    vtkMarchingCubes   *m_pTissueExtractor = vtkMarchingCubes::New();
    vtkPolyDataNormals *m_pTissueNormals   = vtkPolyDataNormals::New();
    vtkPolyDataMapper  *m_pTissueMapper    = vtkPolyDataMapper::New();
    vtkActor           *m_pTissue          = vtkActor::New();

    ImageType::Pointer m_pImageData = readImageFiles();

    double space[] = {1,1,1}; //{0.625, 0.7, 1};
    m_pImageData->SetSpacing(space);

    // execute pipeline
//    InvertIntensityFilterType::Pointer m_pInvertIntensityFilter = InvertIntensityFilterType::New();
//    m_pInvertIntensityFilter->SetInput(m_pImageData);

    FilterType::Pointer m_pConnector = FilterType::New();
    m_pConnector->SetInput(m_pImageData);//(m_pInvertIntensityFilter->GetOutput());
    #if VTK_MAJOR_VERSION <= 5
    m_pTissueExtractor->SetInput(m_pConnector->GetOutput());
    #else
    m_pConnector->Update();
    m_pTissueExtractor->SetInputData(m_pConnector->GetOutput());
    #endif

    //for completely clean data... we should use nIsoSurface value as 230 because more scaler value sets will be created without any loss of data
    //since, the data currently is not completely clean so we keep the nIsoSurface value minimum to avoid noise in the image
    m_pTissueExtractor->SetValue(0, nIsoSurface);

    #if VTK_MAJOR_VERSION <= 5
    m_pTissueNormals->SetInput(m_pTissueExtractor->GetOutput());
    #else
    m_pTissueNormals->SetInputConnection(m_pTissueExtractor->GetOutputPort());
    #endif
    //m_pTissueNormals->SetFeatureAngle(60.0);
    #if VTK_MAJOR_VERSION <= 5
    m_pTissueMapper->SetInput(m_pTissueNormals->GetOutput());
    #else
    m_pTissueMapper->SetInputConnection(m_pTissueNormals->GetOutputPort());
    #endif
    m_pTissueMapper->ScalarVisibilityOff();
    m_pTissue->SetMapper(m_pTissueMapper);


    // save to stl
    vtkSTLWriter* pSTLWriter = vtkSTLWriter::New();
    #if VTK_MAJOR_VERSION <= 5
    pSTLWriter->SetInput(m_pTissueNormals->GetOutput());
    #else
    pSTLWriter->SetInputConnection(m_pTissueNormals->GetOutputPort());
    #endif

    QString strSTLFileName = destinationDir + QString("/%1.stl").arg(volLabel);
    pSTLWriter->SetFileName(strSTLFileName.toStdString().c_str());
    pSTLWriter->SetFileTypeToBinary();
    pSTLWriter->Write();

    return Success;
}

Result Model3DMaker::CreateMultiPageTiffs()
{
    itk::JPEGImageIOFactory::RegisterOneFactory();
    itk::TIFFImageIOFactory::RegisterOneFactory();

    convertFileNames();

    TIFFIOType::Pointer tiffIO = TIFFIOType::New();
    tiffIO->SetPixelType(itk::ImageIOBase::SCALAR);

    QString outputFilename = destinationDir + QString("/%1.tiff").arg(volLabel);
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputFilename.toStdString());
    writer->SetInput(readImageFiles());
    writer->SetImageIO(tiffIO);
    writer->Update();

    return Success;
}

Result Model3DMaker::CreateVtkFiles()
{

    return Success;
}

IEROM_NAMESPACE_END
