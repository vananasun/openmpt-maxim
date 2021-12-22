#ifdef MPT_WITH_REWIRE
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include "ReWireAPI.h"
#include "ReWirePanelAPI.h"
#include "MPTRewirePanel.h"
#include "MPTRewireDebugUtils.h"
#include "../../mptrack/Reporting.h"
#include <algorithm>
#include <thread>
#include <string.h>
#include <stdlib.h>



using namespace ReWire;


static std::string getExecutableDirectory()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string path = std::string(buffer);
	std::string::size_type pos = path.find_last_of("\\/");
	path = path.substr(0, pos);
	return path;
}

/** \returns true if the correct device is the currently registered device after calling. */
static bool unregisterOtherDevice(std::string ourDevicePath)
{
	HKEY key;
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Propellerhead Software\\ReWire\\MPTRewire", 0, KEY_READ, &key))
		return false; // does not exist (or access denied)
	
	BYTE existingPath[MAX_PATH];
	DWORD regType = REG_SZ;
	DWORD size;
	if(!RegQueryValueExA(key, "Device Path", NULL, &regType, existingPath, &size) && size > 0)
	{
		// Only unregister the device if it has a different path from the one we want to use
		if (strcmp(ourDevicePath.c_str(), (char*)existingPath))
		{
			// If this fails, access was denied because we did not run OpenMPT as admin
			if (kReWireError_NotFound == RWPUnregisterReWireDevice((char*)existingPath))
			{
				Reporting::Warning("ReWire Sound Device: Unable to replace the ReWire device path. Please restart OpenMPT as administrator.\nYOU ARE USING AN OUTDATED MPTREWIRE!");
			}
			return false;
		}
	}
	RegCloseKey(key);
	return true; // the correct device was already registered!
}



/*******************************************************************************
 *
 * Main functions: Opening, closing, thread proc
 * 
 ******************************************************************************/

MPTRewirePanel::MPTRewirePanel()
{
	// Open ReWire
	ReWireError status = RWPOpen();
	if(kReWireError_NoError != status && kReWireImplError_ReWireAlreadyOpen != status)
	{
		DEBUG_PRINT("RWPOpen() status=%i\n", status);
		Reporting::Error("ReWire Sound Device: Failed to open ReWire itself.\nTry restarting your computer.");
		m_Errored = true;
		return;
	}

	// Register device, if it is already registered on another path then replace the path
	std::string deviceDllPath = getExecutableDirectory() + "\\MPTRewire.dll";
	if (!unregisterOtherDevice(deviceDllPath))
	{
		status = RWPRegisterReWireDevice(deviceDllPath.c_str());
		if(kReWireError_NoError != status && kReWireError_AlreadyExists != status)
		{
			DEBUG_PRINT("RWPRegisterReWireDevice status=%i\n", (int)status);
			Reporting::Error("ReWire Sound Device: Unable to register the device.\nHave you tried running OpenMPT as administrator?");
			m_Errored = true;
			return;
		}
	}

	// Make sure there are allocated audio buffers at all times
	reallocateBuffers(8192);
}


