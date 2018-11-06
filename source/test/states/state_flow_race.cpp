
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_race.h"

namespace test
{
    namespace states
    {
        Flow_Race::Flow_Race( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_Race::~Flow_Race()
        {}

        void Flow_Race::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_Race");
        }

        std::string Flow_Race::update()
        {
            return std::string();
        }

        void Flow_Race::exit()
        {

        }
    }
}