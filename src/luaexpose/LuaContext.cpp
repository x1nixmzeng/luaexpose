#include "LuaContext.h"

LuaContext::LuaContext( )
	: m_running( false )
{}

LuaContext::~LuaContext( )
{}

bool LuaContext::hasInit( ) const
{
	return( !( m_L == nullptr ) );
}

void LuaContext::init()
{
	// Create new state and open standard libraries

	m_L = luaL_newstate();
	luaL_openlibs( m_L ) ;
}

void LuaContext::destroy()
{
	// Close the member state

	lua_close( m_L );
}

bool LuaContext::loadScript( const string &strFilename )
{
	return( loadScript( strFilename.c_str() ) );
}

bool LuaContext::loadScript( const char *strFilename )
{
	// Attempt to load file

	return( luaL_loadfile( m_L , strFilename ) == 0 );
}

bool LuaContext::run( )
{
	// Attempt to execute global scope

	m_running = true;
	
	if( lua_pcall( m_L, 0, 0, 0 ) == 0 )
	{
		m_running = false;
		return( true );
	}
	
	m_running = false;
	return( false );
}

void LuaContext::setHook( const string &strFunction, lua_CFunction luaCallback )
{
	setHook( strFunction.c_str(), luaCallback );
}

void LuaContext::setHook( const char *strFunction, lua_CFunction luaCallback )
{
	// Register a C function to Lua

	lua_register( m_L, strFunction, luaCallback );
}

void LuaContext::setClassHook( const string &strClass, luaL_Reg *funcList )
{
	setClassHook( strClass.c_str(), funcList );
}

void LuaContext::setClassHook( const char *strClass, luaL_Reg *funcList )
{
	// Register a function list as a Lua class

	string strBaseName;
		
	strBaseName = "LuaL_";
	strBaseName += strClass;

	luaL_newmetatable( m_L, strBaseName.c_str() );
	luaL_register( m_L, 0, funcList );
	lua_pushvalue( m_L, -1 );
	lua_setfield( m_L, -1, "__index" );
	lua_setglobal( m_L, strClass );
}

void LuaContext::setGlobal( const string &strName, const char *strVal )
{
	setGlobal( strName.c_str(), strVal );
}

void LuaContext::setGlobal( const char *strName, const char *strVal )
{
	// Set a global string

	push( strVal );
	lua_setglobal( m_L, strName );
}

void LuaContext::setGlobal( const string &strName, float fVal )
{
	setGlobal( strName.c_str(), fVal );
}

void LuaContext::setGlobal( const char *strName, float fVal )
{
	// Set a global number

	push( fVal );
	lua_setglobal( m_L, strName );
}

void LuaContext::setGlobal( const string &strName, int iVal )
{
	setGlobal( strName.c_str(), iVal );
}

void LuaContext::setGlobal( const char *strName, int iVal )
{
	// Set a global integer

	push( iVal );
	lua_setglobal( m_L, strName );
}

void LuaContext::pop( )
{
	// Remove an item from the stack

	lua_pop( m_L, 1 );
}

void LuaContext::push( const string &strVal )
{
	// Add a string to the stack

	push( strVal.c_str() );
}

void LuaContext::push( const char *strVal )
{
	// Add a string to the stack

	lua_pushstring( m_L, strVal );
}

void LuaContext::push( float fVal )
{
	// Add a number to the stack

	lua_pushnumber( m_L, fVal );
}

void LuaContext::push( int iVal )
{
	// Add a integer to the stack

	lua_pushinteger( m_L, iVal );
};

void LuaContext::push( )
{
	// Add a nil value to the stack

	lua_pushnil( m_L );
}

void LuaContext::assertString( )
{
	// Ensure the top of the stack contains a string

	luaL_checktype( m_L, 1, LUA_TSTRING);
}

void LuaContext::assertNumber( )
{
	// Ensure the top of the stack contains a number

	luaL_checktype( m_L, 1, LUA_TNUMBER);
}

void LuaContext::assertTable( )
{
	// Ensure the top of the stack contains a table

	luaL_checktype( m_L, 1, LUA_TTABLE);
}

const char *LuaContext::errorString( )
{
	// Get the last error

	return( lua_tostring( m_L, -1 ) );
}

void LuaContext::exception( const string &strError )
{
	exception( strError.c_str() );
}

void LuaContext::exception( const char *strError )
{
	// Raise an error

	push( strError );
	lua_error( m_L );
}
