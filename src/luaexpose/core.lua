-- luaexpose
-- core.lua

------------------------------------------
file = "Shaiya/demf_boots001.3DC"
show = SHOW_LINES_AND_FACES

faceColor( 20, 20, 20 )
lineColor( 100, 100, 100 )

------------------------------------------

function log(...) io.write(...) end


function main()
	log("Got ", size(), " bytes!\n")
	
	-- skip blank field
	u32:skip()

	-- skip bones
	numBones = u32:read()
	f32:skip(16*numBones)

	-- read vertices
	numVerts = u32:read()
	log("Verts: ", numVerts, "\n")
	
	vdata = {}

	for vert=1,numVerts do
		vec3 = f32:read(3)
		table.insert(vdata,vec3)

		f32:skip()
		u8:skip(4)
		f32:skip(3+2)
	end

	-- read faces
	numFaces = u32:read()
	log("Faces: ", numFaces, "\n")

	fdata = {}

	for faces=1,numFaces do
		vec3 = u16:read(3)
		table.insert(fdata,vec3)
	end

	--rotateScene(180, 0, -1, 1)

	setVTable( vdata )		--> must be first
	setFITable( fdata )		--> todo: face type (tri_strip, etc)
end
