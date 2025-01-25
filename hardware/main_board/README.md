# rgb switcher main board

Here's the KiCad design file for the board. I've included all required libraries as a lot of the components I had to find or make from scratch. I've also included a bill of materials (BOM) for reference, made to be compatible with JLCPCB's particular format. Bear in mind that you don't absolutely have to use the parts referenced in there if they aren't in stock on LCSC, or if you can source them from elsewhere, or if better/cheaper alternatives will work.

FWIW, here are some Farnell links to things you will need. I used AliExpress myself, but these should be equivalent:

- [Output indicator LEDs](https://uk.farnell.com/multicomp-pro/mcl056purgw/led-5mm-ac-hi-red-grn/dp/1581207)
- [RGB LED connectors](https://uk.farnell.com/amphenol-anytek/oq04125100j0g/terminal-block-r-a-header-4way/dp/3810285)
- [On/off switch](https://uk.farnell.com/c-k-components/7101md9abe/switch-pcb-spdt/dp/9574786) - this one is particularly expensive for what it is but alternatives are quite common elsewhere!

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
- grounded unused hex inverter pins.
- reduced brightness of "output on" leds.
- permitted switching between pins 4 and 5 of the input/output connectors to support using non-rgbs cables.
