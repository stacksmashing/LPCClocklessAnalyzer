#ifndef LPCCLOCKLESS_ANALYZER_RESULTS
#define LPCCLOCKLESS_ANALYZER_RESULTS

#include <AnalyzerResults.h>

class LPCClocklessAnalyzer;
class LPCClocklessAnalyzerSettings;

// struct LPCFrame : public Frame
// {
// 	bool is_write;
// 	uint32_t address;
// 	uint8_t sync;
// 	uint8_t data;
// };

class LPCClocklessAnalyzerResults : public AnalyzerResults
{
public:
	LPCClocklessAnalyzerResults( LPCClocklessAnalyzer* analyzer, LPCClocklessAnalyzerSettings* settings );
	virtual ~LPCClocklessAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );
	std::string stringForFrame(DisplayBase display_base, Frame *f);

protected: //functions

protected:  //vars
	LPCClocklessAnalyzerSettings* mSettings;
	LPCClocklessAnalyzer* mAnalyzer;
};

#endif //LPCCLOCKLESS_ANALYZER_RESULTS
