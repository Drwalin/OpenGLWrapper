/*
This source code is part of Integrated Libraries project

Copyright (C) 2018 Marek Zalewski

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*/

#ifndef STRING_CPP
#define STRING_CPP

#include "String.h"

inline char* String::c_str() const
{
	return data.begin();
}

inline char* String::begin() const
{
	return data.begin();
}

inline char* String::end() const
{
	return data.end()-1;
}

inline unsigned long long int String::size() const
{
	return data.size()-1;
}

inline char& String::operator [] ( const unsigned long long int id )
{
	return data[id];
}

inline String& String::Set( const char * src, const unsigned long long int size )
{
	data.resize( size+1 );
	memcpy( data.begin(), src, size*sizeof(char) );
	data.back() = 0;
	return *this;
}

inline String& String::operator = ( const String& src )
{
	data.resize( src.data.size() );
	memcpy( data.begin(), src.data.begin(), src.data.size() );
	return *this;
}

inline String& String::operator = ( const char * src )
{
	return Set( src, strlen(src) );
}

inline String& String::append( const char * src, const unsigned long long int size )
{
	data.insert( data.size()-1, src, src+size );
	return *this;
}

inline String& String::operator += ( const char * src )
{
	append( src, strlen(src) );
	return *this;
}

inline String& String::operator += ( const String& src )
{
	append( src.data.begin(), src.size() );
	return *this;
}

inline String& String::operator += ( const char src )
{
	data.insert( data.size()-1, src );
	return *this;
}

inline String String::add( const char * src, const unsigned long long int size ) const
{
	String dst;
	dst.data.resize( data.size()+size );
	memcpy( dst.data.begin(), data.begin(), data.size()-1 );
	memcpy( dst.data.begin()+data.size()-1, src, size );
	dst.data.back() = 0;
	return dst;
}

inline String String::operator + ( const char * src )
{
	return add( src, strlen(src) );
}

inline String String::operator + ( const String& src )
{
	return add( src.data.begin(), src.size() );
}

inline String String::operator + ( const char src ) const
{
	String dst;
	dst.data.resize( data.size()+1 );
	memcpy( dst.data.begin(), data.begin(), data.size()-1 );
	dst.data[dst.data.size()-2] = src;
	dst.data.back() = 0;
	return dst;
}

inline bool String::operator < ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) < 0;
}

inline bool String::operator <= ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) <= 0;
}

inline bool String::operator > ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) > 0;
}

inline bool String::operator >= ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) >= 0;
}

inline bool String::operator == ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) == 0;
}

inline bool String::operator != ( const String& src ) const
{
	return memcmp( data.begin(), src.data.begin(), data.size() < src.data.size() ? data.size() : src.data.size() ) != 0;
}

inline String& String::to_lowercase()
{
	char * it = data.begin();
	for( ; it < data.end(); ++it )
		if( *it >= 'A' && *it <= 'Z' )
			*it += 'a'-'A';
	return *this;
}

inline String& String::to_uppercase()
{
	char * it = data.begin();
	for( ; it < data.end(); ++it )
		if( *it >= 'a' && *it <= 'z' )
			*it -= 'a'-'A';
	return *this;
}

inline String String::get_lowercase() const
{
	String dst;
	dst.data.resize( data.size() );
	char * it = data.begin();
	char * itdst = dst.data.begin();
	for( ; it < data.end(); ++it, ++itdst )
	{
		if( *it >= 'A' && *it <= 'Z' )
			*itdst = *it + ('a'-'A');
		else
			*itdst = *it;
	}
	return dst;
}

inline String String::get_uppercase() const
{
	String dst;
	dst.data.resize( data.size() );
	char * it = data.begin();
	char * itdst = dst.data.begin();
	for( ; it < data.end(); ++it, ++itdst )
	{
		if( *it >= 'a' && *it <= 'z' )
			*itdst = *it - ('a'-'A');
		else
			*itdst = *it;
	}
	return dst;
}

String::String()
{
	data.resize( 1 );
	data.back() = 0;
}

String::String( const long long int src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%lld", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const unsigned long long int src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%llu", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const int src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%d", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const unsigned int src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%u", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const short src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%d", (int)src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const unsigned short src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%u", (unsigned int)src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const char src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%c", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const unsigned char src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%c", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const double src )
{
	data.resize( 64 );
	sprintf( data.begin(), "%f", src );
	data.resize( strlen(data.begin())+1 );
}

String::String( const char * src )
{
	unsigned long long int size = strlen(src);
	data.resize( size+1 );
	memcpy( data.begin(), src, data.size()*sizeof(char) );
}

#endif

