from typing import List, Union
import PyAPI.structures as THUAI6


class State:
    teamScore: int
    self: Union[THUAI6.Student, THUAI6.Tricker]

    students: List[THUAI6.Student]
    trickers: List[THUAI6.Tricker]

    props: List[THUAI6.Prop]

    map: List[List[THUAI6.PlaceType]]

    guids: List[int]
