
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_multiplayermenu.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_MultiplayerMenu::Flow_FrontEnd_MultiplayerMenu( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_MultiplayerMenu::~Flow_FrontEnd_MultiplayerMenu()
        {}

        void Flow_FrontEnd_MultiplayerMenu::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_MultiplayerMenu");
        }

        std::string Flow_FrontEnd_MultiplayerMenu::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_MultiplayerMenu::exit()
        {

        }
    }
}