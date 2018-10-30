
#pragma once

#include "flow/schema.h"

namespace test
{
    // very basic source / xml code generator.
    // will generate .cpp / .h create empty states as defined in the state machine schema.
    class StateMachineGenerator
    {
    public:
        enum Modes
        {
            Build = (0),
            Rebuild = (1 << 0),
            Backup = (1 << 1),
        };

        StateMachineGenerator(const flow::schema::StateMachine& schema);

        // generate states, and the states factory table, for all states defined in the state machine schema.
        void generateCppFiles(const std::string& templates_path, const std::string& generated_path, int modes = Modes::Build);

        const flow::schema::StateMachine& getSchema() const { return m_schema; }

    private:
        const flow::schema::StateMachine& m_schema;
    };
}
