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
	.globl	__ZN9gui_forms11compression13InflateStream6chargeEy ; -- Begin function _ZN9gui_forms11compression13InflateStream6chargeEy
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	cmp	x1, x8
	cset	w0, ls
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj ; -- Begin function _ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB3_10:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB3_12
; %bb.11:
	mov	x8, #19                         ; =0x13
	movk	x8, #6, lsl #32
	str	x8, [x0, #32]
LBB3_12:
	str	wzr, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB3_13:
	mov	x9, #19                         ; =0x13
	movk	x9, #3, lsl #32
	str	x9, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj ; -- Begin function _ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.p2align	2
__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj: ; @_ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.cfi_startproc
; %bb.0:
	stp	x20, x19, [sp, #-16]!           ; 16-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	add	x9, x0, #9, lsl #12             ; =36864
	ldr	w10, [x9, #1240]
	cmp	w10, #1
	b.ne	LBB4_3
; %bb.1:
	ldr	w8, [x9, #1260]
	mov	w11, #16                        ; =0x10
	sub	w11, w11, w8
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
	ldr	w8, [x2, #1632]
	cmp	w10, w8
	b.ls	LBB4_5
LBB4_4:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #18, lsl #32
	str	x9, [x0, #32]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_5:
	ldr	x11, [x0, #24]
	ldr	x12, [x1, #8]
	mov	w13, #8                         ; =0x8
	ldr	w14, [x9, #1260]
LBB4_6:                                 ; =>This Inner Loop Header: Depth=1
	cbz	w14, LBB4_8
; %bb.7:                                ;   in Loop: Header=BB4_6 Depth=1
	ldr	x16, [x0, #56]
	sub	w14, w14, #1
	cmp	x11, x16
	b.ne	LBB4_12
	b	LBB4_15
LBB4_8:                                 ;   in Loop: Header=BB4_6 Depth=1
	ldr	x14, [x1, #32]
	cmp	x14, x12
	b.eq	LBB4_22
; %bb.9:                                ;   in Loop: Header=BB4_6 Depth=1
	ldr	x17, [x0, #40]
	ldr	x15, [x0, #8]
	cmp	x17, x15
	b.eq	LBB4_26
; %bb.10:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	x15, [x0, #24]
	ldr	x16, [x0, #56]
	cmp	x15, x16
	b.eq	LBB4_15
; %bb.11:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	x4, [x1]
	ldr	w5, [x9, #1256]
	add	x16, x16, #1
	str	x16, [x0, #56]
	ldrb	w4, [x4, x14]
	orr	w4, w5, w4
	str	w4, [x9, #1256]
	str	w13, [x9, #1260]
	add	x14, x14, #1
	str	x14, [x1, #32]
	add	x14, x17, #1
	str	x14, [x0, #40]
	mov	w14, #7                         ; =0x7
	cmp	x15, x16
	b.eq	LBB4_15
LBB4_12:                                ;   in Loop: Header=BB4_6 Depth=1
	add	x15, x16, #1
	str	x15, [x0, #56]
	ldr	w15, [x9, #1256]
	and	w16, w15, #0x1
	lsr	w15, w15, #1
	str	w15, [x9, #1256]
	str	w14, [x9, #1260]
	ldr	w15, [x9, #1244]
	orr	w15, w15, w16
	ldrh	w16, [x2, w10, uxtw #1]
	ldr	w4, [x9, #1248]
	add	w17, w4, w16
	subs	w4, w15, w4
	ccmp	w15, w17, #2, hs
	b.lo	LBB4_16
; %bb.13:                               ;   in Loop: Header=BB4_6 Depth=1
	ldr	w4, [x9, #1252]
	add	w16, w4, w16
	str	w16, [x9, #1252]
	lsl	w16, w17, #1
	str	w16, [x9, #1248]
	lsl	w15, w15, #1
	str	w15, [x9, #1244]
	add	w10, w10, #1
	str	w10, [x9, #1240]
	cmp	w10, w8
	b.ls	LBB4_6
	b	LBB4_4
LBB4_14:
	ldr	x11, [x0, #24]
	ldr	x5, [x0, #56]
	cmp	x11, x5
	b.ne	LBB4_17
LBB4_15:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_16:
	ldr	w8, [x9, #1252]
	add	w8, w4, w8
	add	x8, x2, w8, uxtw #1
	ldrh	w8, [x8, #32]
	str	w8, [x3]
	mov	w8, #1                          ; =0x1
	str	x8, [x9, #1240]
	str	xzr, [x9, #1248]
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_17:
	add	x13, x5, #1
	str	x13, [x0, #56]
	ldr	w12, [x9, #1256]
	ldr	x7, [x1]
	add	x19, x7, x14
	ldrb	w16, [x19]
	lsl	w16, w16, w8
	orr	w16, w16, w12
	cmp	w15, #2
	b.ne	LBB4_19
; %bb.18:
	ldrb	w15, [x19, #1]
	add	w19, w8, #8
	lsl	w15, w15, w19
	orr	w16, w15, w16
LBB4_19:
	and	w15, w16, #0x1ff
	add	x15, x2, w15, uxtw #1
	ldrh	w15, [x15, #608]
	ands	w16, w15, #0xf
	b.eq	LBB4_3
; %bb.20:
	cmp	w8, w16
	b.hs	LBB4_38
; %bb.21:
	cmp	x4, x14
	b.ne	LBB4_25
LBB4_22:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB4_24
; %bb.23:
	mov	x8, #19                         ; =0x13
	movk	x8, #6, lsl #32
	str	x8, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #3, lsl #32
	str	x9, [x0, #32]
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
	lsl	w10, w10, w8
	orr	w12, w10, w12
	str	w12, [x9, #1256]
	add	w10, w8, #8
	str	w10, [x9, #1260]
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
	str	w12, [x9, #1256]
	add	w10, w8, #16
	str	w10, [x9, #1260]
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
	str	w12, [x9, #1256]
	add	w8, w8, #24
	str	w8, [x9, #1260]
	add	x10, x14, #3
	str	x10, [x1, #32]
	add	x10, x17, #3
	str	x10, [x0, #40]
	cmp	x11, x13
	b.eq	LBB4_15
	b	LBB4_39
LBB4_37:
	mov	x8, x10
LBB4_38:
	cmp	x11, x13
	b.eq	LBB4_15
LBB4_39:
	add	x10, x13, #1
	str	x10, [x0, #56]
	lsr	w10, w12, w16
	str	w10, [x9, #1256]
	sub	w8, w8, w16
	str	w8, [x9, #1260]
	lsr	w8, w15, #4
	str	w8, [x3]
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ldp	x20, x19, [sp], #16             ; 16-byte Folded Reload
	ret
LBB4_40:
	mov	x8, x10
	cmp	x11, x13
	b.eq	LBB4_15
	b	LBB4_39
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb ; -- Begin function _ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
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
	mov	x8, #19                         ; =0x13
	movk	x8, #5, lsl #32
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
	mov	x8, #19                         ; =0x13
	movk	x8, #14, lsl #32
	b	LBB5_26
LBB5_24:
	cbz	w23, LBB5_65
LBB5_25:
	mov	w0, #0                          ; =0x0
	mov	x8, #19                         ; =0x13
	movk	x8, #15, lsl #32
LBB5_26:
	str	x8, [x22, #32]
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
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream11fixed_treesEv ; -- Begin function _ZN9gui_forms11compression13InflateStream11fixed_treesEv
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
	mov	x8, #19                         ; =0x13
	movk	x8, #5, lsl #32
	str	x8, [x0, #32]
LBB6_4:
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE ; -- Begin function _ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.ne	LBB7_2
; %bb.1:
	mov	x9, #19                         ; =0x13
	movk	x9, #4, lsl #32
	str	x9, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB7_6:
	add	x8, x8, #1
	str	x8, [x0, #56]
	mov	w0, #1                          ; =0x1
	ret
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
	.globl	__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm ; -- Begin function _ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
	.p2align	2
__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm: ; @_ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
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
	mov	x19, x2
	mov	x21, x1
	mov	x20, x0
	mov	w8, #32832                      ; =0x8040
	add	x25, x0, x8
	ldr	x23, [x1, #16]
	ldr	x24, [x1, #40]
	add	x26, x23, x24
	add	x8, x0, #64
	ldr	x9, [x25]
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
	add	x1, x26, x22
	bl	_memcpy
	ldr	x8, [x25, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x27, lo
	ldr	w9, [x25]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x25]
	ldr	x8, [x21, #40]
	add	x8, x8, x19
	str	x8, [x21, #40]
	ldr	x8, [x20, #48]
	add	x8, x8, x19
	str	x8, [x20, #48]
	ldr	w8, [x20]
	cmp	w8, #1
	b.eq	LBB9_12
; %bb.1:
	mov	w8, #38172                      ; =0x951c
	add	x8, x20, x8
	cbz	w19, LBB9_4
; %bb.2:
	mov	w9, w19
	sub	x10, x9, #1
	and	x9, x19, #0x3
	cmp	x10, #3
	b.hs	LBB9_5
; %bb.3:
	mov	x13, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w10, #0                         ; =0x0
	b	LBB9_9
LBB9_4:
	mov	w10, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	b	LBB9_11
LBB9_5:
	mov	x11, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	mov	w10, #0                         ; =0x0
	and	x16, x19, #0xfffffffc
	neg	x16, x16
	sub	w17, w19, #3
	sub	w0, w19, #2
	add	x1, x24, x23
	add	x1, x1, #1
	sub	w2, w19, #1
LBB9_6:                                 ; =>This Inner Loop Header: Depth=1
	ldurb	w3, [x1, #-1]
	add	w15, w15, w3
	add	w4, w19, w11
	madd	w10, w4, w3, w10
	ldrb	w3, [x1]
	add	w12, w12, w3
	add	w4, w2, w11
	madd	w10, w4, w3, w10
	ldrb	w3, [x1, #1]
	add	w13, w13, w3
	add	w4, w0, w11
	madd	w10, w4, w3, w10
	ldrb	w3, [x1, #2]
	add	w14, w14, w3
	add	w4, w17, w11
	madd	w10, w4, w3, w10
	sub	x11, x11, #4
	add	x1, x1, #4
	cmp	x16, x11
	b.ne	LBB9_6
; %bb.7:
	add	w12, w12, w15
	add	w13, w14, w13
	add	w12, w13, w12
	cbz	x9, LBB9_11
; %bb.8:
	neg	x13, x11
LBB9_9:
	sub	w11, w19, w13
	add	x14, x23, x24
	add	x13, x14, x13
LBB9_10:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w14, [x13], #1
	add	w12, w12, w14
	madd	w10, w11, w14, w10
	sub	w11, w11, #1
	subs	x9, x9, #1
	b.ne	LBB9_10
LBB9_11:
	ldp	w11, w9, [x8]
	add	w9, w9, w10
	madd	w9, w11, w19, w9
	mov	w10, #32881                     ; =0x8071
	movk	w10, #32775, lsl #16
	umull	x13, w9, w10
	lsr	x13, x13, #47
	mov	w14, #65521                     ; =0xfff1
	msub	w9, w13, w14, w9
	add	w11, w11, w12
	umull	x10, w11, w10
	lsr	x10, x10, #47
	msub	w10, w10, w14, w11
	stp	w10, w9, [x8]
LBB9_12:
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE ; -- Begin function _ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	.p2align	2
__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE: ; @_ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
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
	add	x24, x0, x9
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
	add	x25, x23, x22
	add	x8, x0, #64
	ldr	x9, [x24]
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
	add	x1, x25, x19
	bl	_memcpy
	ldr	x8, [x24, #8]
	add	x8, x8, x21
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x26, lo
	ldr	w9, [x24]
	add	w9, w9, w21
	and	x9, x9, #0x7fff
	stp	x9, x8, [x24]
	ldr	x8, [x28, #40]
	add	x8, x8, x21
	str	x8, [x28, #40]
	ldr	x8, [x27, #48]
	add	x8, x8, x21
	str	x8, [x27, #48]
	ldr	w8, [x27]
	cmp	w8, #1
	b.eq	LBB10_20
; %bb.8:
	cbz	w21, LBB10_12
; %bb.9:
	mov	w8, w21
	sub	x9, x8, #1
	and	x8, x21, #0x3
	cmp	x9, #3
	b.hs	LBB10_13
; %bb.10:
	mov	x12, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	b	LBB10_17
LBB10_11:
	mov	w0, #0                          ; =0x0
	b	LBB10_21
LBB10_12:
	mov	w9, #0                          ; =0x0
	mov	w11, #0                         ; =0x0
	b	LBB10_19
LBB10_13:
	mov	x10, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	and	x15, x21, #0xfffffffc
	neg	x15, x15
	sub	w16, w21, #3
	sub	w17, w21, #2
	add	x0, x22, x23
	add	x0, x0, #1
	sub	w1, w21, #1
LBB10_14:                               ; =>This Inner Loop Header: Depth=1
	ldurb	w2, [x0, #-1]
	add	w14, w14, w2
	add	w3, w21, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0]
	add	w11, w11, w2
	add	w3, w1, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0, #1]
	add	w12, w12, w2
	add	w3, w17, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0, #2]
	add	w13, w13, w2
	add	w3, w16, w10
	madd	w9, w2, w3, w9
	sub	x10, x10, #4
	add	x0, x0, #4
	cmp	x15, x10
	b.ne	LBB10_14
; %bb.15:
	add	w11, w11, w14
	add	w12, w13, w12
	add	w11, w12, w11
	cbz	x8, LBB10_19
; %bb.16:
	neg	x12, x10
LBB10_17:
	sub	w10, w21, w12
	add	x13, x23, x22
	add	x12, x13, x12
LBB10_18:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x12], #1
	add	w11, w11, w13
	madd	w9, w13, w10, w9
	sub	w10, w10, #1
	subs	x8, x8, #1
	b.ne	LBB10_18
LBB10_19:
	ldp	w10, w8, [x20, #68]
	add	w8, w8, w9
	madd	w8, w10, w21, w8
	mov	w9, #32881                      ; =0x8071
	movk	w9, #32775, lsl #16
	umull	x12, w8, w9
	lsr	x12, x12, #47
	mov	w13, #65521                     ; =0xfff1
	msub	w8, w12, w13, w8
	add	w10, w10, w11
	umull	x9, w10, w9
	lsr	x9, x9, #47
	msub	w9, w9, w13, w10
	stp	w9, w8, [x20, #68]
LBB10_20:
	mov	w0, #1                          ; =0x1
LBB10_21:
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE ; -- Begin function _ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
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
	mov	x9, #19                         ; =0x13
	movk	x9, #4, lsl #32
	str	x9, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
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
	mov	x8, #19                         ; =0x13
	movk	x8, #6, lsl #32
	str	x8, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #3, lsl #32
	str	x9, [x0, #32]
	mov	x0, x8
	ret
LBB11_12:
	sub	sp, sp, #112
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
	mov	w17, #38164                     ; =0x9514
	add	x21, x0, x17
	ldr	w24, [x21]
	mov	w17, #4096                      ; =0x1000
	cmp	w24, #1, lsl #12                ; =4096
	csel	w17, w24, w17, lo
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	b	LBB11_26
LBB11_14:
	mov	w12, #32832                     ; =0x8040
	add	x25, x0, x12
	add	x9, x11, x9
	str	x9, [x0, #56]
	ldr	x9, [x1, #16]
	ldr	x11, [x1]
	mov	x26, x0
	add	x0, x9, x8
	mov	x27, x1
	add	x1, x11, x10
	mov	x2, x19
	bl	_memcpy
	ldp	x8, x11, [x27, #32]
	add	x8, x8, x19
	str	x8, [x27, #32]
	ldr	x8, [x26, #40]
	add	x8, x8, x19
	str	x8, [x26, #40]
	ldr	w8, [x21]
	sub	w8, w8, w19
	str	w8, [x21]
	ldr	x23, [x27, #16]
	add	x28, x23, x11
	add	x8, x26, #64
	ldr	x9, [x25]
	mov	w22, #32768                     ; =0x8000
	sub	x10, x22, x9
	cmp	x10, x19
	csel	x20, x10, x19, lo
	add	x0, x8, x9
	str	x11, [sp, #8]                   ; 8-byte Spill
	add	x1, x23, x11
	mov	x2, x20
	bl	_memcpy
	sub	x2, x19, x20
	add	x0, x26, #64
	add	x1, x28, x20
	bl	_memcpy
	ldr	x8, [x25, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x22, lo
	ldr	w9, [x25]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x25]
	ldr	x8, [x27, #40]
	add	x8, x8, x19
	str	x8, [x27, #40]
	ldr	x8, [x26, #48]
	add	x8, x8, x19
	str	x8, [x26, #48]
	ldr	w8, [x26]
	cmp	w8, #1
	b.eq	LBB11_26
; %bb.15:
	cbz	w24, LBB11_18
; %bb.16:
	sub	x11, x19, #1
	and	x8, x19, #0x3
	cmp	x11, #3
	b.hs	LBB11_19
; %bb.17:
	mov	x12, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	ldr	x4, [sp, #8]                    ; 8-byte Reload
	b	LBB11_23
LBB11_18:
	mov	w9, #0                          ; =0x0
	mov	w11, #0                         ; =0x0
	b	LBB11_25
LBB11_19:
	mov	x10, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	and	x16, x19, #0xfffffffffffffffc
	neg	x16, x16
	sub	x17, x19, #3
	sub	x0, x19, #2
	ldr	x4, [sp, #8]                    ; 8-byte Reload
	add	x1, x4, x23
	add	x1, x1, #1
LBB11_20:                               ; =>This Inner Loop Header: Depth=1
	ldurb	w2, [x1, #-1]
	add	w15, w15, w2
	add	w3, w19, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x1]
	add	w12, w12, w2
	add	w3, w11, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x1, #1]
	add	w13, w13, w2
	add	w3, w0, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x1, #2]
	add	w14, w14, w2
	add	w3, w17, w10
	madd	w9, w2, w3, w9
	sub	x10, x10, #4
	add	x1, x1, #4
	cmp	x16, x10
	b.ne	LBB11_20
; %bb.21:
	add	w11, w12, w15
	add	w12, w14, w13
	add	w11, w12, w11
	cbz	x8, LBB11_25
; %bb.22:
	neg	x12, x10
LBB11_23:
	sub	w10, w19, w12
	add	x13, x23, x4
	add	x12, x13, x12
LBB11_24:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x12], #1
	add	w11, w11, w13
	madd	w9, w13, w10, w9
	sub	w10, w10, #1
	subs	x8, x8, #1
	b.ne	LBB11_24
LBB11_25:
	ldp	w10, w8, [x21, #8]
	add	w8, w8, w9
	madd	w8, w10, w19, w8
	mov	w9, #32881                      ; =0x8071
	movk	w9, #32775, lsl #16
	umull	x12, w8, w9
	lsr	x12, x12, #47
	mov	w13, #65521                     ; =0xfff1
	msub	w8, w12, w13, w8
	add	w10, w10, w11
	umull	x9, w10, w9
	lsr	x9, x9, #47
	msub	w9, w9, w13, w10
	stp	w9, w8, [x21, #8]
	mov	w8, #1                          ; =0x1
LBB11_26:
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #112
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE ; -- Begin function _ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
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
	mov	x9, #19                         ; =0x13
	movk	x9, #4, lsl #32
	str	x9, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	mov	x0, x8
	ret
LBB12_6:
	sub	sp, sp, #112
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
	mov	w13, #38148                     ; =0x9504
	add	x22, x0, x13
	add	x13, x9, #1
	str	x13, [x0, #56]
	ldr	w24, [x22]
	cmp	x11, x24
	csel	x11, x11, x24, lo
	cmp	x10, x11
	csel	x19, x10, x11, lo
	sub	x10, x19, #1
	sub	x11, x12, x13
	cmp	x10, x11
	b.ls	LBB12_8
; %bb.7:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
	str	x9, [x0, #32]
	b	LBB12_24
LBB12_8:
	mov	w10, #32832                     ; =0x8040
	add	x25, x0, x10
	add	x9, x19, x9
	str	x9, [x0, #56]
	ldr	w9, [x25]
	str	x0, [sp, #8]                    ; 8-byte Spill
	add	x11, x0, #64
	ldr	w27, [x22, #4]
	sub	w9, w9, w27
	and	x9, x9, #0x7fff
	mov	x23, x1
	ldr	x10, [x1, #16]
	add	x20, x10, x8
	cmp	w27, #1
	b.ne	LBB12_10
; %bb.9:
	mov	x26, x11
	ldrb	w1, [x11, x9]
	mov	x0, x20
	mov	x2, x19
	bl	_memset
	b	LBB12_12
LBB12_10:
	cmp	x19, x27
	csel	x28, x19, x27, lo
	mov	w8, #32768                      ; =0x8000
	sub	x8, x8, x9
	cmp	x8, x28
	csel	x21, x8, x28, lo
	mov	x26, x11
	add	x1, x11, x9
	mov	x0, x20
	mov	x2, x21
	bl	_memcpy
	sub	x2, x28, x21
	add	x0, x20, x21
	ldr	x8, [sp, #8]                    ; 8-byte Reload
	add	x1, x8, #64
	bl	_memcpy
	cmp	x19, x27
	b.ls	LBB12_12
LBB12_11:                               ; =>This Inner Loop Header: Depth=1
	sub	x8, x19, x28
	cmp	x8, x28
	csel	x21, x8, x28, lo
	add	x0, x20, x28
	mov	x1, x20
	mov	x2, x21
	bl	_memcpy
	add	x28, x21, x28
	cmp	x28, x19
	b.lo	LBB12_11
LBB12_12:
	ldr	w8, [x22]
	sub	w8, w8, w19
	str	w8, [x22]
	mov	x27, x23
	ldr	x10, [x23, #16]
	ldr	x23, [x23, #40]
	add	x28, x10, x23
	ldr	x8, [x25]
	mov	w21, #32768                     ; =0x8000
	sub	x9, x21, x8
	cmp	x9, x19
	csel	x20, x9, x19, lo
	add	x0, x26, x8
	str	x10, [sp]                       ; 8-byte Spill
	add	x1, x10, x23
	mov	x2, x20
	bl	_memcpy
	sub	x2, x19, x20
	ldr	x26, [sp, #8]                   ; 8-byte Reload
	add	x0, x26, #64
	add	x1, x28, x20
	bl	_memcpy
	ldr	x8, [x25, #8]
	add	x8, x8, x19
	cmp	x8, #8, lsl #12                 ; =32768
	csel	x8, x8, x21, lo
	ldr	w9, [x25]
	add	w9, w9, w19
	and	x9, x9, #0x7fff
	stp	x9, x8, [x25]
	ldr	x8, [x27, #40]
	add	x8, x8, x19
	str	x8, [x27, #40]
	ldr	x8, [x26, #48]
	add	x8, x8, x19
	str	x8, [x26, #48]
	ldr	w8, [x26]
	cmp	w8, #1
	b.eq	LBB12_24
; %bb.13:
	cbz	w24, LBB12_16
; %bb.14:
	and	x8, x19, #0x3
	cmp	x19, #4
	b.hs	LBB12_17
; %bb.15:
	mov	x12, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	ldr	x4, [sp]                        ; 8-byte Reload
	b	LBB12_21
LBB12_16:
	mov	w9, #0                          ; =0x0
	mov	w11, #0                         ; =0x0
	b	LBB12_23
LBB12_17:
	mov	x10, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w13, #0                         ; =0x0
	mov	w14, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	and	x15, x19, #0xfffffffffffffffc
	neg	x15, x15
	sub	w16, w19, #3
	sub	w17, w19, #2
	ldr	x4, [sp]                        ; 8-byte Reload
	add	x0, x23, x4
	add	x0, x0, #1
	sub	w1, w19, #1
LBB12_18:                               ; =>This Inner Loop Header: Depth=1
	ldurb	w2, [x0, #-1]
	add	w14, w14, w2
	add	w3, w19, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0]
	add	w11, w11, w2
	add	w3, w1, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0, #1]
	add	w12, w12, w2
	add	w3, w17, w10
	madd	w9, w2, w3, w9
	ldrb	w2, [x0, #2]
	add	w13, w13, w2
	add	w3, w16, w10
	madd	w9, w2, w3, w9
	sub	x10, x10, #4
	add	x0, x0, #4
	cmp	x15, x10
	b.ne	LBB12_18
; %bb.19:
	add	w11, w11, w14
	add	w12, w13, w12
	add	w11, w12, w11
	cbz	x8, LBB12_23
; %bb.20:
	neg	x12, x10
LBB12_21:
	sub	w10, w19, w12
	add	x13, x4, x23
	add	x12, x13, x12
LBB12_22:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x12], #1
	add	w11, w11, w13
	madd	w9, w13, w10, w9
	sub	w10, w10, #1
	subs	x8, x8, #1
	b.ne	LBB12_22
LBB12_23:
	ldp	w10, w8, [x22, #24]
	add	w8, w8, w9
	madd	w8, w10, w19, w8
	mov	w9, #32881                      ; =0x8071
	movk	w9, #32775, lsl #16
	umull	x12, w8, w9
	lsr	x12, x12, #47
	mov	w13, #65521                     ; =0xfff1
	msub	w8, w12, w13, w8
	add	w10, w10, w11
	umull	x9, w10, w9
	lsr	x9, x9, #47
	msub	w9, w9, w13, w10
	stp	w9, w8, [x22, #24]
	mov	w8, #1                          ; =0x1
LBB12_24:
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #112
	mov	x0, x8
	ret
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
	str	xzr, [x8, #1256]
	ldr	w8, [x0]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	str	w8, [x0, #32]
LBB13_2:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE ; -- Begin function _ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
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
Lloh0:
	adrp	x10, LJTI14_0@PAGE
Lloh1:
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
	mov	x9, #19                         ; =0x13
	movk	x9, #2, lsl #32
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
	stur	xzr, [x20, #100]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #20, lsl #32
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
	mov	x8, #19                         ; =0x13
	movk	x8, #6, lsl #32
	str	x8, [x0, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #17, lsl #32
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
	ubfx	w14, w12, #5, #5
	add	w15, w14, #1
	stp	w13, w15, [x20, #108]
	ubfx	w12, w12, #10, #4
	add	w12, w12, #4
	str	w12, [x20, #116]
	cmp	w9, #29
	ccmp	w14, #30, #2, ls
	b.lo	LBB14_161
; %bb.108:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #13, lsl #32
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
	stur	xzr, [x20, #100]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #10, lsl #32
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
	mov	x9, #19                         ; =0x13
	movk	x9, #9, lsl #32
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
	mov	x9, #19                         ; =0x13
	movk	x9, #22, lsl #32
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
	stur	xzr, [x20, #100]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #3, lsl #32
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
Lloh2:
	adrp	x12, __ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE@PAGE
Lloh3:
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
Lloh4:
	adrp	x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGE
Lloh5:
	add	x9, x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGEOFF
	ldr	w9, [x9, x8, lsl #2]
Lloh6:
	adrp	x10, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGE
Lloh7:
	add	x10, x10, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGEOFF
	ldr	w8, [x10, x8, lsl #2]
	stp	w9, w8, [x20, #132]
	mov	w8, #15                         ; =0xf
	b	LBB14_181
LBB14_152:
	mov	x9, #19                         ; =0x13
	movk	x9, #16, lsl #32
	b	LBB14_195
LBB14_153:
	ldr	x9, [x21, #8]
	cmp	x9, x8
	b.hs	LBB14_173
; %bb.154:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #21, lsl #32
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
	mov	x9, #19                         ; =0x13
	movk	x9, #12, lsl #32
	b	LBB14_195
LBB14_164:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #7, lsl #32
	b	LBB14_195
LBB14_165:
	cmp	w8, #16
	b.ne	LBB14_180
; %bb.166:
	ldr	w9, [x20, #120]
	cbnz	w9, LBB14_180
; %bb.167:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #17, lsl #32
LBB14_168:
	str	x9, [x19, #32]
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
	mov	x9, #19                         ; =0x13
	movk	x9, #19, lsl #32
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
	mov	x9, #19                         ; =0x13
	movk	x9, #11, lsl #32
	b	LBB14_195
LBB14_178:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #23, lsl #32
	b	LBB14_195
LBB14_179:
	mov	w8, #0                          ; =0x0
	mov	x9, #19                         ; =0x13
	movk	x9, #8, lsl #32
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
Lloh8:
	adrp	x8, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGE
Lloh9:
	add	x8, x8, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGEOFF
	ldr	w8, [x8, w9, uxtw #2]
	str	w8, [x20, #128]
Lloh10:
	adrp	x8, __ZN9gui_forms11compression12_GLOBAL__N_112length_extraE@PAGE
Lloh11:
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
	mov	x9, #19                         ; =0x13
	movk	x9, #4, lsl #32
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
	mov	x9, #19                         ; =0x13
	movk	x9, #5, lsl #32
LBB14_195:
	str	x9, [x0, #32]
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
	.loh AdrpAdd	Lloh0, Lloh1
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpAdd	Lloh6, Lloh7
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpAdd	Lloh8, Lloh9
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
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb ; -- Begin function _ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
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
	mov	x8, #19                         ; =0x13
	movk	x8, #5, lsl #32
	str	x8, [x20, #32]
LBB15_19:
	mov	w8, #3                          ; =0x3
	b	LBB15_21
LBB15_20:
	mov	w8, #2                          ; =0x2
LBB15_21:
	ldr	w9, [x20, #36]
	stp	w8, w9, [x19]
	ldp	x8, x9, [sp, #40]
	stp	x8, x9, [x19, #8]
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
