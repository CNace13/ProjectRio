
#include "Core/MSB_StatTracker.h"

#include <iomanip>
#include <fstream>
#include <ctime>

//For LocalPLayers
#include "Common/CommonPaths.h"
#include "Common/IniFile.h"
#include "Core/LocalPlayersConfig.h"
#include "Common/Version.h"

#include "Common/Swap.h"

// Package for rendering info on screen
#include "VideoCommon/OnScreenDisplay.h"
#include <fmt/chrono.h>
#include <fmt/format.h>

#include <iostream>
#include "Config/MainSettings.h"

#include "Common/TagSet.h"

void StatTracker::Run(const Core::CPUThreadGuard& guard)
{
    lookForTriggerEvents(guard);
}

void StatTracker::lookForTriggerEvents(const Core::CPUThreadGuard& guard)
{
    if (m_game_state == GAME_STATE::UNDEFINED || m_game_state == GAME_STATE::MENU){
        //Look for menu=1
        if (tracker_IsGolfRound.read_value(guard) == 0x1){
            //Transition to Round
            //Gather players
        }
    else if (m_game_state == GAME_STATE::ROUND) {
    
    }
}