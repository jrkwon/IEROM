#include "volumemaker.h"
#include <QDirIterator>


IEROM_NAMESPACE_START

VolumeMaker::VolumeMaker(QString SourcePath, int x, int y, int z, QString DestinationPath)
{
    this->sourceDir = SourcePath;
    this->destinationDir = DestinationPath;
    this->x_sel = x;
    this->y_sel = y;
    this->z_sel = z;

    QDir directory(this->sourceDir);
    this->max_zVals = directory.count() - 2;
    QStringList dirName = directory.dirName().split("_");
    QStringList resolution = dirName.at(1).split("x");
    this->res_w = resolution.at(0).toInt();
    this->res_h = resolution.at(1).toInt();

    QDir destDir(this->destinationDir);
    if(destDir.cdUp())
    {
        this->fillerFile = destDir.absolutePath()+ QString("/filler_%1x%2.jpg").arg(res_w).arg(res_h);
        this->z_lookupFilePath = destDir.absolutePath()+ QString("/z_index_%1x%2.txt").arg(res_w).arg(res_h);
        QString temp_Zdir = destDir.absolutePath()+QString("/Z_Directories");
        QDir tempdir(temp_Zdir);
        if(!tempdir.exists()) tempdir.mkdir(".");

    this->tempDir = tempdir.absolutePath();
    }

    this->isDirCreated = true;
    QFile file(fillerFile);
    this->isfillerCreated = file.exists() ? true : false;
    QFile textfile(z_lookupFilePath);
    this->isArrayFileCreated = textfile.exists() ? true : false;
}



///Itk function to read an image file
///\return the image file pointer
///
ImageType::ConstPointer VolumeMaker::readFile(QString filePath)
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


//////////////////////////////////////////////////////////////////////////////////
///Takes a file Name as input and the corresponding Z position
///\return the directory path which contains images each of 256*256 dimension
///The name of the directory corresponds to the Z value
///Each file in the directory is named as xPos_yPos
//TODO: filler image for the last col and row using mode value from the image
//
QString VolumeMaker::SplitImageintoSmallImages(QString imageFileName, QString zPos)
{
    QString retZdirPath;
    //Read an Image
    ImageType::ConstPointer image = readFile(imageFileName);
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();

    int w = size.GetSize()[0]; //width of the input image
    int h = size.GetSize()[1]; //height of the input image

    float nW = (float)w/(float)width; int w_int = (int)nW; float w_dec = nW - w_int;
    float nH = (float)h/(float)height;int h_int = (int)nH; float h_dec = nH - h_int;

    int nW_div = w_int + (w_dec > 0.05f ? 1 : 0); //total divisions of width
    int nH_div = h_int + (h_dec > 0.05f ? 1 : 0); //total divisions of height

    int x=0;
    int y=0;
    ImageType::IndexType desiredStart;
    ImageType::SizeType desiredSize;

    for(x=0; x <w_int; ++x){
        for(y=0; y <h_int; ++y){
            int start_x = x*width;
            int start_y = y*height;
            desiredStart[0] = start_x;
            desiredStart[1] = start_y;

            desiredSize[0] = width;
            desiredSize[1] = height;
            ImageType::RegionType desiredRegion(desiredStart,desiredSize);

            FilterType::Pointer filter = FilterType::New();
            filter->SetExtractionRegion(desiredRegion);
            filter->SetInput(image);
            filter->SetDirectionCollapseToIdentity();
            filter->Update();


            if(x==0 && y==0)
            {
                QString newZDir = tempDir + QString("/z%1").arg(zPos);
                QDir dir(newZDir);
                if(!dir.exists()) dir.mkdir(".");
                retZdirPath = dir.absolutePath();
            }

            QString outFileName = retZdirPath + QString("/x%1_y%2").arg(x).arg(y)+ QString(".jpg");
            WriterType::Pointer writer = WriterType::New();
            writer->SetFileName(outFileName.toStdString());
            writer->SetInput(filter->GetOutput());
            writer->Update();
        }
    }

    return retZdirPath;
}

