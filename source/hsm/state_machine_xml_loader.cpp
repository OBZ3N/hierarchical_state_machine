
#include <algorithm>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdarg.h>
#include "TinyXML/tinyxml.h"
#include "hsm/debug.h"
#include "hsm/state_machine_xml_loader.h"

namespace hsm
{
    // loading the schemas from xmls.
    bool StateMachineXmlLoader::load( const std::string& xml_filename )
    {
        debug::logInfo( "loading document '%s'...", xml_filename.c_str() );

        m_schema = schema::StateMachine();

        TiXmlDocument document;

        document.LoadFile( xml_filename.c_str() );

        if ( document.Error() )
        {
            debug::logError( "Failed to load document '%s'.", xml_filename.c_str() );
            return false;
        }

        debug::logInfo( "parsing document '%s'...", xml_filename.c_str() );

        if ( !parseXmlNode( &document ) )
        {
            debug::logError( "Failed to parse document '%s'.", xml_filename.c_str() );
            return false;
        }

        debug::logInfo( "validating document '%s'...", xml_filename.c_str() );

        if ( ! validateSchema() )
        {
            debug::logError( "Failed to validate document '%s'.", xml_filename.c_str() );
            return false;
        }

        debug::logInfo( "state machine schema '%s' loaded.", xml_filename.c_str() );

        return true;
    }

    // compute a state's fullname, from an 
    // current_path : current path in use, (absolute) (e.g. '\\Flow\FrontEnd\MainScreen\Multiplayer').
    // state_ref : a reference to a state, which will be a relative path from the current path (e.g. '.\MultiplayerOptionScreen', '..\Campain', '\\Flow\FrontEnd\MainScreen\Campaign', ect...).
    // for readability we'll be mostly referencing siblings, with no special specification. 
    // - nothing specified : We'll be referencing a sibling state.
    // - '.\' specified : will be referencing one of our child.
    // - '..\' specified : we're going into the parent of our parent.
    // - specifying '\\' means we're indexing from the root, and it's an absolute path.
    // [TODO] maybe think of a better way.
    std::string StateMachineXmlLoader::evaluateStateFullNameFromRelativeName( std::string current_path, std::string state_ref ) const
    {
        // first, replace all '/' into '\'.
        size_t pos = state_ref.find( "/" );
        while ( pos != std::string::npos )
        {
            state_ref.replace( pos, 1, "\\" );
            pos = state_ref.find( "/" );
        }

        // starts with '\\', so a root state.
        if ( state_ref.find( "\\\\" ) == 0 )
        {
            // just return that. No relative path business.
            return state_ref;
        }

        // starts with '.\'.
        if ( state_ref.find( ".\\" ) == 0 )
        {
            // remove the '.\'.
            state_ref = state_ref.substr( 2, state_ref.length() - 2 );

            // the state ref is relative to the current path.
            return current_path + std::string( "\\" ) + state_ref;
        }

        // append a '..\\' to it, so we index the parent folder.
        state_ref = std::string( "..\\" ) + state_ref;


        // find the first '..\'.
        pos = state_ref.find( "..\\" );

        // A whole bunch of '..\'.
        while ( pos == 0 )
        {
            // eat it up.
            state_ref = state_ref.substr( 3, state_ref.length() - 3 );

            // go down one folder on the path.
            size_t p = std::max( current_path.rfind( '\\' ), (size_t)2 );
            current_path = current_path.substr( 0, p );

            // next '..\\'.
            pos = state_ref.find( "..\\" );
        }

        return current_path + std::string( "\\" ) + state_ref;
    }

    bool StateMachineXmlLoader::validateSchema()
    {
        if (m_schema.m_initial_state == "")
        {
            logDebug(debug::LogLevel::Error, "starting transition is undefined.");
            return false;
        }

        for (auto& kt : m_schema.m_states)
        {
            schema::State& state = kt.second;

            for (auto& transition : state.m_transitions)
            {
                validateTransition(state, transition);
            }

            for (auto& exception : state.m_exceptions)
            {
                validateTransition(state, exception);
            }
        }
        return true;
    }

