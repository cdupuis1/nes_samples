;
; framework.asm
;
; Basic NES program to initialize the hardware
;
; Compile instructions:
;
; ca65 framework.asm -o framework.o -t nes
; ld65 framework.o -o framework.nes -t nes
;
; CPU Memory Map
;
; From https://gamefaqs.gamespot.com/nes/916386-nes/faqs/2948
;
;--------------------------------------- $10000
; Upper Bank of Cartridge ROM
;--------------------------------------- $C000
; Lower Bank of Cartridge ROM
;--------------------------------------- $8000
; Cartridge RAM (may be battery-backed)
;--------------------------------------- $6000
; Expansion Modules
;--------------------------------------- $5000
; Input/Output
;--------------------------------------- $2000
; 2kB Internal RAM, mirrored 4 times
;--------------------------------------- $0000
;
; Also used https://pineight.com/nes/#template for reference
.segment "HEADER" ; 16 bytes
    .byte "NES" ; Idendifies as NES ROM header
    .byte $1a   ; Also part of the NES ROM header signature
    .byte $02   ; Number of 16K Program ROM Blocks
    .byte $01   ; Number of 8K Character ROM Blocks
    .byte %00000000 ; ROM control byte
    .byte $00   ; Mapper number to use
    .byte $00, $00, $00, $00, $00, $00, $00, $00 ; Padding bytes for unused flags and such

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

; If this were a real game we could branch to a main function or
; just keep looping forever until the VBLANK interrupt to do our
; main game play
Forever:
  JMP Forever

VBLANK:
  ; Game code would go here
  RTI

.segment "VECTORS"
    .word VBLANK ; NMI interrupt
    .word RESET  ; RESET interrupt
    .word 0      ; IRQ interrupt

.segment "RODATA"
; Read only data such as palette data would go here

.segment "CHARS"
; an .incbin like "your.chr" file with sprite data would go here