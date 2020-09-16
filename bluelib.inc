; bluelib.inc - GG/SMS project boilerplate and standard library.
; Latest revision: Spring 2017 for project astroswab.
; *****************************************************************************
;                              BASE LIBRARY
; *****************************************************************************
.equ ROMSIZE 128                      ; Make a 128k or 256k rom.
.equ INCLUDE_VDP_INTERRUPT_HANDLER    ; Comment out to exclude it.
.equ INCLUDE_PAUSE_BUTTON_HANDLER     ; Comment out to exclude it.
; -----------------------------------------------------------------------------
; HARDWARE DEFINITIONS
; -----------------------------------------------------------------------------
; Video memory and initialization:
.equ NAME_TABLE_START $3800
.equ VISIBLE_NAME_TABLE_SIZE 2*32*24
.equ FULL_NAME_TABLE_SIZE 2*32*28
.equ SPRITE_BANK_START $2000
.equ BACKGROUND_BANK_START $0000
.equ SPRITE_PALETTE_START 16
.equ BACKGROUND_PALETTE_START 0
.equ SAT_Y_START $3f00
.equ SAT_XC_START SAT_Y_START+64+64
.equ SPRITE_TERMINATOR $d0
.equ NTSC 0
.equ PAL 1
.equ FIRST_LINE_OF_VBLANK 192
;
.equ REGISTER_0 0
.equ REGISTER_1 1
.equ REGISTER_2 2
.equ REGISTER_3 3
.equ REGISTER_4 4
.equ REGISTER_5 5
.equ REGISTER_6 6
.equ REGISTER_7 7
.equ REGISTER_8 8
.equ REGISTER_9 9
.equ REGISTER_10 10
.equ REGISTER_11 11
;
; Memory map and initialization:
.equ RAM_START $c000
.equ STACK_INIT_ADDRESS $dff0

; Port communication and control:
.equ SYSTEM_CONTROL_PORT $00          ; GG: start, region, ntsc/pal.
.equ START_BUTTON_BIT %10000000       ; 0 = Switch is on!
.equ INPUT_PORT_1 $dc
.equ INPUT_PORT_2 $dd
.equ PSG_PORT $7f
.equ V_COUNTER_PORT $7e
.equ CONTROL_PORT $BF
.equ DATA_PORT $BE
.equ VRAM_WRITE_COMMAND %01000000
.equ VRAM_READ_COMMAND %00000000
.equ REGISTER_WRITE_COMMAND %10000000
.equ CRAM_WRITE_COMMAND %11000000

; --- NB! Developed for GG -------------------
; Register 0 settings:
.equ SCROLL_0_LINE_0_SPRITES_0 %00000110
; Register 1 settings:
.equ DISPLAY_1_FRAME_1_SIZE_0 %11100000
.equ DISPLAY_0_FRAME_1_SIZE_0 %10100000
.equ DISPLAY_0_FRAME_0_SIZE_0 %10000000
.equ DISPLAY_1_FRAME_1_SIZE_0_ZOOM %11100001
; ---------------------------------------------
.equ SCROLL_0__LCB_1_LINE_0_SPRITES_0 %00100110
.equ SCROLL_0__LCB_0_LINE_0_SPRITES_0 %00000110

;
.equ HORIZONTAL_SCROLL_REGISTER 8
.equ VERTICAL_SCROLL_REGISTER 9
.equ RASTER_INTERRUPT_REGISTER 10

; Banks / ROM / External RAM control:
.equ SET_EXTRAM_BIT %00001000
.equ RESET_EXTRAM_BIT %11110111
.equ EXTRAM_START $8000
.equ EXTRAM_SIZE $4000
.equ SLOT_2_CONTROL $ffff
.equ BANK_CONTROL $fffc

; CRT screen values:
.equ CRT_LEFT_BORDER 0
.equ CRT_RIGHT_BORDER 255
.equ CRT_TOP_BORDER 0
.equ CRT_BOTTOM_BORDER 191

; LCD screen values:
.equ LCD_RIGHT_BORDER (6*8)+(20*8)
.equ LCD_LEFT_BORDER 6*8
.equ LCD_TOP_BORDER 3*8
.equ LCD_BOTTOM_BORDER (3*8)+(18*8)
.equ LCD_WIDTH 20*8
.equ LCD_HEIGHT 18*8

