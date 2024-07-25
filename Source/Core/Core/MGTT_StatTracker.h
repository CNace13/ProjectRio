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
#include "Core/HW/Memmap.h"

#include "Common/HttpRequest.h"

#include "Common/FileSearch.h"
#include "Common/FileUtil.h"

#include "Core/LocalPlayers.h"
#include "Core/Logger.h"
#include "Core/TrackerAdr.h"
#include "Core/MemoryTracker.h"
#include "Core/MemoryTrackerCriteria.h"
#include "Common/TagSet.h"
#include "Core/JsonWriter.h"

#include <iostream>
#include <functional>
#include <unordered_map>

namespace Tag {
class TagSet;
}

enum class MGTT_State
{
  INIT,
  MENU,           //Wait for IsGolfRound==1
  ROUND_INFO,     //Collect info related to round
  PRESWING,       //Wait for swing to start
  POSTSWING,      //Collect Lie and such
  TRANSITION,     //Wait for next swing - inbetween holes or something
  ROUND_OVER,
  UNDEFINED
};

// Consts
// Stage times
static const u32 cStage_0 = 0;
static const u32 cStage_1 = 0;

// Memory Addresses
static const u32 aIsGolfRound = 0x80162B5F;
constexpr auto criteria_1 = std::make_pair(1, eq(0));
constexpr auto criteria_0 = std::make_pair(0, eq(1));
MemoryTracker<u8, 2, decltype(criteria_0), decltype(criteria_1)> rIsGolfRound(aIsGolfRound, criteria_0, criteria_1);


static const u32 aPlayerPorts = 0x804E6674;
MemoryTracker<u32, 1> rPlayerPorts(aPlayerPorts); //Array of bytes

static const u32 aPlayerTurn = 0x804E68FB; 
MemoryTracker<u8, 1> rPlayerTurn(aPlayerTurn);


// Round Addrs

static const u32 aGameMode = 0x804E6753;
//Only run tracker for game modes 0,1,2 (Stroke play, match play, skins)
constexpr auto game_mode_criteria_0 = std::make_pair(0, or_(or_(eq(0), eq(1)), eq(2)));
MemoryTracker<u8, 16, decltype(game_mode_criteria_0)> rGameMode(aGameMode, game_mode_criteria_0);

static const u32 aCourseId = 0x8044afdf;
MemoryTracker<u8, 16> rCourseId(aCourseId);

static const u32 aRoundFormat = 0x8044afe7;
constexpr auto round_format_criteria_1 = std::make_pair(1, not_(eq(5)));
constexpr auto round_format_criteria_0 = std::make_pair(0, eq(5));
MemoryTracker<u8, 16, decltype(round_format_criteria_0), decltype(round_format_criteria_1)> rRoundFormat(aRoundFormat, round_format_criteria_0, round_format_criteria_1);

static const u32 aGreenType = 0x8044bd9b;
MemoryTracker<u8, 16> rGreenType(aGreenType);

static const u32 aTees = 0x8044afe3;
MemoryTracker<u8, 16> rTees(aTees);

static const u32 aPlayerCount = 0x804E68FA;
constexpr auto player_count_criteria_1 = std::make_pair(1, eq(0));
constexpr auto player_count_criteria_0 = std::make_pair(0, or_(or_(eq(1), eq(2)), or_(eq(4), eq(3))));
MemoryTracker<u8, 2, decltype(criteria_0), decltype(criteria_1)> rPlayerCount(aPlayerCount, criteria_0, criteria_1);

static const u32 aStarredAtMenu_P1 = 0x8044BA68;
MemoryTrackerArray<u32, 16, 4> rStarredAtMenu(aStarredAtMenu_P1, 0x4);

//Handicaps
static const u32 aHandicapsEnabled_P1 = 0x8044C7A8;
MemoryTrackerArray<u32, 16, 4> rHandicapsEnabled(aHandicapsEnabled_P1, 0x4);

static const u32 aSimulationLine_P1 = 0x8044C6B4;
MemoryTrackerArray<u32, 16, 4> rSimulationLine(aSimulationLine_P1, 0x4);

static const u32 aMulligans_P1 = 0x8044C6C4;
MemoryTrackerArray<u32, 16, 4> rMulligans(aMulligans_P1, 0x4);

static const u32 aHandicapTees_P1 = 0x8044C6D4;
MemoryTrackerArray<u32, 16, 4> rHandicapTees(aHandicapTees_P1, 0x4);

// Chars - InGame
static const u32 aCharIdMenu_P1 = 0x8044BA48;
MemoryTrackerArray<u32, 16, 4> rCharId(aCharIdMenu_P1, 0x4);
static const u32 aHandednessMenu_P1 = 0x8044BA90;
MemoryTrackerArray<u32, 16, 4> rHandedness(aHandednessMenu_P1, 0x4);

// static const u32 aHandedness_P1 = 0x804F205E; // (P2=804F7262)
// MemoryTrackerArray<u8, 1, 4> rHandedness(aHandedness_P1, 0x5204);

// static const u32 aCharId_P1 = 0x804F205F; 
// MemoryTrackerArray<u8, 1, 4> rCharId(aCharId_P1, 0x5204);

// Clubs - InGame
static const u32 aWoodsMenu_P1 = 0x8017602C;
MemoryTrackerArray<u8, 1, 4> rWoods(aWoodsMenu_P1, 0x4);

