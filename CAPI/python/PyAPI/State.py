from typing import List, Union
import PyAPI.structures as THUAI6


class State:
    teamScore: int
    self: Union[THUAI6.Student, THUAI6.Tricker]

    students: List[THUAI6.Student]
    trickers: List[THUAI6.Tricker]

    props: List[THUAI6.Prop]

    gameMap: List[List[THUAI6.PlaceType]]

    bullets: List[THUAI6.Bullet]
    bombedBullets: List[THUAI6.BombedBullet]

    mapInfo: THUAI6.GameMap

    gameInfo: THUAI6.GameInfo

    guids: List[int]
