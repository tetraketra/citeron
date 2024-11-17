# WARNING:
#  - Expects Python==3.12.5
#  - STD-only, so no `requirements.txt`.

from pathlib import Path

HEADER = """
#ifndef CITERON_H
#define CITERON_H

"""

FOOTER = """

#endif
#endif
"""

HEADERS = [
    "common.h",
    "mem.h",
    "tuple.h",
]

C_FILES = [
    "mem.c",
    "tuple.c",
]

BASE_DIR = Path("./citeron/")

if __name__ == "__main__":
    # write file
    with open("citeron.h", "w") as outfile:
        outfile.write(HEADER)

        # headers
        badline_keywords = ("#ifndef CIT", "#define CIT", "#endif", '#include "')
        for ip in [BASE_DIR.joinpath(header) for header in HEADERS]:
            with open(ip, "r") as infile:
                outfile.write(f"\n/* ====[ FILE: {ip.name} ]==== */\n\n")
                outfile.writelines(
                    filter(
                        lambda l: all(k not in l for k in badline_keywords),  # noqa: E741
                        infile.readlines(),
                    )
                )

        # implementations
        outfile.write("#ifdef CITERON_IMPLEMENTATION\n")

        for ip in [BASE_DIR.joinpath(header) for header in C_FILES]:
            with open(ip, "r") as infile:
                outfile.write(f"\n/* ====[ FILE: {ip.name} ]==== */\n\n")
                outfile.writelines(
                    filter(
                        lambda l: all(k not in l for k in badline_keywords),  # noqa: E741
                        infile.readlines(),
                    )
                )

        # wrapup
        outfile.write(FOOTER)

    # clean file
    lines: str

    with open("citeron.h", "r") as infile:
        lines = infile.readlines()

    with open("citeron.h", "w") as outfile:
        purging_empties = False
        for line in lines:
            if purging_empties and not line.isspace():
                purging_empties = False

            if not purging_empties:
                outfile.write(line)

            if line.isspace():
                purging_empties = True
