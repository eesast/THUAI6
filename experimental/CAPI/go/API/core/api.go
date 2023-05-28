package core

import (
	thuai6 "API/thuai6"
)

type ILogic interface {
	Move(time int64, angle float64) bool
	TryConnection() bool
	GetStudentSelfInfo() *thuai6.Student
	GetTrickerSelfInfo() *thuai6.Tricker
}

type StudentAPI struct {
	logic ILogic
}

type TrickerAPI struct {
	logic ILogic
}

func NewStudentAPI(logic ILogic) *StudentAPI {
	return &StudentAPI{
		logic: logic,
	}
}

func NewTrickerAPI(logic ILogic) *TrickerAPI {
	return &TrickerAPI{
		logic: logic,
	}
}

func (api *StudentAPI) Move(timeInMilliseconds int64, angleInRadian float64) <-chan bool {
	res := make(chan bool, 1)
	go func() {
		res <- api.logic.Move(timeInMilliseconds, angleInRadian)
	}()
	return res
}

func (api *StudentAPI) GetSelfInfo() *thuai6.Student {
	return api.logic.GetStudentSelfInfo()
}

func (api *StudentAPI) StartTimer() {
	// Nothing
}

func (api *StudentAPI) EndTimer() {
	// Nothing
}

func (api *StudentAPI) Play(ai thuai6.IAI) {
	ai.StudentPlay(api)
}

func (api *TrickerAPI) Move(timeInMilliseconds int64, angleInRadian float64) <-chan bool {
	res := make(chan bool, 1)
	go func() {
		res <- api.logic.Move(timeInMilliseconds, angleInRadian)
	}()
	return res
}

func (api *TrickerAPI) GetSelfInfo() *thuai6.Tricker {
	return api.logic.GetTrickerSelfInfo()
}

func (api *TrickerAPI) StartTimer() {
	// Nothing
}

func (api *TrickerAPI) EndTimer() {
	// Nothing
}

func (api *TrickerAPI) Play(ai thuai6.IAI) {
	ai.TrickerPlay(api)
}
