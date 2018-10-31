
#include <algorithm>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdarg.h>
#include "TinyXML/tinyxml.h"
#include "flow/state_machine.h"
#include "flow/state_machine_factory.h"
#include "flow/state_machine_xml_loader.h"

namespace flow
{
    StateMachine::StateMachine()
        : m_factory(nullptr)
    {
        m_statusString = "NULL";
        m_transition = nullptr;
    }

    StateMachine::~StateMachine()
    {
        shutdown();
    }

    void StateMachine::setFactory(StateMachineFactory* factory)
    {
        m_factory = factory;
    }

    void StateMachine::logDebug(debug::LogLevel level, const char* format, ...) const
    {
        va_list args;
        va_start(args, format);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::ostringstream stream;

        if (!isLoaded())
        {
            stream << "[FSM] " << std::left << std::setfill(' ') << std::setw(16) << m_statusString << " " << buffer;
        }
        else
        {
            if (m_statesToUpdate.empty())
            {
                stream << "[FSM] " << std::left << std::setfill(' ') << std::setw(16) << m_statusString << " " << buffer;
            }
            else
            {
                stream << "[FSM] " << std::left << std::setfill(' ') << std::setw(16) << m_statusString << " <" << m_statesToUpdate.front()->getSchema().m_fullname << "> " << buffer;
            }
        }

        debug::logDebug(level, stream.str().c_str());
    }

    void StateMachine::shutdown()
    {
        m_statusString = "SHUTTING_DOWN";

        m_schema = schema::StateMachine();

        for (auto asset : m_assetsToLoad)
            delete asset;

        for (auto asset : m_assetsToUnload)
            delete asset;

        for (auto state : m_statesToUpdate)
            delete state;

        delete m_transition;
        m_transition = nullptr;

        m_assetsToLoad.clear();
        m_assetsToUnload.clear();
        m_statesToUpdate.clear();

        m_statusString = "SHUTDOWN";

        logDebug(debug::LogLevel::Trace, "Shutdown.");
    }

    bool StateMachine::load(const std::string& xml_filename)
    {
        logDebug(debug::LogLevel::Info, "loading '%s'...", xml_filename.c_str());

        m_statusString = "LOADING";

        StateMachineXmlLoader loader;

        if (!loader.load(xml_filename))
        {
            logDebug(debug::LogLevel::Error, "schema '%s' failed to load.", xml_filename.c_str());

            m_statusString = "LOADING FAILED";

            return false;
        }

        m_schema = loader.getSchema();

        if (!calculateLookupTable())
        {
            logDebug(debug::LogLevel::Error, "failed to build lookup table.");

            m_statusString = "LOADING FAILED";

            return false;
        }

        m_statusString = "LOADED";

        logDebug(debug::LogLevel::Info, "schema '%s' loaded.", xml_filename.c_str());

        return true;
    }

    bool StateMachine::isLoaded() const
    {
        return !m_schema.m_states.empty();
    }

    // loading the schemas and initialising the states.
    void StateMachine::start()
    {
        // already started.
        if (isStarted())
            return;

        restart();
    }

    // loading the schemas and initialising the states.
    void StateMachine::restart()
    {
        m_statusString = "STARTING";

        if (m_factory == nullptr)
        {
            logDebug(debug::LogLevel::Error, "Factory is nullptr.");
            return;
        }

        if (isStarted())
        {
            // need to exit first before restarting.
            schema::Transition start_transition;
            start_transition.m_next_state = "";
            start_transition.m_event_name = "restart_state_machine";
            m_transition = m_factory->createTransition(start_transition);
        }
        else
        {
            // can start with a new transition.
            m_transition = m_factory->createTransition(m_schema.m_transition_start_state_machine);
        }
    }

    void StateMachine::stop()
    {
        // already stopped.
        if (isStopped())
            return;

        m_statusString = "STOPPING";

        // generate a stopping transition.
        schema::Transition stop_transition;
        stop_transition.m_next_state = "";
        stop_transition.m_event_name = "stop_state_machine";

        // override old transition. 
        if (m_transition != nullptr)
            delete m_transition;

        if (m_factory == nullptr)
        {
            logDebug(debug::LogLevel::Error, "Factory is nullptr.");
        }
        else
        {
            m_transition = m_factory->createTransition(stop_transition);
        }
    }

