#include "handle_data.h"
#include "packets.h"
#include "crc16.h"
#include "stdio.h"
#include "serial.h"

#define MAX_DATA_BUFFER_SIZE 1024

UcbPacketStruct primaryUcbPacket;
static uint8_t dataBuffer[MAX_DATA_BUFFER_SIZE];

static uint8_t decode_packet(UcbPacketStruct* ucbPacket, char* text);

uint16_t countnum = 0;

/* get fields (little-endian) ------------------------------------------------*/
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((signed char *)(p)))
static unsigned short U2(unsigned char* p) { unsigned short u; memcpy(&u, p, 2); return u; }
static unsigned int   U4(unsigned char* p) { unsigned int   u; memcpy(&u, p, 4); return u; }
static short		  I2(unsigned char* p) { short          u; memcpy(&u, p, 2); return u; }
static int            I4(unsigned char* p) { int            u; memcpy(&u, p, 4); return u; }
static float          R4(unsigned char* p) { float          r; memcpy(&r, p, 4); return r; }
static double         R8(unsigned char* p) { double         r; memcpy(&r, p, 8); return r; }

int input_dataf(FILE* fp, char* text, UcbPacketStruct* ucbPacket)
{
	static int bytesInBuffer = 0, pos = 0, state = 0, crcError = 0, len = 0;
	static uint8_t* ptr;
	static uint16_t crcMsg = 0, code;
	static uint32_t sync = 0;
	unsigned char tmp;
	unsigned int synced = 0, type;
	uint16_t crcCalc;

	while (1) {
		if (!bytesInBuffer) {
			bytesInBuffer = fread(dataBuffer, 1, MAX_DATA_BUFFER_SIZE, fp);
			/*if (bytesInBuffer != MAX_DATA_BUFFER_SIZE)
			{
				std::cout << "bytesInBuffer = " << bytesInBuffer << "\r\n";
			}*/
			if (bytesInBuffer <= 0) {
				/*std::cout << "countnum = " << countnum << "\r\n";*/
				return -1;  // end or err
			}
			//countnum++;
			pos = 0;
		}
		tmp = dataBuffer[pos++];
		bytesInBuffer--;
		sync = (sync << 8) | tmp;
		synced = 0;
		if ((sync & 0xFFFF0000) == 0x55550000) {
			code = sync & 0xffff;
			for (uint8_t i = 0; i < USR_PACKET_SIZE; i++){
				if (code == usrPacketType[i])
				{
					synced = 1;
					type = i;
					break;
				}
			}
		}
		if (synced) {
			ucbPacket->packetType = type;
			ucbPacket->payloadLength = 0;
			ucbPacket->code_MSB = (sync >> 8) & 0xff;
			ucbPacket->code_LSB = sync & 0xff;
			state = 1;
			len = 0;
			synced = 0;
			continue;
		}
		switch (state) {
		case 0:
			break;
		case 1:
			ucbPacket->payloadLength = tmp;
			if (tmp == 0) {
				state = 3;  // crc next
			}
			else {
				state = 2;  // data next
				len = 0;
			}
			ptr = ucbPacket->payload;
			break;
		case 2:
			if (len++ > UCB_MAX_PAYLOAD_LENGTH) {
				state = 0;
				break;
			}
			*ptr++ = tmp;
			if (len == ucbPacket->payloadLength) {
				//crc next
				state = 3;
				crcMsg = 0;
			}
			break;
		case 3:
			crcMsg = tmp;
			*ptr++ = tmp;
			state = 4;
			break;
		case 4:
			state = 0;
			crcMsg = crcMsg | ((uint16_t)tmp << 8);
			*ptr++ = tmp;
			crcCalc = CalculateCRC((uint8_t*)&ucbPacket->code_MSB, len + 3);
			if (crcMsg != crcCalc) {
				crcError++;
			}
			else {
				// process message here
				return decode_packet(ucbPacket, text); // will come back later
			}
			break;
		default:
			std::cout << "raw data err!\r\n";
			while (1) {}; // should not be here
		}
	}
}

