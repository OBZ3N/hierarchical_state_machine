
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_multiplayer_lobby_waitforplayers.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers::Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers::~Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers()
        {}

        void Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers");
        }

        std::string Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_Multiplayer_Lobby_WaitForPlayers::exit()
        {

        }
    }
}