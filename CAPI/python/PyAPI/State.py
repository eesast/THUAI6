from typing import List, Union
import PyAPI.structures as THUAI6


class State:
    def __init__(self, **kwargs) -> None:
        self.teamScore = 0
        self.self = THUAI6.Student()
        self.students = []
        self.trickers = []
        self.props = []
        self.gameMap = []
        self.bullets = []
        self.bombedBullets = []
        self.mapInfo = THUAI6.GameMap()
        self.gameInfo = THUAI6.GameInfo()
        self.guids = []

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
