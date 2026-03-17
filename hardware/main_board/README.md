# rgb switcher main board

Here's the KiCad design file for the main board. I've included all required libraries as a lot of the components I had to find or make from scratch. I've also included a bill of materials (BOM) for reference, which you will need the rather excellent [KiCAD JLCPCB tools](https://github.com/bouni/kicad-jlcpcb-tools) plugin to use. I've done this so that JLCPCB-compatible BOM and CPL files can be directly exported without fuss.

Bear in mind that you don't absolutely have to use the parts referenced in there if they aren't in stock on LCSC, or if you can source them from elsewhere, or if better/cheaper alternatives will work.

FWIW, here are some alternative links to things you will need. I used AliExpress myself, but these should be equivalent:

- [Output indicator LEDs](https://uk.farnell.com/multicomp-pro/mcl056purgw/led-5mm-ac-hi-red-grn/dp/1581207) (Farnell)
- [+5v in/out connector](https://uk.farnell.com/camdenboss/ctb932hd-2/terminal-block-header-2pos-th/dp/2493677) (Farnell)
- [Video input/output connectors](https://www.digikey.co.uk/en/products/detail/kycon-inc/KMDGX-9S-BS-1/10127398?srsltid=AfmBOoohKXPbVtalDInVmYf89jq7ZdBaHLBsx2CvSOdNHBqDBLMhTcrD) (DigiKey)

## changelog

### v0.1

- initial version. mistakes were made. 🥲

### v0.2

- corrected reversed pinout of icsp connector.
- corrected reversed pinout of uart connector.
- corrected orientation of prev/next buttons.
- corrected "output on" leds so rgb is red and ypbpr is green.
- corrected stupid pin numbering of mini-din footprint.
- added holes to 9-pin mini-din footprint to prevent ripping connector off board during disconnection.
- moved rgb/ypbpr switches closer to edge for ease of use.
- reduced brightness of "output on" leds.
- permitted switching between pins 4 and 5 of the input/output connectors to support using non-rgbs cables.
- connected PIN_SELEN net to front LED line decoders to allow them to be switched off.

### v1.0

- removed led functionality so it functions only as a video switcher.
- reduced the board dimensions to 135x226mm.
- moved prev/next buttons closer to edge for ease of use.
- migrated bom to [KiCAD JLCPCB tools](https://github.com/bouni/kicad-jlcpcb-tools) plugin format for ease of exporting bom and cpl files.
- lots of routing improvements
