#ifndef LPCCLOCKLESS_ANALYZER_SETTINGS
#define LPCCLOCKLESS_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

class LPCClocklessAnalyzerSettings : public AnalyzerSettings
{
public:
	LPCClocklessAnalyzerSettings();
	virtual ~LPCClocklessAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	// Channel mInputChannel;
	Channel mLaddChannel[4];
	Channel mFrameChannel;
	U32 mBitRate;

protected:
	// std::unique_ptr< AnalyzerSettingInterfaceChannel >	mInputChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mLaddChannelInterface[4];
	std::unique_ptr< AnalyzerSettingInterfaceChannel >	mFrameChannelInterface;
	std::unique_ptr< AnalyzerSettingInterfaceInteger >	mBitRateInterface;
};

#endif //LPCCLOCKLESS_ANALYZER_SETTINGS
