using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using COSXML;
using COSXML.Auth;
using COSXML.Model.Object;
using COSXML.Model.Bucket;
using COSXML.CosException;
using System.IO;
using Newtonsoft.Json;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text.Json.Nodes;
using System.Diagnostics;

namespace Installer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }
    }
}

namespace Downloader
{
    class Program
    {
        static List<string> newFileName = new List<string>();         //新文件名
        static List<string> updateFileName = new List<string>();      //更新文件名
        static string ProgramName = "THUAI6";                       //要运行或下载的程序名称
        static string playerFolder = "player";                      //选手代码保存文件夹路径
        static string startName = "maintest.exe";                   //启动的程序名

        public class Data
        {
            public static string path = "";       //标记路径记录文件THUAI6.dat的路径
            public static string FilePath = "";  //最后一级为THUAI6文件夹所在目录
            public static string dataPath = "";   //C盘的文档文件夹
            public Data(string path)
            {
                //dataPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                dataPath= new DirectoryInfo(".").FullName;
                Data.path=System.IO.Path.Combine(dataPath, "THUAI6.dat");
                if (File.Exists(Data.path))
                {
                    using (StreamReader r = new StreamReader(Data.path))
                        FilePath = r.ReadLine().Replace('\\', '/');           //读取THUAI6.dat文件的第一行
                }
                else
                {
                    FilePath = System.IO.Path.GetDirectoryName(@path);

                    //将dat文件写入程序运行路径
                    FileStream fs = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                    StreamWriter sw = new StreamWriter(fs);
                    sw.Write(path);
                    sw.Flush();
                    sw.Close();
                }
            }

            public static void ChangeData(string newLine)
            {
                if (Directory.Exists(@newLine))  //判断文件夹是否存在
                {
                    Console.Write($"是否创建新路径 {newLine}？y/n:");
                    if (Console.Read() != 'y')
                    {
                        Console.WriteLine("创建取消!"); return;
                    }

                    using (StreamWriter w = new StreamWriter(path))
                        w.WriteLine(@newLine.Trim('\r').Trim('\n'));
                    Console.WriteLine($"当前下载路径为{newLine}");
                }
            }

