import SimpleITK as sitk
import numpy as np
from scipy import misc
from scipy.signal import medfilt,convolve2d
from scipy.ndimage.filters import gaussian_filter,gaussian_filter1d
from skimage.feature import corner_harris, corner_peaks
from collections import Counter
import os
import matplotlib.pyplot as plt
from scipy.ndimage.morphology import binary_erosion

#Define Data Specific Parameters
NEAREST_RIGHT_EDGE = 3000
FARTHEST_LEFT_EDGE = 1000
TISSUE_WIDTH = 2400
RIGHT = 0
LEFT = 1

sobel5 = np.array([[-1, -2, 0, 2, 1],
                   [-4, -8, 0, 8, 4],
                   [-6,-12, 0,12, 6],
                   [-4, -8, 0, 8, 4],
                   [-1, -2, 0, 2, 1]])

#Function Definitions
#def find_right_edge1(img):
    #"""
    #Function takes an input image in sitk format
    #Returns (error,pixel_position)
    #error = 0: pixel_position is right edge
    #error = 1: ignore pixel_position - right edge not found
    #"""
    #timg = sitk.LiThreshold(img,0,255)
    ##sitk.WriteImage(timg,"thresholded.jpg")
    #array  = sitk.GetArrayFromImage(timg)
    #array1,array0 = np.nonzero(array)
    #array = Counter(array0).most_common()
    #array.sort(key=lambda x:x[0],reverse=True) #Sort the tuples (x,occ) from lowest to highest x
    #array_new = array[0:50]
    #x,o = zip(*array_new)
    #inx = np.argmax(o)
    #array = array[inx:inx+40]
    #x,o = zip(*array)
    #inx = 0
    #for h in range(1,len(o)-1):
        #if((o[h] < o[h+1]) and (o[h] < o[h-1])): #(o[0]-o[h]>THRESHOLD_PIX_CNT) ):
            #inx = h
            #print(h) 
            #break
    #if(inx == 0):
        #for h in range(1,len(o)-1):
            #if(o[h] < o[h+1]):
                #inx = h
                #print(h)
                #break
    ##print(inx)
    ##print(x[inx])
    #return x[inx]

def find_right_edge(scipy_img):
    gaussian_filter(scipy_img, 2, 1, scipy_img, mode="reflect")
    #edge_img = sitk.GetImageFromArray(scipy_img)
    #sitk.Show(edge_img)    
    y,x = np.nonzero(scipy_img)
    x = x[np.logical_not(np.isnan(x))]
    x0 = Counter(x).most_common()
    #print(x0)    
    x0.sort(key=lambda x0:x0[0],reverse=True)
    x,o = zip(*x0[0:10])
    inx = np.argmax(o)
    #for h in range(1,len(o)-1):
        #if((o[h] > o[h+1]) and (o[h] > o[h-1])):#(o[0]-o[h]>THRESHOLD_PIX_CNT) ):
            #inx = h
            #print(h)
            #break
    #if(inx == 0):
        #for h in range(1,len(o)-1):
            #if(o[h] > o[h+1]):
                #inx = h
                #print(h)
                #break
    print(inx)
    print(x[inx])
    return x[inx]

def find_left_edge(scipy_img):
    """
    """
    scipy_img.astype('int32')
    scipy_img = convolve2d(scipy_img,sobel5)
    #gaussian_filter1d(scipy_img, 1, axis=1, order=1, output=scipy_img, mode="reflect")
    #gaussian_filter1d(scipy_img, 1, axis=0, order=1, output=scipy_img, mode="reflect")
    scipy_img.astype('uint8')
    edge_img = sitk.GetImageFromArray(scipy_img)
    edge_img = sitk.Mean(edge_img,(5,1))
    thresholded = sitk.MaximumEntropyThreshold(edge_img,0,255)
    #edge_img = sitk.BinaryErode(thresholded,(3,3),2,0,255)
    edge_img = sitk.Median(thresholded,(9,1))
    #sitk.Show(edge_img)
    array = sitk.GetArrayFromImage(edge_img)
    array1,array0 = np.nonzero(array)
    array = Counter(array0).most_common()    
    array.sort(key=lambda x:x[0]) #Sort the tuples (x,occ) from lowest to highest x
    array_new = array[0:50]
    x,o = zip(*array)
    n = [x[i] for i in range(len(x)) if x[i]>875]
    print(n)
    print(x.index(n[0]))
    slice_s = x.index(n[0])
    x,o = zip(*array[slice_s:slice_s+50])
    #inx = np.argmax(o)
    #array = array[inx:inx+50]
    #x,o = zip(*array)
    s_inx = np.argmax(o)
    #x = x[inx:]
    #o = o[inx:]
    for h in range(s_inx+int(s_inx==0),len(o)-1):
        if((o[h] > o[h+1]) and (o[h] > o[h-1])):#(o[0]-o[h]>THRESHOLD_PIX_CNT) ):
            inx = h
            print(h)
            break
    #if(inx == 0):
        #for h in range(1,len(o)-1):
            #if(o[h] < o[h+1]):
                #inx = h
                #print(h)
                #break    
    #plt.plot(x,o,'ro')
    #plt.show()
    print(inx)
    print(x[inx])
    return x[inx]

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
    return sitk.Crop(img,(int(x_start),0,0),(int(x_end),0,0))


#TrialRuns
in_img_dir="C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/Sample_Images_New"
out_img_dir = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/Output_Images_New"
#in_img_dir="C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/failed"
#out_img_dir = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/test1_out"
images = os.listdir(in_img_dir)


for img in images:
    try:
        #print(img)
        name,dummy = img.split('.jpg')
        im_in = sitk.ReadImage(os.path.join(in_img_dir,img))
        img_array = sitk.GetArrayFromImage(im_in)
        index = find_right_edge(img_array)
        brightness = (np.mean(img_array[5000:7000,index-100:index]))
        print(brightness)
        if(brightness < 0.1 or index<3300):
            #img_array = sitk.GetArrayFromImage(im_in)
            index = find_left_edge(img_array)
            img_left = name+'_l_inx'+str(index)+'.jpg'
            out_img = crop_image(im_in,index,LEFT)
            #sitk.Show(out_img)
            sitk.WriteImage(out_img,os.path.join(out_img_dir,img_left))
        else:
            img_right = name+'_r_inx'+str(index)+'.jpg'
            out_img = crop_image(im_in,index,RIGHT)
            #sitk.Show(out_img)
            sitk.WriteImage(out_img,os.path.join(out_img_dir,img_right))
    except:
        img_fail = name+'_fail.jpg'
        sitk.WriteImage(im_in,os.path.join(out_img_dir,img_fail))

#im_in = sitk.ReadImage("Sample5.jpg")
#index = find_right_edge(im_in)
#if(index < 3000):
    #img_array = GetArrayFromImage(im_in)
    #index = find_left_edge(scipy_in)
    #if(index > 1200):
        #print("Cannot Find Edge")
        #exit()
    #else:
        #out_img = crop_image(im_in,index,LEFT)
        #sitk.Show(out_img)
        #sitk.WriteImage(out_img,"Sample5_out_left.jpg")
#else:
    #out_img = crop_image(im_in,index,RIGHT)
    #sitk.Show(out_img)
    #sitk.WriteImage(out_img,"Sample5_out_right.jpg")