int input_dataCom(char* text, UcbPacketStruct* ucbPacket)
{
	static int bytesInBuffer = 0, pos = 0, state = 0, crcError = 0, len = 0;
	static uint8_t* ptr;
	static uint16_t crcMsg = 0, code;
	static uint32_t sync = 0;
	unsigned char tmp;
	unsigned int synced = 0, type;
	uint16_t crcCalc;

	while (1) {
		if (!bytesInBuffer) {
			bytesInBuffer = serial_read(dataBuffer, MAX_DATA_BUFFER_SIZE);
			if (bytesInBuffer <= 0) {
				return -1;  // end or err
			}
			pos = 0;
		}
		tmp = dataBuffer[pos++];
		bytesInBuffer--;
		sync = (sync << 8) | tmp;
		synced = 0;
		if ((sync & 0xFFFF0000) == 0x55550000) {
			code = sync & 0xffff;
			for (uint8_t i = 0; i < USR_PACKET_SIZE; i++) {
				if (code == usrPacketType[i])
				{
					synced = 1;
					type = i;
					break;
				}
			}
		}
		if (synced) {
			ucbPacket->packetType = type;
			ucbPacket->payloadLength = 0;
			ucbPacket->code_MSB = (sync >> 8) & 0xff;
			ucbPacket->code_LSB = sync & 0xff;
			state = 1;
			len = 0;
			synced = 0;
			continue;
		}
		switch (state) {
		case 0:
			break;
		case 1:
			ucbPacket->payloadLength = tmp;
			if (tmp == 0) {
				state = 3;  // crc next
			}
			else {
				state = 2;  // data next
				len = 0;
			}
			ptr = ucbPacket->payload;
			break;
		case 2:
			if (len++ > UCB_MAX_PAYLOAD_LENGTH) {
				state = 0;
				break;
			}
			*ptr++ = tmp;
			if (len == ucbPacket->payloadLength) {
				//crc next
				state = 3;
				crcMsg = 0;
			}
			break;
		case 3:
			crcMsg = tmp;
			*ptr++ = tmp;
			state = 4;
			break;
		case 4:
			state = 0;
			crcMsg = crcMsg | ((uint16_t)tmp << 8);
			*ptr++ = tmp;
			crcCalc = CalculateCRC((uint8_t*)& ucbPacket->code_MSB, len + 3);
			if (crcMsg != crcCalc) {
				crcError++;
			}
			else {
				// process message here
				return decode_packet(ucbPacket, text); // will come back later
			}
			break;
		default:
			std::cout << "raw data err!\r\n";
			while (1) {}; // should not be here
		}
	}
}

static void devoce_e1(uint8_t* payload, char* text)
{
	ekf1_payload_t* pld = (ekf1_payload_t*)payload;
	sprintf(text, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n"
		, pld->tstmp, pld->dbTstmp, pld->roll, pld->pitch, pld->yaw
		, pld->accels_g[0], pld->accels_g[1], pld->accels_g[2]
		, pld->rates_dps[0], pld->rates_dps[1], pld->rates_dps[2]
		, pld->rateBias[0], pld->rateBias[1], pld->rateBias[2]
		, pld->mags[0], pld->mags[1], pld->mags[2]
		, pld->opMode, pld->accelLinSwitch, pld->turnSwitch);
}

static void devoce_e2(uint8_t* payload, char* text)
{
	ekf2_payload_t* pld = (ekf2_payload_t*)payload;
	sprintf(text, "%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%d,%d\n"
		, pld->tstmp, pld->dbTstmp, pld->roll, pld->pitch, pld->yaw
		, pld->accels_g[0], pld->accels_g[1], pld->accels_g[2]
		, pld->accelBias[0], pld->accelBias[1], pld->accelBias[2]
		, pld->rates_dps[0], pld->rates_dps[1], pld->rates_dps[2]
		, pld->rateBias[0], pld->rateBias[1], pld->rateBias[2]
		, pld->velocity[0], pld->velocity[1], pld->velocity[2]
		, pld->mags[0], pld->mags[1], pld->mags[2]
		, pld->pos[0], pld->pos[1], pld->pos[2]
		, pld->opMode, pld->accelLinSwitch, pld->turnSwitch);
}

static void devoce_pS(uint8_t* payload, char* text)
{
	pos_payload_t* pld = (pos_payload_t*)payload;
	sprintf(text, "%d,%d,%f,%f,%f,%f,%f,%f,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"
		, pld->systemTime, pld->positionMode, pld->timeOfWeek
		, pld->latitude, pld->longitude, pld->ellipsoidalHeight
		, pld->mslHeight, pld->positionRMS
		, pld->velocityMode, pld->velocityNorth, pld->velocityEast
		, pld->velocityDown, pld->velocityRMS
		, pld->attitudeStatus, pld->roll, pld->pitch, pld->heading, pld->attitudeRMS);
}

static void devoce_sK(uint8_t* payload, uint8_t payloadLength, char* text)
{
	skyview_payload_t* pld = (skyview_payload_t*)payload;
	uint8_t arraySize = payloadLength / sizeof(skyview_payload_t);
	char* ptrText = text;
	
	for (uint8_t i = 0; i < arraySize; i++)
	{
		sprintf(ptrText, "%f,%d,%d,%d,%d,%d,%f,%f\n"
			, pld->timeOfWeek, pld->satelliteId, pld->systemId
			, pld->antennaId, pld->l1cn0, pld->l2cn0
			, pld->azimuth, pld->elevation);

		pld++;
		ptrText = text + strlen(text);
	}
}

static uint8_t decode_packet(UcbPacketStruct* ucbPacket, char* text)
{
	switch (ucbPacket->packetType)
	{
	case 0 :
		devoce_e1(ucbPacket->payload, text);
		break;
	case 1:
		devoce_e2(ucbPacket->payload, text);
		break;
	case 2:
		devoce_pS(ucbPacket->payload, text);
		break;
	case 3:
		devoce_sK(ucbPacket->payload, ucbPacket->payloadLength, text);
		break;

	default:
		sprintf(text, "\n");
		break;
	}

	return ucbPacket->packetType;
}
