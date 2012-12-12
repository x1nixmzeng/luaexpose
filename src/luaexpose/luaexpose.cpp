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

#include "LuaContext.h"

LuaContext g_lua;

class Testc
{
public:
	Testc(){}
};


LUA_CLASSCONSTRUCTOR(Testc);

static int luaTestcNew( lua_State*L)
{
	return 1;
}

static int luaTestcDestroy( lua_State*L)
{
	Testc *inst = lua_convertUserdataForTestc( g_lua );

	return 1;
}

int main( int, char ** )
{
	g_lua.init();
	g_lua.loadScript("core.lua");
	
	LUA_CLASSDESCBEGIN( Testc )
		LUA_CLASSDESCADD( "new",	luaTestcNew )
		LUA_CLASSDESCADD( "__gc",	luaTestcDestroy )
	LUA_CLASSDESCEND

	LUA_CLASSDESCAPPLY(Testc)

	g_lua.run();
	printf("%s\n", g_lua.errorString());
	g_lua.destroy();

	return 0;
}


namespace LuaExpose
{
	#include "MyVec.h"

	const char *ScriptName = "expose.lua";
};

using namespace LuaExpose::Vectors;

lua_State *g_context;



#define asFloat(x)	x/255.0f


typedef std::pair<Vec2f, Vec3f >	vertex;
typedef vector<vertex >				pointList;

Vec3f lastColour( 0.9f );

pointList g_points;


// From a Lua C++ binding example
// See https://gist.github.com/1594905

class Foo
{
	const char *m_name;

	public:
		Foo( const char *name )
			: m_name( name )
		{
			printf("Foo %s is born\n", m_name);
		}

		int Add(int a, int b)
		{
			return( a+b );
		}

		~Foo( )
		{
			printf("Foo %s has gone!\n", m_name );
		}
};

int l_Foo_constructor(lua_State * l)
{
	const char * name = luaL_checkstring(l, 1);

	Foo ** udata = (Foo **)lua_newuserdata(l, sizeof(Foo *));
	*udata = new Foo(name);

	luaL_getmetatable(l, "luaL_Foo");
	lua_setmetatable(l, -2);

	return 1;
}

Foo * l_CheckFoo(lua_State * l, int n)
{
	 return *(Foo **)luaL_checkudata(l, n, "luaL_Foo");
}

int l_Foo_add(lua_State * l)
{
	Foo * foo = l_CheckFoo(l, 1);

	int a = luaL_checknumber(l, 2);
	int b = luaL_checknumber(l, 3);

	lua_pushnumber( l, foo->Add( a, b ) );

	return 1;
}

int l_Foo_destructor(lua_State * l)
{
	Foo * foo = l_CheckFoo(l, 1);
	delete foo;

	return 0;
}

extern "C"
{
	luaL_Reg sFooRegs[] =
	{
		{ "new", l_Foo_constructor },
		{ "add", l_Foo_add },
		{ "__gc", l_Foo_destructor },
		{ NULL, NULL }
	};

	void registerSprite(lua_State *l)
	{
		luaL_newmetatable(l, "luaL_Foo");
		luaL_register(l, 0, sFooRegs);
		lua_pushvalue(l, -1);
		lua_setfield(l, -1, "__index");
		lua_setglobal(l, "Foo");
	}


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

void callbackDisplay();
void callbackKeyboard(unsigned char, int, int);

#define LUA_SETGLOBALSTRING(L,name,val)lua_pushstring(L,val);lua_setglobal(L,name)

void luaexposeCleanup()
{
	printf("Session ending\n\n");
	

	lua_close( g_context );
	g_points.clear();
}

//int main( int argc, char **argv )
int main2( int argc, char **argv )
{
	//#include <Windows.h>
	//SetConsoleTitle("luaexpose debug");

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

	registerSprite( g_context );

	lua_register( g_context, "pushVtx", pushVertex );
	lua_register( g_context, "getVtx", getVertex );
	lua_register( g_context, "setVtxColour", setColour );
	lua_register( g_context, "pushVtxBuffer", setVertexBuffer ); // array of Vec2s

	glClearColor( asFloat(120), asFloat(120), asFloat(130), 1);
	gluOrtho2D(0,800,600,0);

	callbackKeyboard('r',0,0);


	// We need to define an exit call to clear our memory correctly
	atexit( luaexposeCleanup );


	glutMainLoop();
	return 0;
}

void callbackKeyboard(unsigned char keycode, int, int)
{
	if( keycode == 'r' )
	{
		printf("Reloading script..\n");
		lua_close( g_context );

		printf("Creating new context\n");

	g_context = luaL_newstate();
	
	LUA_SETGLOBALSTRING( g_context, "LUAEXPOSE_DESC", "LuaExpose v0.01" );

	luaL_openlibs( g_context ) ; // math, etc
	//luaopen_math( g_context );

	registerSprite( g_context );

	lua_register( g_context, "pushVtx", pushVertex );
	lua_register( g_context, "getVtx", getVertex );
	lua_register( g_context, "setVtxColour", setColour );
	lua_register( g_context, "pushVtxBuffer", setVertexBuffer ); // array of Vec2s



		if( !( luaL_loadfile( g_context, LuaExpose::ScriptName ) == 0 ) )
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

				//OutputDebugString( lua_tostring( g_context, -1 ) );
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
