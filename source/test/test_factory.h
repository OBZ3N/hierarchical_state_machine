#pragma once

#include <unordered_map>
#include "hsm/state_machine_factory.h"

namespace test
{
    class Game;

    // factory functions.
    typedef std::function<hsm::State*(const hsm::schema::State&, test::Game&)> StateFactoryFunc;
    typedef std::unordered_map<std::string, StateFactoryFunc> StateFactoryTable;
    extern void generateStateFactoryTable(std::unordered_map<std::string, StateFactoryFunc>& state_factory_table, test::Game& game);

    class StateMachineFactory : public hsm::StateMachineFactory
    {
    public:
        StateMachineFactory(const hsm::schema::StateMachine& schema, test::Game& game);

        virtual hsm::State*                  createState( const hsm::schema::State& state_schema ) override;
        virtual std::list<hsm::Asset*>       createAssets( const std::list<hsm::schema::Asset>& asset_schemas ) override;
        virtual std::list<hsm::Transition*>  createTransitions( const std::list<hsm::schema::Transition>& transition_schemas ) override;

        virtual hsm::Asset*                  createAsset( const hsm::schema::Asset& asset_schema ) override;
        virtual hsm::Transition*             createTransition( const hsm::schema::Transition& transition_schema ) override;

    private:
        StateFactoryTable m_state_factory_table;

        test::Game& m_game;

        const hsm::schema::StateMachine& m_schema;
    };
}