    bool StateMachine::isStopped() const
    {
        // transitioning. Not finished.
        if (m_transition != nullptr)
            return false;

        // still states being updated.
        if (!m_statesToUpdate.empty())
            return false;

        // assets still being loaded.
        if (!m_assetsToLoad.empty())
            return false;

        // assets still being unloaded.
        if (!m_assetsToUnload.empty())
            return false;

        return true;
    }

    bool StateMachine::isStarted() const
    {
        return !isStopped();
    }

    void StateMachine::update()
    {
        // some assets are being unloaded. This will block everything else until done.
        if (!m_assetsToUnload.empty())
        {
            m_statusString = "CLOSE_ASSETS";
            updateAssetsToUnload();

            // do nothing else until all the assets have been unloaded properly.
            return;
        }

        // some assets are being loaded. This will block everything else until done.
        if (!m_assetsToLoad.empty())
        {
            // check if we need to exit states to reach the state requested by the transition.
            m_statusString = "OPEN_ASSETS";
            updateAssetsToLoad();

            // do nothing else until all the assets have been loaded properly.
            return;
        }

        // no transitions being processed, update state stacks to find the next transition.
        if (m_transition == nullptr)
        {
            m_statusString = "UPDATING_STATES";
            updateStatesStack();
        }

        // transition fired. Pop and push states to reach the targeted state.
        if (m_transition != nullptr)
        {
            updateTransition();
        }
    }

    void StateMachine::updateStatesStack()
    {
        // start updating states, from the bottom up.
        for (auto it = m_statesToUpdate.rbegin(); it != m_statesToUpdate.rend(); ++it)
        {
            auto state = *it;

            std::string event = state->update();

            // state fired a transition event.
            if (!event.empty())
            {
                // find the transition definition.
                for (auto transition_schema : state->getSchema().m_transitions)
                {
                    if (transition_schema.m_event_name == event)
                    {
                        if (m_factory == nullptr)
                        {
                            logDebug(debug::LogLevel::Error, "Factory is nullptr.");
                        }
                        else
                        {
                            // transition found.
                            m_transition = m_factory->createTransition(transition_schema);
                            break;
                        }
                    }
                }

                // transition invalid. The event probably doesn't match a transition in the schema data.
                if (m_transition == nullptr)
                {
                    logDebug(debug::LogLevel::Error, "Transition not found for event '%s'.", event.c_str());
                }
                else
                {
                    logDebug(debug::LogLevel::Info, "transition '%s' ----> <%s>.", m_transition->getEvent().c_str(), m_transition->getNextState().c_str());

                    // stop updating the state stack, we're going some place.
                    break;
                }
            }
        }
    }


