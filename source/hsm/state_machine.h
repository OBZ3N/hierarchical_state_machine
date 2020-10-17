#pragma once

#include <list>
#include <utility>
#include <unordered_map>
#include <string>

#include "hsm/resource.h"
#include "hsm/bitfield.h"
#include "hsm/debug.h"
#include "hsm/state.h"
#include "hsm/state_machine_factory.h"

class TiXmlNode;

namespace hsm
{
    class StateMachine
    {
    public:
        StateMachine( const schema::StateMachine& schema, StateMachineFactory* factory );
        ~StateMachine();

        void start( std::string& initial_state = std::string() );
        void restart( std::string& initial_state = std::string() );
        void stop();

        bool isStopped() const;
        bool isStarted() const;

        void update();
        void shutdown();

        StateMachineFactory* getFactory() { return m_factory; }
        const schema::StateMachine& getSchema() const { return m_schema; }

        void setTransition(const State& from_state, const std::string& event_name, const std::string& message="");
        void setTransition(const State& from_state, const std::string& event_name, const std::unordered_map<std::string, std::string>& attributes, const std::string& message = "");

        void throwException(const std::string& name, const std::string& message="");
        void throwException(const std::string& name, const std::unordered_map<std::string, std::string>& attributes, const std::string& message = "");

        std::list<const State*> getCurrentStates() const;
        std::list<const Resource*> getCurrentResources() const;
        std::list<const schema::Transition*> getCurrentExceptions() const;
        const schema::Transition* getCurrentTransition() const;

        const std::string& getStatusString() const { return m_statusString; }
        
    private:
        void updateStatus_LoadResources();
        void updateStatus_EnterState();
        void updateStatus_ExitState();
        void updateStatus_UnloadResources();
        void consumeTransitions();

        enum eTransitionFlags
        {
            eEnterNextState         = (1 << 0), // enter the next state.
            eExitCurrentState       = (1 << 1), // exit the current state.
            eNextStateIsCurrent     = (1 << 2), // we've reached the desired state.
            eNextStateIsSibling     = (1 << 4), // current state and next state are siblings (TODO : do not enter / exit resources that are common to the states).
        };

        struct TransitionOperation
        {
            unsigned int    m_flags;
            std::string     m_next_state;
            std::string     m_current_state;
        };

        // utils.
        void getRequiredResources(const std::string state_name, std::list<schema::Resource>& resources) const;
        void updateTransition(const schema::Transition& transition);
        void evaluateTransitionOperations(const schema::Transition& transition, TransitionOperation& operation) const;
        void evaluateResourceOperations(const TransitionOperation& operations);
        void evaluateStateOperations(const TransitionOperation& operations);
        
        bool isNullState(const std::string& a) const;
        bool isSameState(const std::string& a, const std::string& b) const;
        bool isSiblingState(const std::string& a, const std::string& b) const;
        bool isParentState(const std::string& a, const std::string& b) const;

        // debugging (extra string formating context).
        void logDebug( debug::LogLevel level, const char* format, ... ) const;

        // members.
        StateMachineFactory*                        m_factory;              // create components (resource, state, transition...).
        schema::StateMachine                        m_schema;               // state machine definition (loaded from xml).
        
        schema::Transition                          m_transition;           // current transition.
        schema::Transition                          m_start;                // transition for a restart.
        schema::Transition                          m_stop;                 // transition for a restart.
        std::list<schema::Transition>               m_exceptions;           // current exceptions.

        std::list<Resource*>                        m_resourcesToLoad;      // resources to load before entering a state.
        std::list<Resource*>                        m_resourcesToUpdate;    // resource stack to update.
        std::list<Resource*>                        m_resourcesToUnload;    // resources to unload before exiting a state.
        
        State*                                      m_stateToEnter;         // new state to enter.
        std::list<State*>                           m_statesToUpdate;       // state stack to update.
        State*                                      m_stateToExit;          // new state to exit.

        std::string                                 m_statusString;         // status of the state machine (debugging).

        // lookup table utils.
        // the state bitname is a bitfield representation, of the location of the state within the hierarchy. 
        // This is unique to each state, and can be use to accelerate transition evaluations (what state to push, pop, ect... to reach the desired state).
        std::vector<const schema::State*> calculateSubStates( const schema::State* parent );
        void calculateLookupKey( const Bitfield& bitfield, const schema::State* state );
        bool calculateLookupTable();

        // lookup tables between state bitnames, and state fullnames.
        std::unordered_map<Bitfield, std::string> m_state_fullname_lookup;
        std::unordered_map<std::string, Bitfield> m_state_bitname_lookup;
    };
}