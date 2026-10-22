	.build_version macos, 26, 0	sdk_version 26, 2
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE ; -- Begin function _ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	.p2align	2
__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE: ; @_ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1]
	cmp	w8, #1
	b.hi	LBB0_4
; %bb.1:
	ldr	w8, [x1, #4]
	and	w9, w8, #0xfffffff8
	ldp	x10, x11, [x1, #8]
	ldr	x12, [x1, #24]
	mov	w8, #1                          ; =0x1
	cmp	w9, #8
	ccmn	x10, #1, #4, eq
	ccmn	x11, #1, #4, ne
	ccmn	x12, #1, #4, ne
	b.eq	LBB0_3
; %bb.2:
	mov	w9, #38104                      ; =0x94d8
	add	x9, x0, x9
	mov	w10, #32848                     ; =0x8050
	ldp	q0, q1, [x1]
	stp	q0, q1, [x0]
	ldr	w11, [x1]
	cmp	w11, #1
	mov	w11, #3                         ; =0x3
	csinc	w11, w11, wzr, eq
	str	w11, [x0, #32]
	str	wzr, [x0, #60]
	movi.2d	v0, #0000000000000000
	stur	q0, [x0, #44]
	stur	xzr, [x0, #36]
	str	q0, [x0, #32832]
	ldr	w11, [x1, #4]
	lsl	x11, x8, x11
	str	x11, [x0, x10]
	str	w8, [x9]
	mov	w10, #38108                     ; =0x94dc
	add	x10, x0, x10
	stp	q0, q0, [x10]
	str	q0, [x10, #32]
	stur	q0, [x10, #45]
	str	w8, [x9, #68]
	str	q0, [x0, #38176]
	mov	w8, #0                          ; =0x0
LBB0_3:
	mov	x0, x8
	ret
LBB0_4:
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE ; -- Begin function _ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE
	.p2align	2
__ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE: ; @_ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE
	.cfi_startproc
; %bb.0:
	mov	w8, #19                         ; =0x13
	stp	w8, w1, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream6chargeEy
lCPI2_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream6chargeEy
	.p2align	2
__ZN9gui_forms11compression13InflateStream6chargeEy: ; @_ZN9gui_forms11compression13InflateStream6chargeEy
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #24]
	ldr	x9, [x0, #56]
	sub	x8, x8, x9
	cmp	x1, x8
	b.hi	LBB2_2
; %bb.1:
	add	x9, x9, x1
	str	x9, [x0, #56]
	cmp	x1, x8
	cset	w0, ls
	ret
LBB2_2:
Lloh0:
	adrp	x9, lCPI2_0@PAGE
Lloh1:
	ldr	d0, [x9, lCPI2_0@PAGEOFF]
	str	d0, [x0, #32]
	cmp	x1, x8
	cset	w0, ls
	ret
	.loh AdrpLdr	Lloh0, Lloh1
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
lCPI3_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI3_1:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI3_2:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
	.p2align	2
__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj: ; @_ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
	.cfi_startproc
; %bb.0:
	add	x8, x0, #9, lsl #12             ; =36864
	ldr	w10, [x8, #1260]
	cmp	w10, w2
	b.hs	LBB3_8
; %bb.1:
	mov	x11, #0                         ; =0x0
	ldr	x17, [x0, #8]
	ldr	x9, [x0, #24]
	ldp	x4, x5, [x1]
	ldr	x12, [x1, #32]
	ldr	x14, [x0, #40]
	ldr	x13, [x0, #56]
	ldr	w15, [x8, #1256]
	sub	x16, x13, x9
	sub	x17, x14, x17
	add	x4, x4, x12
	sub	x5, x12, x5
LBB3_2:                                 ; =>This Inner Loop Header: Depth=1
	cmn	x5, x11
	b.eq	LBB3_10
; %bb.3:                                ;   in Loop: Header=BB3_2 Depth=1
	add	x6, x17, x11
	cbz	x6, LBB3_13
; %bb.4:                                ;   in Loop: Header=BB3_2 Depth=1
	add	x6, x16, x11
	cbz	x6, LBB3_9
; %bb.5:                                ;   in Loop: Header=BB3_2 Depth=1
	add	x6, x13, x11
	add	x6, x6, #1
	str	x6, [x0, #56]
	ldrb	w6, [x4, x11]
	lsl	w6, w6, w10
	orr	w15, w6, w15
	str	w15, [x8, #1256]
	add	w10, w10, #8
	str	w10, [x8, #1260]
	add	x6, x12, x11
	add	x6, x6, #1
	str	x6, [x1, #32]
	add	x6, x14, x11
	add	x6, x6, #1
	str	x6, [x0, #40]
	add	x11, x11, #1
	cmp	w10, w2
	b.lo	LBB3_2
; %bb.6:
	add	x10, x13, x11
	cmp	x9, x10
	b.eq	LBB3_9
LBB3_7:
	add	x9, x10, #1
	str	x9, [x0, #56]
	mov	w9, #-1                         ; =0xffffffff
	lsl	w9, w9, w2
	ldr	w10, [x8, #1256]
	bic	w9, w10, w9
	str	w9, [x3]
	ldr	w9, [x8, #1256]
	lsr	w9, w9, w2
	str	w9, [x8, #1256]
	ldr	w9, [x8, #1260]
	sub	w9, w9, w2
	str	w9, [x8, #1260]
	mov	w0, #1                          ; =0x1
	ret
LBB3_8:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.ne	LBB3_7
LBB3_9:
Lloh2:
	adrp	x9, lCPI3_0@PAGE
Lloh3:
	ldr	d0, [x9, lCPI3_0@PAGEOFF]
	str	d0, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB3_10:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB3_12
; %bb.11:
Lloh4:
	adrp	x8, lCPI3_2@PAGE
Lloh5:
	ldr	d0, [x8, lCPI3_2@PAGEOFF]
	str	d0, [x0, #32]
LBB3_12:
	str	wzr, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB3_13:
Lloh6:
	adrp	x9, lCPI3_1@PAGE
Lloh7:
	ldr	d0, [x9, lCPI3_1@PAGEOFF]
	str	d0, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
	.loh AdrpLdr	Lloh2, Lloh3
	.loh AdrpLdr	Lloh4, Lloh5
	.loh AdrpLdr	Lloh6, Lloh7
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
lCPI4_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI4_1:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI4_2:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
lCPI4_4:
	.long	19                              ; 0x13
	.long	18                              ; 0x12
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0
lCPI4_3:
	.long	1                               ; 0x1
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.p2align	2
__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj: ; @_ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.cfi_startproc
; %bb.0:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	add	x8, x0, #9, lsl #12             ; =36864
	ldr	w10, [x8, #1240]
	cmp	w10, #1
	b.ne	LBB4_3
; %bb.1:
	ldr	w9, [x8, #1260]
	mov	w11, #16                        ; =0x10
	sub	w11, w11, w9
	lsr	w15, w11, #3
	ldr	x4, [x1, #8]
	ldr	x14, [x1, #32]
	sub	x11, x4, x14
	cmp	x11, x15
	b.lo	LBB4_3
; %bb.2:
	ldr	x6, [x0, #8]
	ldr	x17, [x0, #40]
	sub	x11, x6, x17
	cmp	x11, x15
	b.hs	LBB4_14
LBB4_3:
	ldr	w11, [x2, #1632]
	cmp	w10, w11
	b.ls	LBB4_5
LBB4_4:
	mov	w8, #0                          ; =0x0
Lloh8:
	adrp	x9, lCPI4_4@PAGE
Lloh9:
	ldr	d0, [x9, lCPI4_4@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_5:
	add	x9, x8, #1240
	ldr	x12, [x0, #24]
	ldr	x13, [x1, #8]
	mov	w14, #8                         ; =0x8
	ldr	w15, [x8, #1260]
LBB4_6:                                 ; =>This Inner Loop Header: Depth=1
	cbz	w15, LBB4_8
; %bb.7:                                ;   in Loop: Header=BB4_6 Depth=1
	ldr	x17, [x0, #56]
	sub	w15, w15, #1
	cmp	x12, x17
	b.ne	LBB4_12
	b	LBB4_15
LBB4_8:                                 ;   in Loop: Header=BB4_6 Depth=1
	ldr	x15, [x1, #32]
	cmp	x15, x13
	b.eq	LBB4_22
; %bb.9:                                ;   in Loop: Header=BB4_6 Depth=1
	ldr	x4, [x0, #40]
	ldr	x16, [x0, #8]
	cmp	x4, x16
	b.eq	LBB4_26
; %bb.10:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	x16, [x0, #24]
	ldr	x17, [x0, #56]
	cmp	x16, x17
	b.eq	LBB4_15
; %bb.11:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	x5, [x1]
	ldr	w6, [x8, #1256]
	add	x17, x17, #1
	str	x17, [x0, #56]
	ldrb	w5, [x5, x15]
	orr	w5, w6, w5
	str	w5, [x8, #1256]
	str	w14, [x8, #1260]
	add	x15, x15, #1
	str	x15, [x1, #32]
	add	x15, x4, #1
	str	x15, [x0, #40]
	mov	w15, #7                         ; =0x7
	cmp	x16, x17
	b.eq	LBB4_15
LBB4_12:                                ;   in Loop: Header=BB4_6 Depth=1
	add	x16, x17, #1
	str	x16, [x0, #56]
	ldr	w16, [x8, #1256]
	and	w17, w16, #0x1
	lsr	w16, w16, #1
	str	w16, [x8, #1256]
	str	w15, [x8, #1260]
	ldr	w16, [x8, #1244]
	orr	w16, w16, w17
	ldrh	w17, [x2, w10, uxtw #1]
	ldr	w5, [x8, #1248]
	add	w4, w5, w17
	subs	w5, w16, w5
	ccmp	w16, w4, #2, hs
	b.lo	LBB4_16
; %bb.13:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	w5, [x8, #1252]
	add	w17, w5, w17
	str	w17, [x8, #1252]
	lsl	w17, w4, #1
	str	w17, [x8, #1248]
	lsl	w16, w16, #1
	str	w16, [x8, #1244]
	add	w10, w10, #1
	str	w10, [x8, #1240]
	cmp	w10, w11
	b.ls	LBB4_6
	b	LBB4_4
LBB4_14:
	ldr	x11, [x0, #24]
	ldr	x5, [x0, #56]
	cmp	x11, x5
	b.ne	LBB4_17
LBB4_15:
	mov	w8, #0                          ; =0x0
Lloh10:
	adrp	x9, lCPI4_0@PAGE
Lloh11:
	ldr	d0, [x9, lCPI4_0@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_16:
	ldr	w8, [x8, #1252]
	add	w8, w5, w8
	add	x8, x2, w8, uxtw #1
	ldrh	w8, [x8, #32]
	str	w8, [x3]
Lloh12:
	adrp	x8, lCPI4_3@PAGE
Lloh13:
	ldr	q0, [x8, lCPI4_3@PAGEOFF]
	str	q0, [x9]
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_17:
	add	x13, x5, #1
	str	x13, [x0, #56]
	ldr	w12, [x8, #1256]
	ldr	x7, [x1]
	add	x19, x7, x14
	ldrb	w16, [x19]
	lsl	w16, w16, w9
	orr	w16, w16, w12
	cmp	w15, #2
	b.ne	LBB4_19
; %bb.18:
	ldrb	w15, [x19, #1]
	add	w19, w9, #8
	lsl	w15, w15, w19
	orr	w16, w15, w16
LBB4_19:
	and	w15, w16, #0x1ff
	add	x15, x2, w15, uxtw #1
	ldrh	w15, [x15, #608]
	ands	w16, w15, #0xf
	b.eq	LBB4_3
; %bb.20:
	cmp	w9, w16
	b.hs	LBB4_38
; %bb.21:
	cmp	x4, x14
	b.ne	LBB4_25
LBB4_22:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB4_24
; %bb.23:
Lloh14:
	adrp	x8, lCPI4_2@PAGE
Lloh15:
	ldr	d0, [x8, lCPI4_2@PAGEOFF]
	str	d0, [x0, #32]
LBB4_24:
	mov	w8, #0                          ; =0x0
	str	wzr, [x1, #52]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_25:
	cmp	x6, x17
	b.ne	LBB4_27
LBB4_26:
	mov	w8, #0                          ; =0x0
Lloh16:
	adrp	x9, lCPI4_1@PAGE
Lloh17:
	ldr	d0, [x9, lCPI4_1@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_27:
	cmp	x11, x13
	b.eq	LBB4_15
; %bb.28:
	add	x13, x5, #2
	str	x13, [x0, #56]
	ldrb	w10, [x7, x14]
	lsl	w10, w10, w9
	orr	w12, w10, w12
	str	w12, [x8, #1256]
	add	w10, w9, #8
	str	w10, [x8, #1260]
	add	x2, x14, #1
	str	x2, [x1, #32]
	add	x19, x17, #1
	str	x19, [x0, #40]
	cmp	w10, w16
	b.hs	LBB4_37
; %bb.29:
	cmp	x2, x4
	b.eq	LBB4_22
; %bb.30:
	cmp	x19, x6
	b.eq	LBB4_26
; %bb.31:
	cmp	x11, x13
	b.eq	LBB4_15
; %bb.32:
	add	x13, x5, #3
	str	x13, [x0, #56]
	ldrb	w2, [x7, x2]
	lsl	w10, w2, w10
	orr	w12, w10, w12
	str	w12, [x8, #1256]
	add	w10, w9, #16
	str	w10, [x8, #1260]
	add	x2, x14, #2
	str	x2, [x1, #32]
	add	x19, x17, #2
	str	x19, [x0, #40]
	cmp	w10, w16
	b.hs	LBB4_40
; %bb.33:
	cmp	x2, x4
	b.eq	LBB4_22
; %bb.34:
	cmp	x19, x6
	b.eq	LBB4_26
; %bb.35:
	cmp	x11, x13
	b.eq	LBB4_15
; %bb.36:
	add	x13, x5, #4
	str	x13, [x0, #56]
	ldrb	w2, [x7, x2]
	lsl	w10, w2, w10
	orr	w12, w10, w12
	str	w12, [x8, #1256]
	add	w9, w9, #24
	str	w9, [x8, #1260]
	add	x10, x14, #3
	str	x10, [x1, #32]
	add	x10, x17, #3
	str	x10, [x0, #40]
	cmp	x11, x13
	b.eq	LBB4_15
	b	LBB4_39
LBB4_37:
	mov	x9, x10
LBB4_38:
	cmp	x11, x13
	b.eq	LBB4_15
LBB4_39:
	add	x10, x13, #1
	str	x10, [x0, #56]
	lsr	w10, w12, w16
	str	w10, [x8, #1256]
	sub	w9, w9, w16
	str	w9, [x8, #1260]
	lsr	w8, w15, #4
	str	w8, [x3]
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_40:
	mov	x9, x10
	cmp	x11, x13
	b.eq	LBB4_15
	b	LBB4_39
	.loh AdrpLdr	Lloh8, Lloh9
	.loh AdrpLdr	Lloh10, Lloh11
	.loh AdrpLdr	Lloh12, Lloh13
	.loh AdrpLdr	Lloh14, Lloh15
	.loh AdrpLdr	Lloh16, Lloh17
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
lCPI5_0:
	.long	19                              ; 0x13
	.long	15                              ; 0xf
lCPI5_1:
	.long	19                              ; 0x13
	.long	14                              ; 0xe
lCPI5_2:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	.p2align	2
__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb: ; @_ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #128
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	mov	x22, x0
	mov	w8, #11                         ; =0xb
	mov	x9, #1088                       ; =0x440
	madd	x8, x3, x8, x9
	ldr	x10, [x0, #24]
	ldr	x9, [x0, #56]
	sub	x10, x10, x9
	cmp	x8, x10
	b.ls	LBB5_2
; %bb.1:
	mov	w0, #0                          ; =0x0
Lloh18:
	adrp	x8, lCPI5_2@PAGE
Lloh19:
	ldr	d0, [x8, lCPI5_2@PAGEOFF]
	b	LBB5_26
LBB5_2:
	mov	x23, x4
	mov	x21, x3
	mov	x20, x2
	mov	x19, x1
	add	x8, x9, x8
	str	x8, [x22, #56]
	add	x24, x1, #608
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x1]
	add	x0, x1, #608
	mov	w1, #1028                       ; =0x404
	bl	_bzero
	cbz	x21, LBB5_24
; %bb.3:
	mov	x9, #0                          ; =0x0
	mov	w8, #0                          ; =0x0
	b	LBB5_5
LBB5_4:                                 ;   in Loop: Header=BB5_5 Depth=1
	add	x9, x9, #1
	cmp	x21, x9
	b.eq	LBB5_7
LBB5_5:                                 ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x20, x9]
	ldrh	w11, [x19, x10, lsl #1]
	add	w11, w11, #1
	strh	w11, [x19, x10, lsl #1]
	cmp	w8, w10
	b.hs	LBB5_4
; %bb.6:                                ;   in Loop: Header=BB5_5 Depth=1
	str	w10, [x19, #1632]
	mov	x8, x10
	b	LBB5_4
LBB5_7:
	cbz	w8, LBB5_24
; %bb.8:
	ldrh	w9, [x19, #2]
	cmp	w9, #2
	b.hi	LBB5_23
; %bb.9:
	ldrh	w10, [x19, #4]
	add	w12, w10, w9, lsl #1
	cmp	w12, #4
	b.hi	LBB5_23
; %bb.10:
	ldrh	w11, [x19, #6]
	add	w13, w11, w12, lsl #1
	cmp	w13, #8
	b.hi	LBB5_23
; %bb.11:
	ldrh	w12, [x19, #8]
	add	w14, w12, w13, lsl #1
	cmp	w14, #16
	b.hi	LBB5_23
; %bb.12:
	ldrh	w13, [x19, #10]
	add	w15, w13, w14, lsl #1
	cmp	w15, #32
	b.hi	LBB5_23
; %bb.13:
	ldrh	w14, [x19, #12]
	add	w15, w14, w15, lsl #1
	cmp	w15, #64
	b.hi	LBB5_23
; %bb.14:
	ldrh	w16, [x19, #14]
	add	w15, w16, w15, lsl #1
	cmp	w15, #128
	b.hi	LBB5_23
; %bb.15:
	ldrh	w17, [x19, #16]
	add	w15, w17, w15, lsl #1
	cmp	w15, #256
	b.hi	LBB5_23
; %bb.16:
	ldrh	w0, [x19, #18]
	add	w15, w0, w15, lsl #1
	cmp	w15, #512
	b.hi	LBB5_23
; %bb.17:
	ldrh	w1, [x19, #20]
	add	w15, w1, w15, lsl #1
	cmp	w15, #1024
	b.hi	LBB5_23
; %bb.18:
	ldrh	w2, [x19, #22]
	add	w15, w2, w15, lsl #1
	cmp	w15, #2048
	b.hi	LBB5_23
; %bb.19:
	ldrh	w3, [x19, #24]
	add	w15, w3, w15, lsl #1
	cmp	w15, #1, lsl #12                ; =4096
	b.hi	LBB5_23
; %bb.20:
	ldrh	w4, [x19, #26]
	add	w15, w4, w15, lsl #1
	cmp	w15, #2, lsl #12                ; =8192
	b.hi	LBB5_23
; %bb.21:
	ldrh	w5, [x19, #28]
	add	w15, w5, w15, lsl #1
	cmp	w15, #4, lsl #12                ; =16384
	b.hi	LBB5_23
; %bb.22:
	ldrh	w6, [x19, #30]
	add	w15, w6, w15, lsl #1
	cmp	w15, #8, lsl #12                ; =32768
	b.ls	LBB5_27
LBB5_23:
	mov	w0, #0                          ; =0x0
Lloh20:
	adrp	x8, lCPI5_1@PAGE
Lloh21:
	ldr	d0, [x8, lCPI5_1@PAGEOFF]
	b	LBB5_26
LBB5_24:
	cbz	w23, LBB5_65
LBB5_25:
	mov	w0, #0                          ; =0x0
Lloh22:
	adrp	x8, lCPI5_0@PAGE
Lloh23:
	ldr	d0, [x8, lCPI5_0@PAGEOFF]
LBB5_26:
	str	d0, [x22, #32]
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #128
	ret
LBB5_27:
	cmp	w8, #1
	csinc	w6, w23, wzr, eq
	cmp	w15, #8, lsl #12                ; =32768
	b.eq	LBB5_29
; %bb.28:
	tbnz	w6, #0, LBB5_25
LBB5_29:
	mov	x15, #0                         ; =0x0
	str	xzr, [sp]
	add	w10, w9, w10
	add	w11, w10, w11
	stp	w9, w10, [sp, #8]
	add	w9, w11, w12
	add	w10, w9, w13
	stp	w11, w9, [sp, #16]
	add	w9, w10, w14
	add	w11, w9, w16
	stp	w10, w9, [sp, #24]
	add	w9, w11, w17
	add	w10, w9, w0
	stp	w11, w9, [sp, #32]
	add	w9, w10, w1
	add	w11, w9, w2
	stp	w10, w9, [sp, #40]
	add	w9, w11, w3
	add	w10, w9, w4
	stp	w11, w9, [sp, #48]
	add	w9, w10, w5
	stp	w10, w9, [sp, #56]
	add	x9, x19, #32
	and	x10, x21, #0x7
	cmp	x21, #8
	b.lo	LBB5_50
; %bb.30:
	and	x11, x21, #0xfffffffffffffff8
	neg	x12, x11
	mov	w11, #3                         ; =0x3
	mov	x13, sp
	b	LBB5_32
LBB5_31:                                ;   in Loop: Header=BB5_32 Depth=1
	add	x11, x11, #8
	add	x14, x12, x11
	cmp	x14, #3
	b.eq	LBB5_48
LBB5_32:                                ; =>This Inner Loop Header: Depth=1
	add	x14, x20, x11
	ldurb	w15, [x14, #-3]
	cbz	x15, LBB5_34
; %bb.33:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w16, [x13, x15, lsl #2]
	sub	x17, x11, #3
	strh	w17, [x9, x16, lsl #1]
	add	w16, w16, #1
	str	w16, [x13, x15, lsl #2]
LBB5_34:                                ;   in Loop: Header=BB5_32 Depth=1
	ldurb	w14, [x14, #-2]
	cbz	x14, LBB5_36
; %bb.35:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w15, [x13, x14, lsl #2]
	sub	x16, x11, #2
	strh	w16, [x9, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x13, x14, lsl #2]
LBB5_36:                                ;   in Loop: Header=BB5_32 Depth=1
	add	x14, x20, x11
	ldurb	w14, [x14, #-1]
	cbz	x14, LBB5_38
; %bb.37:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w15, [x13, x14, lsl #2]
	sub	x16, x11, #1
	strh	w16, [x9, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x13, x14, lsl #2]
LBB5_38:                                ;   in Loop: Header=BB5_32 Depth=1
	ldrb	w14, [x20, x11]
	cbz	x14, LBB5_40
; %bb.39:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w15, [x13, x14, lsl #2]
	strh	w11, [x9, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x13, x14, lsl #2]
LBB5_40:                                ;   in Loop: Header=BB5_32 Depth=1
	add	x14, x20, x11
	ldrb	w15, [x14, #1]
	cbz	x15, LBB5_42
; %bb.41:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w16, [x13, x15, lsl #2]
	add	x17, x11, #1
	strh	w17, [x9, x16, lsl #1]
	add	w16, w16, #1
	str	w16, [x13, x15, lsl #2]
LBB5_42:                                ;   in Loop: Header=BB5_32 Depth=1
	ldrb	w14, [x14, #2]
	cbz	x14, LBB5_44
; %bb.43:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w15, [x13, x14, lsl #2]
	add	x16, x11, #2
	strh	w16, [x9, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x13, x14, lsl #2]
LBB5_44:                                ;   in Loop: Header=BB5_32 Depth=1
	add	x14, x20, x11
	ldrb	w15, [x14, #3]
	cbz	x15, LBB5_46
; %bb.45:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w16, [x13, x15, lsl #2]
	add	x17, x11, #3
	strh	w17, [x9, x16, lsl #1]
	add	w16, w16, #1
	str	w16, [x13, x15, lsl #2]
LBB5_46:                                ;   in Loop: Header=BB5_32 Depth=1
	ldrb	w14, [x14, #4]
	cbz	x14, LBB5_31
; %bb.47:                               ;   in Loop: Header=BB5_32 Depth=1
	ldr	w15, [x13, x14, lsl #2]
	add	x16, x11, #4
	strh	w16, [x9, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x13, x14, lsl #2]
	b	LBB5_31
LBB5_48:
	cbz	x10, LBB5_54
; %bb.49:
	sub	x15, x11, #3
LBB5_50:
	mov	x11, sp
	b	LBB5_52
LBB5_51:                                ;   in Loop: Header=BB5_52 Depth=1
	add	x15, x15, #1
	subs	x10, x10, #1
	b.eq	LBB5_54
LBB5_52:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w12, [x20, x15]
	cbz	x12, LBB5_51
; %bb.53:                               ;   in Loop: Header=BB5_52 Depth=1
	ldr	w13, [x11, x12, lsl #2]
	strh	w15, [x9, x13, lsl #1]
	add	w13, w13, #1
	str	w13, [x11, x12, lsl #2]
	b	LBB5_51
LBB5_54:
	mov	w15, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	mov	w10, #9                         ; =0x9
	cmp	w8, #9
	csel	w10, w8, w10, lo
	add	x8, x19, #32
	add	w10, w10, #1
	mov	w11, #1                         ; =0x1
	mov	w12, #1                         ; =0x1
	b	LBB5_57
LBB5_55:                                ;   in Loop: Header=BB5_57 Depth=1
	mov	x15, x13
LBB5_56:                                ;   in Loop: Header=BB5_57 Depth=1
	lsl	w9, w9, #1
	add	x12, x12, #1
	cmp	x12, x10
	b.eq	LBB5_65
LBB5_57:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB5_60 Depth 2
                                        ;       Child Loop BB5_61 Depth 3
                                        ;       Child Loop BB5_64 Depth 3
	ldrh	w13, [x19, x12, lsl #1]
	add	w13, w15, w13
	cmp	w15, w13
	b.hs	LBB5_56
; %bb.58:                               ;   in Loop: Header=BB5_57 Depth=1
	lsl	w14, w11, w12
	mov	w15, w15
	b	LBB5_60
LBB5_59:                                ;   in Loop: Header=BB5_60 Depth=2
	add	w9, w9, #1
	add	x15, x15, #1
	cmp	x15, x13
	b.eq	LBB5_55
LBB5_60:                                ;   Parent Loop BB5_57 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB5_61 Depth 3
                                        ;       Child Loop BB5_64 Depth 3
	mov	w17, #0                         ; =0x0
	mov	x0, x12
	mov	x2, x9
LBB5_61:                                ;   Parent Loop BB5_57 Depth=1
                                        ;     Parent Loop BB5_60 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	mov	x16, x2
	mov	x1, x17
	mov	x17, x2
	bfi	w17, w1, #1, #31
	lsr	w2, w2, #1
	subs	x0, x0, #1
	b.ne	LBB5_61
; %bb.62:                               ;   in Loop: Header=BB5_60 Depth=2
	lsl	w0, w1, #1
	cmp	w0, #511
	b.hi	LBB5_59
; %bb.63:                               ;   in Loop: Header=BB5_60 Depth=2
	ldrh	w17, [x8, x15, lsl #1]
	orr	w17, w12, w17, lsl #4
	and	w16, w16, #0x1
	orr	w16, w0, w16
LBB5_64:                                ;   Parent Loop BB5_57 Depth=1
                                        ;     Parent Loop BB5_60 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	strh	w17, [x24, x16, lsl #1]
	add	x16, x16, x14
	cmp	x16, #512
	b.lo	LBB5_64
	b	LBB5_59
LBB5_65:
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #128
	ret
	.loh AdrpLdr	Lloh18, Lloh19
	.loh AdrpLdr	Lloh20, Lloh21
	.loh AdrpLdr	Lloh22, Lloh23
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream11fixed_treesEv
lCPI6_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream11fixed_treesEv
	.p2align	2
__ZN9gui_forms11compression13InflateStream11fixed_treesEv: ; @_ZN9gui_forms11compression13InflateStream11fixed_treesEv
	.cfi_startproc
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x9, [x0, #24]
	ldr	x8, [x0, #56]
	sub	x9, x9, x8
	cmp	x9, #319
	b.ls	LBB6_3
; %bb.1:
	add	x8, x8, #320
	str	x8, [x0, #56]
	mov	w8, #37764                      ; =0x9384
	add	x19, x0, x8
	movi.16b	v0, #8
	stp	q0, q0, [x19]
	stp	q0, q0, [x19, #32]
	stp	q0, q0, [x19, #64]
	stp	q0, q0, [x19, #96]
	mov	w8, #38044                      ; =0x949c
	mov	x9, #578721382704613384         ; =0x808080808080808
	str	x9, [x0, x8]
	str	q0, [x19, #128]
	mov	w8, #38020                      ; =0x9484
	add	x8, x0, x8
	mov	x9, #506381209866536711         ; =0x707070707070707
	stp	x9, x9, [x8]
	str	x9, [x8, #16]
	mov	w8, #37908                      ; =0x9414
	add	x8, x0, x8
	movi.16b	v0, #9
	stp	q0, q0, [x8]
	stp	q0, q0, [x8, #32]
	stp	q0, q0, [x8, #64]
	mov	w9, #32856                      ; =0x8058
	str	q0, [x8, #96]
	add	x1, x0, x9
	mov	x20, x0
	mov	x2, x19
	mov	w3, #288                        ; =0x120
	mov	w4, #0                          ; =0x0
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB6_4
; %bb.2:
	movi.16b	v0, #5
	stp	q0, q0, [x19]
	mov	w8, #34492                      ; =0x86bc
	mov	x0, x20
	add	x1, x20, x8
	mov	x2, x19
	mov	w3, #32                         ; =0x20
	mov	w4, #0                          ; =0x0
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	b	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
LBB6_3:
Lloh24:
	adrp	x8, lCPI6_0@PAGE
Lloh25:
	ldr	d0, [x8, lCPI6_0@PAGEOFF]
	str	d0, [x0, #32]
LBB6_4:
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh24, Lloh25
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
lCPI7_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI7_1:
	.long	19                              ; 0x13
	.long	4                               ; 0x4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.ne	LBB7_2
; %bb.1:
Lloh26:
	adrp	x9, lCPI7_1@PAGE
Lloh27:
	ldr	d0, [x9, lCPI7_1@PAGEOFF]
	str	d0, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB7_2:
	ldr	x8, [x1, #40]
	ldr	x9, [x1, #24]
	cmp	x8, x9
	b.ne	LBB7_4
; %bb.3:
	mov	w9, #1                          ; =0x1
	str	w9, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB7_4:
	ldr	x9, [x0, #24]
	ldr	x8, [x0, #56]
	cmp	x9, x8
	b.ne	LBB7_6
; %bb.5:
Lloh28:
	adrp	x9, lCPI7_0@PAGE
Lloh29:
	ldr	d0, [x9, lCPI7_0@PAGEOFF]
	str	d0, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB7_6:
	add	x8, x8, #1
	str	x8, [x0, #56]
	mov	w0, #1                          ; =0x1
	ret
	.loh AdrpLdr	Lloh26, Lloh27
	.loh AdrpLdr	Lloh28, Lloh29
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte ; -- Begin function _ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	.p2align	2
__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte: ; @_ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	.cfi_startproc
; %bb.0:
	mov	w8, #32832                      ; =0x8040
	add	x8, x0, x8
	ldr	x9, [x1, #40]
	ldr	x10, [x1, #16]
	strb	w2, [x10, x9]
	ldr	x9, [x1, #40]
	add	x9, x9, #1
	str	x9, [x1, #40]
	ldr	x9, [x0, #48]
	add	x9, x9, #1
	str	x9, [x0, #48]
	ldr	x9, [x8]
	add	x9, x0, x9
	strb	w2, [x9, #64]
	ldr	w9, [x8]
	add	w9, w9, #1
	and	x9, x9, #0x7fff
	str	x9, [x8]
	ldr	x9, [x8, #8]
	lsr	x10, x9, #15
	cbnz	x10, LBB8_2
; %bb.1:
	add	x9, x9, #1
	str	x9, [x8, #8]
LBB8_2:
	ldr	w8, [x0]
	cmp	w8, #1
	b.eq	LBB8_5
; %bb.3:
	mov	w8, #38172                      ; =0x951c
	add	x8, x0, x8
	mov	w9, #-65521                     ; =0xffff000f
	ldp	w10, w11, [x8]
	add	w10, w10, w2
	add	w12, w10, w9
	mov	w13, #65520                     ; =0xfff0
	cmp	w10, w13
	csel	w12, w12, w10, hi
	mov	w13, #65521                     ; =0xfff1
	add	w10, w12, w11
	cmp	w10, w13
	stp	w12, w10, [x8]
	b.lo	LBB8_5
; %bb.4:
	add	w9, w10, w9
	str	w9, [x8, #4]
LBB8_5:
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
lCPI9_0:
	.quad	14                              ; 0xe
	.quad	15                              ; 0xf
lCPI9_1:
	.quad	12                              ; 0xc
	.quad	13                              ; 0xd
lCPI9_2:
	.quad	10                              ; 0xa
	.quad	11                              ; 0xb
lCPI9_3:
	.quad	8                               ; 0x8
	.quad	9                               ; 0x9
lCPI9_4:
	.quad	6                               ; 0x6
	.quad	7                               ; 0x7
lCPI9_5:
	.quad	4                               ; 0x4
	.quad	5                               ; 0x5
lCPI9_6:
	.quad	2                               ; 0x2
	.quad	3                               ; 0x3
lCPI9_7:
	.quad	0                               ; 0x0
	.quad	1                               ; 0x1
lCPI9_8:
	.byte	12                              ; 0xc
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	13                              ; 0xd
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	14                              ; 0xe
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	15                              ; 0xf
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI9_9:
	.byte	0                               ; 0x0
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	1                               ; 0x1
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	2                               ; 0x2
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	3                               ; 0x3
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI9_10:
	.byte	4                               ; 0x4
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	5                               ; 0x5
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	6                               ; 0x6
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	7                               ; 0x7
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI9_11:
	.byte	8                               ; 0x8
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	9                               ; 0x9
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	10                              ; 0xa
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	11                              ; 0xb
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
	.p2align	2
__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm: ; @_ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
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
	mov	x19, x2
	mov	x21, x1
	mov	x20, x0
	mov	w8, #32832                      ; =0x8040
	add	x26, x0, x8
	ldr	x23, [x1, #16]
	ldr	x24, [x1, #40]
	add	x25, x23, x24
	add	x8, x0, #64
	ldr	x9, [x26]
	mov	w27, #32768                     ; =0x8000
	sub	x10, x27, x9
	cmp	x10, x2
	csel	x22, x10, x2, lo
	add	x0, x8, x9
	add	x1, x23, x24
	mov	x2, x22
	bl	_memcpy
	sub	x2, x19, x22
	add	x0, x20, #64
	add	x1, x25, x22
	bl	_memcpy
	ldr	x8, [x26, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x27, lo
	ldr	w9, [x26]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x26]
	ldr	x8, [x21, #40]
	add	x8, x8, x19
	str	x8, [x21, #40]
	ldr	x8, [x20, #48]
	add	x8, x8, x19
	str	x8, [x20, #48]
	ldr	w8, [x20]
	cmp	w8, #1
	b.eq	LBB9_17
; %bb.1:
	mov	w8, #38172                      ; =0x951c
	add	x8, x20, x8
	cbz	w19, LBB9_4
; %bb.2:
	mov	w9, w19
	cmp	x9, #4
	b.hs	LBB9_5
; %bb.3:
	mov	x10, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	b	LBB9_14
LBB9_4:
	mov	w15, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	b	LBB9_16
LBB9_5:
	dup.2d	v0, x19
	adrp	x12, lCPI9_6@PAGE
	adrp	x11, lCPI9_7@PAGE
	cmp	x9, #16
	b.hs	LBB9_7
; %bb.6:
	mov	x10, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	b	LBB9_11
LBB9_7:
	and	x13, x19, #0xc
	and	x10, x19, #0xfffffff0
Lloh30:
	adrp	x14, lCPI9_0@PAGE
Lloh31:
	ldr	q1, [x14, lCPI9_0@PAGEOFF]
Lloh32:
	adrp	x14, lCPI9_1@PAGE
Lloh33:
	ldr	q2, [x14, lCPI9_1@PAGEOFF]
Lloh34:
	adrp	x14, lCPI9_2@PAGE
Lloh35:
	ldr	q3, [x14, lCPI9_2@PAGEOFF]
Lloh36:
	adrp	x14, lCPI9_3@PAGE
Lloh37:
	ldr	q5, [x14, lCPI9_3@PAGEOFF]
Lloh38:
	adrp	x14, lCPI9_4@PAGE
Lloh39:
	ldr	q6, [x14, lCPI9_4@PAGEOFF]
Lloh40:
	adrp	x14, lCPI9_5@PAGE
Lloh41:
	ldr	q7, [x14, lCPI9_5@PAGEOFF]
	movi.2d	v4, #0000000000000000
Lloh42:
	adrp	x14, lCPI9_8@PAGE
Lloh43:
	ldr	q16, [x14, lCPI9_8@PAGEOFF]
Lloh44:
	adrp	x14, lCPI9_9@PAGE
Lloh45:
	ldr	q17, [x14, lCPI9_9@PAGEOFF]
Lloh46:
	adrp	x14, lCPI9_10@PAGE
Lloh47:
	ldr	q18, [x14, lCPI9_10@PAGEOFF]
Lloh48:
	adrp	x14, lCPI9_11@PAGE
Lloh49:
	ldr	q19, [x14, lCPI9_11@PAGEOFF]
	mov	w14, #16                        ; =0x10
	dup.2d	v20, x14
	and	x14, x19, #0xfffffff0
	movi.2d	v22, #0000000000000000
	movi.2d	v23, #0000000000000000
	movi.2d	v26, #0000000000000000
	movi.2d	v27, #0000000000000000
	movi.2d	v21, #0000000000000000
	ldr	q28, [x12, lCPI9_6@PAGEOFF]
	movi.2d	v24, #0000000000000000
	movi.2d	v25, #0000000000000000
	ldr	q29, [x11, lCPI9_7@PAGEOFF]
LBB9_8:                                 ; =>This Inner Loop Header: Depth=1
	sub.2d	v30, v0, v29
	sub.2d	v31, v0, v28
	ldr	q8, [x25], #16
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v17
	add.4s	v22, v22, v31
	mla.4s	v21, v30, v31
	sub.2d	v30, v0, v7
	sub.2d	v31, v0, v6
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v18
	add.4s	v23, v23, v31
	mla.4s	v4, v30, v31
	sub.2d	v30, v0, v5
	sub.2d	v31, v0, v3
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v19
	add.4s	v26, v26, v31
	mla.4s	v24, v30, v31
	sub.2d	v30, v0, v2
	sub.2d	v31, v0, v1
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v16
	mla.4s	v25, v30, v31
	add.4s	v27, v27, v31
	add.2d	v7, v7, v20
	add.2d	v28, v28, v20
	add.2d	v29, v29, v20
	add.2d	v6, v6, v20
	add.2d	v5, v5, v20
	add.2d	v3, v3, v20
	add.2d	v2, v2, v20
	add.2d	v1, v1, v20
	subs	x14, x14, #16
	b.ne	LBB9_8
; %bb.9:
	add.4s	v1, v22, v26
	add.4s	v2, v23, v27
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w14, s1
	add.4s	v1, v21, v24
	add.4s	v2, v4, v25
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w15, s1
	cmp	x9, x10
	b.eq	LBB9_16
; %bb.10:
	cbz	x13, LBB9_14
LBB9_11:
	mov	x13, x10
	and	x10, x19, #0xfffffffc
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	mov.s	v2[0], w14
	mov.s	v1[0], w15
	dup.2d	v4, x13
	ldr	q3, [x12, lCPI9_6@PAGEOFF]
	orr.16b	v3, v4, v3
	ldr	q5, [x11, lCPI9_7@PAGEOFF]
	orr.16b	v4, v4, v5
	sub	x11, x13, x10
	add	x12, x13, x24
	add	x12, x23, x12
	mov	w13, #4                         ; =0x4
	dup.2d	v5, x13
LBB9_12:                                ; =>This Inner Loop Header: Depth=1
	ldr	s6, [x12], #4
	ushll.8h	v6, v6, #0
	ushll.4s	v7, v6, #0
	uaddw.4s	v2, v2, v6
	sub.2d	v6, v0, v4
	sub.2d	v16, v0, v3
	uzp1.4s	v6, v6, v16
	mla.4s	v1, v6, v7
	add.2d	v3, v3, v5
	add.2d	v4, v4, v5
	adds	x11, x11, #4
	b.ne	LBB9_12
; %bb.13:
	addv.4s	s0, v2
	fmov	w14, s0
	addv.4s	s0, v1
	fmov	w15, s0
	cmp	x9, x10
	b.eq	LBB9_16
LBB9_14:
	add	x11, x10, x24
	add	x11, x23, x11
	sub	x9, x9, x10
LBB9_15:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x11], #1
	add	w14, w14, w10
	madd	w15, w9, w10, w15
	subs	x9, x9, #1
	b.ne	LBB9_15
LBB9_16:
	ldp	w10, w9, [x8]
	add	w9, w9, w15
	madd	w9, w10, w19, w9
	add	w10, w10, w14
	fmov	s0, w10
	mov.s	v0[1], w9
	mov	w9, #32881                      ; =0x8071
	movk	w9, #32775, lsl #16
	dup.2s	v1, w9
	umull.2d	v1, v0, v1
	ushr.2d	v1, v1, #47
	xtn.2s	v1, v1
	mov	w9, #65521                      ; =0xfff1
	dup.2s	v2, w9
	mls.2s	v0, v1, v2
	str	d0, [x8]
LBB9_17:
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh48, Lloh49
	.loh AdrpAdrp	Lloh46, Lloh48
	.loh AdrpLdr	Lloh46, Lloh47
	.loh AdrpAdrp	Lloh44, Lloh46
	.loh AdrpLdr	Lloh44, Lloh45
	.loh AdrpAdrp	Lloh42, Lloh44
	.loh AdrpLdr	Lloh42, Lloh43
	.loh AdrpAdrp	Lloh40, Lloh42
	.loh AdrpLdr	Lloh40, Lloh41
	.loh AdrpAdrp	Lloh38, Lloh40
	.loh AdrpLdr	Lloh38, Lloh39
	.loh AdrpAdrp	Lloh36, Lloh38
	.loh AdrpLdr	Lloh36, Lloh37
	.loh AdrpAdrp	Lloh34, Lloh36
	.loh AdrpLdr	Lloh34, Lloh35
	.loh AdrpAdrp	Lloh32, Lloh34
	.loh AdrpLdr	Lloh32, Lloh33
	.loh AdrpAdrp	Lloh30, Lloh32
	.loh AdrpLdr	Lloh30, Lloh31
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
lCPI10_0:
	.quad	14                              ; 0xe
	.quad	15                              ; 0xf
lCPI10_1:
	.quad	12                              ; 0xc
	.quad	13                              ; 0xd
lCPI10_2:
	.quad	10                              ; 0xa
	.quad	11                              ; 0xb
lCPI10_3:
	.quad	8                               ; 0x8
	.quad	9                               ; 0x9
lCPI10_4:
	.quad	6                               ; 0x6
	.quad	7                               ; 0x7
lCPI10_5:
	.quad	4                               ; 0x4
	.quad	5                               ; 0x5
lCPI10_6:
	.quad	2                               ; 0x2
	.quad	3                               ; 0x3
lCPI10_7:
	.quad	0                               ; 0x0
	.quad	1                               ; 0x1
lCPI10_8:
	.byte	12                              ; 0xc
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	13                              ; 0xd
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	14                              ; 0xe
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	15                              ; 0xf
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI10_9:
	.byte	0                               ; 0x0
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	1                               ; 0x1
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	2                               ; 0x2
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	3                               ; 0x3
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI10_10:
	.byte	4                               ; 0x4
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	5                               ; 0x5
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	6                               ; 0x6
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	7                               ; 0x7
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI10_11:
	.byte	8                               ; 0x8
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	9                               ; 0x9
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	10                              ; 0xa
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	11                              ; 0xb
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
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
	mov	w8, #38104                      ; =0x94d8
	add	x20, x0, x8
	ldr	w8, [x20]
	cmp	w8, #1
	b.ne	LBB10_11
; %bb.1:
	ldp	x8, x9, [x1, #32]
	ldr	x10, [x1, #8]
	ldr	x11, [x1, #24]
	sub	x10, x10, x8
	lsr	x10, x10, #1
	sub	x9, x11, x9
	cmp	x9, x10
	csel	x9, x9, x10, lo
	ldp	x10, x11, [x0, #8]
	ldp	x12, x13, [x0, #40]
	sub	x10, x10, x12
	ldr	x12, [x0, #24]
	ldr	x14, [x0, #56]
	sub	x12, x12, x14
	mov	x14, #-3689348814741910324      ; =0xcccccccccccccccc
	movk	x14, #52429
	umulh	x12, x12, x14
	lsr	x10, x10, #1
	lsr	x12, x12, #2
	cmp	x9, x10
	csel	x9, x9, x10, lo
	sub	x10, x11, x13
	cmp	x9, x10
	csel	x9, x9, x10, lo
	cmp	x9, x12
	csel	x9, x9, x12, lo
	mov	w10, #4096                      ; =0x1000
	cmp	x9, #1, lsl #12                 ; =4096
	csel	x11, x9, x10, lo
	cbz	x9, LBB10_11
; %bb.2:
	mov	x21, #0                         ; =0x0
	mov	w9, #32832                      ; =0x8040
	add	x25, x0, x9
	ldp	w10, w9, [x20, #16]
	mov	w12, #33464                     ; =0x82b8
	add	x12, x0, x12
	mov	w13, #-1                        ; =0xffffffff
LBB10_3:                                ; =>This Inner Loop Header: Depth=1
	ldr	x14, [x1]
	ldrh	w14, [x14, x8]
	lsl	w14, w14, w9
	orr	w15, w14, w10
	and	w14, w15, #0x1ff
	ldrh	w14, [x12, w14, uxtw #1]
	sub	w16, w14, #1, lsl #12           ; =4096
	cmn	w16, #4095
	b.lo	LBB10_6
; %bb.4:                                ;   in Loop: Header=BB10_3 Depth=1
	and	w10, w14, #0xf
	sub	w16, w10, w9
	add	w16, w16, #7
	lsr	w17, w16, #3
	and	w16, w16, #0xfffffff8
	sub	w9, w9, w10
	add	w9, w9, w16
	lsl	w16, w13, w9
	lsr	w10, w15, w10
	bic	w10, w10, w16
	add	x8, x8, x17
	lsr	w14, w14, #4
	ldr	x15, [x1, #40]
	ldr	x16, [x1, #16]
	add	x15, x16, x15
	strb	w14, [x15, x21]
	add	x21, x21, #1
	cmp	x11, x21
	b.ne	LBB10_3
; %bb.5:
	mov	x21, x11
	b	LBB10_7
LBB10_6:
	cbz	x21, LBB10_11
LBB10_7:
	add	x11, x21, x21, lsl #1
	ldr	x12, [x0, #56]
	ldr	x13, [x0, #40]
	ldp	x14, x22, [x1, #32]
	sub	x14, x8, x14
	add	x13, x13, x14
	add	x11, x11, x12
	add	x11, x14, x11
	str	x11, [x0, #56]
	str	x13, [x0, #40]
	str	x8, [x1, #32]
	stp	w10, w9, [x20, #16]
	ldr	x23, [x1, #16]
	add	x24, x23, x22
	add	x8, x0, #64
	ldr	x9, [x25]
	mov	w26, #32768                     ; =0x8000
	sub	x10, x26, x9
	cmp	x10, x21
	csel	x19, x10, x21, lo
	mov	x27, x0
	add	x0, x8, x9
	mov	x28, x1
	add	x1, x23, x22
	mov	x2, x19
	bl	_memcpy
	sub	x2, x21, x19
	add	x0, x27, #64
	add	x1, x24, x19
	bl	_memcpy
	ldr	x8, [x25, #8]
	add	x8, x8, x21
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x26, lo
	ldr	w9, [x25]
	add	w9, w9, w21
	and	x9, x9, #0x7fff
	stp	x9, x8, [x25]
	ldr	x8, [x28, #40]
	add	x8, x8, x21
	str	x8, [x28, #40]
	ldr	x8, [x27, #48]
	add	x8, x8, x21
	str	x8, [x27, #48]
	ldr	w8, [x27]
	cmp	w8, #1
	b.eq	LBB10_25
; %bb.8:
	cbz	w21, LBB10_12
; %bb.9:
	mov	w8, w21
	cmp	x8, #4
	b.hs	LBB10_13
; %bb.10:
	mov	x9, #0                          ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	b	LBB10_22
LBB10_11:
	mov	w0, #0                          ; =0x0
	b	LBB10_26
LBB10_12:
	mov	w14, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	b	LBB10_24
LBB10_13:
	dup.2d	v0, x21
	adrp	x11, lCPI10_6@PAGE
	adrp	x10, lCPI10_7@PAGE
	cmp	x8, #16
	b.hs	LBB10_15
; %bb.14:
	mov	x9, #0                          ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	b	LBB10_19
LBB10_15:
	and	x12, x21, #0xc
	and	x9, x21, #0xfffffff0
Lloh50:
	adrp	x13, lCPI10_0@PAGE
Lloh51:
	ldr	q1, [x13, lCPI10_0@PAGEOFF]
Lloh52:
	adrp	x13, lCPI10_1@PAGE
Lloh53:
	ldr	q2, [x13, lCPI10_1@PAGEOFF]
Lloh54:
	adrp	x13, lCPI10_2@PAGE
Lloh55:
	ldr	q3, [x13, lCPI10_2@PAGEOFF]
Lloh56:
	adrp	x13, lCPI10_3@PAGE
Lloh57:
	ldr	q5, [x13, lCPI10_3@PAGEOFF]
Lloh58:
	adrp	x13, lCPI10_4@PAGE
Lloh59:
	ldr	q6, [x13, lCPI10_4@PAGEOFF]
Lloh60:
	adrp	x13, lCPI10_5@PAGE
Lloh61:
	ldr	q7, [x13, lCPI10_5@PAGEOFF]
	movi.2d	v4, #0000000000000000
Lloh62:
	adrp	x13, lCPI10_8@PAGE
Lloh63:
	ldr	q16, [x13, lCPI10_8@PAGEOFF]
Lloh64:
	adrp	x13, lCPI10_9@PAGE
Lloh65:
	ldr	q17, [x13, lCPI10_9@PAGEOFF]
Lloh66:
	adrp	x13, lCPI10_10@PAGE
Lloh67:
	ldr	q18, [x13, lCPI10_10@PAGEOFF]
Lloh68:
	adrp	x13, lCPI10_11@PAGE
Lloh69:
	ldr	q19, [x13, lCPI10_11@PAGEOFF]
	mov	w13, #16                        ; =0x10
	dup.2d	v20, x13
	and	x13, x21, #0xfffffff0
	movi.2d	v22, #0000000000000000
	movi.2d	v23, #0000000000000000
	movi.2d	v26, #0000000000000000
	movi.2d	v27, #0000000000000000
	movi.2d	v21, #0000000000000000
	ldr	q28, [x11, lCPI10_6@PAGEOFF]
	movi.2d	v24, #0000000000000000
	movi.2d	v25, #0000000000000000
	ldr	q29, [x10, lCPI10_7@PAGEOFF]
LBB10_16:                               ; =>This Inner Loop Header: Depth=1
	sub.2d	v30, v0, v29
	sub.2d	v31, v0, v28
	ldr	q8, [x24], #16
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v17
	add.4s	v22, v22, v31
	mla.4s	v21, v31, v30
	sub.2d	v30, v0, v7
	sub.2d	v31, v0, v6
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v18
	add.4s	v23, v23, v31
	mla.4s	v4, v31, v30
	sub.2d	v30, v0, v5
	sub.2d	v31, v0, v3
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v19
	add.4s	v26, v26, v31
	mla.4s	v24, v31, v30
	sub.2d	v30, v0, v2
	sub.2d	v31, v0, v1
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v16
	mla.4s	v25, v31, v30
	add.4s	v27, v27, v31
	add.2d	v7, v7, v20
	add.2d	v28, v28, v20
	add.2d	v29, v29, v20
	add.2d	v6, v6, v20
	add.2d	v5, v5, v20
	add.2d	v3, v3, v20
	add.2d	v2, v2, v20
	add.2d	v1, v1, v20
	subs	x13, x13, #16
	b.ne	LBB10_16
; %bb.17:
	add.4s	v1, v22, v26
	add.4s	v2, v23, v27
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w13, s1
	add.4s	v1, v21, v24
	add.4s	v2, v4, v25
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w14, s1
	cmp	x8, x9
	b.eq	LBB10_24
; %bb.18:
	cbz	x12, LBB10_22
LBB10_19:
	mov	x12, x9
	and	x9, x21, #0xfffffffc
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	mov.s	v2[0], w13
	mov.s	v1[0], w14
	dup.2d	v4, x12
	ldr	q3, [x11, lCPI10_6@PAGEOFF]
	orr.16b	v3, v4, v3
	ldr	q5, [x10, lCPI10_7@PAGEOFF]
	orr.16b	v4, v4, v5
	sub	x10, x12, x9
	add	x11, x12, x22
	add	x11, x23, x11
	mov	w12, #4                         ; =0x4
	dup.2d	v5, x12
LBB10_20:                               ; =>This Inner Loop Header: Depth=1
	ldr	s6, [x11], #4
	ushll.8h	v6, v6, #0
	ushll.4s	v7, v6, #0
	uaddw.4s	v2, v2, v6
	sub.2d	v6, v0, v4
	sub.2d	v16, v0, v3
	uzp1.4s	v6, v6, v16
	mla.4s	v1, v7, v6
	add.2d	v3, v3, v5
	add.2d	v4, v4, v5
	adds	x10, x10, #4
	b.ne	LBB10_20
; %bb.21:
	addv.4s	s0, v2
	fmov	w13, s0
	addv.4s	s0, v1
	fmov	w14, s0
	cmp	x8, x9
	b.eq	LBB10_24
LBB10_22:
	sub	x8, x8, x9
	add	x9, x9, x22
	add	x9, x23, x9
LBB10_23:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x9], #1
	add	w13, w13, w10
	madd	w14, w10, w8, w14
	subs	x8, x8, #1
	b.ne	LBB10_23
LBB10_24:
	ldp	w9, w8, [x20, #68]
	add	w8, w8, w14
	madd	w8, w9, w21, w8
	add	w9, w9, w13
	fmov	s0, w9
	mov.s	v0[1], w8
	mov	w8, #32881                      ; =0x8071
	movk	w8, #32775, lsl #16
	dup.2s	v1, w8
	umull.2d	v1, v0, v1
	ushr.2d	v1, v1, #47
	xtn.2s	v1, v1
	mov	w8, #65521                      ; =0xfff1
	dup.2s	v2, w8
	mls.2s	v0, v1, v2
	stur	d0, [x20, #68]
LBB10_25:
	mov	w0, #1                          ; =0x1
LBB10_26:
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp], #112              ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh68, Lloh69
	.loh AdrpAdrp	Lloh66, Lloh68
	.loh AdrpLdr	Lloh66, Lloh67
	.loh AdrpAdrp	Lloh64, Lloh66
	.loh AdrpLdr	Lloh64, Lloh65
	.loh AdrpAdrp	Lloh62, Lloh64
	.loh AdrpLdr	Lloh62, Lloh63
	.loh AdrpAdrp	Lloh60, Lloh62
	.loh AdrpLdr	Lloh60, Lloh61
	.loh AdrpAdrp	Lloh58, Lloh60
	.loh AdrpLdr	Lloh58, Lloh59
	.loh AdrpAdrp	Lloh56, Lloh58
	.loh AdrpLdr	Lloh56, Lloh57
	.loh AdrpAdrp	Lloh54, Lloh56
	.loh AdrpLdr	Lloh54, Lloh55
	.loh AdrpAdrp	Lloh52, Lloh54
	.loh AdrpLdr	Lloh52, Lloh53
	.loh AdrpAdrp	Lloh50, Lloh52
	.loh AdrpLdr	Lloh50, Lloh51
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
lCPI11_0:
	.quad	14                              ; 0xe
	.quad	15                              ; 0xf
lCPI11_1:
	.quad	12                              ; 0xc
	.quad	13                              ; 0xd
lCPI11_2:
	.quad	10                              ; 0xa
	.quad	11                              ; 0xb
lCPI11_3:
	.quad	8                               ; 0x8
	.quad	9                               ; 0x9
lCPI11_4:
	.quad	6                               ; 0x6
	.quad	7                               ; 0x7
lCPI11_5:
	.quad	4                               ; 0x4
	.quad	5                               ; 0x5
lCPI11_6:
	.quad	2                               ; 0x2
	.quad	3                               ; 0x3
lCPI11_7:
	.quad	0                               ; 0x0
	.quad	1                               ; 0x1
lCPI11_8:
	.byte	12                              ; 0xc
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	13                              ; 0xd
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	14                              ; 0xe
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	15                              ; 0xf
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI11_9:
	.byte	0                               ; 0x0
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	1                               ; 0x1
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	2                               ; 0x2
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	3                               ; 0x3
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI11_10:
	.byte	4                               ; 0x4
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	5                               ; 0x5
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	6                               ; 0x6
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	7                               ; 0x7
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI11_11:
	.byte	8                               ; 0x8
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	9                               ; 0x9
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	10                              ; 0xa
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	11                              ; 0xb
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0
lCPI11_12:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI11_13:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI11_14:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
lCPI11_15:
	.long	19                              ; 0x13
	.long	4                               ; 0x4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	subs	x11, x9, x8
	b.ne	LBB11_2
; %bb.1:
	mov	w8, #0                          ; =0x0
Lloh70:
	adrp	x9, lCPI11_15@PAGE
Lloh71:
	ldr	d0, [x9, lCPI11_15@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB11_2:
	ldr	x8, [x1, #40]
	ldr	x9, [x1, #24]
	subs	x13, x9, x8
	b.ne	LBB11_4
; %bb.3:
	mov	w9, #1                          ; =0x1
	str	w9, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB11_4:
	ldr	x12, [x0, #24]
	ldr	x9, [x0, #56]
	cmp	x12, x9
	b.ne	LBB11_6
; %bb.5:
	mov	w8, #0                          ; =0x0
Lloh72:
	adrp	x9, lCPI11_12@PAGE
Lloh73:
	ldr	d0, [x9, lCPI11_12@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB11_6:
	add	x14, x9, #1
	str	x14, [x0, #56]
	ldr	x10, [x1, #32]
	ldr	x15, [x1, #8]
	subs	x15, x15, x10
	b.ne	LBB11_10
; %bb.7:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB11_9
; %bb.8:
Lloh74:
	adrp	x8, lCPI11_14@PAGE
Lloh75:
	ldr	d0, [x8, lCPI11_14@PAGEOFF]
	str	d0, [x0, #32]
LBB11_9:
	str	wzr, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB11_10:
	ldr	x16, [x0, #40]
	ldr	x17, [x0, #8]
	subs	x16, x17, x16
	b.ne	LBB11_12
; %bb.11:
	mov	w8, #0                          ; =0x0
Lloh76:
	adrp	x9, lCPI11_13@PAGE
Lloh77:
	ldr	d0, [x9, lCPI11_13@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB11_12:
	sub	sp, sp, #128
	stp	d9, d8, [sp, #16]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #32]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
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
	mov	w17, #38164                     ; =0x9514
	add	x20, x0, x17
	ldr	w25, [x20]
	mov	w17, #4096                      ; =0x1000
	cmp	w25, #1, lsl #12                ; =4096
	csel	w17, w25, w17, lo
	cmp	x13, x17
	csel	x13, x13, x17, lo
	cmp	x15, x13
	csel	x13, x15, x13, lo
	cmp	x16, x13
	csel	x13, x16, x13, lo
	cmp	x11, x13
	csel	x19, x11, x13, lo
	lsl	x11, x19, #1
	sub	x13, x11, #1
	sub	x12, x12, x14
	cmp	x13, x12
	b.ls	LBB11_14
; %bb.13:
	mov	w8, #0                          ; =0x0
Lloh78:
	adrp	x9, lCPI11_12@PAGE
Lloh79:
	ldr	d0, [x9, lCPI11_12@PAGEOFF]
	str	d0, [x0, #32]
	b	LBB11_31
LBB11_14:
	mov	w12, #32832                     ; =0x8040
	add	x26, x0, x12
	add	x9, x11, x9
	str	x9, [x0, #56]
	ldr	x9, [x1, #16]
	ldr	x11, [x1]
	mov	x27, x0
	add	x0, x9, x8
	mov	x28, x1
	add	x1, x11, x10
	mov	x2, x19
	bl	_memcpy
	ldp	x8, x11, [x28, #32]
	add	x8, x8, x19
	str	x8, [x28, #32]
	ldr	x8, [x27, #40]
	add	x8, x8, x19
	str	x8, [x27, #40]
	ldr	w8, [x20]
	sub	w8, w8, w19
	mov	x21, x20
	str	w8, [x20]
	ldr	x12, [x28, #16]
	add	x24, x12, x11
	add	x8, x27, #64
	ldr	x9, [x26]
	mov	w22, #32768                     ; =0x8000
	sub	x10, x22, x9
	cmp	x10, x19
	csel	x20, x10, x19, lo
	add	x0, x8, x9
	mov	x23, x11
	str	x12, [sp, #8]                   ; 8-byte Spill
	add	x1, x12, x11
	mov	x2, x20
	bl	_memcpy
	sub	x2, x19, x20
	add	x0, x27, #64
	add	x1, x24, x20
	bl	_memcpy
	ldr	x8, [x26, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x22, lo
	ldr	w9, [x26]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x26]
	ldr	x8, [x28, #40]
	add	x8, x8, x19
	str	x8, [x28, #40]
	ldr	x8, [x27, #48]
	add	x8, x8, x19
	str	x8, [x27, #48]
	ldr	w8, [x27]
	cmp	w8, #1
	b.eq	LBB11_31
; %bb.15:
	cbz	w25, LBB11_18
; %bb.16:
	cmp	x19, #4
	b.hs	LBB11_19
; %bb.17:
	mov	x8, #0                          ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	ldr	x15, [sp, #8]                   ; 8-byte Reload
	b	LBB11_28
LBB11_18:
	mov	w13, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	b	LBB11_30
LBB11_19:
	dup.2d	v0, x19
	adrp	x10, lCPI11_6@PAGE
	adrp	x9, lCPI11_7@PAGE
	cmp	x19, #16
	ldr	x15, [sp, #8]                   ; 8-byte Reload
	b.hs	LBB11_21
; %bb.20:
	mov	x8, #0                          ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	b	LBB11_25
LBB11_21:
	and	x11, x19, #0xc
	and	x8, x19, #0x7ffffff0
Lloh80:
	adrp	x12, lCPI11_0@PAGE
Lloh81:
	ldr	q1, [x12, lCPI11_0@PAGEOFF]
Lloh82:
	adrp	x12, lCPI11_1@PAGE
Lloh83:
	ldr	q2, [x12, lCPI11_1@PAGEOFF]
Lloh84:
	adrp	x12, lCPI11_2@PAGE
Lloh85:
	ldr	q3, [x12, lCPI11_2@PAGEOFF]
Lloh86:
	adrp	x12, lCPI11_3@PAGE
Lloh87:
	ldr	q5, [x12, lCPI11_3@PAGEOFF]
Lloh88:
	adrp	x12, lCPI11_4@PAGE
Lloh89:
	ldr	q6, [x12, lCPI11_4@PAGEOFF]
Lloh90:
	adrp	x12, lCPI11_5@PAGE
Lloh91:
	ldr	q7, [x12, lCPI11_5@PAGEOFF]
	movi.2d	v4, #0000000000000000
Lloh92:
	adrp	x12, lCPI11_8@PAGE
Lloh93:
	ldr	q16, [x12, lCPI11_8@PAGEOFF]
Lloh94:
	adrp	x12, lCPI11_9@PAGE
Lloh95:
	ldr	q17, [x12, lCPI11_9@PAGEOFF]
Lloh96:
	adrp	x12, lCPI11_10@PAGE
Lloh97:
	ldr	q18, [x12, lCPI11_10@PAGEOFF]
Lloh98:
	adrp	x12, lCPI11_11@PAGE
Lloh99:
	ldr	q19, [x12, lCPI11_11@PAGEOFF]
	mov	w12, #16                        ; =0x10
	dup.2d	v20, x12
	and	x12, x19, #0x7ffffff0
	movi.2d	v22, #0000000000000000
	movi.2d	v23, #0000000000000000
	movi.2d	v26, #0000000000000000
	movi.2d	v27, #0000000000000000
	movi.2d	v21, #0000000000000000
	ldr	q28, [x10, lCPI11_6@PAGEOFF]
	movi.2d	v24, #0000000000000000
	movi.2d	v25, #0000000000000000
	ldr	q29, [x9, lCPI11_7@PAGEOFF]
LBB11_22:                               ; =>This Inner Loop Header: Depth=1
	sub.2d	v30, v0, v29
	sub.2d	v31, v0, v28
	ldr	q8, [x24], #16
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v17
	add.4s	v22, v22, v31
	mla.4s	v21, v31, v30
	sub.2d	v30, v0, v7
	sub.2d	v31, v0, v6
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v18
	add.4s	v23, v23, v31
	mla.4s	v4, v31, v30
	sub.2d	v30, v0, v5
	sub.2d	v31, v0, v3
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v19
	add.4s	v26, v26, v31
	mla.4s	v24, v31, v30
	sub.2d	v30, v0, v2
	sub.2d	v31, v0, v1
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v16
	mla.4s	v25, v31, v30
	add.4s	v27, v27, v31
	add.2d	v7, v7, v20
	add.2d	v28, v28, v20
	add.2d	v29, v29, v20
	add.2d	v6, v6, v20
	add.2d	v5, v5, v20
	add.2d	v3, v3, v20
	add.2d	v2, v2, v20
	add.2d	v1, v1, v20
	subs	x12, x12, #16
	b.ne	LBB11_22
; %bb.23:
	add.4s	v1, v22, v26
	add.4s	v2, v23, v27
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w12, s1
	add.4s	v1, v21, v24
	add.4s	v2, v4, v25
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w13, s1
	cmp	x19, x8
	b.eq	LBB11_30
; %bb.24:
	cbz	x11, LBB11_28
LBB11_25:
	mov	x11, x8
	and	x8, x19, #0x7ffffffc
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	mov.s	v2[0], w12
	mov.s	v1[0], w13
	dup.2d	v4, x11
	ldr	q3, [x10, lCPI11_6@PAGEOFF]
	orr.16b	v3, v4, v3
	ldr	q5, [x9, lCPI11_7@PAGEOFF]
	orr.16b	v4, v4, v5
	sub	x9, x11, x8
	add	x10, x11, x23
	add	x10, x15, x10
	mov	w11, #4                         ; =0x4
	dup.2d	v5, x11
LBB11_26:                               ; =>This Inner Loop Header: Depth=1
	ldr	s6, [x10], #4
	ushll.8h	v6, v6, #0
	ushll.4s	v7, v6, #0
	uaddw.4s	v2, v2, v6
	sub.2d	v6, v0, v4
	sub.2d	v16, v0, v3
	uzp1.4s	v6, v6, v16
	mla.4s	v1, v7, v6
	add.2d	v3, v3, v5
	add.2d	v4, v4, v5
	adds	x9, x9, #4
	b.ne	LBB11_26
; %bb.27:
	addv.4s	s0, v2
	fmov	w12, s0
	addv.4s	s0, v1
	fmov	w13, s0
	cmp	x19, x8
	b.eq	LBB11_30
LBB11_28:
	sub	x9, x19, x8
	add	x8, x8, x23
	add	x8, x15, x8
LBB11_29:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x8], #1
	add	w12, w12, w10
	madd	w13, w10, w9, w13
	subs	x9, x9, #1
	b.ne	LBB11_29
LBB11_30:
	ldp	w9, w8, [x21, #8]
	add	w8, w8, w13
	madd	w8, w9, w19, w8
	add	w9, w9, w12
	fmov	s0, w9
	mov.s	v0[1], w8
	mov	w8, #32881                      ; =0x8071
	movk	w8, #32775, lsl #16
	dup.2s	v1, w8
	umull.2d	v1, v0, v1
	ushr.2d	v1, v1, #47
	xtn.2s	v1, v1
	mov	w8, #65521                      ; =0xfff1
	dup.2s	v2, w8
	mls.2s	v0, v1, v2
	str	d0, [x21, #8]
	mov	w8, #1                          ; =0x1
LBB11_31:
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #32]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #16]               ; 16-byte Folded Reload
	add	sp, sp, #128
	mov	x0, x8
	ret
	.loh AdrpLdr	Lloh70, Lloh71
	.loh AdrpLdr	Lloh72, Lloh73
	.loh AdrpLdr	Lloh74, Lloh75
	.loh AdrpLdr	Lloh76, Lloh77
	.loh AdrpLdr	Lloh78, Lloh79
	.loh AdrpLdr	Lloh98, Lloh99
	.loh AdrpAdrp	Lloh96, Lloh98
	.loh AdrpLdr	Lloh96, Lloh97
	.loh AdrpAdrp	Lloh94, Lloh96
	.loh AdrpLdr	Lloh94, Lloh95
	.loh AdrpAdrp	Lloh92, Lloh94
	.loh AdrpLdr	Lloh92, Lloh93
	.loh AdrpAdrp	Lloh90, Lloh92
	.loh AdrpLdr	Lloh90, Lloh91
	.loh AdrpAdrp	Lloh88, Lloh90
	.loh AdrpLdr	Lloh88, Lloh89
	.loh AdrpAdrp	Lloh86, Lloh88
	.loh AdrpLdr	Lloh86, Lloh87
	.loh AdrpAdrp	Lloh84, Lloh86
	.loh AdrpLdr	Lloh84, Lloh85
	.loh AdrpAdrp	Lloh82, Lloh84
	.loh AdrpLdr	Lloh82, Lloh83
	.loh AdrpAdrp	Lloh80, Lloh82
	.loh AdrpLdr	Lloh80, Lloh81
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
lCPI12_0:
	.quad	14                              ; 0xe
	.quad	15                              ; 0xf
lCPI12_1:
	.quad	12                              ; 0xc
	.quad	13                              ; 0xd
lCPI12_2:
	.quad	10                              ; 0xa
	.quad	11                              ; 0xb
lCPI12_3:
	.quad	8                               ; 0x8
	.quad	9                               ; 0x9
lCPI12_4:
	.quad	6                               ; 0x6
	.quad	7                               ; 0x7
lCPI12_5:
	.quad	4                               ; 0x4
	.quad	5                               ; 0x5
lCPI12_6:
	.quad	2                               ; 0x2
	.quad	3                               ; 0x3
lCPI12_7:
	.quad	0                               ; 0x0
	.quad	1                               ; 0x1
lCPI12_8:
	.byte	12                              ; 0xc
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	13                              ; 0xd
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	14                              ; 0xe
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	15                              ; 0xf
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI12_9:
	.byte	0                               ; 0x0
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	1                               ; 0x1
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	2                               ; 0x2
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	3                               ; 0x3
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI12_10:
	.byte	4                               ; 0x4
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	5                               ; 0x5
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	6                               ; 0x6
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	7                               ; 0x7
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
lCPI12_11:
	.byte	8                               ; 0x8
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	9                               ; 0x9
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	10                              ; 0xa
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	11                              ; 0xb
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.byte	255                             ; 0xff
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0
lCPI12_12:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI12_13:
	.long	19                              ; 0x13
	.long	4                               ; 0x4
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	subs	x10, x9, x8
	b.ne	LBB12_2
; %bb.1:
	mov	w8, #0                          ; =0x0
Lloh100:
	adrp	x9, lCPI12_13@PAGE
Lloh101:
	ldr	d0, [x9, lCPI12_13@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB12_2:
	ldr	x8, [x1, #40]
	ldr	x9, [x1, #24]
	subs	x11, x9, x8
	b.ne	LBB12_4
; %bb.3:
	mov	w9, #1                          ; =0x1
	str	w9, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB12_4:
	ldr	x12, [x0, #24]
	ldr	x9, [x0, #56]
	cmp	x12, x9
	b.ne	LBB12_6
; %bb.5:
	mov	w8, #0                          ; =0x0
Lloh102:
	adrp	x9, lCPI12_12@PAGE
Lloh103:
	ldr	d0, [x9, lCPI12_12@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB12_6:
	sub	sp, sp, #128
	stp	d9, d8, [sp, #16]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #32]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
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
	mov	w13, #38148                     ; =0x9504
	add	x22, x0, x13
	add	x13, x9, #1
	str	x13, [x0, #56]
	ldr	w25, [x22]
	cmp	x11, x25
	csel	x11, x11, x25, lo
	cmp	x10, x11
	csel	x19, x10, x11, lo
	sub	x10, x19, #1
	sub	x11, x12, x13
	cmp	x10, x11
	b.ls	LBB12_8
; %bb.7:
	mov	w8, #0                          ; =0x0
Lloh104:
	adrp	x9, lCPI12_12@PAGE
Lloh105:
	ldr	d0, [x9, lCPI12_12@PAGEOFF]
	str	d0, [x0, #32]
	b	LBB12_29
LBB12_8:
	mov	w10, #32832                     ; =0x8040
	add	x26, x0, x10
	add	x9, x19, x9
	str	x9, [x0, #56]
	ldr	w9, [x26]
	str	x0, [sp, #8]                    ; 8-byte Spill
	add	x11, x0, #64
	ldr	w28, [x22, #4]
	sub	w9, w9, w28
	and	x9, x9, #0x7fff
	mov	x23, x1
	ldr	x10, [x1, #16]
	add	x20, x10, x8
	cmp	w28, #1
	b.ne	LBB12_10
; %bb.9:
	mov	x27, x11
	ldrb	w1, [x11, x9]
	mov	x0, x20
	mov	x2, x19
	bl	_memset
	b	LBB12_12
LBB12_10:
	cmp	x19, x28
	csel	x24, x19, x28, lo
	mov	w8, #32768                      ; =0x8000
	sub	x8, x8, x9
	cmp	x8, x24
	csel	x21, x8, x24, lo
	mov	x27, x11
	add	x1, x11, x9
	mov	x0, x20
	mov	x2, x21
	bl	_memcpy
	sub	x2, x24, x21
	add	x0, x20, x21
	ldr	x8, [sp, #8]                    ; 8-byte Reload
	add	x1, x8, #64
	bl	_memcpy
	cmp	x19, x28
	b.ls	LBB12_12
LBB12_11:                               ; =>This Inner Loop Header: Depth=1
	sub	x8, x19, x24
	cmp	x8, x24
	csel	x21, x8, x24, lo
	add	x0, x20, x24
	mov	x1, x20
	mov	x2, x21
	bl	_memcpy
	add	x24, x21, x24
	cmp	x24, x19
	b.lo	LBB12_11
LBB12_12:
	ldr	w8, [x22]
	sub	w8, w8, w19
	str	x22, [sp]                       ; 8-byte Spill
	str	w8, [x22]
	mov	x28, x23
	ldr	x10, [x23, #16]
	ldr	x11, [x23, #40]
	add	x24, x10, x11
	ldr	x8, [x26]
	mov	w21, #32768                     ; =0x8000
	sub	x9, x21, x8
	cmp	x9, x19
	csel	x20, x9, x19, lo
	add	x0, x27, x8
	mov	x23, x10
	mov	x22, x11
	add	x1, x10, x11
	mov	x2, x20
	bl	_memcpy
	sub	x2, x19, x20
	ldr	x27, [sp, #8]                   ; 8-byte Reload
	add	x0, x27, #64
	add	x1, x24, x20
	bl	_memcpy
	ldr	x8, [x26, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x21, lo
	ldr	w9, [x26]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x26]
	ldr	x8, [x28, #40]
	add	x8, x8, x19
	str	x8, [x28, #40]
	ldr	x8, [x27, #48]
	add	x8, x8, x19
	str	x8, [x27, #48]
	ldr	w8, [x27]
	cmp	w8, #1
	b.eq	LBB12_29
; %bb.13:
	cbz	w25, LBB12_16
; %bb.14:
	cmp	x19, #4
	b.hs	LBB12_17
; %bb.15:
	mov	x8, #0                          ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	ldr	x16, [sp]                       ; 8-byte Reload
	b	LBB12_26
LBB12_16:
	mov	w13, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	ldr	x16, [sp]                       ; 8-byte Reload
	b	LBB12_28
LBB12_17:
	dup.2d	v0, x19
	adrp	x10, lCPI12_6@PAGE
	adrp	x9, lCPI12_7@PAGE
	cmp	x19, #16
	b.hs	LBB12_19
; %bb.18:
	mov	x8, #0                          ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	ldr	x16, [sp]                       ; 8-byte Reload
	b	LBB12_23
LBB12_19:
	and	x11, x19, #0xc
	and	x8, x19, #0xfffffff0
Lloh106:
	adrp	x12, lCPI12_0@PAGE
Lloh107:
	ldr	q1, [x12, lCPI12_0@PAGEOFF]
Lloh108:
	adrp	x12, lCPI12_1@PAGE
Lloh109:
	ldr	q2, [x12, lCPI12_1@PAGEOFF]
Lloh110:
	adrp	x12, lCPI12_2@PAGE
Lloh111:
	ldr	q3, [x12, lCPI12_2@PAGEOFF]
Lloh112:
	adrp	x12, lCPI12_3@PAGE
Lloh113:
	ldr	q5, [x12, lCPI12_3@PAGEOFF]
Lloh114:
	adrp	x12, lCPI12_4@PAGE
Lloh115:
	ldr	q6, [x12, lCPI12_4@PAGEOFF]
Lloh116:
	adrp	x12, lCPI12_5@PAGE
Lloh117:
	ldr	q7, [x12, lCPI12_5@PAGEOFF]
	movi.2d	v4, #0000000000000000
Lloh118:
	adrp	x12, lCPI12_8@PAGE
Lloh119:
	ldr	q16, [x12, lCPI12_8@PAGEOFF]
Lloh120:
	adrp	x12, lCPI12_9@PAGE
Lloh121:
	ldr	q17, [x12, lCPI12_9@PAGEOFF]
Lloh122:
	adrp	x12, lCPI12_10@PAGE
Lloh123:
	ldr	q18, [x12, lCPI12_10@PAGEOFF]
Lloh124:
	adrp	x12, lCPI12_11@PAGE
Lloh125:
	ldr	q19, [x12, lCPI12_11@PAGEOFF]
	mov	w12, #16                        ; =0x10
	dup.2d	v20, x12
	and	x12, x19, #0xfffffff0
	movi.2d	v22, #0000000000000000
	movi.2d	v23, #0000000000000000
	movi.2d	v26, #0000000000000000
	movi.2d	v27, #0000000000000000
	movi.2d	v21, #0000000000000000
	ldr	q28, [x10, lCPI12_6@PAGEOFF]
	movi.2d	v24, #0000000000000000
	movi.2d	v25, #0000000000000000
	ldr	q29, [x9, lCPI12_7@PAGEOFF]
	ldr	x16, [sp]                       ; 8-byte Reload
LBB12_20:                               ; =>This Inner Loop Header: Depth=1
	sub.2d	v30, v0, v29
	sub.2d	v31, v0, v28
	ldr	q8, [x24], #16
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v17
	add.4s	v22, v22, v31
	mla.4s	v21, v31, v30
	sub.2d	v30, v0, v7
	sub.2d	v31, v0, v6
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v18
	add.4s	v23, v23, v31
	mla.4s	v4, v31, v30
	sub.2d	v30, v0, v5
	sub.2d	v31, v0, v3
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v19
	add.4s	v26, v26, v31
	mla.4s	v24, v31, v30
	sub.2d	v30, v0, v2
	sub.2d	v31, v0, v1
	uzp1.4s	v30, v30, v31
	tbl.16b	v31, { v8 }, v16
	mla.4s	v25, v31, v30
	add.4s	v27, v27, v31
	add.2d	v7, v7, v20
	add.2d	v28, v28, v20
	add.2d	v29, v29, v20
	add.2d	v6, v6, v20
	add.2d	v5, v5, v20
	add.2d	v3, v3, v20
	add.2d	v2, v2, v20
	add.2d	v1, v1, v20
	subs	x12, x12, #16
	b.ne	LBB12_20
; %bb.21:
	add.4s	v1, v22, v26
	add.4s	v2, v23, v27
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w12, s1
	add.4s	v1, v21, v24
	add.4s	v2, v4, v25
	add.4s	v1, v1, v2
	addv.4s	s1, v1
	fmov	w13, s1
	cmp	x19, x8
	b.eq	LBB12_28
; %bb.22:
	cbz	x11, LBB12_26
LBB12_23:
	mov	x11, x8
	and	x8, x19, #0xfffffffc
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	mov.s	v2[0], w12
	mov.s	v1[0], w13
	dup.2d	v4, x11
	ldr	q3, [x10, lCPI12_6@PAGEOFF]
	orr.16b	v3, v4, v3
	ldr	q5, [x9, lCPI12_7@PAGEOFF]
	orr.16b	v4, v4, v5
	sub	x9, x11, x8
	add	x10, x11, x22
	add	x10, x23, x10
	mov	w11, #4                         ; =0x4
	dup.2d	v5, x11
LBB12_24:                               ; =>This Inner Loop Header: Depth=1
	ldr	s6, [x10], #4
	ushll.8h	v6, v6, #0
	ushll.4s	v7, v6, #0
	uaddw.4s	v2, v2, v6
	sub.2d	v6, v0, v4
	sub.2d	v16, v0, v3
	uzp1.4s	v6, v6, v16
	mla.4s	v1, v7, v6
	add.2d	v3, v3, v5
	add.2d	v4, v4, v5
	adds	x9, x9, #4
	b.ne	LBB12_24
; %bb.25:
	addv.4s	s0, v2
	fmov	w12, s0
	addv.4s	s0, v1
	fmov	w13, s0
	cmp	x19, x8
	b.eq	LBB12_28
LBB12_26:
	sub	x9, x19, x8
	add	x8, x8, x22
	add	x8, x23, x8
LBB12_27:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x8], #1
	add	w12, w12, w10
	madd	w13, w10, w9, w13
	subs	x9, x9, #1
	b.ne	LBB12_27
LBB12_28:
	ldp	w9, w8, [x16, #24]
	add	w8, w8, w13
	madd	w8, w9, w19, w8
	add	w9, w9, w12
	fmov	s0, w9
	mov.s	v0[1], w8
	mov	w8, #32881                      ; =0x8071
	movk	w8, #32775, lsl #16
	dup.2s	v1, w8
	umull.2d	v1, v0, v1
	ushr.2d	v1, v1, #47
	xtn.2s	v1, v1
	mov	w8, #65521                      ; =0xfff1
	dup.2s	v2, w8
	mls.2s	v0, v1, v2
	str	d0, [x16, #24]
	mov	w8, #1                          ; =0x1
LBB12_29:
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #32]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #16]               ; 16-byte Folded Reload
	add	sp, sp, #128
	mov	x0, x8
	ret
	.loh AdrpLdr	Lloh100, Lloh101
	.loh AdrpLdr	Lloh102, Lloh103
	.loh AdrpLdr	Lloh104, Lloh105
	.loh AdrpLdr	Lloh124, Lloh125
	.loh AdrpAdrp	Lloh122, Lloh124
	.loh AdrpLdr	Lloh122, Lloh123
	.loh AdrpAdrp	Lloh120, Lloh122
	.loh AdrpLdr	Lloh120, Lloh121
	.loh AdrpAdrp	Lloh118, Lloh120
	.loh AdrpLdr	Lloh118, Lloh119
	.loh AdrpAdrp	Lloh116, Lloh118
	.loh AdrpLdr	Lloh116, Lloh117
	.loh AdrpAdrp	Lloh114, Lloh116
	.loh AdrpLdr	Lloh114, Lloh115
	.loh AdrpAdrp	Lloh112, Lloh114
	.loh AdrpLdr	Lloh112, Lloh113
	.loh AdrpAdrp	Lloh110, Lloh112
	.loh AdrpLdr	Lloh110, Lloh111
	.loh AdrpAdrp	Lloh108, Lloh110
	.loh AdrpLdr	Lloh108, Lloh109
	.loh AdrpAdrp	Lloh106, Lloh108
	.loh AdrpLdr	Lloh106, Lloh107
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream9end_blockEv ; -- Begin function _ZN9gui_forms11compression13InflateStream9end_blockEv
	.p2align	2
__ZN9gui_forms11compression13InflateStream9end_blockEv: ; @_ZN9gui_forms11compression13InflateStream9end_blockEv
	.cfi_startproc
; %bb.0:
	add	x8, x0, #9, lsl #12             ; =36864
	mov	w9, #3                          ; =0x3
	str	w9, [x0, #32]
	ldrb	w9, [x8, #1304]
	cmp	w9, #1
	b.ne	LBB13_2
; %bb.1:
	movi.2d	v0, #0000000000000000
	str	d0, [x8, #1256]
	ldr	w8, [x0]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	str	w8, [x0, #32]
LBB13_2:
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
lCPI14_0:
	.long	19                              ; 0x13
	.long	2                               ; 0x2
lCPI14_1:
	.long	19                              ; 0x13
	.long	23                              ; 0x17
lCPI14_2:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI14_3:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI14_4:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
lCPI14_5:
	.long	19                              ; 0x13
	.long	21                              ; 0x15
lCPI14_6:
	.long	19                              ; 0x13
	.long	22                              ; 0x16
lCPI14_7:
	.long	19                              ; 0x13
	.long	20                              ; 0x14
lCPI14_8:
	.long	19                              ; 0x13
	.long	4                               ; 0x4
lCPI14_9:
	.long	19                              ; 0x13
	.long	17                              ; 0x11
lCPI14_10:
	.long	19                              ; 0x13
	.long	16                              ; 0x10
lCPI14_11:
	.long	4294967291                      ; 0xfffffffb
	.long	4294967286                      ; 0xfffffff6
lCPI14_12:
	.long	31                              ; 0x1f
	.long	15                              ; 0xf
lCPI14_13:
	.long	1                               ; 0x1
	.long	4                               ; 0x4
lCPI14_14:
	.long	19                              ; 0x13
	.long	13                              ; 0xd
lCPI14_15:
	.long	19                              ; 0x13
	.long	12                              ; 0xc
lCPI14_16:
	.long	19                              ; 0x13
	.long	11                              ; 0xb
lCPI14_17:
	.long	19                              ; 0x13
	.long	10                              ; 0xa
lCPI14_18:
	.long	19                              ; 0x13
	.long	7                               ; 0x7
lCPI14_19:
	.long	19                              ; 0x13
	.long	9                               ; 0x9
lCPI14_20:
	.long	19                              ; 0x13
	.long	8                               ; 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #64
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	str	wzr, [sp, #12]
	ldr	w9, [x0, #32]
	cmp	w9, #19
	b.hi	LBB14_3
; %bb.1:
	mov	w8, #0                          ; =0x0
	mov	w10, #38020                     ; =0x9484
	add	x20, x0, x10
	mov	w10, #32832                     ; =0x8040
	add	x21, x0, x10
Lloh126:
	adrp	x10, LJTI14_0@PAGE
Lloh127:
	add	x10, x10, LJTI14_0@PAGEOFF
	adr	x11, LBB14_2
	ldrh	w12, [x10, x9, lsl #1]
	add	x11, x11, x12, lsl #2
	br	x11
LBB14_2:
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_3:
	mov	w8, #0                          ; =0x0
Lloh128:
	adrp	x9, lCPI14_0@PAGE
Lloh129:
	ldr	d0, [x9, lCPI14_0@PAGEOFF]
	b	LBB14_195
LBB14_4:
	ldp	w3, w8, [x20, #108]
	ldr	w9, [x20, #120]
	add	w8, w8, w3
	cmp	w9, w8
	b.ne	LBB14_94
; %bb.5:
	ldrb	w8, [x20]
	cbz	w8, LBB14_152
; %bb.6:
	mov	w8, #37764                      ; =0x9384
	add	x19, x0, x8
	mov	w8, #32856                      ; =0x8058
	add	x1, x0, x8
	mov	x21, x0
	mov	x2, x19
	mov	w4, #0                          ; =0x0
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB14_176
; %bb.7:
	ldp	w8, w3, [x20, #108]
	mov	w9, #34492                      ; =0x86bc
	mov	x0, x21
	add	x1, x21, x9
	add	x2, x19, x8
	mov	w4, #0                          ; =0x0
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB14_176
; %bb.8:
	mov	w8, #11                         ; =0xb
	str	w8, [x21, #32]
	b	LBB14_203
LBB14_9:
	ldr	w9, [x20, #124]
	cmp	w9, #16
	b.ne	LBB14_97
; %bb.10:
	ldr	w8, [x20, #120]
	sub	w8, w8, #1
	add	x8, x0, x8
	mov	w10, #37764                     ; =0x9384
	ldrb	w8, [x8, x10]
	b	LBB14_98
LBB14_11:
	ldr	w9, [x20, #104]
	cmp	w9, #14
	b.hs	LBB14_106
; %bb.12:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB14_100
; %bb.13:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB14_146
; %bb.14:
	ldr	x8, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x8, x14
	b.eq	LBB14_194
; %bb.15:
	ldr	x16, [x1]
	ldr	w17, [x20, #100]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w9
	orr	w2, w2, w17
	add	w17, w9, #8
	stp	w2, w17, [x20, #100]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w9, #5
	b.hi	LBB14_184
; %bb.16:
	cmp	x3, x13
	b.eq	LBB14_100
; %bb.17:
	cmp	x4, x15
	b.eq	LBB14_146
; %bb.18:
	cmp	x8, x10
	b.eq	LBB14_194
; %bb.19:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	orr	w9, w9, #0x10
	stp	w13, w9, [x20, #100]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x8, x10
	b.ne	LBB14_107
	b	LBB14_194
LBB14_20:
	ldr	w8, [x20, #104]
	cmp	w8, #16
	b.hs	LBB14_109
; %bb.21:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB14_100
; %bb.22:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB14_146
; %bb.23:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB14_194
; %bb.24:
	ldr	x16, [x1]
	ldr	w17, [x20, #100]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	add	w17, w8, #8
	stp	w2, w17, [x20, #100]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB14_185
; %bb.25:
	cmp	x3, x13
	b.eq	LBB14_100
; %bb.26:
	cmp	x4, x15
	b.eq	LBB14_146
; %bb.27:
	cmp	x9, x10
	b.eq	LBB14_194
; %bb.28:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	orr	w8, w8, #0x10
	stp	w13, w8, [x20, #100]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB14_110
	b	LBB14_194
LBB14_29:
	ldr	w8, [x20, #136]
	ldr	w9, [x20, #104]
	cmp	w9, w8
	b.hs	LBB14_112
; %bb.30:
	mov	x11, #0                         ; =0x0
	ldr	x17, [x0, #8]
	ldr	x10, [x0, #24]
	ldp	x2, x3, [x1]
	ldr	x12, [x1, #32]
	ldr	x14, [x0, #40]
	ldr	x13, [x0, #56]
	ldr	w15, [x20, #100]
	sub	x16, x13, x10
	sub	x17, x14, x17
	add	x2, x2, x12
	sub	x3, x12, x3
LBB14_31:                               ; =>This Inner Loop Header: Depth=1
	cmn	x3, x11
	b.eq	LBB14_100
; %bb.32:                               ;   in Loop: Header=BB14_31 Depth=1
	add	x4, x17, x11
	cbz	x4, LBB14_146
; %bb.33:                               ;   in Loop: Header=BB14_31 Depth=1
	add	x4, x16, x11
	cbz	x4, LBB14_194
; %bb.34:                               ;   in Loop: Header=BB14_31 Depth=1
	add	x4, x13, x11
	add	x4, x4, #1
	str	x4, [x0, #56]
	ldrb	w4, [x2, x11]
	lsl	w4, w4, w9
	orr	w15, w4, w15
	add	w9, w9, #8
	stp	w15, w9, [x20, #100]
	add	x4, x12, x11
	add	x4, x4, #1
	str	x4, [x1, #32]
	add	x4, x14, x11
	add	x4, x4, #1
	str	x4, [x0, #40]
	add	x11, x11, #1
	cmp	w9, w8
	b.lo	LBB14_31
; %bb.35:
	add	x11, x13, x11
	b	LBB14_113
LBB14_36:
	ldr	w8, [x20, #104]
	cmp	w8, #3
	b.hs	LBB14_115
; %bb.37:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB14_100
; %bb.38:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB14_146
; %bb.39:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.40:
	ldr	x13, [x1]
	ldr	w14, [x20, #100]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	orr	w8, w8, #0x8
	stp	w13, w8, [x20, #100]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB14_116
LBB14_41:
	ldr	w8, [x20, #104]
	cmp	w8, #16
	b.hs	LBB14_120
; %bb.42:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB14_100
; %bb.43:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB14_146
; %bb.44:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB14_194
; %bb.45:
	ldr	x16, [x1]
	ldr	w17, [x20, #100]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	add	w17, w8, #8
	stp	w2, w17, [x20, #100]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB14_186
; %bb.46:
	cmp	x3, x13
	b.eq	LBB14_100
; %bb.47:
	cmp	x4, x15
	b.eq	LBB14_146
; %bb.48:
	cmp	x9, x10
	b.eq	LBB14_194
; %bb.49:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	orr	w8, w8, #0x10
	stp	w13, w8, [x20, #100]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB14_121
	b	LBB14_194
LBB14_50:
	ldp	w9, w8, [x20, #116]
	cmp	w8, w9
	b.hs	LBB14_122
; %bb.51:
	ldr	w9, [x20, #104]
	cmp	w9, #3
	b.hs	LBB14_147
; %bb.52:
	ldr	x10, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x10
	b.eq	LBB14_100
; %bb.53:
	ldr	x13, [x0, #40]
	ldr	x10, [x0, #8]
	cmp	x13, x10
	b.eq	LBB14_146
; %bb.54:
	ldr	x10, [x0, #24]
	ldr	x12, [x0, #56]
	cmp	x10, x12
	b.eq	LBB14_194
; %bb.55:
	ldr	x14, [x1]
	ldr	w15, [x20, #100]
	add	x12, x12, #1
	str	x12, [x0, #56]
	ldrb	w14, [x14, x11]
	lsl	w14, w14, w9
	orr	w14, w14, w15
	orr	w9, w9, #0x8
	stp	w14, w9, [x20, #100]
	add	x11, x11, #1
	str	x11, [x1, #32]
	add	x11, x13, #1
	str	x11, [x0, #40]
	b	LBB14_148
LBB14_56:
	ldr	w8, [x20, #144]
	cbz	w8, LBB14_143
; %bb.57:
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	b	__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
LBB14_58:
	ldr	w8, [x20, #104]
	cmp	w8, #8
	b.hs	LBB14_124
; %bb.59:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB14_100
; %bb.60:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB14_146
; %bb.61:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.62:
	ldr	x13, [x1]
	ldr	w14, [x20, #100]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	orr	w8, w8, #0x8
	stp	w13, w8, [x20, #100]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB14_125
LBB14_63:
	ldr	w8, [x20, #104]
	cmp	w8, #16
	b.hs	LBB14_128
; %bb.64:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB14_100
; %bb.65:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB14_146
; %bb.66:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB14_194
; %bb.67:
	ldr	x16, [x1]
	ldr	w17, [x20, #100]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	add	w17, w8, #8
	stp	w2, w17, [x20, #100]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB14_187
; %bb.68:
	cmp	x3, x13
	b.eq	LBB14_100
; %bb.69:
	cmp	x4, x15
	b.eq	LBB14_146
; %bb.70:
	cmp	x9, x10
	b.eq	LBB14_194
; %bb.71:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	orr	w8, w8, #0x10
	stp	w13, w8, [x20, #100]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB14_129
	b	LBB14_194
LBB14_72:
	ldr	w8, [x20, #136]
	ldr	w9, [x20, #104]
	cmp	w9, w8
	b.hs	LBB14_134
; %bb.73:
	mov	x11, #0                         ; =0x0
	ldr	x17, [x0, #8]
	ldr	x10, [x0, #24]
	ldp	x2, x3, [x1]
	ldr	x12, [x1, #32]
	ldr	x14, [x0, #40]
	ldr	x13, [x0, #56]
	ldr	w15, [x20, #100]
	sub	x16, x13, x10
	sub	x17, x14, x17
	add	x2, x2, x12
	sub	x3, x12, x3
LBB14_74:                               ; =>This Inner Loop Header: Depth=1
	cmn	x3, x11
	b.eq	LBB14_100
; %bb.75:                               ;   in Loop: Header=BB14_74 Depth=1
	add	x4, x17, x11
	cbz	x4, LBB14_146
; %bb.76:                               ;   in Loop: Header=BB14_74 Depth=1
	add	x4, x16, x11
	cbz	x4, LBB14_194
; %bb.77:                               ;   in Loop: Header=BB14_74 Depth=1
	add	x4, x13, x11
	add	x4, x4, #1
	str	x4, [x0, #56]
	ldrb	w4, [x2, x11]
	lsl	w4, w4, w9
	orr	w15, w4, w15
	add	w9, w9, #8
	stp	w15, w9, [x20, #100]
	add	x4, x12, x11
	add	x4, x4, #1
	str	x4, [x1, #32]
	add	x4, x14, x11
	add	x4, x4, #1
	str	x4, [x0, #40]
	add	x11, x11, #1
	cmp	w9, w8
	b.lo	LBB14_74
; %bb.78:
	add	x11, x13, x11
	b	LBB14_135
LBB14_79:
	ldr	w8, [x20, #128]
	cbz	w8, LBB14_201
; %bb.80:
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	b	__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
LBB14_81:
	mov	x22, x0
	mov	x19, x1
	bl	__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	tbnz	w0, #0, LBB14_203
; %bb.82:
	mov	w8, #32856                      ; =0x8058
	mov	x0, x22
	add	x2, x22, x8
	add	x3, sp, #12
	mov	x1, x19
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB14_176
; %bb.83:
	ldr	w8, [sp, #12]
	cmp	w8, #256
	b.ne	LBB14_171
; %bb.84:
	mov	w8, #3                          ; =0x3
	str	w8, [x22, #32]
	ldrb	w8, [x20, #148]
	cmp	w8, #1
	b.ne	LBB14_203
; %bb.85:
	stp	wzr, wzr, [x20, #100]
	ldr	w8, [x22]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	str	w8, [x22, #32]
	b	LBB14_203
LBB14_86:
	ldr	w8, [x20, #104]
	cmp	w8, #8
	b.hs	LBB14_138
; %bb.87:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB14_100
; %bb.88:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB14_146
; %bb.89:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.90:
	ldr	x13, [x1]
	ldr	w14, [x20, #100]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	orr	w8, w8, #0x8
	stp	w13, w8, [x20, #100]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB14_139
LBB14_91:
	mov	w8, #34492                      ; =0x86bc
	add	x2, x0, x8
	add	x3, sp, #12
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB14_176
; %bb.92:
	ldr	w8, [sp, #12]
	cmp	w8, #30
	b.lo	LBB14_151
; %bb.93:
	mov	w8, #0                          ; =0x0
Lloh130:
	adrp	x9, lCPI14_7@PAGE
Lloh131:
	ldr	d0, [x9, lCPI14_7@PAGEOFF]
	b	LBB14_168
LBB14_94:
	mov	w8, #36128                      ; =0x8d20
	add	x2, x0, x8
	add	x3, sp, #12
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB14_176
; %bb.95:
	ldr	w8, [sp, #12]
	cmp	w8, #15
	b.hi	LBB14_165
; %bb.96:
	ldr	w9, [x20, #120]
	add	x9, x19, x9
	mov	w10, #37764                     ; =0x9384
	strb	w8, [x9, x10]
	b	LBB14_150
LBB14_97:
	mov	w8, #0                          ; =0x0
LBB14_98:
	cmp	w9, #17
	mov	w10, #2                         ; =0x2
	cinc	w10, w10, eq
	mov	w11, #3                         ; =0x3
	mov	w12, #11                        ; =0xb
	cmp	w9, #18
	csel	w9, w12, w11, eq
	mov	w11, #7                         ; =0x7
	csel	w10, w11, w10, eq
	ldr	w11, [x20, #104]
	cmp	w11, w10
	b.hs	LBB14_103
; %bb.99:
	ldr	x12, [x1, #8]
	ldr	x13, [x1, #32]
	cmp	x13, x12
	b.ne	LBB14_145
LBB14_100:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB14_102
; %bb.101:
Lloh132:
	adrp	x8, lCPI14_4@PAGE
Lloh133:
	ldr	d0, [x8, lCPI14_4@PAGEOFF]
	str	d0, [x0, #32]
LBB14_102:
	mov	w8, #0                          ; =0x0
	str	wzr, [x1, #52]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_103:
	ldr	x12, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x12, x14
	b.eq	LBB14_194
LBB14_104:
	add	x13, x14, #1
	str	x13, [x0, #56]
	mov	w14, #-1                        ; =0xffffffff
	lsl	w14, w14, w10
	ldr	w15, [x20, #100]
	bic	w14, w15, w14
	lsr	w15, w15, w10
	sub	w10, w11, w10
	stp	w15, w10, [x20, #100]
	ldp	w11, w15, [x20, #108]
	ldr	w10, [x20, #120]
	add	w11, w15, w11
	sub	w11, w11, w10
	add	w9, w14, w9
	cmp	w9, w11
	b.ls	LBB14_155
; %bb.105:
	mov	w8, #0                          ; =0x0
Lloh134:
	adrp	x9, lCPI14_9@PAGE
Lloh135:
	ldr	d0, [x9, lCPI14_9@PAGEOFF]
	b	LBB14_195
LBB14_106:
	ldr	x8, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x8, x10
	b.eq	LBB14_194
LBB14_107:
	add	x11, x10, #1
	str	x11, [x0, #56]
	ldr	w12, [x20, #100]
	lsr	w13, w12, #14
	sub	w9, w9, #14
	stp	w13, w9, [x20, #100]
	and	w9, w12, #0x1f
	add	w13, w9, #257
	str	w13, [x20, #108]
	dup.2s	v0, w12
Lloh136:
	adrp	x12, lCPI14_11@PAGE
Lloh137:
	ldr	d1, [x12, lCPI14_11@PAGEOFF]
	ushl.2s	v0, v0, v1
Lloh138:
	adrp	x12, lCPI14_12@PAGE
Lloh139:
	ldr	d1, [x12, lCPI14_12@PAGEOFF]
	and.8b	v0, v0, v1
Lloh140:
	adrp	x12, lCPI14_13@PAGE
Lloh141:
	ldr	d1, [x12, lCPI14_13@PAGEOFF]
	add.2s	v1, v0, v1
	str	d1, [x20, #112]
	fmov	w12, s0
	cmp	w9, #29
	ccmp	w12, #30, #2, ls
	b.lo	LBB14_161
; %bb.108:
	mov	w8, #0                          ; =0x0
Lloh142:
	adrp	x9, lCPI14_14@PAGE
Lloh143:
	ldr	d0, [x9, lCPI14_14@PAGEOFF]
	b	LBB14_195
LBB14_109:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB14_194
LBB14_110:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	and	w10, w9, #0xffff
	lsr	w9, w9, #16
	sub	w8, w8, #16
	stp	w9, w8, [x20, #100]
	ldr	w8, [x20, #144]
	eor	w8, w10, w8
	mov	w9, #65535                      ; =0xffff
	cmp	w8, w9
	b.ne	LBB14_163
; %bb.111:
	mov	w8, #6                          ; =0x6
	b	LBB14_202
LBB14_112:
	ldr	x10, [x0, #24]
	ldr	x11, [x0, #56]
LBB14_113:
	cmp	x10, x11
	b.eq	LBB14_194
; %bb.114:
	add	x10, x11, #1
	str	x10, [x0, #56]
	mov	w10, #-1                        ; =0xffffffff
	lsl	w10, w10, w8
	ldr	w11, [x20, #100]
	bic	w10, w11, w10
	lsr	w11, w11, w8
	sub	w8, w9, w8
	stp	w11, w8, [x20, #100]
	ldr	w8, [x20, #128]
	add	w8, w8, w10
	str	w8, [x20, #128]
	mov	w8, #14                         ; =0xe
	b	LBB14_202
LBB14_115:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB14_116:
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.117:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	lsr	w10, w9, #3
	sub	w8, w8, #3
	stp	w10, w8, [x20, #100]
	and	w8, w9, #0x1
	strb	w8, [x20, #148]
	ubfx	w8, w9, #1, #2
	cmp	w8, #1
	b.gt	LBB14_159
; %bb.118:
	cbnz	w8, LBB14_174
; %bb.119:
	stp	wzr, wzr, [x20, #100]
	mov	w8, #4                          ; =0x4
	b	LBB14_202
LBB14_120:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB14_194
LBB14_121:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	and	w10, w9, #0xffff
	str	w10, [x20, #144]
	lsr	w9, w9, #16
	sub	w8, w8, #16
	stp	w9, w8, [x20, #100]
	mov	w8, #5                          ; =0x5
	b	LBB14_202
LBB14_122:
	mov	w8, #36128                      ; =0x8d20
	mov	w9, #38084                      ; =0x94c4
	add	x1, x0, x8
	add	x2, x0, x9
	mov	x19, x0
	mov	w3, #19                         ; =0x13
	mov	w4, #1                          ; =0x1
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB14_176
; %bb.123:
	str	wzr, [x20, #120]
	mov	w8, #9                          ; =0x9
	b	LBB14_181
LBB14_124:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB14_125:
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.126:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	lsr	w10, w9, #8
	sub	w8, w8, #8
	stp	w10, w8, [x20, #100]
	ldp	w10, w8, [x20, #164]
	bfi	w9, w8, #8, #24
	add	w8, w10, #1
	stp	w8, w9, [x20, #164]
	cmp	w8, #4
	b.ne	LBB14_203
; %bb.127:
	mov	w8, #0                          ; =0x0
Lloh144:
	adrp	x9, lCPI14_17@PAGE
Lloh145:
	ldr	d0, [x9, lCPI14_17@PAGEOFF]
	b	LBB14_195
LBB14_128:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB14_194
LBB14_129:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	lsr	w10, w9, #16
	sub	w8, w8, #16
	stp	w10, w8, [x20, #100]
	and	w8, w9, #0xf
	cmp	w8, #8
	b.ne	LBB14_164
; %bb.130:
	ubfx	x8, x9, #4, #4
	add	x10, x8, #8
	ldr	w8, [x0, #4]
	cmp	w10, w8
	b.hi	LBB14_179
; %bb.131:
	and	x8, x9, #0xf8
	cmp	w8, #128
	b.hs	LBB14_179
; %bb.132:
	rev	w8, w9
	lsr	w8, w8, #16
	mov	w11, #31711                     ; =0x7bdf
	mul	w8, w8, w11
	and	w8, w8, #0xffff
	cmp	w8, #2115
	b.lo	LBB14_204
; %bb.133:
	mov	w8, #0                          ; =0x0
Lloh146:
	adrp	x9, lCPI14_19@PAGE
Lloh147:
	ldr	d0, [x9, lCPI14_19@PAGEOFF]
	b	LBB14_195
LBB14_134:
	ldr	x10, [x0, #24]
	ldr	x11, [x0, #56]
LBB14_135:
	cmp	x10, x11
	b.eq	LBB14_194
; %bb.136:
	add	x10, x11, #1
	str	x10, [x0, #56]
	mov	w10, #-1                        ; =0xffffffff
	lsl	w10, w10, w8
	ldr	w11, [x20, #100]
	bic	w10, w11, w10
	lsr	w11, w11, w8
	sub	w8, w9, w8
	stp	w11, w8, [x20, #100]
	ldr	w8, [x20, #132]
	ldr	x9, [x21, #16]
	add	w8, w8, w10
	cmp	x9, x8
	str	w8, [x20, #132]
	b.hs	LBB14_153
; %bb.137:
	mov	w8, #0                          ; =0x0
Lloh148:
	adrp	x9, lCPI14_6@PAGE
Lloh149:
	ldr	d0, [x9, lCPI14_6@PAGEOFF]
	b	LBB14_195
LBB14_138:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB14_139:
	cmp	x9, x11
	b.eq	LBB14_194
; %bb.140:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x20, #100]
	lsr	w10, w9, #8
	sub	w8, w8, #8
	stp	w10, w8, [x20, #100]
	ldp	w8, w10, [x20, #160]
	bfi	w9, w8, #8, #24
	add	w8, w10, #1
	stp	w9, w8, [x20, #160]
	cmp	w8, #4
	b.ne	LBB14_203
; %bb.141:
	ldp	w10, w8, [x20, #152]
	orr	w8, w10, w8, lsl #16
	cmp	w9, w8
	b.ne	LBB14_178
; %bb.142:
	mov	w8, #18                         ; =0x12
	b	LBB14_202
LBB14_143:
	mov	w8, #3                          ; =0x3
	str	w8, [x0, #32]
	ldrb	w8, [x20, #148]
	cmp	w8, #1
	b.ne	LBB14_203
; %bb.144:
	stp	wzr, wzr, [x20, #100]
	ldr	w8, [x0]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	b	LBB14_202
LBB14_145:
	ldr	x15, [x0, #40]
	ldr	x12, [x0, #8]
	cmp	x15, x12
	b.ne	LBB14_169
LBB14_146:
	mov	w8, #0                          ; =0x0
Lloh150:
	adrp	x9, lCPI14_3@PAGE
Lloh151:
	ldr	d0, [x9, lCPI14_3@PAGEOFF]
	b	LBB14_195
LBB14_147:
	ldr	x10, [x0, #24]
	ldr	x12, [x0, #56]
LBB14_148:
	cmp	x10, x12
	b.eq	LBB14_194
; %bb.149:
	add	x10, x12, #1
	str	x10, [x0, #56]
	ldr	w10, [x20, #100]
	lsr	w11, w10, #3
	sub	w9, w9, #3
Lloh152:
	adrp	x12, __ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE@PAGE
Lloh153:
	add	x12, x12, __ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE@PAGEOFF
	stp	w11, w9, [x20, #100]
	ldr	w8, [x12, x8, lsl #2]
	and	w9, w10, #0x7
	add	x8, x0, x8
	mov	w10, #38084                     ; =0x94c4
	strb	w9, [x8, x10]
LBB14_150:
	ldr	w8, [x20, #120]
	add	w8, w8, #1
	str	w8, [x20, #120]
	b	LBB14_203
LBB14_151:
Lloh154:
	adrp	x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGE
Lloh155:
	add	x9, x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGEOFF
	ldr	w9, [x9, x8, lsl #2]
Lloh156:
	adrp	x10, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGE
Lloh157:
	add	x10, x10, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGEOFF
	ldr	w8, [x10, x8, lsl #2]
	stp	w9, w8, [x20, #132]
	mov	w8, #15                         ; =0xf
	b	LBB14_181
LBB14_152:
Lloh158:
	adrp	x9, lCPI14_10@PAGE
Lloh159:
	ldr	d0, [x9, lCPI14_10@PAGEOFF]
	b	LBB14_195
LBB14_153:
	ldr	x9, [x21, #8]
	cmp	x9, x8
	b.hs	LBB14_173
; %bb.154:
	mov	w8, #0                          ; =0x0
Lloh160:
	adrp	x9, lCPI14_5@PAGE
Lloh161:
	ldr	d0, [x9, lCPI14_5@PAGEOFF]
	b	LBB14_195
LBB14_155:
	sub	x11, x12, x13
	cmp	x11, x9
	b.lo	LBB14_194
; %bb.156:
	add	x11, x13, x9
	str	x11, [x0, #56]
	mov	w11, #37764                     ; =0x9384
	add	x11, x0, x11
LBB14_157:                              ; =>This Inner Loop Header: Depth=1
	strb	w8, [x11, w10, uxtw]
	ldr	w10, [x20, #120]
	add	w10, w10, #1
	str	w10, [x20, #120]
	subs	w9, w9, #1
	b.ne	LBB14_157
; %bb.158:
	mov	w8, #9                          ; =0x9
	b	LBB14_202
LBB14_159:
	cmp	w8, #2
	b.ne	LBB14_177
; %bb.160:
	mov	w8, #7                          ; =0x7
	b	LBB14_202
LBB14_161:
	sub	x8, x8, x11
	cmp	x8, #18
	b.ls	LBB14_194
; %bb.162:
	add	x8, x10, #20
	str	x8, [x0, #56]
	mov	w8, #38084                      ; =0x94c4
	add	x8, x0, x8
	movi.2d	v0, #0000000000000000
	str	q0, [x8]
	stur	wzr, [x8, #15]
	str	wzr, [x20, #120]
	mov	w8, #8                          ; =0x8
	b	LBB14_202
LBB14_163:
	mov	w8, #0                          ; =0x0
Lloh162:
	adrp	x9, lCPI14_15@PAGE
Lloh163:
	ldr	d0, [x9, lCPI14_15@PAGEOFF]
	b	LBB14_195
LBB14_164:
	mov	w8, #0                          ; =0x0
Lloh164:
	adrp	x9, lCPI14_18@PAGE
Lloh165:
	ldr	d0, [x9, lCPI14_18@PAGEOFF]
	b	LBB14_195
LBB14_165:
	cmp	w8, #16
	b.ne	LBB14_180
; %bb.166:
	ldr	w9, [x20, #120]
	cbnz	w9, LBB14_180
; %bb.167:
	mov	w8, #0                          ; =0x0
Lloh166:
	adrp	x9, lCPI14_9@PAGE
Lloh167:
	ldr	d0, [x9, lCPI14_9@PAGEOFF]
LBB14_168:
	str	d0, [x19, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_169:
	ldr	x12, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x12, x14
	b.eq	LBB14_194
; %bb.170:
	ldr	x16, [x1]
	ldr	w17, [x20, #100]
	add	x14, x14, #1
	str	x14, [x0, #56]
	ldrb	w16, [x16, x13]
	lsl	w16, w16, w11
	orr	w16, w16, w17
	add	w11, w11, #8
	stp	w16, w11, [x20, #100]
	add	x13, x13, #1
	str	x13, [x1, #32]
	add	x13, x15, #1
	str	x13, [x0, #40]
	cmp	x12, x14
	b.ne	LBB14_104
	b	LBB14_194
LBB14_171:
	cmp	w8, #286
	mov	x0, x22
	b.lo	LBB14_182
; %bb.172:
	mov	w8, #0                          ; =0x0
	movi.2s	v0, #19
	b	LBB14_195
LBB14_173:
	mov	w8, #16                         ; =0x10
	b	LBB14_202
LBB14_174:
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream11fixed_treesEv
	cbz	w0, LBB14_176
; %bb.175:
	mov	w8, #11                         ; =0xb
	b	LBB14_181
LBB14_176:
	mov	w8, #0                          ; =0x0
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_177:
	mov	w8, #0                          ; =0x0
Lloh168:
	adrp	x9, lCPI14_16@PAGE
Lloh169:
	ldr	d0, [x9, lCPI14_16@PAGEOFF]
	b	LBB14_195
LBB14_178:
	mov	w8, #0                          ; =0x0
Lloh170:
	adrp	x9, lCPI14_1@PAGE
Lloh171:
	ldr	d0, [x9, lCPI14_1@PAGEOFF]
	b	LBB14_195
LBB14_179:
	mov	w8, #0                          ; =0x0
Lloh172:
	adrp	x9, lCPI14_20@PAGE
Lloh173:
	ldr	d0, [x9, lCPI14_20@PAGEOFF]
	b	LBB14_195
LBB14_180:
	str	w8, [x20, #124]
	mov	w8, #10                         ; =0xa
LBB14_181:
	str	w8, [x19, #32]
	b	LBB14_203
LBB14_182:
	subs	w9, w8, #257
	b.lo	LBB14_188
; %bb.183:
Lloh174:
	adrp	x8, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGE
Lloh175:
	add	x8, x8, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGEOFF
	ldr	w8, [x8, w9, uxtw #2]
	str	w8, [x20, #128]
Lloh176:
	adrp	x8, __ZN9gui_forms11compression12_GLOBAL__N_112length_extraE@PAGE
Lloh177:
	add	x8, x8, __ZN9gui_forms11compression12_GLOBAL__N_112length_extraE@PAGEOFF
	ldr	w8, [x8, w9, uxtw #2]
	str	w8, [x20, #136]
	mov	w8, #13                         ; =0xd
	b	LBB14_202
LBB14_184:
	mov	x9, x17
	cmp	x8, x10
	b.ne	LBB14_107
	b	LBB14_194
LBB14_185:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB14_110
	b	LBB14_194
LBB14_186:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB14_121
	b	LBB14_194
LBB14_187:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB14_129
	b	LBB14_194
LBB14_188:
	str	w8, [x20, #140]
	mov	w8, #12                         ; =0xc
	str	w8, [x0, #32]
	mov	x1, x19
LBB14_189:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.ne	LBB14_191
; %bb.190:
	mov	w8, #0                          ; =0x0
Lloh178:
	adrp	x9, lCPI14_8@PAGE
Lloh179:
	ldr	d0, [x9, lCPI14_8@PAGEOFF]
	b	LBB14_195
LBB14_191:
	ldr	x9, [x1, #40]
	ldr	x8, [x1, #24]
	cmp	x9, x8
	b.ne	LBB14_193
; %bb.192:
	mov	w8, #0                          ; =0x0
	mov	w9, #1                          ; =0x1
	str	w9, [x1, #52]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_193:
	ldr	x10, [x0, #24]
	ldr	x8, [x0, #56]
	cmp	x10, x8
	b.ne	LBB14_196
LBB14_194:
	mov	w8, #0                          ; =0x0
Lloh180:
	adrp	x9, lCPI14_2@PAGE
Lloh181:
	ldr	d0, [x9, lCPI14_2@PAGEOFF]
LBB14_195:
	str	d0, [x0, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_196:
	add	x8, x8, #1
	str	x8, [x0, #56]
	ldr	w8, [x20, #140]
	ldr	x10, [x1, #16]
	strb	w8, [x10, x9]
	ldr	x9, [x1, #40]
	add	x9, x9, #1
	str	x9, [x1, #40]
	ldr	x9, [x0, #48]
	add	x9, x9, #1
	str	x9, [x0, #48]
	ldr	x9, [x21]
	add	x9, x0, x9
	strb	w8, [x9, #64]
	ldr	w9, [x21]
	add	w9, w9, #1
	and	x9, x9, #0x7fff
	str	x9, [x21]
	ldr	x9, [x21, #8]
	lsr	x10, x9, #15
	cbnz	x10, LBB14_198
; %bb.197:
	add	x9, x9, #1
	str	x9, [x21, #8]
LBB14_198:
	ldr	w9, [x0]
	cmp	w9, #1
	b.eq	LBB14_201
; %bb.199:
	and	w9, w8, #0xff
	mov	w8, #-65521                     ; =0xffff000f
	ldp	w10, w11, [x20, #152]
	add	w9, w10, w9
	add	w10, w9, w8
	mov	w12, #65520                     ; =0xfff0
	cmp	w9, w12
	csel	w10, w10, w9, hi
	mov	w12, #65521                     ; =0xfff1
	add	w9, w10, w11
	cmp	w9, w12
	stp	w10, w9, [x20, #152]
	b.lo	LBB14_201
; %bb.200:
	add	w8, w9, w8
	str	w8, [x20, #156]
LBB14_201:
	mov	w8, #11                         ; =0xb
LBB14_202:
	str	w8, [x0, #32]
LBB14_203:
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB14_204:
	mov	w8, #1                          ; =0x1
	lsl	x10, x8, x10
	str	x10, [x21, #16]
	tst	w9, #0x2000
	mov	w9, #2                          ; =0x2
	cinc	w9, w9, eq
	str	w9, [x0, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
	.loh AdrpAdd	Lloh126, Lloh127
	.loh AdrpLdr	Lloh128, Lloh129
	.loh AdrpLdr	Lloh130, Lloh131
	.loh AdrpLdr	Lloh132, Lloh133
	.loh AdrpLdr	Lloh134, Lloh135
	.loh AdrpLdr	Lloh140, Lloh141
	.loh AdrpAdrp	Lloh138, Lloh140
	.loh AdrpLdr	Lloh138, Lloh139
	.loh AdrpAdrp	Lloh136, Lloh138
	.loh AdrpLdr	Lloh136, Lloh137
	.loh AdrpLdr	Lloh142, Lloh143
	.loh AdrpLdr	Lloh144, Lloh145
	.loh AdrpLdr	Lloh146, Lloh147
	.loh AdrpLdr	Lloh148, Lloh149
	.loh AdrpLdr	Lloh150, Lloh151
	.loh AdrpAdd	Lloh152, Lloh153
	.loh AdrpAdd	Lloh156, Lloh157
	.loh AdrpAdd	Lloh154, Lloh155
	.loh AdrpLdr	Lloh158, Lloh159
	.loh AdrpLdr	Lloh160, Lloh161
	.loh AdrpLdr	Lloh162, Lloh163
	.loh AdrpLdr	Lloh164, Lloh165
	.loh AdrpLdr	Lloh166, Lloh167
	.loh AdrpLdr	Lloh168, Lloh169
	.loh AdrpLdr	Lloh170, Lloh171
	.loh AdrpLdr	Lloh172, Lloh173
	.loh AdrpAdd	Lloh176, Lloh177
	.loh AdrpAdd	Lloh174, Lloh175
	.loh AdrpLdr	Lloh178, Lloh179
	.loh AdrpLdr	Lloh180, Lloh181
	.cfi_endproc
	.section	__TEXT,__const
	.p2align	1, 0x0
LJTI14_0:
	.short	(LBB14_3-LBB14_2)>>2
	.short	(LBB14_63-LBB14_2)>>2
	.short	(LBB14_58-LBB14_2)>>2
	.short	(LBB14_36-LBB14_2)>>2
	.short	(LBB14_41-LBB14_2)>>2
	.short	(LBB14_20-LBB14_2)>>2
	.short	(LBB14_56-LBB14_2)>>2
	.short	(LBB14_11-LBB14_2)>>2
	.short	(LBB14_50-LBB14_2)>>2
	.short	(LBB14_4-LBB14_2)>>2
	.short	(LBB14_9-LBB14_2)>>2
	.short	(LBB14_81-LBB14_2)>>2
	.short	(LBB14_189-LBB14_2)>>2
	.short	(LBB14_29-LBB14_2)>>2
	.short	(LBB14_91-LBB14_2)>>2
	.short	(LBB14_72-LBB14_2)>>2
	.short	(LBB14_79-LBB14_2)>>2
	.short	(LBB14_86-LBB14_2)>>2
	.short	(LBB14_2-LBB14_2)>>2
	.short	(LBB14_2-LBB14_2)>>2
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
lCPI15_0:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	.p2align	2
__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb: ; @_ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #96
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	x19, x8
	stp	x1, x2, [sp, #8]
	stp	x3, x4, [sp, #24]
	stp	xzr, xzr, [sp, #40]
	strb	w5, [sp, #56]
	str	wzr, [sp, #60]
	cbz	x2, LBB15_3
; %bb.1:
	cbz	x4, LBB15_3
; %bb.2:
	sub	x8, x3, x1
	cmp	x8, x2
	cset	w8, lo
	subs	x9, x1, x3
	cmp	x9, x4
	cset	w9, lo
	cmp	x1, x3
	csel	w9, w9, w8, hi
	b	LBB15_4
LBB15_3:
	mov	w9, #0                          ; =0x0
	mov	w8, #0                          ; =0x0
	cbz	x2, LBB15_5
LBB15_4:
	subs	x8, x20, x1
	cmp	x8, x2
	cset	w8, lo
	sub	x10, x1, x20
	lsr	x10, x10, #4
	cmp	x10, #2387
	cset	w10, lo
	cmp	x20, x1
	csel	w8, w10, w8, lo
	orr	w8, w8, w9
LBB15_5:
	subs	x9, x20, x3
	cmp	x9, x4
	cset	w9, lo
	sub	x10, x3, x20
	lsr	x10, x10, #4
	cmp	x10, #2387
	cset	w10, lo
	cmp	x20, x3
	csel	w9, w10, w9, lo
	cmp	x4, #0
	csel	w10, wzr, w9, eq
	ldr	w9, [x20, #32]
	orr	w8, w8, w10
	cmp	w8, #1
	b.ne	LBB15_8
; %bb.6:
	and	w8, w9, #0xfffffffe
	cmp	w8, #18
	b.eq	LBB15_8
; %bb.7:
	mov	w8, #1                          ; =0x1
	b	LBB15_10
LBB15_8:
	cbnz	w9, LBB15_11
; %bb.9:
	mov	w8, #2                          ; =0x2
LBB15_10:
	mov	w9, #19                         ; =0x13
	stp	w9, w8, [x20, #32]
LBB15_11:                               ; =>This Inner Loop Header: Depth=1
	ldr	w8, [x20, #32]
	and	w9, w8, #0xfffffffe
	cmp	w9, #18
	b.eq	LBB15_15
; %bb.12:                               ;   in Loop: Header=BB15_11 Depth=1
	ldr	x9, [x20, #24]
	ldr	x8, [x20, #56]
	cmp	x9, x8
	b.eq	LBB15_18
; %bb.13:                               ;   in Loop: Header=BB15_11 Depth=1
	add	x8, x8, #1
	str	x8, [x20, #56]
	add	x1, sp, #8
	mov	x0, x20
	bl	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	tbnz	w0, #0, LBB15_11
; %bb.14:
	ldr	w8, [x20, #32]
LBB15_15:
	cmp	w8, #18
	b.eq	LBB15_20
; %bb.16:
	cmp	w8, #19
	b.eq	LBB15_19
; %bb.17:
	ldr	w8, [sp, #60]
	b	LBB15_21
LBB15_18:
Lloh182:
	adrp	x8, lCPI15_0@PAGE
Lloh183:
	ldr	d0, [x8, lCPI15_0@PAGEOFF]
	str	d0, [x20, #32]
LBB15_19:
	mov	w8, #3                          ; =0x3
	b	LBB15_21
LBB15_20:
	mov	w8, #2                          ; =0x2
LBB15_21:
	ldr	w9, [x20, #36]
	stp	w8, w9, [x19]
	ldur	q0, [sp, #40]
	stur	q0, [x19, #8]
	ldur	q0, [x20, #40]
	stur	q0, [x19, #24]
	ldr	x8, [x20, #56]
	str	x8, [x19, #40]
	mov	w8, #38188                      ; =0x952c
	ldr	w8, [x20, x8]
	str	w8, [x19, #48]
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #96
	ret
	.loh AdrpLdr	Lloh182, Lloh183
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ; @_ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE
__ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE:
	.long	16                              ; 0x10
	.long	17                              ; 0x11
	.long	18                              ; 0x12
	.long	0                               ; 0x0
	.long	8                               ; 0x8
	.long	7                               ; 0x7
	.long	9                               ; 0x9
	.long	6                               ; 0x6
	.long	10                              ; 0xa
	.long	5                               ; 0x5
	.long	11                              ; 0xb
	.long	4                               ; 0x4
	.long	12                              ; 0xc
	.long	3                               ; 0x3
	.long	13                              ; 0xd
	.long	2                               ; 0x2
	.long	14                              ; 0xe
	.long	1                               ; 0x1
	.long	15                              ; 0xf

	.p2align	2, 0x0                          ; @_ZN9gui_forms11compression12_GLOBAL__N_111length_baseE
__ZN9gui_forms11compression12_GLOBAL__N_111length_baseE:
	.long	3                               ; 0x3
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.long	7                               ; 0x7
	.long	8                               ; 0x8
	.long	9                               ; 0x9
	.long	10                              ; 0xa
	.long	11                              ; 0xb
	.long	13                              ; 0xd
	.long	15                              ; 0xf
	.long	17                              ; 0x11
	.long	19                              ; 0x13
	.long	23                              ; 0x17
	.long	27                              ; 0x1b
	.long	31                              ; 0x1f
	.long	35                              ; 0x23
	.long	43                              ; 0x2b
	.long	51                              ; 0x33
	.long	59                              ; 0x3b
	.long	67                              ; 0x43
	.long	83                              ; 0x53
	.long	99                              ; 0x63
	.long	115                             ; 0x73
	.long	131                             ; 0x83
	.long	163                             ; 0xa3
	.long	195                             ; 0xc3
	.long	227                             ; 0xe3
	.long	258                             ; 0x102

	.p2align	2, 0x0                          ; @_ZN9gui_forms11compression12_GLOBAL__N_112length_extraE
__ZN9gui_forms11compression12_GLOBAL__N_112length_extraE:
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	1                               ; 0x1
	.long	1                               ; 0x1
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	2                               ; 0x2
	.long	2                               ; 0x2
	.long	2                               ; 0x2
	.long	3                               ; 0x3
	.long	3                               ; 0x3
	.long	3                               ; 0x3
	.long	3                               ; 0x3
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	5                               ; 0x5
	.long	5                               ; 0x5
	.long	5                               ; 0x5
	.long	0                               ; 0x0

	.p2align	2, 0x0                          ; @_ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE
__ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE:
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	3                               ; 0x3
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	7                               ; 0x7
	.long	9                               ; 0x9
	.long	13                              ; 0xd
	.long	17                              ; 0x11
	.long	25                              ; 0x19
	.long	33                              ; 0x21
	.long	49                              ; 0x31
	.long	65                              ; 0x41
	.long	97                              ; 0x61
	.long	129                             ; 0x81
	.long	193                             ; 0xc1
	.long	257                             ; 0x101
	.long	385                             ; 0x181
	.long	513                             ; 0x201
	.long	769                             ; 0x301
	.long	1025                            ; 0x401
	.long	1537                            ; 0x601
	.long	2049                            ; 0x801
	.long	3073                            ; 0xc01
	.long	4097                            ; 0x1001
	.long	6145                            ; 0x1801
	.long	8193                            ; 0x2001
	.long	12289                           ; 0x3001
	.long	16385                           ; 0x4001
	.long	24577                           ; 0x6001

	.p2align	2, 0x0                          ; @_ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE
__ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE:
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	2                               ; 0x2
	.long	3                               ; 0x3
	.long	3                               ; 0x3
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.long	6                               ; 0x6
	.long	7                               ; 0x7
	.long	7                               ; 0x7
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	9                               ; 0x9
	.long	9                               ; 0x9
	.long	10                              ; 0xa
	.long	10                              ; 0xa
	.long	11                              ; 0xb
	.long	11                              ; 0xb
	.long	12                              ; 0xc
	.long	12                              ; 0xc
	.long	13                              ; 0xd
	.long	13                              ; 0xd

.subsections_via_symbols
