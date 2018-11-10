
// Saturday 03 November 2018, 20:53:34 : generated from 'xmls\state_machine.xml'.

#include "test/test_game.h"
#include "test/states/state_flow_checkinvite.h"

namespace test
{
    namespace states
    {
        Flow_CheckInvite::Flow_CheckInvite( const hsm::schema::State& schema, test::Game& game )
            : test::StateBase( schema, game )
        {}
        
        Flow_CheckInvite::~Flow_CheckInvite()
        {}

        void Flow_CheckInvite::enter()
        {
             debug::logWarning("[TEST] [TODO] IMPLEMENT ME ('%s').", "Flow_CheckInvite");
        }

        std::string Flow_CheckInvite::update()
        {
            if ( m_game.m_interrupt_manager->isRaised( "invite_accepted" ) )
            {
                return "multiplayer";
            }
            else
            {
                return "front_end";
            }
        }

        void Flow_CheckInvite::exit()
        {

        }
    }
}