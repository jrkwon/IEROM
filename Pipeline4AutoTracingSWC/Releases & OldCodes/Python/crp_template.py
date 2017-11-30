from scipy import signal
from scipy import misc
import numpy as np

image = misc.imread('Sample1.jpg')
image -= image.mean()
image_slice = np.copy(image[0:2000,:])
template = misc.imread('Sample1_crop_org.jpg')
template -= template.mean()
print "correlating"
corr = signal.correlate2d(image_slice, template, boundary='fill', fillvalue=0)
print "correlated"
print corr
print np.argmax(corr)
y, x = np.unravel_index(np.argmax(corr), corr.shape) # find the match
print x, y
print "done~"

import matplotlib.pyplot as plt
fig, (ax_orig, ax_template, ax_corr) = plt.subplots(1, 3)
ax_orig.imshow(image_slice, cmap='gray')
ax_orig.set_title('Original')
ax_orig.set_axis_off()
ax_template.imshow(template, cmap='gray')
ax_template.set_title('Template')
ax_template.set_axis_off()
ax_corr.imshow(corr, cmap='gray')
ax_corr.set_title('Cross-correlation')
ax_corr.set_axis_off()
ax_orig.plot(x, y, 'ro')
fig.show()
