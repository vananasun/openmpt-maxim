#ifdef DEBUG
#ifdef MPT_WITH_REWIRE
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "./MPTRewireDebugUtils.h"

using namespace ReWire;


// Debug variables
int g_DummyDataIndexInt   = 0;
int g_DummyDataIndexFloat = 0;
LARGE_INTEGER g_DebugPerfFreq, g_DebugTicksNow, g_DebugTicksStart;
double g_DebugDiffMs;



const char *GetRWEventName(ReWireEvent *event)
{
	switch(event->fEventType)
	{
		case kReWireMIDIEvent: return "MIDI";
		case kReWireRequestRepositionEvent: return "Reposition";
		case kReWireRequestSignatureEvent: return "Signature";
		case kReWireRequestTempoEvent: return "Tempo";
		case kReWireRequestLoopEvent: return "Loop";
		case kReWireRequestPlayEvent: return "Play";
		case kReWireRequestStopEvent: return "Stop";
		case kReWireRevealEventTargetEvent: return "Reveal Event Target";
		case kReWireEventTargetChangedEvent: return "Event Target Changed";
		default: return "<Invalid>";
	}
}

void PrintRWEvent(ReWireEvent *event)
{
	char str[256];

	switch(event->fEventType)
	{
		case kReWireRequestRepositionEvent: {
			sprintf_s(str, sizeof(str), "fPPQ15360Pos: %li", ((ReWireRequestRepositionEvent*)event)->fPPQ15360Pos);
		} break;
			
		case kReWireRequestTempoEvent: {
			sprintf_s(str, sizeof(str), "fTempo: %lu", ((ReWireRequestTempoEvent*)event)->fTempo);
		} break;

		case kReWireRevealEventTargetEvent: {
			ReWireEventTarget *target = &((ReWireRevealEventTargetEvent*)event)->fEventTarget;
			sprintf_s(str, sizeof(str), "fMIDIBusIndex: %u, fChannel: %u",
				(unsigned int)target->fMIDIBusIndex,
				(unsigned int)target->fChannel
			);
		} break;

		default: {
			sprintf_s(str, sizeof(str), "<>");
		} break;
	}

	DEBUG_PRINT("ReWire %s Event: %s\n", GetRWEventName(event), str);
}


#endif // MPT_WITH_REWIRE
#endif // DEBUG