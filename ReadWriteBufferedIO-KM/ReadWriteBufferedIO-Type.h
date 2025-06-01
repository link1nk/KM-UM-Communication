#pragma once

#define BUFFER_SIZE 64

typedef struct _READ_REQUEST
{
	CHAR OutputBuffer[BUFFER_SIZE];
} READ_REQUEST, * PREAD_REQUEST;

typedef struct _WRITE_REQUEST
{
	CHAR InputBuffer[BUFFER_SIZE];
} WRITE_REQUEST, * PWRITE_REQUEST;