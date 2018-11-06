
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_race_trackenvironment_racing.h"

namespace test
{
    namespace states
    {
        Flow_Race_TrackEnvironment_Racing::Flow_Race_TrackEnvironment_Racing( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_Race_TrackEnvironment_Racing::~Flow_Race_TrackEnvironment_Racing()
        {}

        void Flow_Race_TrackEnvironment_Racing::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_Race_TrackEnvironment_Racing");
        }

        std::string Flow_Race_TrackEnvironment_Racing::update()
        {
            return std::string();
        }

        void Flow_Race_TrackEnvironment_Racing::exit()
        {

        }
    }
}