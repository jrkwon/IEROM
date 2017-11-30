__author__ = 'Shruthi Raghavan'
desired_intensity = 150.0
threshold = 50.0
foreground_fact = 0.8
#Packages Used
import numpy as np
from scipy import misc
import os

#Constants
in_path = "C:/Users/BI2S_STUDENT/Desktop" #"C:/Users/BI2S_STUDENT/Documents/IEROM/Summer2015/cropped2/"
out_path = "C:/Users/BI2S_STUDENT/Desktop" #"C:/Users/BI2S_STUDENT/Documents/IEROM/Summer2015/norm_trial1/"
images = ["out_r_inx3194.jpg"] #os.listdir(in_path)

for img in images:
    name, dummy = img.split('.jpg')
    #Input Image Read
    print('reading... ', img)
    in_arr = misc.imread(os.path.join(in_path, img))
    img_out = name+"_norm.jpg"
    N_row = np.zeros_like(in_arr)
    N_col = np.zeros_like(in_arr)
    nrows = in_arr.shape[0]
    ncols = in_arr.shape[1]
    print(nrows)
    print(ncols)
    print('read!')

    #Row Processing
    for row in range(nrows):
        #print(row)
        M_row = np.median(in_arr[row, :])
        row_scale = desired_intensity/M_row
        if M_row < threshold:
            for col in range(ncols):
                if in_arr[row, col] < M_row*threshold:
                    pass
                else:
                    in_arr[row, col] *= row_scale
        else:
            for col in range(ncols):
                in_arr[row, col] *= row_scale
    print('done in rows')
    #Column Processing
    for col in range(ncols):
        #print(col)
        M_col = np.median(in_arr[:, col])
        col_scale = desired_intensity/M_col
        if M_col < threshold:
            for row in range(nrows):
                if in_arr[row,col] < M_col*threshold:
                    pass
                else:
                    in_arr[row, col] *= col_scale
        else:
            for row in range(nrows):
                in_arr[row, col] *= col_scale
    print('done');
    misc.imsave(os.path.join(out_path, img_out),in_arr)