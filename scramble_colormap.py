import numpy as np
from PIL import Image
import matplotlib.pyplot as plt
import matplotlib as mpl
import sys

# Read in a segmentation TIFF image.
im = Image.open(sys.argv[1])
im.show()
# Import Image Data into Numpy array.
x = np.array(im)
# Obtain number of segmentation area.
N = np.max(x)
# Randomly set color map.
cmap = mpl.colors.ListedColormap(np.random.rand(N+1, 3))
plt.imshow(x, cmap=cmap, interpolation='none')
plt.colorbar()

plt.savefig(sys.argv[2])

plt.show()