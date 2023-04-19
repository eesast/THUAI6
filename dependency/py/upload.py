# -*- coding=utf-8
from qcloud_cos import CosConfig
from qcloud_cos import CosS3Client
import sys
import os
import logging
import getopt
import hashlib
import json

def upload(path):
    response = client.upload_file(
        Bucket='thuai6-1314234950',
        LocalFilePath= path,
        Key = "TEST",
        PartSize=1,
        MAXThread=10,
        EnableMD5=False
    )
    return response

def generateHashFile():
    file = targetfolder
    hashlist={}
    for root, dirs, files in os.walk(file):
        for file in files:
            path = os.path.join(root, file)
            with open(path, 'rb') as f:
                data = f.read()
                hashlist[path.replace(file)]=hashlib.md5(data)
    res = json.dumps(hashlist)
    

try:
    opts,args = getopt.getopt(sys.argv[1:],"t:i:k:")
except getopt.GetoptError:
    print("upload.py -d<targetfolder> -i<SecretID> -k<SecretKey>")

sKey=""
sId=""
targetfolder=""
for opt,arg in opts:
    if opt == "-t":
        targetfolder = arg
    elif opt == "-i":
        sId = arg
    elif opt == "-k":
        sKey = arg

print("skey:"+sKey+"  sid:"+sId+"  tar:"+targetfolder)
# 正常情况日志级别使用 INFO，需要定位时可以修改为 DEBUG，此时 SDK 会打印和服务端的通信信息
logging.basicConfig(level=logging.INFO, stream=sys.stdout)

# 1. 设置用户属性, 包括 secret_id, secret_key, region等。Appid 已在 CosConfig 中移除，请在参数 Bucket 中带上 Appid。Bucket 由 BucketName-Appid 组成
secret_id = sId     # 用户的 SecretId，建议使用子账号密钥，授权遵循最小权限指引，降低使用风险。子账号密钥获取可参见 https://cloud.tencent.com/document/product/598/37140
secret_key = sKey   # 用户的 SecretKey，建议使用子账号密钥，授权遵循最小权限指引，降低使用风险。子账号密钥获取可参见 https://cloud.tencent.com/document/product/598/37140
region = 'ap-beijing'      # 替换为用户的 region，已创建桶归属的 region 可以在控制台查看，https://console.cloud.tencent.com/cos5/bucket
                           # COS 支持的所有 region 列表参见 https://cloud.tencent.com/document/product/436/6224
token = None               # 如果使用永久密钥不需要填入 token，如果使用临时密钥需要填入，临时密钥生成和使用指引参见 https://cloud.tencent.com/document/product/436/14048
scheme = 'https'           # 指定使用 http/https 协议来访问 COS，默认为 https，可不填

config = CosConfig(Region=region, SecretId=secret_id, SecretKey=secret_key, Token=token, Scheme=scheme)
client = CosS3Client(config)

generateHashFile()

#### 高级上传接口（推荐）
# 根据文件大小自动选择简单上传或分块上传，分块上传具备断点续传功能。
