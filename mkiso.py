#!/usr/bin/env python3
import sys, struct

def le16(v): return struct.pack("<H", v & 0xFFFF)
def le32(v): return struct.pack("<I", v & 0xFFFFFFFF)
def be16(v): return struct.pack(">H", v & 0xFFFF)
def be32(v): return struct.pack(">I", v & 0xFFFFFFFF)
def both16(v): return le16(v) + be16(v)
def both32(v): return le32(v) + be32(v)

def dir_rec(lba, size, name, is_dir):
    rec_len = 33 + len(name)
    if rec_len % 2:
        rec_len += 1
    r = bytearray()
    r.append(rec_len)
    r.append(0)
    r += both32(lba)
    r += both32(size)
    r += b"\x00" * 7
    r.append(2 if is_dir else 0)
    r += b"\x00\x00"
    r += both16(1)
    r.append(1)
    r += name
    if len(name) % 2 == 0:
        r.append(0)
    return bytes(r)

def root_rec(lba, size):
    r = bytearray(34)
    r[0] = 34
    r[2:10] = both32(lba)
    r[10:18] = both32(size)
    r[25] = 2
    r[28:30] = le16(1)
    r[30:32] = be16(1)
    return bytes(r)

def path_rec(lba, little):
    r = bytearray()
    r.append(1)
    r.append(0)
    r += (le32 if little else be32)(lba)
    r += (le16 if little else be16)(1)
    r += b"\x00\x00"
    if len(r) % 2:
        r += b"\x00"
    return bytes(r)

def build(kernel_path, boot_path, out_path):
    with open(kernel_path, "rb") as f:
        kernel = f.read()
    with open(boot_path, "rb") as f:
        boot = f.read()

    marker = b"BOJANBOOT"
    midx = boot.find(marker)
    if midx < 0:
        raise SystemExit("boot marker not found")

    boot_vsec = max((len(boot) + 511) // 512, 4)
    bootimg = bytearray(boot[:boot_vsec * 512])
    if len(bootimg) < boot_vsec * 512:
        bootimg += b"\x00" * (boot_vsec * 512 - len(bootimg))

    kernel_vsec = (len(kernel) + 2047) // 2048

    LBA_DESCS = 16
    LBA_BR = 16
    LBA_PVD = 17
    LBA_VDT = 18
    LBA_BOOT = 19
    LBA_KERNEL = LBA_BOOT + boot_vsec // 4
    LBA_ROOT = LBA_KERNEL + kernel_vsec
    LBA_PTLE = LBA_ROOT + 1
    LBA_PTBE = LBA_PTLE + 1
    LBA_BOOTCAT = LBA_PTBE + 1
    TOTAL_SECTORS = LBA_BOOTCAT + 1

    patch = le32(LBA_KERNEL) + le32(kernel_vsec)
    bootimg[midx + len(marker):midx + len(marker) + 8] = patch
    assert bootimg[midx + len(marker):midx + len(marker) + 8] == patch

    root = bytearray()
    root += root_rec(LBA_ROOT, 2048)
    root += dir_rec(LBA_ROOT, 2048, b"\x01", True)
    root += dir_rec(LBA_KERNEL, len(kernel), b"KERNEL.BIN;1", False)
    root += b"\x00" * (2048 - len(root))

    pt_le = path_rec(LBA_ROOT, True)
    pt_be = path_rec(LBA_ROOT, False)

    bootcat = bytearray(2048)
    v = bytearray(32)
    v[0] = 0x01
    v[1] = 0x00
    v[2:7] = b"CD001"
    v[7] = 0x01
    v[8:32] = b"BojanOS".ljust(24, b" ")
    v[28:30] = le16(0xAA55)
    v[26:28] = b"\x00\x00"
    s = 0
    for i in range(0, 32, 2):
        s += struct.unpack("<H", v[i:i+2])[0]
    chk = (-s) & 0xFFFF
    v[26:28] = le16(chk)
    bootcat[0:32] = v

    e = bytearray(32)
    e[0] = 0x88
    e[1] = 0x00
    e[2] = 0x00
    e[3] = 0x00
    e[4] = boot_vsec & 0xFF
    e[5:9] = le32(LBA_BOOT)
    bootcat[32:64] = e

    pvd = bytearray(2048)
    pvd[0] = 1
    pvd[1:6] = b"CD001"
    pvd[6] = 1
    pvd[8:40] = b"BojanOS".ljust(32, b" ")
    pvd[40:72] = b"".ljust(32, b" ")
    pvd[80:88] = both32(TOTAL_SECTORS)
    pvd[120:124] = both16(2048)
    pvd[132:140] = both32(LBA_PTLE)
    pvd[140:144] = le32(0)
    pvd[144:152] = both32(LBA_PTBE)
    pvd[152:156] = be32(0)
    rr = root_rec(LBA_ROOT, 2048)
    pvd[156:156 + len(rr)] = rr
    pvd[190:318] = b"".ljust(128, b" ")
    pvd[318:446] = b"".ljust(128, b" ")
    pvd[446:574] = b"".ljust(128, b" ")
    pvd[648:650] = le16(1)
    pvd[650:652] = be16(1)

    vdt = bytearray(2048)
    vdt[0] = 255
    vdt[1:6] = b"CD001"
    vdt[6] = 1

    br = bytearray(2048)
    br[0] = 0
    br[1:6] = b"CD001"
    br[6] = 1
    br[7:39] = b"EL TORITO SPECIFICATION".ljust(32, b" ")
    br[71:75] = le32(LBA_BOOTCAT)

    for nm, arr in (('pvd',pvd),('vdt',vdt),('br',br),('bootcat',bootcat),('root',root)):
        if len(arr) > 2048:
            print('TOOLONG', nm, len(arr))
    iso = bytearray()
    iso += b"\x00" * (16 * 2048)
    iso += br
    iso += pvd
    iso += vdt
    iso += bytes(bootimg)
    iso += kernel + b"\x00" * (kernel_vsec * 2048 - len(kernel))
    iso += bytes(root)
    iso += bytes(pt_le) + b"\x00" * (2048 - len(pt_le))
    iso += bytes(pt_be) + b"\x00" * (2048 - len(pt_be))
    iso += bytes(bootcat)

    expected = TOTAL_SECTORS * 2048
    if len(iso) != expected:
        raise SystemExit("layout mismatch: %d != %d" % (len(iso), expected))

    with open(out_path, "wb") as f:
        f.write(iso)
    print("wrote %s: %d sectors (%d bytes), kernel at LBA %d (%d sectors)"
          % (out_path, TOTAL_SECTORS, len(iso), LBA_KERNEL, kernel_vsec))

if __name__ == "__main__":
    build(sys.argv[1], sys.argv[2], sys.argv[3])
