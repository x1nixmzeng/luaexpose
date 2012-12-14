-- core.lua

--> automatically reload the script when it is saved
autoReload()


print("Hello from ", LUAEXPOSEDESC, "!")

t = sampleTable()								-- {100, 101, 102}

for i=1, #t do
	print(t[i])
end

print("result = ", #t)							-- 3

print("size of 1x   u32 = ", u32:size())		-- 4
print("size of 1x   u8  = ", u8:size())			-- 1
print("size of 80x  u32 = ", u32:size(80))		-- 320
print("size of 160x u8  = ", u8:size(160))		-- 160
print("size of 1x   u24 = ", u24:size())		-- 3
print("size of 80x  u24 = ", u24:size(80))		-- 240

f32:read()
u8:skip(8)

amm = u32:read(10)		-- this should instead return a table
f32:read(amm)			--> f32:read(amm[1])

print( str:read() )		-- read a null-terminated string
print( str:read(18) )	-- read a fixed-length string
print( #str:read(18) )	-- get length of string (either nt or fl)
