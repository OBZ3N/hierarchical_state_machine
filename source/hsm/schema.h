#pragma once

#include <list>
#include <utility>
#include <string>
#include <unordered_map>

namespace hsm
{
    namespace schema
    {
        struct Resource
        {
            std::string                                  m_name;
            std::unordered_map<std::string, std::string> m_attributes;
        };

        struct Transition
        {
            std::string m_event;
            std::string m_state;
            std::unordered_map<std::string, std::string> m_attributes;
        };

        struct State
        {
            std::string                                         m_shortname;
            std::string                                         m_fullname;
            std::list<Resource>                                 m_resources;
            std::list<Transition>                               m_transitions;
            std::list<Transition>                               m_exceptions;
            std::unordered_map<std::string, std::string>        m_attributes;

            // state hierarchy.
            std::string                                         m_parent;
            std::list<std::string>                              m_children;
        };

        // represents the entire state machine, in a flat hierarchy.
        struct StateMachine
        {
            std::unordered_map<std::string, State>                      m_states;
            std::unordered_map<std::string, std::list<std::string>>     m_shortname_mappings;
            std::string                                                 m_filename;
            std::string                                                 m_initial_state;
        };
    }
}
