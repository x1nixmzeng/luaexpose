// Constructors
Vec3( )
	: x( 0 )
	, y( 0 )
	, z( 0 )
{}
Vec3( T xVal, T yVal, T zVal )
	: x( xVal )
	, y( yVal )
	, z( zVal )
{}
Vec3( T ini )
	: x( ini )
	, y( ini )
	, z( ini )
{}
Vec3( const Vec3& c )
	: x( c.x )
	, y( c.y )
	, z( c.z )
{}

// Assignment
const Vec3 &operator=( const Vec3 &c )
{
	x = c.x;
	y = c.y;
	z = c.z;
	return( *this );
}

// Comparison
bool operator==( const Vec3 &r )
{
	return( ( x == r.x ) && ( y == r.y ) && ( z == r.z ) );
}

// Add to
const Vec3 &operator+=( const Vec3 &c )
{
	x += c.x;
	y += c.y;
	z += c.z;
	return( *this );
}

// Subtract from
const Vec3 &operator-=( const Vec3 &c )
{
	x -= c.x;
	y -= c.y;
	z -= c.z;
	return( *this );
}

// Multiply by
const Vec3 &operator*=( const Vec3 &c )
{
	x *= c.x;
	y *= c.y;
	z *= c.z;
	return( *this );
}

// Multiply by (scalar)
const Vec3 &operator*=( T val )
{
	x *= val;
	y *= val;
	z *= val;
	return( *this );
}

// Divide by
const Vec3 &operator/=( const Vec3 &c )
{
	x /= c.x;
	y /= c.y;
	z /= c.z;
	return( *this );
}

// Divide by (scalar)
const Vec3 &operator/=( T val )
{
	x /= val;
	y /= val;
	z /= val;
	return( *this );
}

const Vec3 &operator+( const Vec3 &r )
{
	*this += r;
	return( *this );
}

const Vec3 &operator-( const Vec3 &r )
{
	*this -= r;
	return( *this );
}

const Vec3 &operator*( const Vec3 &r )
{
	*this *= r;
	return( *this );
}

const Vec3 &operator/( const Vec3 &r )
{
	*this /= r;
	return( *this );
}

