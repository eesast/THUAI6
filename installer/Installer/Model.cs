using COSXML.Auth;
using COSXML.CosException;
using COSXML.Model.Object;
using COSXML;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System;
using COSXML.Model.Bucket;
using System.Runtime.InteropServices;
using System.Text.Json.Nodes;
using System.Net.Http.Json;
using System.Text.Json;
using System.Xml.Schema;
using static Downloader.Program;
using System.Threading.Tasks;
using System.Net.Http;
using System.Windows;
using System.Windows.Shapes;
//using System.Windows.Forms;

using MessageBox = System.Windows.MessageBox;
using Downloader;

namespace starter.viewmodel.settings
{
    /// <summary>
    /// Settings Window Model
    /// </summary>
    public class SettingsModel
    {
        /// <summary>
        /// downloader function
        /// </summary>
        private Data configData = new Data("");
        private Tencent_cos_download cloud = new Tencent_cos_download();

        private HttpClient client = new HttpClient();
        private WebConnect.Web web = new WebConnect.Web();

        /// <summary>
        /// save settings
        /// </summary>
        public bool install()
        {
            if (Tencent_cos_download.CheckAlreadyDownload())
            {
                MessageBoxResult repeatOption = MessageBox.Show($"文件已存在于{Downloader.Program.Data.FilePath},是否移动到新位置？", "重复安装", MessageBoxButton.YesNo, MessageBoxImage.Warning, MessageBoxResult.No);
                // ask if abort install, with warning sign, defalut no;
                if (repeatOption == MessageBoxResult.No)
                {
                    return false;  // 回到选择地址界面
                }
                else
                {
                    Downloader.Program.Tencent_cos_download.MoveProgram(Route);
                    return true;
                }
            }
            else
            {
                Data.ResetFilepath(Route);
                Tencent_cos_download.DownloadAll();
                return true;
            }
        }
        public int move()
        {
            return Tencent_cos_download.MoveProgram(Route);
        }
        ///<summary>
        ///check for update
        /// </summary>
        static bool ProfileAvailable
        {
            get; set;
        }
        /// <summary>
        /// 检查更新
        /// </summary>
        /// <returns></returns>
        public Status checkUpdate()
        {
            UpdateInfo updateInfo = Tencent_cos_download.Check();
            if (updateInfo.newFileCount == -1)
            {
                if (updateInfo.changedFileCount == -1)
                {
                    return Status.error;
                }
                else
                {
                    return Status.disconnected;
                }
            }
            else
            {
                if (updateInfo.changedFileCount != 0 || updateInfo.newFileCount != 0)
                {
                    Updates = "发现新版本" + updateInfo.status;
                }
                return Status.menu;
            }
        }

        public void Login()
        {
            _ = web.LoginToEEsast(client, Username, Password);
        }
        public bool Update()
        {
            return Tencent_cos_download.Update();
        }
        public int Uninst()
        {
            return Tencent_cos_download.DeleteAll();
        }

        public bool Launch()
        {
            if (Tencent_cos_download.CheckAlreadyDownload())
            {
                Process.Start(System.IO.Path.Combine(Data.FilePath, startName));
                return true;
            }
            else
            {
                MessageBox.Show($"文件还不存在，请安装主体文件", "文件不存在", MessageBoxButton.OK, MessageBoxImage.Warning, MessageBoxResult.OK);
                return false;
            }
        }
        /// <summary>
        /// Route of files
        /// </summary>
        public string Route
        {
            get
            {
                return Data.FilePath;
            }
            set
            {
                Data.FilePath = value;
            }
        }

        public string Username
        {
            get; set;
        }
        public string Password
        {
            get; set;
        }
        /// <summary>
        /// 关于更新的屏幕显示信息
        /// </summary>
        private string updates;
        public string Updates
        {
            get
            {
                return updates;
            }
            set
            {
                updates = value;
            }
        }
        /// <summary>
        /// 关于介绍的屏幕显示信息
        /// </summary>
        public enum Status { newUser, menu, move, working, disconnected, error, successful, login, web };
        public Status status
        {
            get; set;
        }
        public bool Working
        {
            get; set;
        }
        /// <summary>
        /// if an update is planned
        /// </summary>
        public bool UpdatePlanned
        {
            get
            {
                return Program.UpdatePlanned;
            }
        }

