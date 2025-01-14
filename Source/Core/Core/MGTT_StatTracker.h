#pragma once

#include <string>
#include <array>
#include <vector>
#include <optional>
#include <map>
#include <set>
#include <tuple>
#include <iostream>
#include <picojson.h>
#include <utility>
#include <type_traits>
#include <ctime>
#include <fmt/format.h>
#include "Core/HW/Memmap.h"

#include "Common/HttpRequest.h"

#include "Common/FileSearch.h"
#include "Common/FileUtil.h"

#include "Core/LocalPlayers.h"
#include "Core/LocalPlayersConfig.h"
#include "Core/Logger.h"
#include "Core/TrackerAdr.h"
#include "Core/MemoryTracker.h"
#include "Core/MemoryTrackerCriteria.h"
#include "Common/TagSet.h"
#include "Core/RioUtil.h"

#include <iostream>
#include <functional>
#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// TODOs
//   Add extra check for menu scene to prevent tracker from running when game idles and displays demo
//   Fix timing of club memory reads
//   Check netplay game id, users, hosting
//   Check doubles

namespace Tag {
class TagSet;
}

enum class MGTT_State
{
  INIT,
  MENU,           //Wait for IsGolfRound==1
  TRANSITION,     //Wait for next swing - inbetween holes or something
  ROUND_INFO,     //Collect info related to round
  HOLE_INFO,     //Collect info related to round
  SWING,       //Wait for swing to start and Collect Lie and such
  SWING_OVER,
  ROUND_OVER,
  RESET,
  UNDEFINED
};

// Consts
// Stage times
static const u32 cStage_0 = 0;
static const u32 cStage_1 = 0;

// Memory Addresses
static const u32 aIsGolfRound = 0x80162B5F;
constexpr auto bool_hi_criteria_1 = std::make_pair(1, eq(0));
constexpr auto bool_hi_criteria_0 = std::make_pair(0, eq(1));

static const u32 aPlayerPorts = 0x804E6674;
static const u32 aPlayerTurn = 0x804E68FB; 

//0x1=CharSel, 0x2=GameModeSel, 0x3=RoundFormat, 0x4=Handicap. Doubles adds additional scenes as well
static const u32 aMenuScene = 0x8044CDAF;
constexpr auto return_to_menu_1 = std::make_pair(1, eq(0x18));
constexpr auto return_to_menu_0 = std::make_pair(0, not_(eq(0x18)));

constexpr auto start_of_round_1 = std::make_pair(1, not_(eq(0x18)));
constexpr auto start_of_round_0 = std::make_pair(0, eq(0x18));

// Round Addrs
static const u32 aGameMode = 0x804E6753;
//Only run tracker for game modes 0,1,2 (Stroke play, match play, skins)
constexpr auto game_mode_criteria_0 = std::make_pair(0, or_(or_(eq(0), eq(1)), eq(2)));

static const u32 aCourseId = 0x8044afdf;
static const u32 aRoundFormat = 0x8044afe7;
constexpr auto round_format_criteria_1 = std::make_pair(1, not_(eq(5)));
constexpr auto round_format_criteria_0 = std::make_pair(0, eq(5));

static const u32 aGreenType = 0x8044bd9b;
static const u32 aTees = 0x8044afe3;
static const u32 aPlayerCount = 0x804E68FA;

constexpr auto player_count_criteria_7 = std::make_pair(7, eq(0));
constexpr auto player_count_criteria_6 = std::make_pair(6, eq(1));
constexpr auto player_count_criteria_0 = std::make_pair(0, or_(or_(eq(1), eq(2)), or_(eq(4), eq(3))));

constexpr auto debug_criteria = std::make_pair(0, neq_stage<u8>(1));

static const u32 aStarredAtMenu_P1 = 0x8044BA68;

//Handicaps
static const u32 aHandicapsEnabled_P1 = 0x8044C7A8;
static const u32 aSimulationLine_P1 = 0x8044C6B4;
static const u32 aMulligans_P1 = 0x8044C6C4;
static const u32 aHandicapTees_P1 = 0x8044C6D4;

// Chars - InGame
static const u32 aCharIdMenu_P1 = 0x8044BA48;
static const u32 aHandednessMenu_P1 = 0x8044BA90;

// static const u32 aHandedness_P1 = 0x804F205E; // (P2=804F7262)
//

// static const u32 aCharId_P1 = 0x804F205F; 
//

// Clubs - InGame
static const u32 aWoodsMenu_P1 = 0x8017602C;
static const u32 aIronsMenu_P1 = 0x8017602D;
static const u32 aWedgesMenu_P1 = 0x8017602E;

// Hole Addrs
static const u32 aHoleNum = 0x804E674B;
constexpr auto prev_hole = std::make_pair(1, lt(19));
constexpr auto curr_hole = std::make_pair(0, neq_stage<u8>(1));

static const u32 aHoleIndex = 0x804E68F8;
static const u32 aPar = 0x804E685C;
static const u32 aWindRads = 0x804E6850;
static const u32 aWindSpeed = 0x804E6854;
static const u32 aRainBool = 0x804E6858;
static const u32 aPin = 0x804E6714;
static const u32 aPin2 = 0x804E66AC;

//Shot Addrs
static const u32 aShotPhase = 0x804ECD4B;
constexpr auto sp_swing_start_1 = std::make_pair(1, not_(eq(0xB)));
constexpr auto sp_swing_start_0 = std::make_pair(0, eq(0xB));

constexpr auto sp_init_1 = std::make_pair(1, not_(eq(0x5)));
constexpr auto sp_init_0 = std::make_pair(0, eq(0x5));

constexpr auto sp_debug_criteria_0 = std::make_pair(0, neq_stage<u8>(1));

static const u32 aPlayerShotStatus_P1 = 0x804ECE98;
constexpr auto pss_preswing_criteria_1 = std::make_pair(1, eq(4));
constexpr auto pss_preswing_criteria_0 = std::make_pair(0, eq(4));

