using Newtonsoft.Json.Linq;
using System;
using System.Net;
using System.Text;

namespace Server
{
    class HttpSender
    {
        private string url;
        private string token;
        private string method;
        public HttpSender(string url, string token, string method)
        {
            this.url = url;
            this.token = token;
            this.method = method;
        }
        public void SendHttpRequest(JObject body)
        {
            try
            {
                var request = WebRequest.CreateHttp(url);
                request.Method = method;
                request.Headers.Add("Authorization", $"Bearer {token}");

                request.ContentType = "application/json";
                var raw = Encoding.UTF8.GetBytes(body.ToString());
                request.GetRequestStream().Write(raw, 0, raw.Length);

                Console.WriteLine("Send to web successfully!");
                var response = request.GetResponse();
                Console.WriteLine($"Web response: {response}");

            }
            catch (Exception e)
            {
                Console.WriteLine("Fail to send msg to web!");
                Console.WriteLine(e);
            }
        }
    }
}
