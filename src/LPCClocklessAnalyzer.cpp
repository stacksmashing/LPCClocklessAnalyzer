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

uint8_t LPCClocklessAnalyzer::getBits() {
	return (mLadd[3]->GetBitState() << 3) |
	       (mLadd[2]->GetBitState() << 2) |
		   (mLadd[1]->GetBitState() << 1) |
		   (mLadd[0]->GetBitState() << 0);
}

void LPCClocklessAnalyzer::advanceAllToFrame() {
	for(int i=0; i < 4; i++) {
		mLadd[i]->AdvanceToAbsPosition(mFrame->GetSampleNumber());
	}
}

void LPCClocklessAnalyzer::addFrameLabel(uint64_t start, uint64_t end, uint64_t label) {
	Frame frame;
	frame.mData1 = label;
	frame.mFlags = 0;
	frame.mStartingSampleInclusive = start;
	frame.mEndingSampleInclusive = end;

	mResults->AddFrame( frame );
	mResults->CommitResults();
	ReportProgress(end);
}

void LPCClocklessAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
	mFrame = GetAnalyzerChannelData( mSettings->mFrameChannel );
	for(int i=0; i < 4; i++) {
		mLadd[i] = GetAnalyzerChannelData( mSettings->mLaddChannel[i] );
	}

	// mSerial = GetAnalyzerChannelData( mSettings->mInputChannel );



// -       U32 samples_per_bit = mSampleRateHz / mSettings->mBitRate;

	U32 samples_per_bit = mSampleRateHz / (mSettings->mBitRate * 1000);
	std::cout << "Samples per bit: " << samples_per_bit << std::endl;
	// U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );

	for( ; ; )
	{
		bool is_write = false;

		U8 data = 0;
		U8 mask = 1 << 7;


		if( mFrame->GetBitState() == BIT_HIGH )
			mFrame->AdvanceToNextEdge();
		
		mFrame->Advance(samples_per_bit/2);
		advanceAllToFrame();
		// Check pattern
		uint8_t bits = getBits();
		// printf("Bits: %d\n", bits);
		if(getBits() == 0b0101) {
			mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Start, mSettings->mFrameChannel );
		} else {
			mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::ErrorX, mSettings->mFrameChannel );
			mFrame->AdvanceToNextEdge();
			mResults->CommitResults();
			ReportProgress( mFrame->GetSampleNumber() );
			continue;
		}
		
		
		// mLadd[0]->AdvanceToAbsPosition(mFrame->GetSampleNumber());
		// // Often the ladd lines are delayed to the actual clock transition in my testing.
		// if( mLadd[0]->GetBitState() == BIT_HIGH ) {
		// 	mLadd[0]->AdvanceToNextEdge();
		// }


		// mFrame->AdvanceToAbsPosition(mLadd[0]->GetSampleNumber());
		// for(int i=1; i < 4; i++) {
		// 	mLadd[i]->AdvanceToAbsPosition(mLadd[0]->GetSampleNumber());
		// }
		
		// std::cout << "Advancing..." << mFrame->GetSampleNumber() << std::endl;
		
		// mFrame->Advance(samples_per_bit/2);

		// mSerial->AdvanceToNextEdge(); //falling edge -- beginning of the start bit

		U64 starting_sample = mFrame->GetSampleNumber();


		// Go to CYCTYPE + DIR
		// 0000 = read
		// 0010 = write
		
		mFrame->Advance( samples_per_bit );
		advanceAllToFrame();
		
		uint8_t cyctype_dir = getBits();
		if(cyctype_dir == 0x02) {
			printf("Is write\n");
			is_write = true;
		}
		addFrameLabel(starting_sample, mFrame->GetSampleNumber(), cyctype_dir);
		// Frame frame;
		// frame.mData1 = cyctype_dir;
		// frame.mFlags = 0;
		// frame.mStartingSampleInclusive = starting_sample;
		// frame.mEndingSampleInclusive = mFrame->GetSampleNumber();

		// mResults->AddFrame( frame );
		// mResults->CommitResults();
		//let's put a dot exactly where we sample this bit:
		mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mFrameChannel );

		starting_sample = mFrame->GetSampleNumber();
		// Iterate over address fields
		uint32_t address = 0;
		for(int i=0; i < 4; i++) {
			mFrame->Advance( samples_per_bit );
			advanceAllToFrame();

			address |= getBits() << 4 * (3-i);
			mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mFrameChannel );
		}
		addFrameLabel(starting_sample, mFrame->GetSampleNumber(), address);
		// frame.mData1 = address;
		// frame.mFlags = 0;
		// frame.mStartingSampleInclusive = starting_sample;
		// frame.mEndingSampleInclusive = mFrame->GetSampleNumber();

		// mResults->AddFrame( frame );
		// mResults->CommitResults();


		// Next bits depend on whether read or write



			// if( mSerial->GetBitState() == BIT_HIGH )
			// 	data |= mask;

			// mFrame->Advance( samples_per_bit );

			// mask = mask >> 1;
		// }


		//we have a byte to save. 
		// Frame frame;
		// // frame.mData1 = data;
		// frame.mFlags = 0;
		// frame.mStartingSampleInclusive = starting_sample;
		// frame.mEndingSampleInclusive = mFrame->GetSampleNumber();

		// mResults->AddFrame( frame );
		// mResults->CommitResults();
		// ReportProgress( frame.mEndingSampleInclusive );

		mFrame->AdvanceToNextEdge();
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