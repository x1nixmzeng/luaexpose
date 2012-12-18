/*
	LuaContextTable
	Written by x1nixmzeng

	Encapsulated table creation
*/
#ifndef _H_LUACONTEXTTABLE_
#define _H_LUACONTEXTTABLE_

#include "LuaContextBase.h"

class LuaContextTable
	: public LuaContextBase
{
	int m_tableid;
	int m_maxele;
	int m_curele;
	LuaContextTable( ){}

	void pushNil(){}

public:
	LuaContextTable( lua_State *, int );
	~LuaContextTable( );

	void pushStr( const string & );
	void pushCStr( const char * );
	void pushNum( float );
	void pushInt( int );
};

#endif
