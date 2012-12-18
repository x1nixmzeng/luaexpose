/*
	exposedTypes
	Written by x1nixmzeng

	Templates (incomplete) to define the common base type properties

	TODO: Allow class to switch endian :little() :big()
	This probally means userdata and class casting..
*/

union u24Base
{
	int val;
	struct{ unsigned char val1, val2, val3; };

	int toInt( ) const{ return( val ); }
};

template<class T> inline int template_size( int n = 1 )
{
	return( sizeof( T ) * n );
}

template<> inline int template_size<u24Base >( int n )
{
	return( 3 * n );
}

/*
	Read single value		read()		Returns value of type
	Read number of values	read(n)		Returns table of values of type
*/
template<class T> int l_template_read(lua_State *L)
{
	LuaContextBase l(L);

	int args = l.countArguments();

	if( args == 0 || args > 2 )
		l.exception("Wrong number of arguments for 'read' member function");

	if( args == 2 )
	{
		int n = l.getInt( -1 );
		LuaContextTable t(L, n);

		for( int i=0; i<n; ++i )
		{
			unsigned int val=0;// use this for all types

			if( g_file.read( (char *)&val, sizeof(T) ).eof() )
				l.exception("Failed to read data from file for 'read' member function");

			t.pushInt( val );
		}
	}
	else
	{
		unsigned int val=0;// use this for all types

		if( g_file.read( (char *)&val, sizeof(T) ).eof() )
			l.exception("Failed to read data from file for 'read' member function");

		l.pushInt( val );
	}

	return 1;
}

int l_read_f32(lua_State *L)
{
	LuaContextBase l(L);
	int args = l.countArguments();
	if( args == 0 || args > 2 )
		l.exception("Wrong number of arguments for 'read' member function");

	if( args == 2 )
	{
		int n = l.getInt( -1 );
		LuaContextTable t(L, n);

		for( int i=0; i<n; ++i )
		{
			float val;

			if( g_file.read( (char*)&val, sizeof(float) ).eof() )
				l.exception("Failed to read data from file for 'read' member function");

			t.pushNum( val );
		}
	}
	else
	{
		float val;

		if( g_file.read( (char*)&val, sizeof(float) ).eof() )
			l.exception("Failed to read data from file for 'read' member function");

		l.pushNum( val );
	}

	return 1;
}

// :size(n=1) template
template<class T>
int l_template_size(lua_State *L)
{
	LuaContextBase l(L);
	int args = l.countArguments();
	int n = 1;

	if( args == 0 || args > 2 )
	{
		l.exception("Wrong number of arguments for 'size' member function");
	}

	if( args == 2 )
	{
		n = l.getInt( -1 );
		l.pop();
	}
	
	l.pushInt( template_size<T>( n ) );

	return 1;
}

// :skip(n=1) template
template<class T>
int l_template_skip(lua_State *L)
{
	LuaContextBase l(L);

	if( !( g_file.good() ) )
		l.exception("Bad data source");

	int args = l.countArguments();
	int n = 1;

	if( args == 0 || args > 2 )
	{
		l.exception("Wrong number of arguments for 'skip' member function");
	}

	if( args == 2 )
	{
		n = l.getInt( -1 );
		l.pop();
	}

	g_file.seekg( template_size<T>( n ), ios::cur );

	return 0;
}

#define MAKE_CALL_TABLE(n,t,readf) \
	luaL_Reg n[] = \
	{ \
		{ "read", readf }, \
		{ "size", l_template_size<t> }, \
		{ "skip", l_template_skip<t> }, \
		{ NULL, NULL } \
	}

// -- unsigned int
MAKE_CALL_TABLE( u32methods,	unsigned int,		l_template_read<unsigned int > );

// -- int
MAKE_CALL_TABLE( s32methods,	int,				NULL );

// -- unsigned short
MAKE_CALL_TABLE( u16methods,	unsigned short,		l_template_read<unsigned short > );

// -- short
MAKE_CALL_TABLE( s16methods,	short,				NULL );

// -- unsigned char
MAKE_CALL_TABLE( u8methods,		unsigned char,		l_template_read<unsigned char > );

// -- char
MAKE_CALL_TABLE( s8methods,		char,				NULL );

// -- float
MAKE_CALL_TABLE( f32methods,	float,				l_read_f32 );

// -- custom type (24-bit number)
//MAKE_CALL_TABLE( u24methods,	u24Base );
