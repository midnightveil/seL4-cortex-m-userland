#!/usr/bin/env python3

import ctypes
import io
import json
import subprocess

# matches the seL4_UserspaceSetupData_t struct
class UserlandSetupData32(ctypes.LittleEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("magic", ctypes.c_char * 4),
        ("entrypoint", ctypes.c_uint32),
    ]

    def __str__(self):
        out = "struct " + self.__class__.__name__ + " {\n"
        for field_name, field_type in self._fields_:
            out += "    ." + field_name + " = " + str(getattr(self, field_name)) + ";\n"
        out += ("}")

        return out


def readelf(filename: str, *kinds: list[str]):
    readelf_proc = subprocess.run(
        ["llvm-readelf", "--elf-output-style", "JSON", *kinds, filename],
        capture_output=True, check=True, encoding="utf-8"
    )

    return json.loads(readelf_proc.stdout)




def main():
    import argparse

    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", "-O", required=True)
    parser.add_argument("--kernel", required=True)
    parser.add_argument("--roottask", required=True)
    parser.add_argument("--roottask_bin", required=True)
    parser.add_argument("--gen_config", required=True)

    args = parser.parse_args()

    with open(args.gen_config, "r") as gen_config_f:
        gen_config = json.load(gen_config_f)

    match gen_config["WORD_SIZE"]:
        case "32":
            UserlandSetupData = UserlandSetupData32
        case _:
            raise NotImplementedError("only 32 bit word size supported")


    entrypoint = readelf(args.roottask, "--headers")[0]["ElfHeader"]["Entry"]

    symbols = [sym for sym in readelf(args.kernel, "--symbols")[0]["Symbols"] if sym["Symbol"]["Name"]["Name"] == "ki_userspace_start"]
    assert(len(symbols) == 1)
    ki_userspace_start = symbols[0]["Symbol"]["Value"]

    setup_data = UserlandSetupData(
        magic=b"meL4",
        entrypoint=entrypoint,
    )

    # NOTE: This code relies upon the assumptions about the roottask linker.ld,
    #       specifically, that the entrypoint is the *first loadable address*.
    # Mask off bit 0 / thumb bit
    load_start = entrypoint & ~0x1

    load_start_offset = load_start - ki_userspace_start
    assert load_start_offset >= len(bytes(setup_data))

    # print("load_start: {:#x}".format(load_start))
    # print("ki_userspace_start: {:#x}".format(ki_userspace_start))
    # print("ki_userspace_start + header len: {:#x}".format(ki_userspace_start + len(bytes(setup_data))))
    # print("load_start_offset: {:#x}".format(load_start_offset))
    # print("ki_userspace_start + load_start_offset: {:#x}".format(ki_userspace_start + load_start_offset))

    new_userland = io.BytesIO()
    new_userland.write(setup_data)
    new_userland.seek(load_start_offset, 0)
    with open(args.roottask_bin, "rb") as roottask_f:
        new_userland.write(roottask_f.read())

    old_userland: bytes | None = None

    try:
        with open(args.output, "rb") as old_userland_file:
            old_userland = old_userland_file.read()
    except FileNotFoundError:
        pass

    if old_userland is None or old_userland != new_userland.getvalue():
        print("userland changed, updating...")
        with open(args.output, "wb") as output_file:
            output_file.write(new_userland.getbuffer())
    else:
        print("userland unchanged")


if __name__ == "__main__":
    main()
