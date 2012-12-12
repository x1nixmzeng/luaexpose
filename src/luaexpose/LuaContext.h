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

typedef lua_State* luaSandbox;

class LuaContext
{
	luaSandbox m_L;
	bool m_running;
public:
	LuaContext( );
	~LuaContext( );

	luaSandbox &fetchContext( ){ return( m_L ); }

	void init( );
	void destroy( );

	bool loadScript( const char * );
	bool run( );

	void setHook( const char *, lua_CFunction );
	void setClassHook( const char *, luaL_Reg * );

	void setGlobal( const char *, const char * );	// string
	void setGlobal( const char *, float );			// number
	void setGlobal( const char *, int );			// integer
	
	void pop( );
	void push( const string& );	// string
	void push( const char * );	// string
	void push( float );			// number
	void push( int );			// integer
	void push( );				// nil value

	void assertTable( );

	const char *errorString( );

	/*
		NOTE: This should only be called from hooked functions
	*/
	void assert( bool, const char * = nullptr );
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

// no need to use templates ..
#define LUA_CLASSCONSTRUCTOR(T) \
	T *lua_convertUserdataFor##T( LuaContext &luaContext ) \
	{ \
		return *(T **)luaL_checkudata( luaContext.fetchContext(), 1, "luaL_"#T ); \
	}

#endif
