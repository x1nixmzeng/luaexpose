/*
	LuaContext
	Written by x1nixmzeng

	Additional encapsulation to aid creating a new context
*/
#ifndef _H_LUACONTEXT_
#define _H_LUACONTEXT_

#include "LuaContextBase.h"

class LuaContext
	: public LuaContextBase
{
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
};

/*
	Bind a public class function member to a Lua context
		(to the effect of creating a static run-time method)

	LIMITATIONS
		The class instance must be static/global
		The target function must be templated
			(which gives it a compile-time parameter to the class)
*/

#define LUA_BINDCLASSMETHOD(lua,c,cfunc,cinst) \
	lua.setHook( #cfunc, c::cfunc<cinst > )

#endif
