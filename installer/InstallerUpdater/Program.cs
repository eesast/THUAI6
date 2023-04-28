using COSXML.Auth;
using COSXML.CosException;
using COSXML.Model.Object;
using COSXML;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using Newtonsoft.Json;
using System.Windows.Markup;
using System;
using System.Security.Cryptography;
using System.Diagnostics;
using System.Windows;
using System.Windows.Documents;

namespace Program
{
    class Updater
    {
        public static string Dir = System.IO.Path.GetDirectoryName(System.Diagnostics.Process.GetCurrentProcess().MainModule?.FileName)
            ?? throw new Exception("Cannot get current directory");
        public static string InstallerName = "Installer.exe";
        public static string jsonKey = "installerHash.json";
        public static string KeyHead = "Installer/";

        public static bool UpdateInstaller()
        {
            string json;
            try
            {
                using (StreamReader r = new StreamReader(System.IO.Path.Combine(Dir, "updateList.json")))
                    json = r.ReadToEnd();
                json = json.Replace("\r", string.Empty).Replace("\n", string.Empty);
                List<string> jsonList = JsonConvert.DeserializeObject<List<string>>(json)
                    ?? throw new Exception("Failed to deserialize json!");
                foreach (string todo in jsonList)
                {
                    if (!todo.Equals("None"))
                    {
                        File.Delete(Path.Combine(Dir, todo));
                        download(Path.Combine(Dir, todo), KeyHead + todo);
                    }
                }
            }
            catch (IOException ex)
            {
                MessageBox.Show($"下载器本体未能成功关闭：\n{ex}");
                return false;
            }
            catch (Exception ex)
            {
                MessageBox.Show($"尝试下载时出现问题：\n{ex}\n{ex.StackTrace}");
                return false;
            }
            return true;
        }

        public static int TellDismiss()
        {
            try
            {
                string savepath = System.IO.Path.Combine(Dir, "updateList.json");
                FileStream fs = new FileStream(savepath, FileMode.Open, FileAccess.ReadWrite);
                StreamReader sr = new StreamReader(fs);
                string json = sr.ReadToEnd();
                if (json == null || json == "")
                {
                    json += @"{""None""}";
                }
                List<string> ls = new List<string>();
                ls = JsonConvert.DeserializeObject<List<string>>(json)
                    ?? throw new Exception("Failed to deserialize json!");
                if (!ls.Contains("Dismiss"))
                {
                    ls.Add("Dismiss");
                }
                sr.Close();
                fs.Close();

                StreamWriter sw = new StreamWriter(System.IO.Path.Combine(Dir, "updateList.json"), false);
                sw.WriteLine(JsonConvert.SerializeObject(ls));
                sw.Close();

                return 0;//成功
            }
            catch
            {
                return -1;//失败
            }
        }

        public static void download(string download_dir, string key)
        {
            // download_dir标记根文件夹路径，key为相对根文件夹的路径（不带./）
            // 初始化CosXmlConfig（提供配置SDK接口）
            string appid = "1314234950";   // 设置腾讯云账户的账户标识（APPID）
            string region = "ap-beijing";  // 设置一个默认的存储桶地域
            CosXmlConfig config = new CosXmlConfig.Builder()
                                      .IsHttps(true)      // 设置默认 HTTPS 请求
                                      .SetAppid(appid)    // 设置腾讯云账户的账户标识 APPID
                                      .SetRegion(region)  // 设置一个默认的存储桶地域
                                      .SetDebugLog(true)  // 显示日志
                                      .Build();           // 创建 CosXmlConfig 对象

            // 永久密钥访问凭证
            string secretId = "***"; //"云 API 密钥 SecretId";
            string secretKey = "***"; //"云 API 密钥 SecretKey";

            long durationSecond = 1000;  // 每次请求签名有效时长，单位为秒
            QCloudCredentialProvider cosCredentialProvider = new DefaultQCloudCredentialProvider(
                secretId, secretKey, durationSecond
            );
            // 初始化 CosXmlServer
            CosXmlServer cosXml = new CosXmlServer(config, cosCredentialProvider);

            // 创建存储桶
            try
            {
                string bucket = "thuai6-1314234950";                                // 格式：BucketName-APPID
                string localDir = System.IO.Path.GetDirectoryName(download_dir)     // 本地文件夹
                    ?? throw new Exception("Failed to get directory name!");
                string localFileName = System.IO.Path.GetFileName(download_dir);    // 指定本地保存的文件名
                GetObjectRequest request = new GetObjectRequest(bucket, key, localDir, localFileName);

                Dictionary<string, string> test = request.GetRequestHeaders();
                request.SetCosProgressCallback(delegate (long completed, long total)
                {
                    //Console.WriteLine(String.Format("progress = {0:##.##}%", completed * 100.0 / total));
                });
                // 执行请求
                GetObjectResult result = cosXml.GetObject(request);
                // 请求成功
            }
            catch (CosClientException clientEx)
            {
                throw clientEx;
            }
            catch (CosServerException serverEx)
            {
                throw serverEx;
            }
        }
    }
}