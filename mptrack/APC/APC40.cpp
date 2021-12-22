#ifdef MPT_WITH_APC
#include "../../soundlib/plugins/PlugInterface.h"
#include "../MPTrack.h"
#include "../Moddoc.h"
#include "./APC40.h"
#include <APCAPI.h>

static void OnError(const std::string &msg, void *userData)
{
	MPT_UNREFERENCED_PARAMETER(userData);
	Reporting::Warning(msg, nullptr);
	APC40::Enabled = false;
}

bool APC40::Enabled = false;



APC40::APC40()
{
	m_api = new APCAPI::APC40MkII(OnError, this);
	if (m_api->connect())
	{
		m_sampleRecorder = new APC40Recorder();
		APC40::Enabled = true;
	} else
	{
		APC40::Enabled = false;
	}
}


void APC40::onNewDocument(CSoundFile &sndFile)
{
	m_api->resetDisplay();
	sndFile.m_apc40ControlMap->clearAll();
	setChannelPage(sndFile, 0);
	m_recordAllParams = false;
}

void APC40::onOpenDocument(CSoundFile &sndFile)
{
	m_api->resetDisplay();
	sndFile.m_apc40ControlMap->updateLEDs(sndFile);
	setChannelPage(sndFile, 0);
	setGlobalParamRecord(sndFile, false);
}

void APC40::onPlay()
{
	m_api->setPlay(true);
}

void APC40::onStop()
{
	m_api->setPlay(false);
}


void APC40::updateTrackLEDs(CSoundFile &sndFile)
{
	for(int iTrack = 0; iTrack < 8; iTrack++)
	{
		int iChannel = (8 * m_channelPage) + iTrack;
		if (iChannel >= sndFile.GetNumChannels())
		{
			m_api->setTrackActivator(iTrack, false);
			m_api->setTrackAB(iTrack, 0);
			m_api->setTrackSolo(iTrack, false);
			m_api->setTrackRecord(iTrack, false);
		}
		else
		{
			m_api->setTrackActivator(iTrack, !sndFile.ChnSettings[iChannel].dwFlags[CHN_MUTE]);
			m_api->setTrackAB(iTrack, false);  // @TODO: Crossfader A|B ??
			m_api->setTrackSolo(iTrack, sndFile.ChnSettings[iChannel].dwFlags[CHN_SOLO]);
			m_api->setTrackRecord(iTrack, sndFile.m_bChannelMuteTogglePending[iChannel]);
		}

	}
}




void APC40::setChannelLED(CModDoc *doc, CHANNELINDEX nChn, bool enabled)
{
	if(nChn >= doc->GetNumChannels()) return;
	if(nChn >= (8 * m_channelPage + 8)) return;
	m_api->setTrackActivator(channelToTrackIndex(nChn), enabled);
}

void APC40::toggleChannel(CModDoc *doc, CHANNELINDEX nChn)
{
	if (nChn >= doc->GetNumChannels()) return;

	doc->MuteChannel(nChn, !doc->IsChannelMuted(nChn)); // automatically sets LED
	
	// Make any solo channel disappear
	for (CHANNELINDEX iChannel = 0; iChannel < doc->GetNumChannels(); iChannel++)
	{
		if (doc->IsChannelSolo(iChannel))
		{
			doc->SoloChannel(iChannel, false);
		}
	}
	for (int iTrack = 0; iTrack < 8; iTrack++)
	{
		m_api->setTrackSolo(iTrack, false);
	}

	doc->UpdateAllViews(GeneralHint(nChn).Channels());
}






void APC40::toggleSolo(CModDoc *doc, int trackId)

{	CHANNELINDEX nChn = trackToChannelIndex(trackId);
	if(nChn >= doc->GetNumChannels()) return;

	if(doc->IsChannelSolo(nChn))
	{
		// Reset all channels to original state and unsolo them
		for(CHANNELINDEX i = 0; i < doc->GetNumChannels(); i++)
		{
			// Actually, don't mute any channels, ONLY unmute them
			if (m_unsoloChannels[i]) doc->MuteChannel(i, false);
			//doc->MuteChannel(i, !m_unsoloChannels[i]);
			doc->SoloChannel(i, false);
		}
	} else
	{
		// Find whether another channel is already solo'd
		CHANNELINDEX nAlreadySoloChn = (CHANNELINDEX)-1;
		for(CHANNELINDEX i = 0; i < doc->GetNumChannels(); i++)
		{
			if (doc->IsChannelSolo(i))
			{
				nAlreadySoloChn = i;
				break;
			}
		}

		if (nAlreadySoloChn != (CHANNELINDEX)-1)
		{
			// Just move the solo toward the new channel,
			// and turn off the other channel.
			doc->MuteChannel(nChn, false);
			doc->SoloChannel(nChn, true);
			doc->MuteChannel(nAlreadySoloChn, true);
			doc->SoloChannel(nAlreadySoloChn, false);
		} else
		{
			// Remember all enabled channels,
			// mute all channels except solo'd channel,
			// unmute & solo solo'd channel.
			for(CHANNELINDEX i = 0; i < doc->GetNumChannels(); i++)
			{
				m_unsoloChannels[i] = !doc->IsChannelMuted(i);
				if (i != nChn) doc->MuteChannel(i, true);
			}
			doc->MuteChannel(nChn, false);
			doc->SoloChannel(nChn, true);
		}

	}

	doc->UpdateAllViews(GeneralHint(nChn).Channels());
}



void APC40::toggleMutePending(CModDoc *doc, int trackId)
{
	CHANNELINDEX nChannel = trackToChannelIndex(trackId);
	if (nChannel >= doc->GetNumChannels()) return;
	doc->GetSoundFile().m_bChannelMuteTogglePending[nChannel] = !doc->GetSoundFile().m_bChannelMuteTogglePending[nChannel];
	m_api->setTrackRecord(trackId, doc->GetSoundFile().m_bChannelMuteTogglePending[nChannel]);
}


void APC40::setGlobalParamRecord(CSoundFile &sndFile, bool enabled)
{
	m_recordAllParams = enabled;
	for (PLUGINDEX iPlug = 0; iPlug < MAX_MIXPLUGINS; iPlug++)
	{
		SNDMIXPLUGIN &pl = sndFile.m_MixPlugins[iPlug];
		if (!pl.pMixPlugin) continue;
		pl.pMixPlugin->m_recordAutomation = m_recordAllParams;
	}
	m_api->setUser(enabled);
}



void APC40::setChannelPage(CSoundFile &sndFile, int nPage)
{
	m_channelPage = nPage;
	m_api->setTrackSelector(nPage);
	updateTrackLEDs(sndFile);
}

int APC40::channelToTrackIndex(CHANNELINDEX nChn)
{
	if(nChn >= (8 * m_channelPage + 8)) return -1;
	return nChn - (8 * m_channelPage);
};

CHANNELINDEX APC40::trackToChannelIndex(int trackId)
{
	return static_cast<CHANNELINDEX>(8 * m_channelPage + trackId);
}


#endif