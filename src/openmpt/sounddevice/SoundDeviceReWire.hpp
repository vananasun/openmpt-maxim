/*
 * SoundDeviceReWire.h
 * ------------------------
 * Purpose: ReWire sound device driver class.
 * Notes  : (currently none)
 * Authors: Maxim van Dijk
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */


#pragma once

#include "openmpt/all/BuildSettings.hpp"

#include "SoundDevice.hpp"
#include "SoundDeviceUtilities.hpp"

#include "../common/ComponentManager.h"

#include "../mptrack/rewire/ReWireAPI.h"
#include "../mptrack/rewire/ReWirePanelAPI.h"
#include "../mptrack/rewire/MPTRewirePanel.h"


OPENMPT_NAMESPACE_BEGIN

namespace SoundDevice
{

#if defined(MPT_WITH_REWIRE)

class ComponentReWire : public ComponentBuiltin
{
	MPT_DECLARE_COMPONENT_MEMBERS(ComponentReWire, "ReWire")
public:
	ComponentReWire() {}
	virtual ~ComponentReWire() {}
	bool DoInitialize() override { return true; }
};

#define REWIRE_CHANNEL_COUNT 128


extern uint32 g_RewireRequestedSampleRate; // for when switching sample rates


class CReWireDevice : public SoundDevice::Base
{
private:
	bool m_Open = false;
	bool m_RequestedClose = false;
	void *m_MasterBuffer = nullptr;

public:
	MPTRewirePanel *m_Panel = nullptr;

	// Number of frames requested by ReWire.
	unsigned long m_FramesToRender;

	// Number of frames that have already been rendered, goes on until (m_FramesDoneDoubled / 2 ) == m_FramesToRender,
	// resets in InternalFillAudioBuffer().
	unsigned long m_FramesDoneDoubled = 0;




	CReWireDevice(ILogger &logger, SoundDevice::Info info, SoundDevice::SysInfo sysInfo);
	~CReWireDevice();
	
	bool InternalOpen();
	bool InternalClose();
	void InternalFillAudioBuffer();
	bool InternalStart();
	void InternalStop();
	bool InternalIsOpen() const { return m_Open; }
	bool InternalHasGetStreamPosition() const { return false; }
	int64 InternalGetStreamPositionFrames() const;
	SoundDevice::BufferAttributes InternalGetEffectiveBufferAttributes() const;
	SoundDevice::Statistics GetStatistics() const;
	SoundDevice::Caps InternalGetDeviceCaps();
	SoundDevice::DynamicCaps GetDeviceDynamicCaps(const std::vector<uint32> &baseSampleRates);
	bool OpenDriverSettings();
	bool OnIdle();
	
	static std::unique_ptr<SoundDevice::BackendInitializer> BackendInitializer() { return std::make_unique<SoundDevice::BackendInitializer>(); }
	static std::vector<SoundDevice::Info> EnumerateDevices(ILogger &logger, SoundDevice::SysInfo sysInfo);

	bool PanelStreamCallback(unsigned int framesToRender);
	void PanelAudioInfoCallback(int sampleRate, int maxBufferSize);
	void PanelMixerQuitCallback();

};

#endif // MPT_WITH_REWIRE


}; // namespace SoundDevice


OPENMPT_NAMESPACE_END
