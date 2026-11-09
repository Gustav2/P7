
from pathlib import Path
from struct import unpack_from
from intelhex import IntelHex

# ==========================================
# Project configuration
# ==========================================

project = Path(
    r"C:\Users\bjark\OneDrive\Desktop\AAU\P7\SecurityMonitor"
)

monitor_file = project / "Monitor/monitor-CW308_SAM4S.hex"
customer_file = project / "Customer_Application/customer-CW308_SAM4S.hex"

output = project / "combined.bin"

# SAM4S flash allocation
FLASH_START = 0x00400000
CUSTOMER_START = 0x00410000
FLASH_END = 0x00420000

FLASH_SIZE = FLASH_END - FLASH_START
CUSTOMER_OFFSET = CUSTOMER_START - FLASH_START

# SAM4S page configuration
PAGE_SIZE = 512
PAGE_COUNT = FLASH_SIZE // PAGE_SIZE

# Workaround for the installed ChipWhisperer page-count bug
MAX_PROGRAM_SIZE = PAGE_COUNT * PAGE_SIZE - (PAGE_SIZE - 1)

# ==========================================
# 1. Load firmware images
# ==========================================

for path in (monitor_file, customer_file):
    if not path.is_file():
        raise FileNotFoundError(f"Firmware not found: {path}")

monitor = IntelHex(str(monitor_file))
customer = IntelHex(str(customer_file))

# ==========================================
# 2. Validate flash addresses
# ==========================================

assert monitor.minaddr() == FLASH_START
assert monitor.maxaddr() < CUSTOMER_START

assert customer.minaddr() == CUSTOMER_START
assert customer.maxaddr() < FLASH_END

print("Flash address validation successful")

# ==========================================
# 3. Validate vector tables
# ==========================================

for name, image, address in [
    ("Monitor", monitor, FLASH_START),
    ("Customer", customer, CUSTOMER_START),
]:

    vectors = bytes(
        image.tobinarray(start=address, end=address + 7)
    )

    stack, reset = unpack_from("<II", vectors)

    assert 0x20000000 <= stack <= 0x20010000
    assert (stack & 7) == 0
    assert (reset & 1) == 1
    assert address <= (reset & ~1) < address + 0x10000

    print(f"{name}:")
    print(f"  Stack pointer: 0x{stack:08X}")
    print(f"  Reset handler: 0x{reset:08X}")

# ==========================================
# Redirect Customer SysTick to Monitor
# ==========================================

# Cortex-M SysTick vector is entry 15
SYSTICK_VECTOR_OFFSET = 15 * 4  # 0x3C

# Read the Monitor's SysTick handler address
monitor_vector_address = (
    FLASH_START + SYSTICK_VECTOR_OFFSET
)

monitor_handler_bytes = bytes(
    monitor.tobinarray(
        start=monitor_vector_address,
        end=monitor_vector_address + 3
    )
)

monitor_handler = int.from_bytes(
    monitor_handler_bytes,
    byteorder="little"
)

# Validate handler address
assert monitor_handler & 1, "Invalid Thumb handler address"
assert (
    FLASH_START <= (monitor_handler & ~1) < CUSTOMER_START
), "SysTick handler is not in Monitor flash"

# Replace Customer's SysTick vector entry
customer_vector_address = (
    CUSTOMER_START + SYSTICK_VECTOR_OFFSET
)

for i, byte in enumerate(monitor_handler_bytes):
    customer[customer_vector_address + i] = byte

print(
    f"Customer SysTick redirected to "
    f"Monitor handler: 0x{monitor_handler:08X}"
)

# ==========================================
# 4. Merge firmware images
# ==========================================

# Keep Monitor as primary entry image
customer.start_addr = None

# Reject overlapping addresses
monitor.merge(customer, overlap="error")

# Fill unused flash addresses with 0xFF
monitor.padding = 0xFF

combined = bytes(
    monitor.tobinarray(
        start=FLASH_START,
        end=FLASH_END - 1
    )
)

assert len(combined) == FLASH_SIZE

print("\nFirmware images merged successfully")

# ==========================================
# 5. Trim unused trailing flash padding
# ==========================================

# Remove trailing 0xFF bytes only.
# Padding BETWEEN the two images is preserved.
trimmed = combined.rstrip(b"\xFF")

# Ensure the Customer vector table is still present
assert len(trimmed) >= CUSTOMER_OFFSET + 8

# Verify compatibility with installed programmer
if len(trimmed) > MAX_PROGRAM_SIZE:
    raise RuntimeError(
        "Firmware cannot be safely trimmed enough for "
        "the installed ChipWhisperer programmer. "
        "Fix the programmer's page-count calculation."
    )

# ==========================================
# 6. Save combined binary
# ==========================================

output.write_bytes(trimmed)

print(f"\nCreated: {output}")
print(f"Original size: {len(combined)} bytes")
print(f"Trimmed size:  {len(trimmed)} bytes")
print(f"Removed:       {len(combined) - len(trimmed)} bytes")

# ==========================================
# 7. Verify saved binary
# ==========================================

firmware = output.read_bytes()

assert firmware == trimmed

for name, offset in [
    ("Monitor", 0x00000),
    ("Customer", CUSTOMER_OFFSET),
]:
    stack, reset = unpack_from("<II", firmware, offset)

    print(f"\n{name}:")
    print(f"  Stack pointer: 0x{stack:08X}")
    print(f"  Reset handler: 0x{reset:08X}")

print("\nCombined firmware verified successfully!")
