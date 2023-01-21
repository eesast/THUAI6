using System;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface ICharacter : IGameObj
    {
        public long TeamID { get; }
        public int HP { get; set; }
        public double Vampire { get; }
    }
}