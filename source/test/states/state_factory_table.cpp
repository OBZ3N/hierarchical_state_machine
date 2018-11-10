
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_factory.h"
#include "test/test_game.h"

#include "state_flow_race_trackenvironment_raceresults.h"
#include "state_flow_race_trackenvironment_endracecutscene.h"
#include "state_flow_race_trackenvironment_racing.h"
#include "state_flow_race_trackenvironment_gridcountdown.h"
#include "state_flow_race_trackenvironment_introcutscene.h"
#include "state_flow_race_waitfortrackdatasync.h"
#include "state_flow_race_loading.h"
#include "state_flow_race_trackenvironment.h"
#include "state_flow_race_unloading.h"
#include "state_flow_race_trackenvironment_nextracevote.h"
#include "state_flow_checkinvite.h"
#include "state_flow_race_trackenvironment_waitforgridplayers.h"
#include "state_flow_attractscreen.h"
#include "state_flow_pressstartscreen.h"
#include "state_flow_race.h"
#include "state_flow.h"
#include "state_flow_race_postunloading.h"

namespace test
{
    void generateStateFactoryTable( StateFactoryTable& state_factory_table, test::Game& game )
    {
        state_factory_table[states::Flow_Race_TrackEnvironment_RaceResults::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_RaceResults::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_EndRaceCutscene::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_EndRaceCutscene::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_Racing::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_Racing::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_GridCountdown::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_GridCountdown::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_IntroCutscene::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_IntroCutscene::Instantiate);
        state_factory_table[states::Flow_Race_WaitForTrackDataSync::GetTypeName()] = (&states::Flow_Race_WaitForTrackDataSync::Instantiate);
        state_factory_table[states::Flow_Race_Loading::GetTypeName()] = (&states::Flow_Race_Loading::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment::GetTypeName()] = (&states::Flow_Race_TrackEnvironment::Instantiate);
        state_factory_table[states::Flow_Race_Unloading::GetTypeName()] = (&states::Flow_Race_Unloading::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_NextRaceVote::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_NextRaceVote::Instantiate);
        state_factory_table[states::Flow_CheckInvite::GetTypeName()] = (&states::Flow_CheckInvite::Instantiate);
        state_factory_table[states::Flow_Race_TrackEnvironment_WaitForGridPlayers::GetTypeName()] = (&states::Flow_Race_TrackEnvironment_WaitForGridPlayers::Instantiate);
        state_factory_table[states::Flow_AttractScreen::GetTypeName()] = (&states::Flow_AttractScreen::Instantiate);
        state_factory_table[states::Flow_PressStartScreen::GetTypeName()] = (&states::Flow_PressStartScreen::Instantiate);
        state_factory_table[states::Flow_Race::GetTypeName()] = (&states::Flow_Race::Instantiate);
        state_factory_table[states::Flow::GetTypeName()] = (&states::Flow::Instantiate);
        state_factory_table[states::Flow_Race_PostUnloading::GetTypeName()] = (&states::Flow_Race_PostUnloading::Instantiate);
    }
}