MPTPanelStatus MPTRewirePanel::open(
	MPTRenderCallback renderCallback,
	MPTAudioInfoCallback audioInfoCallback,
	MPTMixerQuitCallback mixerQuitCallback,
	void *callbackUserData)
{

#if defined(WINDOWS) && defined(DEBUG)
	AllocConsole();
	if(!freopen("CONOUT$", "w", stderr)) return MPTPanelStatus::UnknownDeviceProblem;
#endif

	// Check whether mixer is running
	ReWire_char_t isRunning = 0;
	ReWireError status = RWPIsReWireMixerAppRunning(&isRunning);
	if(kReWireError_NoError != status || !isRunning)
	{
		return MPTPanelStatus::MixerNotRunning;
	}

	// Load the device
	// DEBUG_PRINT("Loading ReWire device at \"%s\".\n", deviceDllPath.c_str());
	m_EventToDevice = CreateEventA(NULL, FALSE, FALSE, "OPENMPT_REWIRE_PANEL_TO_DEVICE");
	status = RWPLoadDevice(m_DeviceName);
	if (kReWireError_UnableToOpenDevice == status) {
		return MPTPanelStatus::UnknownDeviceProblem;
	} else if(kReWireError_NoError != status)
	{
		DEBUG_PRINT("RWPLoadDevice status=%i\n", (int)status);
		return MPTPanelStatus::DeviceNotInstalled;
	}

	// Set up communications with the device
	m_EventFromDevice = OpenEventA(SYNCHRONIZE, FALSE, "OPENMPT_REWIRE_DEVICE_TO_PANEL");
	if(!m_EventFromDevice)
	{
		DEBUG_PRINT("OpenEventA failed, error=%i.\n", (int)GetLastError());
		return MPTPanelStatus::UnknownDeviceProblem;
	}

	status = RWPComConnect("OMPT", &m_PanelPortHandle);
	if(status != kReWireError_NoError) // @TODO: kReWireError_Busy (I think when connecting after mixer quit)
	{
		DEBUG_PRINT("RWPComConnect status=%i\n", (int)status);
		return MPTPanelStatus::ReWireProblem;
	}

	// Start audio thread
	m_CallbackUserData = callbackUserData;
	m_RenderCallback = renderCallback;
	m_AudioInfoCallback = audioInfoCallback;
	m_MixerQuitCallback = mixerQuitCallback;
	m_MixerQuit = false;
	m_Running = true;
	m_Thread = std::thread(&MPTRewirePanel::threadProc, this);
	return MPTPanelStatus::Ok;
}





MPTRewirePanel::~MPTRewirePanel()
{
	// Try to close panel API
	ReWire_char_t okFlag = 0;
	ReWireError status = RWPIsCloseOK(&okFlag);
	if(kReWireError_NoError == status && okFlag)
		RWPClose();
	else
		DEBUG_PRINT("RWPIsCloseOk status=%i okFlag=%i\n", (int)status, (int)okFlag);

	deallocateBuffers();
}

bool MPTRewirePanel::close()
{
	if(!m_Running) return true;
	m_Running = false;
	if(m_Thread.joinable()) m_Thread.join();
	CloseHandle(m_EventToDevice);

	ReWireError status = RWPComDisconnect(m_PanelPortHandle);
	if(kReWireError_NoError != status)
	{
		DEBUG_PRINT("RWPComDisconnect status=%i\n", (int)status);
	}

	// Unload device, unfortunately due to a bug in ReWire, if the mixer crashes, and the panel tries to unload
	// during the crash, the thread blocks for a good 15 seconds and then returns kReWireError_UnableToOpenDevice.
	// NOTE: In the future I might just write a check that tests if the ReWire device has a blocking thread.
	status = RWPUnloadDevice(m_DeviceName);
	if(kReWireError_NoError != status && kReWireImplError_ReWireNotOpen != status)
	{
		DEBUG_PRINT("RWPUnloadDevice status=%i\n", (int)status);
	}

	// deallocateBuffers();
	// m_MaxBufferSize = m_SampleRate = 0; // force re-allocate buffers when we open the device again
	return true;
}



/**
 * Panel thread that checks for audio requests and tells OpenMPT to render audio
**/
void MPTRewirePanel::threadProc()
{
	while(m_Running)
	{
		checkComConnection();
		pollAudioRequests();
	}
}



/*******************************************************************************
 *
 * Audio buffer functions
 * 
 ******************************************************************************/

void MPTRewirePanel::deallocateBuffers() {
	if(m_AudioBuffers)
	{
		for(int i = 0; i < kReWireAudioChannelCount / 2; i++)
			if(m_AudioBuffers[i])
				delete[] m_AudioBuffers[i];
		delete[] m_AudioBuffers;
		m_AudioBuffers = nullptr;
	}
	if(m_AudioResponseBuffer)
	{
		delete[] m_AudioResponseBuffer;
		m_AudioResponseBuffer = nullptr;
	}
}


void MPTRewirePanel::reallocateBuffers(int32_t maxBufferSize)
{
	m_MaxBufferSize = maxBufferSize;

	deallocateBuffers();
	m_AudioBuffers = new int *[kReWireAudioChannelCount / 2];
	for(int i = 0; i < kReWireAudioChannelCount / 2; i++)
		m_AudioBuffers[i] = new int[(size_t)m_MaxBufferSize * 2];
	m_AudioResponseBuffer = reinterpret_cast<MPTAudioResponse *>(new uint8_t[sizeof(MPTAudioResponse) + ((size_t)m_MaxBufferSize * 2 * sizeof(int32_t))]);
}




