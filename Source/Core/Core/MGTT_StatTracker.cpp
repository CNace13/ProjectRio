#include "Core/MGTT_StatTracker.h"
#include "Core/NetPlayClient.h"

void MGTT_StatTracker::readLocalPlayers(int num_players){ //TODO - Assumes all human players, each with own port
    if (rioInfo.netplay){
        rioInfo.rioUsers = NetPlay::NetPlayClient::getNetplayerUserInfo();
      return;
    }
    
    for (int i=0; i<num_players; ++i){
        switch (i)
        {
        case 0:
            rioInfo.rioUsers[i] = LocalPlayers::m_local_player_1;
            break;
        case 1:
            rioInfo.rioUsers[i] = LocalPlayers::m_local_player_2;
            break;
        case 2:
            rioInfo.rioUsers[i] = LocalPlayers::m_local_player_3;
            break;
        case 3:
            rioInfo.rioUsers[i] = LocalPlayers::m_local_player_4;
            break;
        default:
            break;
        }
    }
    logger << fmt::format("readLocalPlayers m_local_player_0, username={}\n", LocalPlayers::m_online_player.GetUsername());
    logger << fmt::format("readLocalPlayers m_local_player_1, username={}\n", LocalPlayers::m_local_player_1.GetUsername());
    logger << fmt::format("readLocalPlayers m_local_player_2, username={}\n", LocalPlayers::m_local_player_2.GetUsername());
    logger << fmt::format("readLocalPlayers m_local_player_3, username={}\n", LocalPlayers::m_local_player_3.GetUsername());
    logger << fmt::format("readLocalPlayers m_local_player_4, username={}\n", LocalPlayers::m_local_player_4.GetUsername());
}

void MGTT_StatTracker::setGameID(uint64_t inGameID){
    if (rioInfo.netplay && !rioInfo.is_netplay_host) {
        gameID = inGameID;
    }
}

void MGTT_StatTracker::resetMemoryTrackers(){
    rIsGolfRound.reset();
    rPlayerPorts.reset();
    rPlayerTurn.reset();
    rGameMode.reset();
    rCourseId.reset();
    rRoundFormat.reset();
    rGreenType.reset();
    rTees.reset();
    rPlayerCount.reset();
    rStarredAtMenu.reset();
    rHandicapsEnabled.reset();
    rSimulationLine.reset();
    rMulligans.reset();
    rHandicapTees.reset();
    rCharId.reset();
    rHandedness.reset();
    rWoods.reset();
    rIrons.reset();
    rWedges.reset();
    rHoleNum.reset();
    rHoleIndex.reset();
    rPar.reset();
    rWindRads.reset();
    rWindSpeed.reset();
    rRainBool.reset();
    rPin.reset();
    rPin2.reset();
    rShotPhase.reset();
    rPowerShotCount.reset();
    rHoleDone.reset();
    rPlayerShotStatus_preswing.reset();
    rPlayerShotStatus_swing.reset();
    rPlayerShotStatus_postswing.reset();
    rPlayerShotStatus_posthole.reset();
    rPlayerShotNum.reset();
    rShotType.reset();
    rClubType.reset();
    rPowerMeterSetting.reset();
    rPowerMeterSettingCopy.reset();
    rPowerMeterMaximum.reset();
    rPowerMeterActual.reset();
    rPowerMeterActualCopy.reset();
    rShotAccuracy.reset();
    rShotAccuracy2.reset();
    rManualVsAuto.reset();
    rAimAngleRadians.reset();
    rImpactPoint_Y_Preshot.reset();
    rImpactPoint_X_Preshot.reset();
    rImpactPoint_X.reset();
    rImpactPoint_Y.reset();
    rSpin.reset();
    rBallPos_X.reset();
    rBallPos_Y.reset();
    rBallPos_Z.reset();
    rDistanceToHole.reset();
    rShotDistance.reset();
    rLieType.reset();
    rLieQuality.reset();
    rLieRngRange.reset();
    rLieRng.reset();
    rLieRngSeed.reset();
    rFinalScoreHoleTotal.reset();
    rFinalScoreHolePutts.reset();
    rFinalScoreHoleStrokes.reset();

    initMemoryTrackers();
}