static const u32 aIronsMenu_P1 = aIronsMenu_P1;
MemoryTrackerArray<u8, 1, 4> rIrons(aIronsMenu_P1, 0x4);

static const u32 aWedgesMenu_P1 = 0x8017602E;
MemoryTrackerArray<u8, 1, 4> rWedges(aWedgesMenu_P1, 0x4);

// Hole Addrs
static const u32 aCurrentHole = 0x804E674B;
constexpr auto prev_hole = std::make_pair(1, lt(19));
constexpr auto curr_hole = std::make_pair(0, neq_stage<u8>(1));
MemoryTracker<u8, 2, decltype(prev_hole), decltype(curr_hole)> rCurrentHole(aCurrentHole, prev_hole, curr_hole);

static const u32 aWindRads = 0x804E6850;
MemoryTracker<float, 1> rWindRads(aWindRads);

static const u32 aWindSpeed = 0x804E6854;
MemoryTracker<float, 1> rWindSpeed(aWindSpeed);

static const u32 aRainBool = 0x804E6858;
MemoryTracker<u8, 1> rRainBool(aRainBool);

static const u32 aPin = 0x804E6714;
MemoryTracker<u8, 1> rPin(aPin);

static const u32 aPin2 = 0x804E66AC;
MemoryTracker<u32, 1> rPin2(aPin2);

//Shot Addrs
static const u32 aShotPhase_P1 = 0x804ECD4B;
constexpr auto sp_swing_start_1 = std::make_pair(1, not_(eq(0xB)));
constexpr auto sp_swing_start_0 = std::make_pair(0, eq(0xB));
MemoryTracker<u8, 2, decltype(sp_swing_start_0), decltype(sp_swing_start_1)> rShotPhase(aShotPhase_P1, sp_swing_start_0, sp_swing_start_1);

static const u32 aPlayerShotStatus_P1 = 0x804ECE98;
constexpr auto pss_preswing_criteria_1 = std::make_pair(1, eq(4));
constexpr auto pss_preswing_criteria_0 = std::make_pair(0, eq(4));
MemoryTrackerArray<u32, 2, 4, decltype(pss_preswing_criteria_0), decltype(pss_preswing_criteria_1)> rPlayerShotStatus_preswing(aPlayerShotStatus_P1, 0x5204, pss_preswing_criteria_0, pss_preswing_criteria_1);

constexpr auto pss_swing_criteria_1 = std::make_pair(1, not_(eq(6)));
constexpr auto pss_swing_criteria_0 = std::make_pair(0, eq(6));
MemoryTrackerArray<u32, 2, 4, decltype(pss_swing_criteria_0), decltype(pss_swing_criteria_1)> rPlayerShotStatus_swing(aPlayerShotStatus_P1, 0x5204, pss_swing_criteria_0, pss_swing_criteria_1);

constexpr auto pss_postswing_criteria_1 = std::make_pair(1, not_(eq(8)));
constexpr auto pss_postswing_criteria_0 = std::make_pair(0, eq(8));
MemoryTrackerArray<u32, 2, 4, decltype(pss_postswing_criteria_0), decltype(pss_postswing_criteria_1)> rPlayerShotStatus_postswing(aPlayerShotStatus_P1, 0x5204, pss_postswing_criteria_0, pss_postswing_criteria_1);

static const u32 aShotNum_P1 = 0x806CB4FC;
MemoryTrackerArray<u32, 1, 4> rPlayerShotNum(aShotNum_P1, 0x1c8);

static const u32 aShotType = 0x804ecd50;
MemoryTracker<u32, 1> rShotType(aShotType);

static const u32 aClubType = 0x804ecd58;
MemoryTracker<u32, 1> rClubType(aClubType);

static const u32 aPowerMeterSetting = 0x804e3658;
MemoryTracker<u32, 1> rPowerMeterSetting(aPowerMeterSetting);

static const u32 aPowerMeterSettingCopy = 0x804ecd54;
MemoryTracker<u32, 1> rPowerMeterSettingCopy(aPowerMeterSettingCopy);

static const u32 aPowerMeterMaximum = 0x804ecdd4;
MemoryTracker<u32, 1> rPowerMeterMaximum(aPowerMeterMaximum);

static const u32 aPowerMeterActual = 0x804e6768;
MemoryTracker<u32, 1> rPowerMeterActual(aPowerMeterActual);

static const u32 aPowerMeterActualCopy = 0x804ecd1c;
MemoryTracker<u32, 1> rPowerMeterActualCopy(aPowerMeterActualCopy);

static const u32 aShotAccuracy = 0x80523cac;
MemoryTracker<u32, 1> rShotAccuracy(aShotAccuracy);

static const u32 aShotAccuracy2 = 0x804ECD30; //Confirm
MemoryTracker<u32, 1> rShotAccuracy2(aShotAccuracy2);

static const u32 aManualVsAuto = 0x804ecd88;
MemoryTracker<u32, 1> rManualVsAuto(aManualVsAuto);

static const u32 aAimAngleRadians = 0x804ecd5c;
MemoryTracker<float, 1> rAimAngleRadians(aAimAngleRadians);

static const u32 aImpactPoint_Y_Preshot = 0x804ECDA0;
MemoryTracker<u32, 1> rImpactPoint_Y_Preshot(rImpactPoint_Y_Preshot);

static const u32 aImpactPoint_X_Preshot = 0x804ECDA4;
MemoryTracker<u32, 1> rImpactPoint_X_Preshot(rImpactPoint_X_Preshot);

