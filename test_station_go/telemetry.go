package main


type TelemetryPacket struct{
	TimestampMs uint32
	
	Pitch       int16 
	Roll        int16
	Yaw         int16

	State       uint8
}