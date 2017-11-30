import SimpleITK as sitk
import pylab
image = sitk.Image(256,128,64,sitk.sitkInt16) #A 3D Image
image_2D1 = sitk.Image(64,64,sitk.sitkFloat32)
image_2D2 = sitk.Image([32,32],sitk.sitkUInt32)
image_RGB = sitk.Image([128,128],sitk.sitkVectorUInt8,3)

print image.GetSize()
print image.GetOrigin()
print image.GetSpacing()
print image.GetDirection()
print image.GetNumberOfComponentsPerPixel()
print image_RGB.GetNumberOfComponentsPerPixel()
print image_2D2.GetNumberOfComponentsPerPixel()
