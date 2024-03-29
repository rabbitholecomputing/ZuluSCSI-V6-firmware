# ZuluSCSI V6 Firmware

<img src="https://zuluscsi.com/assets/img/ZuluSCSI_V6.4-Rev2024a.jpg" alt="ZuluSCSI V6.4 PCB" height="300">

This repository contains firmware for the ZuluSCSI™ V6, a SCSI2SD V6 derivative released in April of 2024. The [ZuluSCSI V6.4 firmware](https://github.com/rabbitholecomputing.com/ZuluSCSI-V6-firmware) is derived from the original [SCSI2SD V6](http://www.codesrc.com/gitweb/index.cgi?p=SCSI2SD-V6.git;a=summary) firmware.

Like the original ZuluSCSI firmware, ZuluSCSI V6 firmware is built using PlatformIO, unlike the original SCSI2SD V6 firmware. The original SCSI2SD V6 firmware used the [STM32cube framework](https://docs.platformio.org/en/latest/frameworks/stm32cube.html), as does this firmware.

## ZuluSCSI V6.4 hardware features

* [Open-source firmware](https://github.com/zuluscsi/zuluscsi-firmware), licensed under the GPLv3
* Emulates up to 7 SCSI devices simultaneously, including CD-ROM, Magneto Optical, removable (SyQuest/Jaz-style), and SCSI floppy device types
* Speaks both SCSI-1 and SCSI-2, including 10MB/sec Fast SCSI
* Up to 9.5 megabytes/second read AND write speeds
* SCSI Termination is software-controlled, via ZuluSCSI-V6-util.exe
* Firmware upgrade simplicity; As easy as copying a .uf2 file to the ZuluSCSI via USB.
* External activity LED pin header for attaching remote activity LED
* Designed to be powered via SCSI termination power, when provided by the host
* Identical dimensions and mounting holes as that of SCSI2SD V6, V5.2, ZuluSCSI V1.1, and ZuluSCSI RP2040 Full Size

## License
The ZuluSCSI™ V6 firmware is licensed under the GNU General Public License V3, or any later version.

ZuluSCSI is a registered trademark of Rabbit Hole Computing LLC, and may not be used in commercial context without express written permission.
