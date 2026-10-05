#!/usr/bin/env python3
"""Embed a PNG as an LVGL lv_image_dsc_t (LV_COLOR_FORMAT_RAW_ALPHA), decoded
at runtime by LVGL's bundled lodepng (LV_USE_LODEPNG in lv_conf.h).

Usage: png_to_c.py name1=path1.png [name2=path2.png ...] > out.c
Also writes a matching .h with extern declarations next to the first arg's
directory, named after the first identifier's prefix — see call site.
"""
import struct
import sys

HEADER = """#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif
"""


def png_dims(path):
    with open(path, "rb") as f:
        data = f.read()
    # PNG IHDR: 8-byte signature, then 4-byte length + 'IHDR' + width/height (4 bytes each, big-endian)
    w, h = struct.unpack(">II", data[16:24])
    return data, w, h


def emit(name, path):
    data, w, h = png_dims(path)
    out = [f"const LV_ATTRIBUTE_MEM_ALIGN uint8_t {name}_map[] = {{"]
    for i in range(0, len(data), 16):
        out.append("    " + ", ".join(f"0x{b:02x}" for b in data[i:i + 16]) + ",")
    out.append("};\n")
    out.append(f"const lv_image_dsc_t {name} = {{")
    out.append("    .header = {")
    out.append("        .cf = LV_COLOR_FORMAT_RAW_ALPHA,")
    out.append(f"        .w = {w},")
    out.append(f"        .h = {h},")
    out.append("    },")
    out.append(f"    .data_size = sizeof({name}_map),")
    out.append(f"    .data = {name}_map,")
    out.append("};\n")
    return "\n".join(out)


def emit_header(names):
    out = ["#pragma once", "", "#include <lvgl.h>", ""]
    for n in names:
        out.append(f"extern const lv_image_dsc_t {n};")
    return "\n".join(out) + "\n"


if __name__ == "__main__":
    pairs = [arg.split("=", 1) for arg in sys.argv[1:]]
    print(HEADER)
    for name, path in pairs:
        print(emit(name, path))
