// Constructors
Vec2( )
	: x( 0 )
	, y( 0 )
{}
Vec2( T xVal, T yVal )
	: x( xVal )
	, y( yVal )
{}
Vec2( T ini )
	: x( ini )
	, y( ini )
{}
Vec2( const Vec2& c )
	: x( c.x )
	, y( c.y )
{}

// Assignment
Vec2 operator=( const Vec2 &c )
{
	x = c.x;
	y = c.y;
	return( *this );
}

// Comparison
bool operator==( const Vec2 &r )
{
	return( ( x == r.x ) && ( y == r.y ) );
}

// Add to
Vec2 operator+=( const Vec2 &c )
{
	x += c.x;
	y += c.y;
	return( *this );
}

// Subtract from
Vec2 operator-=( const Vec2 &c )
{
	x -= c.x;
	y -= c.y;
	return( *this );
}

// Multiply by
Vec2 operator*=( const Vec2 &c )
{
	x *= c.x;
	y *= c.y;
	return( *this );
}

// Multiply by (scalar)
Vec2 operator*=( T val )
{
	x *= val;
	y *= val;
	return( *this );
}

// Divide by
Vec2 operator/=( const Vec2 &c )
{
	x /= c.x;
	y /= c.y;
	return( *this );
}

// Divide by (scalar)
Vec2 operator/=( T val )
{
	x /= val;
	y /= val;
	return( *this );
}

Vec2 operator+( const Vec2 &r )
{
	*this += r;
	return( *this );
}

Vec2 operator-( const Vec2 &r )
{
	*this -= r;
	return( *this );
}

Vec2 operator*( const Vec2 &r )
{
	*this *= r;
	return( *this );
}

Vec2 operator/( const Vec2 &r )
{
	*this /= r;
	return( *this );
}
