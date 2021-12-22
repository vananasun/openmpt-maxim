#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct WinEvent {
    HANDLE m_handle = NULL;

    bool createNew(const char* name);
    bool openOrCreate(const char* name);
    int wasTriggered();
    bool set();
	bool reset();
    void close();
};