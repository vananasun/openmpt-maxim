/*
 * SoundDeviceRewire.cpp
 * --------------------------
 * Purpose: ReWire sound device.
 * Notes  : (currently none)
 * Authors: Maxim van Dijk
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */


#if defined(MPT_WITH_REWIRE)

#include "stdafx.h"

#include "SoundDeviceReWire.hpp"
#include "SoundDevice.hpp"
#include "SoundDeviceUtilities.hpp"

#include "../mptrack/rewire/MPTRewirePanel.h"

#include "../common/misc_util.h"
#include "../common/mptStringBuffer.h"
#include "../mptrack/Reporting.h"
#include "../mptrack/Mainfrm.h"
#include "../mptrack/Moddoc.h"


OPENMPT_NAMESPACE_BEGIN


namespace SoundDevice {
	
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Shell32.lib")

	

bool PanelStreamCallbackWrapper(unsigned int framesToRender, void *userData) {
	return reinterpret_cast<CReWireDevice *>(userData)->PanelStreamCallback(framesToRender);
}

void PanelAudioInfoCallbackWrapper(unsigned int sampleRate, unsigned int maxBufferSize, void *userData) {
	return reinterpret_cast<CReWireDevice *>(userData)->PanelAudioInfoCallback(sampleRate, maxBufferSize);
}

void PanelMixerQuitCallbackWrapper(void *userData) {
	return reinterpret_cast<CReWireDevice *>(userData)->PanelMixerQuitCallback();
}





CReWireDevice::CReWireDevice(ILogger &logger, SoundDevice::Info info, SoundDevice::SysInfo sysInfo)
	: SoundDevice::Base(logger, info, sysInfo)
{
	m_MasterBuffer = (void *)malloc(static_cast<size_t>(8192) * 2 * sizeof(float));
	m_Panel = new MPTRewirePanel();
}

CReWireDevice::~CReWireDevice() {
	if(m_MasterBuffer) free(m_MasterBuffer);  // @TODO: THIS SHOULD BE REMOVED COMPLETELY FOR PERFORMANCE!
	delete m_Panel;
	Close();
}

bool CReWireDevice::InternalOpen() {
	if(m_Open) return true; // true because we do not want to alarm anyone :)
	if(m_Panel->m_Errored) return false;
	const char *errorString = "ReWire Sound Device: An unexpected error has occurred.";
	switch(m_Panel->open(PanelStreamCallbackWrapper, PanelAudioInfoCallbackWrapper, PanelMixerQuitCallbackWrapper, this))
	{
		case MPTPanelStatus::Ok: {
			m_Open = true;
		} return true;
		case MPTPanelStatus::MixerNotRunning:
			errorString = "ReWire Sound Device: No mixer app is currently running; Start the mixer app and then retry.";
			break;
		/*case MPTPanelStatus::UnableToRegisterDevice:
			errorString = "ReWire Sound Device: Unable to register the device.\nHave you tried running OpenMPT as administrator?";
			break;*/
		case MPTPanelStatus::DeviceNotInstalled:
			errorString = "ReWire Sound Device: The device was not recognised by the ReWire subsystem.\nDoes MPTRewire.dll exist?";
			break;
		//case MPTPanelStatus::ReWireProblem:
		//	errorString = "ReWire Sound Device: There was a problem with ReWire while connecting to the device.\nTry restarting your computer.";
		//	break;
		case MPTPanelStatus::UnknownDeviceProblem:
			errorString = "ReWire Sound Device: A problem occurred from within the device!\nTry restarting your computer.";
			break;
	}
	m_Panel->close();
	Reporting::Error(errorString);
	return false;
}

bool CReWireDevice::InternalClose()
{
	if (m_RequestedClose) {
		m_RequestedClose = false;
		m_Open = false;
		return m_Panel->close();
	}
	return true;
}

bool CReWireDevice::InternalStart() { return true; }

void CReWireDevice::InternalStop() {}

void CReWireDevice::InternalFillAudioBuffer()
{
	m_FramesDoneDoubled = 0;
	CallbackLockedAudioReadPrepare(m_FramesToRender, 0);
	CallbackLockedAudioProcessVoid(m_MasterBuffer, nullptr, m_FramesToRender);
	CallbackLockedAudioProcessDone();
}

int64 CReWireDevice::InternalGetStreamPositionFrames() const
{
	return 0;
}

SoundDevice::BufferAttributes CReWireDevice::InternalGetEffectiveBufferAttributes() const
{
	SoundDevice::BufferAttributes bufferAttributes;
	int sampleRate = m_Panel->m_SampleRate ? m_Panel->m_SampleRate : 44100;
	bufferAttributes.Latency = static_cast<double>(m_Panel->m_MaxBufferSize) / static_cast<double>(sampleRate);
	bufferAttributes.UpdateInterval = static_cast<double>(m_Panel->m_MaxBufferSize) / static_cast<double>(sampleRate);
	bufferAttributes.NumBuffers = 1;
	return bufferAttributes;
}

SoundDevice::Statistics CReWireDevice::GetStatistics() const {
	SoundDevice::Statistics result;
	result.InstantaneousLatency = m_Settings.Latency;
	result.LastUpdateInterval = m_Settings.UpdateInterval;
	result.text = U_("ReWire support for Ableton.\nE-mail mvandijk303@gmail.com for bug reports etc! <3");
	return result;
}

SoundDevice::Caps CReWireDevice::InternalGetDeviceCaps() {
	SoundDevice::Caps caps;

	caps.Available = true;
	caps.CanUpdateInterval = false;
	caps.CanSampleFormat = false;
	caps.CanExclusiveMode = false;
	caps.CanBoostThreadPriority = false;
	caps.CanKeepDeviceRunning = true;
	caps.CanUseHardwareTiming = false;
	caps.CanChannelMapping = false;
	caps.CanInput = false;
	caps.HasNamedInputSources = true;
	caps.CanDriverPanel = false;

	caps.LatencyMin = 0.0;         // disabled
	caps.LatencyMax = 0.0;         // disabled
	caps.UpdateIntervalMin = 0.0;  // disabled
	caps.UpdateIntervalMax = 0.0;  // disabled

	caps.DefaultSettings.Channels = 2;
	caps.DefaultSettings.sampleFormat = SampleFormat::Float32;

	return caps;
}

SoundDevice::DynamicCaps CReWireDevice::GetDeviceDynamicCaps(const std::vector<uint32>& baseSampleRates) {
	MPT_UNREFERENCED_PARAMETER(baseSampleRates);
	SoundDevice::DynamicCaps caps;
	// caps.supportedSampleRates = { 22050, 44100, 48000, 96000, 176400, 192000 };
	caps.supportedSampleRates = { 44100 };
	caps.supportedExclusiveSampleRates = { 0 };
	caps.supportedSampleFormats = {SampleFormat::Float32};
	caps.supportedExclusiveModeSampleFormats = {SampleFormat::Float32};
	return caps;
}

bool CReWireDevice::OpenDriverSettings() {
	return false;
}

bool CReWireDevice::OnIdle() {
	return false;
}

std::vector<SoundDevice::Info> CReWireDevice::EnumerateDevices(ILogger &logger, SoundDevice::SysInfo sysInfo) {
	MPT_UNREFERENCED_PARAMETER(logger);
	MPT_UNREFERENCED_PARAMETER(sysInfo);
	std::vector<SoundDevice::Info> devices;
	SoundDevice::Info info;
	info.type = TypeREWIRE;
	info.internalID = mpt::ufmt::dec(0);
	info.apiName = U_("REWIRE");
	info.useNameAsIdentifier = true;
	info.name = U_("ReWire");
	info.default_ = Info::Default::None;
	info.flags = {
		Info::Usability::Usable,
		Info::Level::Primary,
		Info::Compatible::No,
		Info::Api::Native,
		Info::Io::Unknown,
		Info::Mixing::Unknown,
		Info::Implementor::External
	};
	devices.push_back(info);
	return devices;
}

/**
 * Gets called by ReWire panel
**/
bool CReWireDevice::PanelStreamCallback(unsigned int framesToRender) {
	if(!IsPlaying()) return false;
	m_FramesToRender = framesToRender;
	CallbackFillAudioBufferLocked();
	return true;
}

/**
 * Called when the mixer app quit unexpectedly.
**/
void CReWireDevice::PanelMixerQuitCallback() {
	m_RequestedClose = true;
	RequestClose();
	Reporting::Warning("The ReWire mixer application has quit.");
}




uint32 g_RewireRequestedSampleRate = 0; // yes, I know. Global.

/**
 * Called when mixer app's sample rate or buffer size changed.
**/
void CReWireDevice::PanelAudioInfoCallback(int sampleRate, int maxBufferSize)
{
	MPT_UNREFERENCED_PARAMETER(maxBufferSize);
	
	g_RewireRequestedSampleRate = sampleRate;
	m_Settings.Samplerate = sampleRate;
	m_RequestedClose = true;
	RequestReset();
		
	/*Reporting::Notification(
		MPT_UFORMAT("The ReWire mixer application's sample rate and buffer size have changed to {} frames @ {}Hz.")
		(sampleRate, maxBufferSize)
	);*/

}



} // namespace SoundDevice

OPENMPT_NAMESPACE_END

#endif  // MPT_WITH_REWIRE