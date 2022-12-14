#include "discord/discord.h"
#include "thirdparty/hook/hardware-breakpoint.h"
#include "thirdparty/hook/kiero.h"
#include "thirdparty/t7api/t7api.h"
#include "threads/threads.h"
#include <chrono>
#include "config.h"
#include "update.h"
#include "parser/parser.h"

// Most Important
// TODO: Add LargeImages for Multiplayer and Campaign.
// TODO: Create a tool that handles the creation of the remote thread allowing for ease of use without any third party tools.
// TODO: Update the Multiplayer configuration.

// Least Important
// TODO: Add presence information for when a user is in a pregame lobby.
// TODO: Fix the Difficulty Parser Rule on Multiplayer.
// TODO: Add parser rules for freerun and Theater mode.
// TODO: Add gamemode checks to rules that can cause issues and crashes when used incorrectly.
// TODO: Add rules for the Primary, Secondary and Mule Kick Weapons.
// TODO: Add parser rules for easter-egg bits.
// TODO: Add descriptions to the parser rules.
// TODO: Investigate a potential error that happens when losing internet connection while in a mp game.
// TODO: Investigate a crash that happens when using the difficulty rule on campaign.
 
unsigned long long g_entities = 0;
HRESULT(__stdcall* present)(IDXGISwapChain*, UINT, UINT);

std::chrono::steady_clock::time_point delta = std::chrono::steady_clock::now();
std::chrono::seconds getdelta(std::chrono::steady_clock::time_point now)
{
    return std::chrono::duration_cast<std::chrono::seconds>(now - delta);
}

