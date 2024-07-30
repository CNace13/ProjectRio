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
#include "Core/JsonWriter.h"

#include <iostream>
#include <functional>
#include <unordered_map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

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
constexpr auto player_count_criteria_1 = std::make_pair(1, eq(0));
constexpr auto player_count_criteria_0 = std::make_pair(0, or_(or_(eq(1), eq(2)), or_(eq(4), eq(3))));

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
static const u32 aIronsMenu_P1 = aIronsMenu_P1;
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

static const u32 aPlayerShotStatus_P1 = 0x804ECE98;
constexpr auto pss_preswing_criteria_1 = std::make_pair(1, eq(4));
constexpr auto pss_preswing_criteria_0 = std::make_pair(0, eq(4));

constexpr auto pss_swing_criteria_1 = std::make_pair(1, not_(eq(6)));
constexpr auto pss_swing_criteria_0 = std::make_pair(0, eq(6));

constexpr auto pss_postswing_criteria_1 = std::make_pair(1, not_(eq(8)));
constexpr auto pss_postswing_criteria_0 = std::make_pair(0, eq(8));

constexpr auto pss_posthole_criteria_0 = std::make_pair(0, or_(gt(8), lt(4)));

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
static const u32 aPowerShotCount_P1 = 0x804ECE7C;
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
            logger << "Transitioning states: " << stateToString(currentState) << " -> " << stateToString(newState) << "\n";
            currentState = newState;
        } else {
            std::cerr << "Invalid MGTT_State transition!" << std::endl;
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

    void setNetplayerUserInfo(std::map<int, LocalPlayers::LocalPlayers::Player> userInfo){
        rioInfo.rioUsers = userInfo;
        // rioInfo.netplay = true;
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
    void readLocalPlayers(int num_players, bool netplay);

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
            case MGTT_State::UNDEFINED: return "UNDEFINED";
            default: return "INVALID";
        }
    }