    bool StateMachineXmlLoader::validateTransition(const schema::State& parent_state, schema::Transition& transition)
    {
        std::string current_path = parent_state.m_fullname;

        std::string state_ref = transition.m_state;

        std::string next_state_fullname = evaluateStateFullNameFromRelativeName( current_path, state_ref );

        auto jt = m_schema.m_states.find( next_state_fullname );

        if ( jt == m_schema.m_states.end() )
        {
            auto it = m_schema.m_shortname_mappings.find(state_ref);

            if (it == m_schema.m_shortname_mappings.end())
            {
                logDebug(debug::LogLevel::Error, "<%s> transition('%s', '%s') evaluated to state <%s>, but cannot be found in the schema.",
                    parent_state.m_fullname.c_str(),
                    transition.m_event.c_str(),
                    transition.m_state.c_str(),
                    next_state_fullname.c_str());
            }
            else
            {
                const std::list<std::string>& states = it->second;

                next_state_fullname = states.front();

                if (states.size() != 1)
                {
                    if (states.empty())
                    {
                        logDebug(debug::LogLevel::Fatal, "<%s> transition('%s', '%s') evaluated to state <%s>, but no state found with that name.",
                            parent_state.m_fullname.c_str(),
                            transition.m_event.c_str(),
                            transition.m_state.c_str(),
                            next_state_fullname.c_str());
                    }
                    else
                    {
                        logDebug(debug::LogLevel::Error, "<%s> transition('%s', '%s') evaluated to state <%s>, but found multiple states with the name.",
                            parent_state.m_fullname.c_str(),
                            transition.m_event.c_str(),
                            transition.m_state.c_str(),
                            next_state_fullname.c_str());
                    }
                }
                else
                {
                    logDebug(debug::LogLevel::Trace, "<%s> transition('%s', '%s') evaluated next state to <%s>.",
                        parent_state.m_fullname.c_str(),
                        transition.m_event.c_str(),
                        transition.m_state.c_str(),
                        next_state_fullname.c_str());

                    // assign the full name.
                    transition.m_state = next_state_fullname;
                }
            }
        }
        else
        {
            logDebug( debug::LogLevel::Trace, "<%s> transition('%s', '%s') evaluated next state to <%s>.",
                parent_state.m_fullname.c_str(),
                transition.m_event.c_str(),
                transition.m_state.c_str(),
                next_state_fullname.c_str() );

            // assign the full name.
            transition.m_state = next_state_fullname;
        }
        return true;
    }

    std::string StateMachineXmlLoader::indentString( int indent ) const
    {
        std::string string;

        string.assign( indent * 2, ' ' );

        return string;
    }

