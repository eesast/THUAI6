using System;

namespace Preparation.Utility
{
   
    public class Vector
    {
        public int x;
        public int y;
        public Vector(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public override string ToString()
        {
            return "(" + x.ToString() + "," + y.ToString() + ")";
        }
        public static int operator*(Vector v1, Vector v2)
        {
            return (v1.x * v2.x) + (v1.y * v2.y);
        }
        public static Vector operator +(Vector v1, Vector v2)
        {
            return new Vector(v1.x + v2.x, v1.y + v2.y);
        }
        public static Vector operator -(Vector v1, Vector v2)
        {
            return new Vector(v1.x - v2.x, v1.y - v2.y);
        }
    }
    public class XYPosition: Vector
    {
        public XYPosition(int x, int y):base(x,y){}
       
        public static double Distance(XYPosition p1, XYPosition p2)
        {
            return Math.Sqrt(((long)(p1.x - p2.x) * (p1.x - p2.x)) + ((long)(p1.y - p2.y) * (p1.y - p2.y)));
        } 
    }
}
