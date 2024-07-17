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
#include "Core/HW/Memmap.h"

#include "Common/HttpRequest.h"

#include "Common/FileSearch.h"
#include "Common/FileUtil.h"

#include "Core/LocalPlayers.h"
#include "Core/Logger.h"
#include "Core/TrackerAdr.h"
#include "Core/MemoryTracker.h"

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
  SWING,          //Collect Power, club, shot info
  BALL_IN_MOTION, //Wait for ball to land
  POSTSWING,      //Collect Lie and such
  TRANSITION,     //Wait for next swing - inbetween holes or something
  LOGGED,
  UNDEFINED
};

// Consts
// Stage times
static const u32 cStage_0 = 0;
static const u32 cStage_1 = 0;

// Memory Addresses
static const u32 aIsGolfRound = 0x80162B5F;
std::map<u32, u8> newly_active_map = {{cStage_0, 1}, {cStage_1, 0}};
MemoryTracker<u8, 2> rIsGolfRound(aIsGolfRound, newly_active_map);

static const u32 aCourseId = 0x8044afdf;
MemoryTracker<u8, 1> rCourseId(aCourseId);

static const u32 aPlayerPorts = 0x804E6674;
MemoryTracker<u32, 1> rPlayerPorts(aPlayerPorts); //Array of bytes

static const u32 aPlayerTurn = 0x804E68FB; 
MemoryTracker<u8, 1> rPlayerTurn(aPlayerTurn);


// Round Addrs
static const u32 aRoundFormat = 0x8044afe7;
MemoryTracker<u8, 1> rRoundFormat(aRoundFormat);

static const u32 aGreenType = 0x8044bd9b;
MemoryTracker<u8, 1> rGreenType(aGreenType);

static const u32 aTees = 0x8044afe3;
MemoryTracker<u8, 1> rTees(aTees);

static const u32 aPlayerCount = 0x804E68FA;
MemoryTracker<u8, 1> rPlayerCount(aPlayerCount);

//Shot Addrs
static const u32 aPlayerShotStatus = 0x804ECE98;
MemoryTrackerArray<u32, 1, 4> rPlayerShotStatus(aPlayerShotStatus, 0x5204);
// std::map<u32, u8> newly_ReadyToSwing = {{cStage_0, 4}, {cStage_1, 0}};
// std::map<u32, u8> newly_active_map = {{cStage_0, 1}, {cStage_1, 0}};
// std::map<u32, u8> newly_active_map = {{cStage_0, 1}, {cStage_1, 0}};
// MemoryTrackerArray<u8, 1, 4> rPlayerShotStatus_ReadyToSwing(aPlayerShotStatus, 0x5204);
// MemoryTrackerArray<u8, 1, 4> rPlayerShotStatus_SwingLockedIn(aPlayerShotStatus, 0x5204);
// MemoryTrackerArray<u8, 1, 4> rPlayerShotStatus_SwingDone(aPlayerShotStatus, 0x5204);

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

static const u32 aImpactPoint_X = 0x804ecdac; // Confirm
MemoryTracker<u32, 1> rImpactPoint_X(rImpactPoint_X);

static const u32 aImpactPoint_Y = 0x804ecda8; // Confirm
MemoryTracker<u32, 1> rImpactPoint_Y(aImpactPoint_Y);

static const u32 aSpin = 0x804ecd4c;
MemoryTracker<u32, 1> rSpin(aSpin);

static const u32 aDistanceToHole = 0x802D7368; //Confirm
MemoryTracker<float, 2> rDistanceToHole(aDistanceToHole);

//Lie/PostShot

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

// Type definition for MGTT_State transition functions
using StateFunction = std::function<void(const Core::CPUThreadGuard&)>;

// MGTT_StatTracker class
class MGTT_StatTracker {
public:
    MGTT_StatTracker() : currentState(MGTT_State::INIT) {
        // MGTT_StatTracker MGTT_State functions map
        stateFunctions[MGTT_State::INIT] = [this](const Core::CPUThreadGuard& guard) { this->initState(guard); };
        stateFunctions[MGTT_State::MENU] = [this](const Core::CPUThreadGuard& guard) { this->menuState(guard); };
        stateFunctions[MGTT_State::ROUND_INFO] = [this](const Core::CPUThreadGuard& guard) { this->roundInfoState(guard); };
        stateFunctions[MGTT_State::TRANSITION] = [this](const Core::CPUThreadGuard& guard) { this->transitionState(guard); };
        stateFunctions[MGTT_State::PRESWING] = [this](const Core::CPUThreadGuard& guard) { this->preswingState(guard); };
        stateFunctions[MGTT_State::SWING] = [this](const Core::CPUThreadGuard& guard) { this->swingState(guard); };
        stateFunctions[MGTT_State::BALL_IN_MOTION] = [this](const Core::CPUThreadGuard& guard) { this->ballInMotionState(guard); };
        std::cout << "Init MGTT_StatTracker" << std::endl;
    }

