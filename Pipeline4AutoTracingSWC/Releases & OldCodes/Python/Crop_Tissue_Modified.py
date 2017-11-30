import SimpleITK as sitk
import numpy as np
from scipy import misc
from scipy.signal import medfilt,convolve2d
from scipy.ndimage.filters import gaussian_filter,gaussian_filter1d,sobel 
from collections import Counter
import os
import matplotlib.pyplot as plt
from scipy.ndimage.morphology import binary_erosion
from skimage import transform
from math import pi

#Define Data Specific Parameters
NEAREST_RIGHT_EDGE = 3000
FARTHEST_LEFT_EDGE = 1000
TISSUE_WIDTH = 2400
RIGHT = 0
LEFT = 1
angles = np.array([0.0])

sobel5 = np.array([[-1, -2, 0, 2, 1],
                   [-4, -8, 0, 8, 4],
                   [-6,-12, 0,12, 6],
                   [-4, -8, 0, 8, 4],
                   [-1, -2, 0, 2, 1]])

sobelm = np.array([[-1, 0, 1],
                   [-1, 0, 1],
                   [-1, 0, 1],
                   [-1, 0, 1],
                   [-1, 0, 1],
                   [-1, 0, 1],
                   [-1, 0, 1]])

#Function Definitions
def find_right_edge(scipy_img):
    gaussian_filter(scipy_img, 2, 1, scipy_img, mode="reflect")
    #edge_img = sitk.GetImageFromArray(scipy_img)
    #sitk.Show(edge_img)
    y,x = np.nonzero(scipy_img)
    if(x.size==0): return 0
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

def find_right_li(img):
    thresholded = sitk.LiThreshold(img,0,255)
    arr = sitk.GetArrayFromImage(thresholded)
    y,x = np.nonzero(arr)
    x = np.array(x[np.logical_not(np.isnan(x))]).tolist()
    x.sort(reverse=True)
    print(x[0])
    return x[0]

def find_left_edge(scipy_img):
    """
    """
    scipy_img.astype('int32')
    #sobel(scipy_img, axis=0, output=scipy_img)
    #sobel(scipy_img, axis=1, output=scipy_img)
    gaussian_filter1d(scipy_img, 1, axis=0, order=1, output=scipy_img, mode="reflect")
    gaussian_filter1d(scipy_img, 1, axis=1, order=1, output=scipy_img, mode="reflect")
    scipy_img.astype('uint8')
    edge_img = sitk.GetImageFromArray(scipy_img)
    edge_img = sitk.TriangleThreshold(edge_img,0,255)
    #edge_img = sitk.Median(edge_img,(5,1))
    edge_img = sitk.BinaryErode(edge_img,(3,2),2,0,255)
    #sitk.Show(edge_img)
    img = sitk.GetArrayFromImage(edge_img)
    lines = transform.probabilistic_hough_line(img[:,:2048],0.5,6000,500,angles)
    height = 6000
    while(lines == []):
        if(height < 0): break
        else:
            print(lines)
            height = height - 1000
            lines = transform.probabilistic_hough_line(img[:,:2048],0.5,height,500,angles)
    if(lines == []):
        print("Failed to find any edge")
        return -2000
    print(lines)
    l0,l1 = zip(*lines)
    l0 = list(l0)
    l1 = list(l1)
    l00,l01 = zip(*l0)
    l10,l11 = zip(*l1)
    l01 = list(l01)
    l11 = list(l11)
    d = [a-b for a,b in zip(l01,l11)]
    inx = l00[np.argmax(d)]
    img_slice = img[:,inx:inx+30]
    m = 0
    for i in range(20):
        sl1 = img[:,inx+i-1]
        sl2 = img[:,inx+i]
        sl3 = img[:,inx+i+1]
        c1 = (sl1 != 0).sum(0)
        c2 = (sl2 != 0).sum(0)
        c3 = (sl3 != 0).sum(0)
        if c1>c2 and c3>c2:
            m = inx+i
            break
    #array = sitk.GetArrayFromImage(edge_img)
    #array1,array0 = np.nonzero(array)
    #array = Counter(array0).most_common()    
    #array.sort(key=lambda x:x[0]) #Sort the tuples (x,occ) from lowest to highest x
    #array_new = array[0:50]
    #x,o = zip(*array)
    #n = [x[i] for i in range(len(x)) if x[i]>875]
    #print(n)
    #print(x.index(n[0]))
    #slice_s = x.index(n[0])
    #x,o = zip(*array[slice_s:slice_s+50])
    ##inx = np.argmax(o)
    ##array = array[inx:inx+50]
    ##x,o = zip(*array)
    #s_inx = np.argmax(o)
    ##x = x[inx:]
    ##o = o[inx:]
    #for h in range(s_inx+int(s_inx==0),len(o)-1):
        #if((o[h] > o[h+1]) and (o[h] > o[h-1])):#(o[0]-o[h]>THRESHOLD_PIX_CNT) ):
            #inx = h
            #print(h)
            #break
    ##if(inx == 0):
        ##for h in range(1,len(o)-1):
            ##if(o[h] < o[h+1]):
                ##inx = h
                ##print(h)
                ##break    
    ##plt.plot(x,o,'ro')
    ##plt.show()
    print(m)
    return m

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
#in_img_dir="C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/abc"
#out_img_dir = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Python/Output_Images"

