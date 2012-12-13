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

LuaContext lua;

const char *BaseScript = "core.lua";


using namespace Vectors;

#define asFloat(x)	x/255.0f

typedef std::pair<Vec2f, Vec3f >	vertex;
typedef vector<vertex >				pointList;

Vec3f lastColour( 0.9f );

pointList g_points;

static int myPrint( lua_State *L )
{
	int args = lua_gettop(L);

	if( args > 0 )
	{
		LuaContextBase tmpLua( L );

		printf("Script > ");

		// It looks like numbers are converted to strings anyway..
		// Also due to the stack, things are pushed in reverse order.. (fixed)

		for( int i = args; i > 0; --i )
			printf( "%s", tmpLua.getStringFromStack( -i ) );

		printf("\n");

	}

	return 0;
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
	lua.setGlobal( "CUR_DIR",			"C:/" );
	lua.setGlobal( "CUR_FILE",			"C:/rip_002B1B80.dat" );
	lua.setGlobal( "CUR_FILENAME",		"rip_002B1B80" );
	lua.setGlobal( "CUR_FILEEXT",		"dat" );
	lua.setGlobal( "CUR_FILESIZE",		1024 );

	if( !( lua.loadScript( BaseScript ) ) )
	{
		printf("SETUP ERROR: %s\n", lua.errorString() );
		return( false );
	}

	lua.setHook("print",		myPrint );
	lua.setHook("pushVtx",		pushVertex );
	lua.setHook("getVtx",		getVertex );
	lua.setHook("setVtxColour",	setColour );
	lua.setHook("pushVtxBuffer",setVertexBuffer );

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

	// -- Setup default values here
	lastColour = Vec3f( 0.9f );

	// --

	return( LuaExposeSetup() );
}

void callbackDisplay();
void callbackKeyboard(unsigned char, int, int);

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