    void StateMachine::updateTransition()
    {
        // see what needs to be done to reach the targeted transition.
        std::string state_name;
        bool push_state;
        bool pop_state;
        evaluateTransitionOperation(state_name, push_state, pop_state);

        //-------------------------------------------------------------
        // SANITY CHECK OUR TRANSITION OPERATION.
        //-------------------------------------------------------------
#if defined(_DEBUG)
        // use state fullnames to evaluate the transition operation and compare with our previous results.
        std::string sanity_check_state_name;
        bool sanity_check_push_state;
        bool sanity_check_pop_state;
        evaluateTransitionOperationWithFullNames(sanity_check_state_name, sanity_check_push_state, sanity_check_pop_state);

        // check if both evaluations match.
        ASSERT_SANITY(  ( sanity_check_state_name == state_name ) &&
                        ( sanity_check_push_state == push_state ) &&
                        ( sanity_check_pop_state == pop_state ),
            "EvaluateTransition() : bitfield evaluation('%s', %s) does not match string evaluation('%s', %s).",
            state_name.c_str(),
            push_state && pop_state ? "push&pop???" : push_state ? "push" : pop_state ? "pop" : "---",
            sanity_check_state_name.c_str(),
            sanity_check_push_state && sanity_check_pop_state ? "push&pop???" : sanity_check_push_state ? "push" : sanity_check_pop_state ? "pop" : "---" );
#endif

        // we need to pop states from the stack so we can reach to the transition's next state.
        if (pop_state)
        {
            // first state to pop.
            State* state = m_statesToUpdate.front();

            // sanity check.
            if (state->getFullName() != state_name)
            {
                logDebug(debug::LogLevel::SanityCheck, "invalid pop called on state('%s').", state_name.c_str());
                return;
            }

            // exit state first.
            state->exit();

            logDebug(debug::LogLevel::Info, "exited.");

            // cleanup.
            delete state;

            logDebug(debug::LogLevel::Trace, "state '%s' destroyed.", state_name);

            m_statesToUpdate.pop_front();

            // start unloading assets.
            const schema::State& state_schema = state->getSchema();

            if (m_factory == nullptr)
            {
                logDebug(debug::LogLevel::Error, "Factory is nullptr.");
            }
            else
            {
                // generate the list of asets to unload.
                m_assetsToUnload = m_factory->createAssets(state_schema.m_assets);

                logDebug(debug::LogLevel::Trace, "unloading %d assets...", m_assetsToUnload.size());
            }

            // run one update.
            updateAssetsToUnload();
        }

        // we need to push states on the stack to reach the transition's state target.
        if (push_state)
        {
            auto it = m_schema.m_states.find(state_name);

            if (it == m_schema.m_states.end())
            {
                logDebug(debug::LogLevel::Error, "couldn't find schema for state '%s'.", state_name);
                return;
            }

            const schema::State& state_schema = it->second;

            State* state = nullptr;

            if (m_factory == nullptr)
            {
                logDebug(debug::LogLevel::Error, "Factory is nullptr.");
            }
            else
            {
                // first state to pop.
                state = m_factory->createState(state_schema);
            }

            // sanity check.
            if (state == nullptr)
            {
                logDebug(debug::LogLevel::Error, "couldn't create state '%s'.", state_schema.m_fullname.c_str());
            }
            else
            {
                m_statesToUpdate.push_front(state);

                m_assetsToLoad = m_factory->createAssets(state_schema.m_assets);

                // signal we'll be laoding assets.
                if (!m_assetsToLoad.empty())
                {
                    logDebug(debug::LogLevel::Info, "loading %d assets...", m_assetsToLoad.size());
                }

                // start loading assets.
                updateAssetsToLoad();
            }
        }

        // nothing to do, clear transition.
        if (!pop_state && !push_state)
        {
            // Clear everything, we're done.
            delete m_transition;
            m_transition = nullptr;
        }
    }

    void StateMachine::evaluateTransitionOperation(std::string& state_fullname, bool& push_state, bool& pop_state) const
    {
        // use state bitnames for evaluation.
        Bitfield state_bitname;
        evaluateTransitionOperation(state_bitname, push_state, pop_state);

        // translate bitname into fullname (if we need to do anything).
        if (push_state || pop_state)
        {
            // find state name matching the bitname.
            std::string state_name;
            auto it = m_state_fullname_lookup.find(state_bitname);
            if (it == m_state_fullname_lookup.end())
            {
                logDebug(debug::LogLevel::SanityCheck, "failed to find state matching the bitfield.");
                push_state = false;
                pop_state = false;
                return;
            }
            state_name = it->second;
        }
    }

