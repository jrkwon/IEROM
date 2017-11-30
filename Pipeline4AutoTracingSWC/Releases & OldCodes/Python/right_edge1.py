from scipy import signal
from scipy import misc
import numpy as np
import SimpleITK as sitk

sobel = np.array([[-1,0,1],
                  [-2,0,2],
                  [-1,0,1]])
TISSUE_WIDTH = 2420
in_img = sitk.ReadImage('Sample6.jpg') #Read in as sitk image
t_img = sitk.LiThreshold(in_img,0,255)
#sitk.Show(t_img)
t_img_arr = sitk.GetArrayFromImage(t_img)
y_nz,x_nz = np.nonzero(t_img_arr)
x_inx = max(x_nz)
print x_inx

x_end = in_img.GetWidth() - x_inx
x_start = x_inx - TISSUE_WIDTH
print x_end,x_start
image_out = sitk.Crop(in_img,(x_start,0,0),(x_end,0,0))
sitk.Show(image_out)
