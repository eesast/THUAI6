using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Grpc.Core;
using Google.Protobuf;
using Protobuf;
using System.Data;
//using UnityEditor.U2D.Path;

public class MessageReceiver : MonoBehaviour
{
    private static int studentNum = 2;
    // Start is called before the first frame update
    async void Start()
    {
        var channel = new Channel("127.0.0.1:8888", ChannelCredentials.Insecure);
        var client = new AvailableService.AvailableServiceClient(channel);
        PlayerMsg msg = new PlayerMsg();
        msg.PlayerType = PlayerType.StudentPlayer;
        msg.PlayerId = 3000;
        map = null;
        var response = client.AddPlayer(msg);
        //while (await response.ResponseStream.MoveNext())
        //{
        //    var responseVal = response.ResponseStream.Current;
        //    if (isMap)
        //    {
        //        map = responseVal.ObjMessage[0].MapMessage;
        //        //Debug.Log(map.ToString());
        //        isMap = false;
        //        for (int i = 0; i < studentNum; i++)
        //            Instantiate(student_1, new Vector3(0f, 0f, 10.0f), new Quaternion(0, 0, 0, 0));
        //    }
        //    else
        //    {
        //        for (int i = 0; i < studentNum; i++)
        //        {
        //            Student[i] = responseVal.ObjMessage[i].StudentMessage;
        //        }


        //    }
        //}
        if(await response.ResponseStream.MoveNext())
        {
            var responseVal = response.ResponseStream.Current;
            map = responseVal.ObjMessage[0].MapMessage;
        }
        if (await response.ResponseStream.MoveNext())
        {
            var responseVal = response.ResponseStream.Current;
            for (int i = 0; i < studentNum; i++)
            {
                Student[i] = responseVal.ObjMessage[i].StudentMessage;
                Instantiate(student_1, new Vector3(0f, 0f, 10.0f), new Quaternion(0, 0, 0, 0));
            }
        }
        while(await response.ResponseStream.MoveNext())
        {
            var responseVal = response.ResponseStream.Current;
            for(int i=0;i<studentNum;i++)
            {
                Student[i] = responseVal.ObjMessage[i].StudentMessage;
            }
        }
    }
    // Update is called once per frame
    void Update()
    {
    }
    public static MessageOfMap map;
    public static MessageOfStudent[] Student = new MessageOfStudent[studentNum];
    public GameObject student_1;
}
