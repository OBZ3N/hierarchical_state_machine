
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_race_postunloading.h"

namespace test
{
    namespace states
    {
        Flow_Race_PostUnloading::Flow_Race_PostUnloading( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_Race_PostUnloading::~Flow_Race_PostUnloading()
        {}

        void Flow_Race_PostUnloading::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_Race_PostUnloading");
        }

        std::string Flow_Race_PostUnloading::update()
        {
            return std::string();
        }

        void Flow_Race_PostUnloading::exit()
        {

        }
    }
}