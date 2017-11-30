import SimpleITK as sitk
import numpy as np
from scipy import misc
from scipy import signal
from collections import Counter

NEAREST_RIGHT_EDGE = 3000
FARTHEST_LEFT_EDGE = 1000
TISSUE_WIDTH = 2420
LEFT_EDGE_REGION = 50
RIGHT_EDGE_REGION = 50
RIGHT = 0
LEFT = 1
THRESHOLD_PIX_CNT = 800

#import matplotlib.pyplot as plt

sobel3 = np.array([[-1,0,-1],
                   [-2,0,2],
                   [-1,0,1]])

sobel5 = np.array([[-1, -2, 0, 2, 1],
                   [-4, -8, 0, 8, 4],
                   [-6,-12, 0,12, 6],
                   [-4, -8, 0, 8, 4],
                   [-1, -2, 0, 2, 1]])

def find_dip(arr):
    """
    """
    inx = 0
    x,o = zip(*arr)
    for h in range(1,len(o)-1):
        print h,o[h]
        if((o[h] < o[h+1]) and (o[h] < o[h-1])):#(o[0]-o[h]>THRESHOLD_PIX_CNT) ):
            inx = h
            break
    if(inx == 0):
        inx = len(o)/2
    return x[inx]

def find_inx_from_array(in_arr,edge_pos):
    """
    """
    y,x = np.nonzero(in_arr)
    array = Counter(x).most_common() #Ouput (value,# of occurences) of each item in list
    array.sort(key=lambda x:x[0]) #Sort the tuples (x,occ) from lowest to highest x
    if(edge_pos == LEFT):
        array_pts = array[0:LEFT_EDGE_REGION] #Slice the least 50 pixels nearest to left edge
        array_pts.sort(key=lambda x:x[1]) # Sort the x index values of possible edges by occurrence
        x_inx = find_dip(array_pts)
        #x_inx,occ = array_pts[LEFT_EDGE_REGION-1] #Most occurring index is chosen
        if (x_inx > FARTHEST_LEFT_EDGE):
            return 1,0
        else:
            return 0,x_inx
    elif(edge_pos == RIGHT):
        l = len(array)
        array_pts = array[l-RIGHT_EDGE_REGION:l]
        array_pts.sort(key=lambda x:x[1])
        #x_inx,occ = array_pts[RIGHT_EDGE_REGION/2]
        #plt.scatter(*zip(*array_pts))
        #plt.show()
        if(min(array_pts) < NEAREST_RIGHT_EDGE):
            return 1,0
        x_inx = find_dip(array_pts)
        if(x_inx < NEAREST_RIGHT_EDGE):
            return 1,0
        else:
            return 0,x_inx    

def find_left_edge(scipy_img):
    """
    """
    scipy_img = scipy_img.astype('int32')
    scipy_img = signal.convolve2d(scipy_img,sobel5)
    print scipy_img.dtype
    edge_img = sitk.GetImageFromArray(scipy_img)
    edge_img = sitk.Mean(edge_img,(3,1))
    print edge_img.GetPixelIDTypeAsString()
    thresholded = sitk.MaximumEntropyThreshold(edge_img,0,255)
    sitk.Show(thresholded)
    array = sitk.GetArrayFromImage(thresholded)
    return find_inx_from_array(array,LEFT)


def crop_image(img,x_inx,edge_pos):
    """
    Edge_Pos:  0 = right
               1 = left
    """
    if(edge_pos == RIGHT):
        x_start = x_inx - TISSUE_WIDTH
        x_end = img.GetWidth() - x_inx
    elif(edge_pos == LEFT):
        x_start = x_inx
        x_end = img.GetWidth() - x_inx - TISSUE_WIDTH
    return sitk.Crop(img,(x_start,0,0),(x_end,0,0))


#MAIN
scipy_in = misc.imread('Sample1.jpg')
error,index = find_left_edge(scipy_in)
if(error):
    print "Cannot Find Edge"
    exit()
else:
    out_img = crop_image(im_in,index,LEFT)
    sitk.Show(out_img)
    sitk.WriteImage(out_img,"Sample1_out_left.jpg")

