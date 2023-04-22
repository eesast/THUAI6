using Protobuf;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapManager : MonoBehaviour
{
    // Start is called before the first frame update
    private bool mapFinished;
    private MessageOfMap map;
    private MessageOfStudent Student;
    private int rowCount = 50;
    private int colCount = 50;

    public GameObject wall;
    public GameObject grass;
    public GameObject land; 
    public GameObject door;
    public GameObject window;
    public GameObject box;
    public GameObject book;
    public GameObject student_1;
    public GameObject student_2;
    public GameObject student_3;
    public GameObject student_4;
    public GameObject Monster_1;
    public GameObject Monster_2;
    public GameObject Monster_3;
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
            ShowMap(map);
        }

    }

    private void ShowMap(MessageOfMap map)
    {
        var position = new Vector3(-0.5f, 49.5f, 49.5f);
        var block = new GameObject();
        for (int i = 0; i < rowCount; i++)
        {
            for (int j = 0; j < colCount; j++)
            {
                position.x = position.x + 1;
                block = ShowBlock(map.Row[i].Col[j]);
                if (block != null)
                {
                    Instantiate(block, position, new Quaternion(0, 0, 0, 0));
                }
            }
            position.x = -0.5f;
            position.z=position.y = position.y - 1.0f;
        }
    }

    private GameObject ShowBlock(PlaceType obj)
    {
        switch(obj)
        {
            case PlaceType.Land:return land;
            case PlaceType.Grass:return grass;
            case PlaceType.Wall:return wall;
            case PlaceType.Door3: return door;
            case PlaceType.Door5: return door;
            case PlaceType.Door6: return door;
            case PlaceType.Window: return window;
            case PlaceType.Chest:return box;
            case PlaceType.Classroom:return book;
            default:return land;
        }
    }
}
