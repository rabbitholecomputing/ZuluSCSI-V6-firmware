# ZuluSCSI™ - Copyright (c) 2024 Rabbit Hole Computing™
#
# ZuluSCSI™ file is licensed under the GPL version 3 or any later version. 
#
# https://www.gnu.org/licenses/gpl-3.0.html
# ----
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version. 
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details. 
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# This builds a UF2 file and a DFU file for ZuluSCSI v6 series boards
# - The target's boot loader shall have _bootloader appended to the PIO environment target name
# - uf2_mcu_family shall be defined in the platformio.ini file

Import ("env")

# We delete the bin file here because we want the dfu and uf2 file to always be generated when the project is built
# 'buildprog' doesn't get run if there are no changes to the target. The boot loader is outside of the target so changes to
# it will not be picked up, unless the target's bin file is deleted.
env.AddPreAction("checkprogsize",
    env.VerboseAction("python ./tools/rm-file.py \"${BUILD_DIR}/${PROGNAME}.bin\"", "Deleting: \"${BUILD_DIR}/${PROGNAME}.bin\"")
)

env.AddPostAction("buildprog", [
    env.VerboseAction(" ".join([
        "pio", "run", "-e${PIOENV}_bootloader"
        ]), 
        "Building boot loader: \"${BUILD_DIR}_bootloader/${PROGNAME}.bin\""),
    env.VerboseAction(" ".join([
        "python ./tools/dfu-convert.py", 
            "-b", "0x08000000:\"${BUILD_DIR}_bootloader/${PROGNAME}.bin\"",
            "-b", "0x08008000:\"${BUILD_DIR}/${PROGNAME}.bin\"", 
            "\"$BUILD_DIR/${PROGNAME}.dfu\""
        ]),
        "Creating DFU file: \"$BUILD_DIR/${PROGNAME}.dfu\""),
    env.VerboseAction(" ".join([
        "python ./tools/uf2conv.py", 
            "\"${BUILD_DIR}/${PROGNAME}.bin\"",
            "--base", "0x08008000",
            "--family", env.GetProjectOption("uf2_mcu_family"), 
            "--output", "\"$BUILD_DIR/${PROGNAME}.uf2\""
        ]),
        "Creating UF2 file: \"$BUILD_DIR/${PROGNAME}.uf2\"")
    ])
         