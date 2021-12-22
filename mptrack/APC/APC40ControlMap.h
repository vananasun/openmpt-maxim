#ifdef MPT_WITH_APC
#pragma once

#define NUM_ASSIGNABLE_CONTROLS 16

class APC40ControlMap
{
private:

	struct Association
	{
		PLUGINDEX pluginId;
		int paramId;
	};
	/** \brief 0-7 are track knobs, 8-15 are device knobs */
	Association m_assocs[NUM_ASSIGNABLE_CONTROLS];

	PLUGINDEX m_targetPlugin = 0;
	int m_targetParam = -1;
	bool m_enabled = false;

public:

	void toggleAssignmentMode();
	bool isInAssignmentMode() { return m_enabled; };
	void clearAll();
	void setControlLED(int controlId, uint8 value);
	void setTargetParam(PLUGINDEX pluginId, int32 paramId);
	void mapControlToTarget(int controlId);
	bool doesControlHaveAssignment(int controlId);
	void updatePluginParamValue(CSoundFile &sndFile, int controlId, uint8 value);
	void setLEDFromVSTParam(IMixPlugin *pMixPlug, int paramId);
	bool write(std::ostream &oStrm, const CSoundFile &sndFile);
	bool read(std::istream &iStrm, CSoundFile &sndFile);
	void updateLEDs(CSoundFile &sndFile);
};
#endif