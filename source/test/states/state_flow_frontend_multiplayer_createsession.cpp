
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_multiplayer_createsession.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_Multiplayer_CreateSession::Flow_FrontEnd_Multiplayer_CreateSession( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_Multiplayer_CreateSession::~Flow_FrontEnd_Multiplayer_CreateSession()
        {}

        void Flow_FrontEnd_Multiplayer_CreateSession::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_Multiplayer_CreateSession");
        }

        std::string Flow_FrontEnd_Multiplayer_CreateSession::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_Multiplayer_CreateSession::exit()
        {

        }
    }
}