constexpr auto pss_swing_criteria_1 = std::make_pair(1, and_(not_(eq(7)), not_(eq(6))));
constexpr auto pss_swing_criteria_0 = std::make_pair(0, or_(eq(7), eq(6)));

constexpr auto pss_postswing_criteria_1 = std::make_pair(1, not_(eq(8)));
constexpr auto pss_postswing_criteria_0 = std::make_pair(0, eq(8));

constexpr auto pss_posthole_criteria_0 = std::make_pair(0, or_(gt(8), lt(4)));

constexpr auto pss_debug_criteria_0 = std::make_pair(0, neq_stage<u8>(1));

// Shot
static const u32 aShotNum_P1 = 0x806CB4FC;
static const u32 aShotType = 0x804ecd50;
static const u32 aClubType = 0x804ecd58;
static const u32 aPowerMeterSetting = 0x804e3658;
static const u32 aPowerMeterSettingCopy = 0x804ecd54;
static const u32 aPowerMeterMaximum = 0x804ecdd4;
static const u32 aPowerMeterActual = 0x804e6768;
static const u32 aPowerMeterActualCopy = 0x804ecd1c;
static const u32 aShotAccuracy = 0x80523cac;
static const u32 aShotAccuracy2 = 0x804ECD30; //Confirm
static const u32 aManualVsAuto = 0x804ecd88;
static const u32 aAimAngleRadians = 0x804ecd5c;
static const u32 aImpactPoint_Y_Preshot = 0x804ECDA0;
static const u32 aImpactPoint_X_Preshot = 0x804ECDA4;
static const u32 aImpactPoint_X = 0x804ecdac;
static const u32 aImpactPoint_Y = 0x804ecda8;
static const u32 aSpin = 0x804ecd4c;


// Lie/PostShot
static const u32 aPowerShotCount_P1 = 0x804F2070;
static const u32 aHoleDone_P1 = 0x804ECE7C;
static const u32 aBallPos_X = 0x805DC0B0;
static const u32 aBallPos_Y = 0x805DC0C0;
static const u32 aBallPos_Z = 0x805DC0D0;
static const u32 aDistanceToHole = 0x802D7368;
static const u32 aShotDistance = 0x803A46C4;
static const u32 aLieType = 0x804e364c;
static const u32 aLieQuality = 0x804e3650;
static const u32 aLieRngRange = 0x804ecdbc;
static const u32 aLieRng = 0x804ecdc0;
static const u32 aLieRngSeed = 0x804ecd3c;


// //Final Score Addrs
constexpr uint32_t PLAYER_OFFSET = 0x1c8;
constexpr uint32_t HOLE_OFFSET = 0x8;
constexpr uint32_t NUM_PLAYERS = 4;
constexpr uint32_t NUM_HOLES = 18;
constexpr std::size_t NUM_STAGES = 1;
static const u32 aFinalScoreHoleTotal   = 0x806CB348;
static const u32 aFinalScoreHolePutts   = 0x806CB349;
static const u32 aFinalScoreHoleStrokes = 0x806CB34A;

// Type definition for MGTT_State transition functions
using StateFunction = std::function<void(const Core::CPUThreadGuard&)>;

// MGTT_StatTracker class
class MGTT_StatTracker {
public:
    MGTT_StatTracker() : currentState(MGTT_State::INIT) {
        // MGTT_StatTracker MGTT_State functions map
        stateFunctions[MGTT_State::INIT] = [this](const Core::CPUThreadGuard& guard) { this->initState(guard); };
        stateFunctions[MGTT_State::MENU] = [this](const Core::CPUThreadGuard& guard) { this->menuState(guard); };
        stateFunctions[MGTT_State::TRANSITION] = [this](const Core::CPUThreadGuard& guard) { this->transitionState(guard); };
        stateFunctions[MGTT_State::HOLE_INFO] = [this](const Core::CPUThreadGuard& guard) { this->holeInfoState(guard); };
        stateFunctions[MGTT_State::SWING] = [this](const Core::CPUThreadGuard& guard) { this->swingState(guard); };
        stateFunctions[MGTT_State::SWING_OVER] = [this](const Core::CPUThreadGuard& guard) { this->swingOverState(guard); };
        stateFunctions[MGTT_State::ROUND_OVER] = [this](const Core::CPUThreadGuard& guard) { this->roundOverState(guard); };
        stateFunctions[MGTT_State::RESET] = [this](const Core::CPUThreadGuard& guard) { this->resetState(guard); };
        stateFunctions[MGTT_State::UNDEFINED] = [this](const Core::CPUThreadGuard& guard) { this->undefinedState(guard); };

        logger << "Init MGTT_StatTracker" << std::endl;
    }

    void initMemoryTrackers();
    void resetMemoryTrackers();

    // Method to transition to a new MGTT_State
    void transitionTo(MGTT_State newState) {
        if (stateFunctions.find(newState) != stateFunctions.end()) {
            logger << "  " << stateToString(currentState) << " -> " << stateToString(newState) << "\n";
            currentState = newState;
        } else {
            logger << " ERROR: Invalid MGTT_State transition. InvalidState=" << stateToString(newState) << "\n";
        }
    }

    // Method to start the MGTT_State machine
    void start() {
        transitionTo(MGTT_State::MENU);
    }

    // Method to run the current state's function
    void run(const Core::CPUThreadGuard& guard) {
        ++frame;
        if (stateFunctions.find(currentState) != stateFunctions.end()) {
            stateFunctions[currentState](guard);
        } else {
            std::cerr << "No function defined for the current state!" << std::endl;
        }
    }

    // RioFunctions
    void setNetplaySession(bool netplay_session, std::string opponent_name){
        rioInfo.netplay = netplay_session;
    }

    void setIsNetplayHost(std::optional<bool> isNetplayHost){
        if (rioInfo.netplay && !isNetplayHost.has_value()){
            logger << "ERROR: Netplay session but invalid host\n";
        }
        rioInfo.is_netplay_host = *isNetplayHost;
    }

