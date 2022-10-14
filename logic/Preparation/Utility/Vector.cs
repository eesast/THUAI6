using System;

namespace Preparation.Utility
{
    public struct Vector
    {
        public double angle;
        public double length;

        public static XYPosition VectorToXY(Vector v)
        {
            return new XYPosition((int)(v.length * Math.Cos(v.angle)), (int)(v.length * Math.Sin(v.angle)));
        }
        public Vector2 ToVector2()
        {
            return new Vector2((int)(this.length * Math.Cos(this.angle)), (int)(this.length * Math.Sin(this.angle)));
        }
        public static Vector XYToVector(double x, double y)
        {
            return new Vector(Math.Atan2(y, x), Math.Sqrt((x * x) + (y * y)));
        }
        public Vector(double angle, double length)
        {
            if (length < 0)
            {
                angle += Math.PI;
                length = -length;
            }
            this.angle = Tools.CorrectAngle(angle);
            this.length = length;
        }
    }

    public struct Vector2
    {
        public double x;
        public double y;
        public Vector2(double x, double y)
        {
            this.x = x;
            this.y = y;
        }

        public static double operator *(Vector2 v1, Vector2 v2)
        {
            return (v1.x * v2.x) + (v1.y * v2.y);
        }
        public static Vector2 operator +(Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.x + v2.x, v1.y + v2.y);
        }
        public static Vector2 operator -(Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.x - v2.x, v1.y - v2.y);
        }
    }
}
