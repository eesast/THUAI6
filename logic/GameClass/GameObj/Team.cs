using System.Collections.Generic;

namespace GameClass.GameObj
{
    public class Team
    {
        private static long currentMaxTeamID = 0;
        public static long CurrentMaxTeamID => currentMaxTeamID;
        private readonly long teamID;
        public long TeamID => teamID;
        public const long invalidTeamID = long.MaxValue;
        public const long noneTeamID = long.MinValue;
        private readonly List<Character> playerList;
        public int Score
        {
            get {
                int score = 0;
                foreach (var player in playerList)
                    score += player.Score;
                return score;
            }
        }
        public Character? GetPlayer(long ID)
        {
            foreach (Character player in playerList)
            {
                if (player.ID == ID)
                {
                    return player;
                }
            }
            return null;
        }
        public void AddPlayer(Character player)
        {
            playerList.Add(player);
        }
        public void OutPlayer(long ID)
        {
            int i;
            for (i = 0; i < playerList.Count; ++i)
            {
                if (playerList[i].ID == ID)
                    break;
            }
            playerList.RemoveAt(i);
        }
        public long[] GetPlayerIDs()
        {
            long[] playerIDs = new long[playerList.Count];
            int num = 0;
            foreach (Character player in playerList)
            {
                playerIDs[num++] = player.ID;
            }
            return playerIDs;
        }
        public static bool teamExists(long findTeamID)
        {
            return findTeamID < currentMaxTeamID;
        }
        public Team()
        {
            teamID = currentMaxTeamID++;
            playerList = new List<Character>();
        }
    }
}
