
#include <algorithm>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdarg.h>
#include "TinyXML/tinyxml.h"
#include "hsm/state_machine.h"
#include "hsm/state_machine_factory.h"

namespace hsm
{
    StateMachine::StateMachine( const schema::StateMachine& schema, StateMachineFactory* factory )
        : m_schema( schema )
        , m_factory( factory )
        , m_stateToEnter( nullptr )
        , m_stateToExit( nullptr )
    {
        if ( !calculateLookupTable() )
        {
            ASSERT_FATAL( false, "failed to build bitname lookup table." );
        }

        m_statusString = "CONSTRUCTED";
    }

    StateMachine::~StateMachine()
    {
        shutdown();
    }

    void StateMachine::logDebug(debug::LogLevel level, const char* format, ...) const
    {
        va_list args;
        va_start( args, format );
        std::string message = debug::formatString( format, args );
        va_end(args);

        std::ostringstream stream;

        if (m_statesToUpdate.empty())
        {
            stream << "[FSM] " << std::left << std::setfill(' ') << std::setw(16) << m_statusString << " " << message;
        }
        else
        {
            stream << "[FSM] " << std::left << std::setfill(' ') << std::setw(16) << m_statusString << " <" << m_statesToUpdate.back()->getSchema().m_fullname << "> " << message;
        }
        
        debug::logDebug(level, stream.str().c_str());
    }

    void StateMachine::shutdown()
    {
        m_statusString = "SHUTTING_DOWN";

        for (auto resource : m_resourcesToLoad)
            delete resource;
        m_resourcesToLoad.clear();

        for (auto resource : m_resourcesToUnload)
            delete resource;
        m_resourcesToUnload.clear();

        for (auto resource : m_resourcesToUpdate)
            delete resource;
        m_resourcesToUpdate.clear();

        for (auto state : m_statesToUpdate)
            delete state;
        m_statesToUpdate.clear();

        delete m_stateToEnter;
        m_stateToEnter = nullptr;

        delete m_stateToExit;
        m_stateToExit = nullptr;

        m_stop = schema::Transition();
        m_start = schema::Transition();
        m_transition = schema::Transition();
        m_exceptions.clear();

        m_statusString = "SHUTDOWN";
        logDebug(debug::LogLevel::Trace, "Shutdown.");
    }

    // loading the schemas and initialising the states.
    void StateMachine::start( std::string& initial_state )
    {
        // already started.
        if (isStarted())
            return;

        m_start.m_state = initial_state.empty() ? m_schema.m_initial_state : initial_state;
        m_start.m_event = "_enter";

        m_statusString = "STARTING";
    }

    // loading the schemas and initialising the states.
    void StateMachine::restart( std::string& initial_state )
    {
        // first, do an exit transition.
        m_stop.m_state = "";
        m_stop.m_event = "_exit";

        // second, cache a new restart transition.
        m_start.m_state = initial_state.empty() ? m_schema.m_initial_state : initial_state;
        m_start.m_event = "_enter";

        m_statusString = "RESTARTING";
    }

    void StateMachine::stop()
    {
        // already stopped.
        if (isStopped())
            return;

        // clear the start request.
        m_start = schema::Transition();

        m_stop.m_event = "_exit";
        m_stop.m_state = "";

        m_statusString = "STOPPING";
    }

    std::list<const Resource*> StateMachine::getCurrentResources() const
    {
        std::list<const Resource*> resources;

        for (auto resource : m_resourcesToUpdate)
        {
            resources.push_back(resource);
        }
        
        for (auto resource : m_resourcesToLoad)
        {
            resources.push_back(resource);
        }
        return resources;
    }

    std::list<const State*> StateMachine::getCurrentStates() const
    {
        std::list<const State*> states;

        for (auto state : m_statesToUpdate)
        {
            states.push_back(state);
        }

        if (m_stateToEnter != nullptr)
        {
            states.push_back(m_stateToEnter);
        }

        return states;
    }

    std::list<const schema::Transition*> StateMachine::getCurrentExceptions() const
    {
        std::list<const schema::Transition*> exceptions;

        for (const auto& ex : m_exceptions)
        {
            exceptions.push_back(&ex);
        }
        return exceptions;
    }

    const schema::Transition* StateMachine::getCurrentTransition() const
    {
        if (!m_stop.m_event.empty())
            return &m_stop;

        if (!m_start.m_event.empty())
            return &m_start;

        if (!m_exceptions.empty())
            return &m_exceptions.front();

        if (!m_transition.m_event.empty())
            return &m_transition;

        return nullptr;
    }