            public static void ResetFilepath(string newPath)
            {
                FilePath=newPath.Replace('\\', '/');
                path=System.IO.Path.Combine(dataPath, "THUAI6.dat");
                FileStream fs = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                StreamWriter sw = new StreamWriter(fs);
                fs.SetLength(0);
                sw.Write(newPath);
                sw.Flush();
                sw.Close();
                fs.Close();
            }

        }
        public class Tencent_cos_download
        {
            public void download(string download_dir, string key)
            {
                //download_dir标记根文件夹路径，key为相对根文件夹的路径（不带./）
                //初始化CosXmlConfig（提供配置SDK接口）
                string appid = "1314234950";  //设置腾讯云账户的账户标识（APPID）
                string region = "ap-beijing";   //设置一个默认的存储桶地域
                CosXmlConfig config = new CosXmlConfig.Builder()
                  .IsHttps(true)  //设置默认 HTTPS 请求
                  .SetAppid(appid)  //设置腾讯云账户的账户标识 APPID
                  .SetRegion(region)  //设置一个默认的存储桶地域
                  .SetDebugLog(true)  //显示日志
                  .Build();  //创建 CosXmlConfig 对象

                //永久密钥访问凭证
                string secretId = ""; //"云 API 密钥 SecretId";
                string secretKey = ""; //"云 API 密钥 SecretKey";

                long durationSecond = 1000;  //每次请求签名有效时长，单位为秒
                QCloudCredentialProvider cosCredentialProvider = new DefaultQCloudCredentialProvider(
                    secretId, secretKey, durationSecond);

                //初始化 CosXmlServer
                CosXmlServer cosXml = new CosXmlServer(config, cosCredentialProvider);

                //创建存储桶
                try
                {
                    string bucket = "thuai6-1314234950"; //格式：BucketName-APPID
                    string localDir = System.IO.Path.GetDirectoryName(download_dir);    //本地文件夹
                    string localFileName = System.IO.Path.GetFileName(download_dir);    //指定本地保存的文件名
                    GetObjectRequest request = new GetObjectRequest(bucket, key, localDir, localFileName);

                    Dictionary<string, string> test = request.GetRequestHeaders();
                    request.SetCosProgressCallback(delegate (long completed, long total)
                    {
                        Console.WriteLine(String.Format("progress = {0:##.##}%", completed * 100.0 / total));
                    });
                    //执行请求
                    GetObjectResult result = cosXml.GetObject(request);
                    //请求成功
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

            public static void GetNewHash()
            {
                Tencent_cos_download Downloader = new Tencent_cos_download();
                Downloader.download(System.IO.Path.Combine(Data.FilePath, "hash.json"), "hash.json");
            }

            public static string GetFileMd5Hash(string strFileFullPath)
            {
                FileStream fst = null;
                try
                {
                    fst=new FileStream(strFileFullPath, FileMode.Open);
                    byte[] data = MD5.Create().ComputeHash(fst);

                    StringBuilder sBuilder = new StringBuilder();

                    for (int i = 0; i < data.Length; i++)
                    {
                        sBuilder.Append(data[i].ToString("x2"));
                    }

                    fst.Close();
                    return sBuilder.ToString().ToLower();
                }
                catch (Exception)
                {
                    if (fst != null)
                        fst.Close();
                    return "";
                }
                finally { }
            }

            private static void Check()
            {
                string json, MD5, jsonName;
                int newFile = 0, updateFile = 0;
                newFileName.Clear();
                updateFileName.Clear();
                jsonName = "hash.json";

                Tencent_cos_download Downloader = new Tencent_cos_download();
                try
                {
                    //如果json存在就删了重新下
                    if (File.Exists(System.IO.Path.Combine(Data.FilePath, jsonName)))
                    {
                        File.Delete(System.IO.Path.Combine(Data.FilePath, jsonName));
                        Downloader.download(System.IO.Path.Combine(Data.FilePath, jsonName), jsonName);
                    }
                    else
                    {
                        Downloader.download(System.IO.Path.Combine(Data.FilePath, jsonName), jsonName);
                    }
                }
                catch (CosClientException clientEx)
                {
                    //请求失败
                    Console.WriteLine("CosClientException: " + clientEx.ToString() + Environment.NewLine);
                    return;
                }
                catch (CosServerException serverEx)
                {
                    //请求失败
                    Console.WriteLine("CosClientException: " + serverEx.ToString() + Environment.NewLine);
                    return;
                }

                using (StreamReader r = new StreamReader(System.IO.Path.Combine(Data.FilePath, jsonName)))
                    json = r.ReadToEnd();
                json = json.Replace("\r", string.Empty).Replace("\n", string.Empty);
                Dictionary<string, string> jsonDict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                foreach (KeyValuePair<string, string> pair in jsonDict)
                {
                    MD5=GetFileMd5Hash(System.IO.Path.Combine(Data.FilePath, pair.Key));
                    if (MD5.Length == 0)    //文档不存在
                        newFileName.Add(pair.Key);
                    else if (MD5 != pair.Value)     //MD5不匹配
                        updateFileName.Add(pair.Key);
                }

                newFile = newFileName.Count;
                updateFile = updateFileName.Count;
                Console.WriteLine("----------------------" + Environment.NewLine);

                if (newFile + updateFile == 0)
                {
                    Console.WriteLine("当前平台已是最新版本！" + Environment.NewLine);
                    newFileName.Clear(); updateFileName.Clear();
                }
                else
                {
                    Console.WriteLine($"发现{newFile}个新文件" + Environment.NewLine);
                    foreach (string filename in newFileName)
                    {
                        Console.WriteLine(filename);
                    }
                    Console.WriteLine(Environment.NewLine + $"发现{updateFile}个文件更新" + Environment.NewLine);
                    foreach (string filename in updateFileName)
                    {
                        Console.WriteLine(filename);
                    }
                    Console.Write(Environment.NewLine + "是否下载新文件？ y/n：");
                    if (Console.Read() != 'y') Console.WriteLine("下载取消!");
                    else Download();
                }
            }
            private static void Download()
            {
                Tencent_cos_download Downloader = new Tencent_cos_download();

                int newFile = 0, updateFile = 0;
                int totalnew = newFileName.Count, totalupdate = updateFileName.Count;

                if (newFileName.Count > 0 || updateFileName.Count > 0)
                {
                    try
                    {
                        foreach (string filename in newFileName)
                        {
                            Console.WriteLine(newFile + 1 + "/" + totalnew + ":开始下载" + filename);
                            Downloader.download(System.IO.Path.Combine(@Data.FilePath, filename), filename);
                            Console.WriteLine(filename + "下载完毕!" + Environment.NewLine);
                            newFile++;
                        }
                        foreach (string filename in updateFileName)
                        {
                            Console.WriteLine(updateFile + 1 + "/" + totalupdate + ":开始下载" + filename);
                            File.Delete(System.IO.Path.Combine(@Data.FilePath, filename));
                            Downloader.download(System.IO.Path.Combine(@Data.FilePath, filename), filename);
                            Console.WriteLine(filename + "下载完毕!" + Environment.NewLine);
                            updateFile++;
                        }
                    }
                    catch (CosClientException clientEx)
                    {
                        //请求失败
                        Console.WriteLine("CosClientException: " + clientEx.ToString() + Environment.NewLine);
                        return;
                    }
                    catch (CosServerException serverEx)
                    {
                        //请求失败
                        Console.WriteLine("CosClientException: " + serverEx.ToString() + Environment.NewLine);
                        return;
                    }
                    catch (Exception)
                    {
                        throw;
                    }
                }
                else Console.WriteLine("当前平台已是最新版本！" + Environment.NewLine);
                newFileName.Clear();
                updateFileName.Clear();
            }

            public static bool CheckAlreadyDownload()   //检查是否已经下载
            {
                string existpath = System.IO.Path.Combine(Data.dataPath, "Exists.txt");
                if (!File.Exists(existpath))    //文件不存在
                {
                    FileStream fs = new FileStream(existpath, FileMode.Create, FileAccess.ReadWrite);
                    fs.Close();
                    return false;
                }
                else        //文件存在
                {
                    FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.Read);
                    StreamReader sr = new StreamReader(fs);
                    if ("true"==sr.ReadLine())
                    {
                        sr.Close();
                        fs.Close();
                        return true;
                    }
                    else
                    {
                        sr.Close();
                        fs.Close();
                        return false;
                    }
                }
            }

            public static void DownloadAll()            //下载全部文件
            {
                string jsonName = "hash.json";
                string json;
                Tencent_cos_download Downloader = new Tencent_cos_download();

                try
                {
                    //如果json存在就删了重新下
                    if (File.Exists(System.IO.Path.Combine(Data.FilePath, jsonName)))
                    {
                        File.Delete(System.IO.Path.Combine(Data.FilePath, jsonName));
                        Downloader.download(System.IO.Path.Combine(Data.FilePath, jsonName), jsonName);
                    }
                    else
                    {
                        Downloader.download(System.IO.Path.Combine(Data.FilePath, jsonName), jsonName);
                    }
                }
                catch (CosClientException clientEx)
                {
                    //请求失败
                    Console.WriteLine("CosClientException: " + clientEx.ToString() + Environment.NewLine);
                    return;
                }
                catch (CosServerException serverEx)
                {
                    //请求失败
                    Console.WriteLine("CosClientException: " + serverEx.ToString() + Environment.NewLine);
                    return;
                }
                using (StreamReader r = new StreamReader(System.IO.Path.Combine(Data.FilePath, jsonName)))
                    json = r.ReadToEnd();
                json = json.Replace("\r", string.Empty).Replace("\n", string.Empty);
                Dictionary<string, string> jsonDict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);

                newFileName.Clear();
                updateFileName.Clear();
                foreach (KeyValuePair<string, string> pair in jsonDict)
                {
                    newFileName.Add(pair.Key);
                }
                Download();

                string existpath = System.IO.Path.Combine(Data.dataPath, "Exists.txt");
                FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                StreamWriter sw = new StreamWriter(fs);
                fs.SetLength(0);
                sw.Write("true");
                sw.Close();
                fs.Close();
            }

