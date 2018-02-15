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

#ifndef ARRAY_CPP
#define ARRAY_CPP

#include <cstring>
#include <cstdlib>

template < class T >
class Array
{
private:
	T * ptr;
	unsigned long long int ptrsize;
	unsigned long long int fullsize;		// includes reserved memory
	int ARRAY_SHIFT_BIT_FULL_SIZE;
	
public:
	
	inline unsigned long long int size() const
	{
		return ptrsize;
	}
	
	inline void operator = ( const Array < T > & src )
	{
		resize( src.size() );
		memcpy( ptr, src.ptr, ptrsize*sizeof(T) );
	}
	
	inline T * begin() const
	{
		return ptr;
	}
	
	inline T * end() const
	{
		return ptr+ptrsize;
	}
	
	inline T& operator[]( const unsigned long long int id )
	{
		return *(ptr+id);
	}
	
	inline T operator[]( const unsigned long long int id ) const
	{
		return *(ptr+id);
	}
	
	inline T& at( const unsigned long long int id )
	{
		return *(ptr+id);
	}
	
	inline void insert( unsigned long long int point, const T& src )
	{
		unsigned long long int bsize = ptrsize - point;
		resize( ptrsize + 1 );
		
		memmove( ptr + point + 1, ptr + point, bsize * sizeof(T) );
		ptr[point] = src;
	}
	
	inline void insert( unsigned long long int point, const T * beg, const T * end )
	{
		unsigned long long int bsize = ptrsize - point;
		resize( ptrsize + end - beg );
		
		memmove( ptr + point + (end - beg), ptr + point, bsize * sizeof(T) );
		memcpy( ptr + point, beg, (end - beg) * sizeof(T) );
	}
	
	inline void erase( unsigned long long int beg, unsigned long long int end )
	{
		if( end < beg )
		{
			unsigned long long int temp = beg;
			beg = end;
			end = temp;
		}
		
		if( end < ptrsize )
		{
			memmove( ptr + beg, ptr + end, (ptrsize - end) * sizeof(T) );
			resize( beg + ( ptrsize - end ) );
		}
		else
		{
			resize( beg );
		}
	}
	
	inline void erase( const T * beg, const T * end )
	{
		erase( (unsigned long long int)(beg-begin()), (unsigned long long int)(end-begin()) );
	}
	
	inline T & front()
	{
		return *ptr;
	}
	
	inline T & back()
	{
		if( ptrsize )
			return *(ptr+ptrsize-1);
		return *ptr;
	}
	
	inline void push_back( const T& src )
	{
		resize( ptrsize + 1 );
		ptr[ptrsize-1] = src;
	}
	
	inline void push_front( const T& src )
	{
		insert( 0, src );
	}
	
	inline T pop_back()
	{
		T dst;
		dst = back();
		reszie( size()-1 );
		return dst;
	}
	
	inline T pop_front()
	{
		T dst;
		dst = front();
		erase( 0, 1 );
		return dst;
	}
	
	inline void shrink_to_fit()
	{
		unsigned long long int temp = ( ( ptrsize >> ARRAY_SHIFT_BIT_FULL_SIZE ) + 1 ) << ARRAY_SHIFT_BIT_FULL_SIZE;
		if( temp < fullsize )
		{
			fullsize = temp;
			ptr = (T*) realloc( ptr, fullsize * sizeof(T) );
		}
	}
	
	inline void resize( const unsigned long long int size )
	{
		unsigned long long int temp = ( ( size >> ARRAY_SHIFT_BIT_FULL_SIZE ) + 1 ) << ARRAY_SHIFT_BIT_FULL_SIZE;
		
		if( temp > fullsize )
		{
			fullsize = temp;
			ptr = (T*) realloc( ptr, fullsize * sizeof(T) );
		}
		this->ptrsize = size;
	}
	
	inline void clear()
	{
		ptr = (T*) realloc( ptr, sizeof(T) << ARRAY_SHIFT_BIT_FULL_SIZE );
		ptrsize = 0;
		fullsize = 1 << ARRAY_SHIFT_BIT_FULL_SIZE;
	}
	
	inline unsigned long long int capacity() const
	{
		return fullsize;
	}
	
	inline void reserve_mem( unsigned long long int size /*size>ptrsize*/ )
	{
		unsigned long long int temp = ( ( size >> ARRAY_SHIFT_BIT_FULL_SIZE ) + 1 ) << ARRAY_SHIFT_BIT_FULL_SIZE;
		if( temp > fullsize )
		{
			fullsize = temp;
			ptr = (T*) realloc( ptr, fullsize * sizeof(T) );
		}
	}
	
	Array()
	{
		ARRAY_SHIFT_BIT_FULL_SIZE = 10;
		ptr = (T*) malloc( ( 1 << ARRAY_SHIFT_BIT_FULL_SIZE ) * sizeof(T) );
		ptrsize = 0;
		fullsize = 1 << ARRAY_SHIFT_BIT_FULL_SIZE;
	}
	
	Array( const int src )
	{
		ARRAY_SHIFT_BIT_FULL_SIZE = src;
		ptr = (T*) malloc( ( 1 << ARRAY_SHIFT_BIT_FULL_SIZE ) * sizeof(T) );
		ptrsize = 0;
		fullsize = 1 << ARRAY_SHIFT_BIT_FULL_SIZE;
	}
	
	~Array()
	{
		if( ptr )
			free( ptr );
		ptr = NULL;
		ptrsize = 0;
		fullsize = 0;
	}
};

#endif

