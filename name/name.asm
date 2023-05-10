;
; name.asm
;
; Print my name to the screen
;
.segment "HEADER" ; 16 bytes
    .byte "NES" ; Idendifies as NES ROM header
    .byte $1a   ; Also part of the NES ROM header signature
    .byte $02   ; Number of 16K Program ROM Blocks
    .byte $01   ; Number of 8K Character ROM Blocks
    .byte %00000000 ; ROM control byte
    .byte $00   ; Mapper number to use
    .byte $00, $00, $00, $00, $00, $00, $00, $00 ; Padding bytes for unused flags and such

; Defines
PPUCTRL = $2000
PPUMASK = $2001
OAMADDR = $2003
PPUADDR = $2006
PPUDATA = $2007
OAM_DMA = $4014

.segment "STARTUP"

; Put the most common accessed variables here.  Memory addresses
; $00-$FF are faster access than other areas of the memory map
.segment "ZEROPAGE"

; Actual executable code for the game goes in this segment.  This starts at
; $8000 in the NES memory map
.segment "CODE"

; Helper function to check if VBLANK interrupt has happened
WAITVBLANK:
    BIT $2002 ; Test PPUSTATUS register with the found found in register A
    BPL WAITVBLANK ; Branch if PLus flag is set
    RTS ; Return from subroutine

; Reset vector.  NES jumps to this code when it powers on
RESET:
  SEI          ; disable IRQs
  CLD          ; disable decimal mode
  LDX #$40
  STX $4017    ; disable APU frame IRQ
  LDX #$FF      ; Stack starts at $0xff
  TXS          ; Set up stack base address
  INX          ; now X = 0; Increment 0xff will be 0x00
  STX $2000    ; disable NMI
  STX $2001    ; disable rendering
  STX $4010    ; disable DMC IRQs

  JSR WAITVBLANK

; Zero out general purpose RAM
clrmem:
  LDA #$00 ; Value to write to memory is stored in accumulator
  STA $0000, x ; This memory address is abosolute + offset stored in x
  STA $0100, x
  STA $0200, x
  STA $0400, x
  STA $0500, x
  STA $0600, x
  STA $0700, x
  LDA #$FE
  STA $0300, x
  INX
  BNE clrmem

  JSR WAITVBLANK

  ; Load the palettes
  LDA #$3F ; $3f00 is the memory mapped address for the PPU palette memory
  STA PPUADDR ; Store BG palette address into PPUADDR both hi and low bytes
  LDA #$00
  STA PPUADDR
  LDX #$00
LoadPaletteLoop:
  LDA MyPalettes,x ; Load address of the MyPalettes plus offset in x
  STA PPUDATA
  INX
  CPX #$20 ; Load 32 bytes
  BNE LoadPaletteLoop 

  ; Reenable vblank interrupt and drawing to the screen
  LDA #%10010000  ; turns on NMI, just like in RESET
  STA PPUCTRL
  LDA #%00011110  ; turns on rendering, just like in RESET
  STA PPUMASK
Forever:
  ; Game logic could go here

  ; Set sprites to spell my name
  ;
  ; Information for a sprite is 4 bytes:
  ; byte 0 - Y position of the sprite
  ; byte 1 - Sprite tile to use
  ; byte 2 - Attribute data including which palette to use
  ; byte 3 - X position of the sprite

  ; C
  LDA #8 ; Y position
  STA $200
  LDA #34 ; Sprite tile to use
  STA $201
  LDA #0 ; X position
  STA $203

  ; h
  LDA #8
  STA $204
  LDA #71 ;
  STA $205
  LDA #8
  STA $207

  ; a
  LDA #8
  STA $208
  LDA #$40
  STA $209
  LDA #16
  STA $20b

  ; d
  LDA #8
  STA $20c
  LDA #$43
  STA $20d
  LDA #24
  STA $20f

  ; Change the Y value to 17 as any line on the NES can only have 8 sprites

  ; D
  LDA #17
  STA $210
  LDA #$23
  STA $211
  LDA #0
  STA $213

  ; u
  LDA #17
  STA $214
  LDA #$54
  STA $215
  LDA #8
  STA $217

  ; p
  LDA #17
  STA $218
  LDA #$4f
  STA $219
  LDA #16
  STA $21b

  ; u
  LDA #17
  STA $21c
  LDA #$54
  STA $21d
  LDA #24
  STA $21f

  ; i
  LDA #17
  STA $220
  LDA #$48
  STA $221
  LDA #32
  STA $223

  ; s
  LDA #17
  STA $224
  LDA #$52
  STA $225
  LDA #40
  STA $227

  JMP Forever

VBLANK:
  ; Reload sprite data to DMA to PPU
  LDA #$00     ; Low byte of sprite data address is 00
  STA OAMADDR  ; sets the low byte of the sprite RAM address
  LDA #$02     ; High byte of sprite data address is 20
  STA OAM_DMA    ; sets high byte of the RAM address

  RTI

.segment "VECTORS"
    .word VBLANK ; NMI interrupt
    .word RESET  ; RESET interrupt
    .word 0      ; IRQ interrupt

.segment "RODATA"
; Palette data
;
; A palette is made up of four colors and there are eight total palettes:
;
; 4 background
; 4 sprite
MyPalettes:
; Background palettes
.byte $E, $20, $15, $19,  $E, $1, $2, $3,  $E, $4, $5, $6,  $E, $7, $8, $9

; Sprite palettes
.byte $E, $20, $15, $19,  $E, $a, $b, $c,  $E, $d, $e, $f,  $E, $10, $11, $12

.segment "CHARS"

.incbin "ascii.chr"