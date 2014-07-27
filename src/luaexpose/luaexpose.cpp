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

#define DUPETHIS(x)	(x),(x)
#define TRIPTHIS(x) (x),(x),(x)

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

Vec3f colPoints(0.3f );// black
Vec3f colLines( 0.6f );// black
Vec3f colFaces( 1.0f );// white

vector<pointList> g_modelVerts;
vector<indexList> g_modelInd;
Vec3f g_midPoint;

enum RENDERAS
{
	RENDERAS_MIN = 0,

	RENDERAS_POINTS,
	RENDERAS_LINES,
	RENDERAS_FACES,
	RENDERAS_MIXED,

	RENDERAS_MAX
};

RENDERAS g_renderType = RENDERAS_POINTS;

#include <iostream>
#include <fstream>

using namespace std;
ifstream g_file;
std::streampos g_fileSize;

// --

#include "exposedTypes.hpp"

static int myLuaString( lua_State *L )
{
	LuaContextBase l( L );
	int len = 0;

	if( l.countArguments() > 0 )
		len = l.getInt( -1 );

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

	l.pushStr( str );

	return l.pushedItemCount();
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
	
  // not implemented yet

	//g_points.push_back( Vec3f( x, y, z )/*, lastColour */);

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
template< Vec3f& whichColour >
static int setColour( lua_State *L )
{
	float r = lua_tonumber(L, -3);
	float g = lua_tonumber(L, -2);
	float b = lua_tonumber(L, -1);

	whichColour.xyz[0] = r;
	whichColour.xyz[1] = g;
	whichColour.xyz[2] = b;
	whichColour /= 255.0f;

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

  g_modelVerts.back().push_back(demo); // lastColour

	return( true );
}

static int setTableIndexBuffer( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

  indexList tmp;

  while( lua_next(L, -2) != 0 )
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

		tmp.push_back( demo );

		lua_pop(L, 1);
	}

  // if not null
  g_modelInd.push_back(tmp);

	return 0;
}

static int setVertexBuffer( lua_State *L )
{
	luaL_checktype(L, 1, LUA_TTABLE);

	lua_pushnil(L);

  g_modelVerts.resize(g_modelVerts.size() + 1);

  while( lua_next(L, -2) != 0 )
	{
		luaGetVec2( L );

		lua_pop(L, 1);
	}

	return 0;
}

//static int getVertex( lua_State *L )
//{
//	if( lua_gettop(L) < 1 )
//	{
//		lua_pushstring(L, "getVtx expects 1 parameter (vertex index)");
//		lua_error(L);
//
//		return 1; // i guess?
//	}
//
//	int indx = lua_tointeger(L, -1);
//	
//	if( indx < 0 || indx >= g_points.size() )
//	{
//		lua_pushstring(L, "Supplied vertex index value was invalid");
//		lua_error(L);
//
//		return 1;
//	}
//	/*
//	const Vec2f &point = g_points.at( indx ).first;
//
//	lua_pushnumber( L, point.x );
//	lua_pushnumber( L, point.y );
//
//	return 2;
//	*/
//	return 0;
//}

float g_modelScale(1.0f);

static int setModelScale(lua_State* L)
{
  g_modelScale = lua_tonumber(L, -1);

  lua_pop(L, 1);

  return 0;
}

static int setModelTranslation(lua_State* L)
{
  g_midPoint.z = lua_tonumber(L, -1);
  g_midPoint.y = lua_tonumber(L, -2);
  g_midPoint.x = lua_tonumber(L, -3);

  lua_pop(L, 3);

  return 0;
}

static int dataSize( lua_State *L )
{
	LuaContextBase l(L);

	// Pushes the cached filesize to the Lua stack
	l.pushInt( g_fileSize.seekpos() );
	return l.pushedItemCount();
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

	return 0;
}

