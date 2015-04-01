import random 
import sys

def random_matrix(n):
    print("{}".format(n))
    for x in range (0, n):
        s = ""
        for y in range (0, n):
            s += "{}\t".format(random.uniform(-10000, 10000))
        print(s)


random_matrix(int(sys.argv[1]))

