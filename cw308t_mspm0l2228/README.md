# CW308T-MSPM0L2228 — SCA target board

A minimal ChipWhisperer **CW308 UFO "victim" daughterboard** for the TI
**MSPM0L2228** (LQFP-80, PN package), built to attack the problem we diagnosed on
the eCTF board: the AES-ADV key-dependent power leakage sits at the noise floor
because of (1) long shunt air-wires, (2) an asynchronous, under-sampled clock, and
(3) measuring the internal-LDO-filtered VCORE node.

This board fixes all three at the hardware level:

| Problem on the eCTF setup | Fix on this board |
|---|---|
| Long air-wires from chip to SHUNTL (inductance + EMI, bandwidth squashed <5 MHz) | Chip + shunt path on one PCB, millimetres long, straight into the CW308/CW313 shunt |
| ~29 MS/s, asynchronous to the core clock (jitter, aliasing) | **External clock from the Husky** into `HFCLK_IN` (PA6) → synchronous, sample-locked capture |
| Shunt on VCORE = internal-LDO output = pre-filtered | Measure **VDD** (full die current), the node where the switching actually flows |
| Whole-board noise (LEDs, debugger, peripherals) | Nothing on the board but the MCU + decoupling |

Plugs into your **CW308 → CW312 adapter → CW313** chain.

