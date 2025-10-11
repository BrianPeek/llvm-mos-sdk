; Copyright 2025 LLVM-MOS Project
; Licensed under the Apache License, Version 2.0 with LLVM Exceptions.
; See https://github.com/llvm-mos/llvm-mos-sdk/blob/main/LICENSE for license
; information.

; Originally from cc65. Modified from original version.

.include "lynx.inc"
.include "imag.inc"
;
; Helper stuff for the cartridge file functions. This version can deal
; with 1024 bytes/block carts that are using CART0 as a read strobe.
; Also the default crt0.s supports this most common Lynx cart format.

.text

;**********************************
; Skip bytes on bank 0
; X:Y count (EOR $FFFF)
;**********************************
seclynxskip0:
        inx
        bne 1f
        iny
        beq exit
1:      jsr secreadbyte0
        bra seclynxskip0

;**********************************
; Read bytes from bank 0
; X:Y count (EOR $ffff)
;**********************************
seclynxread0:
        inx
        bne 1f
        iny
        beq exit
1:      jsr secreadbyte0
        sta (_FileDestPtr)
        inc _FileDestPtr
        bne seclynxread0
        inc _FileDestPtr+1
        bra seclynxread0

;**********************************
; Read one byte from cartridge
;**********************************
secreadbyte0:
        lda RCART0
        inc _FileBlockByte
        bne exit
        inc _FileBlockByte+1
        bne exit

;**********************************
; Select a block
;**********************************
seclynxblock:
        pha
        phx
        phy
        lda __iodat
        and #$fc
        tay
        ora #2
        tax
        lda _FileCurrBlock
        inc _FileCurrBlock
        sec
        bra 2f
0:      bcc 1f
        stx IODAT
        clc
1:      inx
        stx SYSCTL1
        dex
2:      stx SYSCTL1
        rol
        sty IODAT
        bne 0b
        lda __iodat
        sta IODAT
        stz _FileBlockByte
        lda #<__BOOTLDRBLOCK__
        sta _FileBlockByte+1
        ply
        plx
        pla

exit:   rts
