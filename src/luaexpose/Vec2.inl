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
const Vec2 &operator=( const Vec2 &c )
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
const Vec2 &operator+=( const Vec2 &c )
{
	x += c.x;
	y += c.y;
	return( *this );
}

// Subtract from
const Vec2 &operator-=( const Vec2 &c )
{
	x -= c.x;
	y -= c.y;
	return( *this );
}

// Multiply by
const Vec2 &operator*=( const Vec2 &c )
{
	x *= c.x;
	y *= c.y;
	return( *this );
}

// Multiply by (scalar)
const Vec2 &operator*=( T val )
{
	x *= val;
	y *= val;
	return( *this );
}

// Divide by
const Vec2 &operator/=( const Vec2 &c )
{
	x /= c.x;
	y /= c.y;
	return( *this );
}

// Divide by (scalar)
const Vec2 &operator/=( T val )
{
	x /= val;
	y /= val;
	return( *this );
}

const Vec2 &operator+( const Vec2 &r )
{
	*this += r;
	return( *this );
}

const Vec2 &operator-( const Vec2 &r )
{
	*this -= r;
	return( *this );
}

const Vec2 &operator*( const Vec2 &r )
{
	*this *= r;
	return( *this );
}

const Vec2 &operator/( const Vec2 &r )
{
	*this /= r;
	return( *this );
}
