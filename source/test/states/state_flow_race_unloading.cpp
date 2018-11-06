
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_race_unloading.h"

namespace test
{
    namespace states
    {
        Flow_Race_Unloading::Flow_Race_Unloading( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_Race_Unloading::~Flow_Race_Unloading()
        {}

        void Flow_Race_Unloading::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_Race_Unloading");
        }

        std::string Flow_Race_Unloading::update()
        {
            return std::string();
        }

        void Flow_Race_Unloading::exit()
        {

        }
    }
}