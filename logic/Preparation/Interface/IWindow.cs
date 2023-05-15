using Preparation.Utility;

namespace Preparation.Interface
{
    public interface IWindow : IGameObj
    {
        public void Enter2Stage(XY xy);
        public bool TryToClimb(ICharacter character);
        public XY Stage { get; }
        public void FinishClimbing();
    }
}