using System;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface ICharacter : IGameObj
    {
        public long TeamID { get; }
        public int HP { get; set; }
        public int Score { get; }
        public void AddScore(int add);
        public double Vampire { get; }
        public PlayerStateType PlayerState { get; set; }
        public BulletType BulletOfPlayer { get; set; }

        public bool IsGhost();
    }
}