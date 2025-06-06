#include "pch.h"
#include "Musical.h"

#include <Windows.h>

BAKKESMOD_PLUGIN(Musical, "Musical", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Musical::onLoad()
{
	_globalCvarManager = cvarManager;

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) 
		{
			this->RenderCanvas(canvas);
		}
	);
}

void Musical::onUnload()
{

}

void Musical::RenderSettings()
{
    ImGui::Begin("Spotify Controls");

    this->FetchMediaMeta();

    ImGui::Text("Now Playing: %s", this->CurrentTitle.c_str());
    ImGui::Text("Artist: %s", this->CurrentArtist.c_str());

    if (ImGui::Button("Previous")) this->PreviousSong();

    ImGui::SameLine();
    if (ImGui::Button(this->MediaPlayling ? "Pause" : "Play")) PlayPause();

    ImGui::SameLine();
    if (ImGui::Button("Next")) this->NextSong();

    if (ImGui::Button("Restart")) this->RestartSong();

    ImGui::End();
}

void Musical::RenderWindow()
{

}

void Musical::RenderCanvas(CanvasWrapper& canvas)
{

}

void Musical::PlayPause() const
{
	keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENDEDKEY, 0);
}

void Musical::NextSong() const
{
	keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_EXTENDEDKEY, 0);
}

void Musical::PreviousSong() const
{
	keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_EXTENDEDKEY, 0);
}

void Musical::RestartSong() const
{
	PreviousSong();

	gameWrapper->SetTimeout([this](GameWrapper* gw)
		{
			this->PreviousSong();
		}, 
	0.1f); // 100ms
}

void Musical::FetchMediaMeta()
{
    try
    {
        init_apartment();

        auto sessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        auto currentSession = sessionManager.GetCurrentSession();

        if (currentSession)
        {
            auto mediaProps = currentSession.TryGetMediaPropertiesAsync().get();

            this->CurrentTitle = winrt::to_string(mediaProps.Title());
            this->CurrentArtist = winrt::to_string(mediaProps.Artist());

            auto playbackInfo = currentSession.GetPlaybackInfo();
            this->MediaPlayling = playbackInfo.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
        }
        else
        {
            this->CurrentTitle = "Not Playing";
            this->CurrentArtist = "None";
            this->MediaPlayling = false;
        }
    }
    catch (const std::exception& e)
    {
        this->CurrentTitle = "Error";
        this->CurrentArtist = e.what();
        this->MediaPlayling = false;
    }
}