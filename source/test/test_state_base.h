#pragma once

#include "hsm/schema.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/debug.h"
#include "test/test_game.h"

namespace test
{
    class StateBase : public hsm::State
    {
    public:
        StateBase( const hsm::schema::State& schema, test::Game& game );

    protected:
        test::Game& m_game;
    };
}