/*******************************************************************************
 *
 * Basic COM functions
 * 
 ******************************************************************************/

void MPTRewirePanel::checkComConnection()
{
	if (kReWireError_PortConnected == RWPComCheckConnection(m_PanelPortHandle)) {
		return; // all is fine; no action
	}
	if (!m_MixerQuit) {
		m_MixerQuit = true;
		m_MixerQuitCallback(m_CallbackUserData);
	}
}


void MPTRewirePanel::swallowRemainingMessages()
{
	ReWire_uint16_t messageSize = 0;
	ReWireError status;
	do
	{
		status = RWPComRead(m_PanelPortHandle, PIPE_RT, &messageSize, m_Message);
	} while(kReWireError_NoError == status);  // while a message was still in the pipe
}


bool MPTRewirePanel::waitForEventFromDevice(const int milliseconds)
{
	switch(WaitForSingleObject(m_EventFromDevice, milliseconds))
	{
		case WAIT_ABANDONED:
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			{
				// Detect if the mixer app has quit abruptly
				ReWire_char_t isRunning = 0;
				if(kReWireError_NoError != RWPIsReWireMixerAppRunning(&isRunning) || !isRunning)
				{
					if(!m_MixerQuit)
					{
						m_MixerQuit = true;
						m_MixerQuitCallback(m_CallbackUserData);
					}
				}
				break;
			}
		case WAIT_FAILED:  // 6 = INVALID_HANDLE
			DEBUG_PRINT("waitForEventFromDevice WAIT_FAILED, error=%i.\n", (int)GetLastError());
	}
	return false;  // timeout or error
}




/*******************************************************************************
 *
 * Audio requests
 * 
 ******************************************************************************/

void MPTRewirePanel::pollAudioRequests()
{
	// Wait for the device to request audio from us
	if(!waitForEventFromDevice()) return;

	// Read requested audio buffer properties
	MPTRequestAudio request;
	ReWire_uint16_t messageSize = 0;
	ReWireError status = RWPComRead(m_PanelPortHandle, PIPE_RT, &messageSize, m_Message);
	if(kReWireError_NoError != status)
	{
		if(kReWireError_NoMoreMessages != status)
			DEBUG_PRINT("RWPComRead returned %i.\n", (int)status);
		return;
	}

	if(messageSize < sizeof(MPTRequestAudio)) return;  // prevent potential access violation
	memcpy(&request, m_Message, messageSize);

	// Handle changes in samplerate and buffer size
	// This also happens after opening the panel to (re-)allocate the buffers
	if(m_SampleRate != request.sampleRate || m_MaxBufferSize != request.maxBufferSize)
	{
		handleAudioInfoChange(request.sampleRate, request.maxBufferSize);
	}

	// Handle sync requests
	if(request.shouldSync)
	{
		DEBUG_PRINT("Should sync. request.syncPos15360PPQ = %i\n", request.syncPos15360PPQ);
		m_SyncPos15360PPQ = request.syncPos15360PPQ;
		m_GonnaSync = true;
	}

	swallowRemainingMessages();
	generateAudioAndUploadToDevice(request);
}



