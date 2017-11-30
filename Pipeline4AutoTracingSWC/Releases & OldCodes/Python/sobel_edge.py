import numpy
import scipy
from scipy import ndimage
num_pixels = 2750
im = scipy.misc.imread('Sample3.jpg')
im = im.astype('int32')
dx = ndimage.sobel(im, 0)  # vertical derivative

scipy.misc.imsave('sobel3.jpg', dx)

import SimpleITK as sitk
im_in = sitk.ReadImage("Sample3.jpg")
image = sitk.ReadImage("sobel3.jpg")
thresholded = sitk.MaximumEntropyThreshold(image,0,255)
mean_image = sitk.Median(thresholded,(5,5)) #Default Radius is (3,1)
sitk.WriteImage ( thresholded, "sobel_t_mean3.jpg")


##sobel_array = sitk.GetArrayFromImage( mean_image )
##image_array = sitk.GetArrayFromImage( im_in )
##x = numpy.nonzero(sobel_array)
##x_inx = min(x[1])
##y_inx = im_in.GetHeight() - 1
##x_start = x_inx - num_pixels
##x_end = image.GetWidth() - x_inx - num_pixels
##image_out = sitk.Crop(im_in,(x_inx,0,0),(x_end,0,0))
##sitk.WriteImage ( image_out,'Sample3_out.jpg')