static int dataPos( lua_State *L )
{
	LuaContextBase l(L);

	// Pushes the current file position to the Lua stack
	l.pushNum( g_file.tellg().seekpos() );
	return l.pushedItemCount();
}

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

	// -- Other globals
	lua.setGlobal("SHOW_POINTS",			RENDERAS_POINTS );
	lua.setGlobal("SHOW_LINES",				RENDERAS_LINES );
	lua.setGlobal("SHOW_FACES",				RENDERAS_FACES );
	lua.setGlobal("SHOW_LINES_AND_FACES",	RENDERAS_MIXED );

	// -- Data specific
	lua.setHook("size",			dataSize);
	lua.setHook("seek",			dataSeek);
	lua.setHook("pos",			dataPos);

	// -- Output specific
	lua.setHook("rotateScene",	setRotations );
	//lua.setHook("pushVtx",		pushVertex );
	//lua.setHook("getVtx",		getVertex );
	
	// (colours)
	lua.setHook("pointColour",	setColour<colPoints> );
	lua.setHook("pointColor",	setColour<colPoints> );
	lua.setHook("lineColour",	setColour<colLines> );
	lua.setHook("lineColor",	setColour<colLines> );
	lua.setHook("faceColour",	setColour<colFaces> );
	lua.setHook("faceColor",	setColour<colFaces> );

	// -- Render specific
	lua.setHook("setFITable",	setTableIndexBuffer );
	lua.setHook("setVTable",	setVertexBuffer );

  lua.setHook("scale", setModelScale);
  lua.setHook("move", setModelTranslation);

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
  g_modelVerts.clear();
  g_modelInd.clear();

  g_midPoint = Vec3fZero;
}

