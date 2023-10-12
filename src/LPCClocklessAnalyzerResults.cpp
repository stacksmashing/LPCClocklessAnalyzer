#include "LPCClocklessAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "LPCClocklessAnalyzer.h"
#include "LPCClocklessAnalyzerSettings.h"
#include <iostream>
#include <fstream>



LPCClocklessAnalyzerResults::LPCClocklessAnalyzerResults( LPCClocklessAnalyzer* analyzer, LPCClocklessAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

LPCClocklessAnalyzerResults::~LPCClocklessAnalyzerResults()
{
}

void LPCClocklessAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )
{
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 48, number_str, 128 );
	AddResultString( stringForFrame(display_base, &frame).c_str() );
}

std::string LPCClocklessAnalyzerResults::stringForFrame(DisplayBase display_base, Frame *f) {
	uint64_t data = f->mData1;
	std::string ret = "";
	if((data & 0xff0000000000) == 0x020000000000) {
		ret += "WRITE ";
	} else {
		ret += "READ ";
	}
	char address[128];
	AnalyzerHelpers::GetNumberString((f->mData1 >> 8) & 0xFFFFFFFF, display_base, 32, address, 128);
	ret += address;
	ret += " DATA ";
	char data_str[8];
	AnalyzerHelpers::GetNumberString(f->mData1 & 0xFF, display_base, 8, data_str, 8);
	ret += data_str;
	return ret;
}

void LPCClocklessAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id )
{
	std::ofstream file_stream( file, std::ios::out );

	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();

	file_stream << "Time [s],Value" << std::endl;

	U64 num_frames = GetNumFrames();
	for( U32 i=0; i < num_frames; i++ )
	{
		Frame frame = GetFrame( i );
		
		char time_str[128];
		AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

		char number_str[128];
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 8, number_str, 128 );

		file_stream << time_str << "," << number_str << std::endl;

		if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
		{
			file_stream.close();
			return;
		}
	}

	file_stream.close();
}

void LPCClocklessAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef SUPPORTS_PROTOCOL_SEARCH
	
	Frame frame = GetFrame( frame_index );
	// if(frame.mType == 0x99) {
	// 	LPCFrame& req( ( LPCFrame& )frame);
	// 	if(req.is_write) {
	// 		AddTabularText("WRTE");
	// 	} else {
	// 		AddTabularText("READ");
	// 	}

	// 	char number_str[128];
	// 	AnalyzerHelpers::GetNumberString( req.address, display_base, 8, number_str, 128 );
	// 	AddTabularText( number_str );

	// 	char number_str2[128];
	// 	AnalyzerHelpers::GetNumberString( req.data, display_base, 8, number_str2, 128 );
	// 	AddTabularText( number_str2 );



	// } else {
		ClearTabularText();

		char number_str[128];
		// AnalyzerHelpers::GetNumberString()
		AnalyzerHelpers::GetNumberString( frame.mData1, display_base, 48, number_str, 128 );
		AddTabularText( number_str );
		AddResultString( stringForFrame(display_base, &frame).c_str() );
		// AddTabularText("TEST");
	// }
	
#endif
}

void LPCClocklessAnalyzerResults::GeneratePacketTabularText( U64 packet_id, DisplayBase display_base )
{
	//not supported

}

void LPCClocklessAnalyzerResults::GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base )
{
	//not supported
}