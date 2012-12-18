#include "LuaContext.h"

LuaContext::LuaContext( )
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
	
	/*
		NOTE: We don't want to provide access to everything BUT
		there is a problem loading libaries seperately
		See http://stackoverflow.com/a/10821793
	*/

	// So for now..
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
	// NOTE: The call fix has been replaced with a base class

	return( lua_pcall( m_L, 0, 0, 0 ) == 0 );
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

	pushCStr( strVal );
	lua_setglobal( m_L, strName );
}

void LuaContext::setGlobal( const string &strName, float fVal )
{
	setGlobal( strName.c_str(), fVal );
}

void LuaContext::setGlobal( const char *strName, float fVal )
{
	// Set a global number

	pushNum( fVal );
	lua_setglobal( m_L, strName );
}

void LuaContext::setGlobal( const string &strName, int iVal )
{
	setGlobal( strName.c_str(), iVal );
}

void LuaContext::setGlobal( const char *strName, int iVal )
{
	// Set a global integer

	pushInt( iVal );
	lua_setglobal( m_L, strName );
}
