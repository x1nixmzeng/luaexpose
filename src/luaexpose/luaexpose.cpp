/*
	luaexpose
	A Lua/OpenGl data format experiment by x1nixmzeng
*/

#include <vector>
using std::vector;
#include <iterator>
#include <algorithm>

#include "GL/glut.h"

#define LUA_COMPAT_ALL

#include "lua/lua.hpp"

#include "MyVec.h"
#include "LuaContextBase.h"
#include "LuaContext.h"

#include "LuaContextTable.h"

#if defined( WIN32 )
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
#endif

#include <stdio.h>

// -- our collection of globals (TOFIX)
bool g_autoreload( true );
LuaContext lua;
const char *BaseScript = "core.lua";

using namespace Vectors;

#define asFloat(x)	x/255.0f

//typedef std::pair<Vec3f, Vec3f >	vertex;
typedef vector<Vec3f >				pointList;

struct index
{
	unsigned short ia, ib, ic;
};

typedef vector<index >				indexList;

Vec3f lastColour( 0.9f );
pointList g_points;
indexList g_indices;

#include <iostream>
#include <fstream>

using namespace std;
ifstream g_file;

// --

#include "exposedTypes.hpp"

static int myLuaString( lua_State *L )
{
	LuaContextBase mylua( L );
	int len = 0;

	if( mylua.countArguments() > 0 )
		len = mylua.getInt( -1 );

	std::string str;

	char mychr=1;

	if( len == 0 )
	{
		do
		{
			if( !( g_file.read( (char*)&mychr, sizeof(char) ).eof() || mychr == 0 ) )
				str += mychr;
		}
		while( mychr );
	}
	else
	{
		char *mystr = new char[len+1];
		mystr[len]=0;
		g_file.read( mystr, len );
		str.assign( mystr, len );
		delete mystr;
	}

	mylua.pushStr( str );

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
	
	g_points.push_back( Vec3f( x, y, z )/*, lastColour */);

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
	LuaContextBase l(L);

	l.assertTable();

	l.pushNil();

	// TODO: Determine if there are 2 nodes here and use them without the loop
	
	Vec3f demo;
	int i(0);

	while( lua_next(L, -2) != 0 )
	{
		if( i == 0 )
			demo.x = l.getNum( -1 );
		if( i == 1 )
			demo.y = l.getNum( -1 );
		if( i == 2 )
			demo.z = l.getNum( -1 );

		l.pop();
		++i;
	}

	g_points.push_back( demo/*, lastColour */);

	return( true );
}

static int setTableIndexBuffer( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

    while( lua_next(L, -2) != 0 )
	{
		{
			luaL_checktype(L, 1, LUA_TTABLE);
			lua_pushnil(L);

			index demo;
			int i(0);

			while( lua_next(L, -2) != 0 )
			{
				if( lua_isnumber( L, -1 ) )
				{
					if( i == 0 )
						demo.ia = luaL_checkinteger( L, -1 );
					if( i == 1 )
						demo.ib = luaL_checkinteger( L, -1 );
					if( i == 2 )
						demo.ic = luaL_checkinteger( L, -1 );
				}

				lua_pop(L, 1);
				++i;
			}

			g_indices.push_back( demo );
		}

		lua_pop(L, 1);
	}

	return 0;
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

	unsigned int curpos = g_file.tellg();

	g_file.seekg( 0, ios::end );
	l.pushNum( g_file.tellg() );
	g_file.seekg( curpos, ios::beg );


	return 1;
}

static int dataSeek( lua_State *L )
{
	LuaContextBase l(L);

	if( l.countArguments() == 1 )
	{
		g_file.seekg( l.getInt( -1 ), ios::beg );
		l.pop();
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
	l.pushNum( g_file.tellg() );

	return 1;
}

static int dataAssert( lua_State *L )
{
	
	return 1;
}

class test
{
public:
	static int sampleTest( lua_State *L )
	{
		return 0;
	}

};

test myTest;

bool LuaExposeSetup()
{
	printf("> Creating new session\n");

	lua.init();
	
	lua.setGlobal( "LUAEXPOSEDESC",		"LuaExpose" );
	lua.setGlobal( "LUAEXPOSEVERSTR",	"v0.01" );
	lua.setGlobal( "LUAEXPOSEVER",		0.01f );

#if defined( WIN32 )
	// --> This should appear before loadScript incase of error
	if( !( getWriteTime( BaseScript, &g_modTime ) ) )
	{
		printf("SETUP ERROR: Failed to get last mod time\n");
		return( false );
	}
#endif

	if( !( lua.loadScript( BaseScript ) ) )
	{
		printf("SETUP ERROR: %s\n", lua.errorString() );
		return( false );
	}

	// -- Data specific
	lua.setHook("size",			dataSize);
	lua.setHook("seek",			dataSeek);
	lua.setHook("pos",			dataPos);
	
	lua.setHook("assert",		myTest.sampleTest );

	// -- Output specific
	lua.setHook("rotateScene",	setRotations );
	lua.setHook("pushVtx",		pushVertex );
	lua.setHook("getVtx",		getVertex );
	lua.setHook("setVtxColour",	setColour );

	// -- Render specific
	lua.setHook("setFITable",	setTableIndexBuffer );
	lua.setHook("setVTable",	setVertexBuffer );

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

	if( g_file.is_open() )
		g_file.close();

	lua.destroy();
	g_points.clear();
	g_indices.clear();
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
			g_file.open( fname, ios::binary );

			if( !( g_file.is_open() ) )
			{
				printf("ERROR: Failed to load data\n");
				return;
			}

			lua.call("main");

			// here, check for gl stuff

			printf("> Got %i points\n", g_points.size());
			printf("> Got %i indexes\n", g_indices.size());

			if( g_indices.size() > 0 )
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(3, GL_FLOAT, 4*3, &g_points.at(0).x);

				printf("Setup vertex pointers\n");
			}
		}
		else
			printf("WARNING: No input file has been specified\n");
	}
}

