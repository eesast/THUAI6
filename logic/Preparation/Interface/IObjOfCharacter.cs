using System;

namespace Preparation.Interface
{
    public interface IObjOfCharacter : IGameObj
    {
        ICharacter? Parent { get; set; }
    }
}
