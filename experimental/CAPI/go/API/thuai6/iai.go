package thuai6

type IGameTimer interface {
	StartTimer()
	EndTimer()
	Play(ai IAI)
}

type IAI interface {
	StudentPlay(api IStudentAPI)
	TrickerPlay(api ITrickerAPI)
}

type CreateAIFunc = func(playerID int64) IAI
