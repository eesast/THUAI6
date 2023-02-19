from typing import List, Union
import PyAPI.structures as THUAI6


class State:
    teamScore: int
    self: Union[THUAI6.Human, THUAI6.Butcher]

    humans: List[THUAI6.Human]
    butchers: List[THUAI6.Butcher]

    props: List[THUAI6.Prop]

    map: List[List[THUAI6.PlaceType]]

    guids: List[int]