static const u32 aImpactPoint_X = 0x804ecdac;
MemoryTracker<u32, 1> rImpactPoint_X(rImpactPoint_X);

static const u32 aImpactPoint_Y = 0x804ecda8;
MemoryTracker<u32, 1> rImpactPoint_Y(aImpactPoint_Y);

static const u32 aSpin = 0x804ecd4c;
MemoryTracker<u32, 1> rSpin(aSpin);


//Lie/PostShot
static const u32 aBallPos_X = 0x805DC0B0;
MemoryTracker<float, 1> rBallPos_X(aBallPos_X);

static const u32 aBallPos_Y = 0x805DC0C0;
MemoryTracker<float, 1> rBallPos_Y(aBallPos_Y);

static const u32 aBallPos_Z = 0x805DC0D0;
MemoryTracker<float, 1> rBallPos_Z(aBallPos_Z);

static const u32 aDistanceToHole = 0x802D7368;
MemoryTracker<float, 1> rDistanceToHole(aDistanceToHole);

static const u32 aShotDistance = 0x803A46C4;
MemoryTracker<float, 1> rShotDistance(aShotDistance);

static const u32 aLieType = 0x804e364c;
MemoryTracker<u32, 1> rLieType(aLieType);

static const u32 aLieQuality = 0x804e3650;
MemoryTracker<u32, 1> rLieQuality(aLieQuality);

static const u32 aLieRngRange = 0x804ecdbc;
MemoryTracker<u32, 1> rLieRngRange(aLieRngRange);

static const u32 aLieRng = 0x804ecdc0;
MemoryTracker<u32, 1> rLieRng(aLieRng);

static const u32 aLieRngSeed = 0x804ecd3c;
MemoryTracker<u32, 1> rLieRngSeed(aLieRngSeed);


// //Final Score Addrs
constexpr uint32_t PLAYER_OFFSET = 0x1c8;
constexpr uint32_t HOLE_OFFSET = 0x8;
constexpr uint32_t NUM_PLAYERS = 4;
constexpr uint32_t NUM_HOLES = 18;
constexpr std::size_t NUM_STAGES = 1;
static const u32 aFinalScoreHoleTotal = 0x806CB348;
MemoryTrackerNestedArray<u8, 1> rFinalScoreHoleTotal(aFinalScoreHoleTotal, {NUM_PLAYERS, NUM_HOLES}, {PLAYER_OFFSET, HOLE_OFFSET});

static const u32 aFinalScoreHolePutts = 0x806CB349;
MemoryTrackerNestedArray<u8, 1> rFinalScoreHolePutts(aFinalScoreHolePutts, {NUM_PLAYERS, NUM_HOLES}, {PLAYER_OFFSET, HOLE_OFFSET});

