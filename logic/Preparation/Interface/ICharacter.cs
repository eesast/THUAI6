using System;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface ICharacter : IMoveable
    {
        public long TeamID { get; }
        public int HP { get; set; }
        public long Score { get; }
        public void AddScore(long add);
        public double Vampire { get; }
        public PlayerStateType PlayerState { get; }
        public BulletType BulletOfPlayer { get; set; }
        public CharacterType CharacterType { get; }
        public ActiveSkill FindActiveSkill(ActiveSkillType activeSkillType);
        public int UpdateBulletNum(int time);
        public long SetPlayerState(PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null);

        public bool IsGhost();
    }
    public interface IStudent : ICharacter { }
    public interface IGolem : IStudent { }
}