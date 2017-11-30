import numpy as N
import scipy.ndimage as I
import matplotlib.image as IM
import matplotlib.pyplot as plt

image_in = scipy.misc.imread('Sample3.jpg')

def hough_transform(img_bin, theta_res=1, rho_res=1):