    void setNetplayerUserInfo(std::map<int, LocalPlayers::LocalPlayers::Player> userInfo){
        rioInfo.rioUsers = userInfo;
    }
    void setTagSet(std::optional<Tag::TagSet> tag_set) {
        if (tag_set){
            rioInfo.tag_set_id_name = std::make_pair((*tag_set).id, (*tag_set).name);
        }
        else { clearTagSet(); }
    }
    void clearTagSet() {
        rioInfo.tag_set_id_name = std::nullopt;
    }

    std::optional<uint64_t> getGameID(){
        return gameID;
    }

    // Only accept incoming GameID if netplay and not the host
    void setGameID(uint64_t inGameID );
    

    // Method to convert MGTT_State to string
    std::string stateToString(MGTT_State state) const {
        switch (state) {
            case MGTT_State::INIT: return "INIT";
            case MGTT_State::MENU: return "MENU";
            case MGTT_State::TRANSITION: return "TRANSITION";
            case MGTT_State::ROUND_INFO: return "ROUND_INFO";
            case MGTT_State::HOLE_INFO: return "HOLE_INFO";
            case MGTT_State::SWING: return "SWING";
            case MGTT_State::SWING_OVER: return "SWING_OVER";
            case MGTT_State::ROUND_OVER: return "ROUND_OVER";
            case MGTT_State::RESET: return "RESET";
            case MGTT_State::UNDEFINED: return "UNDEFINED";
            default: return "INVALID";
        }
    }

    void debugMemoryTrackers(const Core::CPUThreadGuard& guard)
    {
        rShotPhaseDebug->run(guard);
        rPlayerShotStatusDebug->run(guard);

        if ((*rPlayerShotStatusDebug)[0].isActive() || (*rPlayerShotStatusDebug)[1].isActive() || (*rPlayerShotStatusDebug)[2].isActive() || (*rPlayerShotStatusDebug)[3].isActive() || rShotPhaseDebug->isActive()){
            logger << fmt::format("DEBUG frame={}\n", frame);
            logger << fmt::format("DEBUG prev ShotStatus={:02x} | {:02x} | {:02x} | {:02x} ShotPhase={:02x}\n", *(*rPlayerShotStatusDebug)[0].getValue(1), *(*rPlayerShotStatusDebug)[1].getValue(1), *(*rPlayerShotStatusDebug)[2].getValue(1), *(*rPlayerShotStatusDebug)[3].getValue(1), *rShotPhaseDebug->getValue(1));
            logger << fmt::format("DEBUG curr ShotStatus={:02x} | {:02x} | {:02x} | {:02x} ShotPhase={:02x}\n", *(*rPlayerShotStatusDebug)[0].getValue(0), *(*rPlayerShotStatusDebug)[1].getValue(0), *(*rPlayerShotStatusDebug)[2].getValue(0), *(*rPlayerShotStatusDebug)[3].getValue(0), *rShotPhaseDebug->getValue(0));
        }

        rIsGolfRoundDebug->run(guard);
        if (rIsGolfRoundDebug->isActive()){
            logger << fmt::format("DEBUG frame={}\n", frame);
            logger << fmt::format("DEBUG | prev rIsGolfRound={:02x}\n", *rIsGolfRoundDebug->getValue(1));
            logger << fmt::format("DEBUG | curr rIsGolfRound={:02x}\n", *rIsGolfRoundDebug->getValue(0));
        }

        // rPlayerCountDebug->run(guard);
        // if (rPlayerCountDebug->isActive()){
        //     logger << fmt::format("DEBUG frame={}\n", frame);
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 7, *rPlayerCountDebug->getValue(7));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 6, *rPlayerCountDebug->getValue(6));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 5, *rPlayerCountDebug->getValue(5));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 4, *rPlayerCountDebug->getValue(4));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 3, *rPlayerCountDebug->getValue(3));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 2, *rPlayerCountDebug->getValue(2));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 1, *rPlayerCountDebug->getValue(1));
        //     logger << fmt::format("DEBUG {} PlayerCount={:02x}\n", 0, *rPlayerCountDebug->getValue(0));
        // }

        
        // rGameModeDebug->run(guard);
        // if (rGameModeDebug->isActive()){
        //     logger << fmt::format("DEBUG frame={}\n", frame);
        //     logger << fmt::format("DEBUG | prev rGameMode={:02x}\n", *rGameModeDebug->getValue(1));
        //     logger << fmt::format("DEBUG | curr rGameMode={:02x}\n", *rGameModeDebug->getValue(0));
        // }
        // rRoundFormatDebug->run(guard);
        // if (rRoundFormatDebug->isActive()){
        //     logger << fmt::format("DEBUG frame={}\n", frame);
        //     logger << fmt::format("DEBUG | prev rRoundFormat={:02x}\n", *rRoundFormatDebug->getValue(1));
        //     logger << fmt::format("DEBUG | curr rRoundFormat={:02x}\n", *rRoundFormatDebug->getValue(0));
        // }
        // rCourseDebug->run(guard);
        // if (rCourseDebug->isActive()){
        //     logger << fmt::format("DEBUG frame={}\n", frame);
        //     logger << fmt::format("DEBUG | prev rCourse={:02x}\n", *rCourseDebug->getValue(1));
        //     logger << fmt::format("DEBUG | curr rCourse={:02x}\n", *rCourseDebug->getValue(0));
        // }
        // rWoodsDebug->run(guard);
        // if ((*rWoodsDebug)[0].isActive() || (*rWoodsDebug)[1].isActive() || (*rWoodsDebug)[2].isActive() || (*rWoodsDebug)[3].isActive()){
        //     logger << fmt::format("DEBUG frame={}\n", frame);
        //     logger << fmt::format("DEBUG prev Woods={:02x} | {:02x} | {:02x} | {:02x}\n", *(*rWoodsDebug)[0].getValue(1), *(*rWoodsDebug)[1].getValue(1), *(*rWoodsDebug)[2].getValue(1), *(*rWoodsDebug)[3].getValue(1));
        //     logger << fmt::format("DEBUG curr Woods={:02x} | {:02x} | {:02x} | {:02x}\n", *(*rWoodsDebug)[0].getValue(0), *(*rWoodsDebug)[1].getValue(0), *(*rWoodsDebug)[2].getValue(0), *(*rWoodsDebug)[3].getValue(0));
        // }
    }

    void log(std::string msg){
        logger << msg;
    }

