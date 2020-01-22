// openrtk_parse.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "handle_data.h"
#include "packets.h"
#include "serial.h"

void parse_data_from_file(char* fName)
{
	FILE* fdat = NULL;
	FILE* fLog[USR_PACKET_SIZE] = {NULL};
	char outFileName[255] = { 0 };
	char text[4096] = { 0 };
	
	fdat = fopen(fName, "rb");
	if (fdat == NULL)
	{
		std::cout << "can't find data file\r\n";
		return;
	}
	
	memset(first_packetType_flag, 0, USR_PACKET_SIZE);

	// parse data and write to new files
	int type = 0;
	while ((type = input_dataf(fdat, text, &primaryUcbPacket)) > -1)
	{
		//std::cout << "type = " << type << "\r\n";
		if (first_packetType_flag[type] == 0)
		{
			first_packetType_flag[type] = 1;
			// create file 
			sprintf(outFileName, "%s_%s.csv", fName, usrPacket[type].packetCode);
			fLog[type] = fopen(outFileName, "w");

			// write title bar
			for (uint8_t i = 0; i < usrPacket[type].payloadSize; i++)
			{
				if (fLog[type] != NULL)
				{
					fprintf(fLog[type], "%s,", usrPacket[type].payload[i].name);
				}
			}
			if (fLog[type] != NULL)
			{
				fprintf(fLog[type], "\n");
			}
		}

		// write data to file
		if (fLog[type] != NULL)
		{
			fprintf(fLog[type], (const char *)text);
		}
	}

	// close
	for (uint8_t i = 0; i < USR_PACKET_SIZE; i++)
	{
		if (fLog[i] != NULL)
		{
			fclose(fLog[i]);
		}
	}
}

void parse_data_from_COM(char* COMname, int baudRate, char* fName)
{
	BOOL ret;
	FILE* fLog[USR_PACKET_SIZE] = { NULL };
	char outFileName[255] = { 0 };
	char text[4096] = { 0 };

	ret = serial_open(COMname, baudRate);
	if (ret)
	{
		int type = 0;
		while ((type = input_dataCom(text, &primaryUcbPacket)) > -1)
		{
			//std::cout << "type = " << type << "\r\n";
			if (first_packetType_flag[type] == 0)
			{
				first_packetType_flag[type] = 1;
				// create file 
				sprintf(outFileName, "%s_%s.csv", fName, usrPacket[type].packetCode);
				fLog[type] = fopen(outFileName, "w");

				// write title bar
				for (uint8_t i = 0; i < usrPacket[type].payloadSize; i++)
				{
					if (fLog[type] != NULL)
					{
						fprintf(fLog[type], "%s,", usrPacket[type].payload[i].name);
					}
				}
				if (fLog[type] != NULL)
				{
					fprintf(fLog[type], "\n");
				}
			}

			// write data to file
			if (fLog[type] != NULL)
			{
				fprintf(fLog[type], (const char*)text);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		parse_data_from_file(argv[1]);
	}
	else if (argc == 4)
	{
		parse_data_from_COM(argv[1], atoi(argv[2]), argv[3]);
	}
	else
	{
		std::cout << "./openrtk_parse.exe [rawDataFileName]\r\n";
		std::cout << "./openrtk_parse.exe [COMx] [BaudRate] [OutFileName]\r\n";
	}

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
