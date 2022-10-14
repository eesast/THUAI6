using System;

namespace Preparation.Utility
{
    public struct XYPosition
    {
        public int x;
        public int y;
        public XYPosition(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public override string ToString()
        {
            return "(" + x.ToString() + "," + y.ToString() + ")";
        }
        public static XYPosition operator +(XYPosition p1, XYPosition p2)
        {
            return new XYPosition(p1.x + p2.x, p1.y + p2.y);
        }
        public static XYPosition operator -(XYPosition p1, XYPosition p2)
        {
            return new XYPosition(p1.x - p2.x, p1.y - p2.y);
        }
        public static double Distance(XYPosition p1, XYPosition p2)
        {
            return Math.Sqrt(((long)(p1.x - p2.x) * (p1.x - p2.x)) + ((long)(p1.y - p2.y) * (p1.y - p2.y)));
        }
        /*public static XYPosition[] GetSquareRange(uint edgeLen) // 从THUAI4的BULLET.CS移植而来，不知还有用否
        {
            XYPosition[] range = new XYPosition[edgeLen * edgeLen];
            int offset = (int)(edgeLen >> 1);
            for (int i = 0; i < (int)edgeLen; ++i)
            {
                for (int j = 0; j < (int)edgeLen; ++j)
                {
                    range[i * edgeLen + j].x = i - offset;
                    range[i * edgeLen + j].y = j - offset;
                }
            }
            return range;
        }*/
        public Vector2 ToVector2()
        {
            return new Vector2(this.x, this.y);
        }
    }
}
