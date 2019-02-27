#include "ComLib.h"

ComLib::ComLib(const std::string& secret, const size_t& bufferSize, TYPE type)
{
	realSize = bufferSize * (1 << 20);
	//realSize = 250;
	//try to create ur filemap
	hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, (DWORD)0, (DWORD)realSize, (LPCSTR)secret.c_str());
	if (hFileMap == NULL)
	{
		//ERROR HANDLING
		exit;
	}
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//failemap has already been created but we get a handle
		exists = true;
	}

	//create a view of the map
	mData = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//buffSize = realSize;

	//base = (char*)mData;
	head = (size_t*)mData;
	tail = head + 1;
	base = (char*)(tail + 1);
	//std::cout << "memStart: " << *base << std::endl;
	free = realSize - sizeof(head) - sizeof(tail);

	if (!exists)
	{
		*head = 0;
		*tail = 0;
	}

	this->type = type;

}

ComLib::~ComLib()
{
	UnmapViewOfFile((LPCVOID)mData);
	CloseHandle(hFileMap);
}

bool ComLib::send(const void * msg, const size_t length)
{
	HEADER h;
	size_t freeMem = realSize - sizeof(base);
	size_t tempTail = *tail;

	bool returnValue = false;

	if (*head >= tempTail)
	{
		freeMem = (free - (*head));
	}
	if (*head < tempTail)
	{
		freeMem = tempTail - (*head);
	}


	if (freeMem > (length + (sizeof(HEADER) * 3)))
	{
		
		h.type = NORMAL;
		h.length = length;
		memcpy(base + (*head), &h, sizeof(HEADER));
		memcpy(base + (*head) + sizeof(HEADER), (char*)msg, length);
		*head = *head + sizeof(HEADER) + length;


		returnValue = true;
	}
	else
	{
		if (tempTail == 0)
		{

			returnValue = false;
		}
		else
		{
			//std::cout << "DUMMY SEND" << std::endl;

			h.type = DUMMY;
			h.length = 1;
			memcpy(base + (*head), &h, sizeof(HEADER));

			*head = 0;
			send(msg, length);
			//returnValue = false;
			
		}
	}

	return returnValue;
}

bool ComLib::recv(char *ms, size_t & length)
{
	size_t tempHead = *head;
	bool returnValue = false;

	if (*tail != tempHead && length > 0)
	{
		HEADER h;
		memcpy(&h, base + (*tail), sizeof(HEADER));

		if (h.type == NORMAL)
		{

			memcpy(ms, base + (*tail) + sizeof(HEADER), h.length);
			*tail = *tail + sizeof(HEADER) + h.length;

			returnValue = true;
		}
		if (h.type == DUMMY)
		{
			//std::cout << "DUMMY RECV" << std::endl;
			*tail = 0;
			recv(ms, length);
		}
	}
	
	return returnValue;
}

size_t ComLib::nextSize()
{
	size_t tempHead = *head;
	size_t tempTail = *tail;

	if (tempTail != tempHead)
	{
		HEADER h;
		memcpy(&h, base + (tempTail), sizeof(HEADER));

		if (h.length > 0)
		{
			return h.length;
		}
		
	}
	return 0;
};

