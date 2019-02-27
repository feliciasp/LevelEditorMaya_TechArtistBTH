#include <Windows.h>
#include <iostream>
#include <sstream>

#include "ComLib.h"

void produce(int numMsgs);
void consume(int numMsgs);


float delay = 0;
int memorySize = 0;
int numMessages = 0;
int msgSize = 0;

int main(int arg, char** argv)
{
	delay = (float(atof(argv[2])));
	memorySize = atoi(argv[3]);
	numMessages = atoi(argv[4]);

	if (argv[5][0] == 'r')
	{
		msgSize = rand() % 100 + 1;
	}
	else
	{
		msgSize = atoi(argv[5]);
	}

	std::string prod = "producer";
	std::string cons = "consumer";
	std::string prod2 = argv[1];
	std::string cons2 = argv[1];
	//
	if (prod.compare(prod2) == 0)
	{
		//we are a producer
		produce(numMessages);
	}
	if (cons.compare(cons2) == 0)
	{
		//we are a consumer
		consume(numMessages);
	}

	return 0;
}

void produce(int numMsgs)
{

	TYPE job = PRODUCER;
	std::string myFile = "myFileMap";
	size_t testes = size_t(memorySize);
	ComLib comLib(myFile, testes, job);

	size_t maxSize = 2050;
	size_t currMsgSize = msgSize;
	char* msg = new char[2050];
	const char characters[] = "0123456789qwertyuiopasdfghjklzxcvbnm";

	int testFailed = 0;

	bool sent;
	int i = 0;
	while (i < numMsgs)
	{

		if (currMsgSize > maxSize)
		{
			maxSize = currMsgSize;
			delete[] msg;
			msg = new char[maxSize];
		}

		for (int j = 0; j < currMsgSize; j++)
		{
			msg[j] = characters[rand() % sizeof(characters) - 1];
		}

		sent = comLib.send(msg, currMsgSize);
		if (sent == false)
		{
			continue;
		}

		std::cout << std::string((char*)msg, currMsgSize) << std::endl;

		i++;
		Sleep(delay);
		
	}

	delete[] msg;
}

void consume(int numMsgs)
{
	TYPE job = CONSUMER;
	std::string myFile = "myFileMap";
	size_t testes = size_t(memorySize);
	ComLib comLib(myFile, testes, job);

	size_t recvBuffSize = 1024;
	size_t nextSize;
	char* buff = (char*)malloc(recvBuffSize);
	
	int testFailed = 0;
	int i = 0;
	bool recieved;

	while (i < numMsgs)
	{
		Sleep(delay);
		nextSize = comLib.nextSize();

		if (nextSize > recvBuffSize)
		{
			free(buff);
			buff = (char*)malloc(nextSize);
		}

		recieved = comLib.recv(buff, nextSize);
		
		if (recieved == false)
		{
			//std::cout << "CONSUME FALSE" << std::endl;
			/*testFailed++;
			if (testFailed > 10000)
			{
				std::cout << "CONSUME FAILED" << std::endl;
				break;
			}*/
			continue;
		}

		std::cout << std::string((char*)buff, nextSize) << std::endl;

		i++;
	}

	
	free(buff);
}
