#ifdef MPT_WITH_APC 
#pragma once
#include <APCAPI.h>
#include "./APC40Recorder.h"
#include "./APC40ControlMap.h"

class CModDoc;

class APC40
{
private:
	int m_channelPage;
	bool m_apcTicking = false; // prevents multiple instances from running the tick() function during lag
	bool m_unsoloChannels[MAX_CHANNELS];
	bool m_recordAllParams = false;



public:
	static bool Enabled;

	APCAPI::APC40MkII *m_api;
	APC40Recorder *m_sampleRecorder;
	uint32 m_rowLedsIndex = (uint32)-1; // display pattern index LEDs optimization


	APC40();
	void tick();
	
	void onNewDocument(CSoundFile &sndFile);
	void onOpenDocument(CSoundFile &sndFile);
	void onPlay();
	void onStop();

	void updateTrackLEDs(CSoundFile &sndFile);
	void setChannelLED(CModDoc *doc, CHANNELINDEX nChn, bool enabled);
	void toggleChannel(CModDoc *doc, CHANNELINDEX nChn);
	void toggleSolo(CModDoc *doc, int trackId);
	void toggleMutePending(CModDoc *doc, int trackId);
	void setGlobalParamRecord(CSoundFile &sndFile, bool enabled);

	void setChannelPage(CSoundFile &sndFile, int nPage);
	int channelToTrackIndex(CHANNELINDEX nChn);
	CHANNELINDEX trackToChannelIndex(int trackId);
};
#endif