///Create Filler files in the Source directory to complete the last volume layer
/// \return the file path
///
void VolumeMaker::CreateFillerFile(ImageType::ConstPointer image, ImageType::RegionType region, ImageType::SizeType size)
{
    ImageType::Pointer emptyImage = ImageType::New();
    emptyImage->SetRegions(region);
    emptyImage->Allocate();

    ConstIteratorType in(image, region);
    IteratorType out(emptyImage,region);
    in.GoToBegin();
    out.GoToBegin();

    while(!in.IsAtEnd()){
        out.Set(0); //background black
        ++in; ++out;
    }

    //save the image
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(fillerFile.toStdString());
    writer->SetInput(emptyImage);
    writer->Update();
    isfillerCreated = true;
}

///Create Z dir for the filler files : called once during the whole execution
/// \return z directory path
///
void VolumeMaker::CreateZDirforfiller()
{
    int d_int = ((int)max_zVals/(int)depth);
    float d_dec = ((float)max_zVals/(float)depth) - d_int;

    int nAvailfiles = d_dec*depth;
    float zIndex = 0.0f;
    int i = (d_int*depth)+ nAvailfiles;

    float lastFile  = zIndexArray.at((d_int*depth)+ nAvailfiles-1).toFloat();

    if(!isfillerCreated){
        QString imageFileName = GetSelectedFileinDir(sourceDir, zIndexArray.at(0));
        ImageType::ConstPointer image = readFile(imageFileName);
        ImageType::RegionType region = image->GetLargestPossibleRegion();
        ImageType::SizeType size = region.GetSize();
        CreateFillerFile(image, region, size);
    }

    //update the length and values of the array
    max_zVals = max_zVals + (depth - nAvailfiles);
    for(zIndex = (lastFile+0.0010f); i<max_zVals; zIndex+=0.0010f,++i){
        QDir dir(tempDir +QDir::separator()+QString("z%1").arg(QString::number(zIndex, 'f', 4)));
        if(!dir.exists())
           SplitImageintoSmallImages(fillerFile, QString::number(zIndex, 'f', 4));
        zIndexArray.insert(i, QString::number(zIndex, 'f', 4));
    }
}

//use this function only if source directory name includes more than z information
///Gets the z value from the name of the file
///\return z value
///
QString VolumeMaker::getZCoordinate(QString filename)
{
    QStringList myStringList = filename.split('z');
    QString coordinates = myStringList.at(1);//.split('z');
    return coordinates;
}


///Initializes an array which contains all the Z values
///\return the absolute file path of the text file created for Z index referencing
///
void VolumeMaker::CreateIndexFileforZ()
{
    QDirIterator diriter(sourceDir,QStringList() << "*.jpg", QDir::Files);
    QString next;
    QString inFileName;
    int index = 0;

    while(diriter.hasNext()){
        next = diriter.next();
        inFileName = diriter.fileInfo().completeBaseName();
        QString z = getZCoordinate(inFileName);
        //QString z = inFileName.remove(QChar('z'), Qt::CaseInsensitive);
        zIndexArray.insert(index, z);
        ++index;
    }

    //Create the filler file, create the Z directories for the filler images and then append the Z array
    CreateZDirforfiller();

    if(!isArrayFileCreated){
        QFile file(z_lookupFilePath);
        QFileInfo fileInfo(file.fileName());
        if (file.open(QFile::WriteOnly|QFile::Text))
        {
            QTextStream out(&file);
            int i=0;
            for(i=0; i<index; ++i)
              out << i <<" - " <<zIndexArray.at(i) <<"\r\n";

        }
        isArrayFileCreated = true;
        file.close();
    }
}


///Get the image file from the directory
///\return file path for selected file
///
QString VolumeMaker::GetSelectedFileinDir(QString Directory, QString FileName)
{
    QDirIterator dir_iter(Directory, QStringList()<<"*jpg", QDir::Files);
    QString next;
    QString inFileName;

    while(dir_iter.hasNext()){
        next = dir_iter.next();
        inFileName = dir_iter.fileInfo().completeBaseName();
        if(inFileName.contains(FileName))
        {
           return dir_iter.fileInfo().absoluteFilePath();
        }
        else
           continue;
    }
    return NULL;
}

