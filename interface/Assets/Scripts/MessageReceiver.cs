using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Grpc.Core;
using Google.Protobuf;
using Protobuf;
using System.Data;
using UnityEditor.U2D.Path;

public class MessageReceiver : MonoBehaviour
{
    private bool isMap;
    // Start is called before the first frame update
    async void Start()
    {
        var channel = new Channel("127.0.0.1:8888", ChannelCredentials.Insecure);
        var client = new AvailableService.AvailableServiceClient(channel);
        PlayerMsg msg = new PlayerMsg();
        msg.PlayerType = PlayerType.StudentPlayer;
        msg.PlayerId = 0;
        isMap = true;
        map = null;
        var response = client.AddPlayer(msg);
        while (await response.ResponseStream.MoveNext())
        {
            var responseVal = response.ResponseStream.Current;
            if (isMap)
            {
                map = responseVal.ObjMessage[0].MapMessage;
                isMap = false;
            }
        }
    }
    // Update is called once per frame
    void Update()
    {
    }

    public static MessageOfMap map;
}
