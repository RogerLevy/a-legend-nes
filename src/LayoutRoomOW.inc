; (( Repeated in every overworld "chunk" ))

.INCLUDE "Variables.inc"

; Imports from RAM code bank 01

.IMPORT AddToInt16At0

; Params:
; Load the address of world flags in [$08:09].

LevelInfo = $08

LayoutRoomOrCaveOW_Common:
    LDA LevelInfo_WorldFlagsAddr
    STA LevelInfo
    LDA LevelInfo_WorldFlagsAddr+1
    STA LevelInfo+1
    
    LDA #<RoomTestData
    STA $02
    LDA #>RoomTestData
    STA $03

    LDY #$00
    STY $07 ; source offset
    STY $06 ; dest offset
    
@LoopRow:
    JSR FetchTileMapAddr
    JSR @DoColumn
    INC $06
    INC $06
    LDA $06
    CMP #22
    BNE @LoopRow
    RTS

@DoColumn:
    LDA #16
    PHA
@LoopColumn:
    LDY $07
    INC $07
    LDA ($02), Y
    TAY
    LDA SquareConversionMap, Y
    SEC
    SBC #1
    TAX
    STX $0D ; "square index"
    LDA PrimarySquaresOW, X  
    LDY $06
    JSR WriteSquareOW
       
    ; go to next column in dest
    ; (we're laying out left-to-right, top-to-bottom, but the play area is stored vertically)
    LDA #44
    JSR AddToInt16At0
    
    ; done?
    PLA
    SEC
    SBC #1 
    PHA
    BNE @LoopColumn
    PLA
    
    RTS

; Params:
; A: primary square
; Y: offset from [$00:01]
; [$0D]: square index
; [$00:01]: pointer to play area
;
WriteSquareOW:
    LDX $0D                     ; Get square index.
    CPX #$10
    BCC @WriteType3             ; If square index < $10, go handle a secondary square.

    ; Type 1 square.
    ; Primary is the first tile. Next 3 tiles in CHR form the rest of the square.
    ; Square index >= $10.
    TAX
    STA ($00), Y                ; Write tile+0 to (col, row).
    INY
    INX
    TXA
    STA ($00), Y                ; Write tile+1 to (col, row+1).
    TYA
    CLC
    ADC #$15
    TAY
    INX
    TXA
    STA ($00), Y                ; Write tile+2 to (col+1, row).
    INX
    TXA

@WriteLastTile:
    INY
    STA ($00), Y                ; Write last tile to (col+1, row+1).
    RTS

@WriteType3:
    ; Type 3 square.
    ; Square index refers to a set of 4 tile indexes in secondary squares table.
    ; Square index < $10.
    TXA                         ; X := (square index * 4)
    ASL
    ASL
    TAX
    LDA SecondarySquaresOW, X
    STA ($00), Y                ; Write tile+0 to (col, row).
    INY
    INX
    LDA SecondarySquaresOW, X
    STA ($00), Y                ; Write tile+1 to (col, row+1).
    TYA
    CLC
    ADC #$15
    TAY
    INX
    LDA SecondarySquaresOW, X
    STA ($00), Y                ; Write tile+2 to (col+1, row).
    INX
    LDA SecondarySquaresOW, X
    JMP @WriteLastTile          ; Go write tile+3 to (col+1, row+1).

; Returns:
; [$00:01]: address of room tile map
FetchTileMapAddr:
    LDA #$30
    STA $00
    LDA #$65
    STA $01
    RTS

SquareConversionMap:
    ;0
    .BYTE 0,28,6,54,53,1,15,51,52,20
    ;10
    .BYTE 22,7,23,34,35,36,46,9,10,45
    ;20
    .BYTE 38,37,11,27,24,8,25,48,47,55
    ;30
    .BYTE 29,31,30,32,33,28,26,26,54,15
    ;40
    .BYTE 56,21,34,46,36,0,0,0,0,0
    ;50
    .BYTE 26,35,45,37,38,49,7,23,6,10
    ;60
    .BYTE 27,11,28,53,54,53,30,22,34,35
    ;70
    .BYTE 36,6,45,37,38,12,24,8,25,52
    ;80
    .BYTE 51,51,52,27,11,46,47,20,12,48,50


RoomTestData:
    ;.BYTE 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
    ;.BYTE 16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ;.BYTE 32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
    ;.BYTE 48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
    .BYTE 58,58,58,58,58,58,59,6,6,50,50,50,50,50,50,50,58,58,58,58,58,58,59,6,6,50,50,50,50,50,50,50,58,58,58,58,58,58,59,6,6,6,6,6,6,6,6,6,58,58,58,58,58,58,59,6,6,6,19,6,19,6,6,6,77,77,77,77,77,77,78,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,83,83,83,6,6,6,6,6,6,19,6,6,6,19,6,6,62,62,62,6,6,6,6,6,6,6,19,19,19,6,6,6,62,62,62,6,6,6,6,6,6,6,6,6,6,6,6,6,62,62,62,50,50,50,50,50,50,50,50,50,50,50,50,50,62,62,62,50,50,50,50,50,50,50,50,50,50,50,50,50

PrimarySquaresOW:
    .BYTE $24, $6F, $F3, $FA, $98, $90, $8F, $95
    .BYTE $8E, $90, $74, $76, $F3, $24, $26, $89
    .BYTE $03, $04, $70, $C8, $BC, $8D, $8F, $93
    .BYTE $95, $C4, $CE, $D8, $B0, $B4, $AA, $AC
    .BYTE $B8, $9C, $A6, $9A, $A2, $A0, $E5, $E6
    .BYTE $E7, $E8, $E9, $EA, $C0, $E0, $78, $7A
    .BYTE $7E, $80, $CC, $D0, $D4, $DC, $89, $84

SecondarySquaresOW:
    .BYTE $24, $24, $24, $24, $6F, $6F, $6F, $6F
    .BYTE $F3, $F3, $F3, $F3, $FA, $FA, $FA, $FA
    .BYTE $98, $95, $26, $26, $90, $95, $90, $95
    .BYTE $8F, $90, $8F, $90, $95, $96, $95, $96
    .BYTE $8E, $93, $90, $95, $90, $95, $92, $97
    .BYTE $74, $74, $75, $75, $76, $77, $76, $77
    .BYTE $F3, $24, $F3, $24, $24, $24, $24, $24
    .BYTE $26, $26, $26, $26, $89, $88, $8B, $88

