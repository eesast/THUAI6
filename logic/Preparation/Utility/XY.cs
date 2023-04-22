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
            if (Direction.x == 0 && Direction.y == 0)
            {
                this.x = 0;
                this.y = 0;
            }
            else
            {
                this.x = (int)(length * Math.Cos(Direction.Angle()));
                this.y = (int)(length * Math.Sin(Direction.Angle()));
            }
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
        public static bool operator ==(XY v1, XY v2)
        {
            return v1.x == v2.x && v1.y == v2.y;
        }
        public static bool operator !=(XY v1, XY v2)
        {
            return v1.x != v2.x || v1.y != v2.y;
        }
        public static double Distance(XY p1, XY p2)
        {
            long c = (((long)(p1.x - p2.x) * (p1.x - p2.x)) + ((long)(p1.y - p2.y) * (p1.y - p2.y)))*1000000;
            long t = c/2+1;
            while (t*t>c||(t+1)*(t+1)<c)
                t=(c/t+t)/2;
            return (double)t/1000.0;
        }
        public double Length()
        {
            return Math.Sqrt(((long)x * x) + ((long)y * y));
        }
        public double Angle()
        {
            return Math.Atan2(y, x);
        }

        public override bool Equals(object? obj) => obj is null || obj is XY ? false : this == (XY)obj;

        public override int GetHashCode()
        {
            return this.x.GetHashCode() ^ this.y.GetHashCode();
        }
    }
}
