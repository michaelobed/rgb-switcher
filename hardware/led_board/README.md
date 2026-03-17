# rgb switcher led board

Here's the KiCad design file for the LED board. As before, third-party libraries are included where necessary, and there's an included BOM in the [KiCAD JLCPCB tools](https://github.com/bouni/kicad-jlcpcb-tools) format.

Bear in mind that you don't absolutely have to use the parts referenced in there if they aren't in stock on LCSC, or if you can source them from elsewhere, or if better/cheaper alternatives will work.

FWIW, here are some alternative links to things you will need. I used AliExpress myself, but these should be equivalent:

- [RGB LED connectors](https://uk.farnell.com/amphenol-anytek/oq04125100j0g/terminal-block-r-a-header-4way/dp/3810285) (Farnell)
- [+5v in/out connector](https://uk.farnell.com/camdenboss/ctb932hd-2/terminal-block-header-2pos-th/dp/2493677) (Farnell)
- [On/off switch](https://www.digikey.co.uk/en/products/detail/e-switch/100SP3T2B4M6RE/378850) (DigiKey)

## changelog

### v1.0

- initial version, based on v0.2 main board design.
- eliminated line decoder and NOT gate so power mosfets are driven directly by mcu gpio.
