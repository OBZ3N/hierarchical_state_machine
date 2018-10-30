#pragma once

#include "flow/schema.h"
#include "flow/asset.h"
#include "flow/state.h"
#include "flow/debug.h"
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
            static Flow* Instantiate(const flow::schema::State& schema, test::Game& game ) { return new Flow( schema, game ); }

        public:
            Flow(const flow::schema::State& schema, test::Game& game);
            ~Flow();

            virtual void enter() override;
            virtual std::string update() override;
            virtual void exit() override;

            test::SimulatedEvent m_simul_boot_sequence;
        };
    }
}
