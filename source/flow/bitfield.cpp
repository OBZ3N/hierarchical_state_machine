
#include "flow/bitfield.h"

Bitfield::Bitfield()
    : m_num_bits(0)
{}

Bitfield::~Bitfield()
{}

bool Bitfield::getBit( size_t position ) const
{
    if ( position >= m_num_bits )
        return false;

    size_t word_pos = position / 64;
    size_t bit_pos = position & 63;

    uint64_t val = m_bitfield[ word_pos ] & ( (uint64_t)1 << (uint64_t)bit_pos );
    return ( val != 0 );
}

void Bitfield::pushBit( bool value )
{
    setBit( m_num_bits, value );
}

void Bitfield::setBit( size_t position, bool value )
{
    size_t word_pos = position / 64;
    size_t bit_pos = position & 63;
        
    for ( size_t i = m_bitfield.size(); i <= word_pos; ++i )
    {
        uint64_t val = 0;
        m_bitfield.push_back( val );
    }

    if ( value )
    {
        m_bitfield[ word_pos ] |= ( (uint64_t)1 << (uint64_t)bit_pos );
    }
    else
    {
        m_bitfield[ word_pos ] &= ~( (uint64_t)1 << (uint64_t)bit_pos );
    }

    m_num_bits = std::max( m_num_bits, position+1 );
}

std::size_t Bitfield::gethash() const
{
    std::hash<uint64_t> hasher;

    size_t seed = 0;

    for ( auto word : m_bitfield )
    {
        seed ^= hasher( word ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
    }

    return seed;
}

bool Bitfield::operator == ( const Bitfield& rhs ) const
{
    size_t len = std::min( m_bitfield.size(), rhs.m_bitfield.size() );

    for ( size_t i = 0; i < len; ++i )
    {
        if ( m_bitfield[ i ] != rhs.m_bitfield[ i ] )
        {
            return false;
        }
    }

    for ( size_t i = len; i < m_bitfield.size(); ++i )
    {
        if ( m_bitfield[ i ] != 0 )
            return false;
    }

    for ( size_t i = len; i < rhs.m_bitfield.size(); ++i )
    {
        if ( rhs.m_bitfield[ i ] != 0 )
            return false;
    }

    return true;
}

Bitfield& Bitfield::operator = ( const Bitfield& rhs )
{
    m_bitfield = rhs.m_bitfield;

    m_num_bits = rhs.m_num_bits;

    return *this;
}

Bitfield& Bitfield::operator &= ( const Bitfield& rhs )
{
    size_t len = std::min( m_bitfield.size(), rhs.m_bitfield.size() );

    m_num_bits = std::max( m_num_bits, rhs.m_num_bits );

    for ( size_t i = 0; i < len; ++i )
    {
        m_bitfield[ i ] &= rhs.m_bitfield[ i ];
    }

    for ( size_t i = len; i < m_bitfield.size(); ++i )
    {
        m_bitfield[ i ] = 0;
    }
    return *this;
}

Bitfield& Bitfield::operator |= ( const Bitfield& rhs )
{
    size_t len = std::min( m_bitfield.size(), rhs.m_bitfield.size() );

    m_num_bits = std::max( m_num_bits, rhs.m_num_bits );

    for ( size_t i = 0; i < len; ++i )
    {
        m_bitfield[ i ] |= rhs.m_bitfield[ i ];
    }

    for ( size_t i = len; i < rhs.m_bitfield.size(); ++i )
    {
        m_bitfield.push_back( rhs.m_bitfield[ i ] );
    }
    return *this;
}

Bitfield& Bitfield::operator ^= ( const Bitfield& rhs )
{
    size_t len = std::min( m_bitfield.size(), rhs.m_bitfield.size() );

    m_num_bits = std::max( m_num_bits, rhs.m_num_bits );

    for ( size_t i = 0; i < len; ++i )
    {
        m_bitfield[ i ] ^= rhs.m_bitfield[ i ];
    }

    for ( size_t i = len; i < rhs.m_bitfield.size(); ++i )
    {
        m_bitfield.push_back( ~( rhs.m_bitfield[ i ] ) );
    }
    return *this;
}

Bitfield Bitfield::operator ~ () const
{
    Bitfield temp;

    temp.m_num_bits = m_num_bits;

    for ( size_t i = 0; i < m_bitfield.size(); ++i )
    {
        temp.m_bitfield.push_back( ~( m_bitfield[ i ] ) );
    }

    return temp;
}
