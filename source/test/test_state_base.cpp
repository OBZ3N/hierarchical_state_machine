
#include "test/test_state_base.h"

namespace test
{
    StateBase::StateBase( const hsm::schema::State& schema, test::Game & game )
        : hsm::State( schema )
        , m_game( game)
    {}
}