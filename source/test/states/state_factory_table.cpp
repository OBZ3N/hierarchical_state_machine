
// Wednesday 17 October 2018, 14:19:28 : generated from 'Source\test\xmls\state_machine.xml'.

#include "test/test_factory.h"
#include "test/test_game.h"

#include "state_flow_pressstartscreen.h"
#include "state_flow.h"

namespace test
{
    void generateStateFactoryTable( StateFactoryTable& state_factory_table, test::Game& game )
    {
        state_factory_table[states::Flow_PressStartScreen::GetTypeName()] = (&states::Flow_PressStartScreen::Instantiate);
        state_factory_table[states::Flow::GetTypeName()] = (&states::Flow::Instantiate);
    }
}
