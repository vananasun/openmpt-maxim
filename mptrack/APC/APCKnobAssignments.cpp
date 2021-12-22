#ifdef MPT_WITH_APC
//#include <APC40MkII.h>
//#include "../MPTrack.h"
//#include "../Mainfrm.h"
//#include "./APC.h"
//
//void APC::LoadKnobAssignments(CSoundFile &sndFile)
//{
//	// Set knob LEDs to plugin parameter values
//	for(int iKnob = 0; iKnob < NUM_ASSIGNABLE_KNOBS; iKnob++)
//	{
//
//		// Get value to set the LED to
//		KnobAssociation assoc = sndFile.m_apcSavedData.m_knob[iKnob];
//		SNDMIXPLUGIN &plugin = sndFile.m_MixPlugins[assoc.pluginId];
//		unsigned char paramValue = plugin.pMixPlugin ? static_cast<unsigned char>(plugin.pMixPlugin->GetParameter(assoc.paramId) * 127.0f)
//													 : 0;
//
//		// Set LED
//		if(iKnob > 8)
//		{
//			theApp.m_apc40mkii->setDeviceControlKnob(iKnob - 8, paramValue);
//		} else
//		{
//			theApp.m_apc40mkii->setAssignableKnob(iKnob, paramValue);
//		}
//	}
//}

#endif