void LuaExposeLoad()
{
	// -- Setup default values here
	colPoints = Vec3f(0.3f );
	colLines = Vec3f( 0.6f );
	colFaces = Vec3f( 1.0f );
	g_angle = 0.0f;
	g_x = g_y = g_z = 0.0f;
	g_renderType = RENDERAS_POINTS;
	// --

	if( LuaExposeSetup() )
	{
		const char *fname = lua.getGlobalString("file");

		// Check that the gobal has been set in the Lua script
		if( fname )
		{
			// Check the main function is defined in the Lua script
			if( !( lua.hasFunction("main") ) )
			{
				// Fatal error as nothing can be run
				printf("ERROR: No main() function declared in Lua script!\n");
				return;
			}

			printf("> Source file is \"%s\"\n", fname );

			// Open the file and seek to the end (to calculate the file size)
			g_file.open( fname, ios::binary | ios::ate );

			if( !( g_file.is_open() ) )
			{
				// Fatal error as there is no data to work with
				printf("ERROR: Failed to load data\n");
				return;
			}

			// Cache the current file position and sek to the beginning
			g_fileSize = g_file.tellg();
			g_file.seekg( 0, ios::beg );

			if( g_fileSize.seekpos() == 0 )
			{
				printf("WARNING: There is no data (as the file is empty)\n");
			}

			// Attempt to set the render state
			int renderType = lua.getGlobalInteger("show");

			if( renderType > RENDERAS_MIN && renderType < RENDERAS_MAX )
				g_renderType = (RENDERAS)renderType;

			// Call the main Lua script data parsing function
			lua.call("main");

			// -- Setup our persistant data buffers
			// See http://www.opengl.org/sdk/docs/man2/xhtml/glEnableClientState.xml

      
      printf("> Got %i models\n", g_modelInd.size());

      vector<pointList>::iterator vit = g_modelVerts.begin();

      while (vit != g_modelVerts.end())
      {
        printf("> Got %i vertices\n", vit->size());
        vit++;
      }

      vector<indexList>::iterator iit = g_modelInd.begin();

      while (iit != g_modelInd.end())
      {
        printf("> Got %i points\n", iit->size());
        iit++;
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

float rot_y(0.0f), rot_x(0.0f), rot_z(0.0f);

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

  g_modelVerts.reserve(10); // 10 models max

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

float g_scaleScalar( 2.0f );

void callbackKeyboard(unsigned char keycode, int, int)
{
	if( keycode == 'r' || keycode == 'R' )
	{
		printf("> Session reloading\n");
		LuaExposeSetupCleanup();
		LuaExposeLoad();
		
		// Update screen after new data
		glutPostRedisplay();
	}
	else

	
	//if( keycode == 's' || keycode == 'S' )
	//{
	//	printf("> Exporting OBJ..\n");

	//	objRenderer obj;

	//	for( pointList::const_iterator cit( g_points.begin() );
	//		cit != g_points.end();
	//		++cit )
	//	{
	//		obj( *cit );
	//	}
	//	
	//	for( indexList::const_iterator cit( g_indices.begin() );
	//		cit != g_indices.end();
	//		++cit )
	//	{
	//		obj( *cit );
	//	}

	//	obj.Save();
	//}
  // else
	if( keycode == 'u' || keycode == 'U' )
	{
		rot_x += 2.0f;
		glutPostRedisplay();
	}

	else

	if( keycode == 'q' || keycode == 'Q'
		|| keycode == 'a' || keycode == 'A' )
	{

		if( keycode == 'q' || keycode == 'Q' )
			g_scaleScalar += 0.2f;
		else
			g_scaleScalar -= 0.2f;

		glutPostRedisplay();
	}
}

void renderPoint( const Vec3f &p )
{
	glBegin(GL_POINTS);
		glVertex3f( p.x, p.y, p.z );
	glEnd();
}

// hacky, by worky
int mdl_index = -1;

void renderFace( const index &i )
{
  if (mdl_index == -1)
    return;

  const pointList& pl = g_modelVerts[mdl_index];

	glBegin( GL_TRIANGLES );
    glVertex3f(pl.at(i.ia).x, pl.at(i.ia).y, pl.at(i.ia).z);
    glVertex3f(pl.at(i.ib).x, pl.at(i.ib).y, pl.at(i.ib).z);
    glVertex3f(pl.at(i.ic).x, pl.at(i.ic).y, pl.at(i.ic).z);
	glEnd();
}

void renderGrid( int width, int height )
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glBegin( GL_LINES );
	glColor3f( TRIPTHIS( 0.4f ) );
	
	for( int x = -width; x <= width; ++x )
	{
		if( !( x == 0 ) )
		{
			glVertex3f( x, 0.0f,-height );
			glVertex3f( x, 0.0f, height );
		}
	}

	for( int z = -height; z <= height; ++z )
	{
		if( !( z == 0 ) )
		{
			glVertex3f(-width, 0.0f, z );
			glVertex3f( width, 0.0f, z );
		}
	}

	glColor3f( TRIPTHIS( 0.2f ) );
	glVertex3f( width, 0.0f, 0.0f );
	glVertex3f(-width, 0.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, height );
	glVertex3f( 0.0f, 0.0f,-height );

	glEnd();
}

void callbackDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glDisable( GL_CULL_FACE );

	glLoadIdentity();
	gluLookAt(40.0f, 20.0f, -40.0f, 0, 0, -1, 0, 1, 0);

	glPushMatrix();

	//glRotatef(rot_y, 1.0f, 0.0f, 0.0f);
	glRotatef(rot_x, 0.0f, 1.0f, 0.0f); // this is actually y
	//glRotatef(rot_z, 0.0f, 0.0f, 1.0f);

	glScalef( TRIPTHIS( g_scaleScalar ) );

	glRotatef( g_angle, g_x, g_y, g_z );

	renderGrid( DUPETHIS( 5 ) );

	switch( g_renderType )
	{
		case RENDERAS_LINES :
		{
			glColor3f( colLines.x, colLines.y, colLines.z );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			break;
		}
		case RENDERAS_MIXED:
		case RENDERAS_FACES:
		{
			glColor3f( colFaces.x, colFaces.y, colFaces.z );
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			break;
		}
		default:
		{
			glColor3f( colPoints.x, colPoints.y, colPoints.z );
			glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
		}
	}

  glPushMatrix();

  //glScalef(TRIPTHIS(g_modelScale));
  glTranslatef(g_midPoint.x, g_midPoint.y, g_midPoint.z);

	/*
	if( g_indices.size() > 0 )
	{
		// This method will only work with OpenGl 3.1+ (so I have an outdated version)
		//glDrawElements(GL_TRIANGLES, g_indices.size() -1, GL_UNSIGNED_SHORT, &g_indices.at(0).ia);
	}
	else
		*/
	{
    vector<indexList>::iterator it = g_modelInd.begin();
    int i = 0;
    while (it != g_modelInd.end())
    {
      if (i >= g_modelVerts.size())
        break;

      if (g_modelVerts[i].empty())
        break;

      mdl_index = i;

      glPushMatrix();

      for_each
        (
        it->begin(),
        it->end(),
        renderFace
        );

      glPopMatrix();

      i++;
      it++;
    }
	}
	
	if( g_renderType == RENDERAS_MIXED )
	{
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glColor3f( colLines.x, colLines.y, colLines.z );

    vector<indexList>::iterator it = g_modelInd.begin();

    int i = 0;
    while (it != g_modelInd.end())
    {
      if (i >= g_modelVerts.size())
        break;

      if (g_modelVerts[i].empty())
        break;

      mdl_index = i;
      for_each
        (
        it->begin(),
        it->end(),
        renderFace
        );

      i++;
      it++;
    }
	}

  glPopMatrix();

	glPopMatrix();

	glutSwapBuffers();
	glFlush();
}
