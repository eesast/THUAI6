package thuai6

type GameState int8

const (
	NullGameState GameState = 0
	GameStart     GameState = 1
	GameRunning   GameState = 2
	GameEnd       GameState = 3
)

type PlayerType int8

const (
	NullPlayerType PlayerType = 0
	StudentPlayer  PlayerType = 1
	TrickerPlayer  PlayerType = 2
)

type StudentType int8

const (
	NullStudentType  StudentType = 0
	Athlete          StudentType = 1
	Teacher          StudentType = 2
	StraightAStudent StudentType = 3
	Robot            StudentType = 4
	TechOtaku        StudentType = 5
	Sunshine         StudentType = 6
)

type TrickerType int8

const (
	NullTrickerType TrickerType = 0
	Assassin        TrickerType = 1
	Klee            TrickerType = 2
	ANoisyPerson    TrickerType = 3
	Idol            TrickerType = 4
)

type Player struct {
	x int32
	y int32
}

type Student struct {
	Player
	determination int32
}

type Tricker struct {
	Player
	trickDesire float64
}
