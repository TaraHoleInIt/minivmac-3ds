Mini vMac: mydriver/README
Paul C. Pratt
www.gryphel.com
February 23, 2002

The mydriver folder contains source code
for a replacement disk driver that is patched
into the emulated rom.

The compiled driver is already in ROMEMDEV.c
(the initialization of sony_driver variable),
so this source code is not needed for building
Mini vMac. It is only needed if you want
to change this driver.

To compile the driver, use MPW commands
such as:

set srcdir hd4:Topaz:MinivMac:mydriver:
asm -case on {srcdir}mydriver.a -o {srcdir}mydriver.a.o
c {srcdir}mydriver.c -r -b -mbg off -opt full -o {srcdir}mydriver.c.o
link {srcdir}mydriver.a.o {srcdir}mydriver.c.o -rt DRVR=128 -o {srcdir}mydriver

Then you can use ResEdit to copy the hex data out
of the DRVR 128 resource, format it a bit, and
replace the initialization data of the variable
sony_driver in the file ROMEMDEV.c

(note: this is using the old c compiler, to use
the current c compiler, use:
SC {srcdir}mydriver.c -w 17 -proto strict -b -mbg off -opt all -o {srcdir}mydriver.c.o
)