; Invisible area:
.equ INVISIBLE_AREA_TOP_BORDER 192
.equ INVISIBLE_AREA_BOTTOM_BORDER 224
; -----------------------------------------------------------------------------
; SOFTWARE DEFINITIONS
; -----------------------------------------------------------------------------
.equ TRUE $ff
.equ FALSE 0
.equ ENABLED $ff
.equ DISABLED 0
.equ FLAG_SET $ff
.equ FLAG_RESET $00
.equ PAUSE_FLAG_RESET $00
.equ PAUSE_FLAG_SET $ff
.equ UNUSED_BYTE $0
.equ UNUSED_WORD $0000
.equ PERCENT_CHANCE_50 128
.equ PERCENT_CHANCE_100 255
;
.equ BYTES_PER_TILE 32
.equ ONE_TILEMAP_ROW 32*2
.equ END_OF_TABLE $ff

; Font / printing support.
.equ CHARS_IN_ASCII_SET 91
.equ STRING_TERMINATOR 3        ; Terminate strings with #
.equ ASCII_ZERO 16        ; Where in the tilebank is the ASCII zero?
.equ ASCII_ONE 17
.equ ASCII_NINE ASCII_ZERO+9
.equ ASCII_SPACE 0
.equ ASCII_EXCLAMATION 1
.equ ASCII_TABLE_START 0
.asciitable
  map " " to "z" = ASCII_TABLE_START
