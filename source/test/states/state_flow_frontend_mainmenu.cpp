
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_frontend_mainmenu.h"

namespace test
{
    namespace states
    {
        Flow_FrontEnd_MainMenu::Flow_FrontEnd_MainMenu( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_FrontEnd_MainMenu::~Flow_FrontEnd_MainMenu()
        {}

        void Flow_FrontEnd_MainMenu::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_FrontEnd_MainMenu");
        }

        std::string Flow_FrontEnd_MainMenu::update()
        {
            return std::string();
        }

        void Flow_FrontEnd_MainMenu::exit()
        {

        }
    }
}