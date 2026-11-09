# Wire jumpers

The board is single-sided (F.Cu only, no vias), so 8 connections are made with wire jumpers.
It is a hat: the parts sit on the copper side, so the wires run on the top as well. Use
insulated wire (e.g. 30 AWG wire-wrap).

Each wire end is a dedicated through-hole pad (2.0 mm, 0.8 mm drill, labelled `W1A` …),
except where there was no room for a hole. Those ends (in **bold**) are soldered to an
existing pin or component lead instead. No wire ever lands on the MSP.

## Table

Positions are in mm from the **top-left corner of the board** (x to the right, y down),
seen from the top.

| Wire | From | To | Length | Signal |
|---|---|---|---|---|
| W1 | **pin P4-7** (22.3, 67.5) | hole W1B (18.8, 70.4) | 4.6 mm | VREF / 3.3 V link (J4-7) |
| W2 | hole W2A (31.0, 58.8) | hole W2B (40.8, 41.6) | 19.8 mm | RST |
| W3 | **pin P1-11** (4.6, 37.0) | hole W3B (25.0, 62.0) | 32.3 mm | RST |
| W4 | **lead C5 pin 1** (20.0, 25.6) | hole W4B (25.6, 23.0) | 6.2 mm | 3V3 to decap C5 |
| W5 | hole W5A (36.6, 22.4) | hole W5B (25.6, 20.4) | 11.2 mm | GND (C2 / C6) |
| W6 | hole W6A (36.2, 29.8) | hole W6B (23.0, 23.0) | 14.8 mm | GND (C1, VCORE decap) |
| W7 | hole W7A (11.6, 31.4) | hole W7B (12.8, 26.6) | 5.0 mm | GND (J5 pin 2) |
| W8 | **pin J5-1** (15.3, 34.5) | hole W8B (21.0, 45.8) | 12.6 mm | BSL |

## Other notes

- **J5** is now a 2-pin header: pin 1 = BSL, pin 2 = GND. The debugger connects to the
  breakout pins: SWDIO P4-5, SWCLK P4-6, TX P1-7, RX P1-8, RST P1-11, 3V3 P3-2.
- **Firmware: the trigger moved to MSP pin 4 = PB16** (it was PA0). The UART stays on
  PA10/PA11 (BSL UART).
- **Ground through the adapter:** R4, SW1 and C4 reach ground through the nearest CW308 GND
  pin, i.e. via the adapter's ground. The MSP and all decaps share one GND copper piece on
  the hat (plus wires W5–W6).
- **Checking:** KiCad's DRC lists 22 unconnected items. They are these 8 wires, the 3 grounds
  via the adapter, and 11 unused CW308 GND pins. After soldering, check each wire for
  continuity.
