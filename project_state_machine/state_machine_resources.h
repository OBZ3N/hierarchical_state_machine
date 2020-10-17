#pragma once

#include <list>
#include <utility>
#include <unordered_map>
#include <string>

#include "hsm/resource.h"
#include "hsm/bitfield.h"
#include "hsm/debug.h"
#include "hsm/state.h"
#include "hsm/transition.h"

namespace hsm
{
    class StateMachineFactory;
    class StateMachineResources
    {
    public:
        StateMachineResources(StateMachineFactory* factory);
        ~StateMachineResources();

        std::list<Resource*> m_resources;
    };
}
