
# Pins

R/W#: The read/write control pin. For I2C, it's connected to Vss.

D/C#: The data/command control pin. For I2C, it controls the SA0 bit.

# Interface



The R/W# bit indicates if the device is in read mode (1) or write mode (0).

The communication follows the following steps...
- The MCU sends a start condition
- The MCU sends the SSD1315 slave address 
- The MCU sends a control byte. The SSD1315 sends an ACK.
- The MCU sends a data byte. The SSD1315 sends an ACK.
- The MCU sends an end condiution

The slave address is formatted as: `0 1 1 1 1 0 SA0 R/W#`
- SA0 bit: Let's us choose the slave address for the SSD1315. Set by the D/C# pin.
- R/W# bit: Read mode (1) or write mode (0).

The control byte is formatted as: `C0 D/C# 0 0 0 0 0 0`
- C0 bit: Transmission contains data bytes only (0), or not (1)
- D/C# bit: Next byte is command byte (0), or display data written to GDDRAM (1)


# Graphic Display Data Ram (GDDRA)

The GDDRAM is 128 x 64 bit static RAM. It's divided into 8 pages.

COM refers to a row. SEG refers to a column. A page consists of 8 COMs.

The frame frequency can be computed as follows...

$$$
F_{FRM} = \frac{F_{OSC}}{D \times K \times Mux}
$$$
- $F_{OSC}$ is the oscillator frequency. It can be set from command D5 A[7:4].
- $D$ is the clock divide ration. It can be set from command D5 A[3:0].
- $K$ is the number of display clocks per row. See section 6.6.
- $Mux$ is the multiplex ratio. It can be set from command A6.




## Page Addressing Mode

After each read/write, the column address pointer is incremented by 1.
At the end of the page, the user must increment the page address by 1 and reset the column address pointer.

The user must specify the following...
- Page start address:
- Lower start column address:
- Upper start column address: 

## Horizontal Addressing Mode

After each read/write, the column address pointer is incremented by 1.
At the end of the page, the page address is incremented by 1 and the column address pointer is reset.


## Verticcal Addressing Mode

No


## Common Commands

AE: Set display off
AF: Set display on

00-0F: Set start column address lower nibble (page addressing mode)
    - `0 0 0 0 x3 x2 x1 x0`
10-17: Set start column address upper nibble (page addressing mode)
    - `0 0 0 1 0 x2 x1 x0`

40-7F: Set display start line register
    - `0 1 x5 x4 x3 x2 x1 x0`

21 S E: Set column address start to S and end to E

22 S E: Set page address start to S and end to E

81 C: Set the contrast to C

A0: Set segment remapping to default. See figure 6-14. 
A1: Set segment remapping to flipped. See figure 6-14.

C0: Set scan mode to normal - COM[0] to COM[N-1]
C9: Set scan mode to rempped - COM[N-1] to COM[0] 
// TODO: N is multiplex ration. What is that??

A6: Set normal display mode (0 is off, 1 is on)
A7: Set inverse display mode (o is on, 1 is off)

A8 N[5:0]: Set the multiplex ratio to N+1. The upper 2 bits of N are ignored. N can be 15 to 63

D3 A[5:0]: Set the display offset to A. Defaults to 0. This offsets COLs


# Example Code

## Init (OLED Draw)

Note that most of these are just re-setting the default values and can be omitted.

0xAE // Set display off
0x00 // Set column start address lower nibble to 0
0x10 // Set column start address upper nibble to 0
0x40 // Set display start line register  -- DEFAULT
0x81 // Set contrast to CF -- UNNECESSARY
0xCF // --
0xA1 // Set segment remapping to flipped -- UNNECESSARY
0xC0 // Set normal row scan direction -- DEFAULT
0xA6 // Set normal (non-inverted) display mode -- DEFAULT
0xA8 // Set multiplex ratio to N=0x3f=63 => M = 64 -- DEFAULT
0x3f // --
0xD3 // Set display offset to 0 -- DEFAULT
0x00 // --
0xD5 // Set display clock divide ratio / oscillator frequency -- DEFAULT
0x80 // --
0xD9 // Set pre-charge periods. Phase 1 is 1 clock. Phase 2 is 30 clocks. -- UNSURE
0xF1 // --
0xDA // Set COM pin hardware configuration to alternative, left-right remap disabled - DEFAULT
0x12 // -- 
0xDB // Set COM select voltage -- UNSURE
0x40 // -- THIS IS INVALID. THIS SHOULD BE 0x20
0x20 // Set addressing mode to paged -- DEFAULT
0x02 // --
0x8D // Set charge pump to enabled
0x14 // --
0xA4 // Set display enabled
0xA6 // Set normal (non-inverted display mode)
0xAF // Set display on


## Init (STM32) 

LCD_IO_Init();

0x80
0x8D  /* Disable charge pump regulator */
0x14
0x20  /* Set Memory Addressing Mode */
0x00  /* 00b: Horizontal Addressing Mode */
0x40  /* 00b: Horizontal Addressing Mode */
0xC8  /* c8:flip the 64 rows */
0xA1  /* a1:flip the 128 columns */
0xAF  /* Display On */