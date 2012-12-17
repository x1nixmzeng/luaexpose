/*
	LuaContextBase
	Written by x1nixmzeng

	Base encapsulated for minimum manipulation
*/
#ifndef _H_LUACONTEXTBASE_
#define _H_LUACONTEXTBASE_

#define LUA_COMPAT_ALL
#include "lua\lua.hpp"

#include <string>
using std::string;

#include <vector>
using std::vector;

class LuaContextBase
{
protected:
	lua_State* m_L;
public:
	LuaContextBase( );
	LuaContextBase( lua_State* );

	//bool hasGlobal( const string & );
	//bool hasGlobal( const char * );

	int countArguments( );

	const char *getStringFromStack( int );
	float getNumberFromStack( int );
	int getIntegerFromStack( int );

	//const char *popString( );

	void call( const char * );
	int getGlobalInteger( const char * );
	const char *getGlobalString( const char * );

	void pop( );

	void push( const string& );	// string
	void push( const char * );	// string
	void push( float );			// number
	void push( int );			// integer
	void push( );				// nil

	int createTable( int elements )
	{
		lua_createtable( m_L, elements, 0 );
		return lua_gettop( m_L );
	}

	void pushTableInteger( int value, int table, int &index )
	{
		push( value );
		lua_rawseti( m_L, table, index++ );
	}

	void pushTableNumber( float value, int table, int &index )
	{
		push( value );
		lua_rawseti( m_L, table, index++ );
	}

	// -- NEW push vector of type T as a table
	template<class T> void push( const vector<T > &pool ) // table
	{
		if( pool.size() > 0 )
		{
			lua_createtable( m_L, pool.size(), 0 );
			int newTable = lua_gettop( m_L );
			int index = 1;

			for( vector<T >::const_iterator cit( pool.begin() ); cit != pool.end(); ++cit )
			{
				push( *cit );
				lua_rawseti( m_L, newTable, index++ );
			}
		}
	}
	// -- end

	void assertString( );
	void assertNumber( );
	void assertTable( );

	const char *errorString( );

	void exception( const string & );
	void exception( const char * );
};

#endif