    // find what operation needs to be performed for the transition.
    void StateMachine::evaluateTransitionOperation(Bitfield& state_bitname, bool& push_state, bool& pop_state) const
    {
        // default, do nothing.
        push_state = false;
        pop_state = false;

        // no transition to process.
        if (m_transition == nullptr)
            return;

        // reached the bottom of the state machine, and transition tries to reach the bottom.
        if (m_statesToUpdate.empty() && m_transition->getNextState().empty())
        {
            // check if we're booting or re-booting the state machine.
            if (m_transition->getEvent() == "start_state_machine" ||
                m_transition->getEvent() == "restart_state_machine")
            {
                // the state we're trying to reach is the root state.
                state_bitname.setBit(0, true);
                push_state = true;
                return;
            }

            // stopping the state machine should be fired by the event 'stop_state_machine'.
            ASSERT_SANITY( m_transition->getEvent() == "stop_state_machine" , "state machine is stopped by an unexpected event '%s'.", m_transition->getEvent().c_str());
            return;
        }

        // find state bitname we're trying to reach.
        auto it = m_state_bitname_lookup.find(m_transition->getNextState());
        if (it == m_state_bitname_lookup.end())
        {
            // do nothing. maybe an error in the xml definition, so we'll be stuck here, but that's ok.
            logDebug(debug::LogLevel::Error, "could not find lookup bitfield for state '%s'.", m_transition->getNextState().c_str());
            return;
        }

        // state stack is empty. Just go to root state.
        if (m_statesToUpdate.empty())
        {
            // the state we're trying to reach is now the startup state.
            state_bitname.setBit(0, true);
            push_state = true;
            return;
        }

        // find state bitname we're currently at.
        auto jt = m_state_bitname_lookup.find(m_statesToUpdate.front()->getSchema().m_fullname);
        if (jt == m_state_bitname_lookup.end())
        {
            //. this is not ok. SHould not happen.
            logDebug(debug::LogLevel::SanityCheck, "could not find lookup bitfield for state '%s'.", m_statesToUpdate.front()->getSchema().m_fullname.c_str());
            return;
        }

        // find bitfields that identify the states we're trying to reach, and the state we're currently at.
        const Bitfield& state_to_reach = it->second;
        const Bitfield& state_current = jt->second;

        // both match each other. It's a restart request.
        if (state_to_reach == state_current)
        {
            // restart the state.
            state_bitname = state_current;
            pop_state = true;
            return;
        }

        // find the bitfield that identifies the state that is common to the target and current states.
        Bitfield common_bitfield = state_to_reach & state_current;

        // we've reached the common state. start loading states.
        if (common_bitfield == state_current)
        {
            for (size_t i = state_current.getNumBits(); i < state_to_reach.getNumBits(); ++i)
            {
                // stop at first bitfield with a bit set to true.
                // that will be the first sub-state we need to enter.
                if (state_current.getBit(i))
                {
                    // generate the sub-state bitfield we need to enter.
                    state_bitname = common_bitfield;
                    state_bitname.setBit(i, true);
                    push_state = true;
                    return;
                }
            }
        }
        // we're not at the common state. Start unloading states to reach the common state.
        else
        {
            // start from the top, work your way down.
            state_bitname = state_current;
            pop_state = true;
        }
    }

    // load all assets before entering the state.
    void StateMachine::updateAssetsToLoad()
    {
        Asset* first_asset = m_assetsToLoad.empty() ? nullptr : m_assetsToLoad.front();

        Asset* asset = first_asset;

        while (asset != nullptr)
        {
            // sanity check.
            ASSERT_SANITY(!m_statesToUpdate.empty(),
                "asset '%s' for state '%s'. No state are being updated.",
                asset->getSchema().m_asset_name.c_str(),
                asset->getSchema().m_state_name.c_str());

            // sanity check.
            ASSERT_SANITY(asset->getSchema().m_state_name == m_statesToUpdate.front()->getFullName(),
                "asset '%s' for state '%s'. topmost state '%s' doesn't match.",
                asset->getSchema().m_asset_name.c_str(),
                asset->getSchema().m_state_name.c_str(),
                m_statesToUpdate.front()->getFullName().c_str());

            // start loading, if necessary.
            bool asset_load_started = asset->isLoading() ? true : asset->load();

            // check if loading completed.
            bool asset_load_finished = asset_load_started && asset->isLoaded();

            // no loading further required.
            if (!asset_load_started || asset_load_finished)
            {
                logDebug(debug::LogLevel::Trace, "asset '%s' %s.", asset->getSchema().m_asset_name.c_str(), asset_load_started ? "loaded" : "load skipped");

                // move to the next asset to load.
                m_assetsToLoad.pop_front();

                asset = m_assetsToLoad.empty() ? nullptr : m_assetsToLoad.front();
            }
            else
            {
                break;
            }
        }

        // loading finished.
        if (asset == nullptr)
        {
            if (first_asset != nullptr)
            {
                // we had at least some assets to load. Tell when it's finished.
                logDebug(debug::LogLevel::Trace, "all assets loaded.");
            }

            // now enter the front state, which should be the state we've loaded assets for.
            State* state = m_statesToUpdate.front();

            // entering next state.
            state->enter();

            logDebug(debug::LogLevel::Info, "entered.");

            // we've reached the state requested by the transition.
            if (m_transition != nullptr && state->getSchema().m_fullname == m_transition->getNextState())
            {
                logDebug(debug::LogLevel::Trace, "transition completed. We've reached our target state.");

                // we can destroy the transition now.
                delete m_transition;

                m_transition = nullptr;
            }
        }
    }

