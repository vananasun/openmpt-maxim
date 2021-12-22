#pragma once
#ifdef MPT_WITH_REWIRE
#include <chrono>
#include <thread>
#include <string>
#include <stdint.h>

#include "MPTRewire.h"

#define PIPE_EVENTS 0
#define PIPE_RT     1  // realtime audio thread


enum class MPTPanelStatus
{
	Ok = 0,
	MixerNotRunning = 1,
	UnableToRegisterDevice = 2,
	UnknownDeviceProblem = 3,
	ReWireProblem = 4,
	DeviceNotInstalled = 5,
	FirstTime = 6,
};


/*******************************************************************************
 *
 * Requests that get sent from the panel to the device.
 *
 ******************************************************************************/

enum class MPTRequestType
{
	Play = 0,
	Stop = 1,
	ChangeBPM = 2,
	Reposition = 3
};

struct MPTRequest { uint8_t type; uint32_t _0; };

struct MPTRequestPlay {
	uint8_t type;
	uint32_t tempo1000;
};

typedef struct {
	uint8_t type;
} MPTRequestStop;

typedef struct {
	uint8_t type;
	int32_t position15360PPQ;
} MPTRequestReposition;

typedef struct {
	uint8_t type;
	uint32_t tempo1000;
} MPTRequestBPM;

typedef struct {
	int32_t sampleRate;
	int32_t maxBufferSize;
	uint32_t framesToRender;
	int32_t syncPos15360PPQ;
	bool shouldSync;
} MPTRequestAudio;

typedef struct {
	int32_t sampleRate;
	int32_t maxBufferSize;
} MPTRequestAudioInfo;



/* Responses from device that get sent back to panel */

typedef struct
{
	uint32_t servedChannelsBitfield[4];  // 128 bits, one stereo channel for each bit
} MPTAudioResponseHeader;                // sent once before a bunch of MPTAudioResponse packets are sent

typedef struct
{
	uint16_t channelIndex;  // @TODO: optimize this away for performance reasons, then this struct becomes:
							//        typedef int32_t* MPTAudioResponse;
	// <interleaved audio channel (2 * fFramesToRender * sizeof(int))>
} MPTAudioResponse;

/*******************************************************************************
 ******************************************************************************/


typedef bool (*MPTRenderCallback)(unsigned int framesToRender, void *userData);
typedef void (*MPTAudioInfoCallback)(unsigned int sampleRate, unsigned int maxBufferSize, void *userData);
typedef void (*MPTMixerQuitCallback)(void *userData);
typedef void *TRWPPortHandle;
typedef void *HANDLE;



class MPTRewirePanel
{
private:
	std::thread m_Thread;
	bool m_Running = false;
	bool m_MixerQuit = false;
	const char *m_DeviceName = "OpenMPT";

	void *m_CallbackUserData = nullptr;
	MPTRenderCallback m_RenderCallback = nullptr;
	MPTAudioInfoCallback m_AudioInfoCallback = nullptr;
	MPTMixerQuitCallback m_MixerQuitCallback = nullptr;

	MPTAudioResponse *m_AudioResponseBuffer = nullptr;
	TRWPPortHandle m_PanelPortHandle = nullptr;
	uint8_t m_Message[8192];
	uint32_t m_ServedChannelsBitfield[4];  // 128 bits

	// Signals to device whenever an audio buffer was sent by us.
	HANDLE m_EventToDevice;

	// Lets us know when an audio buffer sent by us was received and processed by the device.
	HANDLE m_EventFromDevice;


	void deallocateBuffers();
	void reallocateBuffers(int32_t maxBufferSize);
	void checkComConnection();
	void handleAudioInfoChange(int sampleRate, int maxBufferSize);
	void pollAudioRequests();
	bool waitForEventFromDevice(const int milliseconds = 100);
	void swallowRemainingMessages();
	void generateAudioAndUploadToDevice(MPTRequestAudio incomingRequest);
	bool sendAudioResponseHeaderToDevice();



public:
	bool m_Errored = false;
	int m_SampleRate = 0;
	int m_MaxBufferSize = 0;

	// Interleaved buffers
	int **m_AudioBuffers = nullptr;

	// Whether to wait for the next audio render request where we will sync OpenMPT
	// with the Mixer App's position.
	bool m_GonnaSync = false;

	// Mixer position to synchronize to.
	int32_t m_SyncPos15360PPQ = 0;



	MPTRewirePanel();
	~MPTRewirePanel();
	MPTPanelStatus open(
		MPTRenderCallback renderCallback,
		MPTAudioInfoCallback audioInfoCallback,
		MPTMixerQuitCallback mixerQuitCallback,
		void *callbackUserData
	);
	bool close();
	void threadProc();
	bool isRunning() { return m_Running; }
	void stop() { m_Running = false; }
	inline void markChannelAsRendered(int index) {
		m_ServedChannelsBitfield[index >> 5] |= 1 << (index & 0x1f);
	}

	void signalPlayAndSync(double bpm);
	void signalStop();
	void signalBPMChange(double bpm);

};
#endif
