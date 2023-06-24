package main

import (
	thuai6 "API/thuai6"
)

const Asynchronous = false

func GetStudentType() []thuai6.StudentType {
	return []thuai6.StudentType{thuai6.Athlete, thuai6.Teacher, thuai6.StraightAStudent, thuai6.Sunshine}
}

func GetTrickerType() thuai6.TrickerType {
	return thuai6.Assassin
}
