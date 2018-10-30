
#include <algorithm>
#include <regex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdarg.h>
#include "TinyXML/tinyxml.h"
#include "flow/debug.h"
#include "flow/state_machine_xml_loader.h"

namespace flow
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
    std::string StateMachineXmlLoader::evaluateStateFullNameFromReference( std::string current_path, std::string state_ref ) const
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
        if ( m_schema.m_transition_start_state_machine.m_next_state == "" )
        {
            logDebug( debug::LogLevel::Error, "starting transition is undefined." );
            return false;
        }

        for ( auto& kt : m_schema.m_states )
        {
            schema::State& state = kt.second;

            std::string current_path = state.m_fullname;

            for ( auto& transition : state.m_transitions )
            {
                std::string state_ref = transition.m_next_state;

                std::string next_state_fullname = evaluateStateFullNameFromReference( current_path, state_ref );

                auto jt = m_schema.m_states.find( next_state_fullname );

                if ( jt == m_schema.m_states.end() )
                {
                    logDebug( debug::LogLevel::Error, "<%s> transition('%s', '%s') evaluated to state <%s>, but cannot be found in the schema.",
                        state.m_fullname.c_str(),
                        transition.m_event_name.c_str(),
                        transition.m_next_state.c_str(),
                        next_state_fullname.c_str() );
                }
                else
                {
                    logDebug( debug::LogLevel::Trace, "<%s> transition('%s', '%s') evaluated next state to <%s>.",
                        state.m_fullname.c_str(),
                        transition.m_event_name.c_str(),
                        transition.m_next_state.c_str(),
                        next_state_fullname.c_str() );

                    // assign the full name.
                    transition.m_next_state = next_state_fullname;
                }
            }
        }
        return true;
    }

    std::string StateMachineXmlLoader::indentString( int indent ) const
    {
        std::string string;

        string.assign( indent * 2, ' ' );

        return string;
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

        if ( name == "asset" )
        {
            return false;
        }

        if ( name == "transition" )
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
                            debug_stream << indent_string << " - attribute " << attr->NameTStr() << " = '" << attr->ValueStr() << "'" << std::endl;

                            state->m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                        }
                    }
                }
                else if ( element->ValueStr() == "transition" )
                {
                    if ( state != nullptr )
                    {
                        const char* event_value = element->Attribute( "event" );
                        const char* next_state_value = element->Attribute( "next_state" );

                        if ( event_value != nullptr && next_state_value != nullptr )
                        {
                            debug_stream << indent_string << " - transition " << event_value << " ----> <" << next_state_value << ">" << std::endl;

                            schema::Transition transition;
                            transition.m_event_name = event_value;
                            transition.m_next_state = next_state_value;

                            state->m_transitions.push_back( transition );
                        }
                    }
                }
                else if ( element->ValueStr() == "asset" )
                {
                    if ( state != nullptr )
                    {
                        const char* asset_name = element->Attribute( "id" ); // single asset loaded.

                        if ( asset_name != nullptr )
                        {
                            debug_stream << indent_string << " - asset";

                            schema::Asset asset;
                            asset.m_asset_name = asset_name;
                            asset.m_state_name = state->m_fullname;

                            // add state attributes.
                            for ( const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next() )
                            {
                                debug_stream << " " << attr->NameTStr() << " = '" << attr->ValueStr() << "'";

                                asset.m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                            }
                            debug_stream << "." << std::endl;
                            state->m_assets.push_back( asset );
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

                        m_schema.m_transition_start_state_machine.m_event_name = "start_state_machine";
                        m_schema.m_transition_start_state_machine.m_next_state = new_state.m_fullname;
                    }
                    // child state.
                    else
                    {
                        new_state.m_fullname = state->m_fullname + std::string( "\\" ) + element->ValueStr();
                    }

                    m_schema.m_states[ new_state.m_fullname ] = new_state;

                    state = &m_schema.m_states[ new_state.m_fullname ];

                    debug_stream << indent_string << "<" << new_state.m_fullname << ">" << std::endl;

                    if ( state != nullptr )
                    {
                        // add state attributes.
                        for ( const TiXmlAttribute* attr = element->FirstAttribute(); attr != nullptr; attr = attr->Next() )
                        {
                            debug_stream << indent_string << " - attribute " << attr->NameTStr() << " = '" << attr->ValueStr() << "'" << std::endl;

                            state->m_attributes[ attr->NameTStr() ] = attr->ValueStr();
                        }
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
