# -*- coding=utf-8
import sys
import os
import argparse
import getopt
import hashlib
import json
import mimetypes
        

def generateHashFile():
    file = targetfolder
    hashlist={}
    for root, dirs, files in os.walk(file):
        for file in files:
            path = os.path.join(root, file)
            with open(path, 'rb') as f:
                data = f.read()
                pathr = path.replace('../','') 
                pathr = path.replace('./','') 
                hashlist[pathr.replace('\\','/')]=hashlib.md5(data).hexdigest()
    targetFile=os.path.join(targetfolder,'hash.json')
    with open(targetFile, 'w') as fp:
        json.dump(hashlist,fp)
    
parser = argparse.ArgumentParser()
parser.add_argument('-t',type=str)

args=parser.parse_args()

targetfolder = args.t

generateHashFile()
