-- luaexpose
-- core.lua

------------------------------------------
file = "Shaiya/demf_boots001.3DC"
------------------------------------------

function log(...) io.write(...) end

function main()
	log("Got ", size(), " bytes\n")
	
	-- skip 4 bytes
	u32:skip()

	-- read bones
	numBones = u32:read()
	f32:skip(16*numBones)

	-- read vertices
	numVerts = u32:read()
	log("Verts: ", numVerts, "\n")
	
	vdata = {}

	for vert=1,numVerts do
		vec3 = f32:read(3)
		--vec3[1] = vec3[1]*100
		--vec3[2] = vec3[2]
		--vec3[3] = vec3[3]*100
		table.insert(vdata,vec3)

		f32:skip()
		u8:skip(4)
		f32:skip(3+2)
	end

	pushVtxBuffer( vdata )
	log("Done\n");
end
