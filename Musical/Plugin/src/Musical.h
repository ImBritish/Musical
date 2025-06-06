#pragma once

#include <Plugin/external/bakkes/gui/GuiBase.h>
#include <bakkesmod/plugin/bakkesmodplugin.h>
#include <bakkesmod/plugin/pluginwindow.h>
#include <bakkesmod/plugin/PluginSettingsWindow.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Media.MediaProperties.h>

using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Media::MediaProperties;

#include "version.h"

constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class Musical: public BakkesMod::Plugin::BakkesModPlugin, public SettingsWindowBase, public PluginWindowBase
{
public: // Overrides
	void onLoad() override;
	void onUnload() override;
	void RenderSettings() override;
	void RenderWindow() override;
public:
	void RenderCanvas(CanvasWrapper& canvas);
private:
	void PlayPause() const;
	void NextSong() const;
	void PreviousSong() const;
	void RestartSong() const;

	void FetchMediaMeta();
private:
	std::string CurrentTitle = "Not Playing", CurrentArtist = "None";
	bool MediaPlayling = false;
};
