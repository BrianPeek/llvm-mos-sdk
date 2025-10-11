/* Atari Lynx .LYX linker script. */

/* Provide imaginary (zero page) registers. */
__rc0 = 0x00;
INCLUDE imag-regs.ld
ASSERT(__rc31 == 0x001f, "Inconsistent zero page map.")

__stack = 0xc038; /* Set initial soft stack address to just above last memory address. (It grows down.) */

__BOOTLDRBLOCK__   = 0x100 - (__BANK0BLOCKSIZE__ >> 8);
__DIRECTORYSTART__ = 0xCB;

PROVIDE(__BANK0BLOCKSIZE__ = 0x0400);
PROVIDE(__BANK1BLOCKSIZE__ = 0x0000);
PROVIDE(__VERSION__        = 1);
PROVIDE(__ROTATION__       = 0);

/* if no .cartnamemfr section provided, force values to pass ASSERT tests */
PROVIDE(__cartend  = 48);
PROVIDE(__cartname =  0);
PROVIDE(__cartmfr  = 32);

/* ensure name and manufacturer are the correct lengths */
ASSERT(__cartmfr - __cartname == 32, "Cart name must be 31 characters")
ASSERT(__cartend - __cartmfr  == 16, "Cart manufacturer must be 15 characters")
ASSERT(__ROTATION__ >=0 && __ROTATION__ <= 2, "Invalid rotation value")

MEMORY {
    lnxheader (r)  : ORIGIN = 0x10000,    LENGTH = 64
    secbootldr (r) : ORIGIN = 0xfb68,     LENGTH = 151
    directory (r)  : ORIGIN = 0x00cb,     LENGTH = 8
    zp (rw)        : ORIGIN = __rc31 + 1, LENGTH = 0x100 - (__rc31 + 1)
    ram (rw)       : ORIGIN = 0x0200,     LENGTH = 0xc038 - 0x200
}

REGION_ALIAS("c_readonly", ram)
REGION_ALIAS("c_writeable", ram)

SECTIONS {
    .lnxheader : {
        BYTE(0x4C) BYTE(0x59) BYTE(0x4E) BYTE(0x58) /* "LYNX" */
        SHORT(__BANK0BLOCKSIZE__)
        SHORT(__BANK1BLOCKSIZE__)
        SHORT(__VERSION__)
        KEEP(*(.cartnamemfr)) /* Cart name (32), manufacturer name (16) */
        BYTE(__ROTATION__) /* Rotation - 0 = None, 1 = Left, 2 = Right */
        BYTE(0x00) BYTE(0x00) BYTE(0x00) BYTE(0x00) BYTE(0x00) /* Padding */
    } >lnxheader

    .secbootldr : {
        KEEP(*(.secbootldr))
    } >secbootldr

    .directory : {
        INCLUDE directory.ld
    } >directory

    INCLUDE text.ld
    INCLUDE rodata.ld
    INCLUDE data.ld
    INCLUDE zp.ld
    INCLUDE bss.ld
    INCLUDE noinit.ld
}

OUTPUT_FORMAT {
    FULL(lnxheader)
    INCLUDE bootldr.ld
    FULL(secbootldr)
    FULL(directory)
    TRIM(ram)
}