    void StateMachine::setTransition(const State& from_state, const std::string& event_name, const std::string& message)
    {
        std::unordered_map<std::string, std::string> attributes;
        setTransition(from_state, event_name, attributes, message);
    }

    void StateMachine::throwException(const std::string& name, const std::string& message)
    {
        std::unordered_map<std::string, std::string> attributes;
        throwException(name, attributes, message);
    }

    void StateMachine::setTransition(const State& from_state, const std::string& event_name, const std::unordered_map<std::string, std::string>& attributes, const std::string& message)
    {
        // find the transition definition.
        for (auto transition_schema : from_state.getSchema().m_transitions)
        {
            if (transition_schema.m_event == event_name && attributes == transition_schema.m_attributes)
            {
                if (m_factory == nullptr)
                {
                    logDebug(debug::LogLevel::Error, "Factory is nullptr.");
                }
                else
                {
                    // transition found.
                    m_transition = transition_schema;
                    break;
                }
            }
        }

        if (attributes.empty())
        {
            logDebug(debug::LogLevel::Info, "transition '%s' called from state '%s'.", event_name.c_str(), from_state.getSchema().m_fullname.c_str());
        }
        else
        {
            std::string str;
            std::ostringstream stream(str);
            for (auto it = attributes.begin(); it != attributes.end(); ++it)
            {
                if (it != attributes.begin())
                {
                    stream << ", ";
                }
                stream << it->first << "=" << it->second;
            }
            logDebug(debug::LogLevel::Info, "transition '%s' [%s] called from state '%s'.", event_name.c_str(), str.c_str(), from_state.getSchema().m_fullname.c_str());
        }
    }

    void StateMachine::throwException(const std::string& name, const std::unordered_map<std::string, std::string>& attributes, const std::string& message)
    {
        // start updating states, from the bottom up.
        for (auto it = m_statesToUpdate.rbegin(); it != m_statesToUpdate.rend(); ++it)
        {
            auto state = *it;

            // find the transition definition.
            for (auto exception_schema : state->getSchema().m_exceptions)
            {
                if (exception_schema.m_event == name && attributes == exception_schema.m_attributes)
                {
                    if (m_factory == nullptr)
                    {
                        logDebug(debug::LogLevel::Error, "Factory is nullptr.");
                    }
                    else
                    {
                        // transition found.
                        m_exceptions.push_back(exception_schema);
                        m_transition = schema::Transition();
                        break;
                    }
                }
            }
        }

        if (attributes.empty())
        {
            logDebug(debug::LogLevel::Warning, "exception '%s' thrown.", name.c_str());
        }
        else
        {
            std::string str;
            std::ostringstream stream(str);
            for (auto it = attributes.begin(); it != attributes.end(); ++it)
            {
                if (it != attributes.begin())
                {
                    stream << ", ";
                }
                stream << it->first << "=" << it->second;
            }
            logDebug(debug::LogLevel::Warning, "exception '%s' [%s] thrown.", name.c_str(), str.c_str());
        }
    }

    bool StateMachine::isStopped() const
    {
        // transitioning. Not finished.
        if(!m_transition.m_event.empty())
            return false;

        // still has a state to enter.
        if(m_stateToEnter != nullptr)
            return false;

        // still has a state to exit.
        if(m_stateToExit != nullptr)
            return false;

        // still states being updated.
        if (!m_statesToUpdate.empty())
            return false;

        // resources still being loaded.
        if (!m_resourcesToLoad.empty())
            return false;

        // resources still being unloaded.
        if (!m_resourcesToUnload.empty())
            return false;

        return true;
    }

    bool StateMachine::isStarted() const
    {
        return !isStopped();
    }

    void StateMachine::update()
    {
        // start updating states, from the bottom up.
        for (auto it = m_statesToUpdate.begin(); it != m_statesToUpdate.end(); ++it)
        {
            auto state = *it;

            state->update();

            // state fired a transition event.
            if (getCurrentTransition())
                break;
        }

        const schema::Transition* transition = getCurrentTransition();

        // state fired a transition event.
        if (transition != nullptr)
        {
            updateTransition(*transition);
        }
    }

    void StateMachine::updateStatus_EnterState()
    {
        if (m_stateToEnter != nullptr)
        {
            std::string state_name = m_stateToEnter->getSchema().m_fullname.c_str();

            // entering next state.
            m_stateToEnter->enter();

            logDebug(debug::LogLevel::Trace, "state '%s' entered.", state_name.c_str());

            m_statesToUpdate.push_back(m_stateToEnter);

            m_stateToEnter = nullptr;
        }
    }