static const u32 aFinalScoreHoleStrokes = 0x806CB350;
MemoryTrackerNestedArray<u8, 1> rFinalScoreHoleStrokes(aFinalScoreHoleStrokes, {NUM_PLAYERS, NUM_HOLES}, {PLAYER_OFFSET, HOLE_OFFSET});

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
        stateFunctions[MGTT_State::PRESWING] = [this](const Core::CPUThreadGuard& guard) { this->preswingState(guard); };
        stateFunctions[MGTT_State::POSTSWING] = [this](const Core::CPUThreadGuard& guard) { this->postswingState(guard); };
        stateFunctions[MGTT_State::ROUND_OVER] = [this](const Core::CPUThreadGuard& guard) { this->roundOverState(guard); };
        std::cout << "Init MGTT_StatTracker" << std::endl;
    }

    // Method to transition to a new MGTT_State
    void transitionTo(MGTT_State newState) {
        if (stateFunctions.find(newState) != stateFunctions.end()) {
            std::cout << "Transitioning states: " << stateToString(currentState) << " -> " << stateToString(newState) << "\n";
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
    void setTagSet(Tag::TagSet tag_set, bool netplay) {
        rioInfo.tag_set_id_name = std::make_pair(tag_set.id, tag_set.name);
    }
    void clearTagSet(bool netplay) {
        rioInfo.tag_set_id_name = std::nullopt;
    }
    void readLocalPlayers(int num_players){ //TODO - Assumes all human players, each with own port
        for (int i=1; i<num_players+1; ++i){
            switch (i)
            {
            case 0:
                rioInfo.rioUsers[i] = LocalPlayers::m_local_player_1;
            case 1:
                rioInfo.rioUsers[i] = LocalPlayers::m_local_player_2;
            case 2:
                rioInfo.rioUsers[i] = LocalPlayers::m_local_player_3;
            case 3:
                rioInfo.rioUsers[i] = LocalPlayers::m_local_player_3;
            
            default:
                break;
            }
        }
    }

    // Method to convert MGTT_State to string
    std::string stateToString(MGTT_State state) const {
        switch (state) {
            case MGTT_State::INIT: return "INIT";
            case MGTT_State::MENU: return "MENU";
            case MGTT_State::ROUND_INFO: return "ROUND_INFO";
            case MGTT_State::PRESWING: return "PRESWING";
            case MGTT_State::POSTSWING: return "POSTSWING";
            case MGTT_State::TRANSITION: return "TRANSITION";
            case MGTT_State::ROUND_OVER: return "ROUND_OVER";
            case MGTT_State::UNDEFINED: return "UNDEFINED";
            default: return "INVALID";
        }
    }

private:
    MGTT_State currentState;
    std::unordered_map<MGTT_State, StateFunction> stateFunctions;
    int frame = 0;

    std::unique_ptr<JSONWriter> writer;

    void initializeWriter(std::string filename) {
        std::string outputFilename = fmt::format("{}{}.json", File::GetUserPath(D_MSSBFILES_IDX), filename);
        std::cout << "JSON File Loc=" << outputFilename << "\n";
        writer = std::make_unique<JSONWriter>(outputFilename);
    }

    struct RioInfo {
        std::map<int, LocalPlayers::LocalPlayers::Player> rioUsers;
        std::optional<std::pair<int, std::string>> tag_set_id_name;
        bool netplay;
    } rioInfo;

    // MGTT_State methods
    void initState(const Core::CPUThreadGuard& guard) {
        // std::cout << "running INIT MGTT_State." << std::endl;
        // Transition logic for INIT MGTT_State
        transitionTo(MGTT_State::MENU);
    }

    void menuState(const Core::CPUThreadGuard& guard) {
        if (!rPlayerCount.isActive()) { 
            rGameMode.run(guard);
            rCourseId.run(guard);
            rRoundFormat.run(guard);
            rGreenType.run(guard);
            rTees.run(guard);
            rPlayerCount.run(guard);
            rPlayerPorts.run(guard);
            //Per player, Menu (Stage15)
            rStarredAtMenu.run(guard);
            rHandicapsEnabled.run(guard);
            rSimulationLine.run(guard);
            rMulligans.run(guard);
            rHandicapTees.run(guard);

            //Per Player, InGame (Stage0)
            rCharId.run(guard);
            rHandedness.run(guard);
            rWoods.run(guard);
            rIrons.run(guard);
            rWedges.run(guard);

            if (rPlayerCount.isActive() && rGameMode.isActive()) { 
                u32 gameId = RioUtil::genRand32();
                initializeWriter(std::to_string(gameId));
                std::cout << "RoundInfo | GameID=" <<  std::to_string(gameId) << "\n";
                std::cout << "RoundInfo | TagSetID=" << (rioInfo.tag_set_id_name.has_value() ? std::to_string((*rioInfo.tag_set_id_name).first) : "null") << "\n";
                std::cout << "RoundInfo | TagSet=" << (rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null") << "\n";
                std::cout << "RoundInfo | GameMode=" <<  std::to_string(*rGameMode.getValue(14)) << "\n";
                std::cout << "RoundInfo | CourseID=" <<  std::to_string(*rCourseId.getValue(14)) << "\n";
                std::cout << "RoundInfo | RoundFormat=" << std::to_string(*rRoundFormat.getValue(14)) << "\n";
                std::cout << "RoundInfo | GreenType=" << std::to_string(*rGreenType.getValue(14)) << "\n";
                std::cout << "RoundInfo | Tees=" << std::to_string(*rTees.getValue(14)) << "\n";
                std::cout << "RoundInfo | PlayerCount=" << std::to_string(*rPlayerCount.getValue()) << "\n";

                writer->startObject("Round Info");
                writer->writeKeyValue("GameId", gameId);
                writer->writeKeyValue("TagSetID", (rioInfo.tag_set_id_name.has_value() ? std::to_string((*rioInfo.tag_set_id_name).first) : "null"));
                writer->writeKeyValue("TagSet", (rioInfo.tag_set_id_name.has_value() ? (*rioInfo.tag_set_id_name).second : "null"));
                writer->writeKeyValue("GameMode", *rGameMode.getValue(14));
                writer->writeKeyValue("CourseID", *rCourseId.getValue(14));
                writer->writeKeyValue("RoundFormat", *rRoundFormat.getValue(14));
                writer->writeKeyValue("GreenType", *rGreenType.getValue(14));
                writer->writeKeyValue("Tees", *rTees.getValue(14));
                writer->writeKeyValue("PlayerCount", *rPlayerCount.getValue());

                // Get User info
                if (!rioInfo.netplay) {
                    readLocalPlayers(*rPlayerCount.getValue());
                }

                // Process users
                writer->startArray("Golfers");
                for (int i=0; i < *rPlayerCount.getValue(); ++i){
                    auto player_port = rPlayerPorts.getByteValue(0, i);

                    //Get Rio Name
                    std::cout << "P" << std::to_string(i) << " Port=" << std::to_string(*player_port) << "\n";
                    std::cout << "RoundInfo | Player=" << std::to_string(i) << "\n";
                    std::cout << "RoundInfo | Port=" << std::to_string(*player_port) << "\n";
                    std::cout << "RoundInfo | RioUsername=" << rioInfo.rioUsers[*player_port].GetUsername() << "\n";

                    std::cout << "RoundInfo | HandicapsEnabled=" << std::to_string(*rHandicapsEnabled[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | SimulationLine=" << std::to_string(*rSimulationLine[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | Mulligans=" << std::to_string(*rMulligans[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | HandicapTees=" << std::to_string(*rHandicapTees[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | CharId=" << std::to_string(*rCharId[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | Starred=" << std::to_string(*rStarredAtMenu[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | Handedness=" << std::to_string(*rHandedness[i].getValue(14)) << "\n";
                    std::cout << "RoundInfo | Woods=" << std::to_string(*rWoods[i].getValue()) << "\n";
                    std::cout << "RoundInfo | Irons=" << std::to_string(*rIrons[i].getValue()) << "\n";
                    std::cout << "RoundInfo | Wedges=" << std::to_string(*rWedges[i].getValue()) << "\n";
                    
                    
                    writer->startObject(fmt::format("P{}", i));
                    writer->writeKeyValue("Port", *player_port);
                    writer->writeKeyValue("RioUsername", rioInfo.rioUsers[*player_port].GetUsername());

                    writer->writeKeyValue("HandicapsEnabled", *rHandicapsEnabled[i].getValue(14));
                    writer->writeKeyValue("SimulationLine", *rSimulationLine[i].getValue(14));
                    writer->writeKeyValue("Mulligans", *rMulligans[i].getValue(14));
                    writer->writeKeyValue("HandicapTees", *rHandicapTees[i].getValue(14));
                    writer->writeKeyValue("CharId", *rCharId[i].getValue(14));
                    writer->writeKeyValue("Starred", *rStarredAtMenu[i].getValue(14));
                    writer->writeKeyValue("Handedness", *rHandedness[i].getValue(14));
                    writer->writeKeyValue("Woods", *rWoods[i].getValue());
                    writer->writeKeyValue("Irons", *rIrons[i].getValue());
                    writer->writeKeyValue("Wedges", *rWedges[i].getValue());
                    writer->endElement();
                }
                writer->endElement();
            }
        }

        rIsGolfRound.run(guard);
        if (rIsGolfRound.isActive() && rGameMode.isActive()) {
            rCurrentHole.run(guard);
            rWindRads.run(guard);
            rWindSpeed.run(guard);
            rRainBool.run(guard);
            rPin.run(guard);
            rPin2.run(guard);

            writer->startArray("Holes");
            std::cout << "  HoleInfo | CurrentHole=" << std::to_string(*rCurrentHole.getValue()) << "\n";
            std::cout << "  HoleInfo | WindRads=" << std::to_string(*rWindRads.getValue()) << "\n";
            std::cout << "  HoleInfo | WindSpeed=" << std::to_string(*rWindSpeed.getValue()) << "\n";
            std::cout << "  HoleInfo | RainBool=" << std::to_string(*rRainBool.getValue()) << "\n";
            std::cout << "  HoleInfo | Pin=" << std::to_string(*rPin.getValue()) << "\n";
            std::cout << "  HoleInfo | Pin2=" << std::to_string(*rPin2.getValue()) << "\n";


            writer->startObject(fmt::format("H{}", *rCurrentHole.getValue()));
            writer->writeKeyValue("CurrentHole", *rCurrentHole.getValue());
            writer->writeKeyValue("HoleIndex", "???");
            writer->writeKeyValue("WindRads", *rWindRads.getValue());
            writer->writeKeyValue("WindSpeed", *rWindSpeed.getValue());
            writer->writeKeyValue("RainBool", *rRainBool.getValue());
            writer->writeKeyValue("Pin", *rPin.getValue());
            writer->writeKeyValue("Pin2", *rPin2.getValue());
            writer->startArray("Shots");
            
            transitionTo(MGTT_State::TRANSITION);
        }
    }

    // void waitingForRoundStartState(const Core::CPUThreadGuard& guard) {

    //     rIsGolfRound.run(guard);
    //     if (rIsGolfRound.isActive()) {
    //         transitionTo(MGTT_State::TRANSITION);

    //         rCurrentHole.run(guard);
    //         rWindRads.run(guard);
    //         rWindSpeed.run(guard);
    //         rRainBool.run(guard);
    //         rPin.run(guard);
    //         rPin2.run(guard);
    //         std::cout << "  HoleInfo | CurrentHole=" << std::to_string(*rCurrentHole.getValue()) << "\n";
    //         std::cout << "  HoleInfo | WindRads=" << std::to_string(*rWindRads.getValue()) << "\n";
    //         std::cout << "  HoleInfo | WindSpeed=" << std::to_string(*rWindSpeed.getValue()) << "\n";
    //         std::cout << "  HoleInfo | RainBool=" << std::to_string(*rRainBool.getValue()) << "\n";
    //         std::cout << "  HoleInfo | Pin=" << std::to_string(*rPin.getValue()) << "\n";
    //         std::cout << "  HoleInfo | Pin2=" << std::to_string(*rPin2.getValue()) << "\n";
    //     }
    // }

    void transitionState(const Core::CPUThreadGuard& guard) {
        // Transition logic for ROUND_INFO MGTT_State

        // Check for new hole
        rCurrentHole.run(guard);
        rWindRads.run(guard);
        rWindSpeed.run(guard);
        rRainBool.run(guard);
        rPin.run(guard);
        rPin2.run(guard);

        rShotPhase.run(guard);
        // std::cout << "H " << std::to_string(*rCurrentHole.getValue()) << "-" << std::to_string(*rCurrentHole.getValue(1)) << " T\n";
        // std::cout << "H " << std::to_string(*rShotPhase.getValue()) << "-" << std::to_string(*rShotPhase.getValue(1)) << " T\n";

        if (rCurrentHole.isActive()){
            std::cout << "  HoleInfo | CurrentHole=" << std::to_string(*rCurrentHole.getValue()) << "\n";
            std::cout << "  HoleInfo | WindRads=" << std::to_string(*rWindRads.getValue()) << "\n";
            std::cout << "  HoleInfo | WindSpeed=" << std::to_string(*rWindSpeed.getValue()) << "\n";
            std::cout << "  HoleInfo | RainBool=" << std::to_string(*rRainBool.getValue()) << "\n";
            std::cout << "  HoleInfo | Pin=" << std::to_string(*rPin.getValue()) << "\n";
            std::cout << "  HoleInfo | Pin2=" << std::to_string(*rPin2.getValue()) << "\n";

            writer->endElement(); // End previous ShotArray
            writer->endElement(); // End previous Hole
            writer->startObject(fmt::format("H{}", *rCurrentHole.getValue()));
            writer->writeKeyValue("CurrentHole", *rCurrentHole.getValue());
            writer->writeKeyValue("HoleIndex", "???");
            writer->writeKeyValue("WindRads", *rWindRads.getValue());
            writer->writeKeyValue("WindSpeed", *rWindSpeed.getValue());
            writer->writeKeyValue("RainBool", *rRainBool.getValue());
            writer->writeKeyValue("Pin", *rPin.getValue());
            writer->writeKeyValue("Pin2", *rPin2.getValue());

            writer->startArray("Shots");
        }

        rPlayerTurn.run(guard);
        auto player_turn = rPlayerTurn.getValue();
        // Check for swing start
        if (player_turn && (*player_turn <= 3)) {
            //Is Golfer ready to swing?
            if (rShotPhase.isActive()){
                std::cout << "Golfer P" << std::to_string(*player_turn) << " Started Swing\n";
                transitionTo(MGTT_State::PRESWING);
            }
        }


        // Check if game is over
        rIsGolfRound.run(guard);
        if (*rIsGolfRound.getValue() == 0) {
            transitionTo(MGTT_State::ROUND_OVER);
        }
    }

    void preswingState(const Core::CPUThreadGuard& guard) {
        rPlayerTurn.run(guard);

        auto player_turn = rPlayerTurn.getValue();

        if (player_turn && (*player_turn <= 3)) {
            rPlayerShotStatus_swing[*player_turn].run(guard);
            // Has golfer swung?
            if (rPlayerShotStatus_swing[*player_turn].isActive()){
                std::cout << "Golfer P" << std::to_string(*player_turn) << " Swing Committed\n";

                //Collect swing info
                rPlayerShotNum.run(guard);
                rShotType.run(guard);
                rClubType.run(guard);
                rPowerMeterSetting.run(guard);
                rPowerMeterSettingCopy.run(guard);
                rPowerMeterMaximum.run(guard);
                rPowerMeterActual.run(guard);
                rPowerMeterActualCopy.run(guard);
                rShotAccuracy.run(guard);
                rShotAccuracy2.run(guard);
                rManualVsAuto.run(guard);
                rAimAngleRadians.run(guard);
                rImpactPoint_Y_Preshot.run(guard);
                rImpactPoint_X_Preshot.run(guard);
                rImpactPoint_X.run(guard);
                rImpactPoint_Y.run(guard);
                rSpin.run(guard);
                
                std::cout << "  ShotInfo | ShotInfo=" << std::to_string(*rShotType.getValue()) << "\n";
                std::cout << "  ShotInfo | ClubType=" << std::to_string(*rClubType.getValue()) << "\n";
                std::cout << "  ShotInfo | PowerMeterSetting=" << std::to_string(*rPowerMeterSetting.getValue()) << "\n";
                std::cout << "  ShotInfo | PowerMeterSettingCopy=" << std::to_string(*rPowerMeterSettingCopy.getValue()) << "\n";
                std::cout << "  ShotInfo | PowerMeterMaximum=" << std::to_string(*rPowerMeterMaximum.getValue()) << "\n";
                std::cout << "  ShotInfo | PowerMeterActual=" << std::to_string(*rPowerMeterActual.getValue()) << "\n";
                std::cout << "  ShotInfo | PowerMeterActualCopy=" << std::to_string(*rPowerMeterActualCopy.getValue()) << "\n";
                std::cout << "  ShotInfo | ShotAccuracy=" << std::to_string(*rShotAccuracy.getValue()) << "\n";
                std::cout << "  ShotInfo | ShotAccuracy2=" << std::to_string(*rShotAccuracy2.getValue()) << "\n";
                std::cout << "  ShotInfo | ManualVsAuto=" << std::to_string(*rManualVsAuto.getValue()) << "\n";
                std::cout << "  ShotInfo | AimAngleRadians=" << std::to_string(*rAimAngleRadians.getValue()) << "\n";
                std::cout << "  ShotInfo | ImpactPoint_Y_Preshot=" << std::to_string(*rImpactPoint_Y_Preshot.getValue()) << "\n";
                std::cout << "  ShotInfo | ImpactPoint_X_Preshot=" << std::to_string(*rImpactPoint_X_Preshot.getValue()) << "\n";
                std::cout << "  ShotInfo | ImpactPoint_X=" << std::to_string(*rImpactPoint_X.getValue()) << "\n";
                std::cout << "  ShotInfo | ImpactPoint_Y=" << std::to_string(*rImpactPoint_Y.getValue()) << "\n";
                std::cout << "  ShotInfo | Spin=" << std::to_string(*rSpin.getValue()) << "\n";

                writer->startObject(fmt::format("ShotP{}S{}", *player_turn, *rPlayerShotNum[*player_turn].getValue()));
                writer->writeKeyValue("ShotInfo", *rShotType.getValue());
                writer->writeKeyValue("ClubType", *rClubType.getValue());
                writer->writeKeyValue("PowerMeterSetting", *rPowerMeterSetting.getValue());
                writer->writeKeyValue("PowerMeterSettingCopy", *rPowerMeterSettingCopy.getValue());
                writer->writeKeyValue("PowerMeterMaximum", *rPowerMeterMaximum.getValue());
                writer->writeKeyValue("PowerMeterActual", *rPowerMeterActual.getValue());
                writer->writeKeyValue("PowerMeterActualCopy", *rPowerMeterActualCopy.getValue());
                writer->writeKeyValue("ShotAccuracy", *rShotAccuracy.getValue());
                writer->writeKeyValue("ShotAccuracy2", *rShotAccuracy2.getValue());
                writer->writeKeyValue("ManualVsAuto", *rManualVsAuto.getValue());
                writer->writeKeyValue("AimAngleRadians", *rAimAngleRadians.getValue());
                writer->writeKeyValue("ImpactPoint_Y_Preshot", *rImpactPoint_Y_Preshot.getValue());
                writer->writeKeyValue("ImpactPoint_X_Preshot", *rImpactPoint_X_Preshot.getValue());
                writer->writeKeyValue("ImpactPoint_X", *rImpactPoint_X.getValue());
                writer->writeKeyValue("ImpactPoint_Y", *rImpactPoint_Y.getValue());
                writer->writeKeyValue("Spin", *rSpin.getValue());

                transitionTo(MGTT_State::POSTSWING);
            }
        }


        // Check if game is over
        rIsGolfRound.run(guard);
        if (*rIsGolfRound.getValue() == 0) {
            transitionTo(MGTT_State::ROUND_OVER);
        }
    }

    void postswingState(const Core::CPUThreadGuard& guard) {
        rPlayerTurn.run(guard);

        auto player_turn = rPlayerTurn.getValue();

        if (player_turn && (*player_turn <= 3)) {
            rPlayerShotStatus_postswing[*player_turn].run(guard);
            
            if (rPlayerShotStatus_postswing[*player_turn].isActive()){
                std::cout << "Golfer P" << std::to_string(*player_turn) << " Swing Done\n";
                
                //Collect lie/result info
                rBallPos_X.run(guard);
                rBallPos_Y.run(guard);
                rBallPos_Z.run(guard);
                rDistanceToHole.run(guard);
                rLieType.run(guard);
                rLieQuality.run(guard);
                rLieRngRange.run(guard);
                rLieRng.run(guard);
                rLieRngSeed.run(guard);
                rShotDistance.run(guard);
                
                std::cout << "  Shot Result | BallPos_X=" << std::to_string(*rBallPos_X.getValue()) << "\n";
                std::cout << "  Shot Result | BallPos_Y=" << std::to_string(*rBallPos_Y.getValue()) << "\n";
                std::cout << "  Shot Result | BallPos_Z=" << std::to_string(*rBallPos_Z.getValue()) << "\n";
                std::cout << "  Shot Result | DistanceToHole=" << std::to_string(*rDistanceToHole.getValue()) << "\n";
                std::cout << "  Shot Result | LieType=" << std::to_string(*rLieType.getValue()) << "\n";
                std::cout << "  Shot Result | LieQuality=" << std::to_string(*rLieQuality.getValue()) << "\n";
                std::cout << "  Shot Result | LieRngRange=" << std::to_string(*rLieRngRange.getValue()) << "\n";
                std::cout << "  Shot Result | LieRng=" << std::to_string(*rLieRng.getValue()) << "\n";
                std::cout << "  Shot Result | LieRngSeed=" << std::to_string(*rLieRngSeed.getValue()) << "\n";
                std::cout << "  Shot Result | rShotDistance=" << std::to_string(*rShotDistance.getValue()) << "\n";

                writer->writeKeyValue("BallPos_X", *rBallPos_X.getValue());
                writer->writeKeyValue("BallPos_Y", *rBallPos_Y.getValue());
                writer->writeKeyValue("BallPos_Z", *rBallPos_Z.getValue());
                writer->writeKeyValue("DistanceToHole", *rDistanceToHole.getValue());
                writer->writeKeyValue("LieType", *rLieType.getValue());
                writer->writeKeyValue("LieQuality", *rLieQuality.getValue());
                writer->writeKeyValue("LieRngRange", *rLieRngRange.getValue());
                writer->writeKeyValue("LieRng", *rLieRng.getValue());
                writer->writeKeyValue("LieRngSeed", *rLieRngSeed.getValue());
                writer->writeKeyValue("rShotDistance", *rShotDistance.getValue());
                writer->endElement(); //End Shot
                transitionTo(MGTT_State::TRANSITION);
            }
        }
    }


    void roundOverState(const Core::CPUThreadGuard& guard) {

        // Write the JSON file
        writer->endJSON();

        // Reset the writer
        writer.reset();
        
        rFinalScoreHoleTotal.run(guard);
        rFinalScoreHolePutts.run(guard);
        rFinalScoreHoleStrokes.run(guard);

        for (u32 player = 0; player < NUM_PLAYERS; ++player) {
            std::cout << "Player " << player + 1 << ":\n";
            
            std::cout << "  FinalScores | Player=" << player << "\n";
            for (u32 hole = 0; hole < NUM_HOLES; ++hole) {            
                auto total_value = rFinalScoreHoleTotal.at({player, hole}).getValue();
                auto putts_value = rFinalScoreHolePutts.at({player, hole}).getValue();
                auto strokes_value = rFinalScoreHoleStrokes.at({player, hole}).getValue();
                std::cout << "    FinalScores | Hole=" << hole << "\n";
                std::cout << "      FinalScores | Score=" << (total_value ? std::to_string(*total_value) : "null") << "\n";
                std::cout << "      FinalScores | Strokes=" << (total_value ? std::to_string(*strokes_value) : "null") << "\n";
                std::cout << "      FinalScores | Putts=" << (total_value ? std::to_string(*putts_value) : "null") << "\n";
            }
        }
        writer->endJSON();

        // Reset the writer
        writer.reset();

    }

    /*
    void ballInMotionState(const Core::CPUThreadGuard& guard) {
        // Transition logic for PRESWING MGTT_State
        // If shot status is 6 (I think) shot details are locked in

        rPlayerTurn.run(guard);

        auto player_turn = rPlayerTurn.getValue();

        if (player_turn && player_turn.value() <= 3) {
            //Read the player shot status of each player to find the "active" player
            rPlayerShotStatus[player_turn.value()].run(guard);

            std::cout << "P" << std::to_string(player_turn.value()) << " ShotStatus=" << std::to_string(rPlayerShotStatus[player_turn.value()].getValue().value()) << "\n";

            //If player is ready to swing, transition to preswing()
            if (rPlayerShotStatus[player_turn.value()].getValue().value() == 8) { //Ready to swing
                transitionTo(MGTT_State::SWING);
            }
        }

        // Read lie values

        // Transition
        transitionTo(MGTT_State::TRANSITION);
    }
    */

};

/*

    union
    {
        u32 num;
        float fnum;
    } float_converter;

    // void setTagSetId(Tag::TagSet tag_set, bool netplay);
    // void clearTagSetId(bool netplay);
    // void setNetplaySession(bool netplay_session, std::string opponent_name = "");
    // void setAvgPing(int avgPing);
    // void setLagSpikes(int nLagSpikes);
    // void setNetplayerUserInfo(std::map<int, LocalPlayers::LocalPlayers::Player> userInfo);
    // void setGameID(u32 gameID);
    // void setTags(std::vector tags);
    // void setTagSet(int tagset);

    void Run(const Core::CPUThreadGuard& guard);
    void lookForTriggerEvents(const Core::CPUThreadGuard& guard);

    void logGameInfo(const Core::CPUThreadGuard& guard);

    //Quit function
    void onGameQuit(const Core::CPUThreadGuard& guard);
    bool shouldSubmitGame();

    //Read players from ini file and assign to team
    void readPlayerNames(bool local_game);
    //void setDefaultNames(bool local_game);

    float floatConverter(u32 in_value) {
        float out_float;
        float_converter.num = in_value;
        out_float = float_converter.fnum;
        return out_float;
    }

    Common::HttpRequest m_http{std::chrono::minutes{3}};

    //The type of value to decode, the value to be decoded, bool for decode if true or original value if false
    std::string decode(std::string type, u8 value, bool decode);

    //Returns JSON, PathToWriteTo
    std::string getStatJSON(bool inDecode, bool hide_riokey = true);
    std::string getEventJSON(u16 in_event_num, Event& in_event, bool inDecode);
    std::string getHUDJSON(std::string in_event_num, Event& in_curr_event, std::optional<Event> in_prev_event, bool inDecode);
    //Returns path to save json
    std::string getStatJsonPath(std::string prefix);

    void postOngoingGame(Event& in_event);
    void updateOngoingGame(Event& in_event);

    std::pair<u8,u8> getBatterFielderPorts(const Core::CPUThreadGuard& guard){
        // These values are the actual port numbers
        // and are indexed into using the below u8s
        std::array<u8, 2> ports = {PowerPC::MMU::HostRead_U8(guard, 0x800e874c), PowerPC::MMU::HostRead_U8(guard, 0x800e874d)};

        // These registers will always be 0 or 1
        // and swap values each half inning
        u32 BattingTeam = PowerPC::MMU::HostRead_U32(guard, 0x80892990);
        u32 PitchingTeam = PowerPC::MMU::HostRead_U32(guard, 0x80892994);
        
        u8 BattingPort = ports[BattingTeam];
        u8 FieldingPort = ports[PitchingTeam];

        return std::make_pair(BattingPort, FieldingPort);
    }
    void initPlayerInfo(const Core::CPUThreadGuard& guard);
    void initCaptains(const Core::CPUThreadGuard& guard);

    //If mid-game, dump game
    void dumpGame(const Core::CPUThreadGuard& guard){
        if (m_game_state == GAME_STATE::INGAME){
            m_game_info.quitter_team = 2;
            logGameInfo(guard);

            //Remove current event, wasn't finished
            auto it = m_game_info.events.find(m_game_info.event_num);
            if ((&it != NULL) && (it != m_game_info.events.end()))
            {
              m_game_info.events.erase(it);
            }

            //Game has ended. Write file but do not submit
            std::string jsonPath = getStatJsonPath("crash.decode.");
            std::string json = getStatJSON(true);
            
            File::WriteStringToFile(jsonPath, json);

            jsonPath = getStatJsonPath("crash.");
            json = getStatJSON(false, true);
            
            File::WriteStringToFile(jsonPath, json);
            init();
        }
    }
};
*/