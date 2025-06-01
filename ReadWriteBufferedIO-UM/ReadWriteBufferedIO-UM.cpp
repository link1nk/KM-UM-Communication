#include <Windows.h>
#include <iostream>
#include "..\ReadWriteBufferedIO-KM\ReadWriteBufferedIO-Type.h"

int main(void)
{
	HANDLE hReadWriteBufferedIO = CreateFile(L"\\\\.\\ReadWriteBufferedIO", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, NULL);

	if (hReadWriteBufferedIO == INVALID_HANDLE_VALUE)
	{
		std::cerr << "Não foi possivel abrir um handle para ReadWriteBufferedIO\n";
		return 1;
	}

	/// Test: Read Request

	READ_REQUEST ReadRequest = { 0 };
	DWORD BytesRead = 0;

	BOOL Success = ReadFile(hReadWriteBufferedIO, &ReadRequest, sizeof(READ_REQUEST), &BytesRead, nullptr);

	if (!Success)
	{
		std::cerr << "Falha ao fazer uma Read Request\n";
		CloseHandle(hReadWriteBufferedIO);
		return 2;
	}

	std::cout << "Read Request -> ReadRequest.OutputBuffer: " << ReadRequest.OutputBuffer << '\n';
	std::cout << "Read Request -> Bytes Read: " << BytesRead << '\n';

	/// Test: Write Request

	WRITE_REQUEST WriteRequest = { 0 };
	DWORD BytesWritten = 0;

	Success = WriteFile(hReadWriteBufferedIO, &WriteRequest, sizeof(WRITE_REQUEST), &BytesWritten, nullptr);

	if (!Success)
	{
		std::cerr << "Falha ao fazer uma Write Request\n";
		CloseHandle(hReadWriteBufferedIO);
		return 2;
	}

	std::cout << "Write Request -> Bytes Written: " << BytesWritten << '\n';

	CloseHandle(hReadWriteBufferedIO);

	return 0;
}