            public static void Change_all_hash(string topDir, Dictionary<string, string> jsonDict)    //更改HASH
            {
                DirectoryInfo theFolder = new DirectoryInfo(@topDir);
                bool ifexist = false;

                //遍历文件
                foreach (FileInfo NextFile in theFolder.GetFiles())
                {
                    string filepath = topDir+@"/"+NextFile.Name;//文件路径
                    Console.WriteLine(filepath);
                    foreach (KeyValuePair<string, string> pair in jsonDict)
                    {
                        if (System.IO.Path.Equals(filepath, System.IO.Path.Combine(Data.FilePath, pair.Key).Replace('\\', '/')))
                        {
                            ifexist = true;
                            string MD5 = GetFileMd5Hash(filepath);
                            jsonDict[pair.Key] = MD5;
                        }
                    }
                    if (!ifexist && NextFile.Name!="hash.json")
                    {
                        string MD5 = GetFileMd5Hash(filepath);
                        string relapath = filepath.Replace(Data.FilePath+'/', string.Empty);
                        jsonDict.Add(relapath, MD5);
                    }
                    ifexist = false;
                }

                //遍历文件夹
                foreach (DirectoryInfo NextFolder in theFolder.GetDirectories())
                {
                    if (System.IO.Path.Equals(NextFolder.FullName, 
                        System.IO.Path.GetFullPath(System.IO.Path.Combine(Data.FilePath, playerFolder))))
                    {
                        foreach (FileInfo NextFile in NextFolder.GetFiles())
                        {
                            if (NextFile.Name=="README.md")
                            {
                                string MD5 = GetFileMd5Hash(NextFile.FullName);
                                string relapath = NextFile.FullName.Replace('\\', '/').Replace(Data.FilePath+'/', string.Empty);
                                jsonDict.Add(relapath, MD5);
                            }
                        }
                        continue;       //如果是选手文件夹就忽略
                    }
                    Change_all_hash(NextFolder.FullName.Replace('\\', '/'), jsonDict);
                }
            }
            public static void UpdateHash()
            {
                while (true)
                {
                    if (Directory.Exists(Data.FilePath))
                    {
                        string json;
                        if (!File.Exists(System.IO.Path.Combine(Data.FilePath, "hash.json")))
                        {
                            Console.WriteLine("hash.json文件丢失！即将重新下载该文件！");
                            GetNewHash();
                        }
                        using (StreamReader r = new StreamReader(System.IO.Path.Combine(Data.FilePath, "hash.json")))
                            json = r.ReadToEnd();
                        json = json.Replace("\r", string.Empty).Replace("\n", string.Empty).Replace("/", @"\\");
                        Dictionary<string, string> jsonDict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                        Change_all_hash(Data.FilePath, jsonDict);
                        OverwriteHash(jsonDict);
                        break;
                    }
                    else
                    {
                        Console.WriteLine("读取路径失败！请重新输入文件路径：");
                        Data.ResetFilepath(Console.ReadLine());
                    }
                }
            }

