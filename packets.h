#ifndef PACKETS_H
#define PACKETS_H

#include <iostream>

typedef struct {
	char type[8];
	char name[20];
	char unit[12];
} packetPayloadStruct;

typedef struct {
	uint8_t				packetCode[3];
	uint8_t				payloadSize;
	packetPayloadStruct	payload[30];
} usrPacketStruct;

#pragma pack(1)

typedef struct {
	uint32_t tstmp;
	double   dbTstmp;
	float    roll;
	float    pitch;
	float    yaw;
	float    accels_g[3];
	float    rates_dps[3];
	float    rateBias[3];
	float    mags[3];
	uint8_t  opMode;
	uint8_t  accelLinSwitch;
	uint8_t  turnSwitch;
}ekf1_payload_t;

// payload structure of enchanced EKF message
typedef struct {
	uint32_t tstmp;
	double   dbTstmp;
	float    roll;
	float    pitch;
	float    yaw;
	float    accels_g[3];
	float    accelBias[3];
	float    rates_dps[3];
	float    rateBias[3];
	float    velocity[3];
	float    mags[3];
	double   pos[3];
	uint8_t  opMode;
	uint8_t  accelLinSwitch;
	uint8_t  turnSwitch;
}ekf2_payload_t;

// payload structure of standard pos data message
typedef struct {
	uint32_t systemTime;
	uint32_t positionMode;
	double   timeOfWeek;
	double   latitude;
	double   longitude;
	double   ellipsoidalHeight;
	double   mslHeight;
	double   positionRMS;
	uint32_t velocityMode;
	float    velocityNorth;
	float    velocityEast;
	float    velocityDown;
	float    velocityRMS;
	float    attitudeStatus;
	float    roll;
	float    pitch;
	float    heading;
	float    attitudeRMS;
}pos_payload_t;

// payload structure of standard skyview data message
typedef struct {
	double   timeOfWeek;
	uint8_t  satelliteId;
	uint8_t  systemId;
	uint8_t  antennaId;
	uint8_t  l1cn0;
	uint8_t  l2cn0;
	float    azimuth;
	float    elevation;
}skyview_payload_t;

#pragma pack()

#define USR_PACKET_SIZE 4

extern uint8_t first_packetType_flag[USR_PACKET_SIZE];

extern uint16_t usrPacketType[USR_PACKET_SIZE];

extern const usrPacketStruct usrPacket[USR_PACKET_SIZE];


#endif /* PACKETS_H */
