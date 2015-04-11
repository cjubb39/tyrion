from numpy import matrix 
from scipy.linalg import qr, svd


if __name__ == "main":
    filename = "tutorial-input.txt"
    with open(filename, "r") as f: 
        text = f.read() 
        a = matrix(text) 

        print a
