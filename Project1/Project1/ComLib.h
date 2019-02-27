#pragma once

#ifndef COMLIB_H
#define COMLIB_H

#include <windows.h>
#include <iostream>

#include <sstream>

//variables for creating our shared memory
enum MSG_TYPE { NORMAL, DUMMY };
enum TYPE { PRODUCER, CONSUMER };

struct HEADER
{
	//normal or dummy
	MSG_TYPE type;
	size_t length;
};

class ComLib
{
private:
	size_t *head;
	size_t *tail;
	char *base;
	size_t buffSize;
	TYPE type; 
	size_t free;

	size_t realSize;
	size_t *memStart;
	
	HANDLE hFileMap;
	void* mData;
	bool exists = false;
	unsigned int mSize = 1 << 10;


public:

	ComLib(const std::string& secret, const size_t& bufferSize, TYPE type);
	~ComLib();

	bool send(const void* msg, const size_t length);

	bool recv(char*ms, size_t& length);

	size_t nextSize();

	
};

#endif