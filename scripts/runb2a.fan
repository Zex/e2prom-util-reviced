platform=Native #AEOS_Intel
version=debug
LD_LIBRARY_PATH=build/$platform/$version

build/$platform/$version/bin2ascii fanu.bin ref/xtype.ini fanu.ascii

#xxd ref/eeprom_in.bin > my
