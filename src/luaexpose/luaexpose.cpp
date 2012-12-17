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
bool g_autoreload( true );
LuaContext lua;
const char *BaseScript = "core.lua";

using namespace Vectors;

#define asFloat(x)	x/255.0f

typedef std::pair<Vec3f, Vec3f >	vertex;
typedef vector<vertex >				pointList;

Vec3f lastColour( 0.9f );
pointList g_points;

FILE *g_fHandle;
// --

#include "exposedTypes.hpp"

static int myLuaString( lua_State *L )
{
	LuaContextBase mylua( L );
	int len = 0;

	if( mylua.countArguments() > 0 )
		len = mylua.getNumberFromStack( -1 );

	std::string str;

	char mychr=1;

	if( len == 0 )
	{
		do
		{
			if( fread(&mychr,1,1,g_fHandle) && mychr != 0 )
				str += mychr;
		}
		while( mychr );
	}
	else
	{
		char *mystr = new char[len+1];
		mystr[len]=0;

		fread(mystr,1,len,g_fHandle);
		str.assign( mystr, len );
		delete mystr;
	}

	mylua.push( str );

	return 1;
}

void setupExposedTypes()
{
	// -- integer types
	lua.setClassHook("u32", u32methods);
	lua.setClassHook("s32", s32methods);
	//lua.setClassHook("u24",	u24methods);
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
	float x = lua_tonumber(L, -3);
	float y = lua_tonumber(L, -2);
	float z = lua_tonumber(L, -1);
	
	g_points.push_back( vertex( Vec3f( x, y, z ), lastColour ) );

	lua_pop(L,3);

    return 0;
}

float g_angle(0.0f), g_x(0.0f), g_y(0.0f), g_z(0.0f);

static int setRotations( lua_State *L )
{
	g_angle = lua_tonumber(L, -4);
	g_x = lua_tonumber(L, -3);
	g_y = lua_tonumber(L, -2);
	g_z = lua_tonumber(L, -1);

	lua_pop(L,3);

	return 0;
}

// -- converts from rgb
static int setColour( lua_State *L )
{
	float r = lua_tonumber(L, -3);
	float g = lua_tonumber(L, -2);
	float b = lua_tonumber(L, -1);

	lastColour.xyz[0] = r;
	lastColour.xyz[1] = g;
	lastColour.xyz[2] = b;
	lastColour /= 255.0f;

	lua_pop(L,3);

	return 0;
}

bool luaGetVec2( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

	// TODO: Determine if there are 2 nodes here and use them without the loop
	
	Vec3f demo;
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
	/*
	const Vec2f &point = g_points.at( indx ).first;

	lua_pushnumber( L, point.x );
	lua_pushnumber( L, point.y );

	return 2;
	*/
	return 0;
}

static int dataSize( lua_State *L )
{
	LuaContextBase l(L);

	unsigned int curpos = ftell( g_fHandle );
	fseek( g_fHandle, 0, SEEK_END );

	l.push( ftell( g_fHandle ) );

	fseek( g_fHandle, curpos, SEEK_SET );


	return 1;
}

static int dataSeek( lua_State *L )
{
	LuaContextBase l(L);

	if( l.countArguments() == 1 )
	{
		fseek(g_fHandle, l.getNumberFromStack(-1), SEEK_SET);
	}
	else
	{
		l.exception("Expected 1 argument for 'seek' function");
	}

	return 1;
}

static int dataPos( lua_State *L )
{
	LuaContextBase l(L);
	l.push( ftell( g_fHandle ) );

	return 1;
}

bool LuaExposeSetup()
{
	printf("> Creating new session\n");

	lua.init();
	
	lua.setGlobal( "LUAEXPOSEDESC",		"LuaExpose" );
	lua.setGlobal( "LUAEXPOSEVERSTR",	"v0.01" );
	lua.setGlobal( "LUAEXPOSEVER",		0.01f );

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

	// -- Data specific
	lua.setHook("size",			dataSize);
	lua.setHook("seek",			dataSeek);
	lua.setHook("pos",			dataPos);

	// -- Output specific
	lua.setHook("rotateScene",	setRotations );
	lua.setHook("pushVtx",		pushVertex );
	lua.setHook("getVtx",		getVertex );
	lua.setHook("setVtxColour",	setColour );
	lua.setHook("pushVtxBuffer",setVertexBuffer );

	setupExposedTypes();

	if( !( lua.run() ) )
	{
		printf("ERROR: %s\n", lua.errorString() );
		return false;
	}

	return( true );
}

void LuaExposeSetupCleanup()
{
	printf("> Session ending\n");

	if( g_fHandle )
	{
		fclose( g_fHandle );
		g_fHandle = nullptr;
	}

	lua.destroy();
	g_points.clear();
}

void LuaExposeLoad()
{
	// -- Setup default values here
	lastColour = Vec3f( 0.9f );
	g_angle = 0.0f;
	g_x = g_y = g_z = 0.0f;
	// --

	if( LuaExposeSetup() )
	{
		const char *fname = lua.getGlobalString("file");

		if( fname )
		{
			g_fHandle = fopen( fname, "rb" );

			if( !( g_fHandle ) )
			{
				printf("ERROR: Failed to load data\n");
				return;
			}

			lua.call("main");
		}
		else
			printf("WARNING: No input file has been specified\n");
	}
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
				FlashWindow( GetConsoleWindow(), true );
			}
		}
	}
}

int main( int argc, char **argv )
{
	// todo: optional argument for display window (opengl)
	// messages would need processes in other ways though
	printf("luaexpose\n\n");

	printf("> Starting OpenGl\n");

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(100,100);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);

	glutCreateWindow("luaexpose");
	glutDisplayFunc(callbackDisplay);
	glutKeyboardFunc(callbackKeyboard);
	glutIdleFunc(callbackAutoReload);

	glClearColor( asFloat(120), asFloat(120), asFloat(130), 1);
	//gluOrtho2D(0,800,600,0);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 800.f/600.f, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);

	LuaExposeLoad();

	// We need to define an exit call to clear our memory correctly
	atexit( LuaExposeSetupCleanup );

	glutMainLoop();
	return 0;
}

void callbackKeyboard(unsigned char keycode, int, int)
{
	if( keycode == 'r' )
	{
		printf("> Session reloading\n");
		LuaExposeSetupCleanup();
		LuaExposeLoad();
		
		// Update screen after new data
		glutPostRedisplay();
	}
}

void renderPoints( vertex &point )
{
	glColor3f( point.second.x, point.second.y, point.second.z );

	glBegin(GL_POINTS);
	glVertex3f( point.first.x, point.first.y, point.first.z );
	glEnd();
}

void callbackDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, -50.0f, 0, 0, -1, 0, 1, 0);

	glPushMatrix();

	glRotatef( g_angle, g_x, g_y, g_z );
	glScalef( 10.0f, 10.0f, 10.0f );

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
