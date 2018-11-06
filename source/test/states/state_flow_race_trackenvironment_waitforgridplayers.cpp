
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_race_trackenvironment_waitforgridplayers.h"

namespace test
{
    namespace states
    {
        Flow_Race_TrackEnvironment_WaitForGridPlayers::Flow_Race_TrackEnvironment_WaitForGridPlayers( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_Race_TrackEnvironment_WaitForGridPlayers::~Flow_Race_TrackEnvironment_WaitForGridPlayers()
        {}

        void Flow_Race_TrackEnvironment_WaitForGridPlayers::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_Race_TrackEnvironment_WaitForGridPlayers");
        }

        std::string Flow_Race_TrackEnvironment_WaitForGridPlayers::update()
        {
            return std::string();
        }

        void Flow_Race_TrackEnvironment_WaitForGridPlayers::exit()
        {

        }
    }
}