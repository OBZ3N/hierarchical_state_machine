#pragma once

#include "flow/schema.h"
#include "flow/asset.h"
#include "flow/state.h"
#include "flow/debug.h"
#include "test/test_game.h"

namespace test
{
    class StateBase : public flow::State
    {
    public:
        StateBase( const flow::schema::State& schema, test::Game& game );

    protected:
        test::Game& m_game;
    };
}
