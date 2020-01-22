#ifndef HANDLEDATA_H
#define HANDLEDATA_H

#include <iostream>

#define UCB_MAX_PAYLOAD_LENGTH		255

#pragma pack(1)

typedef struct {
	uint8_t		packetType;
	uint8_t		sync_MSB;        
	uint8_t		sync_LSB;        
	uint8_t		code_MSB;        
	uint8_t		code_LSB;        
	uint8_t		payloadLength;   
	uint8_t		payload[UCB_MAX_PAYLOAD_LENGTH];
} UcbPacketStruct;

#pragma pack()

extern UcbPacketStruct primaryUcbPacket;

int input_dataf(FILE* fp, char* text, UcbPacketStruct* ucbPacket);
int input_dataCom(char* text, UcbPacketStruct* ucbPacket);

#endif /* HANDLEDATA_H */