private:
    MGTT_State currentState;
    std::unordered_map<MGTT_State, StateFunction> stateFunctions;
    int frame = 0;

    // Memory Trackers
    std::unique_ptr<MemoryTracker<u8, 2, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>> rIsGolfRound;
    std::unique_ptr<MemoryTracker<u32, 1>> rPlayerPorts; //Array of bytes
    std::unique_ptr<MemoryTracker<u8, 1>> rPlayerTurn;
    std::unique_ptr<MemoryTracker<u8, 25, decltype(game_mode_criteria_0)>> rGameMode;
    std::unique_ptr<MemoryTracker<u8, 25, decltype(return_to_menu_1), decltype(return_to_menu_0)>> rMenuScene;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(start_of_round_0), decltype(start_of_round_1)>> rMenuScene_start_round;
    std::unique_ptr<MemoryTracker<u8, 25>> rCourseId;
    std::unique_ptr<MemoryTracker<u8, 25, decltype(round_format_criteria_0), decltype(round_format_criteria_1)>> rRoundFormat;
    std::unique_ptr<MemoryTracker<u8, 25>> rGreenType;
    std::unique_ptr<MemoryTracker<u8, 25>> rTees;
    std::unique_ptr<MemoryTracker<u8, 9, decltype(player_count_criteria_0), decltype(player_count_criteria_6), decltype(player_count_criteria_7)>> rPlayerCount;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rStarredAtMenu;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rHandicapsEnabled;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rSimulationLine;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rMulligans;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rHandicapTees;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rCharId;
    std::unique_ptr<MemoryTrackerArray<u32, 25, 4>> rHandedness;
    // std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rHandedness;
    // std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rCharId;
    std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rWoods;
    std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rIrons;
    std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rWedges;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(prev_hole), decltype(curr_hole)>> rHoleNum;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(prev_hole), decltype(curr_hole)>> rHoleIndex;
    std::unique_ptr<MemoryTracker<u8, 1>> rPar;
    std::unique_ptr<MemoryTracker<float, 1>> rWindRads;
    std::unique_ptr<MemoryTracker<float, 1>> rWindSpeed;
    std::unique_ptr<MemoryTracker<u8, 1>> rRainBool;
    std::unique_ptr<MemoryTracker<u8, 1>> rPin;
    std::unique_ptr<MemoryTracker<u32, 1>> rPin2;
    std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rPowerShotCount;
    std::unique_ptr<MemoryTrackerArray<u8, 2, 4, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>> rHoleDone;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(sp_swing_start_0), decltype(sp_swing_start_1)>> rShotPhase_swing;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(sp_init_0), decltype(sp_init_1)>> rShotPhase_init;
    std::unique_ptr<MemoryTrackerArray<u32, 2, 4, decltype(pss_preswing_criteria_0), decltype(pss_preswing_criteria_1)>> rPlayerShotStatus_preswing;
    std::unique_ptr<MemoryTrackerArray<u32, 2, 4, decltype(pss_swing_criteria_0), decltype(pss_swing_criteria_1)>> rPlayerShotStatus_swing;
    std::unique_ptr<MemoryTrackerArray<u32, 2, 4, decltype(pss_postswing_criteria_0), decltype(pss_postswing_criteria_1)>> rPlayerShotStatus_postswing;
    std::unique_ptr<MemoryTrackerArray<u32, 2, 4, decltype(pss_posthole_criteria_0)>> rPlayerShotStatus_posthole;
    std::unique_ptr<MemoryTrackerArray<u8, 1, 4>> rPlayerShotNum;
    std::unique_ptr<MemoryTracker<u32, 1>> rShotType;
    std::unique_ptr<MemoryTracker<u32, 1>> rClubType;
    std::unique_ptr<MemoryTracker<u32, 1>> rPowerMeterSetting;
    std::unique_ptr<MemoryTracker<u32, 1>> rPowerMeterSettingCopy;
    std::unique_ptr<MemoryTracker<u32, 1>> rPowerMeterMaximum;
    std::unique_ptr<MemoryTracker<u32, 1>> rPowerMeterActual;
    std::unique_ptr<MemoryTracker<u32, 1>> rPowerMeterActualCopy;
    std::unique_ptr<MemoryTracker<u32, 1>> rShotAccuracy;
    std::unique_ptr<MemoryTracker<u32, 1>> rShotAccuracy2;
    std::unique_ptr<MemoryTracker<u32, 1>> rManualVsAuto;
    std::unique_ptr<MemoryTracker<float, 1>> rAimAngleRadians;
    std::unique_ptr<MemoryTracker<u32, 1>> rImpactPoint_Y_Preshot;
    std::unique_ptr<MemoryTracker<u32, 1>> rImpactPoint_X_Preshot;
    std::unique_ptr<MemoryTracker<u32, 1>> rImpactPoint_X;
    std::unique_ptr<MemoryTracker<u32, 1>> rImpactPoint_Y;
    std::unique_ptr<MemoryTracker<u32, 1>> rSpin;
    std::unique_ptr<MemoryTracker<float, 1>> rBallPos_X;
    std::unique_ptr<MemoryTracker<float, 1>> rBallPos_Y;
    std::unique_ptr<MemoryTracker<float, 1>> rBallPos_Z;
    std::unique_ptr<MemoryTracker<float, 1>> rDistanceToHole;
    std::unique_ptr<MemoryTracker<float, 1>> rShotDistance;
    std::unique_ptr<MemoryTracker<u32, 1>> rLieType;
    std::unique_ptr<MemoryTracker<u32, 1>> rLieQuality;
    std::unique_ptr<MemoryTracker<u32, 1>> rLieRngRange;
    std::unique_ptr<MemoryTracker<u32, 1>> rLieRng;
    std::unique_ptr<MemoryTracker<u32, 1>> rLieRngSeed;
    std::unique_ptr<MemoryTrackerNestedArray<u8, 1>> rFinalScoreHoleTotal;
    std::unique_ptr<MemoryTrackerNestedArray<u8, 1>> rFinalScoreHolePutts;
    std::unique_ptr<MemoryTrackerNestedArray<u8, 1>> rFinalScoreHoleStrokes;

    // Debug MemoryTrackers
    std::unique_ptr<MemoryTracker<u8, 2, decltype(sp_debug_criteria_0)>>           rShotPhaseDebug;
    std::unique_ptr<MemoryTrackerArray<u32, 2, 4, decltype(pss_debug_criteria_0)>> rPlayerShotStatusDebug;
    std::unique_ptr<MemoryTracker<u8, 8, decltype(debug_criteria)>> rPlayerCountDebug;

    std::unique_ptr<MemoryTracker<u8, 2, decltype(debug_criteria)>> rGameModeDebug;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(debug_criteria)>> rRoundFormatDebug;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(debug_criteria)>> rCourseDebug;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(debug_criteria)>> rIsGolfRoundDebug;
    std::unique_ptr<MemoryTrackerArray<u8, 2, 4, decltype(debug_criteria)>> rWoodsDebug;

    const std::vector<uint32_t> sizes = {NUM_PLAYERS, NUM_HOLES};
    const std::vector<uint32_t> offsets = {PLAYER_OFFSET, HOLE_OFFSET};

    // GameID
    std::optional<uint64_t> gameID = std::nullopt;

    // JSON Info
    std::unique_ptr<json> writer;
    std::unique_ptr<std::unordered_map<std::string, json>> holes; //Needs map to preserve data across entire round
    std::unique_ptr<std::unordered_map<std::string, std::unordered_map<std::string, json>>> shots; //Needs map to preserve data across entire round
    std::unique_ptr<json> summary;

    // Debug Logger
    RioUtil::Logger logger = RioUtil::Logger("DebugLogger", RioUtil::LogOutput::BOTH, File::GetUserPath(D_MGTTFILES_IDX)+"logfile.txt");


    void initWriter(std::string filename) {
        std::string outputFilename = fmt::format("{}{}.json", File::GetUserPath(D_MGTTFILES_IDX), filename);
        logger << "JSON File Loc=" << outputFilename << "\n";
        writer = std::make_unique<json>();
        holes = std::make_unique<std::unordered_map<std::string, json>>();
        shots = std::make_unique<std::unordered_map<std::string, std::unordered_map<std::string, json>>>();
        summary = std::make_unique<json>();
        (*writer)["filename"] = outputFilename;
    }

    void resetWriter() {
        writer.reset();
        holes.reset();
        shots.reset();
        summary.reset();
    }

    void writeJSON() {
        std::string filename = (*writer)["filename"];
        std::ofstream json_file(filename);
        logger << "Attempting JSON Write: " << filename << std::endl;
        if (json_file.is_open()) {
            json_file << writer->dump(4); // Dump with an indentation of 4 spaces
            json_file.close();
            logger << "JSON file created successfully." << std::endl;
        } else {
            std::cerr << "Could not open the file for writing." << std::endl;
        }
    }

    template<typename T>
    std::string getHoleName(T holeNumber, T holeIndex = 0){
        std::string paddedHoleNumber = fmt::format("{:02}", holeNumber);
        std::string paddedHoleIndex = fmt::format("{:02}", holeIndex);

        return fmt::format("{}_H{}", paddedHoleIndex, paddedHoleNumber);
    }

    template<typename T, typename U>
    std::string getShotName(T playerNum, U shotNum){
        return fmt::format("S{}_{}", playerNum, shotNum);
    }

    bool shotExists(std::string holeName, std::string shotName){
        // Check if the hole exists in the map
        bool shotExists = false;
        auto holeIt = shots->find(holeName);
        if (holeIt != shots->end()) {
            // Hole exists, now check if the shot exists in the inner map
            auto shotIt = holeIt->second.find(shotName);
            shotExists = (shotIt != holeIt->second.end());
        }
        return shotExists;
    }

    void assembleFinalJson(std::unique_ptr<json>& writerRef) {
        json holesJson;

        for (const auto& holePair : (*holes)) {
            const std::string& holeName = holePair.first;
            const json& holeData = holePair.second;

            json hole = holeData;
            hole["Shots"] = (*shots)[holeName];

            holesJson[holeName] = hole;
        }

        (*writerRef)["Holes"] = holesJson;
        (*writerRef)["Summary"] = (*summary);
    }

    // Netplay, TagSet, Meta data
    struct RioInfo {
        std::map<int, LocalPlayers::LocalPlayers::Player> rioUsers;
        std::optional<std::pair<int, std::string>> tag_set_id_name;
        bool netplay = false;
        bool is_netplay_host = false;
    } rioInfo;

    void readLocalPlayers(int num_players);

    // State Machine
    void initState(const Core::CPUThreadGuard& guard) {
        initMemoryTrackers();
        transitionTo(MGTT_State::MENU);

        // Only generate a GameID if hosting netplay or playing locally. Nonhosts will get GameID from the host over netplay.
        if (!rioInfo.netplay || rioInfo.is_netplay_host) {
            gameID = RioUtil::genRand64();
        }
    }

    void menuState(const Core::CPUThreadGuard& guard) {
        debugMemoryTrackers(guard);
        if (!rPlayerCount->isActive()) {
            rGameMode->run(guard);
            rCourseId->run(guard);
            rRoundFormat->run(guard);
            rGreenType->run(guard);
            rTees->run(guard);
            rPlayerCount->run(guard);
            rPlayerPorts->run(guard);
            //Per player, Menu (Stage15)
            rStarredAtMenu->run(guard);
            rHandicapsEnabled->run(guard);
            rSimulationLine->run(guard);
            rMulligans->run(guard);
            rHandicapTees->run(guard);

            //Per Player, InGame (Stage0)
            rCharId->run(guard);
            rHandedness->run(guard);
            rWoods->run(guard);
            rIrons->run(guard);
            rWedges->run(guard);

            if (rPlayerCount->isActive() && rGameMode->isActive()) {
                if (!gameID){
                    logger << "ERROR: gameID not set\n";
                }
                logger << fmt::format("ROUND_INFO | frame={}", frame);
                initWriter(fmt::format("{:x}", *gameID));

                //Read start time
                std::time_t unix_time = std::time(nullptr);
                std::asctime(std::localtime(&unix_time));


                std::string tagSet = rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null";
                logger << fmt::format("ROUND_INFO | GameID={:x}, GameMode={}, TagSet={}, CourseID={}, RoundFormat={}, PlayerCount={}\n", *gameID, *rGameMode->getValue(0), tagSet, 
                                      *rCourseId->getValue(20), *rRoundFormat->getValue(20), *rPlayerCount->getValue());
                logger << fmt::format("ROUND_INFO | Netplay={}, IsHost={}\n", rioInfo.netplay, rioInfo.is_netplay_host);

                (*writer)["GameID"] = fmt::format("{:x}", *gameID);
                (*writer)["StartTime"] = std::asctime(std::localtime(&unix_time));
                (*writer)["TagSetID"] = rioInfo.tag_set_id_name.has_value() ? std::to_string((*rioInfo.tag_set_id_name).first) : "null";
                (*writer)["TagSet"] = rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null";
                (*writer)["GameMode"] = *rGameMode->getValue();
                (*writer)["CourseID"] = *rCourseId->getValue(20);
                (*writer)["RoundFormat"] = *rRoundFormat->getValue(20);
                (*writer)["GreenType"] = *rGreenType->getValue(20);
                (*writer)["Tees"] = *rTees->getValue(20);
                (*writer)["PlayerCount"] = *rPlayerCount->getValue();

                // Get User info
                readLocalPlayers(*rPlayerCount->getValue());

                // Process users
                std::vector<json> golfers;
                for (int i=0; i < *rPlayerCount->getValue(); ++i){
                    auto player_port = rPlayerPorts->getByteValue(0, i);

                    json golfer;

                    //Get Rio Name
                    logger << fmt::format("\nPLAYER_INFO | Player={}, Port={}, Username={}\n", std::to_string(i), std::to_string(*player_port), rioInfo.rioUsers[i].GetUsername());
                    logger << fmt::format("PLAYER_INFO | Player={}, CharId={}, Starred={}, Handedness={}, Handicaps={}\n", 
                                          std::to_string(i), (*(*rCharId)[i].getValue(20)), (*(*rStarredAtMenu)[i].getValue(20)),
                                          (*(*rHandedness)[i].getValue(20)), (*(*rHandicapsEnabled)[i].getValue(20)));
                    logger << fmt::format("PLAYER_INFO | Player={}, Handicaps: Simline={}, Muligans={}, Tees={}\n", 
                                          std::to_string(i), (*(*rSimulationLine)[i].getValue(20)), (*(*rMulligans)[i].getValue(20)), (*(*rHandicapTees)[i].getValue(20)));
                    logger << fmt::format("PLAYER_INFO | Player={}, Clubs: Woods={}, Irons={}, Wedges={}\n", 
                                          std::to_string(i), (*(*rWoods)[i].getValue(0)), (*(*rIrons)[i].getValue(0)), (*(*rWedges)[i].getValue(0)));

                    golfer["Port"] = *player_port;
                    golfer["RioUsername"] = rioInfo.rioUsers[i].GetUsername();

                    golfer["HandicapsEnabled"] = (*(*rHandicapsEnabled)[i].getValue(20));
                    golfer["SimulationLine"] = (*(*rSimulationLine)[i].getValue(20));
                    golfer["Mulligans"] = (*(*rMulligans)[i].getValue(20));
                    golfer["HandicapTees"] = (*(*rHandicapTees)[i].getValue(20));
                    golfer["CharId"] = (*(*rCharId)[i].getValue(20));
                    golfer["Starred"] = (*(*rStarredAtMenu)[i].getValue(20));
                    golfer["Handedness"] = (*(*rHandedness)[i].getValue(20));
                    golfer["Woods"] = (*(*rWoods)[i].getValue(0)); //TODO needs timing correction
                    golfer["Irons"] = (*(*rIrons)[i].getValue(0)); //TODO needs timing correction
                    golfer["Wedges"] = (*(*rWedges)[i].getValue(0)); //TODO needs timing correction

                    golfers.emplace_back(golfer);
                }
                (*writer)["Golfers"] = golfers;
            }
        }

        //Catch the transition to inGame
        if (!rMenuScene_start_round->isActive()){
            rMenuScene_start_round->run(guard);
        }

        rShotPhase_init->run(guard);
        if (rMenuScene_start_round->isActive() && rShotPhase_init->isActive()) {
            auto final_menu_scene = *(rMenuScene_start_round->getValue(1));

            //If previous menu screen is 0 then we are in demo/lessons and do not want to record
            //GameMode must be  StrokePlay, MatchPlay, or Skins
            if ((final_menu_scene != 0) && rGameMode->isActive()){
                transitionTo(MGTT_State::HOLE_INFO);
            }
            else {
                transitionTo(MGTT_State::RESET);
            }
        }

    }

    void holeInfoState(const Core::CPUThreadGuard& guard) {
        rHoleNum->run(guard);
        rHoleIndex->run(guard);
        rWindRads->run(guard);
        rWindSpeed->run(guard);
        rRainBool->run(guard);
        rPin->run(guard);
        rPin2->run(guard);
        rPar->run(guard);

        std::string holeName = getHoleName(*rHoleNum->getValue(), *rHoleIndex->getValue());
        logger << fmt::format("\nHOLE_INFO | HoleIndex={}, HoleNum={}\n", *rHoleIndex->getValue(), *rHoleNum->getValue());

        json hole;
        hole["HoleNum"] = *rHoleNum->getValue();
        hole["HoleIndex"] = *rHoleIndex->getValue();
        hole["Par"] = *rPar->getValue();
        hole["WindRads"] = *rWindRads->getValue();
        hole["WindSpeed"] = *rWindSpeed->getValue();
        hole["RainBool"] = *rRainBool->getValue();
        hole["Pin"] = *rPin->getValue();
        hole["Pin2"] = *rPin2->getValue();

        (*holes)[holeName] = hole;
        transitionTo(MGTT_State::TRANSITION);

        debugMemoryTrackers(guard);
    }

    void transitionState(const Core::CPUThreadGuard& guard) {
        // Check for new hole
        rHoleIndex->run(guard);
        if (rHoleIndex->isActive()){
            transitionTo(MGTT_State::HOLE_INFO);
        }

        // Check for swing
        rShotPhase_swing->run(guard);
        rPlayerTurn->run(guard);
        auto player_turn = rPlayerTurn->getValue();
        // Check for swing start
        if (player_turn && (*player_turn <= 3)) {
            //Is Golfer ready to swing?
            if (rShotPhase_swing->isActive()){
                transitionTo(MGTT_State::SWING);
            }
        }

        // Check if game is over
        rMenuScene->run(guard);
        if (rMenuScene->isActive()) {
            transitionTo(MGTT_State::ROUND_OVER);
        }

        debugMemoryTrackers(guard);
    }

    void swingState(const Core::CPUThreadGuard& guard) {
        rPlayerTurn->run(guard);
        auto player_turn = rPlayerTurn->getValue();

        if (player_turn && (*player_turn <= 3)) {
            (*rPlayerShotStatus_swing)[*player_turn].run(guard);
            (*rPlayerShotStatus_postswing)[*player_turn].run(guard);
            (*rHoleDone)[*player_turn].run(guard);            
            
            // Has golfer swung?
            if ((*rPlayerShotStatus_swing)[*player_turn].isActive()){
                // Build Bookkeepping Vars
                rPlayerShotNum->run(guard);
                auto player_shot_num = (*rPlayerShotNum)[*player_turn].getValue();
                std::string shotName = getShotName(*player_turn, *player_shot_num);
                std::string holeName = getHoleName(*rHoleNum->getValue(), *rHoleIndex->getValue());
                logger << fmt::format("SWING | Player={}, HoleName={}, ShotName={}\n", *player_turn, holeName, shotName);

                //Collect swing info
                rShotType->run(guard);
                rClubType->run(guard);
                rPowerMeterSetting->run(guard);
                rPowerMeterSettingCopy->run(guard);
                rPowerMeterMaximum->run(guard);
                rPowerMeterActual->run(guard);
                rPowerMeterActualCopy->run(guard);
                rShotAccuracy->run(guard);
                rShotAccuracy2->run(guard);
                rManualVsAuto->run(guard);
                rAimAngleRadians->run(guard);
                rImpactPoint_Y_Preshot->run(guard);
                rImpactPoint_X_Preshot->run(guard);
                rImpactPoint_X->run(guard);
                rImpactPoint_Y->run(guard);
                rSpin->run(guard);

                json shot;
                shot["ShotNum"] = *player_shot_num;
                shot["ShotType"] = *rShotType->getValue();
                shot["ClubType"] = *rClubType->getValue();
                shot["PowerMeterSetting"] = *rPowerMeterSetting->getValue();
                shot["PowerMeterSettingCopy"] = *rPowerMeterSettingCopy->getValue();
                shot["PowerMeterMaximum"] = *rPowerMeterMaximum->getValue();
                shot["PowerMeterActual"] = *rPowerMeterActual->getValue();
                shot["PowerMeterActualCopy"] = *rPowerMeterActualCopy->getValue();
                shot["ShotAccuracy"] = *rShotAccuracy->getValue();
                shot["ShotAccuracy2"] = *rShotAccuracy2->getValue();
                shot["ManualVsAuto"] = *rManualVsAuto->getValue();
                shot["AimAngleRadians"] = *rAimAngleRadians->getValue();
                shot["ImpactPoint_Y_Preshot"] = *rImpactPoint_Y_Preshot->getValue();
                shot["ImpactPoint_X_Preshot"] = *rImpactPoint_X_Preshot->getValue();
                shot["ImpactPoint_X"] = *rImpactPoint_X->getValue();
                shot["ImpactPoint_Y"] = *rImpactPoint_Y->getValue();
                shot["Spin"] = *rSpin->getValue();

                (*shots)[holeName][shotName] = shot;
            }
            else if ((*rHoleDone)[*player_turn].isActive()){
                // Build Bookkeepping Vars
                auto player_shot_num = (*rPlayerShotNum)[*player_turn].getValue();
                std::string shotName = getShotName(*player_turn, *player_shot_num);
                std::string holeName = getHoleName(*rHoleNum->getValue(), *rHoleIndex->getValue());
                logger << fmt::format("HOLE_DONE | Player={}, HoleName={}, ShotName={}\n\n", *player_turn, holeName, shotName);

                rBallPos_X->run(guard);
                rBallPos_Y->run(guard);
                rBallPos_Z->run(guard);
                rPowerShotCount->run(guard);

                if (!shotExists(holeName, shotName)){
                    logger << "ERROR: No shot associated with this swing result\n";
                }

                json& shot = (*shots)[holeName][shotName];
                shot["BallPos_X"] = *rBallPos_X->getValue();
                shot["BallPos_Y"] = *rBallPos_Y->getValue();
                shot["BallPos_Z"] = *rBallPos_Z->getValue();
                shot["PowerShotCount"] = *(*rPowerShotCount)[*player_turn].getValue();
                shot["Result"] = 1;

                transitionTo(MGTT_State::SWING_OVER);
            }
            else if ((*rPlayerShotStatus_postswing)[*player_turn].isActive()){ //Post Swing
                // Build Bookkeepping Vars
                auto player_shot_num = (*rPlayerShotNum)[*player_turn].getValue();
                std::string shotName = getShotName(*player_turn, *player_shot_num);
                std::string holeName = getHoleName(*rHoleNum->getValue(), *rHoleIndex->getValue());
                logger << fmt::format("POST_SWING | Player={}, HoleName={}, ShotName={}\n\n", *player_turn, holeName, shotName);
                
                //Collect lie/result info
                rBallPos_X->run(guard);
                rBallPos_Y->run(guard);
                rBallPos_Z->run(guard);
                rDistanceToHole->run(guard);
                rLieType->run(guard);
                rLieQuality->run(guard);
                rLieRngRange->run(guard);
                rLieRng->run(guard);
                rLieRngSeed->run(guard);
                rShotDistance->run(guard);
                rPowerShotCount->run(guard);

                if (!shotExists(holeName, shotName)){
                    logger << "ERROR: No shot associated with this swing result\n";
                }
                
                json& shot = (*shots)[holeName][shotName];
                shot["BallPos_X"] = *rBallPos_X->getValue();
                shot["BallPos_Y"] = *rBallPos_Y->getValue();
                shot["BallPos_Z"] = *rBallPos_Z->getValue();
                shot["DistanceToHole"] = *rDistanceToHole->getValue();
                shot["LieType"] = *rLieType->getValue();
                shot["LieQuality"] = *rLieQuality->getValue();
                shot["LieRngRange"] = *rLieRngRange->getValue();
                shot["LieRng"] = *rLieRng->getValue();
                shot["LieRngSeed"] = *rLieRngSeed->getValue();
                shot["PowerShotCount"] = *(*rPowerShotCount)[*player_turn].getValue();
                shot["Result"] = 0;

                transitionTo(MGTT_State::SWING_OVER);
            }
        }

        // Check if game is over
        rMenuScene->run(guard);
        if (rMenuScene->isActive()) {
            transitionTo(MGTT_State::ROUND_OVER);
        }

        debugMemoryTrackers(guard);
    }

    void swingOverState(const Core::CPUThreadGuard& guard) {
        rPlayerTurn->run(guard);
        auto player_turn = rPlayerTurn->getValue();

        if (player_turn && (*player_turn <= 3)) {
            (*rPlayerShotStatus_posthole)[*player_turn].run(guard);
            
            // Swing is over, record final stats
            if ((*rPlayerShotStatus_posthole)[*player_turn].isActive()){
                auto player_shot_num = (*rPlayerShotNum)[*player_turn].getValue();
                std::string shotName = getShotName(*player_turn, *player_shot_num);
                std::string holeName = getHoleName(*rHoleNum->getValue(), *rHoleIndex->getValue());
                logger << fmt::format("SWING_OVER | Player={}, HoleName={}, ShotName={}, ShotStatus={}\n", *player_turn, holeName, shotName, *(*rPlayerShotStatus_posthole)[*player_turn].getValue());
                rFinalScoreHoleTotal->run(guard);
                rFinalScoreHolePutts->run(guard);
                rFinalScoreHoleStrokes->run(guard);
                transitionTo(MGTT_State::TRANSITION);
            }
        }

        debugMemoryTrackers(guard);
    }
    

    void roundOverState(const Core::CPUThreadGuard& guard) {
        std::time_t unix_time = std::time(nullptr);
        (*writer)["EndTime"] = std::asctime(std::localtime(&unix_time));

        for (u32 player = 0; player < *rPlayerCount->getValue(); ++player) {
            json playerSummary;
            std::string playerId = "Player" + std::to_string(player);

            for (u32 hole = 0; hole <= *rHoleIndex->getValue(); ++hole) {
                json holeSummary;
                std::string holeId = "Hole" + std::to_string(hole);

                auto total_value = rFinalScoreHoleTotal->at({player, hole}).getValue();
                auto strokes_value = rFinalScoreHoleStrokes->at({player, hole}).getValue();
                auto putts_value = rFinalScoreHolePutts->at({player, hole}).getValue();

                logger << fmt::format("\nFINAL_SUMMARY | player={}, hole={}, address={:x}, total_value={}\n", playerId, holeId, rFinalScoreHoleTotal->at({player, hole}).getAddress(), total_value ? std::to_string(*total_value) : "null");
                logger << fmt::format("FINAL_SUMMARY | player={}, hole={}, address={:x}, strokes_value={}\n", playerId, holeId, rFinalScoreHoleStrokes->at({player, hole}).getAddress(), strokes_value ? std::to_string(*strokes_value) : "null");
                logger << fmt::format("FINAL_SUMMARY | player={}, hole={}, address={:x}, putts_value={}\n", playerId, holeId, rFinalScoreHolePutts->at({player, hole}).getAddress(), putts_value ? std::to_string(*putts_value) : "null");

                holeSummary["Score"] = total_value ? std::to_string(*total_value) : "null";
                holeSummary["Strokes"] = strokes_value ? std::to_string(*strokes_value) : "null";
                holeSummary["Putts"] = putts_value ? std::to_string(*putts_value) : "null";

                playerSummary[holeId] = holeSummary;
            }

            (*summary)[playerId] = playerSummary;
        }
        // Piece together hole and shot maps
        assembleFinalJson(writer);

        // Write the JSON file
        writeJSON();

        transitionTo(MGTT_State::RESET);
    }

    void resetState(const Core::CPUThreadGuard& guard) {
        // Reset the writer
        resetWriter();
        resetMemoryTrackers();
        gameID = std::nullopt;
        logger << "\n\n ==== NEW GAME ====\n\n";
        transitionTo(MGTT_State::INIT);
    }
    
    void undefinedState(const Core::CPUThreadGuard& guard) {

    }
};
