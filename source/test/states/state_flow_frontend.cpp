
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd::Flow_FrontEnd( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd::~Flow_FrontEnd()
        {}

        void Flow_FrontEnd::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd");
        }

        std::string Flow_FrontEnd::update()
        {
            return std::string();
        }

        void Flow_FrontEnd::exit()
        {

        }
    }
}