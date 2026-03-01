# Tool directories (if they are in PATH, directory-less invocations will work here too).
compiler_dir=avr-gcc
objcopy_dir=avr-objcopy
sreccat_dir=srec_cat

# Change these if you're using anything other than an arm64 Mac. x86_64 Macs use different include directories,
# and I can't remember where Windows stores things!
compiler_incs=( "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/lib/avr-gcc/9/gcc/avr/9.3.0/include"
                "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/lib/avr-gcc/9/gcc/avr/9.3.0/include-fixed"
                "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/avr/include")

# Other variables. Change this if your clock freq or microcontroller have changed.
all_dirs=("./bin" "./main_board/bin" "./main_board/obj" "./main_board_bootloader/bin" "./main_board_bootloader/obj")
mcu_freq=8000000UL
mcu_type=atmega328pb
boot_offset=0x7000

## 1. PREPARE DIRECTORIES.
echo "Preparing directories..."

# Delete all dirs first, then create them.
for dir in ${all_dirs[@]}
do
    if [ -d $dir ]
    then
        rm -rf $dir
        if [ $? -eq 0 ]
        then
            echo "  successfully removed \"$dir\"!"
        else
            echo "  failed!"
            exit 1
        fi
    fi
    mkdir -p $dir
done

## 2. APPLICATION.
echo "Building application..."
cd ./main_board/src

compiler_app="$compiler_dir -DF_CPU=$mcu_freq"
compiler_app+=" -mmcu=$mcu_type -pipe -std=gnu99 -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections"

for path in ${compiler_incs[@]}
do
    compiler_app+=" -I$path"
done

# Compile (but don't link).
for file in *.c
do
    $compiler_app -O2 -c $file -o ../obj/$file.o
done

cd ../../common
for file in *.c
do
    $compiler_app -O2 -c $file -o ../main_board/obj/$file.o
done
cd ../main_board

# Now link here.
$compiler_app -Wall -g -fuse-linker-plugin -Wl,--gc-sections -lm ./obj/*.c.o -o ./bin/app.elf
if [ $? -ne 0 ]
then
    echo "  failed!"
    exit 1
fi

# Convert to .hex and report success.
$objcopy_dir -O ihex ./bin/app.elf ../bin/app.hex
rm -rf ./obj
rm -rf ./bin
echo "  done!"

## 3. BOOTLOADER.
echo "Building bootloader..."
cd ../main_board_bootloader/src

compiler_bl="$compiler_dir -DF_CPU=$mcu_freq"
compiler_bl+=" -mmcu=$mcu_type -pipe -std=gnu99 -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections"

for path in ${compiler_incs[@]}
do
    compiler_bl+=" -I$path"
done

# Compile (but don't link).
for file in *.c
do
    $compiler_bl -Os -c $file -o ../obj/$file.o
done

cd ../../common
for file in *.c
do
    $compiler_app -Os -c $file -o ../main_board_bootloader/obj/$file.o
done
cd ../main_board_bootloader

# Link with the bootloader offset.
$compiler_bl -Wall -g -fuse-linker-plugin -Wl,--gc-sections -Wl,--section-start=.text=$boot_offset -lm ./obj/*.c.o -o ./bin/bl.elf
if [ $? -ne 0 ]
then
    echo "  failed!"
    exit 1
fi

# Convert to .hex and report success.
$objcopy_dir -O ihex ./bin/bl.elf ../bin/bl.hex
rm -rf ./obj
rm -rf ./bin
echo "  done!"

## 4. MERGED BINARY
echo "Merging bootloader with application..."
cd ../bin
$sreccat_dir app.hex -I bl.hex -I -o merged.hex -I
if [ $? -ne 0 ]
then
    echo "  failed!"
    exit 1
else
    echo "  done!"
fi

## 5. UPGRADER FORMAT
echo "Creating .bin file of application..."
$sreccat_dir app.hex -I -o app.bin -Binary
if [ $? -ne 0 ]
then
    echo "  failed!"
    exit 1
fi
rm app.hex

echo "  done!"