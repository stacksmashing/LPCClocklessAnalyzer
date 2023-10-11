#ifndef LPCCLOCKLESS_ANALYZER_H
#define LPCCLOCKLESS_ANALYZER_H

#include <Analyzer.h>
#include "LPCClocklessAnalyzerResults.h"
#include "LPCClocklessSimulationDataGenerator.h"

class LPCClocklessAnalyzerSettings;
class ANALYZER_EXPORT LPCClocklessAnalyzer : public Analyzer2
{
public:
	LPCClocklessAnalyzer();
	virtual ~LPCClocklessAnalyzer();

	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();

protected: //vars
	std::unique_ptr< LPCClocklessAnalyzerSettings > mSettings;
	std::unique_ptr< LPCClocklessAnalyzerResults > mResults;
	// AnalyzerChannelData* mSerial;
	AnalyzerChannelData *mFrame;
	AnalyzerChannelData *mLadd[4];

	// LPCClocklessSimulationDataGenerator mSimulationDataGenerator;
	// bool mSimulationInitilized;

	//Serial analysis vars:
	U32 mSampleRateHz;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;

private: 
	uint8_t getBits();
	void advanceAllToFrame();
	void addFrameLabel(uint64_t start, uint64_t end, uint64_t label);
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //LPCCLOCKLESS_ANALYZER_H