    void StateMachine::updateStatus_ExitState()
    {
        if (m_stateToExit != nullptr)
        {
            std::string state_name = m_stateToExit->getSchema().m_fullname.c_str();

            // entering next state.
            m_stateToExit->exit();

            logDebug(debug::LogLevel::Trace, "state '%s' exited.", state_name.c_str());

            m_statesToUpdate.remove(m_stateToExit);

            delete m_stateToExit;

            logDebug(debug::LogLevel::Trace, "state '%s' deleted.", state_name.c_str());

            m_stateToExit = nullptr;
        }
    }

    void StateMachine::updateStatus_LoadResources()
    {
        for (auto resource : m_resourcesToLoad)
        {
            std::string resource_name = resource->getSchema().m_name;

            resource->load();

            logDebug(debug::LogLevel::Trace, "resource '%s' loaded.", resource_name.c_str());

            m_resourcesToUpdate.push_back(resource);
        }
        m_resourcesToLoad.clear();
    }

    void StateMachine::updateStatus_UnloadResources()
    {
        for (auto resource : m_resourcesToUnload)
        {
            std::string resource_name = resource->getSchema().m_name;

            resource->unload();

            logDebug(debug::LogLevel::Trace, "resource '%s' unloaded.", resource_name.c_str());

            delete resource;

            logDebug(debug::LogLevel::Trace, "resource '%s' deleted.", resource_name.c_str());

        }
        m_resourcesToUnload.clear();
    }

    void StateMachine::getRequiredResources(const std::string state_name, std::list<schema::Resource>& resources) const
    {
        auto it = m_schema.m_states.find(state_name);
        
        if (it != m_schema.m_states.end())
        {
            const schema::State& state = it->second;
        
            resources.insert(resources.begin(), state.m_resources.begin(), state.m_resources.end());

            if (!state.m_parent.empty())
            {
                getRequiredResources(state.m_parent, resources);
            }
        }
    }

    void StateMachine::evaluateResourceOperations(const TransitionOperation& operations)
    {
        // resources used by the next state.
        std::list<schema::Resource> requiredResources;
        getRequiredResources(operations.m_next_state, requiredResources);

        // resources that are not required by the next state.
        for (auto jt = m_resourcesToUpdate.begin(); jt != m_resourcesToUpdate.end(); )
        {
            const schema::Resource& res = (*jt)->getSchema();

            // check if current resource is required.
            auto it = std::find_if(requiredResources.begin(), requiredResources.end(), [&res](const schema::Resource& r)
            {
                return res.m_name == r.m_name;
            });

            if (it == requiredResources.end())
            {
                // add to unload list.
                m_resourcesToUnload.push_back(*jt);

                // remove from update list.
                jt = m_resourcesToUpdate.erase(jt);
            }
            else
            {
                ++jt;
            }
        }

        // resources required by the next state.
        for (const auto& res : requiredResources)
        {
            auto it = std::find_if(m_resourcesToUpdate.begin(), m_resourcesToUpdate.end(), [&res](const Resource* r)
            {
                return res.m_name == r->getSchema().m_name;
            });

            // resource not find in update list.
            if (it == m_resourcesToUpdate.end())
            {
                // add to load list.
                Resource* resource = m_factory->createResource(res);

                if (resource != nullptr)
                {
                    //logDebug(debug::LogLevel::Trace, "resource '%s' created.", resource->getSchema().m_name.c_str());

                    m_resourcesToLoad.push_back(resource);
                }
            }
        }
    }

    void StateMachine::evaluateStateOperations(const TransitionOperation& operations)
    {
        // new state to enter.
        if (operations.m_flags & eEnterNextState)
        {
            auto it = m_schema.m_states.find(operations.m_next_state);

            if (it == m_schema.m_states.end())
            {
                if (!isNullState(operations.m_next_state))
                {
                    logDebug(debug::LogLevel::Error, "couldn't find schema for state '%s'.", operations.m_next_state.c_str());
                }
            }
            else
            {
                const schema::State& state_schema = it->second;

                m_stateToEnter = m_factory->createState(this, state_schema);

                //logDebug(debug::LogLevel::Trace, "state '%s' created.", state_schema.m_fullname.c_str());
            }
        }

        // state to exit.
        if (operations.m_flags & eExitCurrentState)
        {
            auto it = std::find_if(m_statesToUpdate.begin(), m_statesToUpdate.end(), [&operations](State* state)
            {
                return state->getSchema().m_fullname == operations.m_current_state;
            });

            if (it == m_statesToUpdate.end())
            {
                if (!isNullState(operations.m_current_state))
                {
                    logDebug(debug::LogLevel::Error, "couldn't find current state '%s'.", operations.m_current_state.c_str());
                }
            }
            else
            {
                ASSERT_FATAL(it == std::prev(m_statesToUpdate.end(), 1), "state '%s' is not the top-most state in the update list.", operations.m_current_state.c_str());

                State* state = (*it);

                m_stateToExit = state;

                m_statesToUpdate.erase(it);

                //logDebug(debug::LogLevel::Trace, "state '%s' created.", state_schema.m_fullname.c_str());
            }
        }
    }

