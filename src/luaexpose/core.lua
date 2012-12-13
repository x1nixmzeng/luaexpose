-- core.lua

print("Hello from ", LUAEXPOSEDESC, "!")

t = sampleTable()

for i=1, #t do
	print(t[i])
end

print("result = ", #t)

print("size of 1x   u32 = ", u32:size())
print("size of 1x   u8  = ", u8:size())
print("size of 80x  u32 = ", u32:size(80))
print("size of 160x u8  = ", u8:size(160))
