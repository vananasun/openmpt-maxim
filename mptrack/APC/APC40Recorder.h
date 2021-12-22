#ifdef MPT_WITH_APC
#pragma once

class APC40Recorder
{
private:

	struct MptApcMem
	{
		int state;
		bool resetOpenMPT;
	};

	MptApcMem *m_sharedMem = 0;
	bool m_connected = false;
	bool m_errored = false;

	bool establishConnectionWithVST();
	bool loadRecordedSample();
	void resetRecordFunctionality();

public:
	void recordHandler();
	void toggle();

};
#endif