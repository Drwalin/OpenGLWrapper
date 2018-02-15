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

#ifndef STRING_H
#define STRING_H

#include "Array.cpp"
#include <cstdio>

class String
{
private:
	
	Array < char > data;
	
public:
	
	inline char* c_str() const;
	inline char* begin() const;
	inline char* end() const;
	
	inline unsigned long long int size() const;
	
	inline char& operator [] ( const unsigned long long int id );
	
	inline String& Set( const char * src, const unsigned long long int size );
	inline String& operator = ( const String& src );
	inline String& operator = ( const char * src );
	
	inline String& append( const char * src, const unsigned long long int size );
	inline String& operator += ( const char * src );
	inline String& operator += ( const String& src );
	inline String& operator += ( const char src );
	
	inline String add( const char * src, const unsigned long long int size ) const;
	inline String operator + ( const char * src );
	inline String operator + ( const String& src );
	inline String operator + ( const char src ) const;
	
	inline bool operator < ( const String& src ) const;
	inline bool operator <= ( const String& src ) const;
	inline bool operator > ( const String& src ) const;
	inline bool operator >= ( const String& src ) const;
	inline bool operator == ( const String& src ) const;
	inline bool operator != ( const String& src ) const;
	
	inline String& to_lowercase();
	inline String& to_uppercase();
	inline String get_lowercase() const;
	inline String get_uppercase() const;
	
	String();
	String( const long long int src );
	String( const unsigned long long int src );
	String( const int src );
	String( const unsigned int src );
	String( const short src );
	String( const unsigned short src );
	String( const char src );
	String( const unsigned char src );
	String( const double src );
	String( const char * src );
};

#endif

