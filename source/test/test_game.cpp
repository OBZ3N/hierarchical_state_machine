
#include "test/test_game.h"

namespace test
{
    Game::Game()
        : m_interrupt_manager( nullptr )
        , m_simul_session( nullptr )
        , m_simul_network( nullptr )
        , m_simul_online_services( nullptr )
        , m_simul_invites( nullptr )
    {}

    void Game::initialise()
    {
        m_interrupt_manager = new test::InterruptManager( m_schema.m_interrupt_manager );
        m_simul_session = new test::SimulatedSession( *m_interrupt_manager );
        m_simul_network = new test::SimulatedNetwork( *m_interrupt_manager );
        m_simul_online_services = new test::SimulatedOnlineServices( *m_interrupt_manager );
        m_simul_invites = new test::SimulatedInvites( *m_interrupt_manager );
    }

    void Game::shutdown()
    {
        if( m_interrupt_manager ) delete m_interrupt_manager;
        if( m_simul_session ) delete m_simul_session;
        if( m_simul_network ) delete m_simul_network;
        if( m_simul_online_services ) delete m_simul_online_services;
        if( m_simul_invites ) delete m_simul_invites;

        m_interrupt_manager = nullptr;
        m_simul_session = nullptr;
        m_simul_network = nullptr;
        m_simul_online_services = nullptr;
        m_simul_invites = nullptr;
    }

    void Game::start()
    {
        if( m_interrupt_manager ) m_interrupt_manager->reset();
        if( m_simul_session ) m_simul_session->reset();
        if( m_simul_network ) m_simul_network->start();
        if( m_simul_online_services ) m_simul_online_services->start();
        if( m_simul_invites ) m_simul_invites->start();
    }

    void Game::stop()
    {
        if( m_interrupt_manager ) m_interrupt_manager->reset();
        if ( m_simul_session ) m_simul_session->reset();
        if ( m_simul_network ) m_simul_network->stop();
        if ( m_simul_online_services ) m_simul_online_services->stop();
        if ( m_simul_invites ) m_simul_invites->stop();
    }

    void Game::update()
    {}
}