.enda
; =============================================================================
; M A C R O S
; =============================================================================
; -----------------------------------------------------------------------------
.macro SELECT_EXTRAM
; -----------------------------------------------------------------------------
  ; Select external RAM: Now memory addresses from $8000 - $c000 (slot 2)
  ; are mapped to the battery-backed RAM, and thus its contents are saved
  ; between sessions.
  push af
  ld a,(BANK_CONTROL)
  or SELECT_EXTRAM_BIT
  ld (BANK_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_ROM
; -----------------------------------------------------------------------------
  ; Select ROM: Used to switch mapping in slot 2 ($8000 - $c000) back to ROM
  ; if external RAM was selected.
  push af
  ld a,(BANK_CONTROL)
  and RESET_EXTRAM_BIT
  ld (BANK_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_BANK
; -----------------------------------------------------------------------------
  ; Select a bank for slot 2, i.e. SELECT_BANK 4.
  push af
  ld a,\1
  ld (SLOT_2_CONTROL),a
  pop af
.endm
; -----------------------------------------------------------------------------
.macro SELECT_BANK_IN_REGISTER_A
; -----------------------------------------------------------------------------
  ; Select a bank for slot 2, - put value in register A.
  ld (SLOT_2_CONTROL),a
.endm
; -----------------------------------------------------------------------------
.macro SAVE_REGISTERS
; -----------------------------------------------------------------------------
  ; Save all registers, except IX and IY
  push af
  push bc
  push de
  push hl
  push ix
  push iy
.endm
; -----------------------------------------------------------------------------
.macro RESTORE_REGISTERS
; -----------------------------------------------------------------------------
  ; Restore all registers, except IX and IY
  pop iy
  pop ix
  pop hl
  pop de
  pop bc
  pop af
.endm

; -----------------------------------------------------------------------------
.memorymap
; -----------------------------------------------------------------------------
  defaultslot 0
  slotsize $4000
  slot 0 $0000
  slot 1 $4000
  slot 2 $8000
  slotsize $2000
  slot 3 RAM_START
.endme
.if ROMSIZE == 128
  .rombankmap ; 128K rom
    bankstotal 8
    banksize $4000
    banks 8
  .endro
.endif
.if ROMSIZE == 256
  .rombankmap ; 256K rom
    bankstotal 16
    banksize $4000
    banks 16
  .endro
.endif
; -----------------------------------------------------------------------------
.ramsection "bluelib global variables" slot 3
; -----------------------------------------------------------------------------
  VDPStatus db
  pause_flag db
  input_ports dw
  tv_type db
  ;
.ends

.org 0
.bank 0 slot 0
; -----------------------------------------------------------------------------
.section "Boot" force
; -----------------------------------------------------------------------------
boot:
  di
  im 1
  ld sp,STACK_INIT_ADDRESS
  
; Initialize the standard Sega mapper.
  ld de,$fffc
  ld hl,_MapperInitData
  ld bc,$0004
  ldir
  
  call ClearRam ; Except the top (stack, slot/bank registers, etc.)
  call ClearVRam

  call detect_tv_type ; Beware! Messes with interrupts and vdp-registers.
  ld (tv_type),a
  
  call InitializeVDPRegisters
  
  jp init
  
  _MapperInitData:
    .db $00,$00,$01,$02
.ends



; =============================================================================
; H E L P E R  F U N C T I O N S                        (sorted alphabetically)
; =============================================================================



; -----------------------------------------------------------------------------
.section "ClearRam" free
; -----------------------------------------------------------------------------
  ; Overwrite ram with zeroes from RAM_START to STACK_INIT_ADDRESS - 16
  ; Uses AF, BC, DE, HL
  ClearRam:
    ld hl,RAM_START
    ld bc,STACK_INIT_ADDRESS-16-RAM_START
    ld a,0
    call _FillMemory
  ret

  _FillMemory:
    ld (hl),a
    ld d,h
    ld e,l
    inc de
    dec bc
    ld a,b
    or c
    ret z
    ldir
  ret
.ends
;
; -----------------------------------------------------------------------------
.section "ClearVRam" free
; -----------------------------------------------------------------------------
  ; Write 00 to all vram positions. Assumes disabled display.
  ; Uses AF, BC
  ClearVRam:
    xor a
    out (CONTROL_PORT),a
    or VRAM_WRITE_COMMAND
    out (CONTROL_PORT),a
    ld bc,$4000 ; 16K
    -:
      xor a
      out (DATA_PORT),a
      dec bc
      ld a,b
      or c
    jp nz,-
  ret
.ends
;

; -----------------------------------------------------------------------------
.section "detect_tv_type" free
; -----------------------------------------------------------------------------
  detect_tv_type:
    ; Returns a=0 for NTSC, a=1 for PAL
    ; Uses: a, hl, de
    ; From SMS-Power!
    di             ; disable interrupts
    ld a,%01100000 ; set VDP such that the screen is on
    out ($bf),a    ; with VBlank interrupts enabled
    ld a,$81
    out ($bf),a
    ld hl,$0000    ; init counter
-:  in a,($bf)     ; get VDP status
    or a           ; inspect
    jp p,-         ; loop until frame interrupt flag is set
-:  in a,($bf)     ; do the same again, in case we were unlucky and came in just
    or a           ;   before the start of the VBlank with the flag already set
    jp p,-
    ; the VDP must now be at the start of the VBlank
-:  inc hl         ; (6 cycles) increment counter until interrupt flag comes on again
    in a,($bf)     ; (11 cycles)
    or a           ; (4 cycles)
    jp p,-         ; (10 cycles)
    xor a          ; reset carry flag, also set a=0
    ld de,2048     ; see if hl is more or less than 2048
    sbc hl,de
    ret c          ; if less, return a=0
    ld a,1
    ret            ; if more or equal, return a=1
.ends
;

; -----------------------------------------------------------------------------
.section "InitializeVDPRegisters" free
; -----------------------------------------------------------------------------
; Initialize the 11 VDP registers with the preset data block.
; Uses: AF, B, HL
  _VDPRegisterInitData:
    .db SCROLL_0_LINE_0_SPRITES_0
    .db DISPLAY_0_FRAME_0_SIZE_0
    .db $ff,$ff,$ff,$ff,$ff,$00,$00,$00,$ff

  InitializeVDPRegisters:
    ld hl,_VDPRegisterInitData
    xor b
    -:
      ld a,(hl)
      out (CONTROL_PORT),a
      inc hl
      ld a,b
      or REGISTER_WRITE_COMMAND
      out (CONTROL_PORT),a
      cp REGISTER_WRITE_COMMAND|10
      ret z
      inc b
    jr -
.ends
;