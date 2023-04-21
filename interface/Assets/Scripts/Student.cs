using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Grpc.Core;
using Google.Protobuf;
using Protobuf;
using System.Data;

public class Student : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        num = studentCount;
        studentCount++;
        anim = GetComponent<Animator>();
        occupation = MessageReceiver.Student[num].StudentType;
        transform.position = lastPosition = new Vector3(MessageReceiver.Student[num].Y / 1000.0f,
            50.0f - MessageReceiver.Student[num].X / 1000.0f,
            50.0f - MessageReceiver.Student[num].X / 1000.0f - 0.5f);
    }

    // Update is called once per frame
    void Update()
    {
        var currentPosition = new Vector3(MessageReceiver.Student[num].Y / 1000.0f, 
            50.0f - MessageReceiver.Student[num].X / 1000.0f, 
            50.0f - MessageReceiver.Student[num].X / 1000.0f - 0.5f);
        Vector3 step = currentPosition - lastPosition;
        if (step != Vector3.zero)
        {
            transform.position = currentPosition;
            anim.SetBool("isRun", true);
        }
        else
        {
            anim.SetBool("isRun", false);
        }
        lastPosition = currentPosition;
    }
    private int num;
    private StudentType occupation;
    private Vector3 lastPosition = new Vector3(0, 0, 10.0f);
    private static int studentCount = 0;
    private Animator anim;
}
