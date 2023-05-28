package core

import (
	pb "API/proto"
	thuai6 "API/thuai6"
	"context"
	"sync"

	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

type Communication struct {
	stub            pb.AvailableServiceClient
	messageToClient *pb.MessageToClient
	haveNewMessage  bool
	mtxMessage      sync.Mutex
	cvMessage       *sync.Cond
}

func NewCommunication(ip string, port string) (*Communication, error) {
	addr := ip + ":" + port
	conn, err := grpc.Dial(addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		return nil, err
	}
	stub := pb.NewAvailableServiceClient(conn)
	comm := &Communication{stub: stub, messageToClient: nil, haveNewMessage: false, mtxMessage: sync.Mutex{}}
	comm.cvMessage = sync.NewCond(&comm.mtxMessage)
	return comm, nil
}

func (self *Communication) TryConnection(playerID int64) bool {
	res, err := self.stub.TryConnection(context.Background(), &pb.IDMsg{PlayerId: playerID})
	if err != nil {
		// TODO: do loggings
		return false
	}
	return res.GetActSuccess()
}

func (self *Communication) AddPlayer(playerID int64, playerType thuai6.PlayerType, studentType thuai6.StudentType, trickerType thuai6.TrickerType) {
	var playerMsg pb.PlayerMsg
	playerMsg.PlayerId = playerID
	playerMsg.PlayerType = thuai6.PlayerTypeToProto[playerType]
	if playerType == thuai6.StudentPlayer {
		playerMsg.JobType = &pb.PlayerMsg_StudentType{StudentType: thuai6.StudentTypeToProto[studentType]}
	} else if playerType == thuai6.TrickerPlayer {
		playerMsg.JobType = &pb.PlayerMsg_TrickerType{TrickerType: thuai6.TrickerTypeToProto[trickerType]}
	} else {
		// TODO: Report ERROR
	}

	go func() {
		msgReader, err := self.stub.AddPlayer(context.Background(), &playerMsg)
		if err != nil {
			// TODO: Report ERROR
		}
		for {
			msg, err := msgReader.Recv()
			if err != nil {
				break
			}

			func() {
				self.mtxMessage.Lock()
				defer self.mtxMessage.Unlock()
				self.messageToClient = msg
				self.haveNewMessage = true
				self.cvMessage.Signal()
			}()
		}
	}()
}

func (self *Communication) Move(time int64, angle float64, playerID int64) bool {
	res, err := self.stub.Move(context.Background(), &pb.MoveMsg{
		TimeInMilliseconds: time,
		Angle:              angle,
		PlayerId:           playerID,
	})
	if err != nil {
		// TODO: do loggings
		return false
	}
	return res.GetActSuccess()
}

func (self *Communication) GetMessageToClient() pb.MessageToClient {
	self.mtxMessage.Lock()
	defer self.mtxMessage.Unlock()
	for !self.haveNewMessage {
		self.cvMessage.Wait()
	}
	self.haveNewMessage = false
	return *self.messageToClient
}