base_path_in  = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Images/2008-01-23 Whole mouse brain vasculature/"
base_path_out = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Images/Outputs/"
in_dirs = os.listdir(base_path_in)
for dir in in_dirs:
    in_img_dir = os.path.join(base_path_in,dir)
    out_img_dir = os.path.join(base_path_out,dir+"_crop")
    if not os.path.exists(out_img_dir):
        os.makedirs(out_img_dir)
    if dir == '00005':
        print(dir)
        images = os.listdir(in_img_dir)
        for img in images:
            print(img)
            try:
                l_index=0
                r_index=0
                high = 0
                dark = 0
                name,dummy = img.split('.jpg')
                im_in = sitk.ReadImage(os.path.join(in_img_dir,img))
                img_array = sitk.GetArrayFromImage(im_in)
                r_index = find_right_edge(img_array)
                if r_index > 2400:
                    dark = (img_array[5800:6200,r_index-50:r_index] < 10).sum()
                    high = np.max(img_array[5800:6200,r_index-50:r_index])
                    #brightness = np.mean(img_array[5800:6200,r_index-50:r_index])
                    #print(brightness)
                if (high<30 and dark>19000) or r_index < 2400:
                    #img_array = sitk.GetArrayFromImage(im_in)
                    l_index = find_left_edge(img_array)
                    if(l_index != -2000 and l_index<1200):
                        img_left = name+'_l_inx'+str(l_index)+'.jpg'
                        out_img = crop_image(im_in,l_index,LEFT)
                        #sitk.Show(out_img)
                        sitk.WriteImage(out_img,os.path.join(out_img_dir,img_left))
                    else:
                        new_inx = find_right_li(im_in)
                        out_img = crop_image(im_in,new_inx,RIGHT)
                        #sitk.Show(out_img)
                        img_right = name+'_rl_inx'+str(new_inx)+'.jpg'
                        sitk.WriteImage(out_img,os.path.join(out_img_dir,img_right))
                else:
                    img_right = name+'_r_inx'+str(r_index)+'.jpg'
                    out_path = os.path.join(out_img_dir,img_right)
                    out_img = crop_image(im_in,r_index,RIGHT)
                    #sitk.Show(out_img)
                    sitk.WriteImage(out_img,out_path)
            except:
                img_fail = name+'_fail.jpg'
                sitk.WriteImage(im_in,os.path.join(out_img_dir,img_fail))