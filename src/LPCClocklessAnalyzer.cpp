#include "LPCClocklessAnalyzer.h"
#include "LPCClocklessAnalyzerSettings.h"
#include <AnalyzerChannelData.h>
#include <iostream>
LPCClocklessAnalyzer::LPCClocklessAnalyzer()
:	Analyzer2(),  
	mSettings( new LPCClocklessAnalyzerSettings() )
	// mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

LPCClocklessAnalyzer::~LPCClocklessAnalyzer()
{
	KillThread();
}

void LPCClocklessAnalyzer::SetupResults()
{
	mResults.reset( new LPCClocklessAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	mResults->AddChannelBubblesWillAppearOn( mSettings->mFrameChannel );
	// mResults->AddChannelBubblesWillAppearOn( mSettings->mInputChannel );
}

void LPCClocklessAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
	mFrame = GetAnalyzerChannelData( mSettings->mFrameChannel );
	// mSerial = GetAnalyzerChannelData( mSettings->mInputChannel );

	if( mFrame->GetBitState() == BIT_HIGH )
		mFrame->AdvanceToNextEdge();
	



	U32 samples_per_bit = mSampleRateHz / 33000000;
	std::cout << "Samples per bit: " << samples_per_bit << std::endl;
	// U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );

	for( ; ; )
	{
		U8 data = 0;
		U8 mask = 1 << 7;
		// std::cout << "Advancing..." << mFrame->GetSampleNumber() << std::endl;
		mFrame->Advance(samples_per_bit/2);

		// mSerial->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		U64 starting_sample = mFrame->GetSampleNumber();

		// mSerial->Advance( samples_to_first_center_of_first_data_bit );

		for( U32 i=0; i<8; i++ )
		{
			//let's put a dot exactly where we sample this bit:
			mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mFrameChannel );

			// if( mSerial->GetBitState() == BIT_HIGH )
			// 	data |= mask;

			mFrame->Advance( samples_per_bit );

			// mask = mask >> 1;
		}


		//we have a byte to save. 
		Frame frame;
		// frame.mData1 = data;
		frame.mFlags = 0;
		frame.mStartingSampleInclusive = starting_sample;
		frame.mEndingSampleInclusive = mFrame->GetSampleNumber();

		mResults->AddFrame( frame );
		mResults->CommitResults();
		ReportProgress( frame.mEndingSampleInclusive );
	}
}

bool LPCClocklessAnalyzer::NeedsRerun()
{
	return false;
}

U32 LPCClocklessAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	return 0;
}
// 	if( mSimulationInitilized == false )
// 	{
// 		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
// 		mSimulationInitilized = true;
// 	}

// 	return mSimulationDataGenerator.GenerateSimulationData( minimum_sample_index, device_sample_rate, simulation_channels );
// }

U32 LPCClocklessAnalyzer::GetMinimumSampleRateHz()
{
	return mSettings->mBitRate * 4;
}

const char* LPCClocklessAnalyzer::GetAnalyzerName() const
{
	return "LPC Clockless";
}

const char* GetAnalyzerName()
{
	return "LPC Clockless";
}

Analyzer* CreateAnalyzer()
{
	return new LPCClocklessAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}