void MPTRewirePanel::generateAudioAndUploadToDevice(MPTRequestAudio request)
{
	// Let OpenMPT render the audio channels
	ReWireClearBitField(m_ServedChannelsBitfield, kReWireAudioChannelCount / 2);
	m_RenderCallback(request.framesToRender, m_CallbackUserData);

	// Inform the device that we are going to send audio packets
	sendAudioResponseHeaderToDevice();

	// Send response for each interleaved stereo channel
	uint16_t audioDataSize = (uint16_t)(request.framesToRender * 2 * sizeof(int32_t));
	uint16_t responseSize = (uint16_t)(sizeof(MPTAudioResponse) + audioDataSize);
	for(uint16_t channel = 0; channel < kReWireAudioChannelCount / 2; channel++)
	{

		// Only serve rendered channels
		if(!ReWireIsBitInBitFieldSet(m_ServedChannelsBitfield, channel))
			continue;

		// Send channel to device
		m_AudioResponseBuffer->channelIndex = channel;
		uint8_t *pDest = reinterpret_cast<uint8_t *>(m_AudioResponseBuffer) + sizeof(MPTAudioResponse);
		memcpy(pDest, reinterpret_cast<void *>(m_AudioBuffers[channel]), audioDataSize);

		ReWireError status = RWPComSend(m_PanelPortHandle, PIPE_RT, responseSize, (uint8_t *)m_AudioResponseBuffer);
		if(kReWireError_NoError != status)
		{
			DEBUG_PRINT("RWPComSend status=%i channel=%i\n", (int)status, (int)channel);
			break;
		}

		// Signal to device that we have just sent a channel
		SetEvent(m_EventToDevice);

		// ... (Device is going to process our channel) ...

		// Wait for device to signal that it received our channel
		if(!waitForEventFromDevice()) break;
	}
}



bool MPTRewirePanel::sendAudioResponseHeaderToDevice()
{
	MPTAudioResponseHeader packet;
	memcpy((uint8_t *)&packet.servedChannelsBitfield, m_ServedChannelsBitfield, sizeof(MPTAudioResponseHeader::servedChannelsBitfield));

	ReWireError status = RWPComSend(m_PanelPortHandle, PIPE_RT, sizeof(packet), (uint8_t *)&packet);
	if(kReWireError_NoError != status)
	{
		DEBUG_PRINT("sendAudioResponseHeaderToDevice(): RWPComSend status=%i\n", (int)status);
		return false;
	}

	SetEvent(m_EventToDevice);
	return waitForEventFromDevice();
}



void MPTRewirePanel::handleAudioInfoChange(int sampleRate, int maxBufferSize)
{
	DEBUG_PRINT("Samplerate = %i, MaxBufferSize = %i\n", sampleRate, maxBufferSize);
	reallocateBuffers(maxBufferSize);

	// If this function was called because we received our first audio request,
	// then we do not need to notify the ReWire sound device.
	if(0 != m_SampleRate)
	{
		m_AudioInfoCallback(sampleRate, maxBufferSize, m_CallbackUserData);
	}

	m_SampleRate = sampleRate;
}



/*******************************************************************************
 *
 * Event signalling functions that send requests to the mixer.
 * 
 ******************************************************************************/

void MPTRewirePanel::signalPlayAndSync(double bpm)
{
	signalBPMChange(bpm);

	MPTRequestPlay reqPlay;
	reqPlay.type = (uint8_t)MPTRequestType::Play;
	reqPlay.tempo1000 = static_cast<uint32_t>(bpm * 1000);
	RWPComSend(m_PanelPortHandle, PIPE_EVENTS, sizeof(reqPlay), reinterpret_cast<uint8_t *>(&reqPlay));

	MPTRequestReposition reqRepos;
	reqRepos.type = (uint8_t)MPTRequestType::Reposition;
	reqRepos.position15360PPQ = 0;
	RWPComSend(m_PanelPortHandle, PIPE_EVENTS, sizeof(reqRepos), reinterpret_cast<uint8_t *>(&reqRepos));
	// double delayInSeconds = (double)nFrames / m_SampleRate;
	// double beatsPassed = delayInSeconds * (bpm / 60.0);
	// reqRepos.position15360PPQ = static_cast<int32_t>(-15360 * beatsPassed); // 1 beat = 15360 PPQ

}

void MPTRewirePanel::signalStop()
{
	MPTRequestStop req;
	req.type = (uint8_t)MPTRequestType::Stop;
	RWPComSend(m_PanelPortHandle, PIPE_EVENTS, sizeof(req), reinterpret_cast<uint8_t *>(&req));
}

void MPTRewirePanel::signalBPMChange(double bpm)
{
	MPTRequestBPM req;
	req.type = (uint8_t)MPTRequestType::ChangeBPM;
	req.tempo1000 = (uint32_t)(bpm * 1000);
	RWPComSend(m_PanelPortHandle, PIPE_EVENTS, sizeof(req), reinterpret_cast<uint8_t *>(&req));
}
#endif