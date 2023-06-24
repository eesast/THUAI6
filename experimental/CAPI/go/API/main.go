package main

import (
	"fmt"
	"log"
)

var ch = make(chan int, 1)

func Func() {
	log.Println(0)
	ch <- 1
	log.Fatalln(1)
}

func main() {
	fmt.Println("THUAI6")
}
