package states

type FlightState uint8

const (
	Init FlightState = iota  //1. startup of fc, and server
							 //2. start sensor data accumulation and conversion
			 				 //3. 
	
)