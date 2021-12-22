//#pragma once
//#include <vector>
//
//typedef struct
//{
//	int pluginId;
//	int paramId;
//} KnobAssociation;
//
//#define NUM_ASSIGNABLE_KNOBS 24
//
//class APCSavedData
//{
//public:
//
//	KnobAssociation m_knob[NUM_ASSIGNABLE_KNOBS];
//
//
//	APCSavedData();
//
//
//	/**
//	 * knobId: 0 - 7 for assignable knobs, 8 - 15 for device control knobs, 16 - 23 for track faders.
//	 * pluginId: the plugin ID number
//	 * paramId: the plugin's parameter ID
//	**/
//	void mapKnobToParam(int knobId, int pluginId, int paramId);
//
//
//	/**
//	 * Clears all knob associations.
//	**/
//	void reset();
//};