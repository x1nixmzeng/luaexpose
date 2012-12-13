-- parsing.lua
-- NON-FUNCTIONING SPECIFICATION

-- handling types (s32/u32,s16/u16,s8/u8)
--u32:little() or u32:big()
--u32:read() or u32:read(4)
--u32:size() or u32:size(4)
--u32:skip() or u32:skip(4)


-- HEADER
u32:skip()			-- ignore 1 integer value (magic)
vs = u32:read(1)	-- read the vertex count

seek( 1024 )		-- skip to first data

-- VERTEX DATA
points = {}

mark("Vertex data")	-- create a marker (debug string with position)

for i=0,vs do
	p = f32:read(3) -- creates a table of 3 floats
	skip(4)			-- skip 4 bytes

	table.insert(points,p)
end

-- RENDERING
-- todo

