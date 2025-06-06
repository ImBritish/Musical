#include "pch.h"
#include "Musical.h"

#include <Windows.h>
#include <chrono>
#include <algorithm>

BAKKESMOD_PLUGIN(Musical, "Musical", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void Musical::onLoad()
{
	_globalCvarManager = cvarManager;

    this->RegisterCommands();

    this->FetchMediaMeta();

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
    if (ImGui::Checkbox("Show Overlay", &this->DisplayOverlay))
    {
        gameWrapper->Execute([this](GameWrapper* gw) 
            {
                cvarManager->executeCommand("togglemenu " + GetMenuName());
            }
        );
    }
}

void Musical::RenderWindow()
{
    if (!DisplayOverlay || !isWindowOpen_ || !gameWrapper)
        return;

    bool ShowControls = gameWrapper->IsCursorVisible() == 2;
    
    float titleSizeX = ImGui::CalcTextSize(std::string(("Now Playing: ") + CurrentTitle).c_str()).x;
    float artistSizeX = ImGui::CalcTextSize(std::string(("Artist: ") + CurrentArtist).c_str()).x;
    
    float trueSizeX = (titleSizeX >= artistSizeX ? titleSizeX : artistSizeX) + 20;
    
    if (trueSizeX < 175)
        trueSizeX = 175;
    
    ImGui::SetWindowSize({ trueSizeX, (float) (ShowControls ? 70 : 50) });

    ImGui::Text("Now Playing: %s", this->CurrentTitle.c_str());
    ImGui::Text("Artist: %s", this->CurrentArtist.c_str());

    if (ShowControls)
    {
        ImGui::SetNextWindowSize({ 400, 100 });

        if (ImGui::Button("Previous"))
        {
            this->PreviousSong();
            this->FetchMediaMeta();
        }

        ImGui::SameLine();

        if (ImGui::Button(this->MediaPlayling ? "Pause" : "Play"))
        {
            this->PlayPause();
            this->FetchMediaMeta();
        }

        ImGui::SameLine();

        if (ImGui::Button("Next"))
        {
            this->NextSong();
            this->FetchMediaMeta();
        }
    }
}

void Musical::RenderCanvas(CanvasWrapper& canvas)
{
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    auto duration = duration_cast<std::chrono::milliseconds>(now - lastTime).count();

    if (duration >= 1000)
    {
        this->FetchMediaMeta();
        lastTime = now;
    }

    if (isWindowOpen_ && !gameWrapper->IsInGame())
        cvarManager->executeCommand("togglemenu " + GetMenuName());

    if (!isWindowOpen_ && DisplayOverlay && gameWrapper->IsInGame())
        cvarManager->executeCommand("togglemenu " + GetMenuName());
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

void Musical::RestartSong() const // Not in use / dont work lol
{
	PreviousSong();

	gameWrapper->SetTimeout([this](GameWrapper* gw)
		{
			this->PreviousSong();
		}, 
	0.1f); // 100ms
}

void Musical::RegisterCommands()
{
    cvarManager->registerNotifier("musical_next", [this](std::vector<std::string> args) 
        {
            this->NextSong();
            this->FetchMediaMeta();
        }
    , "Play next song in queue", PERMISSION_ALL);

    cvarManager->registerNotifier("musical_prev", [this](std::vector<std::string> args)
        {
            this->PreviousSong();
            this->FetchMediaMeta();
        }
    , "Play previous song", PERMISSION_ALL);

    cvarManager->registerNotifier("musical_pause_play", [this](std::vector<std::string> args)
        {
            this->PlayPause();
            this->FetchMediaMeta();
        }
    , "Play/Pause current song", PERMISSION_ALL);
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