    void StateMachine::updateTransition(const schema::Transition& transition)
    {
        // calculate what operations required to reach the state targeted by transition.
        TransitionOperation operations;
        evaluateTransitionOperations(transition, operations);

        // see what resources to unload or load.
        evaluateResourceOperations(operations);

        // setup what state needs to be entered and exited.
        evaluateStateOperations(operations);

        // execute operations.
        if (m_stateToExit != nullptr)
        {
            // exit top-most state first.
            updateStatus_ExitState();
        }

        if (!m_resourcesToUnload.empty())
        {
            // unload its resources.
            updateStatus_UnloadResources();
        }

        if (!m_resourcesToLoad.empty())
        {
            // load resources for the new state.
            updateStatus_LoadResources();
        }

        if (m_stateToEnter != nullptr)
        {
            // enter new state.
            updateStatus_EnterState();
        }

        // remove transitions that are no longer relevant.
        consumeTransitions();
    }

    void StateMachine::consumeTransitions()
    {
        const schema::State* state_schema = m_statesToUpdate.empty() ? nullptr : &m_statesToUpdate.back()->getSchema();

        if (state_schema == nullptr)
        {
            m_stop = schema::Transition();
            m_start = schema::Transition();
            m_transition = schema::Transition();
            m_exceptions.clear();

            m_statusString = "STOPPED";
        }
        else
        {
            if (m_start.m_state == state_schema->m_fullname)
            {
                m_start = schema::Transition();

                m_statusString = "UPDATING";
            }

            if (m_transition.m_state == state_schema->m_fullname)
            {
                m_transition = schema::Transition();
            }

            for (auto it = m_exceptions.begin(); it != m_exceptions.end(); )
            {
                const schema::Transition& exception = (*it);

                if (exception.m_state == state_schema->m_fullname)
                {
                    it = m_exceptions.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    bool StateMachine::isNullState(const std::string& a) const
    {
        return a.empty() || a == "\\";
    }

    bool StateMachine::isSameState(const std::string& a, const std::string& b) const
    {
        auto at = m_schema.m_states.find(a);
        auto bt = m_schema.m_states.find(b);
        return (at == bt);
    }

    bool StateMachine::isSiblingState(const std::string& a, const std::string& b) const
    {
        auto at = m_schema.m_states.find(a);
        auto bt = m_schema.m_states.find(b);

        if (at == bt)
            return false;

        if (at == m_schema.m_states.end() || bt == m_schema.m_states.end())
            return false;

        auto ap = m_schema.m_states.find(at->second.m_parent);
        auto bp = m_schema.m_states.find(bt->second.m_parent);

        return (ap == bp);
    }

    bool StateMachine::isParentState(const std::string& a, const std::string& b) const
    {
        auto at = m_schema.m_states.find(a);
        auto bt = m_schema.m_states.find(b);

        if (at == bt)
            return false;

        auto ap = (at == m_schema.m_states.end()) ? m_schema.m_states.end() : m_schema.m_states.find(at->second.m_parent);
        
        return (ap == bt);
    }

    void StateMachine::evaluateTransitionOperations(const schema::Transition& transition, TransitionOperation& operations) const
    {
        const std::string& current_state_name = m_statesToUpdate.empty() ? "" : m_statesToUpdate.back()->getSchema().m_fullname;
        const std::string& desired_state_name = transition.m_state;

        if (isSameState(current_state_name, desired_state_name))
        {
            operations.m_flags = eNextStateIsCurrent;
            return;
        }
        
        if (isSiblingState(current_state_name, desired_state_name))
        {
            operations.m_flags = eNextStateIsSibling | eExitCurrentState | eEnterNextState;
            operations.m_next_state = desired_state_name;
            operations.m_current_state = current_state_name;
            return;
        }

        auto it = m_state_bitname_lookup.find(desired_state_name);
        auto jt = m_state_bitname_lookup.find(current_state_name);

        // find bitfields that identify the states we're trying to reach, and the state we're currently at.
        Bitfield null_bitfield;
        const Bitfield& state_to_reach = (it == m_state_bitname_lookup.end()) ? null_bitfield : it->second;
        const Bitfield& state_current = (jt == m_state_bitname_lookup.end()) ? null_bitfield : jt->second;

        // find the bitfield that identifies the state that is common to the target and current states.
        Bitfield common_bitfield = state_to_reach & state_current;

        // As long as there are states not in the desired state hierarchy, 
        // we need to pop them until we reach the desired hierarchy.
        if (common_bitfield != state_current)
        {
            // go down to parent and try again.
            auto it = m_schema.m_states.find(current_state_name);
            std::string previous_state_name = (it == m_schema.m_states.end()) ? "" : it->second.m_parent;

            operations.m_flags         = eExitCurrentState;
            operations.m_next_state    = previous_state_name;
            operations.m_current_state = current_state_name;
            return;
        }

        // we've reached the desired state hierarchy, go back up again to reach the 
        // next state in that hierarchy.
        for (size_t i = state_current.getNumBits(); i < state_to_reach.getNumBits(); ++i)
        {
            // stop at first bit set to true.
            // which will be the path we need to follow to reach the desired state.
            if (state_to_reach.getBit(i))
            {
                // generate the state bitfield we need to enter.
                common_bitfield.setBit(i, true);

                // find state name matching the bitname.
                auto it = m_state_fullname_lookup.find(common_bitfield);
                std::string next_state_name = (it == m_state_fullname_lookup.end()) ? "" : it->second;

                operations.m_flags = eEnterNextState;
                operations.m_next_state = next_state_name;
                operations.m_current_state = current_state_name;
                return;
            }
        }
    }

    // calculate the list of child states for a given state.
    std::vector<const schema::State*> StateMachine::calculateSubStates(const schema::State* parent)
    {
        std::vector<const schema::State*> sub_states;

        std::string parent_name = parent->m_fullname;

        for ( auto& state_schema : m_schema.m_states )
        {
            std::string state_name = state_schema.second.m_fullname;

            size_t pos = state_name.find( parent_name );

            if (pos != 0)
                continue;

            std::string sub_state_name = state_name.substr( parent_name.size() );

            pos = sub_state_name.rfind( "\\" );

            if (pos != 0)
                continue;

            sub_states.push_back(&state_schema.second);
        }

        return sub_states;
    }

    // calculate bitfields for the state hierarchy under a state.
    void StateMachine::calculateLookupKey( const Bitfield& bitfield, const schema::State* state )
    {
        // find sub-states.
        std::vector<const schema::State*> subStates = calculateSubStates( state );

        // calculate child states bitfields.
        std::vector<Bitfield> subStateBitfields;

        // generate bitfields for each child state.
        for ( size_t i = 0; i < subStates.size(); ++i )
        {
            Bitfield subStateBitfield = bitfield;

            for ( size_t j = 0; j < subStates.size(); ++j )
            {
                bool value = (j == i) ? true : false;

                subStateBitfield.pushBit(value);
            }

            subStateBitfields.push_back(subStateBitfield);

            // store lookup entries for the child state.
            m_state_fullname_lookup[subStateBitfield] = subStates[i]->m_fullname;
            m_state_bitname_lookup[subStates[i]->m_fullname] = subStateBitfield;
        }

        // calculate further sub-states bitfields.
        for (size_t i = 0; i < subStates.size(); ++i)
        {
            calculateLookupKey(subStateBitfields[i], subStates[i]);
        }
    }

    // calculate bitfield lookup table for each satte in the state machine.
    bool StateMachine::calculateLookupTable()
    {
        // find the root.
        const std::string& root_name = m_schema.m_initial_state;

        auto it = m_schema.m_states.find(root_name);

        if (it == m_schema.m_states.end())
        {
            return false;
        }

        const schema::State* root = &(it->second);

        // root bitfield.
        Bitfield bitfield;
        bitfield.setBit(0, true);

        // add lookup entries.
        m_state_fullname_lookup[bitfield] = root_name;
        m_state_bitname_lookup[root_name] = bitfield;

        // calculate bitfields for each sub-state under the root.
        calculateLookupKey(bitfield, root);

        return true;
    }
}

