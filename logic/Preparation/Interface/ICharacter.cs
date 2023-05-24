﻿using System;
using Preparation.Utility;

namespace Preparation.Interface
{
    public interface ICharacter : IMoveable
    {
        public long TeamID { get; }
        public long HP { get; set; }
        public long Score { get; }
        public void AddScore(long add);
        public double Vampire { get; }
        public PlayerStateType PlayerState { get; }
        public BulletType BulletOfPlayer { get; set; }
        public CharacterType CharacterType { get; }
        public ActiveSkill FindActiveSkill(ActiveSkillType activeSkillType);
        public int UpdateBulletNum(int time);
        public long SetPlayerState(RunningStateType running, PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null);
        public bool ResetPlayerState(long state);

        public bool IsGhost();
    }
    public interface IStudent : ICharacter { }
    public interface IGolem : IStudent { }
}