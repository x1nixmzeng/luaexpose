#include "LuaContextBase.h"

LuaContextBase::LuaContextBase( )
	: m_L( nullptr )
{}

LuaContextBase::LuaContextBase( lua_State* existingL )
	: m_L( existingL )
{}

const char *LuaContextBase::getStringFromStack( int stackPos )
{
	// Get a string from a stack position

	return( lua_tostring( m_L, stackPos ) );
}

float LuaContextBase::getNumberFromStack( int stackPos )
{
	// Get a number from a stack position

	return( static_cast<float>( lua_tonumber( m_L, stackPos ) ) );
}

int LuaContextBase::getIntegerFromStack( int stackPos )
{
	// Get a integer from a stack position

	return( lua_tointeger( m_L, stackPos ) );
}

void LuaContextBase::pop( )
{
	// Remove an item from the stack

	lua_pop( m_L, 1 );
}

void LuaContextBase::push( const string &strVal )
{
	// Add a string to the stack

	push( strVal.c_str() );
}

void LuaContextBase::push( const char *strVal )
{
	// Add a string to the stack

	lua_pushstring( m_L, strVal );
}

void LuaContextBase::push( float fVal )
{
	// Add a number to the stack

	lua_pushnumber( m_L, fVal );
}

void LuaContextBase::push( int iVal )
{
	// Add a integer to the stack

	lua_pushinteger( m_L, iVal );
};

void LuaContextBase::push( )
{
	// Add a nil value to the stack

	lua_pushnil( m_L );
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

	return( getStringFromStack( -1 ) );
}

void LuaContextBase::exception( const string &strError )
{
	exception( strError.c_str() );
}

void LuaContextBase::exception( const char *strError )
{
	// Raise an error

	push( strError );
	lua_error( m_L );
}
