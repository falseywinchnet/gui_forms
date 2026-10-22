	.build_version macos, 26, 0	sdk_version 26, 2
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function main
lCPI0_0:
	.byte	111                             ; 0x6f
	.byte	220                             ; 0xdc
	.byte	46                              ; 0x2e
	.byte	19                              ; 0x13
	.space	1
	.space	1
	.space	1
	.space	1
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.p2align	2
_main:                                  ; @main
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	sub	sp, sp, #208
	stp	x28, x27, [sp, #112]            ; 16-byte Folded Spill
	stp	x26, x25, [sp, #128]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #144]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #160]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #176]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #192]            ; 16-byte Folded Spill
	add	x29, sp, #192
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	str	x1, [sp, #56]                   ; 8-byte Spill
	mov	x20, x0
	mov	w0, #12                         ; =0xc
	movk	w0, #6, lsl #16
	bl	__Znwm
	mov	x19, x0
	mov	w1, #12                         ; =0xc
	movk	w1, #6, lsl #16
	bl	_bzero
Ltmp0:                                  ; EH_LABEL
	mov	w0, #12                         ; =0xc
	movk	w0, #11, lsl #16
	str	x19, [sp, #96]                  ; 8-byte Spill
	bl	__Znwm
Ltmp1:                                  ; EH_LABEL
; %bb.1:
	mov	x22, x0
	add	x27, x19, #96, lsl #12          ; =393216
	mov	w8, w20
	str	x8, [sp, #48]                   ; 8-byte Spill
	add	x8, x0, #175, lsl #12           ; =716800
	add	x8, x8, #4084
	str	x8, [sp, #88]                   ; 8-byte Spill
	add	x8, x0, #95, lsl #12            ; =389120
	add	x19, x8, #4048
	mov	w1, #65524                      ; =0xfff4
	movk	w1, #5, lsl #16
	bl	_bzero
	add	x8, x22, #24
	mov	w9, #32764                      ; =0x7ffc
	movi	d0, #0x00ffffffffffff
	mov	w10, #-1                        ; =0xffffffff
LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
	stur	d0, [x8, #-24]
	stur	d0, [x8, #-12]
	str	d0, [x8]
	stur	d0, [x8, #12]
	stur	w10, [x8, #-16]
	stur	w10, [x8, #-4]
	str	w10, [x8, #8]
	str	w10, [x8, #20]
	add	x8, x8, #48
	subs	x9, x9, #4
	b.ne	LBB0_2
; %bb.3:
	str	w20, [sp, #68]                  ; 4-byte Spill
	movi	d0, #0x00ffffffffffff
	str	d0, [x19]
	mov	w20, #-1                        ; =0xffffffff
	str	w20, [x19, #8]
	stur	d0, [x19, #12]
	str	w20, [x19, #20]
	str	d0, [x19, #24]
	str	w20, [x19, #32]
	mov	w8, #65524                      ; =0xfff4
	movk	w8, #5, lsl #16
	add	x0, x22, x8
	mov	w1, #327680                     ; =0x50000
	bl	_bzero
	ldr	x8, [sp, #88]                   ; 8-byte Reload
	str	w20, [x8]
	stur	xzr, [x8, #12]
	stur	xzr, [x8, #4]
	str	wzr, [x8, #20]
Lloh0:
	adrp	x8, ___stderrp@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh2:
	ldr	x0, [x8]
	mov	w9, #12                         ; =0xc
	movk	w9, #6, lsl #16
	add	x8, x9, #80, lsl #12            ; =327680
	stp	x9, x8, [sp]
Lloh3:
	adrp	x1, l_.str@PAGE
Lloh4:
	add	x1, x1, l_.str@PAGEOFF
	bl	_fprintf
Ltmp3:                                  ; EH_LABEL
	mov	w0, #513                        ; =0x201
	stur	x22, [x29, #-88]                ; 8-byte Folded Spill
	bl	__Znwm
Ltmp4:                                  ; EH_LABEL
; %bb.4:
	mov	x23, x0
	mov	w1, #513                        ; =0x201
	bl	_bzero
	mov	w12, #0                         ; =0x0
	add	x8, x22, #175, lsl #12          ; =716800
	add	x8, x8, #4092
	str	x8, [sp, #80]                   ; 8-byte Spill
	ldr	x20, [sp, #96]                  ; 8-byte Reload
	add	x26, x20, #64, lsl #12          ; =262144
	mov	w13, #9363                      ; =0x2493
	b	LBB0_6
LBB0_5:                                 ;   in Loop: Header=BB0_6 Depth=1
	mov	x0, x23
	mov	w1, #513                        ; =0x201
	mov	x2, x20
	mov	x3, x22
	bl	__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
	ldr	w12, [sp, #72]                  ; 4-byte Reload
	add	w12, w12, #1
	cmp	w12, #4
	mov	w13, #9363                      ; =0x2493
	b.eq	LBB0_53
LBB0_6:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_9 Depth 2
                                        ;     Child Loop BB0_17 Depth 2
                                        ;       Child Loop BB0_19 Depth 3
                                        ;         Child Loop BB0_26 Depth 4
                                        ;           Child Loop BB0_30 Depth 5
                                        ;           Child Loop BB0_34 Depth 5
                                        ;         Child Loop BB0_44 Depth 4
                                        ;           Child Loop BB0_45 Depth 5
	mov	x8, #0                          ; =0x0
	mov	w9, #3493                       ; =0xda5
	movk	w9, #37345, lsl #16
	b	LBB0_9
LBB0_7:                                 ;   in Loop: Header=BB0_9 Depth=2
	and	w10, w10, #0x3
LBB0_8:                                 ;   in Loop: Header=BB0_9 Depth=2
	strb	w10, [x23, x8]
	add	x8, x8, #1
	cmp	x8, #513
	b.eq	LBB0_15
LBB0_9:                                 ;   Parent Loop BB0_6 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	eor	w9, w9, w9, lsl #13
	eor	w10, w9, w9, lsr #17
	eor	w9, w10, w10, lsl #5
	cbz	w12, LBB0_13
; %bb.10:                               ;   in Loop: Header=BB0_9 Depth=2
	cmp	w12, #2
	b.eq	LBB0_7
; %bb.11:                               ;   in Loop: Header=BB0_9 Depth=2
	cmp	w12, #1
	b.ne	LBB0_14
; %bb.12:                               ;   in Loop: Header=BB0_9 Depth=2
	mul	w10, w8, w13
	lsr	w10, w10, #16
	sub	w11, w8, w10
	and	w11, w11, #0xfffe
	add	w10, w10, w11, lsr #1
	ubfx	w10, w10, #2, #14
	sub	w10, w10, w10, lsl #3
	add	w10, w8, w10
	b	LBB0_8
LBB0_13:                                ;   in Loop: Header=BB0_9 Depth=2
	mov	w10, #0                         ; =0x0
	b	LBB0_8
LBB0_14:                                ;   in Loop: Header=BB0_9 Depth=2
	mov	x10, x9
	b	LBB0_8
LBB0_15:                                ;   in Loop: Header=BB0_6 Depth=1
	str	w12, [sp, #72]                  ; 4-byte Spill
	mov	x21, #0                         ; =0x0
	b	LBB0_17
LBB0_16:                                ;   in Loop: Header=BB0_17 Depth=2
	add	x21, x21, #1
	cmp	x21, #261
	ldur	x22, [x29, #-88]                ; 8-byte Folded Reload
	b.eq	LBB0_5
LBB0_17:                                ;   Parent Loop BB0_6 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB0_19 Depth 3
                                        ;         Child Loop BB0_26 Depth 4
                                        ;           Child Loop BB0_30 Depth 5
                                        ;           Child Loop BB0_34 Depth 5
                                        ;         Child Loop BB0_44 Depth 4
                                        ;           Child Loop BB0_45 Depth 5
	mov	x0, x20
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	str	xzr, [x27]
	str	wzr, [x27, #8]
	ldr	x8, [sp, #88]                   ; 8-byte Reload
	mov	w9, #-1                         ; =0xffffffff
	str	x9, [x8]
	mov	w8, #65524                      ; =0xfff4
	movk	w8, #5, lsl #16
	add	x0, x22, x8
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	mov	w8, #65524                      ; =0xfff4
	movk	w8, #8, lsl #16
	add	x0, x22, x8
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	ldr	x8, [sp, #80]                   ; 8-byte Reload
	stp	xzr, xzr, [x8]
	cbz	x21, LBB0_16
; %bb.18:                               ;   in Loop: Header=BB0_17 Depth=2
	mov	x25, #0                         ; =0x0
	mov	x24, x21
LBB0_19:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ; =>    This Loop Header: Depth=3
                                        ;         Child Loop BB0_26 Depth 4
                                        ;           Child Loop BB0_30 Depth 5
                                        ;           Child Loop BB0_34 Depth 5
                                        ;         Child Loop BB0_44 Depth 4
                                        ;           Child Loop BB0_45 Depth 5
	cmp	x24, #258
	mov	w9, #258                        ; =0x102
	csel	x28, x24, x9, lo
	sub	x8, x21, x25
	cmp	x8, #258
	csel	x19, x8, x9, lo
	cmp	x8, #2
	b.hi	LBB0_21
; %bb.20:                               ;   in Loop: Header=BB0_19 Depth=3
	mov	x20, #0                         ; =0x0
	b	LBB0_40
LBB0_21:                                ;   in Loop: Header=BB0_19 Depth=3
	add	x9, x23, x25
	ldrb	w8, [x9]
	ldrb	w10, [x9, #1]
	ldrb	w11, [x9, #2]
	ubfiz	x8, x8, #12, #4
	eor	x8, x8, x10, lsl #6
	eor	x8, x8, x11
	ldr	w10, [x20, x8, lsl #2]
	ldr	w11, [x27]
	ldr	w12, [x27, #8]
	sub	w14, w25, w11
	subs	w11, w12, w14
	csel	w13, wzr, w11, lo
	cmn	w10, #1
	b.eq	LBB0_38
; %bb.22:                               ;   in Loop: Header=BB0_19 Depth=3
	mov	w11, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	ldr	w15, [x27, #4]
	add	w14, w15, w14
	mov	x15, x10
	b	LBB0_26
LBB0_23:                                ;   in Loop: Header=BB0_26 Depth=4
	eor	x17, x2, x1
	rbit	x17, x17
	clz	x17, x17
	add	w16, w16, w17, lsr #3
LBB0_24:                                ;   in Loop: Header=BB0_26 Depth=4
	sub	w17, w25, w15
	cmp	w16, #2
	ccmp	w16, w12, #0, hi
	csel	w12, w16, w12, hi
	csel	w11, w17, w11, hi
	cmp	w16, w19
	b.eq	LBB0_39
LBB0_25:                                ;   in Loop: Header=BB0_26 Depth=4
	and	w15, w15, #0x7fff
	ldr	w15, [x26, w15, uxtw #2]
	cmn	w15, #1
	b.eq	LBB0_39
LBB0_26:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ;       Parent Loop BB0_19 Depth=3
                                        ; =>      This Loop Header: Depth=4
                                        ;           Child Loop BB0_30 Depth 5
                                        ;           Child Loop BB0_34 Depth 5
	cmp	w12, #3
	b.lo	LBB0_28
; %bb.27:                               ;   in Loop: Header=BB0_26 Depth=4
	add	w16, w12, w25
	ldrb	w16, [x23, w16, uxtw]
	add	w17, w15, w12
	ldrb	w17, [x23, w17, uxtw]
	cmp	w16, w17
	b.ne	LBB0_25
LBB0_28:                                ;   in Loop: Header=BB0_26 Depth=4
	cmp	w15, w14
	csel	w16, w13, wzr, eq
	sub	w17, w19, w16
	cmp	w17, #8
	b.lo	LBB0_32
; %bb.29:                               ;   in Loop: Header=BB0_26 Depth=4
	add	x17, x23, w15, uxtw
	sub	w0, w28, w16
LBB0_30:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ;       Parent Loop BB0_19 Depth=3
                                        ;         Parent Loop BB0_26 Depth=4
                                        ; =>        This Inner Loop Header: Depth=5
	ldr	x1, [x9, w16, uxtw]
	ldr	x2, [x17, w16, uxtw]
	cmp	x1, x2
	b.ne	LBB0_23
; %bb.31:                               ;   in Loop: Header=BB0_30 Depth=5
	add	w16, w16, #8
	sub	w0, w0, #8
	cmp	w0, #7
	b.hi	LBB0_30
LBB0_32:                                ;   in Loop: Header=BB0_26 Depth=4
	cmp	w16, w19
	b.hs	LBB0_24
; %bb.33:                               ;   in Loop: Header=BB0_26 Depth=4
	mov	w17, w16
	sub	w16, w28, w16
	mov	w0, w15
LBB0_34:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ;       Parent Loop BB0_19 Depth=3
                                        ;         Parent Loop BB0_26 Depth=4
                                        ; =>        This Inner Loop Header: Depth=5
	add	w1, w25, w17
	ldrb	w1, [x23, w1, uxtw]
	add	w2, w0, w17
	ldrb	w2, [x23, w2, uxtw]
	cmp	w1, w2
	b.ne	LBB0_37
; %bb.35:                               ;   in Loop: Header=BB0_34 Depth=5
	add	x17, x17, #1
	subs	w16, w16, #1
	b.ne	LBB0_34
; %bb.36:                               ;   in Loop: Header=BB0_26 Depth=4
	mov	x16, x19
	b	LBB0_24
LBB0_37:                                ;   in Loop: Header=BB0_26 Depth=4
	mov	x16, x17
	b	LBB0_24
LBB0_38:                                ;   in Loop: Header=BB0_19 Depth=3
	mov	w12, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
LBB0_39:                                ;   in Loop: Header=BB0_19 Depth=3
	str	w10, [x26, x25, lsl #2]
	str	w25, [x20, x8, lsl #2]
	sub	w8, w25, w11
	stp	w25, w8, [x27]
	str	w12, [x27, #8]
	mov	w8, w12
	orr	x20, x8, x11, lsl #32
LBB0_40:                                ;   in Loop: Header=BB0_19 Depth=3
	lsr	x22, x20, #32
	ldur	x0, [x29, #-88]                 ; 8-byte Folded Reload
	mov	x1, x23
	mov	x2, x21
	mov	x3, x25
	mov	w4, #1                          ; =0x1
	bl	__ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	lsr	x9, x0, #32
	cmp	w0, w20
	b.ne	LBB0_126
; %bb.41:                               ;   in Loop: Header=BB0_19 Depth=3
	cmp	x9, x22
	b.ne	LBB0_126
; %bb.42:                               ;   in Loop: Header=BB0_19 Depth=3
	cbz	x25, LBB0_51
; %bb.43:                               ;   in Loop: Header=BB0_19 Depth=3
	mov	w8, #0                          ; =0x0
	mov	w9, #0                          ; =0x0
	mov	w10, w25
	sub	x11, x10, #1
	mov	w12, #1                         ; =0x1
LBB0_44:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ;       Parent Loop BB0_19 Depth=3
                                        ; =>      This Loop Header: Depth=4
                                        ;           Child Loop BB0_45 Depth 5
	mov	x13, #0                         ; =0x0
LBB0_45:                                ;   Parent Loop BB0_6 Depth=1
                                        ;     Parent Loop BB0_17 Depth=2
                                        ;       Parent Loop BB0_19 Depth=3
                                        ;         Parent Loop BB0_44 Depth=4
                                        ; =>        This Inner Loop Header: Depth=5
	add	w14, w10, w13
	ldrb	w14, [x23, w14, uxtw]
	add	w15, w11, w13
	ldrb	w15, [x23, w15, uxtw]
	cmp	w14, w15
	b.ne	LBB0_48
; %bb.46:                               ;   in Loop: Header=BB0_45 Depth=5
	add	x13, x13, #1
	cmp	x28, x13
	b.ne	LBB0_45
; %bb.47:                               ;   in Loop: Header=BB0_44 Depth=4
	mov	x13, x19
LBB0_48:                                ;   in Loop: Header=BB0_44 Depth=4
	cmp	w13, #2
	ccmp	w13, w9, #0, hi
	csel	w9, w13, w9, hi
	csel	w8, w12, w8, hi
	cmp	x25, w12, uxtw
	b.ls	LBB0_50
; %bb.49:                               ;   in Loop: Header=BB0_44 Depth=4
	add	w12, w12, #1
	sub	x11, x11, #1
	cmp	w13, w19
	b.ne	LBB0_44
LBB0_50:                                ;   in Loop: Header=BB0_19 Depth=3
	orr	x8, x9, x8, lsl #32
	lsr	x9, x8, #32
	cmp	w20, w8
	ccmp	x22, x9, #0, eq
	b.eq	LBB0_52
	b	LBB0_125
LBB0_51:                                ;   in Loop: Header=BB0_19 Depth=3
	mov	x8, #0                          ; =0x0
	lsr	x9, x8, #32
	cmp	w20, w8
	ccmp	x22, x9, #0, eq
	b.ne	LBB0_125
LBB0_52:                                ;   in Loop: Header=BB0_19 Depth=3
	add	x25, x25, #1
	sub	x24, x24, #1
	cmp	x25, x21
	ldr	x20, [sp, #96]                  ; 8-byte Reload
	b.ne	LBB0_19
	b	LBB0_16
LBB0_53:
Ltmp6:                                  ; EH_LABEL
	mov	w0, #259                        ; =0x103
	movk	w0, #1, lsl #16
	bl	__Znwm
Ltmp7:                                  ; EH_LABEL
; %bb.54:
	mov	x24, x0
	mov	w19, #259                       ; =0x103
	movk	w19, #1, lsl #16
	add	x0, x0, #513
	mov	w1, #65282                      ; =0xff02
	bl	_bzero
	mov	x0, x24
	mov	x1, x23
	mov	w2, #513                        ; =0x201
	bl	_memcpy
	mov	x0, x23
	mov	w1, #513                        ; =0x201
	bl	__ZdlPvm
	mov	w8, #111                        ; =0x6f
	strb	w8, [x24]
	sub	x9, x19, #1
	mov	w10, #1                         ; =0x1
	ldr	w21, [sp, #68]                  ; 4-byte Reload
LBB0_55:                                ; =>This Inner Loop Header: Depth=1
	strb	w8, [x24, x10]
	add	x10, x10, #1
	subs	x9, x9, #1
	b.ne	LBB0_55
; %bb.56:
	mov	x0, x24
	mov	w1, #259                        ; =0x103
	movk	w1, #1, lsl #16
	mov	x2, x20
	mov	x3, x22
	bl	__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
Lloh5:
	adrp	x8, lCPI0_0@PAGE
Lloh6:
	ldr	d0, [x8, lCPI0_0@PAGEOFF]
	str	s0, [x24]
	mov	w8, #13591                      ; =0x3517
	strh	w8, [x24, #4]
	mov	w8, #95                         ; =0x5f
	strb	w8, [x24, #6]
	sub	x8, x19, #7
	mov	x9, x24
LBB0_57:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x9]
	strb	w10, [x9, #7]
	add	x9, x9, #1
	subs	x8, x8, #1
	b.ne	LBB0_57
; %bb.58:
	mov	x0, x24
	mov	w1, #259                        ; =0x103
	movk	w1, #1, lsl #16
	mov	x2, x20
	mov	x3, x22
	bl	__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
	mov	x8, #0                          ; =0x0
	mov	x9, #-32767                     ; =0xffffffffffff8001
	add	x9, x24, x9
	mov	w10, #32767                     ; =0x7fff
	mov	w11, #3493                      ; =0xda5
	movk	w11, #37345, lsl #16
	b	LBB0_60
LBB0_59:                                ;   in Loop: Header=BB0_60 Depth=1
	eor	w11, w11, w11, lsl #13
	eor	w11, w11, w11, lsr #17
	eor	w12, w11, w11, lsl #5
	mov	x11, x12
	strb	w12, [x24, x8]
	add	x8, x8, #1
	cmp	x19, x8
	b.eq	LBB0_62
LBB0_60:                                ; =>This Inner Loop Header: Depth=1
	cmp	x8, x10
	b.lo	LBB0_59
; %bb.61:                               ;   in Loop: Header=BB0_60 Depth=1
	ldrb	w12, [x9, x8]
	strb	w12, [x24, x8]
	add	x8, x8, #1
	cmp	x19, x8
	b.ne	LBB0_60
LBB0_62:
	mov	x0, x24
	mov	w1, #259                        ; =0x103
	movk	w1, #1, lsl #16
	mov	x2, x20
	mov	x3, x22
	bl	__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
	mov	x8, #0                          ; =0x0
	mov	x9, #-32768                     ; =0xffffffffffff8000
	mov	w11, #3493                      ; =0xda5
	movk	w11, #37345, lsl #16
	b	LBB0_64
LBB0_63:                                ;   in Loop: Header=BB0_64 Depth=1
	eor	w10, w11, w11, lsl #13
	eor	w10, w10, w10, lsr #17
	eor	w10, w10, w10, lsl #5
	mov	x11, x10
	strb	w10, [x24, x8]
	add	x8, x8, #1
	cmp	x19, x8
	b.eq	LBB0_66
LBB0_64:                                ; =>This Inner Loop Header: Depth=1
	cmp	x8, #8, lsl #12                 ; =32768
	b.lo	LBB0_63
; %bb.65:                               ;   in Loop: Header=BB0_64 Depth=1
	add	x10, x24, x8
	ldrb	w10, [x10, x9]
	strb	w10, [x24, x8]
	add	x8, x8, #1
	cmp	x19, x8
	b.ne	LBB0_64
LBB0_66:
	mov	x0, x24
	mov	w1, #259                        ; =0x103
	movk	w1, #1, lsl #16
	mov	x2, x20
	mov	x3, x22
	bl	__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
Lloh7:
	adrp	x8, ___stderrp@GOTPAGE
Lloh8:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh9:
	ldr	x3, [x8]
Lloh10:
	adrp	x0, l_.str.11@PAGE
Lloh11:
	add	x0, x0, l_.str.11@PAGEOFF
	mov	w1, #42                         ; =0x2a
	mov	w2, #1                          ; =0x1
	bl	_fwrite
	mov	x0, x24
	mov	w1, #259                        ; =0x103
	movk	w1, #1, lsl #16
	bl	__ZdlPvm
	cmp	w21, #2
	b.ne	LBB0_68
; %bb.67:
	ldr	x8, [sp, #56]                   ; 8-byte Reload
	ldr	x0, [x8, #8]
Lloh12:
	adrp	x1, l_.str.1@PAGE
Lloh13:
	add	x1, x1, l_.str.1@PAGEOFF
	bl	_strcmp
	cbz	w0, LBB0_124
LBB0_68:
Lloh14:
	adrp	x0, l_.str.2@PAGE
Lloh15:
	add	x0, x0, l_.str.2@PAGEOFF
	bl	_puts
Ltmp9:                                  ; EH_LABEL
	mov	w0, #131072                     ; =0x20000
	bl	__Znwm
Ltmp10:                                 ; EH_LABEL
; %bb.69:
	mov	x22, x0
	mov	w1, #131072                     ; =0x20000
	bl	_bzero
	cmp	w21, #2
	b.lt	LBB0_78
; %bb.70:
	mov	w19, #0                         ; =0x0
	mov	w21, #0                         ; =0x0
	str	wzr, [sp, #80]                  ; 4-byte Spill
	mov	w28, #1                         ; =0x1
Lloh16:
	adrp	x25, l_.str.7@PAGE
Lloh17:
	add	x25, x25, l_.str.7@PAGEOFF
Lloh18:
	adrp	x26, l_.str.8@PAGE
Lloh19:
	add	x26, x26, l_.str.8@PAGEOFF
Lloh20:
	adrp	x27, l_.str.9@PAGE
Lloh21:
	add	x27, x27, l_.str.9@PAGEOFF
	b	LBB0_73
LBB0_71:                                ;   in Loop: Header=BB0_73 Depth=1
	mov	w19, #1                         ; =0x1
LBB0_72:                                ;   in Loop: Header=BB0_73 Depth=1
	add	x28, x28, #1
	ldr	x8, [sp, #48]                   ; 8-byte Reload
	cmp	x8, x28
	b.eq	LBB0_79
LBB0_73:                                ; =>This Inner Loop Header: Depth=1
	ldr	x8, [sp, #56]                   ; 8-byte Reload
	ldr	x23, [x8, x28, lsl #3]
	mov	x0, x23
	mov	x1, x25
	bl	_strcmp
	cbz	w0, LBB0_71
; %bb.74:                               ;   in Loop: Header=BB0_73 Depth=1
	mov	x0, x23
	mov	x1, x26
	bl	_strcmp
	cbz	w0, LBB0_77
; %bb.75:                               ;   in Loop: Header=BB0_73 Depth=1
	mov	x0, x23
	mov	x1, x27
	bl	_strcmp
	cbnz	w0, LBB0_99
; %bb.76:                               ;   in Loop: Header=BB0_73 Depth=1
	mov	w21, #1                         ; =0x1
	b	LBB0_72
LBB0_77:                                ;   in Loop: Header=BB0_73 Depth=1
	mov	w8, #1                          ; =0x1
	str	w8, [sp, #80]                   ; 4-byte Spill
	b	LBB0_72
LBB0_78:
	add	x8, x22, #32, lsl #12           ; =131072
	str	x8, [sp, #72]                   ; 8-byte Spill
	mov	w21, #0                         ; =0x0
	str	wzr, [sp, #80]                  ; 4-byte Spill
	mov	w28, #1                         ; =0x1
	b	LBB0_100
LBB0_79:
	add	x8, x22, #32, lsl #12           ; =131072
	str	x8, [sp, #72]                   ; 8-byte Spill
	ldr	x8, [sp, #48]                   ; 8-byte Reload
	mov	x28, x8
	tbz	w19, #0, LBB0_100
LBB0_80:
	ldr	w8, [sp, #68]                   ; 4-byte Reload
	cmp	w28, w8
	b.ge	LBB0_122
LBB0_81:
	ldr	x8, [sp, #56]                   ; 8-byte Reload
	add	x19, x8, w28, uxtw #3
	add	x8, x22, #32, lsl #12           ; =131072
	str	x8, [sp, #72]                   ; 8-byte Spill
	add	x24, x22, #32, lsl #12          ; =131072
LBB0_82:                                ; =>This Inner Loop Header: Depth=1
	ldr	x0, [x19]
Ltmp17:                                 ; EH_LABEL
Lloh22:
	adrp	x1, l_.str.10@PAGE
Lloh23:
	add	x1, x1, l_.str.10@PAGEOFF
	bl	_fopen
Ltmp18:                                 ; EH_LABEL
; %bb.83:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x25, x0
	cbz	x0, LBB0_125
; %bb.84:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x0, x25
	mov	x1, #0                          ; =0x0
	mov	w2, #2                          ; =0x2
	bl	_fseek
	cbnz	w0, LBB0_125
; %bb.85:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x0, x25
	bl	_ftell
	mov	w8, #16777216                   ; =0x1000000
	cmp	x0, x8
	b.hi	LBB0_125
; %bb.86:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x26, x0
	str	x28, [sp, #88]                  ; 8-byte Spill
	mov	x0, x25
	mov	x1, #0                          ; =0x0
	mov	w2, #0                          ; =0x0
	bl	_fseek
	cbnz	w0, LBB0_125
; %bb.87:                               ;   in Loop: Header=BB0_82 Depth=1
	sub	x28, x24, x22
	subs	x21, x26, x28
	b.ls	LBB0_91
; %bb.88:                               ;   in Loop: Header=BB0_82 Depth=1
	ldr	x8, [sp, #72]                   ; 8-byte Reload
	sub	x27, x8, x22
	cmp	x26, x27
	b.ls	LBB0_92
; %bb.89:                               ;   in Loop: Header=BB0_82 Depth=1
	lsl	x8, x27, #1
	cmp	x8, x26
	csel	x24, x8, x26, hi
Ltmp20:                                 ; EH_LABEL
	mov	x0, x24
	bl	__Znwm
Ltmp21:                                 ; EH_LABEL
; %bb.90:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x23, x0
	add	x0, x0, x28
	mov	x1, x21
	bl	_bzero
	mov	x0, x23
	mov	x1, x22
	mov	x2, x28
	bl	_memcpy
	add	x8, x23, x26
	add	x9, x23, x24
	str	x9, [sp, #72]                   ; 8-byte Spill
	mov	x24, x8
	mov	x0, x22
	mov	x1, x27
	bl	__ZdlPvm
	mov	x22, x23
	b	LBB0_93
LBB0_91:                                ;   in Loop: Header=BB0_82 Depth=1
	add	x8, x22, x26
	csel	x24, x8, x24, lo
	b	LBB0_93
LBB0_92:                                ;   in Loop: Header=BB0_82 Depth=1
	add	x20, x24, x21
	mov	x0, x24
	mov	x1, x21
	bl	_bzero
	mov	x24, x20
	ldr	x20, [sp, #96]                  ; 8-byte Reload
LBB0_93:                                ;   in Loop: Header=BB0_82 Depth=1
	sub	x26, x24, x22
	sub	x2, x24, x22
	mov	x0, x22
	mov	w1, #1                          ; =0x1
	mov	x3, x25
	bl	_fread
	cmp	x0, x26
	b.ne	LBB0_125
; %bb.94:                               ;   in Loop: Header=BB0_82 Depth=1
	mov	x0, x25
	bl	_fclose
	cbnz	w0, LBB0_125
; %bb.95:                               ;   in Loop: Header=BB0_82 Depth=1
	ldr	w8, [sp, #80]                   ; 4-byte Reload
	tbnz	w8, #0, LBB0_97
; %bb.96:                               ;   in Loop: Header=BB0_82 Depth=1
	ldr	x0, [x19]
Ltmp22:                                 ; EH_LABEL
	sub	x2, x24, x22
	mov	x1, x22
	mov	w3, #1                          ; =0x1
	mov	x4, x20
	ldur	x5, [x29, #-88]                 ; 8-byte Folded Reload
	bl	__ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
Ltmp23:                                 ; EH_LABEL
LBB0_97:                                ;   in Loop: Header=BB0_82 Depth=1
	ldr	x0, [x19], #8
Ltmp24:                                 ; EH_LABEL
	mov	x1, x22
	mov	x2, x26
	mov	w3, #0                          ; =0x0
	mov	x4, x20
	ldur	x5, [x29, #-88]                 ; 8-byte Folded Reload
	bl	__ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
Ltmp25:                                 ; EH_LABEL
; %bb.98:                               ;   in Loop: Header=BB0_82 Depth=1
	ldr	x28, [sp, #88]                  ; 8-byte Reload
	add	w28, w28, #1
	ldr	w8, [sp, #68]                   ; 4-byte Reload
	cmp	w8, w28
	b.gt	LBB0_82
	b	LBB0_123
LBB0_99:
	add	x8, x22, #32, lsl #12           ; =131072
	str	x8, [sp, #72]                   ; 8-byte Spill
	tbnz	w19, #0, LBB0_80
LBB0_100:
	str	x28, [sp, #88]                  ; 8-byte Spill
	mov	x19, #0                         ; =0x0
	mov	w24, #58705                     ; =0xe551
	movk	w24, #64776, lsl #16
	mov	w25, #259                       ; =0x103
	mov	w26, #65                        ; =0x41
	mov	w27, #43691                     ; =0xaaab
	movk	w27, #43690, lsl #16
Lloh24:
	adrp	x28, l___const.main.names@PAGE
Lloh25:
	add	x28, x28, l___const.main.names@PAGEOFF
	b	LBB0_103
LBB0_101:                               ;   in Loop: Header=BB0_103 Depth=1
Ltmp14:                                 ; EH_LABEL
	mov	x0, x23
	mov	x1, x22
	mov	w2, #131072                     ; =0x20000
	mov	w3, #0                          ; =0x0
	mov	x4, x20
	ldur	x5, [x29, #-88]                 ; 8-byte Folded Reload
	bl	__ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
Ltmp15:                                 ; EH_LABEL
LBB0_102:                               ;   in Loop: Header=BB0_103 Depth=1
	add	x19, x19, #1
	cmp	x19, #4
	b.eq	LBB0_121
LBB0_103:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_117 Depth 2
                                        ;     Child Loop BB0_108 Depth 2
                                        ;     Child Loop BB0_111 Depth 2
	cmp	x19, #0
	cset	w8, ne
	and	w8, w21, w8
	tbnz	w8, #0, LBB0_102
; %bb.104:                              ;   in Loop: Header=BB0_103 Depth=1
	cbz	x19, LBB0_110
; %bb.105:                              ;   in Loop: Header=BB0_103 Depth=1
	cmp	w19, #1
	b.eq	LBB0_109
; %bb.106:                              ;   in Loop: Header=BB0_103 Depth=1
	cmp	w19, #2
	b.ne	LBB0_114
; %bb.107:                              ;   in Loop: Header=BB0_103 Depth=1
	mov	w8, #0                          ; =0x0
	mov	x9, x22
	mov	w10, #131072                    ; =0x20000
	mov	w11, #3493                      ; =0xda5
	movk	w11, #37345, lsl #16
LBB0_108:                               ;   Parent Loop BB0_103 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	umull	x12, w8, w24
	lsr	x12, x12, #40
	msub	w12, w12, w25, w8
	eor	w11, w11, w11, lsl #13
	eor	w11, w11, w11, lsr #17
	eor	w11, w11, w11, lsl #5
	cmp	w12, #255
	csel	w12, w26, w11, lo
	strb	w12, [x9], #1
	add	w8, w8, #1
	subs	x10, x10, #1
	b.ne	LBB0_108
	b	LBB0_112
LBB0_109:                               ;   in Loop: Header=BB0_103 Depth=1
	mov	x0, x22
	mov	w1, #131072                     ; =0x20000
	bl	_bzero
	b	LBB0_112
LBB0_110:                               ;   in Loop: Header=BB0_103 Depth=1
	mov	x8, #0                          ; =0x0
	mov	w9, #3493                       ; =0xda5
	movk	w9, #37345, lsl #16
LBB0_111:                               ;   Parent Loop BB0_103 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	eor	w9, w9, w9, lsl #13
	eor	w9, w9, w9, lsr #17
	eor	w9, w9, w9, lsl #5
	strb	w9, [x22, x8]
	add	x8, x8, #1
	cmp	x8, #32, lsl #12                ; =131072
	b.ne	LBB0_111
LBB0_112:                               ;   in Loop: Header=BB0_103 Depth=1
	ldr	x23, [x28, x19, lsl #3]
	ldr	w8, [sp, #80]                   ; 4-byte Reload
	tbnz	w8, #0, LBB0_101
; %bb.113:                              ;   in Loop: Header=BB0_103 Depth=1
Ltmp12:                                 ; EH_LABEL
	mov	x0, x23
	mov	x1, x22
	mov	w2, #131072                     ; =0x20000
	mov	w3, #1                          ; =0x1
	mov	x4, x20
	ldur	x5, [x29, #-88]                 ; 8-byte Folded Reload
	bl	__ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
Ltmp13:                                 ; EH_LABEL
	b	LBB0_101
LBB0_114:                               ;   in Loop: Header=BB0_103 Depth=1
	mov	w8, #0                          ; =0x0
	mov	x9, x22
	mov	w10, #131072                    ; =0x20000
	mov	w11, #3493                      ; =0xda5
	movk	w11, #37345, lsl #16
	b	LBB0_117
LBB0_115:                               ;   in Loop: Header=BB0_117 Depth=2
	ldurb	w12, [x9, #-1]
	and	w13, w13, #0x3
	orr	w12, w13, w12, lsl #6
LBB0_116:                               ;   in Loop: Header=BB0_117 Depth=2
	strb	w12, [x9], #1
	add	w8, w8, #1
	subs	x10, x10, #1
	b.eq	LBB0_112
LBB0_117:                               ;   Parent Loop BB0_103 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	umull	x12, w8, w27
	lsr	x12, x12, #33
	add	w12, w12, w12, lsl #1
	sub	w12, w8, w12
	eor	w11, w11, w11, lsl #13
	eor	w13, w11, w11, lsr #17
	eor	w11, w13, w13, lsl #5
	cmp	w12, #1
	b.eq	LBB0_115
; %bb.118:                              ;   in Loop: Header=BB0_117 Depth=2
	cbnz	w12, LBB0_120
; %bb.119:                              ;   in Loop: Header=BB0_117 Depth=2
	and	w12, w11, #0xfffffff3
	b	LBB0_116
LBB0_120:                               ;   in Loop: Header=BB0_117 Depth=2
	ldurb	w12, [x9, #-1]
	lsl	w12, w12, #6
	b	LBB0_116
LBB0_121:
	ldr	x28, [sp, #88]                  ; 8-byte Reload
	ldr	w8, [sp, #68]                   ; 4-byte Reload
	cmp	w28, w8
	b.lt	LBB0_81
LBB0_122:
	add	x8, x22, #32, lsl #12           ; =131072
	str	x8, [sp, #72]                   ; 8-byte Spill
LBB0_123:
	ldr	x8, [sp, #72]                   ; 8-byte Reload
	sub	x1, x8, x22
	mov	x0, x22
	bl	__ZdlPvm
	ldur	x22, [x29, #-88]                ; 8-byte Folded Reload
LBB0_124:
	mov	w8, #12                         ; =0xc
	movk	w8, #6, lsl #16
	add	x1, x8, #80, lsl #12            ; =327680
	mov	x0, x22
	bl	__ZdlPvm
	mov	x0, x20
	mov	w1, #12                         ; =0xc
	movk	w1, #6, lsl #16
	bl	__ZdlPvm
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #192]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #176]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #160]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #144]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #128]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #112]            ; 16-byte Folded Reload
	add	sp, sp, #208
	ret
LBB0_125:
	bl	_abort
LBB0_126:
Lloh26:
	adrp	x8, ___stderrp@GOTPAGE
Lloh27:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh28:
	ldr	x8, [x8]
                                        ; kill: def $w20 killed $w20 killed $x20 def $x20
	stp	x20, x22, [sp, #32]
                                        ; kill: def $w0 killed $w0 killed $x0 def $x0
	stp	x0, x9, [sp, #16]
                                        ; kill: def $w25 killed $w25 killed $x25 def $x25
	stp	x21, x25, [sp]
Lloh29:
	adrp	x1, l_.str.12@PAGE
Lloh30:
	add	x1, x1, l_.str.12@PAGEOFF
	mov	x0, x8
	bl	_fprintf
	bl	_abort
LBB0_127:
Ltmp11:                                 ; EH_LABEL
	mov	x21, x0
	b	LBB0_136
LBB0_128:
Ltmp8:                                  ; EH_LABEL
	mov	x21, x0
	mov	x0, x23
	mov	w1, #513                        ; =0x201
	b	LBB0_135
LBB0_129:
Ltmp5:                                  ; EH_LABEL
	mov	x21, x0
	b	LBB0_136
LBB0_130:
Ltmp2:                                  ; EH_LABEL
	mov	x21, x0
	ldr	x0, [sp, #96]                   ; 8-byte Reload
	mov	w1, #12                         ; =0xc
	movk	w1, #6, lsl #16
	bl	__ZdlPvm
	mov	x0, x21
	bl	__Unwind_Resume
LBB0_131:
Ltmp19:                                 ; EH_LABEL
	b	LBB0_134
LBB0_132:
Ltmp16:                                 ; EH_LABEL
	b	LBB0_134
LBB0_133:
Ltmp26:                                 ; EH_LABEL
LBB0_134:
	mov	x21, x0
	ldr	x8, [sp, #72]                   ; 8-byte Reload
	sub	x1, x8, x22
	mov	x0, x22
LBB0_135:
	bl	__ZdlPvm
LBB0_136:
	mov	w8, #12                         ; =0xc
	movk	w8, #6, lsl #16
	add	x1, x8, #80, lsl #12            ; =327680
	ldur	x0, [x29, #-88]                 ; 8-byte Folded Reload
	bl	__ZdlPvm
	ldr	x0, [sp, #96]                   ; 8-byte Reload
	mov	w1, #12                         ; =0xc
	movk	w1, #6, lsl #16
	bl	__ZdlPvm
	mov	x0, x21
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh3, Lloh4
	.loh AdrpLdrGotLdr	Lloh0, Lloh1, Lloh2
	.loh AdrpLdr	Lloh5, Lloh6
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpLdrGotLdr	Lloh7, Lloh8, Lloh9
	.loh AdrpAdd	Lloh12, Lloh13
	.loh AdrpAdd	Lloh14, Lloh15
	.loh AdrpAdd	Lloh20, Lloh21
	.loh AdrpAdd	Lloh18, Lloh19
	.loh AdrpAdd	Lloh16, Lloh17
	.loh AdrpAdd	Lloh22, Lloh23
	.loh AdrpAdd	Lloh24, Lloh25
	.loh AdrpAdd	Lloh29, Lloh30
	.loh AdrpLdrGotLdr	Lloh26, Lloh27, Lloh28
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table0:
Lexception0:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ; >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ;   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ; >> Call Site 2 <<
	.uleb128 Ltmp1-Ltmp0                    ;   Call between Ltmp0 and Ltmp1
	.uleb128 Ltmp2-Lfunc_begin0             ;     jumps to Ltmp2
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp1-Lfunc_begin0             ; >> Call Site 3 <<
	.uleb128 Ltmp3-Ltmp1                    ;   Call between Ltmp1 and Ltmp3
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ; >> Call Site 4 <<
	.uleb128 Ltmp4-Ltmp3                    ;   Call between Ltmp3 and Ltmp4
	.uleb128 Ltmp5-Lfunc_begin0             ;     jumps to Ltmp5
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp4-Lfunc_begin0             ; >> Call Site 5 <<
	.uleb128 Ltmp6-Ltmp4                    ;   Call between Ltmp4 and Ltmp6
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp6-Lfunc_begin0             ; >> Call Site 6 <<
	.uleb128 Ltmp7-Ltmp6                    ;   Call between Ltmp6 and Ltmp7
	.uleb128 Ltmp8-Lfunc_begin0             ;     jumps to Ltmp8
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp7-Lfunc_begin0             ; >> Call Site 7 <<
	.uleb128 Ltmp9-Ltmp7                    ;   Call between Ltmp7 and Ltmp9
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp9-Lfunc_begin0             ; >> Call Site 8 <<
	.uleb128 Ltmp10-Ltmp9                   ;   Call between Ltmp9 and Ltmp10
	.uleb128 Ltmp11-Lfunc_begin0            ;     jumps to Ltmp11
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp10-Lfunc_begin0            ; >> Call Site 9 <<
	.uleb128 Ltmp17-Ltmp10                  ;   Call between Ltmp10 and Ltmp17
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp17-Lfunc_begin0            ; >> Call Site 10 <<
	.uleb128 Ltmp18-Ltmp17                  ;   Call between Ltmp17 and Ltmp18
	.uleb128 Ltmp19-Lfunc_begin0            ;     jumps to Ltmp19
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp20-Lfunc_begin0            ; >> Call Site 11 <<
	.uleb128 Ltmp21-Ltmp20                  ;   Call between Ltmp20 and Ltmp21
	.uleb128 Ltmp26-Lfunc_begin0            ;     jumps to Ltmp26
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp21-Lfunc_begin0            ; >> Call Site 12 <<
	.uleb128 Ltmp22-Ltmp21                  ;   Call between Ltmp21 and Ltmp22
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp22-Lfunc_begin0            ; >> Call Site 13 <<
	.uleb128 Ltmp25-Ltmp22                  ;   Call between Ltmp22 and Ltmp25
	.uleb128 Ltmp26-Lfunc_begin0            ;     jumps to Ltmp26
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp14-Lfunc_begin0            ; >> Call Site 14 <<
	.uleb128 Ltmp15-Ltmp14                  ;   Call between Ltmp14 and Ltmp15
	.uleb128 Ltmp16-Lfunc_begin0            ;     jumps to Ltmp16
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp15-Lfunc_begin0            ; >> Call Site 15 <<
	.uleb128 Ltmp12-Ltmp15                  ;   Call between Ltmp15 and Ltmp12
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp12-Lfunc_begin0            ; >> Call Site 16 <<
	.uleb128 Ltmp13-Ltmp12                  ;   Call between Ltmp12 and Ltmp13
	.uleb128 Ltmp16-Lfunc_begin0            ;     jumps to Ltmp16
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp13-Lfunc_begin0            ; >> Call Site 17 <<
	.uleb128 Lfunc_end0-Ltmp13              ;   Call between Ltmp13 and Lfunc_end0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
__ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE: ; @_ZN12_GLOBAL__N_110experimentEPKcNSt3__14spanIKhLm18446744073709551615EEEbRNS_5ChainERNS_5RadixE
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
; %bb.0:
	stp	d9, d8, [sp, #-112]!            ; 16-byte Folded Spill
	stp	x28, x27, [sp, #16]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	sub	sp, sp, #640
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x22, x5
	str	x4, [sp, #344]                  ; 8-byte Spill
	str	w3, [sp, #308]                  ; 4-byte Spill
	mov	x20, x2
	mov	x24, x1
	str	x0, [sp, #168]                  ; 8-byte Spill
	cbz	x2, LBB1_5
; %bb.1:
	tbnz	x20, #63, LBB1_241
; %bb.2:
	mov	x0, x20
	bl	__Znwm
	mov	x19, x0
	mov	x1, x20
	bl	_bzero
	lsr	x8, x20, #61
	cbnz	x8, LBB1_242
; %bb.3:
	lsl	x21, x20, #3
Ltmp27:                                 ; EH_LABEL
	mov	x0, x21
	bl	__Znwm
Ltmp28:                                 ; EH_LABEL
; %bb.4:
	add	x8, x19, x20
	str	x8, [sp, #160]                  ; 8-byte Spill
	add	x8, x0, x20, lsl #3
	str	x8, [sp, #144]                  ; 8-byte Spill
	str	x0, [sp, #376]                  ; 8-byte Spill
	mov	x1, x21
	bl	_bzero
	b	LBB1_6
LBB1_5:
	mov	x19, #0                         ; =0x0
	str	xzr, [sp, #160]                 ; 8-byte Spill
	str	xzr, [sp, #144]                 ; 8-byte Spill
	str	xzr, [sp, #376]                 ; 8-byte Spill
LBB1_6:
	str	x22, [sp, #352]                 ; 8-byte Spill
	ldr	x0, [sp, #344]                  ; 8-byte Reload
	add	x21, x0, #96, lsl #12           ; =393216
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	str	xzr, [x21]
	str	x21, [sp, #360]                 ; 8-byte Spill
	str	wzr, [x21, #8]
	cbz	x20, LBB1_26
; %bb.7:
	mov	x26, #0                         ; =0x0
	mov	w22, #0                         ; =0x0
	str	xzr, [sp, #296]                 ; 8-byte Spill
	mov	w21, #0                         ; =0x0
	mov	w23, #-1                        ; =0xffffffff
	mov	w28, #258                       ; =0x102
	mov	w25, #32768                     ; =0x8000
	b	LBB1_9
LBB1_8:                                 ;   in Loop: Header=BB1_9 Depth=1
	orr	x8, x11, x10, lsl #32
	eor	x9, x8, x0
	cmp	w0, w8
	lsr	x8, x9, #32
	ccmp	x8, #0, #0, eq
	b.eq	LBB1_25
	b	LBB1_230
LBB1_9:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_18 Depth 2
                                        ;       Child Loop BB1_19 Depth 3
	cmp	w21, w22
	ldr	w8, [sp, #308]                  ; 4-byte Reload
	csinc	w27, w8, wzr, ne
	strb	w27, [x19, x26]
	ldr	x0, [sp, #344]                  ; 8-byte Reload
	mov	x1, x24
	mov	x2, x20
	mov	x3, x21
	mov	x4, x27
	bl	__ZN12_GLOBAL__N_15Chain7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	ldr	x8, [sp, #376]                  ; 8-byte Reload
	str	x0, [x8, x26, lsl #3]
	cmp	w27, #1
	b.ne	LBB1_25
; %bb.10:                               ;   in Loop: Header=BB1_9 Depth=1
	ldr	x9, [sp, #296]                  ; 8-byte Reload
	add	w9, w9, #1
	cmp	w0, #1
	csinc	w8, w0, wzr, hi
	add	w22, w8, w21
	cmp	w21, #1024
	str	x9, [sp, #296]                  ; 8-byte Spill
	b.lo	LBB1_16
; %bb.11:                               ;   in Loop: Header=BB1_9 Depth=1
	mov	w8, #1153                       ; =0x481
	movk	w8, #32792, lsl #16
	umull	x8, w21, w8
	lsr	x8, x8, #43
	mov	w10, #4093                      ; =0xffd
	msub	w8, w8, w10, w21
	cbz	w8, LBB1_16
; %bb.12:                               ;   in Loop: Header=BB1_9 Depth=1
	add	w8, w21, #258
	cmp	x20, x8
	b.ls	LBB1_15
; %bb.13:                               ;   in Loop: Header=BB1_9 Depth=1
	cmp	w21, #16, lsl #12               ; =65536
	b.eq	LBB1_15
; %bb.14:                               ;   in Loop: Header=BB1_9 Depth=1
	cmp	w21, #8, lsl #12                ; =32768
	b.ne	LBB1_25
LBB1_15:                                ;   in Loop: Header=BB1_9 Depth=1
	sub	x8, x20, x26
	cmp	x8, #258
	csel	x8, x8, x28, lo
	cmp	w21, #8, lsl #12                ; =32768
	csel	w9, w21, w25, lo
	b	LBB1_17
LBB1_16:                                ;   in Loop: Header=BB1_9 Depth=1
	sub	x8, x20, x26
	cmp	x8, #258
	csel	x8, x8, x28, lo
	cmp	w21, #8, lsl #12                ; =32768
	csel	w9, w21, w25, lo
	cbz	w21, LBB1_24
LBB1_17:                                ;   in Loop: Header=BB1_9 Depth=1
	mov	w10, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w12, #1                         ; =0x1
	mov	x13, x23
LBB1_18:                                ;   Parent Loop BB1_9 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB1_19 Depth 3
	mov	w14, #0                         ; =0x0
	mov	x15, x8
LBB1_19:                                ;   Parent Loop BB1_9 Depth=1
                                        ;     Parent Loop BB1_18 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	w16, w21, w14
	ldrb	w16, [x24, w16, uxtw]
	add	w17, w13, w14
	ldrb	w17, [x24, w17, uxtw]
	cmp	w16, w17
	b.ne	LBB1_22
; %bb.20:                               ;   in Loop: Header=BB1_19 Depth=3
	add	w14, w14, #1
	subs	x15, x15, #1
	b.ne	LBB1_19
; %bb.21:                               ;   in Loop: Header=BB1_18 Depth=2
	mov	x14, x8
LBB1_22:                                ;   in Loop: Header=BB1_18 Depth=2
	cmp	w14, #2
	ccmp	w14, w11, #0, hi
	csel	w11, w14, w11, hi
	csel	w10, w12, w10, hi
	cmp	w12, w9
	b.hs	LBB1_8
; %bb.23:                               ;   in Loop: Header=BB1_18 Depth=2
	add	w12, w12, #1
	sub	w13, w13, #1
	cmp	w14, w8
	b.ne	LBB1_18
	b	LBB1_8
LBB1_24:                                ;   in Loop: Header=BB1_9 Depth=1
	mov	x8, #0                          ; =0x0
	eor	x9, x8, x0
	cmp	w0, w8
	lsr	x8, x9, #32
	ccmp	x8, #0, #0, eq
	b.ne	LBB1_230
LBB1_25:                                ;   in Loop: Header=BB1_9 Depth=1
	add	w26, w21, #1
	add	w23, w23, #1
	mov	x21, x26
	cmp	x20, x26
	b.hi	LBB1_9
	b	LBB1_27
LBB1_26:
	str	xzr, [sp, #296]                 ; 8-byte Spill
LBB1_27:
	ldr	x21, [sp, #344]                 ; 8-byte Reload
	mov	x0, x21
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	ldr	x25, [sp, #360]                 ; 8-byte Reload
	str	xzr, [x25]
	str	wzr, [x25, #8]
	str	x19, [sp, #336]                 ; 8-byte Spill
	cbz	x20, LBB1_55
; %bb.28:
	mov	x28, #0                         ; =0x0
	str	xzr, [sp, #288]                 ; 8-byte Spill
	str	xzr, [sp, #232]                 ; 8-byte Spill
	mov	x8, #0                          ; =0x0
	mov	x22, #0                         ; =0x0
	mov	w27, #0                         ; =0x0
	add	x10, x21, #64, lsl #12          ; =262144
	str	x10, [sp, #368]                 ; 8-byte Spill
	mov	x21, #0                         ; =0x0
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	b	LBB1_30
LBB1_29:                                ;   in Loop: Header=BB1_30 Depth=1
	mov	w8, w21
	add	x8, x8, x8, lsl #15
	add	x9, x19, x22
	add	x22, x9, x8
	add	w8, w27, #1
	mov	x27, x8
	cmp	x20, x8
	ldr	x19, [sp, #336]                 ; 8-byte Reload
	mov	x21, x23
	b.ls	LBB1_56
LBB1_30:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_43 Depth 2
                                        ;       Child Loop BB1_47 Depth 3
                                        ;       Child Loop BB1_52 Depth 3
	sub	x9, x20, w27, uxtw
	cmp	x9, #258
	mov	w10, #258                       ; =0x102
	csel	x11, x9, x10, lo
	sub	x9, x20, x8
	cmp	x9, #258
	csel	x12, x9, x10, lo
	cmp	x9, #3
	b.hs	LBB1_32
; %bb.31:                               ;   in Loop: Header=BB1_30 Depth=1
	mov	x23, x21
	mov	x21, #0                         ; =0x0
	lsr	x19, x21, #32
	add	x26, x30, x8, lsl #3
	ldp	w8, w9, [x26]
	cmp	w8, w21
	ccmp	w9, w19, #0, eq
	b.eq	LBB1_29
	b	LBB1_37
LBB1_32:                                ;   in Loop: Header=BB1_30 Depth=1
	ldrb	w14, [x19, x8]
	add	x13, x24, x8
	ldrb	w9, [x13]
	add	w10, w27, #1
	ldrb	w10, [x24, w10, uxtw]
	add	w15, w27, #2
	ldrb	w15, [x24, w15, uxtw]
	ubfiz	x9, x9, #12, #4
	eor	x9, x9, x10, lsl #6
	eor	x9, x9, x15
	ldr	x19, [sp, #344]                 ; 8-byte Reload
	ldr	w10, [x19, x9, lsl #2]
	subs	w15, w27, #8, lsl #12           ; =32768
	csel	w16, wzr, w15, lo
	ldr	w15, [x25]
	ldr	w17, [x25, #8]
	sub	w0, w27, w15
	subs	w15, w17, w0
	csel	w17, wzr, w15, lo
	cbz	w14, LBB1_36
; %bb.33:                               ;   in Loop: Header=BB1_30 Depth=1
	mov	x26, x22
	cmn	w10, #1
	ccmp	w10, w16, #0, ne
	ldr	x22, [sp, #232]                 ; 8-byte Reload
	mov	w14, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	b.hs	LBB1_39
; %bb.34:                               ;   in Loop: Header=BB1_30 Depth=1
	ldr	x23, [sp, #368]                 ; 8-byte Reload
LBB1_35:                                ;   in Loop: Header=BB1_30 Depth=1
	str	x22, [sp, #232]                 ; 8-byte Spill
	mov	x12, x23
	mov	x23, x21
	and	w11, w27, #0x7fff
	str	w10, [x12, w11, uxtw #2]
	str	w27, [x19, x9, lsl #2]
	ldr	x9, [sp, #288]                  ; 8-byte Reload
	add	x9, x9, #2
	str	x9, [sp, #288]                  ; 8-byte Spill
	sub	w9, w27, w14
	stp	w27, w9, [x25]
	str	w15, [x25, #8]
	mov	w9, w15
	orr	x21, x9, x14, lsl #32
	mov	x22, x26
	lsr	x19, x21, #32
	add	x26, x30, x8, lsl #3
	ldp	w8, w9, [x26]
	cmp	w8, w21
	ccmp	w9, w19, #0, eq
	b.eq	LBB1_29
	b	LBB1_37
LBB1_36:                                ;   in Loop: Header=BB1_30 Depth=1
	mov	x23, x21
	mov	x21, #0                         ; =0x0
	and	w11, w27, #0x7fff
	ldr	x12, [sp, #368]                 ; 8-byte Reload
	str	w10, [x12, w11, uxtw #2]
	str	w27, [x19, x9, lsl #2]
	ldr	x9, [sp, #288]                  ; 8-byte Reload
	add	x9, x9, #2
	str	x9, [sp, #288]                  ; 8-byte Spill
	lsr	x19, x21, #32
	add	x26, x30, x8, lsl #3
	ldp	w8, w9, [x26]
	cmp	w8, w21
	ccmp	w9, w19, #0, eq
	b.eq	LBB1_29
LBB1_37:                                ;   in Loop: Header=BB1_30 Depth=1
Lloh31:
	adrp	x10, ___stderrp@GOTPAGE
Lloh32:
	ldr	x10, [x10, ___stderrp@GOTPAGEOFF]
Lloh33:
	ldr	x0, [x10]
	stp	x8, x9, [sp, #24]
	stp	x21, x19, [sp, #8]
	str	x27, [sp]
Lloh34:
	adrp	x1, l_.str.18@PAGE
Lloh35:
	add	x1, x1, l_.str.18@PAGEOFF
	bl	_fprintf
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	w8, [x26]
	cmp	w8, w21
	b.ne	LBB1_230
; %bb.38:                               ;   in Loop: Header=BB1_30 Depth=1
	ldr	w8, [x26, #4]
	cmp	w8, w19
	b.eq	LBB1_29
	b	LBB1_230
LBB1_39:                                ;   in Loop: Header=BB1_30 Depth=1
	ldr	w1, [x25, #4]
	add	w0, w1, w0
	mov	x1, x10
	ldr	x23, [sp, #368]                 ; 8-byte Reload
	b	LBB1_43
LBB1_40:                                ;   in Loop: Header=BB1_43 Depth=2
	eor	x4, x7, x6
	rbit	x4, x4
	clz	x4, x4
	add	w3, w3, w4, lsr #3
	add	x28, x28, #8
LBB1_41:                                ;   in Loop: Header=BB1_43 Depth=2
	add	x22, x22, x2
	sub	w2, w27, w1
	cmp	w3, #2
	ccmp	w3, w15, #0, hi
	csel	w15, w3, w15, hi
	csel	w14, w2, w14, hi
	cmp	w3, w12
	b.eq	LBB1_35
LBB1_42:                                ;   in Loop: Header=BB1_43 Depth=2
	and	w1, w1, #0x7fff
	ldr	w1, [x23, w1, uxtw #2]
	cmn	w1, #1
	ccmp	w1, w16, #0, ne
	b.lo	LBB1_35
LBB1_43:                                ;   Parent Loop BB1_30 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB1_47 Depth 3
                                        ;       Child Loop BB1_52 Depth 3
	add	x21, x21, #1
	cmp	w15, #3
	b.lo	LBB1_45
; %bb.44:                               ;   in Loop: Header=BB1_43 Depth=2
	add	x28, x28, #1
	add	w2, w15, w27
	ldrb	w2, [x24, w2, uxtw]
	add	w3, w1, w15
	ldrb	w3, [x24, w3, uxtw]
	cmp	w2, w3
	b.ne	LBB1_42
LBB1_45:                                ;   in Loop: Header=BB1_43 Depth=2
	cmp	w1, w0
	csel	w2, w17, wzr, eq
	sub	w3, w12, w2
	cmp	w3, #8
	b.lo	LBB1_50
; %bb.46:                               ;   in Loop: Header=BB1_43 Depth=2
	add	x4, x24, w1, uxtw
	sub	w5, w11, w2
	mov	x3, x2
LBB1_47:                                ;   Parent Loop BB1_30 Depth=1
                                        ;     Parent Loop BB1_43 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	x6, [x13, w3, uxtw]
	ldr	x7, [x4, w3, uxtw]
	cmp	x6, x7
	b.ne	LBB1_40
; %bb.48:                               ;   in Loop: Header=BB1_47 Depth=3
	add	w3, w3, #8
	sub	w5, w5, #8
	add	x28, x28, #8
	cmp	w5, #7
	b.hi	LBB1_47
; %bb.49:                               ;   in Loop: Header=BB1_43 Depth=2
	cmp	w3, w12
	b.hs	LBB1_41
	b	LBB1_51
LBB1_50:                                ;   in Loop: Header=BB1_43 Depth=2
	mov	x3, x2
	cmp	w3, w12
	b.hs	LBB1_41
LBB1_51:                                ;   in Loop: Header=BB1_43 Depth=2
	add	x4, x28, #1
LBB1_52:                                ;   Parent Loop BB1_30 Depth=1
                                        ;     Parent Loop BB1_43 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	mov	x28, x4
	add	w4, w27, w3
	ldrb	w4, [x24, w4, uxtw]
	add	w5, w1, w3
	ldrb	w5, [x24, w5, uxtw]
	cmp	w4, w5
	b.ne	LBB1_41
; %bb.53:                               ;   in Loop: Header=BB1_52 Depth=3
	add	w3, w3, #1
	add	x4, x28, #1
	cmp	w11, w3
	b.ne	LBB1_52
; %bb.54:                               ;   in Loop: Header=BB1_43 Depth=2
	mov	x3, x12
	b	LBB1_41
LBB1_55:
	mov	x21, #0                         ; =0x0
	mov	x28, #0                         ; =0x0
	str	xzr, [sp, #288]                 ; 8-byte Spill
	str	xzr, [sp, #232]                 ; 8-byte Spill
	mov	x22, #0                         ; =0x0
LBB1_56:
	str	x22, [sp, #280]                 ; 8-byte Spill
	str	x21, [sp, #152]                 ; 8-byte Spill
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	add	x8, x23, #175, lsl #12          ; =716800
	add	x19, x8, #4084
	mov	w8, #-1                         ; =0xffffffff
	str	x8, [x19]
	add	x8, x23, #95, lsl #12           ; =389120
	add	x0, x8, #4084
	str	x0, [sp, #264]                  ; 8-byte Spill
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	add	x8, x23, #143, lsl #12          ; =585728
	add	x0, x8, #4084
	str	x0, [sp, #224]                  ; 8-byte Spill
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	stp	xzr, xzr, [x19, #8]
	cbz	x20, LBB1_231
; %bb.57:
	mov	x7, x19
	mov	x27, #0                         ; =0x0
	mov	x22, #0                         ; =0x0
	str	xzr, [sp, #328]                 ; 8-byte Spill
	str	xzr, [sp, #272]                 ; 8-byte Spill
	stp	xzr, xzr, [sp, #200]            ; 16-byte Folded Spill
	stp	xzr, xzr, [sp, #184]            ; 16-byte Folded Spill
	str	xzr, [sp, #240]                 ; 8-byte Spill
	str	xzr, [sp, #176]                 ; 8-byte Spill
	mov	x8, #0                          ; =0x0
	mov	x4, #0                          ; =0x0
	mov	w16, #0                         ; =0x0
	add	x9, x23, #127, lsl #12          ; =520192
	add	x25, x9, #4084
	mov	w19, #258                       ; =0x102
	mov	w21, #12                        ; =0xc
	mov	w5, #7                          ; =0x7
	mov	w9, #-1                         ; =0xffffffff
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	x11, [sp, #280]                 ; 8-byte Reload
	str	x7, [sp, #312]                  ; 8-byte Spill
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	b	LBB1_59
LBB1_58:                                ;   in Loop: Header=BB1_59 Depth=1
	mov	w8, w21
	add	x8, x8, x8, lsl #15
	add	x9, x23, x4
	add	x4, x9, x8
	add	w8, w16, #1
	ldp	x23, x9, [sp, #352]             ; 16-byte Folded Reload
	add	w9, w9, #1
	mov	x16, x8
	cmp	x20, x8
	mov	w21, #12                        ; =0xc
	b.ls	LBB1_232
LBB1_59:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_102 Depth 2
                                        ;     Child Loop BB1_106 Depth 2
                                        ;     Child Loop BB1_116 Depth 2
                                        ;     Child Loop BB1_155 Depth 2
                                        ;     Child Loop BB1_97 Depth 2
                                        ;     Child Loop BB1_88 Depth 2
                                        ;     Child Loop BB1_92 Depth 2
                                        ;     Child Loop BB1_149 Depth 2
                                        ;     Child Loop BB1_166 Depth 2
                                        ;     Child Loop BB1_190 Depth 2
                                        ;     Child Loop BB1_216 Depth 2
                                        ;     Child Loop BB1_221 Depth 2
	mov	w12, w16
	sub	x10, x20, x12
	cmp	x10, #258
	csel	x10, x10, x19, lo
	mov	w9, w9
	stp	x9, x16, [sp, #360]             ; 16-byte Folded Spill
	sub	x13, x20, x8
	cmp	x13, #258
	csel	x9, x13, x19, lo
	cmp	x13, #3
	b.hs	LBB1_61
; %bb.60:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x21, #0                         ; =0x0
	lsr	x23, x21, #32
	add	x26, x30, x8, lsl #3
	ldp	w8, w9, [x26]
	cmp	w8, w21
	ccmp	w9, w23, #0, eq
	b.eq	LBB1_58
	b	LBB1_228
LBB1_61:                                ;   in Loop: Header=BB1_59 Depth=1
	and	w11, w16, #0x7fff
	subs	w13, w16, #8, lsl #12           ; =32768
	b.lo	LBB1_78
; %bb.62:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	w17, [x6, w11, uxtw #2]
	cmp	w17, w13
	b.ne	LBB1_78
; %bb.63:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w14, [x24, x17]
	mov	w13, #-32767                    ; =0xffff8001
	add	w13, w16, w13
	ldrb	w13, [x24, w13, uxtw]
	ldr	x15, [sp, #224]                 ; 8-byte Reload
	add	x14, x15, x14, lsl #9
	mov	x15, x16
	ldrh	w16, [x14, x13, lsl #1]
	strh	w16, [x7]
	and	w15, w15, #0x7fff
	ldr	w0, [x6, w15, uxtw #2]
	cmp	w0, w17
	b.ne	LBB1_66
; %bb.64:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrh	w16, [x25, x15, lsl #1]
	mov	w2, #65535                      ; =0xffff
	cmp	w16, w2
	b.ne	LBB1_67
; %bb.65:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w16, #65535                     ; =0xffff
	strh	w16, [x7]
	ldr	x17, [sp, #184]                 ; 8-byte Reload
	add	x17, x17, #1
	str	x17, [sp, #184]                 ; 8-byte Spill
	ldr	x17, [sp, #328]                 ; 8-byte Reload
	b	LBB1_76
LBB1_66:                                ;   in Loop: Header=BB1_59 Depth=1
	ldr	x17, [sp, #328]                 ; 8-byte Reload
	b	LBB1_77
LBB1_67:                                ;   in Loop: Header=BB1_59 Depth=1
	sub	w17, w16, #8, lsl #12           ; =32768
	umaddl	x17, w17, w21, x23
	ldrh	w0, [x17]
	cmp	w15, w0
	cset	w0, eq
	ldrh	w1, [x17, w0, uxtw #1]
	ldrh	w0, [x17, #4]
	cmp	x0, x2
	b.eq	LBB1_70
; %bb.68:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w2, w0, #8, lsl #12             ; =32768
	umaddl	x2, w2, w21, x23
	ldrh	w3, [x2]
	cmp	w3, w16
	cset	w3, ne
	strh	w1, [x2, w3, uxtw #1]
	mov	w2, #65535                      ; =0xffff
	cmp	w1, w2
	b.ne	LBB1_71
LBB1_69:                                ;   in Loop: Header=BB1_59 Depth=1
	mov	w0, #3                          ; =0x3
	b	LBB1_75
LBB1_70:                                ;   in Loop: Header=BB1_59 Depth=1
	strh	w1, [x7]
	mov	w2, #65535                      ; =0xffff
	cmp	w1, w2
	b.eq	LBB1_69
LBB1_71:                                ;   in Loop: Header=BB1_59 Depth=1
	tbnz	w1, #15, LBB1_73
; %bb.72:                               ;   in Loop: Header=BB1_59 Depth=1
	strh	w0, [x25, x1, lsl #1]
	b	LBB1_74
LBB1_73:                                ;   in Loop: Header=BB1_59 Depth=1
	and	x1, x1, #0x7fff
	umaddl	x1, w1, w21, x23
	strh	w0, [x1, #4]
LBB1_74:                                ;   in Loop: Header=BB1_59 Depth=1
	mov	w0, #4                          ; =0x4
LBB1_75:                                ;   in Loop: Header=BB1_59 Depth=1
	ldr	x1, [sp, #176]                  ; 8-byte Reload
	add	x1, x1, #1
	str	x1, [sp, #176]                  ; 8-byte Spill
	ldrh	w1, [x7, #2]
	strh	w1, [x17]
	strh	w16, [x7, #2]
	ldr	x17, [sp, #328]                 ; 8-byte Reload
	add	x17, x0, x17
	ldrh	w16, [x7]
LBB1_76:                                ;   in Loop: Header=BB1_59 Depth=1
	mov	w0, #-1                         ; =0xffffffff
	str	w0, [x6, x15, lsl #2]
	add	x17, x17, #1
LBB1_77:                                ;   in Loop: Header=BB1_59 Depth=1
	strh	w16, [x14, x13, lsl #1]
	add	x17, x17, #1
	str	x17, [sp, #328]                 ; 8-byte Spill
	ldr	x16, [sp, #368]                 ; 8-byte Reload
LBB1_78:                                ;   in Loop: Header=BB1_59 Depth=1
	ldr	x13, [sp, #336]                 ; 8-byte Reload
	ldrb	w13, [x13, x8]
	str	w13, [sp, #324]                 ; 4-byte Spill
	add	x14, x24, x8
	add	w13, w16, #1
	ldrb	w15, [x24, w13, uxtw]
	ldrb	w13, [x14]
	ldr	x16, [sp, #224]                 ; 8-byte Reload
	add	x0, x16, x13, lsl #9
	ldr	x16, [sp, #368]                 ; 8-byte Reload
	ldrh	w3, [x0, x15, lsl #1]
	mov	x17, x3
	strh	w3, [x7]
	stp	x0, x4, [sp, #248]              ; 16-byte Folded Spill
	cbz	w16, LBB1_82
; %bb.79:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	w13, [x7, #20]
	cmp	w13, #0
	cset	w0, ne
	sub	w5, w13, w0
	ldr	x0, [sp, #272]                  ; 8-byte Reload
	add	x0, x0, x5
	str	x0, [sp, #272]                  ; 8-byte Spill
	cmp	w13, #2
	b.hs	LBB1_99
; %bb.80:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w0, [x14]
	sub	w4, w16, #1
	ldrb	w1, [x24, w4, uxtw]
	cmp	w0, w1
	b.eq	LBB1_100
; %bb.81:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w2, #0                          ; =0x0
	mov	w5, #0                          ; =0x0
	str	wzr, [x7, #20]
	add	x22, x22, #1
	ldr	x13, [sp, #200]                 ; 8-byte Reload
	add	x13, x13, #1
	str	x13, [sp, #200]                 ; 8-byte Spill
	b	LBB1_127
LBB1_82:                                ;   in Loop: Header=BB1_59 Depth=1
	mov	w2, #0                          ; =0x0
	mov	w1, #0                          ; =0x0
	mov	w13, #65535                     ; =0xffff
	cmp	w3, w13
	b.eq	LBB1_159
LBB1_83:                                ;   in Loop: Header=BB1_59 Depth=1
	str	x15, [sp, #216]                 ; 8-byte Spill
	mov	w15, #65535                     ; =0xffff
	tbnz	w17, #15, LBB1_97
LBB1_84:                                ;   in Loop: Header=BB1_59 Depth=1
	and	x5, x17, #0xffff
	ldr	w6, [x6, x5, lsl #2]
	ldr	w13, [x7, #8]
	sub	w0, w16, #1
	ldp	w4, w3, [x7, #16]
	cmp	w6, w0
	mov	w0, #2                          ; =0x2
	csel	w3, w3, w0, eq
	sub	w13, w16, w13
	subs	w0, w4, w13
	b.ls	LBB1_86
; %bb.85:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	w4, [x7, #12]
	cmp	w3, w0
	csel	w0, w3, w0, hi
	add	w13, w4, w13
	cmp	w6, w13
	csel	w3, w3, w0, ne
LBB1_86:                                ;   in Loop: Header=BB1_59 Depth=1
	sub	w13, w9, w3
	cmp	w13, #8
	b.lo	LBB1_110
; %bb.87:                               ;   in Loop: Header=BB1_59 Depth=1
	add	x13, x24, x6
	sub	w0, w10, w3
	mov	x7, x3
LBB1_88:                                ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	x4, [x14, w7, uxtw]
	ldr	x21, [x13, w7, uxtw]
	cmp	x4, x21
	b.ne	LBB1_141
; %bb.89:                               ;   in Loop: Header=BB1_88 Depth=2
	add	w7, w7, #8
	sub	w0, w0, #8
	add	x22, x22, #8
	cmp	w0, #7
	b.hi	LBB1_88
; %bb.90:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, w9
	b.hs	LBB1_111
LBB1_91:                                ;   in Loop: Header=BB1_59 Depth=1
	add	x13, x22, #1
	mov	w21, #12                        ; =0xc
LBB1_92:                                ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x22, x13
	add	w13, w16, w7
	ldrb	w13, [x24, w13, uxtw]
	add	w0, w6, w7
	ldrb	w0, [x24, w0, uxtw]
	cmp	w13, w0
	b.ne	LBB1_112
; %bb.93:                               ;   in Loop: Header=BB1_92 Depth=2
	add	w7, w7, #1
	add	x13, x22, #1
	cmp	w10, w7
	b.ne	LBB1_92
; %bb.94:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x7, x9
	add	x4, x27, #1
	ldr	x13, [sp, #312]                 ; 8-byte Reload
	stp	w16, w6, [x13, #8]
	str	w7, [x13, #16]
	ldr	w13, [sp, #324]                 ; 4-byte Reload
	cbnz	w13, LBB1_142
	b	LBB1_163
LBB1_95:                                ;   in Loop: Header=BB1_97 Depth=2
	ldrb	w0, [x24, x0]
LBB1_96:                                ;   in Loop: Header=BB1_97 Depth=2
	bic	w17, w5, w17
	lsr	w17, w0, w17
	and	w17, w17, #0x1
	ldrsh	w17, [x13, w17, uxtw #1]
	add	x27, x27, #1
	add	x22, x22, #1
	tbz	w17, #31, LBB1_84
LBB1_97:                                ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	and	x13, x17, #0x7fff
	umaddl	x13, w13, w21, x23
	ldrh	w17, [x13, #6]
	add	x0, x8, x17, lsr #3
	cmp	x0, x20
	b.lo	LBB1_95
; %bb.98:                               ;   in Loop: Header=BB1_97 Depth=2
	mov	w0, #0                          ; =0x0
	b	LBB1_96
LBB1_99:                                ;   in Loop: Header=BB1_59 Depth=1
	sub	w4, w16, #1
LBB1_100:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w0, w9, w5
	cmp	w0, #8
	b.lo	LBB1_104
; %bb.101:                              ;   in Loop: Header=BB1_59 Depth=1
	add	x0, x24, w4, uxtw
	cmp	w13, #1
	csinc	w1, w13, wzr, lo
	add	w1, w1, w10
	sub	w13, w1, w13
LBB1_102:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	x1, [x14, w5, uxtw]
	ldr	x2, [x0, w5, uxtw]
	cmp	x1, x2
	b.ne	LBB1_113
; %bb.103:                              ;   in Loop: Header=BB1_102 Depth=2
	add	w5, w5, #8
	sub	w13, w13, #8
	add	x22, x22, #8
	cmp	w13, #7
	b.hi	LBB1_102
LBB1_104:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w5, w9
	b.hs	LBB1_109
; %bb.105:                              ;   in Loop: Header=BB1_59 Depth=1
	add	x13, x22, #1
LBB1_106:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x22, x13
	add	w13, w16, w5
	ldrb	w0, [x24, w13, uxtw]
	sub	w13, w13, #1
	ldrb	w13, [x24, w13, uxtw]
	cmp	w0, w13
	b.ne	LBB1_109
; %bb.107:                              ;   in Loop: Header=BB1_106 Depth=2
	add	w5, w5, #1
	add	x13, x22, #1
	cmp	w10, w5
	b.ne	LBB1_106
; %bb.108:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x5, x9
LBB1_109:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w2, w5
	str	w5, [x7, #20]
	cmp	w5, #257
	b.ls	LBB1_114
	b	LBB1_123
LBB1_110:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x7, x3
	cmp	w7, w9
	b.lo	LBB1_91
LBB1_111:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w21, #12                        ; =0xc
LBB1_112:                               ;   in Loop: Header=BB1_59 Depth=1
	add	x4, x27, #1
	ldr	x13, [sp, #312]                 ; 8-byte Reload
	stp	w16, w6, [x13, #8]
	str	w7, [x13, #16]
	ldr	w13, [sp, #324]                 ; 4-byte Reload
	cbnz	w13, LBB1_142
	b	LBB1_163
LBB1_113:                               ;   in Loop: Header=BB1_59 Depth=1
	eor	x13, x2, x1
	rbit	x13, x13
	clz	x13, x13
	add	w5, w5, w13, lsr #3
	add	x22, x22, #8
	mov	w2, w5
	str	w5, [x7, #20]
	cmp	w5, #257
	b.hi	LBB1_123
LBB1_114:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w5, w9
	b.lo	LBB1_123
; %bb.115:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x1, #0                          ; =0x0
	ldr	x23, [sp, #360]                 ; 8-byte Reload
	add	x6, x24, x23
	add	x7, x24, x12
	sub	x13, x22, x2
	add	x21, x13, #258
	sub	x30, x2, #258
	mov	x0, x12
	mov	x13, x23
LBB1_116:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x23, x2, x0
	cmp	x23, x20
	b.hs	LBB1_119
; %bb.117:                              ;   in Loop: Header=BB1_116 Depth=2
	ldrb	w23, [x7, x2]
	add	x26, x2, x13
	cmp	x26, x20
	b.lo	LBB1_120
LBB1_118:                               ;   in Loop: Header=BB1_116 Depth=2
	mov	w26, #0                         ; =0x0
	cmp	w23, w26
	b.eq	LBB1_121
	b	LBB1_126
LBB1_119:                               ;   in Loop: Header=BB1_116 Depth=2
	mov	w23, #0                         ; =0x0
	add	x26, x2, x13
	cmp	x26, x20
	b.hs	LBB1_118
LBB1_120:                               ;   in Loop: Header=BB1_116 Depth=2
	ldrb	w26, [x6, x2]
	cmp	w23, w26
	b.ne	LBB1_126
LBB1_121:                               ;   in Loop: Header=BB1_116 Depth=2
	sub	x1, x1, #1
	add	x6, x6, #1
	add	x13, x13, #1
	add	x7, x7, #1
	add	x0, x0, #1
	cmp	x30, x1
	b.ne	LBB1_116
; %bb.122:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x22, x21
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	mov	w21, #12                        ; =0xc
	b	LBB1_124
LBB1_123:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w5, #258
	b.ne	LBB1_127
LBB1_124:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	x17, [sp, #248]                 ; 8-byte Reload
	and	w1, w4, #0x7fff
	mov	w10, w1
	ldrh	w12, [x25, w1, uxtw #1]
	mov	w13, #65535                     ; =0xffff
	cmp	x12, x13
	b.eq	LBB1_133
LBB1_125:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w13, w12, #8, lsl #12           ; =32768
	umaddl	x13, w13, w21, x23
	ldrh	w14, [x13]
	cmp	w14, w10
	cset	w14, ne
	strh	w11, [x13, w14, uxtw #1]
	ldrh	w13, [x7]
	b	LBB1_134
LBB1_126:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w5, w2, w1
	sub	x13, x22, x1
	add	x22, x13, #1
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	mov	w21, #12                        ; =0xc
LBB1_127:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w13, w5
	add	x0, x8, x13
	cmp	x0, x20
	b.hs	LBB1_129
; %bb.128:                              ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w0, [x24, x0]
	add	x1, x13, w4, uxtw
	cmp	x1, x20
	b.lo	LBB1_130
	b	LBB1_131
LBB1_129:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w0, #0                          ; =0x0
	add	x1, x13, w4, uxtw
	cmp	x1, x20
	b.hs	LBB1_131
LBB1_130:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w1, [x24, x1]
	eor	w0, w0, w1
LBB1_131:                               ;   in Loop: Header=BB1_59 Depth=1
	clz	w0, w0
	add	w13, w0, w13, lsl #3
	sub	w5, w13, #24
	and	w1, w4, #0x7fff
	cmp	w5, #2064
	b.ne	LBB1_135
; %bb.132:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x5, x2
	ldr	x17, [sp, #248]                 ; 8-byte Reload
	mov	w10, w1
	ldrh	w12, [x25, w1, uxtw #1]
	mov	w13, #65535                     ; =0xffff
	cmp	x12, x13
	b.ne	LBB1_125
LBB1_133:                               ;   in Loop: Header=BB1_59 Depth=1
	strh	w11, [x7]
	and	w13, w16, #0x7fff
LBB1_134:                               ;   in Loop: Header=BB1_59 Depth=1
	strh	w12, [x25, w11, uxtw #1]
	mov	w12, #-1                        ; =0xffffffff
	str	w12, [x6, x10, lsl #2]
	str	w16, [x6, w11, uxtw #2]
	ldr	x10, [sp, #240]                 ; 8-byte Reload
	add	x10, x10, #1
	str	x10, [sp, #240]                 ; 8-byte Spill
	strh	w13, [x17, x15, lsl #1]
	ldr	w10, [sp, #324]                 ; 4-byte Reload
	cmp	w10, #0
	csel	w2, w9, wzr, ne
	cset	w1, ne
	ldr	x9, [sp, #328]                  ; 8-byte Reload
	add	x9, x9, #5
	str	x9, [sp, #328]                  ; 8-byte Spill
	stp	w16, w4, [x7, #8]
	str	w5, [x7, #16]
	ldr	x4, [sp, #256]                  ; 8-byte Reload
	mov	w5, #7                          ; =0x7
	b	LBB1_227
LBB1_135:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrh	w6, [x25, w1, uxtw #1]
	mov	w13, #65535                     ; =0xffff
	cmp	x6, x13
	b.eq	LBB1_137
; %bb.136:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	w13, w6, #8, lsl #12            ; =32768
	mov	w16, #12                        ; =0xc
	umaddl	x13, w13, w21, x23
	ldrh	w0, [x13, #6]
	cmp	w5, w0
	b.ls	LBB1_154
LBB1_137:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x0, x1
	ldr	x26, [sp, #328]                 ; 8-byte Reload
LBB1_138:                               ;   in Loop: Header=BB1_59 Depth=1
	str	x26, [sp, #328]                 ; 8-byte Spill
	cmp	w2, #2
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldp	x4, x6, [sp, #256]              ; 16-byte Folded Reload
	mov	w21, #12                        ; =0xc
	ldr	x16, [sp, #368]                 ; 8-byte Reload
	b.lo	LBB1_158
; %bb.139:                              ;   in Loop: Header=BB1_59 Depth=1
	tbnz	w0, #15, LBB1_157
; %bb.140:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x17, x0
	b	LBB1_158
LBB1_141:                               ;   in Loop: Header=BB1_59 Depth=1
	eor	x13, x21, x4
	rbit	x13, x13
	clz	x13, x13
	add	w7, w7, w13, lsr #3
	add	x22, x22, #8
	mov	w21, #12                        ; =0xc
	add	x4, x27, #1
	ldr	x13, [sp, #312]                 ; 8-byte Reload
	stp	w16, w6, [x13, #8]
	str	w7, [x13, #16]
	ldr	w13, [sp, #324]                 ; 4-byte Reload
	cbz	w13, LBB1_163
LBB1_142:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, #3
	b.lo	LBB1_163
; %bb.143:                              ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, w2
	b.ls	LBB1_163
; %bb.144:                              ;   in Loop: Header=BB1_59 Depth=1
	ldrh	w1, [x25, x5, lsl #1]
	cmp	x1, x15
	b.eq	LBB1_153
; %bb.145:                              ;   in Loop: Header=BB1_59 Depth=1
	lsl	w2, w7, #3
	sub	w13, w1, #8, lsl #12            ; =32768
	umaddl	x0, w13, w21, x23
	ldrh	w21, [x0, #6]
	mov	x30, x5
	mov	x13, x17
	cmp	w2, w21
	mov	w21, #12                        ; =0xc
	b.hi	LBB1_162
; %bb.146:                              ;   in Loop: Header=BB1_59 Depth=1
	ldrh	w30, [x0, #4]
	add	x4, x27, #2
	cmp	x30, x15
	b.eq	LBB1_160
; %bb.147:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	w13, w30, #8, lsl #12           ; =32768
	umaddl	x0, w13, w21, x23
	ldrh	w0, [x0, #6]
	cmp	w2, w0
	b.hi	LBB1_152
; %bb.148:                              ;   in Loop: Header=BB1_59 Depth=1
	add	x4, x27, #3
	mov	x0, x30
	mov	w15, #65535                     ; =0xffff
LBB1_149:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x1, x0
	madd	x13, x13, x21, x23
	ldrh	w0, [x13, #4]
	cmp	x0, x15
	b.eq	LBB1_161
; %bb.150:                              ;   in Loop: Header=BB1_149 Depth=2
	sub	w13, w0, #8, lsl #12            ; =32768
	mov	w16, #12                        ; =0xc
	umaddl	x21, w13, w16, x23
	ldrh	w21, [x21, #6]
	add	x4, x4, #1
	mov	x30, x0
	cmp	w2, w21
	mov	w21, #12                        ; =0xc
	b.ls	LBB1_149
; %bb.151:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	x4, x4, #1
	ldr	x16, [sp, #368]                 ; 8-byte Reload
LBB1_152:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x13, x1
	and	x30, x1, #0xffff
	b	LBB1_162
LBB1_153:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x30, x5
	mov	x13, x17
	b	LBB1_162
LBB1_154:                               ;   in Loop: Header=BB1_59 Depth=1
	add	x7, x27, #1
	ldr	x0, [sp, #328]                  ; 8-byte Reload
	add	x21, x0, #1
                                        ; kill: def $w6 killed $w6 killed $x6 def $x6
LBB1_155:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x0, x6
	mov	x27, x7
	mov	x26, x21
	str	w4, [x13, #8]
	ldrh	w6, [x13, #4]
	mov	w13, #65535                     ; =0xffff
	cmp	x6, x13
	b.eq	LBB1_138
; %bb.156:                              ;   in Loop: Header=BB1_155 Depth=2
	sub	w13, w6, #8, lsl #12            ; =32768
	umaddl	x13, w13, w16, x23
	ldrh	w23, [x13, #6]
	add	x7, x27, #1
	add	x21, x26, #1
	cmp	w5, w23
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	b.ls	LBB1_155
	b	LBB1_138
LBB1_157:                               ;   in Loop: Header=BB1_59 Depth=1
	and	x13, x0, #0x7fff
	umaddl	x13, w13, w21, x23
	ldrh	w13, [x13, #6]
	cmp	w5, w13
	csel	w17, w1, w0, lo
LBB1_158:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	w13, [sp, #324]                 ; 4-byte Reload
	cmp	w13, #0
	cset	w13, ne
	cmp	w2, #2
	cset	w0, hi
	ands	w1, w13, w0
	csel	w2, w2, wzr, ne
	mov	w5, #7                          ; =0x7
	ldr	x0, [sp, #248]                  ; 8-byte Reload
	mov	w13, #65535                     ; =0xffff
	cmp	w3, w13
	b.ne	LBB1_83
LBB1_159:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	x12, [sp, #208]                 ; 8-byte Reload
	add	x12, x12, #1
	str	x12, [sp, #208]                 ; 8-byte Spill
	strh	w11, [x7]
	str	w16, [x6, w11, uxtw #2]
	ldr	x12, [sp, #328]                 ; 8-byte Reload
	add	x12, x12, #3
	strh	w13, [x25, w11, uxtw #1]
	strh	w11, [x0, x15, lsl #1]
	add	x0, x12, #1
	ldr	w11, [sp, #324]                 ; 4-byte Reload
	str	x0, [sp, #328]                  ; 8-byte Spill
	cbnz	w11, LBB1_211
	b	LBB1_227
LBB1_160:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x30, x1
	mov	x13, x1
	b	LBB1_162
LBB1_161:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x13, x1
	ldr	x16, [sp, #368]                 ; 8-byte Reload
LBB1_162:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	x0, [sp, #264]                  ; 8-byte Reload
	add	x0, x0, x30, lsl #2
	and	x1, x13, #0x7fff
	umaddl	x1, w1, w21, x23
	add	x1, x1, #8
	tst	w13, #0x8000
	csel	x13, x1, x0, ne
	ldr	w13, [x13]
	sub	w1, w16, w13
	mov	x2, x7
	ldr	x30, [sp, #376]                 ; 8-byte Reload
LBB1_163:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, #257
	b.hi	LBB1_173
; %bb.164:                              ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, w9
	b.lo	LBB1_173
; %bb.165:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x21, #0                         ; =0x0
	mov	w13, w7
	sub	x0, x22, x13
	add	x0, x0, #258
	add	x7, x13, x6
	add	x27, x24, x7
	add	x12, x13, x12
	add	x30, x24, x12
LBB1_166:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x23, x12, x21
	cmp	x23, x20
	b.hs	LBB1_169
; %bb.167:                              ;   in Loop: Header=BB1_166 Depth=2
	ldrb	w23, [x30, x21]
	add	x26, x7, x21
	cmp	x26, x20
	b.lo	LBB1_170
LBB1_168:                               ;   in Loop: Header=BB1_166 Depth=2
	mov	w26, #0                         ; =0x0
	cmp	w23, w26
	b.eq	LBB1_171
	b	LBB1_175
LBB1_169:                               ;   in Loop: Header=BB1_166 Depth=2
	mov	w23, #0                         ; =0x0
	add	x26, x7, x21
	cmp	x26, x20
	b.hs	LBB1_168
LBB1_170:                               ;   in Loop: Header=BB1_166 Depth=2
	ldrb	w26, [x27, x21]
	cmp	w23, w26
	b.ne	LBB1_175
LBB1_171:                               ;   in Loop: Header=BB1_166 Depth=2
	add	x21, x21, #1
	add	x23, x13, x21
	cmp	x23, #258
	b.ne	LBB1_166
; %bb.172:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x22, x0
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	mov	w15, #65535                     ; =0xffff
	mov	w21, #12                        ; =0xc
	ldrh	w0, [x25, x5, lsl #1]
	b	LBB1_181
LBB1_173:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w7, #258
	mov	w15, #65535                     ; =0xffff
	b.ne	LBB1_176
; %bb.174:                              ;   in Loop: Header=BB1_59 Depth=1
	ldrh	w0, [x25, x5, lsl #1]
	b	LBB1_181
LBB1_175:                               ;   in Loop: Header=BB1_59 Depth=1
	add	w7, w13, w21
	add	x12, x22, x21
	add	x22, x12, #1
	ldr	x30, [sp, #376]                 ; 8-byte Reload
	ldr	x23, [sp, #352]                 ; 8-byte Reload
	mov	w15, #65535                     ; =0xffff
	mov	w21, #12                        ; =0xc
LBB1_176:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, w7
	add	x13, x8, x12
	cmp	x13, x20
	b.hs	LBB1_178
; %bb.177:                              ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w13, [x24, x13]
	add	x0, x12, x6
	cmp	x0, x20
	b.lo	LBB1_179
	b	LBB1_180
LBB1_178:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w13, #0                         ; =0x0
	add	x0, x12, x6
	cmp	x0, x20
	b.hs	LBB1_180
LBB1_179:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w0, [x24, x0]
	eor	w13, w13, w0
LBB1_180:                               ;   in Loop: Header=BB1_59 Depth=1
	clz	w7, w13
	add	w12, w7, w12, lsl #3
	sub	w6, w12, #24
	ldrh	w0, [x25, x5, lsl #1]
	cmp	w6, #2064
	b.ne	LBB1_185
LBB1_181:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w0, w15
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	b.eq	LBB1_183
; %bb.182:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	w12, w0, #8, lsl #12            ; =32768
	umaddl	x12, w12, w21, x23
	ldrh	w13, [x12]
	cmp	w13, w17, uxth
	cset	w13, ne
	strh	w11, [x12, w13, uxtw #1]
	b	LBB1_184
LBB1_183:                               ;   in Loop: Header=BB1_59 Depth=1
	strh	w11, [x7]
LBB1_184:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	x15, [sp, #216]                 ; 8-byte Reload
	ldr	x12, [sp, #240]                 ; 8-byte Reload
	add	x12, x12, #1
	str	x12, [sp, #240]                 ; 8-byte Spill
	strh	w0, [x25, w11, uxtw #1]
	mov	w12, #-1                        ; =0xffffffff
	str	w12, [x6, x5, lsl #2]
	ldr	x12, [sp, #328]                 ; 8-byte Reload
	add	x12, x12, #3
	ldr	x0, [sp, #248]                  ; 8-byte Reload
	b	LBB1_210
LBB1_185:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, #65535                     ; =0xffff
	cmp	w0, w12
	b.eq	LBB1_188
; %bb.186:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	w12, w0, #8, lsl #12            ; =32768
	umaddl	x13, w12, w21, x23
	ldrh	w13, [x13, #6]
	cmp	w6, w13
	b.ls	LBB1_189
; %bb.187:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	w15, #12                        ; =0xc
	mov	w13, #65535                     ; =0xffff
	mov	w27, #0                         ; =0x0
	ldr	x21, [sp, #312]                 ; 8-byte Reload
	ldrh	w5, [x21, #2]
	cmp	w5, w13
	b.ne	LBB1_193
	b	LBB1_196
LBB1_188:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w15, #12                        ; =0xc
	mov	w13, #65535                     ; =0xffff
	mov	w27, #1                         ; =0x1
	mov	w0, #65535                      ; =0xffff
	ldr	x21, [sp, #312]                 ; 8-byte Reload
	ldrh	w5, [x21, #2]
	cmp	w5, w13
	b.ne	LBB1_193
	b	LBB1_196
LBB1_189:                               ;   in Loop: Header=BB1_59 Depth=1
	add	x13, x4, #1
                                        ; kill: def $w0 killed $w0 killed $x0 def $x0
LBB1_190:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x17, x0
	mov	x4, x13
	madd	x12, x12, x21, x23
	ldrh	w0, [x12, #4]
	mov	w12, #65535                     ; =0xffff
	cmp	x0, x12
	b.eq	LBB1_195
; %bb.191:                              ;   in Loop: Header=BB1_190 Depth=2
	sub	w12, w0, #8, lsl #12            ; =32768
	umaddl	x13, w12, w21, x23
	ldrh	w5, [x13, #6]
	add	x13, x4, #1
	cmp	w6, w5
	b.ls	LBB1_190
; %bb.192:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	w15, #12                        ; =0xc
	mov	w13, #65535                     ; =0xffff
	mov	w27, #0                         ; =0x0
                                        ; kill: def $w0 killed $w0 killed $x0 def $x0
	ldr	x21, [sp, #312]                 ; 8-byte Reload
	ldrh	w5, [x21, #2]
	cmp	w5, w13
	b.eq	LBB1_196
LBB1_193:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w12, w5, #8, lsl #12            ; =32768
	umull	x13, w12, w15
	ldrh	w13, [x23, x13]
	strh	w13, [x21, #2]
	lsr	w13, w6, #3
	add	x13, x8, x13
	cmp	x13, x20
	b.hs	LBB1_198
LBB1_194:                               ;   in Loop: Header=BB1_59 Depth=1
	ldrb	w13, [x24, x13]
	b	LBB1_199
LBB1_195:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w15, #12                        ; =0xc
	mov	w13, #65535                     ; =0xffff
	mov	w27, #1                         ; =0x1
	mov	x0, x12
	ldr	x21, [sp, #312]                 ; 8-byte Reload
	ldrh	w5, [x21, #2]
	cmp	w5, w13
	b.ne	LBB1_193
LBB1_196:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	w12, [x21, #4]
	mov	w13, #32767                     ; =0x7fff
	cmp	w12, w13
	b.hs	LBB1_230
; %bb.197:                              ;   in Loop: Header=BB1_59 Depth=1
	orr	w5, w12, #0x8000
	add	w12, w12, #1
	str	w12, [x21, #4]
	and	x12, x5, #0x7fff
	lsr	w13, w6, #3
	add	x13, x8, x13
	cmp	x13, x20
	b.lo	LBB1_194
LBB1_198:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w13, #0                         ; =0x0
LBB1_199:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w15, #12                        ; =0xc
	umaddl	x12, w12, w15, x23
	mov	w16, #7                         ; =0x7
	bic	w7, w16, w7
	lsr	w13, w13, w7
	and	w13, w13, #0x1
	strh	w11, [x12, w13, uxtw #1]
	eor	w13, w13, #0x1
	strh	w17, [x12, w13, uxtw #1]
	strh	w6, [x12, #6]
	ldr	x16, [sp, #368]                 ; 8-byte Reload
	str	w16, [x12, #8]
	cbz	w27, LBB1_203
; %bb.200:                              ;   in Loop: Header=BB1_59 Depth=1
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	strh	w5, [x7]
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	mov	w12, #65535                     ; =0xffff
	cmp	w5, w12
	b.eq	LBB1_204
LBB1_201:                               ;   in Loop: Header=BB1_59 Depth=1
	tbnz	w5, #15, LBB1_206
; %bb.202:                              ;   in Loop: Header=BB1_59 Depth=1
	strh	w0, [x25, x5, lsl #1]
	b	LBB1_207
LBB1_203:                               ;   in Loop: Header=BB1_59 Depth=1
	sub	w12, w0, #8, lsl #12            ; =32768
	umaddl	x12, w12, w15, x23
	ldrh	w13, [x12]
	cmp	w13, w17, uxth
	cset	w13, ne
	strh	w5, [x12, w13, uxtw #1]
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	mov	w12, #65535                     ; =0xffff
	cmp	w5, w12
	b.ne	LBB1_201
LBB1_204:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, #1                         ; =0x1
	ldr	x0, [sp, #248]                  ; 8-byte Reload
	tbz	w17, #15, LBB1_208
LBB1_205:                               ;   in Loop: Header=BB1_59 Depth=1
                                        ; kill: def $w17 killed $w17 killed $x17 def $x17
	and	x13, x17, #0x7fff
	umaddl	x13, w13, w15, x23
	strh	w5, [x13, #4]
	b	LBB1_209
LBB1_206:                               ;   in Loop: Header=BB1_59 Depth=1
	and	w12, w5, #0x7fff
	umaddl	x12, w12, w15, x23
	strh	w0, [x12, #4]
LBB1_207:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, #2                         ; =0x2
	ldr	x0, [sp, #248]                  ; 8-byte Reload
	tbnz	w17, #15, LBB1_205
LBB1_208:                               ;   in Loop: Header=BB1_59 Depth=1
                                        ; kill: def $w17 killed $w17 killed $x17 def $x17
	and	x13, x17, #0xffff
	strh	w5, [x25, x13, lsl #1]
LBB1_209:                               ;   in Loop: Header=BB1_59 Depth=1
	ldr	x15, [sp, #216]                 ; 8-byte Reload
	ldr	x13, [sp, #192]                 ; 8-byte Reload
	add	x13, x13, #1
	str	x13, [sp, #192]                 ; 8-byte Spill
	strh	w5, [x25, w11, uxtw #1]
	ldr	x13, [sp, #328]                 ; 8-byte Reload
	add	x12, x13, x12
	add	x12, x12, #6
LBB1_210:                               ;   in Loop: Header=BB1_59 Depth=1
	str	w16, [x6, x11, lsl #2]
	add	x12, x12, #1
	ldrh	w11, [x7]
	mov	x27, x4
	ldr	x13, [sp, #272]                 ; 8-byte Reload
	add	x13, x13, w3, uxtw
	str	x13, [sp, #272]                 ; 8-byte Spill
	ldr	x4, [sp, #256]                  ; 8-byte Reload
	mov	w5, #7                          ; =0x7
	strh	w11, [x0, x15, lsl #1]
	add	x0, x12, #1
	ldr	w11, [sp, #324]                 ; 4-byte Reload
	str	x0, [sp, #328]                  ; 8-byte Spill
	cbz	w11, LBB1_227
LBB1_211:                               ;   in Loop: Header=BB1_59 Depth=1
	subs	w13, w16, #8, lsl #12           ; =32768
	b.lo	LBB1_227
; %bb.212:                              ;   in Loop: Header=BB1_59 Depth=1
	cmp	w2, w9
	b.hs	LBB1_227
; %bb.213:                              ;   in Loop: Header=BB1_59 Depth=1
	add	x27, x27, #1
	add	x11, x22, #1
	add	w12, w2, w16
	ldrb	w12, [x24, w12, uxtw]
	add	x15, x13, w2, uxtw
	ldrb	w15, [x24, x15]
	cmp	w12, w15
	b.ne	LBB1_226
; %bb.214:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	x12, x20, x8
	cmp	x12, #8
	b.lo	LBB1_219
; %bb.215:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, #0                         ; =0x0
	add	x11, x22, #9
	mov	x15, x10
	add	x13, x24, w13, uxtw
LBB1_216:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	x16, [x14, w12, uxtw]
	ldr	x17, [x13, w12, uxtw]
	cmp	x16, x17
	b.ne	LBB1_224
; %bb.217:                              ;   in Loop: Header=BB1_216 Depth=2
	add	w12, w12, #8
	add	x11, x11, #8
	sub	w15, w15, #8
	cmp	w15, #7
	b.hi	LBB1_216
; %bb.218:                              ;   in Loop: Header=BB1_59 Depth=1
	sub	x11, x11, #8
	ldr	x16, [sp, #368]                 ; 8-byte Reload
	cmp	w12, w9
	b.lo	LBB1_220
	b	LBB1_225
LBB1_219:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w12, #0                         ; =0x0
	cmp	w12, w9
	b.hs	LBB1_225
LBB1_220:                               ;   in Loop: Header=BB1_59 Depth=1
	add	x13, x11, #1
LBB1_221:                               ;   Parent Loop BB1_59 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x11, x13
	add	w13, w16, w12
	ldrb	w14, [x24, w13, uxtw]
	sub	w13, w13, #8, lsl #12           ; =32768
	ldrb	w13, [x24, w13, uxtw]
	cmp	w14, w13
	b.ne	LBB1_225
; %bb.222:                              ;   in Loop: Header=BB1_221 Depth=2
	add	w12, w12, #1
	add	x13, x11, #1
	cmp	w10, w12
	b.ne	LBB1_221
; %bb.223:                              ;   in Loop: Header=BB1_59 Depth=1
	mov	x12, x9
	b	LBB1_225
LBB1_224:                               ;   in Loop: Header=BB1_59 Depth=1
	eor	x9, x17, x16
	rbit	x9, x9
	clz	x9, x9
	add	w12, w12, w9, lsr #3
	ldr	x16, [sp, #368]                 ; 8-byte Reload
LBB1_225:                               ;   in Loop: Header=BB1_59 Depth=1
	cmp	w12, #2
	ccmp	w12, w2, #0, hi
	csel	w2, w12, w2, hi
	mov	w9, #32768                      ; =0x8000
	csel	w1, w9, w1, hi
LBB1_226:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	x22, x11
LBB1_227:                               ;   in Loop: Header=BB1_59 Depth=1
	mov	w9, w2
	orr	x21, x9, x1, lsl #32
	ldr	x11, [sp, #280]                 ; 8-byte Reload
	lsr	x23, x21, #32
	add	x26, x30, x8, lsl #3
	ldp	w8, w9, [x26]
	cmp	w8, w21
	ccmp	w9, w23, #0, eq
	b.eq	LBB1_58
LBB1_228:                               ;   in Loop: Header=BB1_59 Depth=1
Lloh36:
	adrp	x10, ___stderrp@GOTPAGE
Lloh37:
	ldr	x10, [x10, ___stderrp@GOTPAGEOFF]
Lloh38:
	ldr	x0, [x10]
	stp	x8, x9, [sp, #24]
	stp	x21, x23, [sp, #8]
	str	x16, [sp]
Lloh39:
	adrp	x1, l_.str.18@PAGE
Lloh40:
	add	x1, x1, l_.str.18@PAGEOFF
	mov	x19, x20
	mov	x20, x25
	mov	x25, x4
	bl	_fprintf
	ldp	x16, x30, [sp, #368]            ; 16-byte Folded Reload
	mov	w5, #7                          ; =0x7
	mov	x4, x25
	mov	x25, x20
	mov	x20, x19
	mov	w19, #258                       ; =0x102
	ldr	x6, [sp, #264]                  ; 8-byte Reload
	ldr	x7, [sp, #312]                  ; 8-byte Reload
	ldr	x11, [sp, #280]                 ; 8-byte Reload
	ldr	w8, [x26]
	cmp	w8, w21
	b.ne	LBB1_230
; %bb.229:                              ;   in Loop: Header=BB1_59 Depth=1
	ldr	w8, [x26, #4]
	cmp	w8, w23
	b.eq	LBB1_58
LBB1_230:
	bl	_abort
LBB1_231:
	mov	x27, #0                         ; =0x0
	mov	x22, #0                         ; =0x0
	str	xzr, [sp, #328]                 ; 8-byte Spill
	str	xzr, [sp, #272]                 ; 8-byte Spill
	stp	xzr, xzr, [sp, #200]            ; 16-byte Folded Spill
	stp	xzr, xzr, [sp, #184]            ; 16-byte Folded Spill
	str	xzr, [sp, #240]                 ; 8-byte Spill
	str	xzr, [sp, #176]                 ; 8-byte Spill
	mov	x4, #0                          ; =0x0
	ldr	x11, [sp, #280]                 ; 8-byte Reload
LBB1_232:
	cmp	x4, x11
	b.ne	LBB1_230
; %bb.233:
	mov	x26, x11
	ldr	x23, [sp, #336]                 ; 8-byte Reload
	ldr	x8, [sp, #160]                  ; 8-byte Reload
	sub	x21, x8, x23
Lloh41:
	adrp	x8, ___stderrp@GOTPAGE
Lloh42:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh43:
	ldr	x0, [x8]
Lloh44:
	adrp	x8, l_.str.16@PAGE
Lloh45:
	add	x8, x8, l_.str.16@PAGEOFF
Lloh46:
	adrp	x9, l_.str.15@PAGE
Lloh47:
	add	x9, x9, l_.str.15@PAGEOFF
	ldr	w10, [sp, #308]                 ; 4-byte Reload
	cmp	w10, #0
	csel	x9, x9, x8, ne
	ldr	x8, [sp, #200]                  ; 8-byte Reload
	stp	x8, xzr, [sp, #56]
	ldp	x10, x8, [sp, #176]             ; 16-byte Folded Reload
	stp	x8, x10, [sp, #40]
	ldr	x10, [sp, #240]                 ; 8-byte Reload
	ldr	x8, [sp, #192]                  ; 8-byte Reload
	stp	x8, x10, [sp, #24]
	ldr	x8, [sp, #208]                  ; 8-byte Reload
	stp	x9, x8, [sp, #8]
Lloh48:
	adrp	x1, l_.str.14@PAGE
Lloh49:
	add	x1, x1, l_.str.14@PAGEOFF
	str	x9, [sp, #368]                  ; 8-byte Spill
	ldr	x8, [sp, #168]                  ; 8-byte Reload
	str	x8, [sp]
	bl	_fprintf
	ldr	x19, [sp, #344]                 ; 8-byte Reload
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	ldr	x25, [sp, #352]                 ; 8-byte Reload
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	stur	xzr, [x29, #-128]
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x29, #-160]
	stp	q0, q0, [x29, #-192]
	str	xzr, [sp, #528]
	stp	q0, q0, [sp, #496]
	stp	q0, q0, [sp, #464]
	str	xzr, [sp, #448]
	stp	q0, q0, [sp, #416]
	stp	q0, q0, [sp, #384]
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d8, [sp, #464]
	str	d0, [sp, #384]
	fdiv	d0, d8, d0
	stur	d0, [x29, #-192]
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d0, [sp, #472]
	str	d8, [sp, #392]
	fdiv	d0, d0, d8
	stur	d0, [x29, #-184]
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d8, [sp, #480]
	str	d0, [sp, #400]
	fdiv	d0, d8, d0
	stur	d0, [x29, #-176]
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d0, [sp, #488]
	str	d8, [sp, #408]
	fdiv	d0, d0, d8
	stur	d0, [x29, #-168]
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d8, [sp, #496]
	str	d0, [sp, #416]
	fdiv	d0, d8, d0
	stur	d0, [x29, #-160]
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d0, [sp, #504]
	str	d8, [sp, #424]
	fdiv	d0, d0, d8
	stur	d0, [x29, #-152]
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d8, [sp, #512]
	str	d0, [sp, #432]
	fdiv	d0, d8, d0
	stur	d0, [x29, #-144]
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d0, [sp, #520]
	str	d8, [sp, #440]
	fdiv	d0, d0, d8
	stur	d0, [x29, #-136]
	mov	x0, x19
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	mov.16b	v8, v0
	mov	x0, x25
	mov	x1, x24
	mov	x2, x20
	mov	x3, x23
	mov	x4, x21
	mov	x5, x26
	bl	__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	str	d8, [sp, #528]
	str	d0, [sp, #448]
	fdiv	d0, d8, d0
	stur	d0, [x29, #-128]
	sub	x8, x29, #192
Ltmp32:                                 ; EH_LABEL
	sub	x0, x29, #192
	add	x1, x8, #72
	sub	x2, x29, #105
	bl	__ZNSt3__16__sortIRNS_6__lessIddEEPdEEvT0_S5_T_
Ltmp33:                                 ; EH_LABEL
; %bb.234:
	add	x8, sp, #464
Ltmp34:                                 ; EH_LABEL
	add	x0, sp, #464
	add	x1, x8, #72
	sub	x2, x29, #105
	bl	__ZNSt3__16__sortIRNS_6__lessIddEEPdEEvT0_S5_T_
Ltmp35:                                 ; EH_LABEL
; %bb.235:
	add	x8, sp, #384
Ltmp36:                                 ; EH_LABEL
	add	x0, sp, #384
	add	x1, x8, #72
	sub	x2, x29, #105
	bl	__ZNSt3__16__sortIRNS_6__lessIddEEPdEEvT0_S5_T_
Ltmp37:                                 ; EH_LABEL
; %bb.236:
	ldr	d0, [sp, #496]
	ldr	d1, [sp, #416]
	ldur	d2, [x29, #-160]
	ldur	d3, [x29, #-192]
	ldur	d4, [x29, #-128]
	ldr	x9, [sp, #272]                  ; 8-byte Reload
	ldr	x8, [sp, #232]                  ; 8-byte Reload
	stp	x8, x9, [sp, #120]
	ldr	x9, [sp, #328]                  ; 8-byte Reload
	ldr	x8, [sp, #288]                  ; 8-byte Reload
	stp	x8, x9, [sp, #104]
	stp	x28, x22, [sp, #88]
	ldr	x8, [sp, #152]                  ; 8-byte Reload
	stp	x8, x27, [sp, #72]
	stp	d3, d4, [sp, #56]
	stp	d1, d2, [sp, #40]
	str	d0, [sp, #32]
	ldr	x8, [sp, #296]                  ; 8-byte Reload
	stp	x20, x8, [sp, #16]
Lloh50:
	adrp	x0, l_.str.17@PAGE
Lloh51:
	add	x0, x0, l_.str.17@PAGEOFF
	ldr	x9, [sp, #368]                  ; 8-byte Reload
	ldr	x8, [sp, #168]                  ; 8-byte Reload
	stp	x8, x9, [sp]
	bl	_printf
Lloh52:
	adrp	x8, ___stdoutp@GOTPAGE
Lloh53:
	ldr	x8, [x8, ___stdoutp@GOTPAGEOFF]
Lloh54:
	ldr	x0, [x8]
	bl	_fflush
	ldr	x0, [sp, #376]                  ; 8-byte Reload
	cbz	x0, LBB1_238
; %bb.237:
	ldr	x8, [sp, #144]                  ; 8-byte Reload
	sub	x1, x8, x0
	bl	__ZdlPvm
LBB1_238:
	ldr	x0, [sp, #336]                  ; 8-byte Reload
	cbz	x0, LBB1_240
; %bb.239:
	mov	x1, x21
	bl	__ZdlPvm
LBB1_240:
	add	sp, sp, #640
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
LBB1_241:
	bl	__ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev
LBB1_242:
Ltmp29:                                 ; EH_LABEL
	bl	__ZNSt3__16vectorIN12_GLOBAL__N_15MatchENS_9allocatorIS2_EEE20__throw_length_errorB9nqe220108Ev
Ltmp30:                                 ; EH_LABEL
; %bb.243:
	brk	#0x1
LBB1_244:
Ltmp31:                                 ; EH_LABEL
	str	x19, [sp, #336]                 ; 8-byte Spill
	mov	x23, x0
	mov	x21, x20
	mov	x0, x19
	mov	x1, x21
	bl	__ZdlPvm
	mov	x0, x23
	bl	__Unwind_Resume
LBB1_245:
Ltmp38:                                 ; EH_LABEL
	mov	x23, x0
	ldr	x8, [sp, #376]                  ; 8-byte Reload
	cbnz	x8, LBB1_248
; %bb.246:
	ldr	x8, [sp, #336]                  ; 8-byte Reload
	cbnz	x8, LBB1_249
LBB1_247:
	mov	x0, x23
	bl	__Unwind_Resume
LBB1_248:
	ldr	x0, [sp, #376]                  ; 8-byte Reload
	ldr	x8, [sp, #144]                  ; 8-byte Reload
	sub	x1, x8, x0
	bl	__ZdlPvm
	ldr	x8, [sp, #336]                  ; 8-byte Reload
	cbz	x8, LBB1_247
LBB1_249:
	ldr	x0, [sp, #336]                  ; 8-byte Reload
	mov	x1, x21
	bl	__ZdlPvm
	mov	x0, x23
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh34, Lloh35
	.loh AdrpLdrGotLdr	Lloh31, Lloh32, Lloh33
	.loh AdrpAdd	Lloh39, Lloh40
	.loh AdrpLdrGotLdr	Lloh36, Lloh37, Lloh38
	.loh AdrpAdd	Lloh48, Lloh49
	.loh AdrpAdd	Lloh46, Lloh47
	.loh AdrpAdd	Lloh44, Lloh45
	.loh AdrpLdrGotLdr	Lloh41, Lloh42, Lloh43
	.loh AdrpLdrGotLdr	Lloh52, Lloh53, Lloh54
	.loh AdrpAdd	Lloh50, Lloh51
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table1:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Lfunc_begin1-Lfunc_begin1      ; >> Call Site 1 <<
	.uleb128 Ltmp27-Lfunc_begin1            ;   Call between Lfunc_begin1 and Ltmp27
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp27-Lfunc_begin1            ; >> Call Site 2 <<
	.uleb128 Ltmp28-Ltmp27                  ;   Call between Ltmp27 and Ltmp28
	.uleb128 Ltmp31-Lfunc_begin1            ;     jumps to Ltmp31
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp28-Lfunc_begin1            ; >> Call Site 3 <<
	.uleb128 Ltmp32-Ltmp28                  ;   Call between Ltmp28 and Ltmp32
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp32-Lfunc_begin1            ; >> Call Site 4 <<
	.uleb128 Ltmp37-Ltmp32                  ;   Call between Ltmp32 and Ltmp37
	.uleb128 Ltmp38-Lfunc_begin1            ;     jumps to Ltmp38
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp37-Lfunc_begin1            ; >> Call Site 5 <<
	.uleb128 Ltmp29-Ltmp37                  ;   Call between Ltmp37 and Ltmp29
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp29-Lfunc_begin1            ; >> Call Site 6 <<
	.uleb128 Ltmp30-Ltmp29                  ;   Call between Ltmp29 and Ltmp30
	.uleb128 Ltmp31-Lfunc_begin1            ;     jumps to Ltmp31
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp30-Lfunc_begin1            ; >> Call Site 7 <<
	.uleb128 Lfunc_end1-Ltmp30              ;   Call between Ltmp30 and Lfunc_end1
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end1:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
__ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE: ; @_ZN12_GLOBAL__N_111verify_caseENSt3__14spanIKhLm18446744073709551615EEERNS_5ChainERNS_5RadixE
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	x28, x27, [sp, #48]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #64]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #80]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	mov	x20, x3
	mov	x21, x2
	mov	x19, x1
	mov	x22, x0
	add	x23, x3, #175, lsl #12          ; =716800
	add	x24, x2, #96, lsl #12           ; =393216
	mov	x0, x2
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	movi.2d	v0, #0000000000000000
	str	d0, [x24]
	str	wzr, [x24, #8]
	mov	w8, #-1                         ; =0xffffffff
	str	w8, [x23, #4084]
	str	wzr, [x23, #4088]
	add	x8, x20, #95, lsl #12           ; =389120
	add	x0, x8, #4084
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	add	x8, x20, #143, lsl #12          ; =585728
	add	x0, x8, #4084
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	mov	w8, #65532                      ; =0xfffc
	movk	w8, #10, lsl #16
	movi.2d	v0, #0000000000000000
	str	q0, [x20, x8]
	cbz	x19, LBB2_21
; %bb.1:
	mov	x27, #0                         ; =0x0
	mov	w23, #0                         ; =0x0
	mov	w25, #-1                        ; =0xffffffff
	mov	w26, #258                       ; =0x102
LBB2_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_10 Depth 2
                                        ;       Child Loop BB2_11 Depth 3
	mov	x0, x21
	mov	x1, x22
	mov	x2, x19
	mov	x3, x23
	mov	w4, #1                          ; =0x1
	bl	__ZN12_GLOBAL__N_15Chain7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	mov	x24, x0
	lsr	x28, x0, #32
	mov	x0, x20
	mov	x1, x22
	mov	x2, x19
	mov	x3, x23
	mov	w4, #1                          ; =0x1
	bl	__ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	lsr	x9, x0, #32
	cmp	w0, w24
	ccmp	x9, x28, #0, eq
	b.ne	LBB2_22
; %bb.3:                                ;   in Loop: Header=BB2_2 Depth=1
	cmp	x19, #514
	b.hs	LBB2_6
LBB2_4:                                 ;   in Loop: Header=BB2_2 Depth=1
	sub	x8, x19, x27
	cmp	x8, #258
	csel	x8, x8, x26, lo
	cmp	w23, #8, lsl #12                ; =32768
	mov	w9, #32768                      ; =0x8000
	csel	w9, w23, w9, lo
	cbnz	w23, LBB2_9
; %bb.5:                                ;   in Loop: Header=BB2_2 Depth=1
	mov	x8, #0                          ; =0x0
	b	LBB2_17
LBB2_6:                                 ;   in Loop: Header=BB2_2 Depth=1
	cmp	w23, #8, lsl #12                ; =32768
	b.eq	LBB2_8
; %bb.7:                                ;   in Loop: Header=BB2_2 Depth=1
	cmp	w23, #16, lsl #12               ; =65536
	b.ne	LBB2_20
LBB2_8:                                 ;   in Loop: Header=BB2_2 Depth=1
	sub	x8, x19, x27
	cmp	x8, #258
	csel	x8, x8, x26, lo
	mov	w9, #32768                      ; =0x8000
LBB2_9:                                 ;   in Loop: Header=BB2_2 Depth=1
	mov	w10, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w12, #1                         ; =0x1
	mov	x13, x25
LBB2_10:                                ;   Parent Loop BB2_2 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB2_11 Depth 3
	mov	w14, #0                         ; =0x0
	mov	x15, x8
LBB2_11:                                ;   Parent Loop BB2_2 Depth=1
                                        ;     Parent Loop BB2_10 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	w16, w23, w14
	ldrb	w16, [x22, w16, uxtw]
	add	w17, w13, w14
	ldrb	w17, [x22, w17, uxtw]
	cmp	w16, w17
	b.ne	LBB2_14
; %bb.12:                               ;   in Loop: Header=BB2_11 Depth=3
	add	w14, w14, #1
	subs	x15, x15, #1
	b.ne	LBB2_11
; %bb.13:                               ;   in Loop: Header=BB2_10 Depth=2
	mov	x14, x8
LBB2_14:                                ;   in Loop: Header=BB2_10 Depth=2
	cmp	w14, #2
	ccmp	w14, w11, #0, hi
	csel	w11, w14, w11, hi
	csel	w10, w12, w10, hi
	cmp	w12, w9
	b.hs	LBB2_16
; %bb.15:                               ;   in Loop: Header=BB2_10 Depth=2
	add	w12, w12, #1
	sub	w13, w13, #1
	cmp	w14, w8
	b.ne	LBB2_10
LBB2_16:                                ;   in Loop: Header=BB2_2 Depth=1
	orr	x8, x11, x10, lsl #32
LBB2_17:                                ;   in Loop: Header=BB2_2 Depth=1
	cmp	w24, w8
	b.ne	LBB2_23
; %bb.18:                               ;   in Loop: Header=BB2_2 Depth=1
	lsr	x8, x8, #32
	cmp	x28, x8
	b.ne	LBB2_23
LBB2_19:                                ;   in Loop: Header=BB2_2 Depth=1
	add	w27, w23, #1
	add	w25, w25, #1
	mov	x23, x27
	cmp	x19, x27
	b.hi	LBB2_2
	b	LBB2_21
LBB2_20:                                ;   in Loop: Header=BB2_2 Depth=1
	add	w8, w23, #258
	cmp	x19, x8
	b.eq	LBB2_4
	b	LBB2_19
LBB2_21:
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #80]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #64]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #48]             ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB2_22:
Lloh55:
	adrp	x8, ___stderrp@GOTPAGE
Lloh56:
	ldr	x8, [x8, ___stderrp@GOTPAGEOFF]
Lloh57:
	ldr	x8, [x8]
                                        ; kill: def $w24 killed $w24 killed $x24 def $x24
	stp	x24, x28, [sp, #32]
                                        ; kill: def $w0 killed $w0 killed $x0 def $x0
	stp	x0, x9, [sp, #16]
	stp	x19, x23, [sp]
Lloh58:
	adrp	x1, l_.str.12@PAGE
Lloh59:
	add	x1, x1, l_.str.12@PAGEOFF
	mov	x0, x8
	bl	_fprintf
LBB2_23:
	bl	_abort
	.loh AdrpAdd	Lloh58, Lloh59
	.loh AdrpLdrGotLdr	Lloh55, Lloh56, Lloh57
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_15Chain7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
__ZN12_GLOBAL__N_15Chain7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE: ; @_ZN12_GLOBAL__N_15Chain7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	.cfi_startproc
; %bb.0:
	mov	w9, w3
	sub	x8, x2, x9
	mov	w10, #258                       ; =0x102
	cmp	x8, #258
	csel	x10, x8, x10, lo
	cmp	x8, #3
	b.hs	LBB3_2
; %bb.1:
	mov	x0, #0                          ; =0x0
	ret
LBB3_2:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	add	x8, x0, #96, lsl #12            ; =393216
	add	x12, x1, x9
	ldrb	w9, [x12]
	add	w11, w3, #1
	ldrb	w11, [x1, w11, uxtw]
	add	w13, w3, #2
	ldrb	w13, [x1, w13, uxtw]
	ubfiz	x9, x9, #12, #4
	eor	x9, x9, x11, lsl #6
	eor	x9, x9, x13
	ldr	w11, [x0, x9, lsl #2]
	subs	w13, w3, #8, lsl #12            ; =32768
	csel	w15, wzr, w13, lo
	ldr	w13, [x8]
	ldr	w14, [x8, #8]
	sub	w17, w3, w13
	subs	w13, w14, w17
	csel	w16, wzr, w13, lo
	cbz	w4, LBB3_6
; %bb.3:
	cmn	w11, #1
	ccmp	w11, w15, #0, ne
	b.hs	LBB3_7
; %bb.4:
	mov	w14, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
LBB3_5:
	and	w10, w3, #0x7fff
	add	x10, x0, w10, uxtw #2
	mov	w12, #262144                    ; =0x40000
	str	w11, [x10, x12]
	str	w3, [x0, x9, lsl #2]
	sub	w9, w3, w13
	stp	w3, w9, [x8]
	str	w14, [x8, #8]
	mov	w8, w14
	orr	x0, x8, x13, lsl #32
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB3_6:
	and	w10, w3, #0x7fff
	add	x10, x0, w10, uxtw #2
	mov	w12, #262144                    ; =0x40000
	str	w11, [x10, x12]
	str	w3, [x0, x9, lsl #2]
	mov	x0, #0                          ; =0x0
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB3_7:
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	ldr	w2, [x8, #4]
	add	w17, w2, w17
	add	x2, x0, #64, lsl #12            ; =262144
	mov	x4, x11
	b	LBB3_11
LBB3_8:                                 ;   in Loop: Header=BB3_11 Depth=1
	eor	x6, x20, x19
	rbit	x6, x6
	clz	x6, x6
	add	w5, w5, w6, lsr #3
LBB3_9:                                 ;   in Loop: Header=BB3_11 Depth=1
	sub	w6, w3, w4
	cmp	w5, #2
	ccmp	w5, w14, #0, hi
	csel	w14, w5, w14, hi
	csel	w13, w6, w13, hi
	cmp	w5, w10
	b.eq	LBB3_5
LBB3_10:                                ;   in Loop: Header=BB3_11 Depth=1
	and	w4, w4, #0x7fff
	ldr	w4, [x2, w4, uxtw #2]
	cmn	w4, #1
	ccmp	w4, w15, #0, ne
	b.lo	LBB3_5
LBB3_11:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_15 Depth 2
                                        ;     Child Loop BB3_18 Depth 2
	cmp	w14, #3
	b.lo	LBB3_13
; %bb.12:                               ;   in Loop: Header=BB3_11 Depth=1
	add	w5, w14, w3
	ldrb	w5, [x1, w5, uxtw]
	add	w6, w14, w4
	ldrb	w6, [x1, w6, uxtw]
	cmp	w5, w6
	b.ne	LBB3_10
LBB3_13:                                ;   in Loop: Header=BB3_11 Depth=1
	cmp	w4, w17
	csel	w5, w16, wzr, eq
	sub	w6, w10, w5
	cmp	w6, #8
	b.lo	LBB3_17
; %bb.14:                               ;   in Loop: Header=BB3_11 Depth=1
	add	x7, x1, w4, uxtw
LBB3_15:                                ;   Parent Loop BB3_11 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	x19, [x12, w5, uxtw]
	ldr	x20, [x7, w5, uxtw]
	cmp	x19, x20
	b.ne	LBB3_8
; %bb.16:                               ;   in Loop: Header=BB3_15 Depth=2
	add	w5, w5, #8
	sub	w6, w6, #8
	cmp	w6, #7
	b.hi	LBB3_15
LBB3_17:                                ;   in Loop: Header=BB3_11 Depth=1
	cmp	w5, w10
	b.hs	LBB3_9
LBB3_18:                                ;   Parent Loop BB3_11 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	w6, w3, w5
	ldrb	w6, [x1, w6, uxtw]
	add	w7, w4, w5
	ldrb	w7, [x1, w7, uxtw]
	cmp	w6, w7
	b.ne	LBB3_9
; %bb.19:                               ;   in Loop: Header=BB3_18 Depth=2
	add	w5, w5, #1
	cmp	w10, w5
	b.ne	LBB3_18
; %bb.20:                               ;   in Loop: Header=BB3_11 Depth=1
	mov	x5, x10
	b	LBB3_9
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
__ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE: ; @_ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	.cfi_startproc
; %bb.0:
	stp	x26, x25, [sp, #-80]!           ; 16-byte Folded Spill
	stp	x24, x23, [sp, #16]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #32]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #48]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #64]             ; 16-byte Folded Spill
	add	x29, sp, #64
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	w15, w3
	sub	x10, x2, x15
	mov	w8, #258                        ; =0x102
	cmp	x10, #258
	csel	x8, x10, x8, lo
	cmp	x10, #3
	b.hs	LBB4_2
; %bb.1:
	mov	x0, #0                          ; =0x0
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB4_2:
	add	x9, x0, #175, lsl #12           ; =716800
	add	x6, x9, #4092
	add	x9, x0, #175, lsl #12           ; =716800
	add	x11, x9, #4084
	and	w9, w3, #0x7fff
	subs	w13, w3, #8, lsl #12            ; =32768
	b.lo	LBB4_17
; %bb.3:
	add	x12, x0, #95, lsl #12           ; =389120
	add	x12, x12, #4084
	ldr	w17, [x12, w9, uxtw #2]
	cmp	w17, w13
	b.ne	LBB4_17
; %bb.4:
	ldrb	w14, [x1, x17]
	mov	w13, #-32767                    ; =0xffff8001
	add	w13, w3, w13
	ldrb	w13, [x1, w13, uxtw]
	add	x14, x0, x14, lsl #9
	add	x14, x14, #143, lsl #12         ; =585728
	add	x14, x14, #4084
	ldrh	w5, [x14, x13, lsl #1]
	strh	w5, [x11]
	and	w16, w3, #0x7fff
	ldr	w7, [x12, w16, uxtw #2]
	cmp	w7, w17
	b.ne	LBB4_16
; %bb.5:
	add	x17, x0, #127, lsl #12          ; =520192
	add	x7, x17, #4084
	ldrh	w17, [x7, x16, lsl #1]
	mov	w5, #65535                      ; =0xffff
	cmp	w17, w5
	b.ne	LBB4_7
; %bb.6:
	strh	w5, [x11]
	b	LBB4_15
LBB4_7:
	sub	w5, w17, #8, lsl #12            ; =32768
	mov	w21, #12                        ; =0xc
	umaddl	x5, w5, w21, x0
	ldrh	w19, [x5]
	cmp	w16, w19
	cset	w19, eq
	ldrh	w20, [x5, w19, uxtw #1]
	ldrh	w19, [x5, #4]
	mov	w22, #65535                     ; =0xffff
	cmp	x19, x22
	b.eq	LBB4_9
; %bb.8:
	sub	w22, w19, #8, lsl #12           ; =32768
	umaddl	x21, w22, w21, x0
	ldrh	w22, [x21]
	cmp	w22, w17
	cset	w22, ne
	strh	w20, [x21, w22, uxtw #1]
	b	LBB4_10
LBB4_9:
	strh	w20, [x11]
LBB4_10:
	mov	w21, #65535                     ; =0xffff
	cmp	w20, w21
	b.eq	LBB4_14
; %bb.11:
	tbnz	w20, #15, LBB4_13
; %bb.12:
	strh	w19, [x7, x20, lsl #1]
	b	LBB4_14
LBB4_13:
	and	x7, x20, #0x7fff
	mov	w20, #12                        ; =0xc
	umaddl	x7, w7, w20, x0
	strh	w19, [x7, #4]
LBB4_14:
	ldrh	w7, [x11, #2]
	strh	w7, [x5]
	strh	w17, [x11, #2]
	ldrh	w5, [x11]
LBB4_15:
	mov	w17, #-1                        ; =0xffffffff
	str	w17, [x12, x16, lsl #2]
LBB4_16:
	strh	w5, [x14, x13, lsl #1]
LBB4_17:
	add	x12, x1, x15
	ldrb	w14, [x12]
	add	w13, w3, #1
	ldrb	w13, [x1, w13, uxtw]
	add	x14, x0, x14, lsl #9
	add	x14, x14, #143, lsl #12         ; =585728
	add	x14, x14, #4084
	ldrh	w19, [x14, x13, lsl #1]
	mov	x16, x19
	strh	w19, [x11]
	cbz	w3, LBB4_21
; %bb.18:
	ldr	w5, [x11, #20]
	cmp	w5, #0
	cset	w7, ne
	cmp	w5, #2
	b.hs	LBB4_37
; %bb.19:
	ldrb	w20, [x12]
	sub	w17, w3, #1
	ldrb	w21, [x1, w17, uxtw]
	cmp	w20, w21
	b.eq	LBB4_38
; %bb.20:
	mov	w7, #0                          ; =0x0
	b	LBB4_47
LBB4_21:
	mov	w5, #0                          ; =0x0
	mov	w17, #0                         ; =0x0
	mov	w7, #65535                      ; =0xffff
	cmp	w19, w7
	b.eq	LBB4_88
LBB4_22:
	tbz	w16, #15, LBB4_27
; %bb.23:
	mov	w7, #12                         ; =0xc
	mov	w19, #7                         ; =0x7
	b	LBB4_25
LBB4_24:                                ;   in Loop: Header=BB4_25 Depth=1
	ldrb	w21, [x1, x21]
	bic	w20, w19, w20
	lsr	w20, w21, w20
	and	w20, w20, #0x1
	ldrsh	w16, [x16, w20, uxtw #1]
	tbz	w16, #31, LBB4_27
LBB4_25:                                ; =>This Inner Loop Header: Depth=1
	and	x16, x16, #0x7fff
	umaddl	x16, w16, w7, x0
	ldrh	w20, [x16, #6]
	add	x21, x15, x20, lsr #3
	cmp	x21, x2
	b.lo	LBB4_24
; %bb.26:                               ;   in Loop: Header=BB4_25 Depth=1
	mov	w21, #0                         ; =0x0
	bic	w20, w19, w20
	lsr	w20, w21, w20
	and	w20, w20, #0x1
	ldrsh	w16, [x16, w20, uxtw #1]
	tbnz	w16, #31, LBB4_25
LBB4_27:
	add	x7, x0, #95, lsl #12            ; =389120
	add	x7, x7, #4084
	and	x19, x16, #0xffff
	ldr	w20, [x7, x19, lsl #2]
	ldr	w22, [x6]
	sub	w21, w3, #1
	mov	w23, #2                         ; =0x2
	ldp	w25, w24, [x6, #8]
	cmp	w20, w21
	csel	w21, w24, w23, eq
	sub	w22, w3, w22
	subs	w23, w25, w22
	b.ls	LBB4_29
; %bb.28:
	ldr	w24, [x6, #4]
	cmp	w21, w23
	csel	w23, w21, w23, hi
	add	w22, w24, w22
	cmp	w20, w22
	csel	w21, w21, w23, ne
LBB4_29:
	sub	w22, w8, w21
	cmp	w22, #8
	b.lo	LBB4_33
; %bb.30:
	add	x23, x1, x20
LBB4_31:                                ; =>This Inner Loop Header: Depth=1
	ldr	x24, [x12, w21, uxtw]
	ldr	x25, [x23, w21, uxtw]
	cmp	x24, x25
	b.ne	LBB4_72
; %bb.32:                               ;   in Loop: Header=BB4_31 Depth=1
	add	w21, w21, #8
	sub	w22, w22, #8
	cmp	w22, #7
	b.hi	LBB4_31
LBB4_33:
	cmp	w21, w8
	b.hs	LBB4_73
LBB4_34:                                ; =>This Inner Loop Header: Depth=1
	add	w22, w3, w21
	ldrb	w22, [x1, w22, uxtw]
	add	w23, w20, w21
	ldrb	w23, [x1, w23, uxtw]
	cmp	w22, w23
	b.ne	LBB4_73
; %bb.35:                               ;   in Loop: Header=BB4_34 Depth=1
	add	w21, w21, #1
	cmp	w8, w21
	b.ne	LBB4_34
; %bb.36:
	mov	x21, x8
	b	LBB4_73
LBB4_37:
	sub	w17, w3, #1
LBB4_38:
	sub	w7, w5, w7
	sub	w5, w8, w7
	cmp	w5, #8
	b.lo	LBB4_42
; %bb.39:
	add	x20, x1, w17, uxtw
LBB4_40:                                ; =>This Inner Loop Header: Depth=1
	ldr	x21, [x12, w7, uxtw]
	ldr	x22, [x20, w7, uxtw]
	cmp	x21, x22
	b.ne	LBB4_46
; %bb.41:                               ;   in Loop: Header=BB4_40 Depth=1
	add	w7, w7, #8
	sub	w5, w5, #8
	cmp	w5, #7
	b.hi	LBB4_40
LBB4_42:
	cmp	w7, w8
	b.hs	LBB4_47
LBB4_43:                                ; =>This Inner Loop Header: Depth=1
	add	w5, w3, w7
	ldrb	w20, [x1, w5, uxtw]
	sub	w5, w5, #1
	ldrb	w5, [x1, w5, uxtw]
	cmp	w20, w5
	b.ne	LBB4_47
; %bb.44:                               ;   in Loop: Header=BB4_43 Depth=1
	add	w7, w7, #1
	cmp	w8, w7
	b.ne	LBB4_43
; %bb.45:
	mov	x7, x8
	b	LBB4_47
LBB4_46:
	eor	x5, x22, x21
	rbit	x5, x5
	clz	x5, x5
	add	w7, w7, w5, lsr #3
LBB4_47:
	str	w7, [x11, #20]
	cmp	w7, #257
	b.hi	LBB4_56
; %bb.48:
	cmp	w7, w8
	b.lo	LBB4_56
; %bb.49:
	mov	w20, w7
	sub	x5, x20, #258
	add	x21, x20, w17, uxtw
	add	x22, x20, x15
	mov	x20, x7
LBB4_50:                                ; =>This Inner Loop Header: Depth=1
	cmp	x22, x2
	b.hs	LBB4_53
; %bb.51:                               ;   in Loop: Header=BB4_50 Depth=1
	ldrb	w23, [x1, x22]
	cmp	x21, x2
	b.lo	LBB4_54
LBB4_52:                                ;   in Loop: Header=BB4_50 Depth=1
	mov	w24, #0                         ; =0x0
	cmp	w23, w24
	b.eq	LBB4_55
	b	LBB4_58
LBB4_53:                                ;   in Loop: Header=BB4_50 Depth=1
	mov	w23, #0                         ; =0x0
	cmp	x21, x2
	b.hs	LBB4_52
LBB4_54:                                ;   in Loop: Header=BB4_50 Depth=1
	ldrb	w24, [x1, x21]
	cmp	w23, w24
	b.ne	LBB4_58
LBB4_55:                                ;   in Loop: Header=BB4_50 Depth=1
	add	w20, w20, #1
	add	x21, x21, #1
	add	x22, x22, #1
	adds	x5, x5, #1
	b.lo	LBB4_50
	b	LBB4_57
LBB4_56:
	mov	x20, x7
	cmp	w7, #258
	b.ne	LBB4_58
LBB4_57:
	and	w5, w17, #0x7fff
	b	LBB4_63
LBB4_58:
	mov	w5, w20
	add	x20, x5, x15
	cmp	x20, x2
	b.hs	LBB4_60
; %bb.59:
	ldrb	w20, [x1, x20]
	add	x21, x5, w17, uxtw
	cmp	x21, x2
	b.lo	LBB4_61
	b	LBB4_62
LBB4_60:
	mov	w20, #0                         ; =0x0
	add	x21, x5, w17, uxtw
	cmp	x21, x2
	b.hs	LBB4_62
LBB4_61:
	ldrb	w21, [x1, x21]
	eor	w20, w20, w21
LBB4_62:
	clz	w20, w20
	add	w5, w20, w5, lsl #3
	sub	w20, w5, #24
	and	w5, w17, #0x7fff
	cmp	w20, #2064
	b.ne	LBB4_67
LBB4_63:
	add	x10, x0, #127, lsl #12          ; =520192
	add	x12, x10, #4084
	mov	w10, w5
	ldrh	w15, [x12, w5, uxtw #1]
	mov	w16, #65535                     ; =0xffff
	cmp	x15, x16
	b.eq	LBB4_65
; %bb.64:
	sub	w16, w15, #8, lsl #12           ; =32768
	mov	w1, #12                         ; =0xc
	umaddl	x16, w16, w1, x0
	ldrh	w1, [x16]
	cmp	w1, w10
	cset	w1, ne
	strh	w9, [x16, w1, uxtw #1]
	ldrh	w11, [x11]
	b	LBB4_66
LBB4_65:
	strh	w9, [x11]
	and	w11, w3, #0x7fff
LBB4_66:
	strh	w15, [x12, w9, uxtw #1]
	add	x12, x0, #95, lsl #12           ; =389120
	add	x12, x12, #4084
	mov	w15, #-1                        ; =0xffffffff
	str	w15, [x12, x10, lsl #2]
	str	w3, [x12, w9, uxtw #2]
	strh	w11, [x14, x13, lsl #1]
	stp	w3, w17, [x6]
	cmp	w4, #0
	csel	w5, w8, wzr, ne
	mov	x17, x4
	str	w7, [x6, #8]
	b	LBB4_151
LBB4_67:
	add	x21, x0, w5, uxtw #1
	add	x22, x21, #126, lsl #12         ; =516096
	ldrh	w25, [x22, #8180]
	mov	w21, #65535                     ; =0xffff
	cmp	x25, x21
	b.eq	LBB4_82
; %bb.68:
	sub	w21, w25, #8, lsl #12           ; =32768
	mov	w22, #12                        ; =0xc
	umaddl	x23, w21, w22, x0
	ldrh	w24, [x23, #6]
	and	w21, w17, #0x7fff
	cmp	w20, w24
	b.hi	LBB4_83
; %bb.69:
	mov	w24, #65535                     ; =0xffff
                                        ; kill: def $w25 killed $w25 killed $x25 def $x25
LBB4_70:                                ; =>This Inner Loop Header: Depth=1
	mov	x21, x25
	str	w17, [x23, #8]
	ldrh	w25, [x23, #4]
	cmp	x25, x24
	b.eq	LBB4_83
; %bb.71:                               ;   in Loop: Header=BB4_70 Depth=1
	sub	w23, w25, #8, lsl #12           ; =32768
	umaddl	x23, w23, w22, x0
	ldrh	w26, [x23, #6]
	cmp	w20, w26
	b.ls	LBB4_70
	b	LBB4_83
LBB4_72:
	eor	x22, x25, x24
	rbit	x22, x22
	clz	x22, x22
	add	w21, w21, w22, lsr #3
LBB4_73:
	stp	w3, w20, [x6]
	str	w21, [x6, #8]
	cbz	w4, LBB4_91
; %bb.74:
	cmp	w21, #3
	b.lo	LBB4_91
; %bb.75:
	cmp	w21, w5
	b.ls	LBB4_91
; %bb.76:
	add	x17, x0, x19, lsl #1
	add	x5, x17, #126, lsl #12          ; =516096
	ldrh	w5, [x5, #8180]
	mov	w22, #65535                     ; =0xffff
	mov	x6, x19
	mov	x17, x16
	cmp	x5, x22
	b.eq	LBB4_90
; %bb.77:
	lsl	w6, w21, #3
	sub	w23, w5, #8, lsl #12            ; =32768
	mov	w22, #12                        ; =0xc
	umaddl	x17, w23, w22, x0
	ldrh	w24, [x17, #6]
	mov	x17, x16
	cmp	w6, w24
	b.hi	LBB4_81
; %bb.78:
	mov	w24, #65535                     ; =0xffff
	mov	x25, x5
LBB4_79:                                ; =>This Inner Loop Header: Depth=1
	mov	x17, x25
	madd	x23, x23, x22, x0
	ldrh	w25, [x23, #4]
	cmp	x25, x24
	b.eq	LBB4_89
; %bb.80:                               ;   in Loop: Header=BB4_79 Depth=1
	sub	w23, w25, #8, lsl #12           ; =32768
	umaddl	x5, w23, w22, x0
	ldrh	w26, [x5, #6]
	mov	x5, x25
	cmp	w6, w26
	b.ls	LBB4_79
LBB4_81:
	and	x6, x17, #0xffff
	b	LBB4_90
LBB4_82:
	mov	x21, x5
LBB4_83:
	cmp	w7, #2
	b.lo	LBB4_87
; %bb.84:
	tbnz	w21, #15, LBB4_86
; %bb.85:
	mov	x16, x21
	b	LBB4_87
LBB4_86:
	and	x16, x21, #0x7fff
	mov	w17, #12                        ; =0xc
	umaddl	x16, w16, w17, x0
	ldrh	w16, [x16, #6]
	cmp	w20, w16
	csel	w16, w5, w21, lo
LBB4_87:
	cmp	w7, #2
	cset	w17, hi
	ands	w17, w4, w17
	csel	w5, w7, wzr, ne
	mov	w7, #65535                      ; =0xffff
	cmp	w19, w7
	b.ne	LBB4_22
LBB4_88:
	strh	w9, [x11]
	add	x11, x0, w9, uxtw #2
	mov	w15, #65524                     ; =0xfff4
	movk	w15, #5, lsl #16
	str	w3, [x11, x15]
	add	x11, x0, w9, uxtw #1
	mov	w15, #65524                     ; =0xfff4
	movk	w15, #7, lsl #16
	strh	w7, [x11, x15]
	strh	w9, [x14, x13, lsl #1]
	cbnz	w4, LBB4_137
	b	LBB4_151
LBB4_89:
	mov	x6, x5
LBB4_90:
	add	x5, x7, x6, lsl #2
	and	x6, x17, #0x7fff
	mov	w22, #12                        ; =0xc
	umaddl	x6, w6, w22, x0
	add	x6, x6, #8
	tst	w17, #0x8000
	csel	x17, x6, x5, ne
	ldr	w17, [x17]
	sub	w17, w3, w17
	mov	x5, x21
LBB4_91:
	cmp	w21, #257
	b.hi	LBB4_100
; %bb.92:
	cmp	w21, w8
	b.lo	LBB4_100
; %bb.93:
	mov	w23, w21
	sub	x6, x23, #258
	add	x22, x23, x20
	add	x23, x23, x15
LBB4_94:                                ; =>This Inner Loop Header: Depth=1
	cmp	x23, x2
	b.hs	LBB4_97
; %bb.95:                               ;   in Loop: Header=BB4_94 Depth=1
	ldrb	w24, [x1, x23]
	cmp	x22, x2
	b.lo	LBB4_98
LBB4_96:                                ;   in Loop: Header=BB4_94 Depth=1
	mov	w25, #0                         ; =0x0
	cmp	w24, w25
	b.eq	LBB4_99
	b	LBB4_102
LBB4_97:                                ;   in Loop: Header=BB4_94 Depth=1
	mov	w24, #0                         ; =0x0
	cmp	x22, x2
	b.hs	LBB4_96
LBB4_98:                                ;   in Loop: Header=BB4_94 Depth=1
	ldrb	w25, [x1, x22]
	cmp	w24, w25
	b.ne	LBB4_102
LBB4_99:                                ;   in Loop: Header=BB4_94 Depth=1
	add	w21, w21, #1
	add	x22, x22, #1
	add	x23, x23, #1
	adds	x6, x6, #1
	b.lo	LBB4_94
	b	LBB4_101
LBB4_100:
	cmp	w21, #258
	b.ne	LBB4_102
LBB4_101:
	add	x15, x0, x19, lsl #1
	add	x2, x15, #126, lsl #12          ; =516096
	ldrh	w20, [x2, #8180]
	b	LBB4_107
LBB4_102:
	mov	w6, w21
	add	x21, x6, x15
	cmp	x21, x2
	b.hs	LBB4_104
; %bb.103:
	ldrb	w21, [x1, x21]
	add	x20, x6, x20
	cmp	x20, x2
	b.lo	LBB4_105
	b	LBB4_106
LBB4_104:
	mov	w21, #0                         ; =0x0
	add	x20, x6, x20
	cmp	x20, x2
	b.hs	LBB4_106
LBB4_105:
	ldrb	w20, [x1, x20]
	eor	w21, w21, w20
LBB4_106:
	clz	w22, w21
	add	w6, w22, w6, lsl #3
	sub	w21, w6, #24
	add	x6, x0, #127, lsl #12           ; =520192
	add	x6, x6, #4084
	ldrh	w20, [x6, x19, lsl #1]
	cmp	w21, #2064
	b.ne	LBB4_111
LBB4_107:
	mov	w15, #65535                     ; =0xffff
	cmp	w20, w15
	b.eq	LBB4_109
; %bb.108:
	sub	w15, w20, #8, lsl #12           ; =32768
	mov	w2, #12                         ; =0xc
	umaddl	x15, w15, w2, x0
	ldrh	w2, [x15]
	cmp	w2, w16, uxth
	cset	w16, ne
	strh	w9, [x15, w16, uxtw #1]
	b	LBB4_110
LBB4_109:
	strh	w9, [x11]
LBB4_110:
	add	x15, x0, w9, uxtw #1
	mov	w16, #65524                     ; =0xfff4
	movk	w16, #7, lsl #16
	strh	w20, [x15, x16]
	mov	w15, #-1                        ; =0xffffffff
	str	w15, [x7, x19, lsl #2]
	str	w3, [x7, x9, lsl #2]
	ldrh	w9, [x11]
	strh	w9, [x14, x13, lsl #1]
	cbnz	w4, LBB4_137
	b	LBB4_151
LBB4_111:
	add	x24, x0, #175, lsl #12          ; =716800
	mov	w25, #65535                     ; =0xffff
	cmp	w20, w25
	b.eq	LBB4_114
; %bb.112:
	sub	w23, w20, #8, lsl #12           ; =32768
	mov	w19, #12                        ; =0xc
	umaddl	x26, w23, w19, x0
	ldrh	w26, [x26, #6]
	cmp	w21, w26
	b.ls	LBB4_115
; %bb.113:
	mov	w23, #0                         ; =0x0
	b	LBB4_120
LBB4_114:
	mov	w23, #1                         ; =0x1
	mov	w20, #65535                     ; =0xffff
	b	LBB4_120
LBB4_115:
                                        ; kill: def $w20 killed $w20 killed $x20 def $x20
LBB4_116:                               ; =>This Inner Loop Header: Depth=1
	mov	x16, x20
	madd	x20, x23, x19, x0
	ldrh	w20, [x20, #4]
	mov	w26, #65535                     ; =0xffff
	cmp	x20, x26
	b.eq	LBB4_119
; %bb.117:                              ;   in Loop: Header=BB4_116 Depth=1
	sub	w23, w20, #8, lsl #12           ; =32768
	umaddl	x26, w23, w19, x0
	ldrh	w26, [x26, #6]
	cmp	w21, w26
	b.ls	LBB4_116
; %bb.118:
	mov	w23, #0                         ; =0x0
                                        ; kill: def $w20 killed $w20 killed $x20 def $x20
	b	LBB4_120
LBB4_119:
	mov	w23, #1                         ; =0x1
	mov	x20, x26
LBB4_120:
	ldrh	w19, [x24, #4086]
	cmp	w19, w25
	b.ne	LBB4_124
; %bb.121:
	ldr	w25, [x24, #4088]
	mov	w19, #32767                     ; =0x7fff
	cmp	w25, w19
	b.hs	LBB4_152
; %bb.122:
	orr	w19, w25, #0x8000
	add	w25, w25, #1
	str	w25, [x24, #4088]
	and	x25, x19, #0x7fff
	mov	w24, #12                        ; =0xc
	umaddl	x24, w25, w24, x0
	lsr	w25, w21, #3
	add	x15, x25, x15
	cmp	x15, x2
	b.lo	LBB4_125
LBB4_123:
	mov	w15, #0                         ; =0x0
	b	LBB4_126
LBB4_124:
	sub	w25, w19, #8, lsl #12           ; =32768
	mov	w26, #12                        ; =0xc
	umull	x26, w25, w26
	ldrh	w26, [x0, x26]
	strh	w26, [x24, #4086]
	mov	w24, #12                        ; =0xc
	umaddl	x24, w25, w24, x0
	lsr	w25, w21, #3
	add	x15, x25, x15
	cmp	x15, x2
	b.hs	LBB4_123
LBB4_125:
	ldrb	w15, [x1, x15]
LBB4_126:
	mov	w2, #7                          ; =0x7
	bic	w2, w2, w22
	lsr	w15, w15, w2
	and	w15, w15, #0x1
	strh	w9, [x24, w15, uxtw #1]
	eor	w15, w15, #0x1
	strh	w16, [x24, w15, uxtw #1]
	strh	w21, [x24, #6]
	str	w3, [x24, #8]
	cbz	w23, LBB4_128
; %bb.127:
	strh	w19, [x11]
	b	LBB4_129
LBB4_128:
	sub	w15, w20, #8, lsl #12           ; =32768
	mov	w2, #12                         ; =0xc
	umaddl	x15, w15, w2, x0
	ldrh	w2, [x15]
	cmp	w2, w16, uxth
	cset	w2, ne
	strh	w19, [x15, w2, uxtw #1]
LBB4_129:
	mov	w15, #65535                     ; =0xffff
	cmp	w19, w15
	b.eq	LBB4_133
; %bb.130:
	tbnz	w19, #15, LBB4_132
; %bb.131:
	strh	w20, [x6, x19, lsl #1]
	b	LBB4_133
LBB4_132:
	and	w15, w19, #0x7fff
	mov	w2, #12                         ; =0xc
	umaddl	x15, w15, w2, x0
	strh	w20, [x15, #4]
LBB4_133:
	tbnz	w16, #15, LBB4_135
; %bb.134:
                                        ; kill: def $w16 killed $w16 killed $x16 def $x16
	and	x15, x16, #0xffff
	strh	w19, [x6, x15, lsl #1]
	b	LBB4_136
LBB4_135:
                                        ; kill: def $w16 killed $w16 killed $x16 def $x16
	and	x15, x16, #0x7fff
	mov	w16, #12                        ; =0xc
	umaddl	x15, w15, w16, x0
	strh	w19, [x15, #4]
LBB4_136:
	strh	w19, [x6, w9, uxtw #1]
	str	w3, [x7, x9, lsl #2]
	ldrh	w9, [x11]
	strh	w9, [x14, x13, lsl #1]
	cbz	w4, LBB4_151
LBB4_137:
	subs	w11, w3, #8, lsl #12            ; =32768
	b.lo	LBB4_151
; %bb.138:
	cmp	w5, w8
	b.hs	LBB4_151
; %bb.139:
	add	w9, w5, w3
	ldrb	w9, [x1, w9, uxtw]
	add	x13, x11, w5, uxtw
	ldrb	w13, [x1, x13]
	cmp	w9, w13
	b.ne	LBB4_151
; %bb.140:
	cmp	x10, #8
	b.lo	LBB4_144
; %bb.141:
	mov	w9, #0                          ; =0x0
	add	x10, x1, w11, uxtw
	mov	x11, x8
LBB4_142:                               ; =>This Inner Loop Header: Depth=1
	ldr	x13, [x12, w9, uxtw]
	ldr	x14, [x10, w9, uxtw]
	cmp	x13, x14
	b.ne	LBB4_149
; %bb.143:                              ;   in Loop: Header=BB4_142 Depth=1
	add	w9, w9, #8
	sub	w11, w11, #8
	cmp	w11, #7
	b.hi	LBB4_142
	b	LBB4_145
LBB4_144:
	mov	w9, #0                          ; =0x0
LBB4_145:
	cmp	w9, w8
	b.hs	LBB4_150
LBB4_146:                               ; =>This Inner Loop Header: Depth=1
	add	w10, w3, w9
	ldrb	w11, [x1, w10, uxtw]
	sub	w10, w10, #8, lsl #12           ; =32768
	ldrb	w10, [x1, w10, uxtw]
	cmp	w11, w10
	b.ne	LBB4_150
; %bb.147:                              ;   in Loop: Header=BB4_146 Depth=1
	add	w9, w9, #1
	cmp	w8, w9
	b.ne	LBB4_146
; %bb.148:
	mov	x9, x8
	b	LBB4_150
LBB4_149:
	eor	x8, x14, x13
	rbit	x8, x8
	clz	x8, x8
	add	w9, w9, w8, lsr #3
LBB4_150:
	cmp	w9, #2
	ccmp	w9, w5, #0, hi
	csel	w5, w9, w5, hi
	mov	w8, #32768                      ; =0x8000
	csel	w17, w8, w17, hi
LBB4_151:
	mov	w8, w5
	orr	x0, x8, x17, lsl #32
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB4_152:
	bl	_abort
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev ; -- Begin function _ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev
	.globl	__ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev
	.weak_def_can_be_hidden	__ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev
	.p2align	2
__ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev: ; @_ZNSt3__16vectorIhNS_9allocatorIhEEE20__throw_length_errorB9nqe220108Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh60:
	adrp	x0, l_.str.13@PAGE
Lloh61:
	add	x0, x0, l_.str.13@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB9nqe220108EPKc
	.loh AdrpAdd	Lloh60, Lloh61
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__120__throw_length_errorB9nqe220108EPKc ; -- Begin function _ZNSt3__120__throw_length_errorB9nqe220108EPKc
	.globl	__ZNSt3__120__throw_length_errorB9nqe220108EPKc
	.weak_def_can_be_hidden	__ZNSt3__120__throw_length_errorB9nqe220108EPKc
	.p2align	2
__ZNSt3__120__throw_length_errorB9nqe220108EPKc: ; @_ZNSt3__120__throw_length_errorB9nqe220108EPKc
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	w0, #16                         ; =0x10
	bl	___cxa_allocate_exception
	mov	x19, x0
Ltmp39:                                 ; EH_LABEL
	mov	x1, x20
	bl	__ZNSt12length_errorC1B9nqe220108EPKc
Ltmp40:                                 ; EH_LABEL
; %bb.1:
Lloh62:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh63:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh64:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh65:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB6_2:
Ltmp41:                                 ; EH_LABEL
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh64, Lloh65
	.loh AdrpLdrGot	Lloh62, Lloh63
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table6:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Lfunc_begin2-Lfunc_begin2      ; >> Call Site 1 <<
	.uleb128 Ltmp39-Lfunc_begin2            ;   Call between Lfunc_begin2 and Ltmp39
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp39-Lfunc_begin2            ; >> Call Site 2 <<
	.uleb128 Ltmp40-Ltmp39                  ;   Call between Ltmp39 and Ltmp40
	.uleb128 Ltmp41-Lfunc_begin2            ;     jumps to Ltmp41
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp40-Lfunc_begin2            ; >> Call Site 3 <<
	.uleb128 Lfunc_end2-Ltmp40              ;   Call between Ltmp40 and Lfunc_end2
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end2:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt12length_errorC1B9nqe220108EPKc ; -- Begin function _ZNSt12length_errorC1B9nqe220108EPKc
	.globl	__ZNSt12length_errorC1B9nqe220108EPKc
	.weak_def_can_be_hidden	__ZNSt12length_errorC1B9nqe220108EPKc
	.p2align	2
__ZNSt12length_errorC1B9nqe220108EPKc:  ; @_ZNSt12length_errorC1B9nqe220108EPKc
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	__ZNSt11logic_errorC2EPKc
Lloh66:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh67:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh66, Lloh67
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
__ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy: ; @_ZN12_GLOBAL__N_15timedINS_5ChainEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	.cfi_startproc
; %bb.0:
	stp	d9, d8, [sp, #-112]!            ; 16-byte Folded Spill
	stp	x28, x27, [sp, #16]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x21, x5
	mov	x22, x3
	mov	x23, x2
	mov	x24, x1
	mov	x19, x0
	add	x25, x0, #96, lsl #12           ; =393216
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	cbz	x23, LBB8_27
; %bb.1:
	mov	w26, #0                         ; =0x0
	add	x27, x19, #64, lsl #12          ; =262144
	mov	w28, #258                       ; =0x102
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d8, x8
	mov	x8, #-7378697629483820647       ; =0x9999999999999999
	movk	x8, #39322
	movk	x8, #16297, lsl #48
	fmov	d9, x8
LBB8_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB8_5 Depth 2
                                        ;       Child Loop BB8_15 Depth 3
                                        ;         Child Loop BB8_19 Depth 4
                                        ;         Child Loop BB8_22 Depth 4
	mov	x0, x19
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	mov	w8, #0                          ; =0x0
	mov	w1, #0                          ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
	mov	x10, #0                         ; =0x0
	mov	x9, #0                          ; =0x0
	str	xzr, [x25]
	str	wzr, [x25, #8]
	b	LBB8_5
LBB8_3:                                 ;   in Loop: Header=BB8_5 Depth=2
	mov	x12, #0                         ; =0x0
	add	w10, w8, #1
LBB8_4:                                 ;   in Loop: Header=BB8_5 Depth=2
	mov	w13, w12
	add	x13, x13, x13, lsl #15
	add	x9, x9, x12, lsr #32
	add	x9, x9, x13
	add	w8, w8, #1
	cmp	x23, x10
	b.ls	LBB8_25
LBB8_5:                                 ;   Parent Loop BB8_2 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB8_15 Depth 3
                                        ;         Child Loop BB8_19 Depth 4
                                        ;         Child Loop BB8_22 Depth 4
	mov	w13, w8
	sub	x14, x23, x13
	cmp	x14, #258
	csel	x12, x14, x28, lo
	cmp	x14, #2
	b.ls	LBB8_3
; %bb.6:                                ;   in Loop: Header=BB8_5 Depth=2
	ldrb	w3, [x22, x10]
	add	x14, x24, x13
	add	w10, w8, #1
	ldrb	w13, [x24, x10]
	add	w15, w8, #2
	ldrb	w15, [x24, w15, uxtw]
	ldrb	w17, [x14]
	ubfiz	x17, x17, #12, #4
	eor	x13, x17, x13, lsl #6
	eor	x13, x13, x15
	ldr	w15, [x19, x13, lsl #2]
	subs	w17, w8, #8, lsl #12            ; =32768
	csel	w17, wzr, w17, lo
	sub	w2, w8, w16
	subs	w0, w11, w2
	csel	w0, wzr, w0, lo
	cbz	w3, LBB8_10
; %bb.7:                                ;   in Loop: Header=BB8_5 Depth=2
	cmn	w15, #1
	ccmp	w15, w17, #0, ne
	b.hs	LBB8_11
; %bb.8:                                ;   in Loop: Header=BB8_5 Depth=2
	mov	w11, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
LBB8_9:                                 ;   in Loop: Header=BB8_5 Depth=2
	and	w12, w8, #0x7fff
	str	w15, [x27, w12, uxtw #2]
	str	w8, [x19, x13, lsl #2]
	sub	w1, w8, w16
	stp	w8, w1, [x25]
	str	w11, [x25, #8]
	mov	w12, w11
	orr	x12, x12, x16, lsl #32
	mov	x16, x8
	b	LBB8_4
LBB8_10:                                ;   in Loop: Header=BB8_5 Depth=2
	mov	x12, #0                         ; =0x0
	and	w14, w8, #0x7fff
	str	w15, [x27, w14, uxtw #2]
	str	w8, [x19, x13, lsl #2]
	b	LBB8_4
LBB8_11:                                ;   in Loop: Header=BB8_5 Depth=2
	mov	w16, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	add	w1, w1, w2
	mov	x2, x15
	b	LBB8_15
LBB8_12:                                ;   in Loop: Header=BB8_15 Depth=3
	eor	x4, x7, x6
	rbit	x4, x4
	clz	x4, x4
	add	w3, w3, w4, lsr #3
LBB8_13:                                ;   in Loop: Header=BB8_15 Depth=3
	sub	w4, w8, w2
	cmp	w3, #2
	ccmp	w3, w11, #0, hi
	csel	w11, w3, w11, hi
	csel	w16, w4, w16, hi
	cmp	w3, w12
	b.eq	LBB8_9
LBB8_14:                                ;   in Loop: Header=BB8_15 Depth=3
	and	w2, w2, #0x7fff
	ldr	w2, [x27, w2, uxtw #2]
	cmn	w2, #1
	ccmp	w2, w17, #0, ne
	b.lo	LBB8_9
LBB8_15:                                ;   Parent Loop BB8_2 Depth=1
                                        ;     Parent Loop BB8_5 Depth=2
                                        ; =>    This Loop Header: Depth=3
                                        ;         Child Loop BB8_19 Depth 4
                                        ;         Child Loop BB8_22 Depth 4
	cmp	w11, #3
	b.lo	LBB8_17
; %bb.16:                               ;   in Loop: Header=BB8_15 Depth=3
	add	w3, w11, w8
	ldrb	w3, [x24, w3, uxtw]
	add	w4, w2, w11
	ldrb	w4, [x24, w4, uxtw]
	cmp	w3, w4
	b.ne	LBB8_14
LBB8_17:                                ;   in Loop: Header=BB8_15 Depth=3
	cmp	w2, w1
	csel	w3, w0, wzr, eq
	sub	w4, w12, w3
	cmp	w4, #8
	b.lo	LBB8_21
; %bb.18:                               ;   in Loop: Header=BB8_15 Depth=3
	add	x5, x24, w2, uxtw
LBB8_19:                                ;   Parent Loop BB8_2 Depth=1
                                        ;     Parent Loop BB8_5 Depth=2
                                        ;       Parent Loop BB8_15 Depth=3
                                        ; =>      This Inner Loop Header: Depth=4
	ldr	x6, [x14, w3, uxtw]
	ldr	x7, [x5, w3, uxtw]
	cmp	x6, x7
	b.ne	LBB8_12
; %bb.20:                               ;   in Loop: Header=BB8_19 Depth=4
	add	w3, w3, #8
	sub	w4, w4, #8
	cmp	w4, #7
	b.hi	LBB8_19
LBB8_21:                                ;   in Loop: Header=BB8_15 Depth=3
	cmp	w3, w12
	b.hs	LBB8_13
LBB8_22:                                ;   Parent Loop BB8_2 Depth=1
                                        ;     Parent Loop BB8_5 Depth=2
                                        ;       Parent Loop BB8_15 Depth=3
                                        ; =>      This Inner Loop Header: Depth=4
	add	w4, w8, w3
	ldrb	w4, [x24, w4, uxtw]
	add	w5, w2, w3
	ldrb	w5, [x24, w5, uxtw]
	cmp	w4, w5
	b.ne	LBB8_13
; %bb.23:                               ;   in Loop: Header=BB8_22 Depth=4
	add	w3, w3, #1
	cmp	w12, w3
	b.ne	LBB8_22
; %bb.24:                               ;   in Loop: Header=BB8_15 Depth=3
	mov	x3, x12
	b	LBB8_13
LBB8_25:                                ;   in Loop: Header=BB8_2 Depth=1
	cmp	x9, x21
	b.ne	LBB8_32
; %bb.26:                               ;   in Loop: Header=BB8_2 Depth=1
	add	w26, w26, #1
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	scvtf	d0, x8
	fdiv	d0, d0, d8
	fcmp	d0, d9
	b.mi	LBB8_2
	b	LBB8_30
LBB8_27:
	cbnz	x21, LBB8_31
; %bb.28:
	mov	w26, #0                         ; =0x0
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d8, x8
	mov	x8, #-7378697629483820647       ; =0x9999999999999999
	movk	x8, #39322
	movk	x8, #16297, lsl #48
	fmov	d9, x8
LBB8_29:                                ; =>This Inner Loop Header: Depth=1
	mov	x0, x19
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	str	xzr, [x25]
	add	w26, w26, #1
	str	wzr, [x25, #8]
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	scvtf	d0, x8
	fdiv	d0, d0, d8
	fcmp	d0, d9
	b.mi	LBB8_29
LBB8_30:
	ucvtf	d1, w26
	fdiv	d0, d0, d1
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
LBB8_31:
	mov	x0, x19
	mov	w1, #255                        ; =0xff
	mov	w2, #262144                     ; =0x40000
	bl	_memset
	str	xzr, [x25]
	str	wzr, [x25, #8]
LBB8_32:
	bl	_abort
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
__ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy: ; @_ZN12_GLOBAL__N_15timedINS_5RadixEEEdRT_NSt3__14spanIKhLm18446744073709551615EEES7_NS5_IKNS_5MatchELm18446744073709551615EEEy
	.cfi_startproc
; %bb.0:
	stp	d9, d8, [sp, #-112]!            ; 16-byte Folded Spill
	stp	x28, x27, [sp, #16]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	mov	x21, x5
	mov	x22, x3
	mov	x23, x2
	mov	x24, x1
	mov	x19, x0
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	mov	x20, x0
	mov	w28, #65524                     ; =0xfff4
	movk	w28, #5, lsl #16
	mov	w27, #65524                     ; =0xfff4
	movk	w27, #8, lsl #16
	add	x8, x19, #175, lsl #12          ; =716800
	add	x25, x8, #4092
	cbz	x23, LBB9_6
; %bb.1:
	add	x9, x19, #175, lsl #12          ; =716800
	mov	w26, #0                         ; =0x0
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d8, x8
	mov	x8, #-7378697629483820647       ; =0x9999999999999999
	movk	x8, #39322
	movk	x8, #16297, lsl #48
	fmov	d9, x8
LBB9_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB9_3 Depth 2
	add	x8, x9, #4084
	mov	w9, #-1                         ; =0xffffffff
	str	x9, [x8]
	add	x0, x19, x28
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	add	x0, x19, x27
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	mov	x8, #0                          ; =0x0
	mov	x28, #0                         ; =0x0
	stp	xzr, xzr, [x25]
	mov	w27, #1                         ; =0x1
LBB9_3:                                 ;   Parent Loop BB9_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrb	w8, [x22, x8]
	cmp	w8, #0
	cset	w4, ne
	sub	w3, w27, #1
	mov	x0, x19
	mov	x1, x24
	mov	x2, x23
	bl	__ZN12_GLOBAL__N_15Radix7advanceILb0EEENS_5MatchENSt3__14spanIKhLm18446744073709551615EEEjbRNS_4WorkE
	mov	w8, w0
	add	x8, x8, x8, lsl #15
	add	x9, x28, x0, lsr #32
	add	x28, x9, x8
	mov	w8, w27
	add	w27, w27, #1
	cmp	x23, x8
	b.hi	LBB9_3
; %bb.4:                                ;   in Loop: Header=BB9_2 Depth=1
	cmp	x28, x21
	b.ne	LBB9_11
; %bb.5:                                ;   in Loop: Header=BB9_2 Depth=1
	add	w26, w26, #1
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	scvtf	d0, x8
	fdiv	d0, d0, d8
	fcmp	d0, d9
	mov	w27, #65524                     ; =0xfff4
	movk	w27, #8, lsl #16
	mov	w28, #65524                     ; =0xfff4
	movk	w28, #5, lsl #16
	add	x9, x19, #175, lsl #12          ; =716800
	b.mi	LBB9_2
	b	LBB9_9
LBB9_6:
	add	x8, x19, #175, lsl #12          ; =716800
	cbnz	x21, LBB9_10
; %bb.7:
	mov	w26, #0                         ; =0x0
	mov	w21, #-1                        ; =0xffffffff
	mov	x8, #225833675390976            ; =0xcd6500000000
	movk	x8, #16845, lsl #48
	fmov	d8, x8
	mov	x8, #-7378697629483820647       ; =0x9999999999999999
	movk	x8, #39322
	movk	x8, #16297, lsl #48
	fmov	d9, x8
	add	x22, x19, #175, lsl #12         ; =716800
LBB9_8:                                 ; =>This Inner Loop Header: Depth=1
	add	x8, x22, #4084
	str	x21, [x8]
	add	x0, x19, x28
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	add	x0, x19, x27
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	stp	xzr, xzr, [x25]
	add	w26, w26, #1
	bl	__ZNSt3__16chrono12steady_clock3nowEv
	sub	x8, x0, x20
	scvtf	d0, x8
	fdiv	d0, d0, d8
	fcmp	d0, d9
	b.mi	LBB9_8
LBB9_9:
	ucvtf	d1, w26
	fdiv	d0, d0, d1
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
LBB9_10:
	add	x8, x8, #4084
	mov	w9, #-1                         ; =0xffffffff
	str	x9, [x8]
	add	x0, x19, x28
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	add	x0, x19, x27
	mov	w1, #255                        ; =0xff
	mov	w2, #131072                     ; =0x20000
	bl	_memset
	stp	xzr, xzr, [x25]
LBB9_11:
	bl	_abort
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__16vectorIN12_GLOBAL__N_15MatchENS_9allocatorIS2_EEE20__throw_length_errorB9nqe220108Ev
__ZNSt3__16vectorIN12_GLOBAL__N_15MatchENS_9allocatorIS2_EEE20__throw_length_errorB9nqe220108Ev: ; @_ZNSt3__16vectorIN12_GLOBAL__N_15MatchENS_9allocatorIS2_EEE20__throw_length_errorB9nqe220108Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh68:
	adrp	x0, l_.str.13@PAGE
Lloh69:
	add	x0, x0, l_.str.13@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB9nqe220108EPKc
	.loh AdrpAdd	Lloh68, Lloh69
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"chain_index_bytes=%zu tree_index_bytes=%zu\n"

l_.str.1:                               ; @.str.1
	.asciz	"--verify-only"

l_.str.2:                               ; @.str.2
	.asciz	"workload,schedule,bytes,queries,chain_seconds,tree_seconds,tree_speed_ratio,ratio_min,ratio_max,chain_visits,tree_visits,chain_compared,tree_compared,chain_writes,tree_writes,chain_reused,tree_reused"

l_.str.3:                               ; @.str.3
	.asciz	"entropy"

l_.str.4:                               ; @.str.4
	.asciz	"uniform"

l_.str.5:                               ; @.str.5
	.asciz	"late-mismatch"

l_.str.6:                               ; @.str.6
	.asciz	"crowded-buckets"

	.section	__DATA,__const
	.p2align	3, 0x0                          ; @__const.main.names
l___const.main.names:
	.quad	l_.str.3
	.quad	l_.str.4
	.quad	l_.str.5
	.quad	l_.str.6

	.section	__TEXT,__cstring,cstring_literals
l_.str.7:                               ; @.str.7
	.asciz	"--files-only"

l_.str.8:                               ; @.str.8
	.asciz	"--greedy-only"

l_.str.9:                               ; @.str.9
	.asciz	"--random-only"

l_.str.10:                              ; @.str.10
	.asciz	"rb"

l_.str.11:                              ; @.str.11
	.asciz	"boundary_fixtures=1052 exact_answers=PASS\n"

l_.str.12:                              ; @.str.12
	.asciz	"fixture disagreement bytes=%zu position=%u actual=%u/%u expected=%u/%u\n"

l_.str.13:                              ; @.str.13
	.asciz	"vector"

l_.str.14:                              ; @.str.14
	.asciz	"radix_work workload=%s schedule=%s root_insertions=%llu branch_insertions=%llu duplicates=%llu root_expirations=%llu branch_expirations=%llu predictor_rejections=%llu directory_probes=%llu\n"

l_.str.15:                              ; @.str.15
	.asciz	"every-byte"

l_.str.16:                              ; @.str.16
	.asciz	"greedy"

l_.str.17:                              ; @.str.17
	.asciz	"%s,%s,%zu,%u,%.9f,%.9f,%.4f,%.4f,%.4f,%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu\n"

l_.str.18:                              ; @.str.18
	.asciz	"match disagreement position=%u actual=%u/%u expected=%u/%u\n"

.subsections_via_symbols
