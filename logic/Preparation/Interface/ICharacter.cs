using Preparation.Utility;

namespace Preparation.Interface
{
    public interface ICharacter : IMoveable
    {
        public AtomicLong TeamID { get; }
        public LongInTheVariableRange HP { get; }
        public long Score { get; }
        public void AddScore(long add);
        public DoubleInTheVariableRange Vampire { get; }
        public PlayerStateType PlayerState { get; }
        public BulletType BulletOfPlayer { get; set; }
        public CharacterType CharacterType { get; }
        public ActiveSkill FindActiveSkill(ActiveSkillType activeSkillType);
        public IntNumUpdateEachCD BulletNum { get; }
        public long SetPlayerState(RunningStateType running, PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null);
        public bool ResetPlayerState(long state, RunningStateType running = RunningStateType.Null, PlayerStateType value = PlayerStateType.Null, IGameObj? obj = null);

        public bool IsGhost();
    }
    public interface IStudent : ICharacter { }
    public interface IGolem : IStudent { }
}