private:
    MGTT_State currentState;
    std::unordered_map<MGTT_State, StateFunction> stateFunctions;
    int frame = 0;

    // Memory Trackers
    std::unique_ptr<MemoryTracker<u8, 2, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>> rIsGolfRound;
    std::unique_ptr<MemoryTracker<u32, 1>> rPlayerPorts; //Array of bytes
    std::unique_ptr<MemoryTracker<u8, 1>> rPlayerTurn;
    std::unique_ptr<MemoryTracker<u8, 16, decltype(game_mode_criteria_0)>> rGameMode;
    std::unique_ptr<MemoryTracker<u8, 16, decltype(return_to_menu_1), decltype(return_to_menu_0)>> rMenuScene;
    std::unique_ptr<MemoryTracker<u8, 16>> rCourseId;
    std::unique_ptr<MemoryTracker<u8, 16, decltype(round_format_criteria_0), decltype(round_format_criteria_1)>> rRoundFormat;
    std::unique_ptr<MemoryTracker<u8, 16>> rGreenType;
    std::unique_ptr<MemoryTracker<u8, 16>> rTees;
    std::unique_ptr<MemoryTracker<u8, 2, decltype(bool_hi_criteria_0), decltype(bool_hi_criteria_1)>> rPlayerCount;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rStarredAtMenu;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rHandicapsEnabled;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rSimulationLine;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rMulligans;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rHandicapTees;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rCharId;
    std::unique_ptr<MemoryTrackerArray<u32, 16, 4>> rHandedness;
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
    std::unique_ptr<MemoryTracker<u8, 2, decltype(sp_swing_start_0), decltype(sp_swing_start_1)>> rShotPhase;
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

    const std::vector<uint32_t> sizes = {NUM_PLAYERS, NUM_HOLES};
    const std::vector<uint32_t> offsets = {PLAYER_OFFSET, HOLE_OFFSET};

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
        return fmt::format("H{}_{}", holeNumber, holeIndex);
    }

    template<typename T, typename U>
    std::string getShotName(T playerNum, U shotNum){
        return fmt::format("S{}_{}", playerNum, shotNum);
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
    } rioInfo;

    // State Machine
    void initState(const Core::CPUThreadGuard& guard) {
        initMemoryTrackers();
        transitionTo(MGTT_State::MENU);
    }

    void menuState(const Core::CPUThreadGuard& guard) {
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
                u32 gameId = RioUtil::genRand32();
                initWriter(std::to_string(gameId));

                //Read start time
                std::time_t unix_time = std::time(nullptr);
                std::asctime(std::localtime(&unix_time));

                std::string tagSet = rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null";
                logger << fmt::format("ROUND_INFO | GameID={}, GameMode={}, TagSet={}, CourseID={}, RoundFormat={}, PlayerCount={}\n", gameId, *rGameMode->getValue(14), tagSet, 
                                      *rCourseId->getValue(14), *rRoundFormat->getValue(14), *rPlayerCount->getValue());

                // logger << "RoundInfo | GameID=" <<  std::to_string(gameId) << "\n";
                // logger << "RoundInfo | Start time=" <<  std::asctime(std::localtime(&unix_time)) << "\n";
                // logger << "RoundInfo | TagSetID=" << (rioInfo.tag_set_id_name.has_value() ? std::to_string((*rioInfo.tag_set_id_name).first) : "null") << "\n";
                // logger << "RoundInfo | TagSet=" << (rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null") << "\n";
                // logger << "RoundInfo | GameMode=" <<  std::to_string(*rGameMode->getValue(14)) << "\n";
                // logger << "RoundInfo | CourseID=" <<  std::to_string(*rCourseId->getValue(14)) << "\n";
                // logger << "RoundInfo | RoundFormat=" << std::to_string(*rRoundFormat->getValue(14)) << "\n";
                // logger << "RoundInfo | GreenType=" << std::to_string(*rGreenType->getValue(14)) << "\n";
                // logger << "RoundInfo | Tees=" << std::to_string(*rTees->getValue(14)) << "\n";
                // logger << "RoundInfo | PlayerCount=" << std::to_string(*rPlayerCount->getValue()) << "\n";
                (*writer)["GameId"] = gameId;
                (*writer)["StartTime"] = std::asctime(std::localtime(&unix_time));
                (*writer)["TagSetID"] = rioInfo.tag_set_id_name.has_value() ? std::to_string((*rioInfo.tag_set_id_name).first) : "null";
                (*writer)["TagSet"] = rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null";
                (*writer)["GameMode"] = *rGameMode->getValue(14);
                (*writer)["CourseID"] = *rCourseId->getValue(14);
                (*writer)["RoundFormat"] = *rRoundFormat->getValue(14);
                (*writer)["GreenType"] = *rGreenType->getValue(14);
                (*writer)["Tees"] = *rTees->getValue(14);
                (*writer)["PlayerCount"] = *rPlayerCount->getValue();

                // Get User info
                readLocalPlayers(*rPlayerCount->getValue(), rioInfo.netplay);

                // Process users
                std::vector<json> golfers;
                for (int i=0; i < *rPlayerCount->getValue(); ++i){
                    auto player_port = rPlayerPorts->getByteValue(0, i);

                    json golfer;

                    //Get Rio Name
                    logger << fmt::format("PLAYER_INFO | Player={}, Port={}, Username={}\n", std::to_string(i), std::to_string(*player_port), rioInfo.rioUsers[i].GetUsername());

                    // logger << "RoundInfo | HandicapsEnabled=" << std::to_string(*(*rHandicapsEnabled)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | SimulationLine=" << std::to_string(*(*rSimulationLine)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | Mulligans=" << std::to_string(*(*rMulligans)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | HandicapTees=" << std::to_string(*(*rHandicapTees)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | CharId=" << std::to_string(*(*rCharId)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | Starred=" << std::to_string(*(*rStarredAtMenu)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | Handedness=" << std::to_string(*(*rHandedness)[i].getValue(14)) << "\n";
                    // logger << "RoundInfo | Woods=" << std::to_string(*(*rWoods)[i].getValue()) << "\n";
                    // logger << "RoundInfo | Irons=" << std::to_string(*(*rIrons)[i].getValue()) << "\n";
                    // logger << "RoundInfo | Wedges=" << std::to_string(*(*rWedges)[i].getValue()) << "\n";
                    
                    golfer["Port"] = *player_port;
                    golfer["RioUsername"] = rioInfo.rioUsers[i].GetUsername();

                    golfer["HandicapsEnabled"] = (*(*rHandicapsEnabled)[i].getValue(14));
                    golfer["SimulationLine"] = (*(*rSimulationLine)[i].getValue(14));
                    golfer["Mulligans"] = (*(*rMulligans)[i].getValue(14));
                    golfer["HandicapTees"] = (*(*rHandicapTees)[i].getValue(14));
                    golfer["CharId"] = (*(*rCharId)[i].getValue(14));
                    golfer["Starred"] = (*(*rStarredAtMenu)[i].getValue(14));
                    golfer["Handedness"] = (*(*rHandedness)[i].getValue(14));
                    golfer["Woods"] = (*(*rWoods)[i].getValue());
                    golfer["Irons"] = (*(*rIrons)[i].getValue());
                    golfer["Wedges"] = (*(*rWedges)[i].getValue());

                    golfers.emplace_back(golfer);
                }
                (*writer)["Golfers"] = golfers;
            }
        }

        rIsGolfRound->run(guard);
        if (rIsGolfRound->isActive() && rGameMode->isActive()) {
            transitionTo(MGTT_State::HOLE_INFO);
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

        // logger << "  HoleInfo | CurrentHole=" << std::to_string(*rHoleNum->getValue()) << "\n";
        // logger << "  HoleInfo | CurrentHole=" << std::to_string(*rHoleIndex->getValue()) << "\n";
        // logger << "  HoleInfo | Par=" << std::to_string(*rWindRads->getValue()) << "\n";
        // logger << "  HoleInfo | WindRads=" << std::to_string(*rWindRads->getValue()) << "\n";
        // logger << "  HoleInfo | WindSpeed=" << std::to_string(*rWindSpeed->getValue()) << "\n";
        // logger << "  HoleInfo | RainBool=" << std::to_string(*rRainBool->getValue()) << "\n";
        // logger << "  HoleInfo | Pin=" << std::to_string(*rPin->getValue()) << "\n";
        // logger << "  HoleInfo | Pin2=" << std::to_string(*rPin2->getValue()) << "\n";

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
    }

    void transitionState(const Core::CPUThreadGuard& guard) {
        // Check for new hole
        rHoleIndex->run(guard);
        if (rHoleIndex->isActive()){
            transitionTo(MGTT_State::HOLE_INFO);
        }

        // Check for swing
        rShotPhase->run(guard);
        rPlayerTurn->run(guard);
        auto player_turn = rPlayerTurn->getValue();
        // Check for swing start
        if (player_turn && (*player_turn <= 3)) {
            //Is Golfer ready to swing?
            if (rShotPhase->isActive()){
                transitionTo(MGTT_State::SWING);
            }
        }

        // Check if game is over
        rMenuScene->run(guard);
        if (rMenuScene->isActive()) {
            transitionTo(MGTT_State::ROUND_OVER);
        }
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
                
                // logger << "  ShotInfo | ShotNum=" << std::to_string(*(*rPlayerShotNum)[*player_turn].getValue()) << "\n";
                // logger << "  ShotInfo | ShotInfo=" << std::to_string(*rShotType->getValue()) << "\n";
                // logger << "  ShotInfo | ClubType=" << std::to_string(*rClubType->getValue()) << "\n";
                // logger << "  ShotInfo | PowerMeterSetting=" << std::to_string(*rPowerMeterSetting->getValue()) << "\n";
                // logger << "  ShotInfo | PowerMeterSettingCopy=" << std::to_string(*rPowerMeterSettingCopy->getValue()) << "\n";
                // logger << "  ShotInfo | PowerMeterMaximum=" << std::to_string(*rPowerMeterMaximum->getValue()) << "\n";
                // logger << "  ShotInfo | PowerMeterActual=" << std::to_string(*rPowerMeterActual->getValue()) << "\n";
                // logger << "  ShotInfo | PowerMeterActualCopy=" << std::to_string(*rPowerMeterActualCopy->getValue()) << "\n";
                // logger << "  ShotInfo | ShotAccuracy=" << std::to_string(*rShotAccuracy->getValue()) << "\n";
                // logger << "  ShotInfo | ShotAccuracy2=" << std::to_string(*rShotAccuracy2->getValue()) << "\n";
                // logger << "  ShotInfo | ManualVsAuto=" << std::to_string(*rManualVsAuto->getValue()) << "\n";
                // logger << "  ShotInfo | AimAngleRadians=" << std::to_string(*rAimAngleRadians->getValue()) << "\n";
                // logger << "  ShotInfo | ImpactPoint_Y_Preshot=" << std::to_string(*rImpactPoint_Y_Preshot->getValue()) << "\n";
                // logger << "  ShotInfo | ImpactPoint_X_Preshot=" << std::to_string(*rImpactPoint_X_Preshot->getValue()) << "\n";
                // logger << "  ShotInfo | ImpactPoint_X=" << std::to_string(*rImpactPoint_X->getValue()) << "\n";
                // logger << "  ShotInfo | ImpactPoint_Y=" << std::to_string(*rImpactPoint_Y->getValue()) << "\n";
                // logger << "  ShotInfo | Spin=" << std::to_string(*rSpin->getValue()) << "\n";

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
                
                // logger << "  Shot Result | BallPos_X=" << std::to_string(*rBallPos_X->getValue()) << "\n";
                // logger << "  Shot Result | BallPos_Y=" << std::to_string(*rBallPos_Y->getValue()) << "\n";
                // logger << "  Shot Result | BallPos_Z=" << std::to_string(*rBallPos_Z->getValue()) << "\n";
                // logger << "  Shot Result | DistanceToHole=" << std::to_string(*rDistanceToHole->getValue()) << "\n";
                // logger << "  Shot Result | LieType=" << std::to_string(*rLieType->getValue()) << "\n";
                // logger << "  Shot Result | LieQuality=" << std::to_string(*rLieQuality->getValue()) << "\n";
                // logger << "  Shot Result | LieRngRange=" << std::to_string(*rLieRngRange->getValue()) << "\n";
                // logger << "  Shot Result | LieRng=" << std::to_string(*rLieRng->getValue()) << "\n";
                // logger << "  Shot Result | LieRngSeed=" << std::to_string(*rLieRngSeed->getValue()) << "\n";
                // logger << "  Shot Result | PowerShotCount=" << std::to_string( *(*rPowerShotCount)[*player_turn].getValue()) << "\n";
                // logger << "  Shot Result | rShotDistance=" << std::to_string(*rShotDistance->getValue()) << "\n";

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
                logger << fmt::format("SWING_OVER | Player={}, HoleName={}, ShotStatus={}\n", *player_turn, holeName, *(*rPlayerShotStatus_posthole)[*player_turn].getValue());
                rFinalScoreHoleTotal->run(guard);
                rFinalScoreHolePutts->run(guard);
                rFinalScoreHoleStrokes->run(guard);
                transitionTo(MGTT_State::TRANSITION);
            }
        }
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

                logger << fmt::format("FINAL_SUMMARY | player={}, hole={}, address={:x}, total_value={}\n", playerId, holeId, rFinalScoreHoleTotal->at({player, hole}).getAddress(), total_value ? std::to_string(*total_value) : "null");
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
        logger << "\n\n ==== NEW GAME ====\n\n";
        transitionTo(MGTT_State::INIT);
    }
    
    void undefinedState(const Core::CPUThreadGuard& guard) {

    }
};
