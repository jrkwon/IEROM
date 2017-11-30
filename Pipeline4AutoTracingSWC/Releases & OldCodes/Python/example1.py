import SimpleITK as sitk
dataDir = "C:/Users/BI2S_STUDENT/Documents/IEROM/Spring 2015/Images/2008-01-23 Whole mouse brain vasculature"
image = sitk.ReadImage ( dataDir + "/sample.jpg" )
print image
print '======================================='
image.GetMetaDataKeys()
#sitk.Show(image)

for key in image.GetMetaDataKeys():
        print "\"{0}\":\"{1}\"".format(key, image.GetMetaData(key))
