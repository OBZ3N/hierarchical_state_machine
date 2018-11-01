#pragma once

#include <list>
#include <utility>
#include <unordered_map>
#include <string>
#include "hsm/schema.h"

class TiXmlNode;

namespace hsm
{
    class StateMachineFactory;

    // load the schema definitions from a xml file.
    class StateMachineXmlLoader
    {
    public:
        // load the schema from an xml file.
        bool load( const std::string& xml_filename );

        // generate the xml tree that will represent the state machine schema.
        void generateXml(const std::string& xml_filename, const schema::StateMachine& schema);

        // the loaded schema.
        const schema::StateMachine& getSchema() const { return m_schema; }

    private:
        bool parseXmlNode(TiXmlNode* node, schema::State* state = nullptr, int indent = 0);

        bool isValidStateName(const std::string& name) const;

        std::string evaluateStateFullNameFromReference(std::string current_path, std::string state_ref) const;

        std::string indentString(int indent) const;

        bool validateSchema();

        bool isValidStateName(std::string& name) const;

        // values loaded.
        schema::StateMachine m_schema;
    };

}