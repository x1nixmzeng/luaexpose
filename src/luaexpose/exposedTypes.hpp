/*
	exposedTypes
	Written by x1nixmzeng

	Templates (incomplete) to define the common base type properties
*/

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
	
	len.push( static_cast<int>( sizeof(T) * n ) );

	// push str: too many arguments
	//lua_error( l );

	return 1;
}

// -- unsigned int
luaL_Reg u32methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<unsigned int> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- int
luaL_Reg s32methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<int> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- unsigned short
luaL_Reg u16methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<unsigned short> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- short
luaL_Reg s16methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<short> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- unsigned char
luaL_Reg u8methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<unsigned char> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- char
luaL_Reg s8methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<char> },
	{ "skip", NULL },
	{ NULL, NULL }
};

// -- float
luaL_Reg f32methods[] =
{
	{ "read", NULL },
	{ "size", l_template_size<float> },
	{ "skip", NULL },
	{ NULL, NULL }
};