///check the tempDir("Z_Directories") if a directory for the selected z (ex:"z_1.3530") exists
///\return true: if directory exists
///\return false: if directory does not exist
///
bool VolumeMaker::checkDirforZ(QString zPos)
{
    QDir zdir(tempDir);
    QString str = zdir.absolutePath();
    QDirIterator zdir_it(str,QStringList()<< "*",
                         QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    QString next;
    QString zDirName;
    while(zdir_it.hasNext()){
        next = zdir_it.next();
        zDirName = zdir_it.filePath();
        if(zDirName.contains(zPos))
            return true;
        else
            continue;
    }
    return false;
}

///checks if the directory already exists
///\return directory path if exists
///
QString VolumeMaker::GetDirPathforZ(QString zPos)
{
    QDir zdir(tempDir);
    QString str = zdir.absolutePath();
    QDirIterator zdir_it(str,QStringList()<< "*",
                         QDir::AllDirs|QDir::NoDotAndDotDot|QDir::NoSymLinks);
    QString next;
    QString zDirName;
    while(zdir_it.hasNext()){
        next = zdir_it.next();
        zDirName = zdir_it.filePath();
        if(zDirName.contains(zPos))
            return zDirName;
        else
            continue;
    }
    return NULL;
}

///looks for local (int)z index according to the selected (float)z value
///\return index
///
int VolumeMaker::indexForZ(QString zPos)
{
    int i = 0;
    for(i=0; i<max_zVals; ++i){
        if(zIndexArray.at(i).contains(zPos))
            return i;
        else continue;
    }
    return -1;
}



Result VolumeMaker::CreateVolumesAndSaveIt()
{
    QString zDirPath = NULL;
    int uX = (int)x_sel;
    int uY = (int)y_sel;
    int uZ = (int)z_sel;

    //allowed z values
    int minZValSel = 0;
    int maxZValSel = max_zVals;
    int zIndexStart = uZ*256;//indexForZ(zVal);

    //Initialize the z index array
    //Create a z index look up table and save it in a text file
    //return that text file or array in which all z values are saved
    CreateIndexFileforZ();

    //create volumes only when selected z value is in the range of minZValSel and maxZValSel
    if(zIndexStart >= minZValSel && zIndexStart <= maxZValSel)
    {
        //Create Volumes since boundary condition is satisfied
        int i = 0;

        QString xyFilePath = NULL;
        for(i=zIndexStart; i<zIndexStart+depth; ++i)
        {
            //check if a subdirectory for the selected z value exists
            //if it does not exist then make a directory
            //create directories for all z required to make the volume
            if(!checkDirforZ(zIndexArray.at(i)))
            {
                QString zfilePath = GetSelectedFileinDir(sourceDir, zIndexArray.at(i));
                zDirPath  = SplitImageintoSmallImages(zfilePath, zIndexArray.at(i));
            }

            else
                zDirPath = GetDirPathforZ(zIndexArray.at(i));

            //now we have the directory required to make the volume of interest
            //search for the same file name in each z directory
            //Destination directory should have files starting from xsel_ysel_zsel.jpg
            //
            QString xyfilePath = QString("x%1_y%2").arg(QString::number(uX)).arg(QString::number(uY));

            //get the corresponding x_y file from each z_dir
            xyFilePath = GetSelectedFileinDir(zDirPath, xyfilePath);
            QFile file(xyFilePath);
            QString chk = file.fileName();

            //Initialize the destination volume directory
            //Create one if not created and then start putting the files
            QDir destDir(destinationDir);
            QString volDir = QString("Vol_%1_%2_%3").arg(QString::number(uX)).arg(QString::number(uY)).arg(QString::number(uZ));

            if(!destDir.cd(volDir)){
                destDir.mkdir(volDir);
                isDirCreated = true;
            }
            if(destDir.cd(volDir) || isDirCreated){
                QString destfilePath = destDir.absolutePath() + QString("/z%1").arg(zIndexArray.at(i))+QString(".jpg");
                if(chk != "")
                    file.rename(destfilePath);//cut-paste the file from z_directory to the unit volume directory
            }


            //if a directory has zero files, then delete the directory
            QDir tempZDir(zDirPath);
            if(!(tempZDir.count()-2))
                tempZDir.removeRecursively();

        }

    }

    return Success;
}


IEROM_NAMESPACE_END