    // unload all assets that are being loaded.
    void StateMachine::updateAssetsToUnload()
    {
        Asset* first_asset = m_assetsToUnload.empty() ? nullptr : m_assetsToUnload.back();

        Asset* asset = first_asset;

        while (asset != nullptr)
        {
            bool asset_unload_started = asset->isUnloading() ? true : asset->unload();

            bool asset_unload_finished = asset_unload_started && asset->isUnloaded();

            if (!asset_unload_started || asset_unload_finished)
            {
                logDebug(debug::LogLevel::Trace, "asset '%s' %s.", asset->getSchema().m_asset_name.c_str(), asset_unload_started ? "unloaded" : "unload skipped");

                // pop back the last asset.
                m_assetsToUnload.pop_back();

                asset = m_assetsToUnload.empty() ? nullptr : m_assetsToUnload.back();
            }
            else
            {
                break;
            }
        }

        // unloading finished.
        if (asset == nullptr && first_asset != nullptr)
        {
            // we had assets to unload. Tell when it's finished.
            logDebug(debug::LogLevel::Trace, "all assets unloaded.");
        }
    }

    std::vector<schema::State*> StateMachine::calculateSubStates(const schema::State* parent)
    {
        std::vector<schema::State*> sub_states;

        std::string parent_name = parent->m_fullname;

        for (auto& state_schema : m_schema.m_states)
        {
            std::string state_name = state_schema.second.m_fullname;

            size_t pos = state_name.find(parent_name);

            if (pos != 0)
                continue;

            std::string sub_state_name = state_name.substr(parent_name.size());

            pos = sub_state_name.rfind("\\");

            if (pos != 0)
                continue;

            sub_states.push_back(&state_schema.second);
        }

        return sub_states;
    }

    void StateMachine::calculateLookupKey(const Bitfield& bitfield, const schema::State* state)
    {
        // find sub-states.
        std::vector<schema::State*> subStates = calculateSubStates(state);

        // calculate sub-states bitfields.
        std::vector<Bitfield> subStateBitfields;

        for (size_t i = 0; i < subStates.size(); ++i)
        {
            Bitfield subStateBitfield = bitfield;

            for (size_t j = 0; j < subStates.size(); ++j)
            {
                bool value = (j == i) ? true : false;

                subStateBitfield.pushBit(value);
            }

            subStateBitfields.push_back(subStateBitfield);

            // store lookup entries.
            m_state_fullname_lookup[subStateBitfield] = subStates[i]->m_fullname;
            m_state_bitname_lookup[subStates[i]->m_fullname] = subStateBitfield;
        }

        // calculate further sub-states bitfields.
        for (size_t i = 0; i < subStates.size(); ++i)
        {
            calculateLookupKey(subStateBitfields[i], subStates[i]);
        }
    }

