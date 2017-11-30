import SimpleITK as sitk
from scipy import misc
from scipy.fftpack import fft2, ifft2

img_in = misc.imread('20080225_125807_x148.0967y26.6286z5.2580_t0.001000_v20.4534_r_inx3320.jpg')
x = fft2(img_in)
y= ifft2(x)
print(x)
print(y)
print("ALL dONNE")