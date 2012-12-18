#include "LuaContextTable.h"

LuaContextTable::LuaContextTable( lua_State* existingL, int elements )
	: LuaContextBase( existingL )
	, m_maxele( elements )
	, m_curele( 1 )
{
	lua_createtable( m_L, m_maxele, 0 );
	m_tableid = lua_gettop( m_L );
}

LuaContextTable::~LuaContextTable( )
{}

void LuaContextTable::pushStr( const string &strVal )
{
	// TEST THIS CALLS LuaContextTable::pushCStr
	pushCStr( strVal.c_str() );
}

void LuaContextTable::pushCStr( const char *strVal )
{
	if( m_curele <= m_maxele )
	{
		LuaContextBase::pushCStr( strVal );
		lua_rawseti( m_L, m_tableid, m_curele++ );
	}
}

void LuaContextTable::pushNum( float fVal )
{
	if( m_curele <= m_maxele )
	{
		LuaContextBase::pushNum( fVal );
		lua_rawseti( m_L, m_tableid, m_curele++ );
	}
}

void LuaContextTable::pushInt( int iVal )
{
	if( m_curele <= m_maxele )
	{
		LuaContextBase::pushInt( iVal );
		lua_rawseti( m_L, m_tableid, m_curele++ );
	}
}
