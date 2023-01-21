using System;

namespace Preparation.Utility
{
    public class MapEncoder
    {
        static public char Dec2Hex(int d)
        {
            return char.Parse(d.ToString("X"));
        }
        static public int Hex2Dec(char h)
        {
            string hexabet = "0123456789ABCDEF";
            return hexabet.IndexOf(h);
        }
    }
}
