
#include "test/test_state_base.h"

namespace test
{
    StateBase::StateBase( const flow::schema::State& schema, test::Game & game )
        : flow::State( schema )
        , m_game( game)
    {}
}