        public bool CombatCompleted
        {
            get
            {
                return false;
            }
        }

        public bool LoginFailed
        {
            get; set;
        }
    }
}
namespace Downloader
{
    class Program
    {
        static List<string> newFileName = new List<string>();     // 新文件名
        static List<string> updateFileName = new List<string>();  // 更新文件名
        public static string ProgramName = "THUAI6";                     // 要运行或下载的程序名称
        public static string playerFolder = "player";                    // 选手代码保存文件夹路径
        public static string startName = "maintest.exe";          // 启动的程序名

        public struct UpdateInfo                                         // 更新信息，包括新版本版本号、更改文件数和新文件数
        {
            public string status;
            public int changedFileCount;
            public int newFileCount;
        }
        public static bool UpdatePlanned
        {
            get; set;
        }

        static int filenum = 0;                                   // 总文件个数

        public class Data
        {
            public static string path = "";      // 标记路径记录文件THUAI6.json的路径
            public static string FilePath = "";  // 最后一级为THUAI6文件夹所在目录
            public static string dataPath = "";  // C盘的文档文件夹
            public Data(string path)
            {
                // dataPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                dataPath = new DirectoryInfo(".").FullName;
                Data.path = System.IO.Path.Combine(dataPath, "THUAI6.json");
                if (File.Exists(Data.path))
                {
                    var dict = new Dictionary<string, string>();
                    using (StreamReader r = new StreamReader(Data.path))
                    {
                        string json = r.ReadToEnd();
                        if (json == null || json == "")
                        {
                            json += @"{""THUAI6""" + ":" + @"""2023""}";
                        }
                        dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                        if (dict != null && dict.ContainsKey("installpath"))
                        {
                            FilePath = dict["installpath"].Replace('\\', '/');
                        }  //读取安装路径
                    }
                    dict?.TryAdd("installpath", @path);
                    using FileStream fs = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                    using StreamWriter sw = new StreamWriter(fs);
                    sw.Write(JsonConvert.SerializeObject(dict));
                    sw.Flush();
                }
                else
                {
                    FilePath = System.IO.Path.GetDirectoryName(@path);

                    //将dat文件写入程序运行路径
                    string json;
                    Dictionary<string, string> dict = new Dictionary<string, string>();
                    using FileStream fs = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                    using (StreamReader r = new StreamReader(fs))
                    {
                        json = r.ReadToEnd();
                        if (json == null || json == "")
                        {
                            json += @"{""THUAI6""" + ":" + @"""2023""}";
                        }
                        dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                        dict?.Add("installpath", path);
                    }
                    using FileStream fs2 = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                    using StreamWriter sw = new StreamWriter(fs2);
                    sw.Write(JsonConvert.SerializeObject(dict));
                    sw.Flush();
                }
            }

            public static void ResetFilepath(string newPath)
            {
                string json;
                Dictionary<string, string> dict = new Dictionary<string, string>();
                FilePath = newPath.Replace('\\', '/');
                path = System.IO.Path.Combine(dataPath, "THUAI6.json");
                using FileStream fs = new FileStream(Data.path, FileMode.Create, FileAccess.ReadWrite);
                using (StreamReader r = new StreamReader(fs))
                {
                    json = r.ReadToEnd();
                    if (json == null || json == "")
                    {
                        json += @"{""THUAI6""" + ":" + @"""2023""}";
                    }
                    dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                    if (dict != null && dict.ContainsKey("installpath"))
                    {
                        dict["installpath"] = newPath;
                    }
                    else
                    {
                        dict.Add("installpath", newPath);
                    }
                }
                using FileStream fs2 = new FileStream(path, FileMode.Open, FileAccess.ReadWrite);
                using StreamWriter sw = new StreamWriter(fs2);
                fs2.SetLength(0);
                sw.Write(JsonConvert.SerializeObject(dict));
                sw.Flush();
            }
        }
        public class Tencent_cos_download
        {
            public void download(string download_dir, string key)
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
                string secretId = "***";   //"云 API 密钥 SecretId";
                string secretKey = "***";  //"云 API 密钥 SecretKey";

                long durationSecond = 1000;  // 每次请求签名有效时长，单位为秒
                QCloudCredentialProvider cosCredentialProvider = new DefaultQCloudCredentialProvider(
                    secretId, secretKey, durationSecond
                );
                // 初始化 CosXmlServer
                CosXmlServer cosXml = new CosXmlServer(config, cosCredentialProvider);

                // 创建存储桶
                try
                {
                    string bucket = "thuai6-1314234950";                              // 格式：BucketName-APPID
                    string localDir = System.IO.Path.GetDirectoryName(download_dir);  // 本地文件夹
                    string localFileName = System.IO.Path.GetFileName(download_dir);  // 指定本地保存的文件名
                    GetObjectRequest request = new GetObjectRequest(bucket, key, localDir, localFileName);

                    Dictionary<string, string> test = request.GetRequestHeaders();
                    request.SetCosProgressCallback(delegate (long completed, long total)
                    {
                        Console.WriteLine(String.Format("progress = {0:##.##}%", completed * 100.0 / total));
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
                    fst = new FileStream(strFileFullPath, FileMode.Open);
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
                finally
                {
                }
            }

            public static UpdateInfo Check()
            {
                string json, MD5, jsonName;
                int newFile = 0, updateFile = 0;
                newFileName.Clear();
                updateFileName.Clear();
                jsonName = "hash.json";
                UpdateInfo updateInfo;

                Tencent_cos_download Downloader = new Tencent_cos_download();
                try
                {
                    // 如果json存在就删了重新下
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
                    // 请求失败
                    updateInfo.status = "ClientEx: " + clientEx.ToString();
                    updateInfo.newFileCount = -1;
                    updateInfo.changedFileCount = 0;
                    return updateInfo;
                }
                catch (CosServerException serverEx)
                {
                    // 请求失败
                    updateInfo.status = "ServerEx: " + serverEx.ToString();
                    updateInfo.newFileCount = -1;
                    updateInfo.changedFileCount = 0;
                    return updateInfo;
                }

                using (StreamReader r = new StreamReader(System.IO.Path.Combine(Data.FilePath, jsonName)))
                    json = r.ReadToEnd();
                json = json.Replace("\r", string.Empty).Replace("\n", string.Empty);
                Dictionary<string, string> jsonDict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                foreach (KeyValuePair<string, string> pair in jsonDict)
                {
                    MD5 = GetFileMd5Hash(System.IO.Path.Combine(Data.FilePath, pair.Key));
                    if (MD5.Length == 0)  // 文档不存在
                        newFileName.Add(pair.Key);
                    else if (MD5 != pair.Value)  // MD5不匹配
                        updateFileName.Add(pair.Key);
                }

                newFile = newFileName.Count;
                updateFile = updateFileName.Count;
                filenum = newFile + updateFile;
                //Console.WriteLine("----------------------" + Environment.NewLine);

                if (newFile + updateFile == 0)
                {
                    updateInfo.status = "latest";
                    updateInfo.newFileCount = 0;
                    updateInfo.changedFileCount = 0;
                    newFileName.Clear();
                    updateFileName.Clear();
                }
                else
                {
                    updateInfo.status = "old";
                    //TODO:获取版本号
                    updateInfo.newFileCount = newFile;
                    /*
                    foreach (string filename in newFileName)
                    {
                        Console.WriteLine(filename);
                    }
                    */
                    updateInfo.changedFileCount = updateFile;
                    /*
                    foreach (string filename in updateFileName)
                    {
                        Console.WriteLine(filename);
                    }
                    Console.Write(Environment.NewLine + "是否下载新文件？ y/n：");
                    if (Console.Read() != 'y')
                        Console.WriteLine("下载取消!");
                    else
                        Download();
                    */
                    UpdatePlanned = true;
                }
                return updateInfo;
            }

            public static bool Update()
            {
                if (UpdatePlanned)
                {
                    Download();
                    UpdatePlanned = false;
                    return true;
                }
                return false;
            }

            private static void Download()
            {
                Tencent_cos_download Downloader = new Tencent_cos_download();

                int newFile = 0, updateFile = 0;
                int totalnew = newFileName.Count, totalupdate = updateFileName.Count;
                filenum = totalnew + totalupdate;

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
                        // 请求失败
                        Console.WriteLine("CosClientException: " + clientEx.ToString() + Environment.NewLine);
                        return;
                    }
                    catch (CosServerException serverEx)
                    {
                        // 请求失败
                        Console.WriteLine("CosClientException: " + serverEx.ToString() + Environment.NewLine);
                        return;
                    }
                    catch (Exception)
                    {
                        throw;
                    }
                }
                else
                    Console.WriteLine("当前平台已是最新版本！" + Environment.NewLine);
                newFileName.Clear();
                updateFileName.Clear();
            }

            public static bool CheckAlreadyDownload()  // 检查是否已经下载
            {
                string existpath = System.IO.Path.Combine(Data.dataPath, "THUAI6.json");
                if (!File.Exists(existpath))  // 文件不存在
                {
                    using FileStream fs = new FileStream(existpath, FileMode.Create, FileAccess.ReadWrite);
                    return false;
                }
                else  // 文件存在
                {
                    using FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.Read);
                    using StreamReader sr = new StreamReader(fs);
                    string json = sr.ReadToEnd();
                    if (json == null || json == "")
                    {
                        json += @"{""THUAI6""" + ":" + @"""2023""}";
                    }
                    var dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                    if (dict == null || !dict.ContainsKey("download") || "false" == dict["download"])
                    {
                        return false;
                    }
                    else if (dict["download"] == "true")
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            public static void DownloadAll()  // 下载全部文件
            {
                string jsonName = "hash.json";
                string json;
                Tencent_cos_download Downloader = new Tencent_cos_download();

                try
                {
                    // 如果json存在就删了重新下
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
                    // 请求失败
                    Console.WriteLine("CosClientException: " + clientEx.ToString() + Environment.NewLine);
                    return;
                }
                catch (CosServerException serverEx)
                {
                    // 请求失败
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

                string json2;
                Dictionary<string, string> dict = new Dictionary<string, string>();
                string existpath = System.IO.Path.Combine(Data.dataPath, "THUAI6.json");
                using FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                using (StreamReader r = new StreamReader(fs))
                {
                    json2 = r.ReadToEnd();
                    if (json2 == null || json2 == "")
                    {
                        json2 += @"{""THUAI6""" + ":" + @"""2023""}";
                    }
                    dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json2);
                    if (dict == null || !dict.ContainsKey("download"))
                    {
                        dict?.Add("download", "true");
                    }
                    else
                    {
                        dict["download"] = "true";
                    }
                }

                using FileStream fs2 = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                using StreamWriter sw = new StreamWriter(fs2);
                fs2.SetLength(0);
                sw.Write(JsonConvert.SerializeObject(dict));
            }

            public static void Change_all_hash(string topDir, Dictionary<string, string> jsonDict)  // 更改HASH
            {
                DirectoryInfo theFolder = new DirectoryInfo(@topDir);
                bool ifexist = false;

                // 遍历文件
                foreach (FileInfo NextFile in theFolder.GetFiles())
                {
                    string filepath = topDir + @"/" + NextFile.Name;  // 文件路径
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
                    if (!ifexist && NextFile.Name != "hash.json")
                    {
                        string MD5 = GetFileMd5Hash(filepath);
                        string relapath = filepath.Replace(Data.FilePath + '/', string.Empty);
                        jsonDict.Add(relapath, MD5);
                    }
                    ifexist = false;
                }

                // 遍历文件夹
                foreach (DirectoryInfo NextFolder in theFolder.GetDirectories())
                {
                    if (System.IO.Path.Equals(NextFolder.FullName, System.IO.Path.GetFullPath(System.IO.Path.Combine(Data.FilePath, playerFolder))))
                    {
                        foreach (FileInfo NextFile in NextFolder.GetFiles())
                        {
                            if (NextFile.Name == "README.md")
                            {
                                string MD5 = GetFileMd5Hash(NextFile.FullName);
                                string relapath = NextFile.FullName.Replace('\\', '/').Replace(Data.FilePath + '/', string.Empty);
                                jsonDict.Add(relapath, MD5);
                            }
                        }
                        continue;  // 如果是选手文件夹就忽略
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

            public static int DeleteAll()
            {
                DirectoryInfo di = new DirectoryInfo(Data.FilePath);
                DirectoryInfo player = new DirectoryInfo(System.IO.Path.GetFullPath(System.IO.Path.Combine(Data.FilePath, playerFolder)));
                try
                {
                    foreach (FileInfo file in di.GetFiles())
                    {
                        file.Delete();
                    }
                    foreach (FileInfo file in player.GetFiles())
                    {
                        if (file.Name == "README.md")
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
                    return -2;
                }
                catch (DirectoryNotFoundException)
                {
                    Console.WriteLine("文件夹没有找到，请检查是否已经手动更改路径");
                    return -3;
                }
                catch (IOException)
                {
                    Console.WriteLine("文件已经打开，请关闭后再删除");
                    return -1;
                }

                string json2;
                Dictionary<string, string> dict = new Dictionary<string, string>();
                string existpath = System.IO.Path.Combine(Data.dataPath, "THUAI6.json");
                using FileStream fs = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                using (StreamReader r = new StreamReader(fs))
                {
                    json2 = r.ReadToEnd();
                    if (json2 == null || json2 == "")
                    {
                        json2 += @"{""THUAI6""" + ":" + @"""2023""}";
                    }
                    dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json2);
                    if (dict == null || !dict.ContainsKey("download"))
                    {
                        dict?.Add("download", "false");
                    }
                    else
                    {
                        dict["download"] = "false";
                    }
                }
                using FileStream fs2 = new FileStream(existpath, FileMode.Open, FileAccess.ReadWrite);
                using StreamWriter sw = new StreamWriter(fs2);
                fs2.SetLength(0);
                sw.Write(JsonConvert.SerializeObject(dict));

                try
                {
                    File.Delete(Data.path);
                }
                catch (UnauthorizedAccessException)
                {
                    Console.WriteLine("权限不足，无法删除！");
                    return -2;
                }
                catch (DirectoryNotFoundException)
                {
                    Console.WriteLine("文件夹没有找到，请检查是否已经手动更改路径");
                    return -3;
                }
                catch (IOException)
                {
                    Console.WriteLine("文件已经打开，请关闭后再删除");
                    return -1;
                }
                Console.WriteLine($"删除成功！player文件夹中的文件已经放在{ProgramName}的根目录下");
                return 0;
            }

            public static void OverwriteHash(Dictionary<string, string> jsonDict)
            {
                string Contentjson = JsonConvert.SerializeObject(jsonDict);
                Contentjson = Contentjson.Replace("\r", String.Empty).Replace("\n", String.Empty).Replace(@"\\", "/");
                File.WriteAllText(@System.IO.Path.Combine(Data.FilePath, "hash.json"), Contentjson);
            }

            public static int MoveProgram(string newPath)
            {
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
                    return -2;
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
                    return -1;
                }
                Data.ResetFilepath(newPath);
                Console.WriteLine("更改路径成功!");
                return 0;
            }
            public static async Task main(string[] args)
            {
                var client = new HttpClient();
                var web = new WebConnect.Web();
                Data date = new Data("");
                while (true)
                {
                    Console.WriteLine($"1. 更新hash.json   2. 检查更新   3.下载{ProgramName}  4.删除{ProgramName}  5.启动进程  6.移动{ProgramName}到其它路径");
                    string choose = Console.ReadLine();
                    if (choose == "1")
                    {
                        if (!CheckAlreadyDownload())
                        {
                            Console.WriteLine($"未下载{ProgramName}，请先执行下载操作！");
                            continue;
                        }
                        UpdateHash();
                    }
                    else if (choose == "2")
                    {
                        if (!CheckAlreadyDownload())
                        {
                            Console.WriteLine($"未下载{ProgramName}，请先执行下载操作！");
                            continue;
                        }
                        while (true)
                        {
                            if (Data.FilePath != null && Directory.Exists(Data.FilePath))
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
                    else if (choose == "3")
                    {
                        if (CheckAlreadyDownload())
                        {
                            Console.WriteLine($"已经将{ProgramName}下载到{Data.FilePath}！若要重新下载请先完成删除操作！");
                        }
                        else
                        {
                            string newpath;
                            Console.WriteLine("请输入下载路径：");
                            newpath = Console.ReadLine();
                            Data.ResetFilepath(newpath);
                            DownloadAll();
                        }
                    }
                    else if (choose == "4")
                    {
                        DeleteAll();
                    }
                    else if (choose == "5")
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
                    else if (choose == "6")
                    {
                        string newPath;
                        newPath = Console.ReadLine();
                        MoveProgram(newPath);
                    }
                    else if (choose == "7")
                    {
                        Console.WriteLine("请输入email：");
                        string username = Console.ReadLine();
                        Console.WriteLine("请输入密码：");
                        string password = Console.ReadLine();

                        await web.LoginToEEsast(client, username, password);
                    }
                    else if (choose == "8")
                    {
                        await web.UserDetails(client);
                    }
                    else if (choose == "9")
                    {
                        await web.UploadFiles(client);
                    }
                    else if (choose == "exit")
                    {
                        return;
                    }
                }
            }
        }
    }
}

namespace WebConnect
{
    class Web
    {
        public static string logintoken = "";
        async public Task LoginToEEsast(HttpClient client, string useremail, string password)
        {
            string token = "";
            using (var response = await client.PostAsync("https://api.eesast.com/users/login", JsonContent.Create(new
            {
                email = useremail,
                password = password,
            })))
            {
                switch (response.StatusCode)
                {
                    case System.Net.HttpStatusCode.OK:
                        Console.WriteLine("Success login");
                        token = (System.Text.Json.JsonSerializer.Deserialize(await response.Content.ReadAsStreamAsync(), typeof(LoginResponse), new JsonSerializerOptions()
                        {
                            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
                        }) as LoginResponse)
                                    ?.Token ??
                                throw new Exception("no token!");
                        logintoken = token;
                        SaveToken();
                        break;

                    default:
                        int code = ((int)response.StatusCode);
                        Console.WriteLine(code);
                        if (code == 401)
                        {
                            Console.WriteLine("邮箱或密码错误！");
                        }
                        return;
                }
            }
        }
        async public Task UploadFiles(HttpClient client)    //用来上传文件
        {
            if (!ReadToken())   //读取token失败
            {
                return;
            }
            try
            {
                string tarfile; //要上传的文件路径
                string content;
                string filedest;    //文件目的地
                client.DefaultRequestHeaders.Authorization = new("Bearer", logintoken);
                Console.WriteLine("请输入要上传的文件完整路径：");
                tarfile = Console.ReadLine();
                if (!File.Exists(tarfile))
                {
                    Console.WriteLine("文件不存在！");
                    return;
                }
                Console.WriteLine("要将文件上传到何处？：");
                filedest = Console.ReadLine();
                using FileStream fs = new FileStream(tarfile, FileMode.Open, FileAccess.Read);
                using StreamReader sr = new StreamReader(fs);
                content = sr.ReadToEnd();
                using (var response = await client.PostAsync("https://api.eesast.com/files/upload", JsonContent.Create(new
                {
                    file = content,
                    dest = filedest
                })))
                {
                    switch (response.StatusCode)
                    {
                        case System.Net.HttpStatusCode.OK:
                            Console.WriteLine("上传成功！");
                            break;
                        case System.Net.HttpStatusCode.Unauthorized:
                            Console.WriteLine("您未登录或登录过期，请先登录");
                            break;
                        default:
                            Console.WriteLine("上传失败！");
                            break;
                    }
                }
            }
            catch (IOException)
            {
                Console.WriteLine("文件读取错误！请检查文件是否被其它应用占用！");
            }
            catch
            {
                Console.WriteLine("请求错误！请检查网络连接！");
            }
        }

        async public Task UserDetails(HttpClient client)  // 用来测试访问网站
        {
            if (!ReadToken())  // 读取token失败
            {
                return;
            }
            try
            {
                client.DefaultRequestHeaders.Authorization = new("Bearer", logintoken);
                Console.WriteLine(logintoken);
                using (var response = await client.GetAsync("https://api.eesast.com/application/info"))  // JsonContent.Create(new
                                                                                                         //{

                //})))
                {
                    switch (response.StatusCode)
                    {
                        case System.Net.HttpStatusCode.OK:
                            Console.WriteLine("Require OK");
                            Console.WriteLine(await response.Content.ReadAsStringAsync());
                            break;
                        default:
                            int code = ((int)response.StatusCode);
                            if (code == 401)
                            {
                                Console.WriteLine("您未登录或登录过期，请先登录");
                            }
                            return;
                    }
                }
            }
            catch
            {
                Console.WriteLine("请求错误！请检查网络连接！");
            }
        }

        public void SaveToken()  // 保存token
        {
            string savepath = System.IO.Path.Combine(Data.dataPath, "THUAI6.json");
            try
            {
                string json;
                Dictionary<string, string> dict = new Dictionary<string, string>();
                using FileStream fs = new FileStream(savepath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
                using (StreamReader r = new StreamReader(fs))
                {
                    json = r.ReadToEnd();
                    if (json == null || json == "")
                    {
                        json += @"{""THUAI6""" + ":" + @"""2023""}";
                    }
                    dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                    dict?.Add("token", logintoken);
                }
                using FileStream fs2 = new FileStream(savepath, FileMode.OpenOrCreate, FileAccess.ReadWrite);
                using StreamWriter sw = new StreamWriter(fs2);
                fs2.SetLength(0);
                sw.Write(JsonConvert.SerializeObject(dict));   //将token写入文件
            }
            catch (DirectoryNotFoundException)
            {
                Console.WriteLine("保存token时未找到下载器地址！请检查下载器是否被移动！");
            }
            catch (PathTooLongException)
            {
                Console.WriteLine("下载器的路径名太长！请尝试移动下载器！");
            }
            catch (ArgumentNullException)
            {
                Console.WriteLine("下载器路径初始化失败！");
            }
            catch (IOException)
            {
                Console.WriteLine("写入token.dat发生冲突！请检查token.dat是否被其它程序占用！");
            }
        }
        public bool ReadToken()  // 读取token
        {
            try
            {
                string json;
                Dictionary<string, string> dict = new Dictionary<string, string>();
                string savepath = System.IO.Path.Combine(Data.dataPath, "THUAI6.json");
                using FileStream fs = new FileStream(savepath, FileMode.Open, FileAccess.Read);
                using StreamReader sr = new StreamReader(fs);

                json = sr.ReadToEnd();
                if (json == null || json == "")
                {
                    json += @"{""THUAI6""" + ":" + @"""2023""}";
                }
                dict = JsonConvert.DeserializeObject<Dictionary<string, string>>(json);
                if (!dict.ContainsKey("token"))
                {
                    return false;
                }
                else
                {
                    logintoken = dict["token"];
                    return true;
                }
            }
            catch (DirectoryNotFoundException)
            {
                Console.WriteLine("读取token时未找到下载器地址！请检查下载器是否被移动！");
                return false;
            }
            catch (FileNotFoundException)
            {
                //没有登陆
                Console.WriteLine("请先登录！");
                return false;
            }
            catch (PathTooLongException)
            {
                Console.WriteLine("下载器的路径名太长！请尝试移动下载器！");
                return false;
            }
            catch (ArgumentNullException)
            {
                Console.WriteLine("下载器路径初始化失败！");
                return false;
            }
            catch (IOException)
            {
                Console.WriteLine("写入token.dat发生冲突！请检查token.dat是否被其它程序占用！");
                return false;
            }
        }
    }
    [Serializable]
    record LoginResponse
    {
        // Map `Token` to `token` when serializing

        public string Token { get; set; } = "";
    }

}
