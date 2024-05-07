﻿using System.Collections.Generic;
using GameClass.GameObj;
using System.Threading;
using Preparation.Utility;
using System;
using Timothy.FrameRateTask;
using GameEngine;

namespace Gaming
{
    public partial class Game
    {
        private readonly PropManager propManager;
        private class PropManager
        {
            private readonly Map gameMap;
            private readonly CharacterManager characterManager;
            private readonly List<XY> availableCellForGenerateProp;

            public void ConsumeProp(Character player, PropType propType)
            {
                if (player.CharacterType == CharacterType.Robot || player.IsRemoved)
                    return;
                Gadget prop = player.ConsumeProp(propType);
                switch (prop.GetPropType())
                {
                    case PropType.ShieldOrSpear:
                        if (player.IsGhost())
                            player.AddSpear(GameData.PropDuration);
                        else player.AddShield(GameData.PropDuration);
                        break;
                    case PropType.AddLifeOrClairaudience:
                        if (!player.IsGhost())
                            player.AddLife(GameData.PropDuration);
                        else
                        {
                            player.AddScore(GameData.ScorePropClairaudience);
                            player.AddClairaudience(GameData.PropDuration);
                        }
                        break;
                    case PropType.AddSpeed:
                        player.AddScore(GameData.ScorePropAddSpeed);
                        player.AddMoveSpeed(GameData.PropDuration, 2.0);
                        break;
                    case PropType.AddHpOrAp:
                        if (!player.IsGhost())
                            if (player.HP < player.HP.GetMaxV())
                            {
                                player.HP.AddPositiveV(GameData.basicTreatmentDegree / 2);
                                player.AddScore(GameData.ScorePropAddHp);
                            }
                            else player.AddAp(GameData.PropDuration);
                        break;
                    case PropType.RecoveryFromDizziness:
                        if (player.PlayerState == PlayerStateType.Stunned || player.PlayerState == PlayerStateType.Charmed)
                        {
                            player.AddScore(GameData.ScorePropRecoverFromDizziness);
                            player.SetPlayerStateNaturally();
                        }
                        break;
                    default:
                        break;
                }
            }

            /// <summary>
            ///
            /// </summary>
            /// <param name="player"></param>
            /// <param name="propType">若不指定，则自动判断可捡起什么道具</param>
            /// <returns></returns>
            public bool PickProp(Character player, PropType propType = PropType.Null)
            {
                if (!player.Commandable()) return false;
                int indexing = player.IndexingOfAddProp();
                if (indexing == GameData.maxNumOfPropInPropInventory)
                    return false;

                Gadget pickProp = new NullProp();
                if (propType == PropType.Null)  // 自动检查有无道具可捡
                {
                    pickProp = player.PropInventory[indexing] = ((Gadget?)gameMap.OneInTheSameCell(player.Position, GameObjType.Gadget)) ?? new NullProp();
                }
                else
                {
                    foreach (Gadget prop in gameMap.GameObjDict[GameObjType.Gadget])
                    {
                        if (prop.GetPropType() == propType)
                        {
                            if (GameData.IsInTheSameCell(prop.Position, player.Position) && prop.CanMove == false)
                            {
                                pickProp = player.PropInventory[indexing] = prop;
                            }
                        }
                    }
                }

                if (pickProp.GetPropType() != PropType.Null)
                {
                    gameMap.RemoveJustFromMap(pickProp);
                    //gameMap.Add(new Item(pickProp));
                    return true;
                }
                else
                    return false;
            }

            public void ThrowProp(Character player, PropType propType)
            {
                if (!gameMap.Timer.IsGaming || player.IsRemoved)
                    return;
                Gadget prop = player.ConsumeProp(propType);
                if (prop.GetPropType() == PropType.Null)
                    return;

                prop.ReSetPos(player.Position);
                gameMap.Add(prop);
            }

            private static Gadget ProduceOnePropNotKey(Random r, XY Pos)
            {
                return PropFactory.GetConsumables((PropType)r.Next(GameData.numOfTeachingBuilding + 1, GameData.numOfPropSpecies + 1), Pos);
            }

            private Chest GetChest(Random r)
            {
                int index = r.Next(0, GameData.numOfChest);
                while (((Chest)(gameMap.GameObjDict[GameObjType.Chest][index])).PropInChest[0].GetPropType() != PropType.Null) index = (index + 1) % GameData.numOfChest;
                return (Chest)(gameMap.GameObjDict[GameObjType.Chest][index]);
            }

            public void StartProducing()
            {
                int len = availableCellForGenerateProp.Count;
                Random r = new Random(Environment.TickCount);

                int cou = 0;
                while (cou < GameData.numOfKeyEachArea)
                {
                    ++cou;
                    Chest chest = GetChest(r);
                    chest.PropInChest[1] = new Key3(chest.Position);
                    chest.PropInChest[0] = ProduceOnePropNotKey(r, chest.Position);
                }
                cou = 0;
                while (cou < GameData.numOfKeyEachArea)
                {
                    ++cou;
                    Chest chest = GetChest(r);
                    chest.PropInChest[1] = new Key5(chest.Position);
                    chest.PropInChest[0] = ProduceOnePropNotKey(r, chest.Position);
                }
                cou = 0;
                while (cou < GameData.numOfKeyEachArea)
                {
                    ++cou;
                    Chest chest = GetChest(r);
                    chest.PropInChest[1] = new Key6(chest.Position);
                    chest.PropInChest[0] = ProduceOnePropNotKey(r, chest.Position);
                }

                foreach (Chest chest in gameMap.GameObjDict[GameObjType.Chest])
                {
                    if (chest.PropInChest[0].GetPropType() == PropType.Null)
                    {
                        chest.PropInChest[0] = ProduceOnePropNotKey(r, chest.Position);
                        chest.PropInChest[1] = ProduceOnePropNotKey(r, chest.Position);
                    }
                }
                /*
                new Thread
                (
                    () =>
                    {
                        while (!gameMap.Timer.IsGaming)
                            Thread.Sleep(1000);
                        new FrameRateTaskExecutor<int>(
                            () => gameMap.Timer.IsGaming,
                            () =>
                            {
                                int rand = r.Next(0, len);
                                XY randPos = availableCellForGenerateProp[rand];
                                gameMap.Add(ProduceOnePropNotKey(r, randPos));
                            },
                            GameData.PropProduceTime,
                            () => 0
                        )
                            .Start();
                    }
                )
                { IsBackground = true }.Start();
            */
            }
            public PropManager(Map gameMap, CharacterManager characterManager)  // 道具不能扔过墙
            {
                this.characterManager = characterManager;
                this.gameMap = gameMap;
                /*            this.moveEngine = new MoveEngine(
                                gameMap: gameMap,
                                OnCollision: (obj, collision, moveVec) =>
                                { return MoveEngine.AfterCollision.MoveMax; },
                                EndMove: obj =>
                                {
                                    // obj.Place = gameMap.GetPlaceType((GameObj)obj);
                                    obj.CanMove = false;
                                    Debugger.Output(obj, " end move at " + obj.Position.ToString() + " At time: " + Environment.TickCount64);
                                }
                            );*/
                availableCellForGenerateProp = new List<XY>();
                for (int i = 0; i < gameMap.protoGameMap.GetLength(0); i++)
                {
                    for (int j = 0; j < gameMap.protoGameMap.GetLength(1); j++)
                    {
                        if (gameMap.protoGameMap[i, j] == (int)PlaceType.Null)
                        {
                            availableCellForGenerateProp.Add(GameData.GetCellCenterPos(i, j));
                        }
                    }
                }
            }
        }
    }
}