            public static void DeleteAll()
            {
                DirectoryInfo di = new DirectoryInfo(Data.FilePath);
                DirectoryInfo player = new DirectoryInfo(System.IO.
                    Path.GetFullPath(System.IO.Path.Combine(Data.FilePath, playerFolder)));
                try
                {
                    foreach (FileInfo file in di.GetFiles())
                    {
                        file.Delete();
                    }
                    foreach (FileInfo file in player.GetFiles())
                    {
                        if (file.Name=="README.md")
                        {
                            continue;
                        }
                        string filename = System.IO.Path.GetFileName(file.FullName);
                        file.MoveTo(System.IO.Path.Combine(Data.FilePath, filename));
                    }
                    foreach (DirectoryInfo subdi in di.GetDirectories())
                    {
                        subdi.Delete(true);
                    }
                }
                catch (UnauthorizedAccessException)
                {
                    Console.WriteLine("权限不足，无法删除！");
                    return;
                }
                catch (DirectoryNotFoundException)
                {
                    Console.WriteLine("文件夹没有找到，请检查是否已经手动更改路径");
                    return;
                }
                catch (IOException)
                {
                    Console.WriteLine("文件已经打开，请关闭后再删除");
                    return;
                }

                string existpath = System.IO.Path.Combine(Data.dataPath, "Exists.txt");
                FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                StreamWriter sw = new StreamWriter(fs);
                fs.SetLength(0);
                sw.Write("false");
                sw.Close();
                fs.Close();

                try
                {
                    File.Delete(Data.path);
                }
                catch (UnauthorizedAccessException)
                {
                    Console.WriteLine("权限不足，无法删除！");
                    return;
                }
                catch (DirectoryNotFoundException)
                {
                    Console.WriteLine("文件夹没有找到，请检查是否已经手动更改路径");
                    return;
                }
                catch (IOException)
                {
                    Console.WriteLine("文件已经打开，请关闭后再删除");
                    return;
                }
                Console.WriteLine($"删除成功！player文件夹中的文件已经放在{ProgramName}的根目录下");
            }

