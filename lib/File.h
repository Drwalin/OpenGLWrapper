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

#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include "String.h"

class File
{
private:
	
	FILE * file;
	String filename;
	
public:
	
	inline bool open( const char * filename, const char * format );
	inline void close();
	inline bool good();
	inline unsigned long long int getfilesize();
	inline void setpos( const unsigned long long int pos );
	inline unsigned long long int getpos() const;
	
	template < class T >
	inline unsigned long long int read( T * data, const unsigned long long int maxlen )
	{
		if( file )
		{
			memset( data, 0, maxlen*sizeof(T) );
			return (unsigned long long int)fread( data, sizeof(T), maxlen, file );
		}
		return 0;
	}
	
	template < class T >
	inline bool write( const T * data, const unsigned long long int count )
	{
		if( file )
		{
			fwrite( data, sizeof(T), count, file );
		}
		return true;
	}
	
	inline bool write( const String& str );
	void getline( String& dst );
	void getword( String& dst );
	inline bool eof() const;
	double getfloat();
	long long int getll();
	unsigned long long int getull();
	inline char getchar();
	inline void putchar( const char src );
	File();
	File( const char * filename, const char * format );
	~File();
};

#endif

