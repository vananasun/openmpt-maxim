#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "./WinSharedMem.h"

void *SharedMem_Open(const char *name, unsigned long long size, char *existsFlag)
{
	HANDLE mapping = CreateFileMappingA(
		NULL,
		(LPSECURITY_ATTRIBUTES)NULL,
		PAGE_READWRITE,
		(DWORD)(size >> 32),
		(DWORD)(size),
		name);
	if(!mapping) return 0;
	
	if (existsFlag)
	{
		*existsFlag = (ERROR_ALREADY_EXISTS == GetLastError());
	}

	LPVOID addr = MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, size);
	return addr;
}