void MGTT_StatTracker::initMemoryTrackers(){
    rIsGolfRound = std::make_unique<MemoryTracker<u8, 2, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>>(aIsGolfRound, bool_hi_criteria_0, bool_hi_criteria_1);
    rPlayerPorts = std::make_unique<MemoryTracker<u32, 1>>(aPlayerPorts);
    rPlayerTurn = std::make_unique<MemoryTracker<u8, 1>>(aPlayerTurn);
    rGameMode = std::make_unique<MemoryTracker<u8, 16, decltype(game_mode_criteria_0)>>(aGameMode, game_mode_criteria_0);
    rMenuScene = std::make_unique<MemoryTracker<u8, 16, decltype(return_to_menu_1), decltype(return_to_menu_0)>>(aMenuScene, return_to_menu_1, return_to_menu_0);
    rCourseId = std::make_unique<MemoryTracker<u8, 16>>(aCourseId);
    rRoundFormat = std::make_unique<MemoryTracker<u8, 16, decltype(round_format_criteria_0), decltype(round_format_criteria_1)>>(aRoundFormat, round_format_criteria_0, round_format_criteria_1);
    rGreenType = std::make_unique<MemoryTracker<u8, 16>>(aGreenType);
    rTees = std::make_unique<MemoryTracker<u8, 16>>(aTees);
    rPlayerCount = std::make_unique<MemoryTracker<u8, 2, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>>(aPlayerCount, bool_hi_criteria_0, bool_hi_criteria_1);
    rStarredAtMenu = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aStarredAtMenu_P1, 0x4);
    rHandicapsEnabled = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aHandicapsEnabled_P1, 0x4);
    rSimulationLine = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aSimulationLine_P1, 0x4);
    rMulligans = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aMulligans_P1, 0x4);
    rHandicapTees = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aHandicapTees_P1, 0x4);
    rCharId = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aCharIdMenu_P1, 0x4);
    rHandedness = std::make_unique<MemoryTrackerArray<u32, 16, 4>>(aHandednessMenu_P1, 0x4);
    rWoods = std::make_unique<MemoryTrackerArray<u8, 1, 4>>(aWoodsMenu_P1, 0x4);
    rIrons = std::make_unique<MemoryTrackerArray<u8, 1, 4>>(aIronsMenu_P1, 0x4);
    rWedges = std::make_unique<MemoryTrackerArray<u8, 1, 4>>(aWedgesMenu_P1, 0x4);
    rHoleNum = std::make_unique<MemoryTracker<u8, 2, decltype(prev_hole), decltype(curr_hole)>>(aHoleNum, prev_hole, curr_hole);
    rHoleIndex = std::make_unique<MemoryTracker<u8, 2, decltype(prev_hole), decltype(curr_hole)>>(aHoleIndex, prev_hole, curr_hole);
    rPar = std::make_unique<MemoryTracker<u8, 1>>(aPar);
    rWindRads = std::make_unique<MemoryTracker<float, 1>>(aWindRads);
    rWindSpeed = std::make_unique<MemoryTracker<float, 1>>(aWindSpeed);
    rRainBool = std::make_unique<MemoryTracker<u8, 1>>(aRainBool);
    rPin = std::make_unique<MemoryTracker<u8, 1>>(aPin);
    rPin2 = std::make_unique<MemoryTracker<u32, 1>>(aPin2);
    rPowerShotCount = std::make_unique<MemoryTrackerArray<u8, 1, 4>>(aPowerShotCount_P1, 0x5204);
    rHoleDone = std::make_unique<MemoryTrackerArray<u8, 2, 4, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>>(aHoleDone_P1, 0x5204, bool_hi_criteria_0, bool_hi_criteria_1);
    rShotPhase = std::make_unique<MemoryTracker<u8, 2, decltype(sp_swing_start_0), decltype(sp_swing_start_1)>>(aShotPhase, sp_swing_start_0, sp_swing_start_1);
    rPlayerShotStatus_preswing = std::make_unique<MemoryTrackerArray<u32, 2, 4, decltype(pss_preswing_criteria_0), decltype(pss_preswing_criteria_1)>>(aPlayerShotStatus_P1, 0x5204, pss_preswing_criteria_0, pss_preswing_criteria_1);
    rPlayerShotStatus_swing = std::make_unique<MemoryTrackerArray<u32, 2, 4, decltype(pss_swing_criteria_0), decltype(pss_swing_criteria_1)>>(aPlayerShotStatus_P1, 0x5204, pss_swing_criteria_0, pss_swing_criteria_1);
    rPlayerShotStatus_postswing = std::make_unique<MemoryTrackerArray<u32, 2, 4, decltype(pss_postswing_criteria_0), decltype(pss_postswing_criteria_1)>>(aPlayerShotStatus_P1, 0x5204, pss_postswing_criteria_0, pss_postswing_criteria_1);
    rPlayerShotStatus_posthole = std::make_unique<MemoryTrackerArray<u32, 2, 4, decltype(pss_posthole_criteria_0)>>(aPlayerShotStatus_P1, 0x5204, pss_posthole_criteria_0);
    rPlayerShotNum = std::make_unique<MemoryTrackerArray<u8, 1, 4>>(aShotNum_P1, 0x1c8);
    rShotType = std::make_unique<MemoryTracker<u32, 1>>(aShotType);
    rClubType = std::make_unique<MemoryTracker<u32, 1>>(aClubType);
    rPowerMeterSetting = std::make_unique<MemoryTracker<u32, 1>>(aPowerMeterSetting);
    rPowerMeterSettingCopy = std::make_unique<MemoryTracker<u32, 1>>(aPowerMeterSettingCopy);
    rPowerMeterMaximum = std::make_unique<MemoryTracker<u32, 1>>(aPowerMeterMaximum);
    rPowerMeterActual = std::make_unique<MemoryTracker<u32, 1>>(aPowerMeterActual);
    rPowerMeterActualCopy = std::make_unique<MemoryTracker<u32, 1>>(aPowerMeterActualCopy);
    rShotAccuracy = std::make_unique<MemoryTracker<u32, 1>>(aShotAccuracy);
    rShotAccuracy2 = std::make_unique<MemoryTracker<u32, 1>>(aShotAccuracy2);
    rManualVsAuto = std::make_unique<MemoryTracker<u32, 1>>(aManualVsAuto);
    rAimAngleRadians = std::make_unique<MemoryTracker<float, 1>>(aAimAngleRadians);
    rImpactPoint_Y_Preshot = std::make_unique<MemoryTracker<u32, 1>>(aImpactPoint_Y_Preshot);
    rImpactPoint_X_Preshot = std::make_unique<MemoryTracker<u32, 1>>(aImpactPoint_X_Preshot);
    rImpactPoint_X = std::make_unique<MemoryTracker<u32, 1>>(aImpactPoint_X);
    rImpactPoint_Y = std::make_unique<MemoryTracker<u32, 1>>(aImpactPoint_Y);
    rSpin = std::make_unique<MemoryTracker<u32, 1>>(aSpin);
    rBallPos_X = std::make_unique<MemoryTracker<float, 1>>(aBallPos_X);
    rBallPos_Y = std::make_unique<MemoryTracker<float, 1>>(aBallPos_Y);
    rBallPos_Z = std::make_unique<MemoryTracker<float, 1>>(aBallPos_Z);
    rDistanceToHole = std::make_unique<MemoryTracker<float, 1>>(aDistanceToHole);
    rShotDistance = std::make_unique<MemoryTracker<float, 1>>(aShotDistance);
    rLieType = std::make_unique<MemoryTracker<u32, 1>>(aLieType);
    rLieQuality = std::make_unique<MemoryTracker<u32, 1>>(aLieQuality);
    rLieRngRange = std::make_unique<MemoryTracker<u32, 1>>(aLieRngRange);
    rLieRng = std::make_unique<MemoryTracker<u32, 1>>(aLieRng);
    rLieRngSeed = std::make_unique<MemoryTracker<u32, 1>>(aLieRngSeed);
    rFinalScoreHoleTotal = std::make_unique<MemoryTrackerNestedArray<u8, 1>>(aFinalScoreHoleTotal, sizes, offsets);
    rFinalScoreHolePutts = std::make_unique<MemoryTrackerNestedArray<u8, 1>>(aFinalScoreHolePutts, sizes, offsets);
    rFinalScoreHoleStrokes = std::make_unique<MemoryTrackerNestedArray<u8, 1>>(aFinalScoreHoleStrokes, sizes, offsets);
}
