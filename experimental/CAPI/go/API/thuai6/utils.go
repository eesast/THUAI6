package thuai6

import (
	pb "API/proto"
)

var (
	PlayerTypeToProto map[PlayerType]pb.PlayerType = map[PlayerType]pb.PlayerType{
		NullPlayerType: pb.PlayerType_NULL_PLAYER_TYPE,
		StudentPlayer:  pb.PlayerType_STUDENT_PLAYER,
		TrickerPlayer:  pb.PlayerType_TRICKER_PLAYER,
	}

	StudentTypeToProto map[StudentType]pb.StudentType = map[StudentType]pb.StudentType{
		NullStudentType:  pb.StudentType_NULL_STUDENT_TYPE,
		Athlete:          pb.StudentType_ATHLETE,
		Teacher:          pb.StudentType_TEACHER,
		StraightAStudent: pb.StudentType_STRAIGHT_A_STUDENT,
		Robot:            pb.StudentType_ROBOT,
		TechOtaku:        pb.StudentType_TECH_OTAKU,
		Sunshine:         pb.StudentType_SUNSHINE,
	}

	TrickerTypeToProto map[TrickerType]pb.TrickerType = map[TrickerType]pb.TrickerType{
		NullTrickerType: pb.TrickerType_NULL_TRICKER_TYPE,
		Assassin:        pb.TrickerType_ASSASSIN,
		Klee:            pb.TrickerType_KLEE,
		ANoisyPerson:    pb.TrickerType_A_NOISY_PERSON,
		Idol:            pb.TrickerType_IDOL,
	}

	GameStateToTHUAI6 map[pb.GameState]GameState = map[pb.GameState]GameState{
		pb.GameState_NULL_GAME_STATE: NullGameState,
		pb.GameState_GAME_START:      GameStart,
		pb.GameState_GAME_RUNNING:    GameRunning,
		pb.GameState_GAME_END:        GameEnd,
	}
)
