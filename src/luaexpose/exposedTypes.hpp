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
	the :read(n=1) method is too ambiguous with templates
*/

int l_read_u32(lua_State *L)
{
	LuaContextBase l(L);
	int args = l.countArguments();
	int n = 1;

	if( args == 0 || args > 2 )
		l.exception("Wrong number of arguments for 'read' member function");

	if( args == 2 )
		n = l.getIntegerFromStack( -1 );
	
	
	int table = l.createTable(n);
	int index=1;

	for( int i=0; i<n; ++i )
	{
		unsigned int val=0;// use this for all types

		int result = fread((void*)&val,1,sizeof(unsigned int),g_fHandle);
		if (result != sizeof(unsigned int))
			l.exception("Failed to read data from file for 'read' member function");

		l.pushTableElement( val, table, index );
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
		n = l.getIntegerFromStack( -1 );
	
	l.push( template_size<T>( n ) );

	return 1;
}

// :skip(n=1) template
template<class T>
int l_template_skip(lua_State *L)
{
	LuaContextBase l(L);

	if( g_fHandle == nullptr )
		l.exception("Bad data source");

	int args = l.countArguments();
	int n = 1;

	if( args == 0 || args > 2 )
	{
		l.exception("Wrong number of arguments for 'skip' member function");
	}

	if( args == 2 )
		n = l.getIntegerFromStack( -1 );

	// No others checks made
	fseek( g_fHandle, template_size<T>( n ), SEEK_CUR );

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
MAKE_CALL_TABLE( u32methods,	unsigned int,		l_read_u32 );

// -- int
MAKE_CALL_TABLE( s32methods,	int,				NULL );

// -- unsigned short
MAKE_CALL_TABLE( u16methods,	unsigned short,		NULL );

// -- short
MAKE_CALL_TABLE( s16methods,	short,				NULL );

// -- unsigned char
MAKE_CALL_TABLE( u8methods,		unsigned char,		NULL );

// -- char
MAKE_CALL_TABLE( s8methods,		char,				NULL );

// -- float
MAKE_CALL_TABLE( f32methods,	float,				NULL );

// -- custom type (24-bit number)
//MAKE_CALL_TABLE( u24methods,	u24Base );
