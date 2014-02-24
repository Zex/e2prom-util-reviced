platform=Native
version=debug
LD_LIBRARY_PATH=build/$platform/$version
inf=ref/fanu_eeprom.txt
outf=fanu.bin

build/$platform/$version/ascii2bin $inf $outf

#xxd ref/eeprom_in.bin > my