## Files
- `cw308t_mspm0l2228.kicad_pro` — KiCad 9 project
- `cw308t_mspm0l2228.kicad_sch` — schematic (ERC-clean except the intentional PA5 spare net)
- `cw308t_mspm0l2228.kicad_pcb` — PCB: **fully routed** 2-layer board on the exact CW308T
  outline (from NewAE's official CW308T gerbers), with GND pour on both layers
- `cw308t_mspm0l2228.dsn` / `.ses` — Specctra export + Freerouting session (routing provenance)
- `sca.kicad_sym` — the MSPM0L2228 + helper symbols
- `sym-lib-table`, `fp-lib-table` — library tables
- `BOM.csv` — bill of materials (what to populate / leave DNP)
- `gerbers/` + `cw308t_mspm0l2228_gerbers.zip` — **single-sided Gerbers + Excellon drill** (top
  copper + outline + drill) for CNC isolation milling
- `JUMPERS.txt` — the 10 jumper links (ref, net, length)
- `render_top.png`, `sch.pdf`, `layout.pdf` — preview renders

## Pin mapping (PN / 80-pin, from the datasheet Fig 6-1)
| Signal | MCU pin | Net | CW308 connector |
|---|---|---|---|
| Trigger | PA0 (1) | TRIG | J7.10 (GPIO4) |
| UART TX (→host) | PA10 (28) | UART_TX | J7.7 (GPIO1 / tio1=serial_rx) |
| UART RX (←host) | PA11 (29) | UART_RX | J7.8 (GPIO2 / tio2=serial_tx) |
| **Ext clock in** | **PA6 (14)** | CLKIN | **J7.3 (CLK-IN)** |
| SWDIO | PA19 (56) | SWDIO | J9.5 (JTAG_TMS) |
| SWCLK | PA20 (57) | SWCLK | J9.6 (JTAG_TCK) |
| nRST | NRST (6) | nRST | J7.11 / J9.8 |
| VDD / VDD2 / VBAT | 8 / 49 / 7 | VDD_CHIP | J11.2 (SHUNTL) |
| VSS / VSS2 | 9 / 48 | GND | GND pins |
| VCORE | 80 | VCORE | 470nF to GND only (LDO out — do NOT drive) |

`PA5 (13)` is broken out as a spare (alt HFXIN pin) — the single ERC warning.

## Power & measurement topology
- Supply (3V3) enters on **FILT_HP/FILTIN**, through the **CW308/CW313 shunt**
  (`SHUNTH`→`SHUNTL`), into **VDD_CHIP**. The shunt therefore carries the full chip
  current. Bulk cap (C4) is on the **supply** side; only small decoupling on the chip side.
- For an even shorter path you can populate **R1** as a LOCAL shunt right at the pins and
  sense across it (via J_MEAS / CW313 MEASURE) — but then don't also use the CW308 shunt.
- `VREF` is tied to the 3V3 supply so the Husky knows the I/O level.

## Debug interface (as on the MSP-LITO board)
- **J_SWD** — ARM **Cortex 2x5 1.27 mm SWD** header: 1=VTREF, 2=SWDIO (PA19), 4=SWCLK (PA20),
  10=nRESET, odd pins GND. Flash/debug directly with an XDS110 / J-Link, exactly like the LITO's
  debugger connector (you can also still flash via the CW308 J9 JTAG pins).
- **SW1** — nRST push-button (the LITO's NRST button).
- **TP_BSL** — PA18 BSL-invoke test point (bootloader entry).

## Routing (single-sided, CNC-millable)
**One copper layer (top), no vias, no plated through-holes** — built for CNC isolation milling.
Autorouted on the top layer (Freerouting) with GND as a top pour; the unavoidable crossings are
done with **10 jumper links** (see `JUMPERS.txt`). Verified in KiCad 9: **0 unrouted, 0 shorts,
0 clearance/hole violations.** Drilled holes (sockets, SWD, mounting) are non-plated — you drill
them on the CNC and solder the part to the top copper.

### Jumpers (JP1–JP10)
Each JP is two 0.6 mm pads bridged by a surface link (no hole). Solder a **0603/1206 0-Ω** for
short ones, a **short wire** for the 3 longer ones. Net-ties model them so KiCad sees continuity.
7 are ≤3.5 mm (0-Ω); JP4/JP9/JP8 are longer (wire). Full list in `JUMPERS.txt`.

## Finishing the board (in KiCad 9)
1. Open the project. If symbols/footprints show as missing, confirm `sym-lib-table`
   (local `sca`) loaded and that KiCad's standard footprint libs are installed.
2. **Fill zones** (`Edit > Fill All Zones`, or press `B`) so the GND planes pour.
3. Run DRC (already clean apart from the noted connector-corner courtyard + silk cosmetics);
   tidy silk reference text if you want it pretty.
4. **Before ordering, do one mechanical cross-check**: the outline, mounting holes and the
   three 20-pin connectors were taken from NewAE's CW308T gerbers, but verify connector
   **pin-1 orientation** against your physical CW308/adapter silk (if a connector is
   mirrored, the fix is one rotate/renumber in the footprint).

## Firmware / SysConfig changes to use the external clock
In your SysConfig for the attack firmware:
- Enable **HFCLK** sourced from the **external digital clock** (`USEEXTHFCLK = 1`),
  input on **PA6 / HFCLK_IN** (4–32 MHz, square wave).
- Source **MCLK from HFCLK** (not SYSOSC). Run the core at a modest rate (e.g. 8–16 MHz)
  so the Husky can oversample it heavily.
- Keep UART0 on PA10/PA11 and the trigger on PA0 (unchanged — already hardware-proven).

In the notebook, drive the target from the Husky clock and sample synchronously
(`scope.clock` sourced from `clkgen`, `hs2='clkgen'`, ADC a multiple of the DUT clock).

## Fabrication (CNC)
**Single-sided, isolation-milled.** Use `gerbers/cw308t_mspm0l2228-F_Cu.gbr` for the isolation
toolpath, `-Edge_Cuts.gbr` for the outline cut, and `cw308t_mspm0l2228.drl` for drilling the
socket / SWD / mounting holes (all non-plated). Silk/mask/paste gerbers are included but optional
for a milled board. Track 0.25 mm, clearance 0.2 mm, zone clearance 0.254 mm — check your bit can
hold ~0.2 mm isolation (a 0.1 mm / 10° V-bit is typical). The GND pour is on the top layer only.

> Note: the 10 jumper footprints show as "not in library" in KiCad (they're generated inline).
> That's cosmetic and does not affect the Gerbers or the board.

## Honest expectation
This attacks the **real** limit (measurement SNR), not the ones we already ruled out
(memory, trace count, analysis). Synchronous capture + a VDD shunt + a millimetre-scale
path should raise the ~0.005 key-dependent correlation meaningfully. It does **not**
remove the on-die ciphertext-readout confound, so it improves the odds rather than
guaranteeing full recovery — but it is the right next hardware step short of an EM probe.
