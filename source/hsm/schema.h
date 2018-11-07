#pragma once

#include <list>
#include <utility>
#include <string>
#include <unordered_map>

namespace hsm
{
    namespace schema
    {
        struct Asset
        {
            std::string                                  m_state_name;
            std::string                                  m_asset_name;
            std::unordered_map<std::string, std::string> m_attributes;
        };

        struct Transition
        {
            std::string m_event_name;
            std::string m_next_state;
        };

        struct State
        {
            std::string                                     m_shortname;
            std::string                                     m_fullname;
            std::list<Asset>                                m_assets;
            std::list<Transition>                           m_transitions;
            std::unordered_map<std::string, std::string>    m_attributes;
        };

        // represents the entire state machine, in a flat hierarchy.
        struct StateMachine
        {
            std::string                                             m_filename;
            std::unordered_map<std::string, State>                  m_states;
            std::string                                             m_initial_state;
        };
    }
}
