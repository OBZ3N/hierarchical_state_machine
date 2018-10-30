
// Wednesday 17 October 2018, 14:12:33 : generated from 'Source\test\xmls\state_machine.xml'.

#pragma once

#include "flow/schema.h"
#include "flow/asset.h"
#include "flow/state.h"
#include "flow/debug.h"
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
            static Flow_PressStartScreen* Instantiate(const flow::schema::State& schema, test::Game& game ) { return new Flow_PressStartScreen( schema, game ); }

        public:
            Flow_PressStartScreen(const flow::schema::State& schema, test::Game& game);
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