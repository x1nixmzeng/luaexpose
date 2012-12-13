-- core.lua

print("Hello from ", LUAEXPOSEDESC, "!")

buf = {}

for i=100,500,20 do
	--table.insert(buf, {0,i})
	--table.insert(buf, {i,0})
	table.insert(buf, {i,i})
end

pushVtxBuffer( buf )
