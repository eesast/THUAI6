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
            if mimetypes.guess_type(file)[0] == 'text/plain':
                with open(path, 'rU') as f:
                    data = f.read()
                    hashlist[path.replace('\\','/')]=hashlib.md5(data).hexdigest()
            else:
                with open(path, 'rb') as f:
                    data = f.read()
                hashlist[path.replace('\\','/')]=hashlib.md5(data).hexdigest()
    targetFile=os.path.join(targetfolder,'hash.json')
    with open(targetFile, 'w') as fp:
        json.dump(hashlist,fp)
    
parser = argparse.ArgumentParser()
parser.add_argument('-t',type=str)

args=parser.parse_args()

targetfolder = args.t

generateHashFile()
