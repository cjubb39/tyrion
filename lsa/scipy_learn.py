from numpy import matrix, loadtxt, delete, dot, take, append   
from scipy.linalg import qr, svd, diagsvd


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

        s_prime = take(s, [0, 1] )
        s_prime = append(s_prime, [0.0, 0.0, 0.0] ) 
        print(s_prime)
        print (u)
        print("U dot sigma prime")
        a_prime = u.dot(diagsvd(s_prime, 8, 5))
        print(a_prime)
 #       print("from R: U ") 
 #       print(uu)
        print("From r: U dot sigma prime")
        r_a_prime = uu.dot(diagsvd(s_prime, 5, 5)) 
        print(r_a_prime)
#        print ("Q")
#        print(q)

        q = delete(q, [5, 6, 7] , axis=1)
        r_a_prime = q.dot(r_a_prime) 

        print("From r: U dot sigma prime")
        print(r_a_prime) 


