/*
Copyright 2025 LLVM-MOS Project
Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
information.
*/

/* Provides single directory entry main/resident section */

BYTE ((__DIRECTORYSTART__ + 8) / __BANK0BLOCKSIZE__)
SHORT((__DIRECTORYSTART__ + 8) & (__BANK0BLOCKSIZE__ - 1))
BYTE (0x88)
SHORT(ADDR(.text))
SHORT(ADDR(.bss) - ADDR(.text))
