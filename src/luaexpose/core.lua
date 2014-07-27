-- luaexpose
-- core.lua

------------------------------------------
file = "aid_model_ghoulies_background_eastwingdown_snookerroom.model"
show = SHOW_LINES_AND_FACES
--show = SHOW_POINTS

faceColor( 20, 20, 20 )
lineColor( 100, 100, 100 )

------------------------------------------

function log(...) io.write(...) end


-- updated

function readModel(vpos, vsize, fpos, fsize)
	seek(vpos)
	
    vdata = {}
	for verts=1,vsize do
		vec3 = f32:read(3)
		table.insert(vdata,vec3)
	end
	
	seek(fpos)
	fdata = {}
	for faces=1,fsize do
		vec3 = u32:read(3)
		table.insert(fdata,vec3)
	end

	--move(-125,-25,-41)
	setVTable( vdata )		--> must be first
	setFITable( fdata )		--> todo: face type (tri_strip, etc)
end

function readModel1()
	readModel(800504, 100, 803912, 184);
end

function readModel2()
	readModel(794888, (1200/12), 798296, (2208/12));
end

function readModel3()
	readModel(793424, (312/12), 794312, (576/12));
end

function readModel4()
	readModel(789272, (888/12), 791792, (1632/12));
end

function readModel5()
	readModel(783656, (1200/12), 787064, (2208/12));
end

function readModel6()
	readModel(781832, (384/12), 782936, (720/12));
end

function readModel7()
	readModel(780128, (360/12), 781160, (672/12));
end

function main()
	log("Got ", size(), " bytes!\n")

    readModel1() -- top left panel
	readModel2() -- bottom left panel
	readModel3() -- single block (bottom right)
	readModel4() --bottom right panel
	readModel5() -- top right panel
	readModel6() -- right shield
	readModel7() -- left shield
	
	
	move(-125,-25,-41)
	
end
