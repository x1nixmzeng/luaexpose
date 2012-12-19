/*
	LuaContextBase
	Written by x1nixmzeng

	Base encapsulated for minimum manipulation
*/
#ifndef _H_LUACONTEXTBASE_
#define _H_LUACONTEXTBASE_

#define LUA_COMPAT_ALL
#include "lua/lua.hpp"

#include <string>
#include <vector>
#include <iterator>

using namespace std;

class LuaContextBase
{
protected:
	lua_State* m_L;
public:
	LuaContextBase( );
	LuaContextBase( lua_State* );

	int countArguments( );

	// -- Get items from the stack
	const char *getCStr( int );
	float getNum( int );
	int getInt( int );

	// -- Remove item from the stack
	void pop( );

	// -- 
	void call( const char * );
	bool hasFunction( const char * );

	// -- Get global values (must exist)
	int getGlobalInteger( const char * );
	const char *getGlobalString( const char * );

	// -- Push item to the stack
	void pushStr( const string& );
	void pushCStr( const char * );
	void pushNum( float );
	void pushInt( int );
	void pushNil( );

	// -- Table routines (superseeded - see LuaContextTable)
	//int createTable( int elements );
	//void pushTableInteger( int, int, int & );
	//void pushTableNumber( float, int, int & );

	// -- Type asserts (could return values)
	void assertString( );
	void assertNumber( );
	void assertTable( );

	// -- Error routines
	const char *errorString( );

	void exception( const string & );
	void exception( const char * );
};

#endif
