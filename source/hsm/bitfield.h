#pragma once

#include <vector>
#include <cstdint>
#include<algorithm>

class Bitfield
{
public:
    Bitfield();
    ~Bitfield();

    bool getBit( size_t position ) const;
    size_t getNumBits() const { return m_num_bits; }

    void pushBit( bool value );
    void setBit( size_t position, bool value );

    std::size_t gethash() const;

    bool operator == ( const Bitfield& rhs ) const;
    bool operator != ( const Bitfield& rhs ) const { return ( ( *this ) == rhs ); }

    Bitfield& operator = ( const Bitfield& rhs );
    Bitfield& operator &= ( const Bitfield& rhs );
    Bitfield& operator |= ( const Bitfield& rhs );
    Bitfield& operator ^= ( const Bitfield& rhs );

    Bitfield operator & ( const Bitfield& rhs ) const { Bitfield temp = (*this); return temp &= rhs; }
    Bitfield operator | ( const Bitfield& rhs ) const { Bitfield temp = (*this); return temp |= rhs; }
    Bitfield operator ^ ( const Bitfield& rhs ) const { Bitfield temp = (*this); return temp ^= rhs; }
    Bitfield operator ~ () const;

private:
    std::vector<uint64_t> m_bitfield;
    size_t m_num_bits;
};

namespace std
{
    template<>
    struct hash<Bitfield>
    {
        size_t operator()( const Bitfield & obj ) const
        {
            return obj.gethash();
        }
    };
}