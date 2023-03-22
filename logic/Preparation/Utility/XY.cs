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
        public XY(double angle, double length)
        {
            this.x = (int)(length * Math.Cos(angle));
            this.y = (int)(length * Math.Sin(angle));
        }
        public XY(XY Direction, double length)
        {
            this.x = (int)(length * Math.Cos(Direction.Angle()));
            this.y = (int)(length * Math.Sin(Direction.Angle()));
        }
        public override string ToString()
        {
            return "(" + x.ToString() + "," + y.ToString() + ")";
        }
        public static int operator *(XY v1, XY v2)
        {
            return (v1.x * v2.x) + (v1.y * v2.y);
        }
        public static XY operator *(int a, XY v2)
        {
            return new XY(a * v2.x, a * v2.y);
        }
        public static XY operator *(XY v2, int a)
        {
            return new XY(a * v2.x, a * v2.y);
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
        public double Length()
        {
            return Math.Sqrt(((long)x * x) + ((long)y * y));
        }
        public double Angle()
        {
            return Math.Atan2(y, x);
        }
    }
}
