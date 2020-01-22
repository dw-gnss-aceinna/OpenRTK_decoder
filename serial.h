#pragma once
#ifndef SERIAL_H
#define SERIAL_H

#include <iostream>
#include "windows.h"

extern HANDLE hCom;

int serial_open(char* comname, int baudRate);
int serial_read(uint8_t *data, int size);
int serial_write(char lpOutBuffer[]);
void serial_close(void);

#endif /* SERIAL_H */
