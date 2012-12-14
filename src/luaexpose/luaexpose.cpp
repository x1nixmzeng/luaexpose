/*
	luaexpose
	A Lua/OpenGl data format experiment by x1nixmzeng
*/

#include <vector>
using std::vector;
#include <iterator>
#include <algorithm>

#include "GL\glut.h"

#define LUA_COMPAT_ALL

#include "lua\lua.hpp"

#include "MyVec.h"
#include "LuaContextBase.h"
#include "LuaContext.h"

#pragma region Watch for last write time on a file using the WinAPI
#include <Windows.h>

bool getWriteTime( const char *strFilename, FILETIME *outWriteTime )
{
	HANDLE hFile = CreateFileA
	(
		strFilename,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if( hFile == INVALID_HANDLE_VALUE )
		return( false );

	if( !( GetFileTime( hFile, NULL, NULL, outWriteTime ) ) )
	{
		CloseHandle( hFile );
		return( false );
	}

	CloseHandle( hFile );
	return( true );
}

bool doesTimeDiffer( FILETIME *a, FILETIME *b )
{
	return( !( ( a->dwHighDateTime == b->dwHighDateTime )
				&& ( a->dwLowDateTime == b->dwLowDateTime ) ) );
}

FILETIME g_modTime;
#pragma endregion

// -- our collection of globals (TOFIX)
bool g_autoreload( false );
LuaContext lua;
const char *BaseScript = "core.lua";

using namespace Vectors;

#define asFloat(x)	x/255.0f

typedef std::pair<Vec2f, Vec3f >	vertex;
typedef vector<vertex >				pointList;

Vec3f lastColour( 0.9f );
pointList g_points;
// --

static int autoReload( lua_State *L )
{
	// -- no need to toggle
	g_autoreload = true;
	return 0;
}

static int myPrint( lua_State *L )
{
	LuaContextBase mylua( L );

	int args = mylua.countArguments();

	if( args > 0 )
	{
		printf("Script > ");

		// It looks like numbers are converted to strings anyway..
		// Also due to the stack, things are pushed in reverse order.. (fixed)

		for( int i = args; i > 0; --i )
			printf( "%s", mylua.getStringFromStack( -i ) );

		printf("\n");

	}

	return 0;
}

#include "exposedTypes.hpp"

static int myLuaString( lua_State *L )
{
	LuaContextBase mylua( L );
	int len = 0;

	if( mylua.countArguments() > 0 )
		len = mylua.getNumberFromStack( -1 );

	std::string example("This is an example of a c-string result. Hello world!");

	if( len > 0 )
		example = example.substr(0, len);

	mylua.push( example );

	return 1;
}

void setupExposedTypes()
{
	// -- integer types
	lua.setClassHook("u32", u32methods);
	lua.setClassHook("s32", s32methods);
	lua.setClassHook("u24",	u24methods);
	//lua.setClassHook("s24",	s24methods);
	lua.setClassHook("u16",	u16methods);
	lua.setClassHook("s16",	s16methods);
	lua.setClassHook("u8",	u8methods);
	lua.setClassHook("s8",	s8methods);

	// -- float-point types
	lua.setClassHook("f32",	f32methods);

	// -- string type
	
	/*
		NOTE
			Previous attempt was str(n) or str()
			Instead str:read(n) or str:read() may be favorable
			BUT there are now size() or skip() methods without first reading a string..
	*/

	luaL_Reg strmethods[] = 
	{
		{ "read", myLuaString },
		{ NULL, NULL }
	};

	lua.setClassHook("str",	strmethods );
}

static int pushVertex( lua_State *L )
{
	float x = lua_tonumber(L, -2);
	float y = lua_tonumber(L, -1);
	
	g_points.push_back( vertex( Vec2f( x, y ), lastColour ) );

	printf( "Got [%.2f, %.2f]!\n", x, y );

	lua_pop(L,2);

    return 0;
}

static int setColour( lua_State *L )
{
	float r = lua_tonumber(L, -3);
	float g = lua_tonumber(L, -2);
	float b = lua_tonumber(L, -1);

	lastColour.xyz[0] = r;
	lastColour.xyz[1] = g;
	lastColour.xyz[2] = b;

	lua_pop(L,3);

	return 0;
}

bool luaGetVec2( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

	// TODO: Determine if there are 2 nodes here and use them without the loop
	
	Vec2f demo;
	int i(0);

	while( lua_next(L, -2) != 0 )
	{
		if( lua_isnumber( L, -1 ) )
		{
			if( i == 0 )
				demo.x = lua_tonumber( L, -1 );
			if( i == 1 )
				demo.y = lua_tonumber( L, -1 );
		}

		lua_pop(L, 1);
		++i;
	}

	g_points.push_back( vertex( demo, lastColour ) );

	return( true );
}

static int setVertexBuffer( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

    while( lua_next(L, -2) != 0 )
	{
		luaGetVec2( L );

		lua_pop(L, 1);
	}

	return 0;
}

static int getVertex( lua_State *L )
{
	if( lua_gettop(L) < 1 )
	{
		lua_pushstring(L, "getVtx expects 1 parameter (vertex index)");
		lua_error(L);

		return 1; // i guess?
	}

	int indx = lua_tointeger(L, -1);
	
	if( indx < 0 || indx >= g_points.size() )
	{
		lua_pushstring(L, "Supplied vertex index value was invalid");
		lua_error(L);

		return 1;
	}

	const Vec2f &point = g_points.at( indx ).first;

	lua_pushnumber( L, point.x );
	lua_pushnumber( L, point.y );

	return 2;
}

static int sampleTablePush( lua_State *L )
{
	LuaContextBase mylua( L );

	vector<int > test;
	test.push_back(101);
	test.push_back(102);
	test.push_back(103);

	mylua.push<int>( test );

	return 1;
}

bool LuaExposeSetup()
{
	printf("> Creating new session\n");

	lua.init();
	
	lua.setGlobal( "LUAEXPOSEDESC",		"LuaExpose v0.01" );
	lua.setGlobal( "LUAEXPOSEVERSTR",	"v0.01" );
	lua.setGlobal( "LUAEXPOSEVER",		0.01f );

	/*
		For the current file (passed via dialog selection or commandline)
			Filepath
			Filename		
			Extension
			Path
			Filesize

		Required classes
			s32/u32,s16/u16,s8/u8
			OR base class to read/size/skip/little-big

		Required global functions
			seek( int )
			mark( string )
			skip( int )
			(and other rendering settings)
			(and actual rendering of points/lines/polys)

	*/

	// --> This should appear before loadScript incase of error
	if( !( getWriteTime( BaseScript, &g_modTime ) ) )
	{
		printf("SETUP ERROR: Failed to get last mod time\n");
		return( false );
	}

	if( !( lua.loadScript( BaseScript ) ) )
	{
		printf("SETUP ERROR: %s\n", lua.errorString() );
		return( false );
	}

	lua.setHook("sampleTable",	sampleTablePush );

	lua.setHook("print",		myPrint );
	lua.setHook("autoReload",	autoReload );
	lua.setHook("pushVtx",		pushVertex );
	lua.setHook("getVtx",		getVertex );
	lua.setHook("setVtxColour",	setColour );
	lua.setHook("pushVtxBuffer",setVertexBuffer );

	setupExposedTypes();

	return( true );
}

void LuaExposeSetupCleanup()
{
	printf("> Session ending\n");

	lua.destroy();
	g_points.clear();
}

bool LuaExposeReload()
{
	printf("> Session reloading\n");

	LuaExposeSetupCleanup();

	if( LuaExposeSetup() )
	{
		// -- Setup default values here
		lastColour = Vec3f( 0.9f );
		g_autoreload = false;
		// --

		return( true );
	}

	return( false );
}

void callbackDisplay();
void callbackKeyboard(unsigned char, int, int);

void callbackAutoReload( )
{
	if( g_autoreload )
	{
		FILETIME localModTime;

		if( getWriteTime( BaseScript, &localModTime ) )
		{
			if( doesTimeDiffer( &localModTime, &g_modTime ) )
			{
				printf("> Detected script modification..\n");
				callbackKeyboard('r',0,0);
			}
		}
	}
}

int main( int argc, char **argv )
{
	printf("luaexpose\n\n");

	printf("> Starting OpenGl\n");
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("luaexpose");
	glutDisplayFunc(callbackDisplay);
	glutKeyboardFunc(callbackKeyboard);
	glutIdleFunc(callbackAutoReload);

	glClearColor( asFloat(120), asFloat(120), asFloat(130), 1);
	gluOrtho2D(0,800,600,0);

	if( LuaExposeSetup() )
		if( !( lua.run() ) )
			printf("ERROR: %s\n", lua.errorString() );

	// We need to define an exit call to clear our memory correctly
	atexit( LuaExposeSetupCleanup );

	glutMainLoop();
	return 0;
}

void callbackKeyboard(unsigned char keycode, int, int)
{
	if( keycode == 'r' )
	{
		if( LuaExposeReload() )
			if( !( lua.run() ) )
				printf("ERROR: %s\n", lua.errorString() );
		
		// Update screen after new data
		glutPostRedisplay();
	}
}

void renderPoints( vertex &point )
{
	glColor3f( point.second.x, point.second.y, point.second.z );

	glBegin(GL_POINTS);
		glVertex2f( point.first.x, point.first.y );
	glEnd();
}

void callbackDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();

	glPointSize(10);

	std::for_each
	(
		g_points.begin(),
		g_points.end(),
		renderPoints
	);

	glutSwapBuffers();
	glFlush();
}