    // Method to transition to a new MGTT_State
    void transitionTo(MGTT_State newState) {
        if (stateFunctions.find(newState) != stateFunctions.end()) {
            std::cout << " Transitioning states: " << stateToString(currentState) << " -> " << stateToString(newState) << "\n";
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
        if (stateFunctions.find(currentState) != stateFunctions.end()) {
            stateFunctions[currentState](guard);
            std::cout << " Current state: " << stateToString(currentState) << "\n";
        } else {
            std::cerr << "No function defined for the current state!" << std::endl;
        }
    }

    // Method to convert MGTT_State to string
    std::string stateToString(MGTT_State state) const {
        switch (state) {
            case MGTT_State::INIT: return "INIT";
            case MGTT_State::MENU: return "MENU";
            case MGTT_State::ROUND_INFO: return "ROUND_INFO";
            case MGTT_State::PRESWING: return "PRESWING";
            case MGTT_State::SWING: return "SWING";
            case MGTT_State::BALL_IN_MOTION: return "BALL_IN_MOTION";
            case MGTT_State::POSTSWING: return "POSTSWING";
            case MGTT_State::TRANSITION: return "TRANSITION";
            case MGTT_State::LOGGED: return "LOGGED";
            case MGTT_State::UNDEFINED: return "UNDEFINED";
            default: return "INVALID";
        }
    }

private:
    MGTT_State currentState;
    std::unordered_map<MGTT_State, StateFunction> stateFunctions;

    // MGTT_State methods
    void initState(const Core::CPUThreadGuard& guard) {
        // std::cout << "running INIT MGTT_State." << std::endl;
        // Transition logic for INIT MGTT_State
        transitionTo(MGTT_State::MENU);
    }

    void menuState(const Core::CPUThreadGuard& guard) {
        // std::cout << "running MENU." << std::endl;
        // Transition logic for RUNNING MGTT_State
        rIsGolfRound.run(guard);

        if (rIsGolfRound.isActive()) {
            transitionTo(MGTT_State::ROUND_INFO);
        }
    }

    void roundInfoState(const Core::CPUThreadGuard& guard) {
        // Transition logic for ROUND_INFO MGTT_State
        rCourseId.run(guard);
        rRoundFormat.run(guard);
        rGreenType.run(guard);
        rTees.run(guard);
        rPlayerCount.run(guard);
        rPlayerPorts.run(guard);

        auto player_count = rPlayerCount.getValue(cStage_0);
        if (player_count && (player_count.value() >= 1 && player_count.value() <= 4)) {
            for (int i=0; i < player_count.value(); ++i){
                auto player_port = rPlayerPorts.getByteValue(0, i);
                std::cout << "P" << std::to_string(i) << " Port=" << std::to_string(player_port.value());

                //Get Rio Name
            }
            transitionTo(MGTT_State::TRANSITION);
        }
    }

    void transitionState(const Core::CPUThreadGuard& guard) {
        // Transition logic for TRANSITION MGTT_State
        rPlayerTurn.run(guard);

        auto player_turn = rPlayerTurn.getValue();

        if (player_turn && player_turn.value() <= 3) {
            //Read the player shot status of each player to find the "active" player
            rPlayerShotStatus[player_turn.value()].run(guard);

            rPlayerShotStatus.print();

            std::cout << "P" << std::to_string(player_turn.value()) << " ShotStatus=" << std::to_string(rPlayerShotStatus[player_turn.value()].getValue().value()) << "\n";

            //If player is ready to swing, transition to preswing()
            if (rPlayerShotStatus[player_turn.value()].getValue().value() == 4) { //Ready to swing
                transitionTo(MGTT_State::PRESWING);
            }
        }
    }

    void preswingState(const Core::CPUThreadGuard& guard) {
        // Transition logic for PRESWING MGTT_State
        // If shot status is 6 (I think) shot details are locked in
        rPlayerTurn.run(guard);

        auto player_turn = rPlayerTurn.getValue();

        if (player_turn && player_turn.value() <= 3) {
            //Read the player shot status of each player to find the "active" player
            rPlayerShotStatus[player_turn.value()].run(guard);

            std::cout << "P" << std::to_string(player_turn.value()) << " ShotStatus=" << std::to_string(rPlayerShotStatus[player_turn.value()].getValue().value()) << "\n";

            //If player is ready to swing, transition to preswing()
            if (rPlayerShotStatus[player_turn.value()].getValue().value() == 6) { //Ready to swing
                transitionTo(MGTT_State::SWING);
            }
        }
    }

    void swingState(const Core::CPUThreadGuard& guard) {
        // Transition logic for PRESWING MGTT_State
        // If shot status is 6 (I think) shot details are locked in

        //Read swing values

        // Ball in motion
        transitionTo(MGTT_State::BALL_IN_MOTION);
    }

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