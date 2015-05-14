from PIL import Image 
from scipy import misc

from scipy import linalg, dot  

def transform(input_matrix, dim):
    print len(input_matrix)
    u, sigma, vt = linalg.svd(input_matrix)
    for index in xrange(dim, len(input_matrix)):
        sigma[index] = 0.0
    return dot(dot(u, linalg.diagsvd(sigma, len(input_matrix), len(vt))), vt)  




lena = misc.lena()
U, s, Vh = linalg.svd(lena)
compressed = transform(lena, 64) 