void callbackDisplay();
void callbackKeyboard(unsigned char, int, int);


int mousex, mousey, mousestate;

enum
{
	MOUSESTATE_ROTATE = 0,
	MOUSESTATE_NONE
};

bool autoRotate( false );

float rot_y(0.0f), rot_x(0.0f);

void callbackAutoReload( )
{
#if defined( WIN32 )
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
#endif
}

void callbackMouse(int button, int state, int x, int y )
{
	if( button == GLUT_LEFT_BUTTON )
	{
		if( state == GLUT_UP )
		{
			mousex += x;
			mousey += y;
			mousestate = MOUSESTATE_ROTATE;
		}
		else
		if( state == GLUT_DOWN )
		{
			mousestate = MOUSESTATE_NONE;
		}
	}

	else

	if( button == GLUT_RIGHT_BUTTON )
	{

		//autoRotate = !autoRotate;
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

	glutCreateWindow("luaexpose");
	glutDisplayFunc(callbackDisplay);
	glutKeyboardFunc(callbackKeyboard);
	glutIdleFunc(callbackAutoReload);
	glutMouseFunc(callbackMouse);

	glClearColor( asFloat(120), asFloat(120), asFloat(130), 1);
	
	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);

	glEnable(GL_DEPTH_TEST);
	glClearDepth( 1.0f );

	gluPerspective(45.0f, 800.f/600.f, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	

	//glGenBuffersARB( 1, &m_nVBOVertices );

	LuaExposeLoad();

	// We need to define an exit call to clear our memory correctly
	atexit( LuaExposeSetupCleanup );

	glutMainLoop();
	return 0;
}

#include <sstream>
using namespace std;

class objRenderer
{
	ostringstream m_data;
public:
	objRenderer()
	{
		m_data << "# Exported from luaexpose\r\n\r\n";
		m_data.precision(5);
		m_data << fixed;
	}

	~objRenderer()
	{
		m_data.clear();
	}

	void Save()
	{
		ofstream tmp;
		tmp.open( "luaexpose_export.obj" );
		tmp << m_data.str();
		tmp.close();
	}
	
	void operator()( const Vec3f &v )
	{
		m_data << "v ";
		m_data << v.x << " ";
		m_data << v.y << " ";
		m_data << v.z << "\r\n";
	}

	void operator()( const index &i )
	{
		m_data << "f ";
		m_data << i.ia << " ";
		m_data << i.ib << " ";
		m_data << i.ic << "\r\n";
	}	
};

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
	else

	
	if( keycode == 's' )
	{
		printf("> Exporting OBJ..\n");

		objRenderer obj;

		for( pointList::const_iterator cit( g_points.begin() );
			cit != g_points.end();
			++cit )
		{
			obj( *cit );
		}
		
		for( indexList::const_iterator cit( g_indices.begin() );
			cit != g_indices.end();
			++cit )
		{
			obj( *cit );
		}

		obj.Save();
	}

	else

	if( keycode == 'u' )
	{
		glutPostRedisplay();
	}
}

/*
void renderPoints( vertex &point )
{
	glColor3f( point.second.x, point.second.y, point.second.z );

	glBegin(GL_POINTS);
	glVertex3f( point.first.x, point.first.y, point.first.z );
	glEnd();
}
*/

void callbackDisplay()
{
	rot_y += 2.0f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	gluLookAt(0, 50.0f, 0, 0, 0, -1, 0, 1, 0);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glPushMatrix();

	/*
	glRotatef(rot_y, 1.0f, 0.0f, 0.0f);
	glRotatef(rot_x, 0.0f, 1.0f, 0.0f);
	*/

	glScalef( 10.0f, 10.0f, 10.0f );

	glRotatef( g_angle, g_x, g_y, g_z );

	glPointSize(10);

	if( g_points.size() > 0 )
	{
		// https://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml

		// type, start, end, count, 

		void *test = &g_indices.at(0);

		glDrawElements(GL_TRIANGLES, g_indices.size() -1, GL_UNSIGNED_SHORT, &g_indices.at(0));
	}
	
	glPopMatrix();

	glutSwapBuffers();
	glFlush();
}
