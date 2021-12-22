#pragma once
#ifdef MPT_WITH_REWIRE

//inline float ticksPerSecond(ReWire::ReWire_int32_t tempo)
//{
//	return (tempo / (float)1000) / 60 * 15360;
//}
//
//inline ReWire::ReWire_int32_t sec2ticks(float seconds, ReWire::ReWire_int32_t tempo)
//{
//	return (ReWire::ReWire_int32_t)(seconds * ticksPerSecond(tempo));
//}
//
//inline float ticks2sec(ReWire::ReWire_int32_t ticks, ReWire::ReWire_int32_t tempo)
//{
//	return ticks / ticksPerSecond(tempo);
//}

namespace MPTRewire
{

	/**
	 * \brief Calculates the amount of ReWire ticks in 15360 PPQ (per beat) for each
	 *        second that passes.
	 * 
	 * \param bpm - Tempo in beats per minute.
	 * 
	 * \returns Ticks in 15360 PPQ.
	**/
	template <typename T>
	inline T TicksPerSecond(T bpm)
	{
		// ReWire ticks per minute = bpm * 15360
		// ReWire ticks per second = ReWire ticks per minute / 60
		return (bpm * 15360 / 60);
	};

	/**
	 * \brief Converts a number of samples to the number of seconds passed.
	 * 
	 * \param samples
	 * \param sampleRate
	 * 
	 * \returns Seconds passed.
	**/
	template <typename T>
	inline float Samples2Seconds(T samples, T sampleRate)
	{
		return samples / static_cast<float>(sampleRate);
	};

	/**
	 * \brief Calculates the number of samples passed within a number of seconds.
	 * 
	 * \param seconds
	 * \param sampleRate
	 * 
	 * \returns Samples passed.
	**/
	template <typename T>
	inline T Seconds2Samples(T seconds, T sampleRate)
	{
		return seconds * sampleRate;
	};

	/**
	 * \brief Converts ReWire ticks to seconds.
	 * 
	 * \param ticks - Ticks in 15360 PPQ.
	 * \param bpm - Beats per minute.
	 * 
	 * \returns Seconds.
	**/
	template <typename T>
	inline float Ticks2Seconds(T ticks, T bpm)
	{
		return ticks / static_cast<float>(TicksPerSecond(bpm));
	};

	/**
	 * \brief Converts seconds to ReWire ticks.
	 * 
	 * \param seconds - Seconds.
	 * \param bpm - Beats per minute.
	 * 
	 * \returns ReWire ticks in 15360 PPQ.
	**/
	template <typename T>
	inline T Seconds2Ticks(T seconds, T bpm)
	{
		return seconds * TicksPerSecond(bpm);
	};



	template <typename T>
	inline T Ticks2Samples(T ticks, T bpm, T sampleRate)
	{
		return Seconds2Samples(Ticks2Seconds(ticks, bpm), sampleRate);
	};
	


} // namespace ReWire

#endif