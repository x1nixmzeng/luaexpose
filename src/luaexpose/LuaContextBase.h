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

class LuaContextBase
{
protected:
	lua_State* m_L;
public:
	LuaContextBase( );
	LuaContextBase( lua_State* );

	const char *getStringFromStack( int );
	float getNumberFromStack( int );
	int getIntegerFromStack( int );

	const char *popString( );

	void pop( );

	void push( const string& );	// string
	void push( const char * );	// string
	void push( float );			// number
	void push( int );			// integer
	void push( );				// nil

	void assertString( );
	void assertNumber( );
	void assertTable( );

	const char *errorString( );

	void exception( const string & );
	void exception( const char * );
};

#endif
