#include "LPCClocklessAnalyzerSettings.h"
#include <AnalyzerHelpers.h>


LPCClocklessAnalyzerSettings::LPCClocklessAnalyzerSettings() :
// :	mInputChannel( UNDEFINED_CHANNEL ),
	mFrameChannel( UNDEFINED_CHANNEL ),
	mBitRate( 9600 )
{
	ClearChannels();
	for(int i=0; i < 4; i++) {
		mLaddChannel[i] = Channel(UNDEFINED_CHANNEL);
	// }
		mLaddChannelInterface[i].reset( new AnalyzerSettingInterfaceChannel() );
		// const char *name = std::string("Ladd" + std::to_string(i)).c_str();
		mLaddChannelInterface[i]->SetTitleAndTooltip(std::string("Ladd" + std::to_string(i)).c_str(), "Signal.");
		mLaddChannelInterface[i]->SetChannel( mLaddChannel[i] );
		AddInterface( mLaddChannelInterface[i].get() );
		AddChannel( mLaddChannel[i], std::string("Ladd" + std::to_string(i)).c_str(), false );
	}

	mFrameChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	mFrameChannelInterface->SetTitleAndTooltip( "Frame", "Frame signal" );
	mFrameChannelInterface->SetChannel( mFrameChannel );
	AddInterface(mFrameChannelInterface.get());
	// mInputChannelInterface.reset( new AnalyzerSettingInterfaceChannel() );
	// mInputChannelInterface->SetTitleAndTooltip( "Serial", "Standard LPC Clockless" );
	// mInputChannelInterface->SetChannel( mInputChannel );

	mBitRateInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitRateInterface->SetTitleAndTooltip( "Bit Rate (Bits/S)",  "Specify the bit rate in bits per second." );
	mBitRateInterface->SetMax( 6000000 );
	mBitRateInterface->SetMin( 1 );
	mBitRateInterface->SetInteger( mBitRate );

	// AddInterface( mInputChannelInterface.get() );
	AddInterface( mBitRateInterface.get() );

	AddExportOption( 0, "Export as text/csv file" );
	AddExportExtension( 0, "text", "txt" );
	AddExportExtension( 0, "csv", "csv" );

	// ClearChannels();
	AddChannel( mFrameChannel, "Frame", false );
}

LPCClocklessAnalyzerSettings::~LPCClocklessAnalyzerSettings()
{
}

bool LPCClocklessAnalyzerSettings::SetSettingsFromInterfaces()
{
	// mInputChannel = mInputChannelInterface->GetChannel();
	mFrameChannel = mFrameChannelInterface->GetChannel();
	for(int i=0; i < 4; i++) {
		mLaddChannel[i] = mLaddChannelInterface[i]->GetChannel();
	}
	mBitRate = mBitRateInterface->GetInteger();

	ClearChannels();
	for(int i=0; i < 4; i++) {
		AddChannel( mLaddChannel[i], std::string("Ladd" + std::to_string(i)).c_str(), true);
	}
	// AddChannel( mInputChannel, "LPC Clockless", true );
	AddChannel( mFrameChannel, "Frame", true );

	return true;
}

void LPCClocklessAnalyzerSettings::UpdateInterfacesFromSettings()
{
	for(int i=0; i < 4; i++) {
		mLaddChannelInterface[i]->SetChannel( mLaddChannel[i] );
	}
	mFrameChannelInterface->SetChannel( mFrameChannel );
	// mInputChannelInterface->SetChannel( mInputChannel );
	mBitRateInterface->SetInteger( mBitRate );
}

void LPCClocklessAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );
	for(int i=0; i < 4; i++) {
		text_archive >> mLaddChannel[i];
	}
	text_archive >> mFrameChannel;
	text_archive >> mBitRate;

	ClearChannels();
	for(int i=0; i < 4; i++) {
		AddChannel( mLaddChannel[i], std::string("Ladd" + std::to_string(i)).c_str(), true);
	}
	// AddChannel( mInputChannel, "LPC Clockless", true );
	AddChannel( mFrameChannel, "Frame", true );
	// AddChannel( mInputChannel, "LPC Clockless", true );

	UpdateInterfacesFromSettings();
}

const char* LPCClocklessAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	for(int i=0; i < 4; i++) {
		text_archive << mLaddChannel[i];
	}
	text_archive << mFrameChannel;
	// text_archive << mInputChannel;
	text_archive << mBitRate;

	return SetReturnString( text_archive.GetString() );
}