            public static void OverwriteHash(Dictionary<string, string> jsonDict)
            {
                string Contentjson = JsonConvert.SerializeObject(jsonDict);
                Contentjson=Contentjson.Replace("\r", String.Empty).Replace("\n", String.Empty).Replace(@"\\", "/");
                File.WriteAllText(@System.IO.Path.Combine(Data.FilePath, "hash.json"), Contentjson);
            }

            public static void MoveProgram()
            {
                string newPath = Console.ReadLine();
                DirectoryInfo newdi = new DirectoryInfo(newPath);
                DirectoryInfo olddi = new DirectoryInfo(Data.FilePath);
                try
                {
                    foreach (DirectoryInfo direct in olddi.GetDirectories())
                    {
                        direct.MoveTo(System.IO.Path.Combine(newPath, direct.Name));
                    }
                    foreach (FileInfo file in olddi.GetFiles())
                    {
                        file.MoveTo(System.IO.Path.Combine(newPath, file.Name));
                    }
                }
                catch (DirectoryNotFoundException)
                {
                    Console.WriteLine("原路径未找到！请检查文件是否损坏");
                    foreach (DirectoryInfo newdirect in newdi.GetDirectories())
                    {
                        newdirect.MoveTo(System.IO.Path.Combine(Data.FilePath, newdirect.Name));
                    }
                    foreach (FileInfo file in newdi.GetFiles())
                    {
                        file.MoveTo(System.IO.Path.Combine(Data.FilePath, file.Name));
                    }
                    Console.WriteLine("移动失败！");
                }
                catch (IOException)
                {
                    Console.WriteLine("文件已打开或者目标路径下有同名文件！");
                    foreach (DirectoryInfo newdirect in newdi.GetDirectories())
                    {
                        newdirect.MoveTo(System.IO.Path.Combine(Data.FilePath, newdirect.Name));
                    }
                    foreach (FileInfo file in newdi.GetFiles())
                    {
                        file.MoveTo(System.IO.Path.Combine(Data.FilePath, file.Name));
                    }
                    Console.WriteLine("移动失败！");
                }
                Data.ResetFilepath(newPath);
                Console.WriteLine("更改路径成功!");
            }
            public static void main(string[] args)
            {
                Data date = new Data("");
                while (true)
                {
                    Console.WriteLine($"1. 更新hash.json   2. 检查更新   3.下载{ProgramName}  4.删除{ProgramName}  5.启动进程  6.移动{ProgramName}到其它路径");
                    string choose = Console.ReadLine();
                    if (choose=="1")
                    {
                        if (!CheckAlreadyDownload())
                        {
                            Console.WriteLine($"未下载{ProgramName}，请先执行下载操作！");
                            continue;
                        }
                        UpdateHash();
                    }
                    else if (choose=="2")
                    {
                        if (!CheckAlreadyDownload())
                        {
                            Console.WriteLine($"未下载{ProgramName}，请先执行下载操作！");
                            continue;
                        }
                        while (true)
                        {
                            if (Data.FilePath!=null && Directory.Exists(Data.FilePath))
                            {
                                Check();
                                break;
                            }
                            else
                            {
                                Console.WriteLine("读取路径失败！请重新输入文件路径：");
                                Data.ResetFilepath(Console.ReadLine());
                            }
                        }
                    }
                    else if (choose =="3")
                    {
                        if (CheckAlreadyDownload())
                        {
                            Console.WriteLine($"已经将{ProgramName}下载到{Data.FilePath}！若要重新下载请先完成删除操作！");

                        }
                        else
                        {
                            string newpath;
                            Console.WriteLine("请输入下载路径：");
                            newpath=Console.ReadLine();
                            Data.ResetFilepath(newpath);
                            DownloadAll();
                        }
                    }
                    else if (choose=="4")
                    {
                        DeleteAll();
                    }
                    else if (choose=="5")
                    {
                        if (CheckAlreadyDownload())
                        {
                            Process.Start(System.IO.Path.Combine(Data.FilePath, startName));
                        }
                        else
                        {
                            Console.WriteLine($"未下载{ProgramName}，请先执行下载操作！");
                        }
                    }
                    else if (choose=="6")
                    {
                        MoveProgram();
                    }
                    else if (choose=="exit")
                    {
                        return;
                    }
                }
            }
        }
    }
}

