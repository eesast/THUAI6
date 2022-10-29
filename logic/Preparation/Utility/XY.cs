using System;

namespace Preparation.Utility
{

    public struct XY
    {
        public int x;
        public int y;
        public XY(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public override string ToString()
        {
            return "(" + x.ToString() + "," + y.ToString() + ")";
        }
        public static int operator *(XY v1, XY v2)
        {
            return (v1.x * v2.x) + (v1.y * v2.y);
        }
        public static XY operator +(XY v1, XY v2)
        {
            return new XY(v1.x + v2.x, v1.y + v2.y);
        }
        public static XY operator -(XY v1, XY v2)
        {
            return new XY(v1.x - v2.x, v1.y - v2.y);
        }
        public static double Distance(XY p1, XY p2)
        {
            return Math.Sqrt(((long)(p1.x - p2.x) * (p1.x - p2.x)) + ((long)(p1.y - p2.y) * (p1.y - p2.y)));
        }
    }
}
