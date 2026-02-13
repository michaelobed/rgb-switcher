# rgb switcher firmware

This firmware was written for the Microchip ATMEGA328PB. It was initially tested on an ATMEGA328P, which should be pin-compatible. Actually, with a little bit of work, this will probably run on anything, so use anything you like if the world one day runs out of those.

## How to build

You will need:
- A text editor. I use [Visual Studio Code](https://code.visualstudio.com) and have added a `.vscode` folder to the repo.
- The AVR GCC toolchain. How you get this depends on your OS, but if you're on macOS like me, I recommend [this repo](https://github.com/osx-cross/homebrew-avr) which contains a guide using a Homebrew-based approach.
- AVRDude. As with AVR GCC, you will need to get this according to your OS's method of doing that. If you've installed Arduino, you likely already have it. If you're on macOS and used the guide from earlier, you can install it by typing `brew install avrdude` in Terminal.

If you happen to be on an arm64 Mac, you should now be able to do either of the following:
- Open `firmware/main_board/src` in Visual Studio code, then run the _Build (Mac)_ task.
- In Terminal, do `cd firmware/main_board` then `../scripts/build_mac.sh`, which achieves the same thing.

This will build a .hex file to `firmware/main_board/bin`, which you can now flash using AVRDude.

If you're not using an arm64 Mac, you will need to change some directories in the `build_mac.sh` script, or create an equivalent for your platform. If you do happen to be on a Windows machine, please feel free to contribute a batch script or equivalent to the repo!

## UART protocol

The UART can receive commands from other devices and respond to them with an acknowledgement. This is useful both for remotely controlling the rgb switcher and for connected devices finding out its latest state (e.g. if the prev/next buttons are pressed). A command-response system has been implemented for controlling the rgb switcher and uses human-readable ASCII characters for everything.

The UART is configured as 8N1, running at 38400 kbaud.

The list of currently available commands is given below:

| Command | Format | Description |
| --- | --- | --- |
| Previous input | `p` | Decrements currently selected input. |
| Next input | `n` | Increments currently selected input. |
| Switch to input | `sN` | Switches to input `N`. |
| No input | `o` | Deselects all inputs. |
| Change input colour | `cNRRGGBB` | Changes the RGB LED colour associated with input `N` to the colour `RRGGBB`, where `RR`, `GG` and `BB` are 8-bit hexadecimal numbers representing the red, green and blue values of the colour. |
| Acknowledgement | `aCP` | Acknowledgement of the command `C` and its parameters `P`. `P` can be any length depending on the command being acknowledged. |
| Hello | `h` | Currently unused, but could be used to detect the presence of other devices. |

All strings sent or received are required to end in a newline (`\n`) character. Windows style `\r\n` endings are fine as the `\r` will just be ignored. As mentioned in the main README, no string sanitisation is performed, and no bounds checking either, so if you supply a malformed string or pass in an out-of-bounds input index...well I'm sure you can handle it! 😅

Feel free to extend this and make cool remotes and interfaces and things!

## EEPROM memory map

| Range | Description |
| --- | --- |
| 0x0000 - 0x000f | No man's land! It's apparently unreliable, so avoid. |
| 0x0010 - 0x0013 | Signature, known value used to verify that data exists. |
| 0x0020 - 0x003f | RGB LED colour data for each input (4 bytes * 8). |
