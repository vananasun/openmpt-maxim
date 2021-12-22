#include "WinEvent.h"

/** returns false on error */
bool WinEvent::createNew(const char* name)
{
    m_handle = CreateEventA(NULL, FALSE, FALSE, name);
    return (!!m_handle);
}

/** returns false on error */
bool WinEvent::openOrCreate(const char* name)
{
    if (m_handle) return true; // event was already set up
	m_handle = OpenEventA(EVENT_ALL_ACCESS, FALSE, name);
    if (m_handle) return true; // event was set up by OpenMPT and we connected to it
	m_handle = CreateEventExA(NULL, name, NULL, EVENT_ALL_ACCESS);  // m_handle = CreateEventA(NULL, FALSE, FALSE, name);
    if (m_handle) return true; // event had to be created by us
    return false; // nothing worked, error!
}

/** returns 0 if not triggered, returns 1 if triggered, 2 if error */
int WinEvent::wasTriggered()
{
	// return (WAIT_OBJECT_0 == WaitForSingleObject(m_handle, 0));
	switch(WaitForSingleObject(m_handle, 0))
	{
		case WAIT_ABANDONED: return 1;
		case WAIT_OBJECT_0: return 1;
		case WAIT_TIMEOUT: return 0;
	}
	return 2; // WAIT_FAILED probably
}

void WinEvent::close()
{
    CloseHandle(m_handle);
}

bool WinEvent::set()
{
	if(!SetEvent(m_handle)) {
        m_handle = NULL;
		return false;
    }
    return true;
}

bool WinEvent::reset()
{
	ResetEvent(m_handle);
    return true;
}
