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

uint32_t LPCClocklessAnalyzer::readAddress() {
		uint32_t address = 0;
		for(int i=0; i < 4; i++) {
			mFrame->Advance( mSamplesPerBit );
			advanceAllToFrame();

			address |= getBits() << 4 * (3-i);
			mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mFrameChannel );
		}
		return address;
}

void LPCClocklessAnalyzer::skipTar() {
	mFrame->Advance( mSamplesPerBit );
	mFrame->Advance( mSamplesPerBit );
}

uint8_t LPCClocklessAnalyzer::readData() {
	uint8_t data = 0;
	mFrame->Advance( mSamplesPerBit );
	advanceAllToFrame();
	data = getBits();
	mFrame->Advance( mSamplesPerBit );
	advanceAllToFrame();
	data |= getBits() << 4;
	return data;
}

uint8_t LPCClocklessAnalyzer::readSync() {
	mFrame->Advance( mSamplesPerBit );
	advanceAllToFrame();
	return getBits();
}

void LPCClocklessAnalyzer::WorkerThread()
{
	mSampleRateHz = GetSampleRate();
	mSamplesPerBit = mSampleRateHz / (mSettings->mBitRate * 1000);
	mFrame = GetAnalyzerChannelData( mSettings->mFrameChannel );
	for(int i=0; i < 4; i++) {
		mLadd[i] = GetAnalyzerChannelData( mSettings->mLaddChannel[i] );
	}

	// mSerial = GetAnalyzerChannelData( mSettings->mInputChannel );



// -       U32 mSamplesPerBit = mSampleRateHz / mSettings->mBitRate;

	// mSamplesPerBit = mSampleRateHz / (mSettings->mBitRate * 1000);
	std::cout << "Samples per bit: " << mSamplesPerBit << std::endl;
	// U32 samples_to_first_center_of_first_data_bit = U32( 1.5 * double( mSampleRateHz ) / double( mSettings->mBitRate ) );

	for( ; ; )
	{

		// Frame f;
		// f.mType = 0x99;


		bool is_write = false;

		U8 data = 0;
		U8 mask = 1 << 7;


		if( mFrame->GetBitState() == BIT_HIGH )
			mFrame->AdvanceToNextEdge();
		


		uint64_t total_starting_sample = mFrame->GetSampleNumber();

		mFrame->Advance(mSamplesPerBit/2);
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

		U64 starting_sample = mFrame->GetSampleNumber();


		// Go to CYCTYPE + DIR
		// 0000 = read
		// 0010 = write
		mFrame->Advance( mSamplesPerBit );
		advanceAllToFrame();

		uint8_t cyctype_dir = getBits();
		if(cyctype_dir == 0x02) {
			// printf("Is write\n");
			is_write = true;
			// f.is_write = true;
		} else {
			// f.is_write = false;
		}
		// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), cyctype_dir);
		//let's put a dot exactly where we sample this bit:
		mResults->AddMarker( mFrame->GetSampleNumber(), AnalyzerResults::Dot, mSettings->mFrameChannel );

		starting_sample = mFrame->GetSampleNumber();
		// Iterate over address fields
		uint32_t address = readAddress();
		// f.address = address;
		// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), address);



		// Read data/TAR
		if(is_write) {
			starting_sample = mFrame->GetSampleNumber();
			data = readData();
			// f.data = data;
			// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), data);
			skipTar();
			starting_sample = mFrame->GetSampleNumber();
			uint8_t sync = readSync();
			// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), sync);
			skipTar();
		} else {
			skipTar();
			// sync field
			starting_sample = mFrame->GetSampleNumber();
			while(readSync() != 0) {
				
			}
			// uint8_t sync = readSync();
			// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), sync);

			// data field
			starting_sample = mFrame->GetSampleNumber();
			data = readData();
			// f.data = data;
			// addFrameLabel(starting_sample, mFrame->GetSampleNumber(), data);

			skipTar();
		}

		uint64_t transfer =
			((uint64_t)cyctype_dir << 40) |
			((uint64_t)address << 8) |
			(data);
		
		addFrameLabel(total_starting_sample, mFrame->GetSampleNumber(), transfer);
		// f.mFlags = 0;
		// f.mStartingSampleInclusive = total_starting_sample;
		// f.mEndingSampleInclusive = mFrame->GetSampleNumber();
		// mResults->AddFrame( f );

		mResults->CommitResults();
		ReportProgress(mFrame->GetSampleNumber());
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