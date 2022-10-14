﻿using System;

namespace Preparation.Utility
{
    public class Debugger
    {
        static public void Output(object current, string str)
        {
#if DEBUG 
            Console.WriteLine(current.GetType() + " " + current.ToString() + " " + str);
#endif
        }
    }
}
