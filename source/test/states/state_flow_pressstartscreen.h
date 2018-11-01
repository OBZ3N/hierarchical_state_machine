
// Wednesday 17 October 2018, 14:12:33 : generated from 'Source\test\xmls\state_machine.xml'.

#pragma once

#include "hsm/schema.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/debug.h"
#include "test/test_simulated_event.h"
#include "test/test_state_base.h"
#include "test/test_game.h"

namespace test
{
    namespace states
    {
        class Flow_PressStartScreen : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow\\PressStartScreen"; }
            static Flow_PressStartScreen* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow_PressStartScreen( schema, game ); }

        public:
            Flow_PressStartScreen(const hsm::schema::State& schema, test::Game& game);
            ~Flow_PressStartScreen();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

        private:
            test::SimulatedEvent m_simul_pressed_start;
            test::SimulatedEvent m_simul_inactivity_timeout;
        };
    }
}