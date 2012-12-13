/*
	LuaContext
	Written by x1nixmzeng

	Encapsulated Lua state to allow for C++ methods

	>> NOTE: Attempting to create static C functions for C++ classes using templates
*/
#ifndef _H_LUACONTEXT_
#define _H_LUACONTEXT_

#define LUA_COMPAT_ALL
#include "lua\lua.hpp"

#include <string>
using std::string;

class LuaContext
{
	lua_State* m_L;
	bool m_running;

public:
	LuaContext( );
	~LuaContext( );

	lua_State *getContext(){ return m_L; }
	inline bool hasInit( ) const;

	void init( );
	void destroy( );

	bool loadScript( const string & );
	bool loadScript( const char * );
	bool run( );

	void setHook( const string &, lua_CFunction );
	void setHook( const char *, lua_CFunction );
	void setClassHook( const string &, luaL_Reg * );
	void setClassHook( const char *, luaL_Reg * );

	void setGlobal( const string &, const char * );	// string
	void setGlobal( const char *, const char * );	// 
	void setGlobal( const string &, float );		// number
	void setGlobal( const char *, float );			// 
	void setGlobal( const string &, int );			// integer
	void setGlobal( const char *, int );			// 
	
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

/*
	LuaContext::setClassHook helpers

	Creates a static list of functions for the setClassHook function
	Example:
		LUA_CLASSDESCBEGIN(Vec2f)
			LUA_CLASSDESCADD("new", luaVec2fNew)
			LUA_CLASSDESCADD("__gc", luaVec2fDestroy)
		LUA_CLASSDESCEND

		LUA_CLASSDESCAPPLY(Vec2f)
*/

#define LUA_CLASSDESCBEGIN(c)	static luaL_Reg sLuaClass_##c[] = {
#define LUA_CLASSDESCEND		{NULL,NULL} };
#define LUA_CLASSDESCADD(n,c)	{ n, c },
#define LUA_CLASSDESCAPPLY(c)	g_lua.setClassHook(#c, sLuaClass_##c);

#endif
