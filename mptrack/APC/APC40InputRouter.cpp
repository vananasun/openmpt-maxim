#ifdef MPT_WITH_APC
#include <APCAPI.h>
#include "../Mainfrm.h"
#include "../MPTrack.h"
#include "../Moddoc.h"
#include "./APC40.h"

void APC40::tick()
{
	if (!APC40::Enabled) return;
	if (m_apcTicking) return; // prevents multiple instances from running the tick() function during lag
	m_apcTicking = true;

	// Swallow messages if no module is open
	APCAPI::Event e;
	CModDoc *doc = CMainFrame::GetMainFrame()->GetActiveDoc();
	if(!doc)
	{
		while(theApp.m_apc40->m_api->poll(&e)) {};
		m_apcTicking = false;
		return;
	}

	m_sampleRecorder->recordHandler();


	// Poll inputs
	CSoundFile &sndFile = doc->GetSoundFile();
	while(theApp.m_apc40->m_api->poll(&e))
	{

		switch(e.type)
		{
			
			
			case APCAPI::EventType::PlayStop: {
				if(e.value) doc->OnPatternRestart(true);
			} break;

			case APCAPI::EventType::Record: {
				if(e.value) m_sampleRecorder->toggle();
			} break;

			case APCAPI::EventType::Metronome: {
				if(e.value) sndFile.m_apc40ControlMap->toggleAssignmentMode();
			} break;

			case APCAPI::EventType::User: {
				if(e.value) setGlobalParamRecord(sndFile, !m_recordAllParams);
			} break;


			case APCAPI::EventType::TrackKnob:
			case APCAPI::EventType::DeviceKnob: {
				int controlId = (e.type == APCAPI::EventType::DeviceKnob ? 8 : 0) + e.trackId;
				if (sndFile.m_apc40ControlMap->isInAssignmentMode())
				{
					sndFile.m_apc40ControlMap->mapControlToTarget(controlId);
					sndFile.m_apc40ControlMap->toggleAssignmentMode();
				} else if (sndFile.m_apc40ControlMap->doesControlHaveAssignment(controlId))
				{
					sndFile.m_apc40ControlMap->updatePluginParamValue(sndFile, controlId, e.value);
				}
			} break;

			
			case APCAPI::EventType::Tempo: {
				TEMPO newTempo = sndFile.m_nDefaultTempo + TEMPO(e.direction, 0);
				sndFile.m_nDefaultTempo = newTempo;
				sndFile.SetTempo(newTempo, true);
			} break;


			/* Track specific stuff */

			case APCAPI::EventType::TrackSelector: {
				if(e.value) setChannelPage(sndFile, e.trackId);
			} break;
			case APCAPI::EventType::TrackActivator: {
				if(e.value) toggleChannel(doc, trackToChannelIndex(e.trackId));
			} break;
			case APCAPI::EventType::TrackSolo: {
				if(e.value) toggleSolo(doc, e.trackId);
			} break;
			case APCAPI::EventType::TrackRecord: {
				if(e.value) toggleMutePending(doc, e.trackId);
			} break;
				
		}
	}

	m_apcTicking = false;

}

#endif