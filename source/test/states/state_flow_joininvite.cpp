
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"

#include "test/states/state_flow_joininvite.h"

namespace test
{
    namespace states
    {
        Flow_JoinInvite::Flow_JoinInvite( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_JoinInvite::~Flow_JoinInvite()
        {}

        void Flow_JoinInvite::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_JoinInvite");
        }

        std::string Flow_JoinInvite::update()
        {
            return std::string();
        }

        void Flow_JoinInvite::exit()
        {

        }
    }
}