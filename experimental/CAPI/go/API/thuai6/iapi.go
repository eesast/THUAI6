package thuai6

type IAPI interface {
	Move(timeInMilliseconds int64, angleInRadian float64) <-chan bool
}

type IStudentAPI interface {
	IAPI
	GetSelfInfo() *Student
}

type ITrickerAPI interface {
	IAPI
	GetSelfInfo() *Tricker
}