HRESULT WINAPI update(IDXGISwapChain* swapchain, UINT interval, UINT flags)
{
    /*
    // Developer Note: Commented out as it crashes the game for unknown reasons.
    //////////////////////////////////////////////////////////////////////////
    if(GetAsyncKeyState(VK_F9) & 0x8000)
    {
        t7api::cg::boldgamemessagecenter(0, "Detaching from process...");

        discord::running.store(false);
        kiero::unbind(8);
        kiero::shutdown();
        hook_manager::get()["g_entities"]->unhook();

        CreateThread(0, 0, threads::detach, 0, 0, 0);
        return present(swapchain, interval, flags);
    }
    */

    static bool ConfigKeyDown = false;
    if(GetAsyncKeyState(VK_F5) & 0x8000)
    {
        if(ConfigKeyDown == false)
        {
            ConfigKeyDown = true;
        
            t7api::cg::boldgamemessagecenter(0, "Attempting to reload the config...");
            config::load();
        }
    }
    else
    {
        if(ConfigKeyDown == true)
        {
            ConfigKeyDown = false;
        }
    }

    std::chrono::steady_clock steady_clock;
    if(getdelta(steady_clock.now()) >= ((t7api::com::isingame() == true) ? std::chrono::seconds(30) : std::chrono::seconds(5)))
    {
        auto presence = discord::presence::get();

        if(t7api::com::isingame())
        {
            if(presence.startTimestamp == 0)
                presence.startTimestamp = time(0);

            presence.partySize = t7api::lobbysession::getclientcount(1, 0);
                
            if(presence.partySize <= 0)
                presence.partySize = 1;

            presence.partyMax = t7api::lobbysession::getmaxclients(1);

            if(presence.partyMax <= 0)
                presence.partyMax = 1;

            std::string fastfile = reinterpret_cast<const char*>(reinterpret_cast<unsigned long long>(GetModuleHandleA(0)) + (0x7FF77AD1E5E8 - 0x7FF771910000));

            if(fastfile._Starts_with("zm_"))
            {
                if(fastfile != "zm_zod" &&
                   fastfile != "zm_factory" &&
                   fastfile != "zm_castle" &&
                   fastfile != "zm_island" &&
                   fastfile != "zm_stalingrad" &&
                   fastfile != "zm_genesis" &&
                   fastfile != "zm_prototype" &&
                   fastfile != "zm_asylum" &&
                   fastfile != "zm_sumpf" &&
                   fastfile != "zm_theater" &&
                   fastfile != "zm_cosmodrome" &&
                   fastfile != "zm_temple" &&
                   fastfile != "zm_moon" &&
                   fastfile != "zm_tomb")
                fastfile = "logo-black";
            }

            presence.largeImageKey = fastfile.c_str();
            presence.largeImageText = parser::parse("${mapname}", 128).c_str();
            presence.smallImageKey = "null";

            presence.state = "";
            presence.details = "";

            presence.instance = 1;

            try
            {
                const char* lobbymode = "";
                const char* sessionmode = "";

                // Developer Note: Puting this directly into the switch value
                // Will cause the program to crash.
                /////////////////////////////////////////////////////////////
                int mode = *(int*)(t7api::lobbysession::getsession(1) + 8);
                switch(mode)
                {
                    case LOBBY_MODE_PUBLIC:
                    lobbymode = "public";
                    break;
                    case LOBBY_MODE_CUSTOM:
                    lobbymode = "customs";
                    break;
                  // Commented out until support is added
                  //case LOBBY_MODE_THEATER:
                  //lobbymode = "theater";
                  //break;
                    case LOBBY_MODE_ARENA:
                    lobbymode = "arena";
                    break;
                  // Commented out until support is added
                  //case LOBBY_MODE_FREERUN:
                  //lobbymode = "freerun";
                  //break;
                }

                switch(t7api::com::sessionmode::getmode())
                {
                    case MODE_ZOMBIES:
                    sessionmode = "zombies";
                    break;
                    case MODE_MULTIPLAYER:
                    sessionmode = "multiplayer";
                    break;
                    case MODE_CAMPAIGN:
                    sessionmode = "campaign";
                    break;
                }

                
                if(lobbymode != NULL)
                {
                    if(sessionmode != NULL)
                    {
                        char details[128] = "";
                        char state[128] = "";

                        if(strcmp(sessionmode, "zombies") == 0)
                        {
                            const char* partystate = (presence.partySize > 1) ? "party" : "solo";

                            sprintf_s(details, 128, "%s", parser::parse(config::get()["presence"][sessionmode][lobbymode][partystate]["details"].get<std::string>()).c_str());
                            sprintf_s(state, 128, "%s", parser::parse(config::get()["presence"][sessionmode][lobbymode][partystate]["state"].get<std::string>()).c_str());

                            if(!config::get()["presence"][sessionmode][lobbymode][partystate]["show-timestamp"].get<bool>())
                                presence.startTimestamp = 0;

                            if(!config::get()["presence"][sessionmode][lobbymode][partystate]["show-playercount"].get<bool>())
                                presence.instance = 0;
                        }
                        else
                        {
                            sprintf_s(details, 128, "%s", parser::parse(config::get()["presence"][sessionmode][lobbymode]["details"].get<std::string>()).c_str());
                            sprintf_s(state, 128, "%s", parser::parse(config::get()["presence"][sessionmode][lobbymode]["state"].get<std::string>()).c_str());

                            if(!config::get()["presence"][sessionmode][lobbymode]["show-timestamp"].get<bool>())
                                presence.startTimestamp = 0;

                            if(!config::get()["presence"][sessionmode][lobbymode]["show-playercount"].get<bool>())
                                presence.instance = 0;
                        }

                        if(details != NULL)
                            presence.details = details;

                        if(state != NULL)
                            presence.state = state;
                    }
                }
            }
            catch(nlohmann::json::exception &e) { OutputDebugStringA(e.what()); }
        }
        else
        {
            if(presence.startTimestamp != 0)
            {
                presence.startTimestamp = 0;
            }

            presence.details = "";
            presence.state = "";
            presence.largeImageKey = "logo-black";
            presence.largeImageText = "Call of Duty: Black Ops III";
            presence.smallImageKey = "";
            presence.partySize = 0;
            presence.partyMax = 0;
            presence.instance = 0;

            // Add support for pregame lobby later

            try
            {
                char details[128];
                sprintf_s(details, 128, "%s", parser::parse(config::get()["presence"]["mainmenu"]["details"].get<std::string>()).c_str());
                if(details != NULL)
                    presence.details = details;
                
                char state[128];
                sprintf_s(state, 128, "%s", parser::parse(config::get()["presence"]["mainmenu"]["state"].get<std::string>()).c_str());
                if(state != NULL)
                    presence.state = state;
            }
            catch(nlohmann::json::exception &e) { OutputDebugStringA(e.what()); }
        }

        discord::presence::set(presence);
        delta = steady_clock.now();
    }

    return present(swapchain, interval, flags);
}