package main

import (
	// "fmt"
	// "test_station_go/telemetry"
	"test_station_go/states"
)

func main() {
	var currentstate states.FlightState = states.Init
	Init(currentstate)

}

func Init(currentstate states.FlightState) bool {
	InitMPU(currentstate)

	return false
}
