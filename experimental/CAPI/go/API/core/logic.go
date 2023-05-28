package core

import (
	"errors"
	"sync"
	"sync/atomic"

	main "API"
	pb "API/proto"
	thuai6 "API/thuai6"
)

type Logic struct {
	communication *Communication
	playerType    thuai6.PlayerType
	playerID      int64
	studentType   thuai6.StudentType
	trickerType   thuai6.TrickerType
	timer         thuai6.IGameTimer
	mtxAI         sync.Mutex
	mtxState      sync.Mutex
	mtxBuffer     sync.Mutex
	cvBuffer      *sync.Cond
	cvAI          *sync.Cond
	state         [2]thuai6.State
	currentState  *thuai6.State
	bufferState   *thuai6.State
	counterState  int32
	counterBuffer int32
	gameState     thuai6.GameState
	aiStart       bool
	bufferUpdated bool
	aiLoop        int32
	freshed       int32
}

func NewLogic(playerType thuai6.PlayerType, id int64, tricker thuai6.TrickerType, student thuai6.StudentType) *Logic {
	logic := &Logic{
		communication: nil,
		playerType:    playerType,
		playerID:      id,
		studentType:   student,
		trickerType:   tricker,
		timer:         nil,
		mtxAI:         sync.Mutex{},
		mtxState:      sync.Mutex{},
		mtxBuffer:     sync.Mutex{},
		cvBuffer:      nil,
		cvAI:          nil,
		state:         [2]thuai6.State{},
		counterState:  0,
		counterBuffer: 0,
		gameState:     thuai6.NullGameState,
		aiStart:       false,
		bufferUpdated: true,
		aiLoop:        1,
		freshed:       0,
	}
	logic.currentState = &logic.state[0]
	logic.bufferState = &logic.state[1]
	logic.cvBuffer = sync.NewCond(&logic.mtxBuffer)
	logic.cvAI = sync.NewCond(&logic.mtxAI)
	return logic
}

func (logic *Logic) GetStudentSelfInfo() *thuai6.Student {
	logic.mtxState.Lock()
	defer logic.mtxState.Unlock()
	return logic.currentState.StudentSelf
}

func (logic *Logic) GetTrickerSelfInfo() *thuai6.Tricker {
	logic.mtxState.Lock()
	defer logic.mtxState.Unlock()
	return logic.currentState.TrickerSelf
}

func (logic *Logic) Move(time int64, angle float64) bool {
	return logic.communication.Move(time, angle, logic.playerID)
}

func (logic *Logic) TryConnection() bool {
	return logic.communication.TryConnection(logic.playerID)
}

func (logic *Logic) Wait() {
	atomic.StoreInt32(&logic.freshed, 0)
	logic.mtxBuffer.Lock()
	defer logic.mtxBuffer.Unlock()
	for atomic.LoadInt32(&logic.freshed) == 0 {
		logic.cvBuffer.Wait()
	}
}

func (logic *Logic) Update() {
	if main.Asynchronous == false {
		logic.mtxBuffer.Lock()
		defer logic.mtxBuffer.Unlock()
		for logic.bufferUpdated == false {
			logic.cvBuffer.Wait()
		}
		// Go 的 defer 是函数推出时执行而非 block 退出时执行，故包装一层 func
		func() {
			logic.mtxState.Lock()
			defer logic.mtxState.Unlock()
			var tmpState = logic.currentState
			logic.currentState = logic.bufferState
			logic.bufferState = tmpState
			logic.counterState = logic.counterBuffer
		}()
		logic.bufferUpdated = false
	}
}

func (logic *Logic) ProcessMessage() <-chan int {
	endChan := make(chan int, 1)
	go func() {
		logic.communication.AddPlayer(logic.playerID, logic.playerType, logic.studentType, logic.trickerType)
		for logic.gameState != thuai6.GameEnd {
			clientMsg := logic.communication.GetMessageToClient()
			logic.gameState = thuai6.GameStateToTHUAI6[clientMsg.GameState]
			switch logic.gameState {
			case thuai6.GameStart:
				playerGUIDs := make([]int64, 0)
				for _, obj := range clientMsg.ObjMessage {
					switch obj.MessageOfObj.(type) {
					case *(pb.MessageOfObj_StudentMessage):
						playerGUIDs = append(playerGUIDs, obj.GetStudentMessage().GetGuid())
					}
				}
				for _, obj := range clientMsg.ObjMessage {
					switch obj.MessageOfObj.(type) {
					case *(pb.MessageOfObj_TrickerMessage):
						playerGUIDs = append(playerGUIDs, obj.GetTrickerMessage().GetGuid())
					}
				}
				logic.currentState.Guids = playerGUIDs
				logic.bufferState.Guids = playerGUIDs
				// TODO: LoadBuffer
				break
			case thuai6.GameEnd:

			}
		}
		// TODO: ProcessMessage
		endChan <- 0
	}()
	return endChan
}

func (logic *Logic) Main(createAI thuai6.CreateAIFunc, ip string, port string) error {
	comm, err := NewCommunication(ip, port)
	if err != nil {
		return err
	}
	logic.communication = comm

	if logic.playerType == thuai6.StudentPlayer {
		logic.timer = NewStudentAPI(logic)
	} else if logic.playerType == thuai6.TrickerPlayer {
		logic.timer = NewTrickerAPI(logic)
	} else {
		return errors.New("Invalid player type")
	}

	aiThread := func() {
		// Go 的 defer 是函数推出时执行而非 block 退出时执行，故包装一层 func
		func() {
			logic.mtxAI.Lock()
			defer logic.mtxAI.Unlock()
			for logic.aiStart == false {
				logic.cvAI.Wait()
			}
		}()
		ai := createAI(logic.playerID)

		for atomic.LoadInt32(&logic.aiLoop) != 0 {
			if main.Asynchronous {
				logic.Wait()
			} else {
				logic.Update()
			}
			logic.timer.StartTimer()
			logic.timer.Play(ai)
			logic.timer.EndTimer()
		}
	}

	if logic.TryConnection() == false {
		atomic.StoreInt32(&logic.aiLoop, 0)
		return errors.New("Connection failed.")
	}

	aiEnd := make(chan int, 1)
	go func() {
		aiThread()
		aiEnd <- 1
	}()
	<-logic.ProcessMessage()
	<-aiEnd
	return nil
}
