from numpy import matrix, loadtxt, delete  
from scipy.linalg import qr, svd


if __name__ == "__main__":
    filename = "tutorial_input.txt"
    with open(filename, "r") as f: 
        text = f.read()
        #print text 
        a = loadtxt(filename) 
        #print(a)
        u, s, vh = svd(a)
        #print ("Singular values")
        #print(s)

        q, r = qr(a)
        #print ("Q and R in A = QR")
        #print (q)
        #print (r)
        r = delete(r,(5, 6, 7),axis=0)
        #print (r)
        #print ("SVD of R") 
        uu, ss, vvh = svd(r) 

        #print(ss)
        matrix_str = ""
        for row in r:
            row_str = "\t".join(str(e) for e in row) 
            row_str += "\n"
            matrix_str += row_str
        print len(r)
        print matrix_str
