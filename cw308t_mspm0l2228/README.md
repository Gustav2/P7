# CW308T-MSPM0L2228 — SCA target hat (2-layer)

A ChipWhisperer target board for the TI **MSPM0L2228** (LQFP-80, TI PN0080A: 12×12 mm body, 0.5 mm pitch), built to drop onto the
**CW308T→CW312 adapter** and attack the AES-ADV engine with a clean measurement front-end.

## Geometry — matches the CW308T→CW312 adapter
Connector positions, mounting holes and outline are taken from NewAE's **CW308T_toCW312T**
gerbers (the "2-layer solid board" adapter): board **2.32″ × 2.8″ (58.9 × 71.1 mm)** matching the adapter's main body, the three
20-pin connectors on West/East/South edges (West↔East centres 1.9″, South row 0.2″ below the
side columns), 3× M3 mounting holes at the adapter positions.

## Connectors (per your spec)
- **Female 1×20 sockets on the BOTTOM** (J1 West, J3 East, J4 South) — mate **down** onto the
  adapter at the exact CW308 positions.
- **Male 1×20 breakout headers on the TOP** (P1/P3/P4), **outboard of the sockets (toward the edges)**, **all 60 pins**
  duplicated 1:1 with the sockets — plug in cables/probes from above. Sockets and the 3 M3 holes are placed at the adapter's exact absolute positions, so it drops straight on.
- Debug: **J_SWD** (Cortex 2×5 SWD), **SW1** reset, **TP_BSL** (PA18).

## Electrical
- **Debug header J_DBG** (1x8, 2.54 mm, same pins as the connectors), left->right: 3V3, GND, SWDIO, SWCLK, TXD, RXD, RST, BSL. Reset button SW1. Copper label "WolfSniff - Group 741" engraved in the top copper.
- **2-layer, plated vias**, GND **pour on both layers** (10 vias). Traces: power/analog 0.4 mm (necked to 0.3 mm at the chip), logic signals 0.3 mm (the 0.5 mm-pitch QFP pads are 0.25 mm wide). No traces or vias under the chip body. Vias are **1.0 mm drilled, pin-through** (1.4 mm copper, plated both sides) so a pin/wire can be pushed through and soldered on both layers. Pour-to-feature isolation 0.8 mm (CNC clears wide channels around every trace/pad).
- **Decoupling caps hard against the chip pins**: C1 470nF at VCORE(80), C2/C5 at VDD/VBAT(8/9/7),
  C3 at VDD2(49), C4 4.7µF bulk (supply side). R1 (0Ω shunt) / R2 (nRST pull-up) are DNP options.
- Pin map: TRIG=PA0, UART=PA10/PA11, ext-clock=PA6/HFCLK_IN, SWDIO/SWCLK=PA19/PA20, VCORE=pin80.

## Files
- `cw308t_mspm0l2228.kicad_pcb` — routed + poured PCB (KiCad 9). **Authoritative.**
- `cnc/` + `cw308t_mspm0l2228_cnc.zip` — your CAM set:
  `TopLayer.gbr`, `BottomLayer.gbr`, `BoardOutline.gbr`, `DrillPlated.drl` (147 holes: all 120
  connector pins + 8-pin debug header + 10 vias, all plated 1.0 mm), `DrillUnplated.drl` (3× M3).
- `gerbers/` + `cw308t_mspm0l2228_gerbers.zip` — full RS-274X set + drill map (for a fab house).
- `render_top.png`, `render_bottom.png` — 3D previews.
- `BOM.csv`.

## Verify before cutting / ordering
1. DRC is clean (0 unconnected, 0 clearance/hole). The only DRC notes are **courtyard keep-out
   overlaps** where the socket+male pairs sit 0.1″ apart and at the connector corners — expected
   on a board this dense; the actual copper/holes all clear.
2. **Connector pin-1 orientation**: the sockets are bottom-mounted, so confirm pin 1 of each
   socket lands on the adapter's pin 1 (check against the adapter silk). If a connector is
   reversed it's a one-line fix in the generator.
3. The GND pour is filled in the gerbers. If you edit in KiCad, press `B` to re-fill before
   re-exporting.

## Note
The `.kicad_sch` is the earlier logical schematic; the **PCB is authoritative** for this build
(the sockets/males/pass-throughs are a physical breakout, not a change to the logical nets).
