import os 
import sys 
from parser import Parser

from collections import defaultdict 

def _clean(string):
    """ remove any nasty grammar tokens from string """

    grammar_tokens = [".", ",", "<", ">", "?", "!", ":", ";", "\"", "(", ")", "{", "}", "~", "|", "/" ] 

    for g in grammar_tokens: 
        string = string.replace(g, "")

    string = string.replace("\s+"," ")
    string = string.lower()
    return string

def resolve_simple_plurals(wset):
    rlist = []
    for w in wset:
        if w.endswith("s"):
            if w[:len(w) - 1] in wset: 
                continue 
        else:
            rlist.append(w) 
    return set(rlist) 

with open(sys.argv[1], "r") as f: 
    content = f.read(); 
    lines = content.split("<DOC>")
    lines = [line.lstrip() for line in lines] 
    docs = [] 
    vocab = defaultdict(int) 
    
    for line in lines:
        doc_lines = line.split("\n") 
        doc_lines = [l.lstrip() for l in doc_lines if not l.lstrip().startswith("<")] 
        doc_text = " ".join(doc_lines) 
        docs.append(doc_text)

        doc_text = _clean(doc_text) 

        doc_words = doc_text.split(" ") 

        for w in doc_words:
            vocab[w] += 1 

        

    sorted_vocab = sorted(vocab.iteritems(), key = lambda x: x[1], reverse=True)
    sorted_vocab = [x for x in sorted_vocab if x[0].isalpha()]   

    stop_file = open("../data/english.stop", "r") 

    stop_words_list = stop_file.readlines()
    stop_words = {x.strip() for x in stop_words_list}


    sorted_vocab = [x for x in sorted_vocab if x[0] not in stop_words] 
    vocab = [] 
    for i in range(256):
        vocab.append(sorted_vocab[i][0])

    vocab = sorted(vocab) 
    #for v in vocab:
    #    print v


    #generate document matrix from docs 

    final_vocab_set = {x for x in vocab} 

    doc_matrix = [] 

    for i in range(256): 
        doc_dict = defaultdict(int) 

        d_words = docs[i].split(" ")
        d_list = [] 

        for w in d_words:
            if w in final_vocab_set:
                doc_dict[w] += 1

        for v in vocab:
            if v in doc_dict:
                d_list.append(doc_dict[v]) 

            else:
                d_list.append(0) 

        doc_matrix.append(d_list) 
    
    print("256") 
    for d in doc_matrix:
        print " ".join(str(e) for e in d) 



