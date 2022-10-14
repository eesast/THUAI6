using System;

namespace Preparation.Utility
{
    public static class Tools
    {
        public static double CorrectAngle(double angle)  // 将幅角转化为主值0~2pi
        {
            if (double.IsNaN(angle) || double.IsInfinity(angle))
            {
                return 0.0;
            }
            while (angle < 0)
                angle += 2 * Math.PI;
            while (angle >= 2 * Math.PI)
                angle -= 2 * Math.PI;
            return angle;
        }
    }
}
