package telemetry

type TelemetryPacket struct {
	TimestampMs uint32

	Pitch int16
	Roll  int16
	Yaw   int16
}
