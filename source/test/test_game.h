
#pragma once

#include "test/test_schema.h"
#include "test/test_interrupts.h"
#include "test/test_simulated_event.h"
#include "test/test_simulated_invites.h"
#include "test/test_simulated_network.h"
#include "test/test_simulated_online_services.h"
#include "test/test_simulated_session.h"

namespace test
{
    class Game
    {
    public:
        Game();

        void load();
        void shutdown();

        void start();
        void update();
        void stop();

    public:
        test::schema::Game m_schema;

        test::InterruptManager*         m_interrupt_manager;
        test::SimulatedSession*         m_simul_session;
        test::SimulatedNetwork*         m_simul_network;
        test::SimulatedOnlineServices*  m_simul_online_services;
        test::SimulatedInvites*         m_simul_invites;
    };
}