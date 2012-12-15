-- core.lua

--> automatically reload the script when it is saved
autoReload()
nowindow()

if hasData() then
	print("Data size is ", size(), " bytes")

	print(u32:read()[1])

	seek(68);

	print(str:read(100)) -- SWITCH_WALL.pcx

else
	print("Data was not loaded!")
end
