-----------------------------------------------------------------
-- luaexpose
-- v0.01
-----------------------------------------------------------------

--> Vec2 class

Vec2 = {}
mt__Vec2 = { __index = Vec2 }

function Vec2:new(x, y)
	return setmetatable( {x, y}, mt__Vec2)
end

--> Vec2 constants

Vec2Origin = Vec2:new( 0, 0 )
