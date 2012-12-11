/*
	luaexpose
	A Lua/OpenGl data format experiment by x1nixmzeng
*/

#include <vector>
using std::vector;
#include <iterator>
#include <algorithm>

#include "GL\glut.h"
#include "lua\lua.hpp"

namespace LuaExpose
{
	#include "MyVec.h"
};

using namespace LuaExpose::Vectors;

lua_State *g_context;

const char *g_luascript = "expose.lua";

#define asFloat(x)	x/255.0f


typedef std::pair<Vec2f, Vec3f >	vertex;
typedef vector<vertex >				pointList;

Vec3f lastColour( 0.9f );

pointList g_points;

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

void callbackDisplay();
void callbackKeyboard(unsigned char, int, int);

#define LUA_SETGLOBALSTRING(L,name,val)lua_pushstring(L,val);lua_setglobal(L,name)

#include <Windows.h>

int main( int argc, char **argv )
{
	SetConsoleTitle("luaexpose debug");

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800,600);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Untitled - luaexpose");
	glutDisplayFunc(callbackDisplay);
	glutKeyboardFunc(callbackKeyboard);

	printf("luaexpose\n\nNew session started\n");

	g_context = luaL_newstate();
	
	LUA_SETGLOBALSTRING( g_context, "LUAEXPOSE_DESC", "LuaExpose v0.01" );

	luaL_openlibs( g_context ) ; // math, etc
	//luaopen_math( g_context );

	lua_register( g_context, "pushVtx", pushVertex );
	lua_register( g_context, "getVtx", getVertex );
	lua_register( g_context, "setVtxColour", setColour );

	glClearColor( asFloat(120), asFloat(120), asFloat(130), 1);
	gluOrtho2D(0,800,600,0);

	glutMainLoop();

	lua_close( g_context );
	g_points.clear();

	return 0;
}

void callbackKeyboard(unsigned char keycode, int, int)
{
	if( keycode == 'r' )
	{
		printf("Reloading script..\n");

		if( !( luaL_loadfile( g_context, g_luascript ) == 0 ) )
		{
			printf("ERROR: %s\n", lua_tostring( g_context, -1 ) );
		}
		else
		{
			// -- Reset previous stuff
			g_points.clear();
			lastColour = Vec3f( 0.9f );

			if( !( lua_pcall( g_context, 0, 0, 0 ) == 0 ) )
			{
				printf("ERROR: %s\n", lua_tostring( g_context, -1 ) );
			}
			else
			{
				callbackDisplay();
			}
		}
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