    bool StateMachine::calculateLookupTable()
    {
        const std::string& root_name = m_schema.m_transition_start_state_machine.m_next_state;

        auto it = m_schema.m_states.find(root_name);

        if (it == m_schema.m_states.end())
        {
            return false;
        }

        schema::State* root = &(it->second);

        // root bitfield.
        Bitfield bitfield;
        bitfield.setBit(0, true);

        // add lookup entries.
        m_state_fullname_lookup[bitfield] = root_name;
        m_state_bitname_lookup[root_name] = bitfield;

        // calculate sub-states lookup tables.
        calculateLookupKey(bitfield, root);

        return true;
    }
}

#if defined(_DEBUG)
namespace flow
{
    std::string StateMachine::evaluateCommonState(const std::string& state_a, const std::string& state_b) const
    {
        std::string common_state;

        size_t len = std::min(state_a.length(), state_b.length());

        for (size_t i = 0; i < len; ++i)
        {
            if (state_a[i] == state_b[i])
            {
                common_state.push_back(state_a[i]);
            }
            else
            {
                break;
            }
        }

        // root identifier.
        if (common_state == "\\\\")
        {
            // just set to zero.
            common_state.clear();
        }

        // remove trailing '\'.
        if (!common_state.empty() && common_state.back() == '\\')
        {
            common_state.pop_back();
        }
        return common_state;
    }

    void StateMachine::evaluateTransitionOperationWithFullNames(std::string& state, bool& push_state, bool& pop_state) const
    {
        // default, do nothing.
        push_state = false;
        pop_state = false;

        // no transition to process.
        if (m_transition == nullptr)
            return;

        // where the transition is telling us to go to.
        std::string state_to_reach = m_transition->getNextState();

        // reached the bottom of the state machine, and that's where we want to be.
        if (m_statesToUpdate.empty() && state_to_reach.empty())
        {
            // check if we're booting or re-booting the state machine.
            if (m_transition->getEvent() == "start_state_machine" ||
                m_transition->getEvent() == "restart_state_machine")
            {
                // the state we're trying to reach is now the startup state.
                state_to_reach = m_schema.m_transition_start_state_machine.m_next_state;
            }
            // event is 'stop_state_machine', or some unknown event.
            else
            {
                // stopping the state machine should be fired by the event 'stop_state_machine'.
                if (m_transition->getEvent() != "stop_state_machine")
                {
                    logDebug(debug::LogLevel::Error, "state machine is stopped by an unexpected event '%s'.", m_transition->getEvent().c_str());
                }
                // done. No push or pop of states required.
                return;
            }
        }

        // the name of the state at the top of the update stack. 
        // '' if empty.
        std::string state_current;

        if (!m_statesToUpdate.empty())
        {
            state_current = m_statesToUpdate.front()->getFullName();
        }

        // transition points to the current state.
        // So we want to restart the current state.
        if (state_current == state_to_reach)
        {
            // start from the top, work your way down.
            state = state_current;
            pop_state = true;
            return;
        }

        // find the branch state that is common to both the transition target and the current top-most state.
        std::string state_common = evaluateCommonState(state_current, state_to_reach);

        // we've reached the common state. start loading states.
        if (state_common == state_current)
        {
            // the list of states that we'll eventually push onto the stack.
            std::string states_to_push = state_to_reach.substr(state_common.length(), state_to_reach.length() - state_common.length());

            if (states_to_push.find("\\\\") == 0)
            {
                states_to_push = states_to_push.substr(2, states_to_push.length() - 2);
            }


            if (states_to_push.find("\\") == 0)
            {
                states_to_push = states_to_push.substr(1, states_to_push.length() - 1);
            }

            // find the first state to push.
            size_t pos = states_to_push.find_first_of('\\');

            state = (pos != std::string::npos) ? state_common + std::string("\\") + states_to_push.substr(0, pos) : state_to_reach;

            push_state = true;
            return;
        }
        // we're not at the common state. Start unloading statesto reach the common state.
        else
        {
            // start from the top, work your way down.
            state = state_current;
            pop_state = true;
        }
    }
}
#endif
