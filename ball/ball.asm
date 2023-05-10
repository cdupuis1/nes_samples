;
; ball.asm
;
; Bounce a ball off the sides of the screen
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
  Y_POS_ADDR = $0000
  Y_DIR_ADDR = $0001
  X_POS_ADDR = $0002
  X_DIR_ADDR = $0003
  Y_POS_MIN = 8
  Y_POS_MAX = 224
  X_POS_MIN = 0
  X_POS_MAX = (256 - 8)
  UP = 0
  DOWN = 1
  LEFT = 0
  RIGHT = 1

.segment "STARTUP"

; Put the most common accessed variables here.  Memory addresses
; $00-$FF are faster access than other areas of the memory map
.segment "ZEROPAGE"

; Actual executable code for the game goes in this segment.  This starts at
; $8000 in the NES memory map
.segment "CODE"

;
; Vector handlers
;

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

  JSR wait_v_blank

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

  ; Initialize ball position values
  LDA #120 ; Center y position
  STA Y_POS_ADDR
  LDA #DOWN
  STA Y_DIR_ADDR
  LDA #128 ; Center x position
  STA X_POS_ADDR

  JSR wait_v_blank

  ; Load the palettes
  LDA #$3F ; $3f00 is the memory mapped address for the PPU palette memory
  STA PPUADDR ; Store BG palette address into PPUADDR both hi and low bytes
  LDA #$00
  STA PPUADDR
  LDX #$00

load_palette_loop:
  LDA my_palettes,x ; Load address of the my_palettes plus offset in x
  STA PPUDATA
  INX
  CPX #$20 ; Load 32 bytes
  BNE load_palette_loop 

  ; Reenable vblank interrupt and drawing to the screen
  LDA #%10010000  ; turns on NMI, just like in RESET
  STA PPUCTRL
  LDA #%00011110  ; turns on rendering, just like in RESET
  STA PPUMASK
  
forever:
  ; Game logic could go here
  JMP forever

; NMI handler.  This fires inbetween drawing the screen
VBLANK:
  ; Reload sprite data to DMA to PPU
  LDA #$00     ; Low byte of sprite data address is 00
  STA OAMADDR  ; sets the low byte of the sprite RAM address
  LDA #$02     ; High byte of sprite data address is 20
  STA OAM_DMA  ; sets high byte of the RAM address

  ; Set position of ball sprite.  We set the data in the vblank interrupt since
  ; we only want to increment by 1 before the screen is redrawn
  JSR set_y_dir
  JSR set_x_dir

  ; Set sprite 0 data
  LDA Y_POS_ADDR  
  STA $200 ; Y position
  LDA #0 ; Sprite tile to use
  STA $201
  LDA X_POS_ADDR ; X position
  STA $203
  RTI

;
; Subroutines
;

; Helper function to check if VBLANK interrupt has happened
wait_v_blank:
  BIT $2002 ; Test PPUSTATUS register with the found found in register A
  BPL wait_v_blank ; Branch if PLus flag is set
  RTS ; Return from subroutine

; Subroutine sets the y position and direction
set_y_dir:
  LDA Y_DIR_ADDR
  CMP #DOWN
  BEQ inc_y_pos
  BNE dec_y_pos
inc_y_pos:
  LDX Y_POS_ADDR
  INX
  STX Y_POS_ADDR
  CPX #Y_POS_MAX
  BEQ switch_dir_up
  JMP set_y_dir_end
switch_dir_up:
  LDA #UP
  STA Y_DIR_ADDR
  JMP set_y_dir_end
dec_y_pos:
  LDX Y_POS_ADDR
  DEX
  STX Y_POS_ADDR
  CPX #Y_POS_MIN
  BEQ switch_dir_down
  JMP set_y_dir_end
switch_dir_down:
  LDA #DOWN
  STA Y_DIR_ADDR
set_y_dir_end:
  RTS

; Subroutine sets the x position and direction
set_x_dir:
  LDA X_DIR_ADDR
  CMP #RIGHT
  BEQ inc_x_pos
  BNE dec_x_pos
inc_x_pos:
  LDX X_POS_ADDR
  INX
  STX X_POS_ADDR
  CPX #X_POS_MAX
  BEQ switch_dir_left
  JMP set_x_dir_end
switch_dir_left:
  LDA #LEFT
  STA X_DIR_ADDR
  JMP set_x_dir_end
dec_x_pos:
  LDX X_POS_ADDR
  DEX
  STX X_POS_ADDR
  CPX #X_POS_MIN
  BEQ switch_dir_right
  JMP set_x_dir_end
switch_dir_right:
  LDA #RIGHT
  STA X_DIR_ADDR
set_x_dir_end:
  RTS

;
; Vector address table
;
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
my_palettes:
; Background palette info
.byte $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f, $0f

; Sprite palettes
;
; Note that first byte of each palette is the background color for that
; sprite
.byte $0f, $24, $20, $00,  $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00, $00

.segment "CHARS"

.incbin "ball.chr"