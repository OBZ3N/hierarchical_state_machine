
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_multiplayer.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_Multiplayer::Flow_FrontEnd_Multiplayer( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_Multiplayer::~Flow_FrontEnd_Multiplayer()
        {}

        void Flow_FrontEnd_Multiplayer::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_Multiplayer");
        }

        std::string Flow_FrontEnd_Multiplayer::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_Multiplayer::exit()
        {

        }
    }
}