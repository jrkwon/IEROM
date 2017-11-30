//-------------------------------------------------------------------------------------------------//
//          Function :
//-------------------------------------------------------------------------------------------------//
unsigned int find_edge(ImageType::Pointer line){
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
  while(curr_pixel < next_pixel){
      if(next_inx[0] < 0) {
          qDebug() << "Could not find the peak on the right half";
          return 4097;
        }
      curr_pixel = line->GetPixel(next_inx);
      next_inx[0]--;
      next_pixel = line->GetPixel(next_inx);
    }
  QVector<int> list;
  while(curr_pixel == next_pixel){
      list.append(next_inx[0]+1);
      if(next_inx[0] < 0) {
          qDebug() << "Could not find the peak on the right half";
          return 4097;
        }
      curr_pixel = line->GetPixel(next_inx);
      next_inx[0]--;
      next_pixel = line->GetPixel(next_inx);
    }
  if(list.isEmpty()) inx = ++next_inx[0];
  else inx = list[list.size()/2];
//    {
//      if(list.size()%2==0) inx = list[list.size()/2];
//      else inx = list[list.size()/2];
//    }
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
      cv::HoughLinesP(img,lines,rho,CV_PI,height/8,height/4,100);
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
