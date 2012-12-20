#include "LuaContextBase.h"

LuaContextBase::LuaContextBase( )
	: m_stackPos( 0 )
	, m_L( nullptr )
{}

LuaContextBase::LuaContextBase( lua_State* existingL )
	: m_stackPos( 0 )
	, m_L( existingL )
{}

void LuaContextBase::call( const char * strFunc )
{
	lua_getglobal( m_L, strFunc );
	lua_call( m_L, 0, 0 );
}

bool LuaContextBase::hasFunction( const char *strFunc )
{
	lua_getglobal(m_L, strFunc);
	return( lua_type( m_L, -1 ) == LUA_TFUNCTION );
}

int LuaContextBase::getGlobalInteger( const char *strVal )
{
	lua_getglobal( m_L, strVal );
	int r = lua_tointeger( m_L, -1 );
	pop();
	return r;
}

const char *LuaContextBase::getGlobalString( const char *strVal )
{
	lua_getglobal( m_L, strVal );
	const char *r = lua_tostring( m_L, -1 );
	pop();
	return r;
}

int LuaContextBase::countArguments( )
{
	return( lua_gettop( m_L ) );
}

const char *LuaContextBase::getCStr( int stackPos )
{
	// Get a string from a stack position

	return( luaL_checkstring( m_L, stackPos ) );
}

float LuaContextBase::getNum( int stackPos )
{
	// Get a number from a stack position

	return( static_cast<float>( luaL_checknumber( m_L, stackPos ) ) );
}

int LuaContextBase::getInt( int stackPos )
{
	// Get a integer from a stack position

	return( luaL_checkinteger( m_L, stackPos ) );
}

void LuaContextBase::pop( )
{
	// Remove an item from the stack

	lua_pop( m_L, 1 );
	--m_stackPos;
}

void LuaContextBase::pushStr( const string &strVal )
{
	// Add a string to the stack

	pushCStr( strVal.c_str() );
	++m_stackPos;
}

void LuaContextBase::pushCStr( const char *strVal )
{
	// Add a string to the stack

	lua_pushstring( m_L, strVal );
	++m_stackPos;
}

void LuaContextBase::pushNum( float fVal )
{
	// Add a number to the stack

	lua_pushnumber( m_L, fVal );
	++m_stackPos;
}

void LuaContextBase::pushInt( int iVal )
{
	// Add a integer to the stack

	lua_pushinteger( m_L, iVal );
	++m_stackPos;
};

void LuaContextBase::pushNil( )
{
	// Add a nil value to the stack

	lua_pushnil( m_L );
	++m_stackPos;
}

int LuaContextBase::pushedItemCount( ) const
{
	return( m_stackPos );
}

void LuaContextBase::assertString( )
{
	// Ensure the top of the stack contains a string

	luaL_checktype( m_L, 1, LUA_TSTRING);
}

void LuaContextBase::assertNumber( )
{
	// Ensure the top of the stack contains a number

	luaL_checktype( m_L, 1, LUA_TNUMBER);
}

void LuaContextBase::assertTable( )
{
	// Ensure the top of the stack contains a table

	luaL_checktype( m_L, 1, LUA_TTABLE);
}

const char *LuaContextBase::errorString( )
{
	// Get the last error

	return( getCStr( -1 ) );
}

void LuaContextBase::exception( const string &strError )
{
	exception( strError.c_str() );
}

void LuaContextBase::exception( const char *strError )
{
	// Raise an error

	pushStr( strError );
	lua_error( m_L );
}
