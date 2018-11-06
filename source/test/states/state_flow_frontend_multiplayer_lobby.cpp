
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_multiplayer_lobby.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_Multiplayer_Lobby::Flow_FrontEnd_Multiplayer_Lobby( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_Multiplayer_Lobby::~Flow_FrontEnd_Multiplayer_Lobby()
        {}

        void Flow_FrontEnd_Multiplayer_Lobby::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_Multiplayer_Lobby");
        }

        std::string Flow_FrontEnd_Multiplayer_Lobby::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_Multiplayer_Lobby::exit()
        {

        }
    }
}