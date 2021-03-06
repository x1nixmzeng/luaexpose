-----------------------------------------------------------------
-- luaexpose
-- v0.01
-----------------------------------------------------------------

--> Demo scripts

require "Vec2"

-- Because the metatable has been exposed 
-- to us, we can actually add new functions
-- to Foo

function Foo:speak()
    print("Hello, I am a Foo")
end

local fred = Foo.new("fred")
fred:speak()

Foo.add_ = Foo.add

function Foo:add(a,b)
	return self:add_(a,b)
end

function luaFoo()
	local myfoo = Foo.new("bar")
	print("bar says", myfoo:add(1,2))
end

print( fred:add(100, -50) )

luaFoo()

vertexBuffer = {}

-- Points at the edges of the screen
table.insert( vertexBuffer, Vec2Origin )
table.insert( vertexBuffer, Vec2:new( 800, 0 ) )
table.insert( vertexBuffer, Vec2:new( 800, 600 ) )
table.insert( vertexBuffer, Vec2:new( 0, 600 ) )

setVtxColour( 1, 0, 0 )

pushVtxBuffer( vertexBuffer )

setVtxColour( 0.9, 0.9, 0.9 )

-----------------------------------------------------------------
--> We can output messages to the console window
-----------------------------------------------------------------
print("Hello from the lua script!")
print( LUAEXPOSE_DESC )


-----------------------------------------------------------------
--> Setting a single point
-----------------------------------------------------------------
pushVtx( 30, 10 )



-----------------------------------------------------------------
--> Fetching a single point
-----------------------------------------------------------------

x,y = getVtx( 0 )
pushVtx( x - 20, y )


-----------------------------------------------------------------
--> Setting a row of points using a loop
-----------------------------------------------------------------
for i=0,100,20 do
  pushVtx( 10 + i, 10 + 20 )
end



-----------------------------------------------------------------
--> Setting a clockface using some math library functions
-----------------------------------------------------------------
setVtxColour( 1, 0.5, 0 )

for i=0,360,30 do
  x = math.sin( math.rad( i ) ) * 100
  y = math.cos( math.rad( i ) ) * 75

  pushVtx( 400 + x, 300 + y )
end