    std::string trim(const std::string& str)
    {
        std::string result = str;

        auto it1 = std::find_if(result.rbegin(), result.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
        result.erase(it1.base(), result.end());

        auto it2 = std::find_if(result.begin(), result.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
        result.erase(result.begin(), it2);

        return result;
    }

    bool StateMachineXmlLoader::isValidStateName( const std::string& name ) const
    {
        bool contains_non_alpha = !std::regex_match( name, std::regex( "^[A-Za-z0-9_]+$" ) );

        if ( contains_non_alpha )
        {
            return false;
        }

        if ( name == "attribute" )
        {
            return false;
        }

        if ( name == "resource" )
        {
            return false;
        }

        if ( name == "transition" )
        {
            return false;
        }
        if (name == "catch_exception")
        {
            return false;
        }
        return true;
    }

    bool StateMachineXmlLoader::parseXmlNode( TiXmlNode* node, schema::State* state, int indent )
    {
        if ( !node )
            return true;

        int type = node->Type();

        std::ostringstream debug_stream;
        std::string indent_string = indentString( indent );

        schema::State* current_state = nullptr;

        switch ( type )
        {
        case TiXmlNode::TINYXML_DOCUMENT:
            {
                m_schema.m_filename = node->ToDocument()->ValueStr();
                debug_stream << indent_string << "State Machine [" << m_schema.m_filename << "]" << std::endl;

                break;
            }
        case TiXmlNode::TINYXML_ELEMENT:
            {
                TiXmlElement* element = node->ToElement();

                if ( element->ValueStr() == "attribute" )
                {
                    if ( state != nullptr )
                    {
                        for ( const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next() )
                        {
                            debug_stream << indent_string << " - attribute " << attr->NameTStr() << "='" << attr->ValueStr() << "'" << std::endl;

                            state->m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                        }
                    }
                }
                else if ( element->ValueStr() == "transition" )
                {
                    if ( state != nullptr )
                    {
                        const char* event_value = element->Attribute( "event" );
                        const char* next_state_value = element->Attribute( "state" );

                        if ( event_value != nullptr && next_state_value != nullptr )
                        {
                            debug_stream << indent_string << " - transition";

                            schema::Transition transition;
                            transition.m_event = event_value;
                            transition.m_state = next_state_value;

                            // add state attributes.
                            for (const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next())
                            {
                                debug_stream << " " << attr->NameTStr() << "='" << attr->ValueStr() << "'";

                                transition.m_attributes[attr->NameTStr()] = attr->ValueStr();
                            }
                            debug_stream << "." << std::endl;

                            state->m_transitions.push_back( transition );
                        }
                    }
                }
                else if ( element->ValueStr() == "resource" )
                {
                    if ( state != nullptr )
                    {
                        const char* resource_name = element->Attribute( "id" ); // single resource loaded.
                        const char* resources_name = element->Attribute("ids"); // single resource loaded.

                        if (resource_name != nullptr )
                        {
                            debug_stream << indent_string << " - resource";

                            schema::Resource resource;
                            resource.m_name = resource_name;

                            // add state attributes.
                            for ( const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next() )
                            {
                                debug_stream << " " << attr->NameTStr() << "='" << attr->ValueStr() << "'";

                                resource.m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                            }
                            debug_stream << "." << std::endl;


                            auto it = std::find_if(state->m_resources.begin(), state->m_resources.end(), [&resource](const schema::Resource& res)
                            {
                                return resource.m_name == res.m_name;
                            });

                            if (it == state->m_resources.end())
                            {
                                state->m_resources.push_back(resource);
                            }
                        }
                        else if (resources_name != nullptr)
                        {
                            debug_stream << indent_string << " - resources ids='" << resources_name << "'." << std::endl;
                            
                            std::regex pattern{ "\\,+" };
                            std::string str{ resources_name };

                            // Vector to store tokens
                            std::vector<std::string> tokens { std::sregex_token_iterator(str.begin(), str.end(), pattern, -1), std::sregex_token_iterator() };

                            for (auto token : tokens)
                            {
                                schema::Resource resource;
                                resource.m_name = trim(token);

                                auto it = std::find_if(state->m_resources.begin(), state->m_resources.end(), [&resource](const schema::Resource& res)
                                {
                                    return resource.m_name == res.m_name;
                                });

                                if (it == state->m_resources.end())
                                {
                                    state->m_resources.push_back(resource);
                                }
                            }
                        }
                    }
                }
                else if (element->ValueStr() == "catch_exception")
                {
                    if (state != nullptr)
                    {
                        const char* event_value = element->Attribute("event");
                        const char* next_state_value = element->Attribute("state");

                        if (event_value != nullptr && next_state_value != nullptr)
                        {
                            debug_stream << indent_string << " - catch_exception";

                            schema::Transition exception;
                            exception.m_event = event_value;
                            exception.m_state = next_state_value;
                            
                            // add state attributes.
                            for (const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next())
                            {
                                debug_stream << " " << attr->NameTStr() << "='" << attr->ValueStr() << "'";

                                exception.m_attributes[attr->NameTStr()] = attr->ValueStr();
                            }
                            debug_stream << "." << std::endl;

                            state->m_exceptions.push_back(exception);
                        }
                    }
                }
                else if ( !isValidStateName( element->ValueStr() ) )
                {
                    logDebug( debug::LogLevel::Error, "'%s' is not a valid state name.", element->ValueStr().c_str() );
                }
                else
                {
                    // create a new hierachical state.
                    schema::State new_state;
                    new_state.m_shortname = element->ValueStr();

                    // root state.
                    if ( state == nullptr )
                    {
                        new_state.m_fullname = std::string( "\\\\" ) + element->ValueStr();
                        m_schema.m_initial_state = new_state.m_fullname;

                    }
                    // child state.
                    else
                    {
                        new_state.m_fullname = state->m_fullname + std::string( "\\" ) + element->ValueStr();
                    }

                    m_schema.m_states[ new_state.m_fullname ] = new_state;

                    schema::State* parent_state = state;

                    state = &m_schema.m_states[ new_state.m_fullname ];

                    debug_stream << indent_string << "<" << new_state.m_shortname << ">" << std::endl;

                    if ( state != nullptr )
                    {
                        // add state attributes.
                        for ( const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next() )
                        {
                            debug_stream << indent_string << " - attribute " << attr->NameTStr() << "='" << attr->ValueStr() << "'" << std::endl;

                            state->m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                        }

                        if (parent_state != nullptr)
                        {
                            parent_state->m_children.push_back(state->m_fullname);

                            state->m_parent = parent_state->m_fullname;
                        }

                        m_schema.m_shortname_mappings[new_state.m_shortname].push_back(new_state.m_fullname);
                    }
                }
                break;
            }
        case TiXmlNode::TINYXML_COMMENT:
            {
                debug_stream << indent_string << " - COMMENT '" << node->ToComment()->Value() << "'." << std::endl;
                break;
            }
        case TiXmlNode::TINYXML_UNKNOWN:
            {
                debug_stream << indent_string << " - UNKNOWN '" << node->Value() << "'." << std::endl;
                break;
            }
        case TiXmlNode::TINYXML_TEXT:
            {
                debug_stream << indent_string << " - TEXT '" << node->ToText()->Value() << "'." << std::endl;
                break;
            }
        case TiXmlNode::TINYXML_DECLARATION:
            {
                debug_stream << indent_string << " - DECLARATION '" << node->ToDeclaration()->Value() << "'." << std::endl;
                break;
            }
        default:
            {
                break;
            }
        }

        logDebug( debug::LogLevel::Trace, debug_stream.str().c_str() );

        for ( TiXmlNode* child = node->FirstChild(); child != 0; child = child->NextSibling() )
        {
            if ( !parseXmlNode( child, state, indent + 1 ) )
            {
                return false;
            }
        }
        return true;
    }

    // generate the xml representation of the state machine schema.
    void StateMachineXmlLoader::generateXml( const std::string& xml_filename, const schema::StateMachine& schema )
    {
        m_schema = schema;

        // [TODO] IMPLEMENT ME.

    }
}
