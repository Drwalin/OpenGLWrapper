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

#ifndef MAP_CPP
#define MAP_CPP

#include "Array.cpp"

template < class Tkey, class Tval >
class Map
{
private:
	
	class KeyValPair
	{
	public:
		Tkey key;
		Tval val;
		KeyValPair()
		{
			key = Tkey();
			val = Tval();
		}
		KeyValPair( const Tkey& key, const Tval& val )
		{
			this->key = (Tkey)key;
			this->val = (Tval)val;
		}
		KeyValPair( const Tkey& key )
		{
			this->key = (Tkey)key;
			this->val = Tval();
		}
	};
	
	Array < KeyValPair > data;
	KeyValPair defaultPair;
	
public:
	
	inline long long int size() const
	{
		return data.size();
	}
	
	inline KeyValPair * begin() const
	{
		return data.begin();
	}
	
	inline KeyValPair * end() const
	{
		return data.end();
	}
	
	inline Map& operator = ( const Map < Tkey, Tval > & src )
	{
		defaultPair = src.defaultPair;
		data = src.data;
	}
	
	inline KeyValPair * find( const Tkey& key ) const
	{
		if( data.size() == 0 )
		{
			return NULL;
		}
		
		long long int pos = data.size() >> 1;
		long long int move = data.size() >> 2;
		
		while( true )
		{
			if( key == (data.begin()+pos)->key )
			{
				return data.begin()+pos;
			}
			else if( move == 0 )
			{
				if( key < (data.begin()+pos)->key )
				{
					while( key < (data.begin()+pos)->key )
					{
						--pos;
						if( pos < 0 )
						return NULL;
					}
					if( key == (data.begin()+pos)->key )
						return data.begin()+pos;
					else
						return NULL;
				}
				else
				{
					while( key > (data.begin()+pos)->key )
					{
						++pos;
						if( pos >= data.size() )
							return NULL;
					}
					if( key == (data.begin()+pos)->key )
						return data.begin()+pos;
					else
						return NULL;
				}
			}
			else
			{
				if( key < (data.begin()+pos)->key )
				{
					pos -= move;
					move >>= 1;
				}
				else
				{
					pos += move;
					move >>= 1;
				}
			}
		}
		return NULL;
	}
	
	inline void insert( const Tkey& key, const Tval& val )
	{
		if( data.size() == 0 )
		{
			data.push_back( KeyValPair( key, val ) );
			return;
		}
		
		long long int pos = data.size() >> 1;
		long long int move = data.size() >> 2;
		
		while( true )
		{
			if( key == (data.begin()+pos)->key )
			{
				(data.begin()+pos)->val = val;
				return;
			}
			else if( move == 0 )
			{
				if( key < (data.begin()+pos)->key )
				{
					while( key < (data.begin()+pos)->key )
					{
						--pos;
						if( pos < 0 )
						{
							data.insert( 0, KeyValPair( key, val ) );
							return;
						}
					}
					if( key == (data.begin()+pos)->key )
					{
						(data.begin()+pos)->val = val;
						return;
					}
					else
					{
						data.insert( pos, KeyValPair( key, val ) );
						return;
					}
				}
				else
				{
					while( key > (data.begin()+pos)->key )
					{
						++pos;
						if( pos >= data.size() )
						{
							data.insert( data.size(), KeyValPair( key, val ) );
							return;
						}
					}
					if( key == (data.begin()+pos)->key )
					{
						(data.begin()+pos)->val = val;
						return;
					}
					else
					{
						data.insert( pos, KeyValPair( key, val ) );
						return;
					}
				}
			}
			else
			{
				if( key < (data.begin()+pos)->key )
				{
					pos -= move;
					move >>= 1;
				}
				else
				{
					pos += move;
					move >>= 1;
				}
			}
		}
	}
	
	void insert( const Map < Tkey, Tval > & src )		// if keys are equal - left this->...->key
	{
		if( data.size() == 0 )
		{
			data = src.data;
			return;
		}
		
		int i = 0, j = 0, k = 0;
		
		while( true )
		{
			if( j >= src.data.size() )
				return;
			if( src.data[j].key < data[i].key )
			{
				k = j;
				while( true )
				{
					if( j > src.data.size() )
						return;
					if( j == src.data.size() )
						break;
					if( src.data[j].key >= data[i].key )
						break;
					++j;
				}
				if( k < j )
					data.insert( i, src.data.begin()+k, src.data.begin()+j );
			}
			else if( src.data[j].key == data[i].key )
			{
				while( j < src.data.size() && i < data.size() )
				{
					if( src.data[j].key != data[i].key )
						break;
					++i;
					++j;
				}
			}
			else if( i < data.size() )
			{
				++i;
				while( i < data.size() )
				{
					if( src.data[j].key <= data[i].key )
						break;
					++i;
				}
			}
			else
			{
				data.insert( data.size(), src.data.begin()+j, src.data.end() );
				return;
			}
		}
	}
	
	inline Tval& operator [] ( const Tkey& key )
	{
		if( data.size() == 0 )
		{
			data.push_back( KeyValPair( key ) );
			return data.back().val;
		}
		
		long long int pos = data.size() >> 1;
		long long int move = data.size() >> 2;
		
		while( true )
		{
			if( key == (data.begin()+pos)->key )
			{
				return (data.begin()+pos)->val;
			}
			else if( move == 0 )
			{
				if( key < (data.begin()+pos)->key )
				{
					while( key < (data.begin()+pos)->key )
					{
						--pos;
						if( pos < 0 )
						{
							data.insert( 0, KeyValPair( key ) );
							return data.front().val;
						}
					}
					if( key == (data.begin()+pos)->key )
					{
						return (data.begin()+pos)->val;
					}
					else
					{
						data.insert( pos, KeyValPair( key ) );
						return (data.begin()+pos)->val;
					}
				}
				else
				{
					while( key > (data.begin()+pos)->key )
					{
						++pos;
						if( pos >= data.size() )
						{
							data.insert( data.size(), KeyValPair( key ) );
							return data.back().val;
						}
					}
					if( key == (data.begin()+pos)->key )
					{
						return (data.begin()+pos)->val;
					}
					else
					{
						data.insert( pos, KeyValPair( key ) );
						return (data.begin()+pos)->val;
					}
				}
			}
			else
			{
				if( key < (data.begin()+pos)->key )
				{
					pos -= move;
					move >>= 1;
				}
				else
				{
					pos += move;
					move >>= 1;
				}
			}
		}
		return defaultPair.val;
	}
	
	inline KeyValPair * getByID( const long long int id )
	{
		if( id < 0 || id >= data.size() )
			return &defaultPair;
		return data[id];
	}
	
	inline void reserve_mem( long long int size )
	{
		data.reserve_mem( size );
	}
	
	inline void shrink_to_fit()
	{
		data.shrink_to_fit();
	}
	
	inline void erase( const KeyValPair * ptr )
	{
		if( ptr )
		{
			data.erase( ptr, ptr+1 );
		}
	}
	
	inline void erase( const Tkey& key )
	{
		erase( find( key ) );
	}
	
	inline void clear()
	{
		data.clear();
	}
	
	Map()
	{
		data.clear();
		defaultPair = KeyValPair();
	}
	
	Map( const int src )
		: data(src)
	{
	}
};

#endif

