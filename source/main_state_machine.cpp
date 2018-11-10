
#include <windows.h>

#include "hsm/schema.h"
#include "hsm/timer.h"
#include "hsm/random.h"
#include "hsm/asset.h"
#include "hsm/state.h"
#include "hsm/state_machine.h"
#include "hsm/state_machine_factory.h"
#include "hsm/state_machine_xml_loader.h"
#include "test/test_factory.h"
#include "test/test_game.h"
#include "test/test_simulated_event.h"

struct Settings
{
    Settings();

    std::string m_state_machine_input_xml;
    int         m_state_machine_stop_count_limit;
    int         m_state_machine_restart_count_limit;
};

Settings::Settings()
{
    m_state_machine_input_xml = "xmls\\state_machine.xml";
    m_state_machine_stop_count_limit = 0;
    m_state_machine_restart_count_limit = 0;
}


struct Runtime
{
    Runtime(const Settings& settings)
        : m_settings(settings)
    {}

    void Start()
    {
        m_stop_counter = 0;
        m_restart_counter = 0;

        m_stop_timer.start();
        m_restart_timer.start();

        m_stop_time = utils::randomFloat(0.0f, 100.0f);
        m_restart_time = utils::randomFloat(0.0f, 100.0f);
    }

    bool IsCompleted()
    {
        return (m_stop_counter > m_settings.m_state_machine_stop_count_limit || m_restart_counter > m_settings.m_state_machine_restart_count_limit);
    }

    void UpdateStateMachineStops(bool& stop_state_machine)
    {
        stop_state_machine = false;

        if (m_settings.m_state_machine_stop_count_limit == 0)
        {
            stop_state_machine = false;
            return;
        }

        if (!m_stop_timer.isStarted())
            m_stop_timer.start();

        if(m_stop_timer.getElapsedTimeSecs() < m_stop_time)
        {
            stop_state_machine = false;
            return;
        }
        else
        {
            m_stop_counter++;
            m_stop_timer.restart();
            m_stop_time = utils::randomFloat(0.0f, 600.0f);
            stop_state_machine = true;
        }
    }

    void UpdateStateMachineRestarts(bool& restart_state_machine)
    {
        restart_state_machine = false;

        if (m_settings.m_state_machine_restart_count_limit == 0)
        {
            restart_state_machine = false;
            return;
        }

        if (!m_restart_timer.isStarted())
            m_restart_timer.start();

        if (m_restart_timer.getElapsedTimeSecs() < m_restart_time)
        {
            restart_state_machine = false;
            return;
        }
        else
        {
            m_restart_counter++;
            m_restart_timer.restart();
            m_restart_time = utils::randomFloat(0.0f, 300.0f);
            restart_state_machine = true;
        }
    }
    
    int m_stop_counter;
    int m_restart_counter;

    float m_stop_time;
    float m_restart_time;
 
    utils::Timer m_stop_timer;
    utils::Timer m_restart_timer;

    const Settings& m_settings;
};

void main()
{
    Settings settings;
    
    // the game environment.
    test::Game the_game;
    the_game.initialise();

    hsm::StateMachineXmlLoader state_machine_loader;

    if ( !state_machine_loader.load( settings.m_state_machine_input_xml ) )
    {
        debug::logFatal( "schema '%s' failed to load.", settings.m_state_machine_input_xml.c_str() );

        exit( -1 );
    }

    // the factory that will instantiate states for the state machine.
    test::StateMachineFactory state_machine_factory( state_machine_loader.getSchema(), the_game );
    
    // the state machine.
    hsm::StateMachine state_machine( state_machine_loader.getSchema(), &state_machine_factory );

    Runtime runtime(settings);
    runtime.Start();

    state_machine.start();

    while (!runtime.IsCompleted())
    {
        bool stop_state_machine = false;
        runtime.UpdateStateMachineStops(stop_state_machine);

        bool restart_state_machine = false;
        runtime.UpdateStateMachineRestarts(restart_state_machine);
        
        if ( stop_state_machine )
        {
            state_machine.stop();
        }

        if ( restart_state_machine )
        {
            state_machine.start();
        }

        state_machine.update();

        Sleep(10);
    }

    the_game.shutdown();
}