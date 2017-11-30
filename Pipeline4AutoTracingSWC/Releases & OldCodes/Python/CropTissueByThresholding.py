import SimpleITK as sitk
import numpy as np
import os

in_img_dir="C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/Sample_Images"
out_img_dir = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/Output_Images"
images = os.listdir(in_img_dir)

for img in images:
    name,ext = img.split('.')
    print name, ext
    img_t = name+'_t.'+ext
    img_out = name+'_out.'+ext
    print img_t, img_out
    num_pixels = 2750 #This might have to be 2350-2400.
    image = sitk.ReadImage(os.path.join(in_img_dir,img))
    #sitk.Show(image)
    #print image
    #mean_image = sitk.Mean(image)
    #thresholded = sitk.LiThreshold(mean_image,0,255)
    thresholded = sitk.LiThreshold(image,0,255)
    #sitk.Show(thresholded)
    #print thresholded.GetPixelIDTypeAsString()
    stats = sitk.StatisticsImageFilter()
    stats.Execute( thresholded )
    #print stats
    image_array = sitk.GetArrayFromImage( thresholded )
    sitk.WriteImage ( thresholded, os.path.join(out_img_dir,img_t))
    x = np.nonzero(image_array)
    x_inx = max(x[1])
    y_inx = image.GetHeight() - 1
    #print x
    #print x[1][x[1].size - 1]
    #print max(x[1])
    #print x[1].shape
    #print image_array.shape
    #print image_array[1]
    #np.savetxt('test.csv', x[1], delimiter=',')
    print x_inx
    #print y_inx
    #print "%d,%d"%(x_inx,y_inx)
    x_start = x_inx - num_pixels
    x_end = image.GetWidth() - x_inx
    image_out = sitk.Crop(image,(x_start,0,0),(x_end,0,0))
    #print x_start
    #print x_inx
    #print image_out
    sitk.WriteImage ( image_out, os.path.join(out_img_dir,img_out))
