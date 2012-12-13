/*
	exposedTypes
	Written by x1nixmzeng

	Templates (incomplete) to define the common base type properties
*/

union u24Base
{
	int val;
	struct{ unsigned char val1, val2, val3; };

	int toInt( ) const{ return( val ); }
};

template<class T> inline int template_size( int n )
{
	return( sizeof( T ) * n );
}

template<> inline int template_size<u24Base >( int n )
{
	return( 3 * n );
}

// :read(n=1) template
template<class T>
int l_template_read(lua_State *l)
{
	LuaContextBase len(l);
	int args = lua_gettop(l);
	int n = 1;

	if( args == 0 || args > 2 )
	{
		len.exception("Wrong number of arguments for 'read' member function");
	}

	if( args == 2 )
	{
		n = len.getIntegerFromStack( -1 );
	}
	
	printf("Reading %i bytes..\n", template_size<T>( n ) );
	
	len.push( 9000 );
	// todo: push table of values

	return 1;
}

// :size(n=1) template
template<class T>
int l_template_size(lua_State *l)
{
	LuaContextBase len(l);
	int args = lua_gettop(l);
	int n = 1;

	if( args == 0 || args > 2 )
	{
		len.exception("Wrong number of arguments for 'size' member function");
	}

	if( args == 2 ) // self + argument
	{
		//luaL_checktype( l, 0, LUA_TNUMBER);
		n = len.getIntegerFromStack( -1 );
	}
	
	len.push( template_size<T>( n ) );

	return 1;
}

// :skip(n=1) template
template<class T>
int l_template_skip(lua_State *l)
{
	LuaContextBase len(l);
	int args = lua_gettop(l);
	int n = 1;

	if( args == 0 || args > 2 )
	{
		len.exception("Wrong number of arguments for 'skip' member function");
	}

	if( args == 2 )
	{
		n = len.getIntegerFromStack( -1 );
	}
	
	printf("Skipping %i bytes..\n", template_size<T>( n ) );

	return 0;
}

#define MAKE_CALL_TABLE(n,t) \
	luaL_Reg n[] = \
	{ \
		{ "read", l_template_read<t> }, \
		{ "size", l_template_size<t> }, \
		{ "skip", l_template_skip<t> }, \
		{ NULL, NULL } \
	}

// -- unsigned int
MAKE_CALL_TABLE( u32methods,	unsigned int );

// -- int
MAKE_CALL_TABLE( s32methods,	int );

// -- unsigned short
MAKE_CALL_TABLE( u16methods,	unsigned short );

// -- short
MAKE_CALL_TABLE( s16methods,	short );

// -- unsigned char
MAKE_CALL_TABLE( u8methods,		unsigned char );

// -- char
MAKE_CALL_TABLE( s8methods,		char );

// -- float
MAKE_CALL_TABLE( f32methods,	float );

// -- custom type (24-bit number)
MAKE_CALL_TABLE( u24methods,	u24Base );
