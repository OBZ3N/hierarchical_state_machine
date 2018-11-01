#pragma once

#include "hsm/schema.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/debug.h"
#include "test/test_state_base.h"

namespace test
{
    namespace states
    {
        class Flow : public test::StateBase
        {
        public:
            // factory.
            static const char* GetTypeName() { return "\\\\Flow"; }
            static Flow* Instantiate(const hsm::schema::State& schema, test::Game& game ) { return new Flow( schema, game ); }

        public:
            Flow(const hsm::schema::State& schema, test::Game& game);
            ~Flow();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

            test::SimulatedEvent m_simul_boot_sequence;
        };
    }
}
