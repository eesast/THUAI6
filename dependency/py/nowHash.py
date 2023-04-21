# -*- coding=utf-8
import sys
import os
import argparse
import getopt
import hashlib
import json
import mimetypes
        

with open('..\..\.git\FETCH_HEAD', 'r') as f:
    data = f.read()
with open('..\..\hash.txt', 'w') as fw:
    fw.write(data[:data.find('	')])
