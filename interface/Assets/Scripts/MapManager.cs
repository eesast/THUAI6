using Protobuf;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapManager : MonoBehaviour
{
    // Start is called before the first frame update
    private bool mapFinished;
    private MessageOfMap map;
    void Start()
    {
        mapFinished = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!mapFinished && MessageReceiver.map != null)
        {
            map = MessageReceiver.map;
            //Debug.Log("valid map");
            mapFinished = true;
        }
    }
}
