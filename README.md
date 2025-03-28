# rgb-switcher

![Render of v0.2 PCB](https://github.com/user-attachments/assets/11b07148-e02a-4746-8927-08d80e3dff86)

## What is it?

An open-source switcher for analogue RGBS and YPbPr video, so you don't have to.

## Why?

My wife and I are slowly amassing some retro gaming consoles which we quite enjoy, and needed a way to connect them all to a modern TV via a [gbs-control](https://github.com/ramapcsx2/gbs-control)-based line doubler. Unfortunately, most solutions for this either don't support RGBS, don't support YPbPr (a must for certain consoles), are SCART-only, don't have many inputs or are ludicrously expensive! Worst of all, they often don't feature remote control of any kind.

Soooo I did what any irritated engineer would do and made my own. 😂

## Overview

### Video switching

The rgb switcher is based around a very old Ben Heck design which uses [74CBT3244](https://www.ti.com/lit/ds/symlink/sn74cbt3244.pdf?ts=1737705833521&ref_url=https%253A%252F%252Fwww.google.com%252F) FET bus switches. The idea is simple: an [ATMEGA328PB](https://www.microchip.com/en-us/product/atmega328pb) microcontroller selects which of 8 of these bus switches should be active and switch their inputs through to some commonly connected outputs. The inputs consist of red, green, blue, sync, left audio and right audio. Since some of our consoles use YPbPr video, and because gbs-control-based line doublers have separate inputs for RGB(S) and YPbPr video, there are separate outputs for each, also selected with their own bus switch. A slide switch on the front of the PCB lets you select which video format each input uses, and therefore which output it should be routed to when selected. Bicolour LEDs also indicate which input is active and which format the video is in (red for RGBS, green for YPbPr).

I forgot to mention when writing this that the red, green and blue signals become Pr, Y and Pb respectively when an input is configured to be YPbPr instead of RGBS. There is no sync signal on the YPbPr output. 👍🏾

The input bus switches are selected using [74HC138](https://www.ti.com/product/CD74HC138) inverting line decoders. I could have just used pins on the microcontroller, but these lines go multiple places and I already use a hex inverter for something else on the board and didn't feel like having multiple!

I've used the same 9-pin mini-DIN connector that the Sega Mega Drive II uses, using the exact same pinout. One irritation that I discovered while deciding on this is that the cables I bought for testing (from AliExpress, admittedly) don't output the sync signal on pin 5, but instead output the composite video signal on pin 4! I've therefore added little switches to each input and to the RGBS output that let you choose pin 4 or 5 of the connector, and thus what you'd like to use as a sync signal. Be aware that there isn't any capacity to strip the sync signal from a composite video signal, both because I wasn't designing for that and because gbs-control does it already. If you like, you can add an [LM1881](https://www.ti.com/lit/ds/symlink/lm1881.pdf?ts=1737753066078&ref_url=https%253A%252F%252Fwww.google.com%252F) to the board if that's needed. This _doooes_ also imply that you could use this to switch composite video. Ha!

I got the cables I used for input/output from AliExpress, for example [this one](https://www.aliexpress.com/item/1005005996147435.html?spm=a2g0o.productlist.main.1.225e74cfXCYG5h&algo_pvid=0f9260bc-c9ae-4323-ab62-fcc3ea8f7ba0&algo_exp_id=0f9260bc-c9ae-4323-ab62-fcc3ea8f7ba0-0&pdp_npi=4%40dis%21GBP%217.65%217.09%21%21%219.18%218.51%21%402103956b17378184189561915e6008%2112000035228301718%21sea%21UK%211672698285%21X&curPageLogUid=Uh0iq4dljLo4&utparam-url=scene%3Asearch%7Cquery_from%3A), but the cables typically look like this:

![What an I/O cable looks like](https://github.com/user-attachments/assets/2ac21df5-6459-4b52-92e9-456604fd24ca)

You can obtain these by searching for "sega mega drive 2 av cable" and you'll usually find one. You can then put [generic RCA couplers](https://www.aliexpress.com/item/32996572551.html?spm=a2g0o.productlist.main.3.5ff03bcaiFP645&algo_pvid=01b01cbc-0159-45f0-845c-6822facf72eb&algo_exp_id=01b01cbc-0159-45f0-845c-6822facf72eb-1&pdp_npi=4%40dis%21GBP%211.43%211.43%21%21%211.71%211.71%21%402103867617378185427874434e86a9%2166938801173%21sea%21UK%211672698285%21X&curPageLogUid=OJLO7MVlofdW&utparam-url=scene%3Asearch%7Cquery_from%3A) on them to turn them into input cables.

### RGB LEDs

I also wanted the consoles to look nice on shelves or some sort of storage system we haven't decided on yet, so why not have the select lines also illuminate strips of 12V RGB LEDs? 🤣
I added some [IRF540](https://www.vishay.com/docs/91021/irf540.pdf) power MOSFETs to help with this. MOSFETs because efficient, power because current through the LEDs does add up! The LED outputs are selected using a [74HC238](https://www.ti.com/product/CD74HC238) non-inverting line decoder.

They also fade in/out when changing inputs. Yey! ^-^

### UART

Initially, I was going to build into the board a radio transceiver and build a separate board to use as a remote, partly because it's better than IR and because I had some nRF24L01 modules in my parts bin not really doing anything! It turns out that I hate them, powering them is finicky and I don't want to use an Arduino library, so I provided a generic UART interface instead. That way, I can design a remote of some kind in future (or maybe you, most esteemed reader, are the solution! 😉). I've also left a 5V power output near the UART connector so external remote interfaces can be powered.

I've left some brief documentation in the firmware README. The rgb switcher sends strings whenever the prev/next buttons are pressed and responds to external commands given using simple strings. You can even change the colour associated with an input this way!

Be warned, though, that there is no string sanitisation of any kind, so you can probably crash the microcontroller by being a silly billy with what you send it.

### Power

Finally, there's the power input. Anything that supplies 12V at ≥3A is fine, the high current being to drive the LEDs. Be sure to use a centre-positive DC barrel jack with a 5.5mm outer diameter and a 2.5mm inner diameter. I haven't provided any reverse voltage protection because I genuinely forgot. Perhaps a Schottky diode in series with the 12V rail will do the trick.

## Final notes

I'm not an expert PCB designer, and this is my first time doing anything with video, but I managed to succesfully test this and get it working! I did make a hilarious number of really dumb mistakes which you can read in the changelog in the hardware README (yes I did eventually rewire the 9-pin mini-DINs by hand to test them). Despite all this, video and audio quality should be good.

If you like this design, you're welcome (and encouraged!) to fork this repo and make one yourself, modify it, sell it, I don't mind!

Happy gaming!

## In future

These are things I should probably have done but haven't had the time to do yet:

- **Make the board smaller**! It would probably have been better to make the switcher with an "LED board" as a module that communicates over the UART, SPI, I2C, etc. Most importantly, it brings cost down further to not make such a large 4-layer PCB, especially if you're not interested in the RGB LED bit.
- **Create a reference design for a remote board** that lets you use one of those universal IR TV remotes with it, or maybe even learn codes from an existing IR remote.
- **Create an enclosure**. I'm a CAD noob but it shouldn't be too hard. It could even just be an identically-shaped PCB with some elaborate artwork on it!
