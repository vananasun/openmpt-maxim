#ifdef MPT_WITH_APC
#include <APCAPI.h>
#include "../../soundlib/plugins/PlugInterface.h"
#include "../MPTrack.h"
#include "../Mainfrm.h"
#include "../Moddoc.h"
#include "./APC40.h"
#include "./APC40ControlMap.h"
#include "./WinEvent.h"

void APC40ControlMap::toggleAssignmentMode() {
	m_enabled ^= true;
	theApp.m_apc40->m_api->setMetronome(m_enabled);
	if (!m_enabled) m_targetParam = -1;
};


void APC40ControlMap::clearAll()
{
	for(int iControl = 0; iControl < NUM_ASSIGNABLE_CONTROLS; iControl++)
	{
		m_assocs[iControl].pluginId = 0;
		m_assocs[iControl].paramId = -1;
	}
	m_targetParam = -1;
}


void APC40ControlMap::setTargetParam(PLUGINDEX pluginId, int32 paramId)
{
	m_targetPlugin = pluginId;
	m_targetParam = paramId;
}


void APC40ControlMap::mapControlToTarget(int controlId)
{
	if(m_targetParam == -1) return;
	m_assocs[controlId] = { m_targetPlugin, m_targetParam };
}


bool APC40ControlMap::doesControlHaveAssignment(int controlId)
{
	return (-1 != m_assocs[controlId].paramId);
}


void APC40ControlMap::updatePluginParamValue(CSoundFile &sndFile, int controlId, uint8 value)
{
	if(m_assocs[controlId].paramId == -1) return;

	SNDMIXPLUGIN &plugin = sndFile.m_MixPlugins[m_assocs[controlId].pluginId];
	if(plugin.pMixPlugin == nullptr) return;
	IMixPlugin *mixPlug = plugin.pMixPlugin;
	
	// @TODO: Could be done more smoothly later, because this is quantized through a 20ms timer
	mixPlug->SetParameter(m_assocs[controlId].paramId, (float)value / 127.0f);
	mixPlug->AutomateParameter(m_assocs[controlId].paramId);

	setControlLED(controlId, value);
}





/*******************************************************************************
 *
 * IO
 * 
 ******************************************************************************/

bool APC40ControlMap::write(std::ostream &oStrm, const CSoundFile &sndFile)
{
	MPT_UNREFERENCED_PARAMETER(sndFile);

	mpt::IO::WriteIntLE<int>(oStrm, 0); // for versioning

	for(int iControl = 0; iControl < NUM_ASSIGNABLE_CONTROLS; iControl++)
	{
		// MPT_LOG(LogDebug, "Load_it", MPT_UFORMAT("Writing knob {} which is mapped to param {} from plugin {}")(iControl, m_assocs[iControl].paramId, m_assocs[iControl].pluginId));
		mpt::IO::WriteIntLE<PLUGINDEX>(oStrm, m_assocs[iControl].pluginId);
		mpt::IO::WriteIntLE<int>      (oStrm, m_assocs[iControl].paramId);
	}
	return true;
}


bool APC40ControlMap::read(std::istream &iStrm, CSoundFile &sndFile)
{
	MPT_UNREFERENCED_PARAMETER(sndFile);

	int version;
	mpt::IO::ReadIntLE(iStrm, version);

	for(int iControl = 0; iControl < NUM_ASSIGNABLE_CONTROLS; iControl++)
	{
		// MPT_LOG(LogDebug, "Load_it", MPT_UFORMAT("Loading knob {} which is mapped to param {} from plugin {}")(iControl, m_assocs[iControl].paramId, m_assocs[iControl].pluginId));
		mpt::IO::ReadIntLE(iStrm, m_assocs[iControl].pluginId);
		mpt::IO::ReadIntLE(iStrm, m_assocs[iControl].paramId);
	}
	return true;
}



/*******************************************************************************
 *
 * LEDs
 * 
 ******************************************************************************/

/**
 *	\brief Abstraction to set the LED of a control by it's index.
 * 
 *	\param controlId - Index of the control
 *  \param value - LED value from 0 - 127
 */
void APC40ControlMap::setControlLED(int controlId, uint8 value)
{
	if(controlId < 8)
		theApp.m_apc40->m_api->setTrackKnob(controlId, value);
	else
		theApp.m_apc40->m_api->setDeviceKnob(controlId - 8, value);
}


/**
 *	\brief Sets a control's LED value to that of a given VST plugin's parameter.
 *
 *	\param pMixPlug - The VST plugin instance
 *  \param paramId - Parameter to use value of
 */
void APC40ControlMap::setLEDFromVSTParam(IMixPlugin *pMixPlug, int paramId)
{
	for(int iControl = 0; iControl < NUM_ASSIGNABLE_CONTROLS; iControl++)
	{
		if(m_assocs[iControl].pluginId == pMixPlug->GetSlot() && m_assocs[iControl].paramId == paramId)
		{
			setControlLED(iControl, (uint8)(pMixPlug->GetParameter(paramId) * 127.0f));
		}
	}
}


/**
 *	\brief Sets all knob LEDs to their corresponding parameter values.
 * 
 *	\param sndFile
 */
void APC40ControlMap::updateLEDs(CSoundFile &sndFile)
{
	uint8 paramValue;
	for(int iKnob = 0; iKnob < 8; iKnob++)
	{
		SNDMIXPLUGIN &pl = sndFile.m_MixPlugins[m_assocs[iKnob].pluginId];
		paramValue = pl.pMixPlugin ? (uint8)(pl.pMixPlugin->GetParameter(m_assocs[iKnob].paramId) * 127.0f) : 0;
		theApp.m_apc40->m_api->setTrackKnob(iKnob, paramValue);
	}
	for(int iKnob = 8; iKnob < 16; iKnob++)
	{
		SNDMIXPLUGIN &pl = sndFile.m_MixPlugins[m_assocs[iKnob].pluginId];
		paramValue = pl.pMixPlugin ? (uint8)(pl.pMixPlugin->GetParameter(m_assocs[iKnob].paramId) * 127.0f) : 0;
		theApp.m_apc40->m_api->setDeviceKnob(iKnob, paramValue);
	}
}


#endif