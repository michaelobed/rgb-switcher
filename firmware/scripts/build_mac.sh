# Tool directories (if they are in PATH, directory-less invocations will work here too).
compiler_dir=avr-gcc
objcopy_dir=avr-objcopy

# Change these if you're using anything other than an arm64 Mac. x86_64 Macs use different include directories,
# and I can't remember where Windows stores things!
compiler_incs=( "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/lib/avr-gcc/9/gcc/avr/9.3.0/include"
                "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/lib/avr-gcc/9/gcc/avr/9.3.0/include-fixed"
                "/opt/homebrew/Cellar/avr-gcc@9/9.3.0_3/avr/include")

# Other variables. Change this if your clock freq or microcontroller have changed.
all_dirs=("./bin" "./src/bin" "./src/obj")
mcu_freq=8000000UL
mcu_type=atmega328pb

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
cd ./src

compiler_app="$compiler_dir -DF_CPU=$mcu_freq"
compiler_app+=" -mmcu=$mcu_type -pipe -std=gnu99 -Wall -Wstrict-prototypes -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections"

for path in ${compiler_incs[@]}
do
    compiler_app+=" -I$path"
done

# Compile (but don't link).
for file in *.c
do
    $compiler_app -O2 -c $file -o ./obj/$file.o
done

# Now link here.
$compiler_app -Wall -g -fuse-linker-plugin -Wl,--gc-sections -lm ./obj/*.c.o -o ./bin/app.elf
if [ $? -ne 0 ]
then
    echo "  failed!"
    exit 1
fi

# Convert to .hex and report success.
$objcopy_dir -O ihex ./bin/app.elf ../bin/app.hex
rm ./bin/app.elf
rm -rf ./obj
rm -rf ./bin
echo "  done!"