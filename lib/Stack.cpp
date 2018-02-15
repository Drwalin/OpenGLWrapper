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

#ifndef STACK_CPP
#define STACK_CPP

#include "Array.cpp"

template < class T >
class Stack
{
private:
	
	Array < T > data;
	
public:
	
	inline T * begin() const
	{
		return data.begin();
	}
	
	inline T * end() const
	{
		return data.end();
	}
	
	inline long long int size() const
	{
		return data.size();
	}
	
	inline T& operator = ( const Stack < T > & src )
	{
		data = src.data;
	}
	
	inline T& operator [] ( const unsigned long long int id )
	{
		return data[id];
	}
	
	inline void push( const T& src )
	{
		data.push_back( src );
	}
	
	inline T pop()
	{
		T dst;
		dst = data.back();
		if( data.size() )
			data.resize( data.size()-1 );
		return dst;
	}
	
	inline T& front()
	{
		return data.front();
	}
	
	inline T& back()
	{
		return data.back();
	}
	
	inline void reserve_mem( const unsigned long long int size )
	{
		data.reserve_mem( size );
	}
	
	inline void shrink_to_fit()
	{
		data.shrink_to_fit();
	}
	
	inline void clear()
	{
		data.clear();
	}
	
	Stack()
	{
		data.clear();
	}
	
	Stack( const int src )
		: data(src)
	{
	}
};

#endif

