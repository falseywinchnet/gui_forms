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
	add	x9, x0, #8, lsl #12             ; =32768
	ldp	q0, q1, [x1]
	stp	q0, q1, [x0]
	ldr	w10, [x1]
	cmp	w10, #1
	mov	w10, #3                         ; =0x3
	csinc	w10, w10, wzr, eq
	str	w10, [x0, #32]
	str	wzr, [x0, #60]
	movi.2d	v0, #0000000000000000
	stur	q0, [x0, #44]
	stur	xzr, [x0, #36]
	str	q0, [x0, #32832]
	ldr	w10, [x1, #4]
	lsl	x10, x8, x10
	str	x10, [x9, #80]
	str	w8, [x9, #2264]
	mov	w10, #35036                     ; =0x88dc
	add	x10, x0, x10
	stp	q0, q0, [x10]
	str	q0, [x10, #32]
	stur	q0, [x10, #45]
	str	w8, [x9, #2332]
	str	q0, [x0, #35104]
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
	add	x8, x0, #8, lsl #12             ; =32768
	ldr	w10, [x8, #2284]
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
	ldr	w15, [x8, #2280]
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
	str	w15, [x8, #2280]
	add	w10, w10, #8
	str	w10, [x8, #2284]
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
	ldr	w10, [x8, #2280]
	bic	w9, w10, w9
	str	w9, [x3]
	ldr	w9, [x8, #2280]
	lsr	w9, w9, w2
	str	w9, [x8, #2280]
	ldr	w9, [x8, #2284]
	sub	w9, w9, w2
	str	w9, [x8, #2284]
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
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
lCPI4_0:
	.long	1                               ; 0x1
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0
lCPI4_1:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI4_2:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI4_3:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
lCPI4_4:
	.long	19                              ; 0x13
	.long	18                              ; 0x12
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.p2align	2
__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj: ; @_ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.cfi_startproc
; %bb.0:
	add	x8, x0, #8, lsl #12             ; =32768
	ldr	w10, [x2, #608]
	ldr	w11, [x8, #2264]
	cmp	w11, w10
	b.ls	LBB4_2
LBB4_1:
	mov	w8, #0                          ; =0x0
Lloh8:
	adrp	x9, lCPI4_4@PAGE
Lloh9:
	ldr	d0, [x9, lCPI4_4@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB4_2:
	add	x9, x8, #2264
	ldr	x12, [x0, #24]
	ldr	x13, [x1, #8]
	mov	w14, #8                         ; =0x8
	ldr	w15, [x8, #2284]
LBB4_3:                                 ; =>This Inner Loop Header: Depth=1
	cbz	w15, LBB4_5
; %bb.4:                                ;   in Loop: Header=BB4_3 Depth=1
	ldr	x17, [x0, #56]
	sub	w15, w15, #1
	cmp	x12, x17
	b.ne	LBB4_9
	b	LBB4_11
LBB4_5:                                 ;   in Loop: Header=BB4_3 Depth=1
	ldr	x15, [x1, #32]
	cmp	x15, x13
	b.eq	LBB4_13
; %bb.6:                                ;   in Loop: Header=BB4_3 Depth=1
	ldr	x4, [x0, #40]
	ldr	x16, [x0, #8]
	cmp	x4, x16
	b.eq	LBB4_16
; %bb.7:                                ;   in Loop: Header=BB4_3 Depth=1
	ldr	x16, [x0, #24]
	ldr	x17, [x0, #56]
	cmp	x16, x17
	b.eq	LBB4_11
; %bb.8:                                ;   in Loop: Header=BB4_3 Depth=1
	ldr	x5, [x1]
	ldr	w6, [x8, #2280]
	add	x17, x17, #1
	str	x17, [x0, #56]
	ldrb	w5, [x5, x15]
	orr	w5, w6, w5
	str	w5, [x8, #2280]
	str	w14, [x8, #2284]
	add	x15, x15, #1
	str	x15, [x1, #32]
	add	x15, x4, #1
	str	x15, [x0, #40]
	mov	w15, #7                         ; =0x7
	cmp	x16, x17
	b.eq	LBB4_11
LBB4_9:                                 ;   in Loop: Header=BB4_3 Depth=1
	add	x16, x17, #1
	str	x16, [x0, #56]
	ldr	w16, [x8, #2280]
	and	w17, w16, #0x1
	lsr	w16, w16, #1
	str	w16, [x8, #2280]
	str	w15, [x8, #2284]
	ldr	w16, [x8, #2268]
	orr	w16, w16, w17
	ldrh	w17, [x2, w11, uxtw #1]
	ldr	w5, [x8, #2272]
	add	w4, w5, w17
	subs	w5, w16, w5
	ccmp	w16, w4, #2, hs
	b.lo	LBB4_12
; %bb.10:                               ;   in Loop: Header=BB4_3 Depth=1
	ldr	w5, [x8, #2276]
	add	w17, w5, w17
	str	w17, [x8, #2276]
	lsl	w17, w4, #1
	str	w17, [x8, #2272]
	lsl	w16, w16, #1
	str	w16, [x8, #2268]
	add	w11, w11, #1
	str	w11, [x8, #2264]
	cmp	w11, w10
	b.ls	LBB4_3
	b	LBB4_1
LBB4_11:
	mov	w8, #0                          ; =0x0
Lloh10:
	adrp	x9, lCPI4_1@PAGE
Lloh11:
	ldr	d0, [x9, lCPI4_1@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
LBB4_12:
	ldr	w8, [x8, #2276]
	add	w8, w5, w8
	add	x8, x2, w8, uxtw #1
	ldrh	w8, [x8, #32]
	str	w8, [x3]
Lloh12:
	adrp	x8, lCPI4_0@PAGE
Lloh13:
	ldr	q0, [x8, lCPI4_0@PAGEOFF]
	str	q0, [x9]
	mov	w0, #1                          ; =0x1
	ret
LBB4_13:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB4_15
; %bb.14:
Lloh14:
	adrp	x8, lCPI4_3@PAGE
Lloh15:
	ldr	d0, [x8, lCPI4_3@PAGEOFF]
	str	d0, [x0, #32]
LBB4_15:
	str	wzr, [x1, #52]
	mov	w0, #0                          ; =0x0
	ret
LBB4_16:
	mov	w8, #0                          ; =0x0
Lloh16:
	adrp	x9, lCPI4_2@PAGE
Lloh17:
	ldr	d0, [x9, lCPI4_2@PAGEOFF]
	str	d0, [x0, #32]
	mov	x0, x8
	ret
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
	lsl	x8, x3, #1
	add	x8, x8, #64
	ldr	x10, [x0, #24]
	ldr	x9, [x0, #56]
	sub	x10, x10, x9
	cmp	x8, x10
	b.ls	LBB5_2
; %bb.1:
Lloh18:
	adrp	x9, lCPI5_2@PAGE
Lloh19:
	ldr	d0, [x9, lCPI5_2@PAGEOFF]
	str	d0, [x0, #32]
	mov	w0, #0                          ; =0x0
	ret
LBB5_2:
	sub	sp, sp, #96
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	.cfi_def_cfa_offset 96
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	add	x8, x9, x8
	str	x8, [x0, #56]
	str	wzr, [x1, #608]
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x1]
	cbz	x3, LBB5_24
; %bb.3:
	mov	x9, #0                          ; =0x0
	mov	w8, #0                          ; =0x0
	b	LBB5_5
LBB5_4:                                 ;   in Loop: Header=BB5_5 Depth=1
	add	x9, x9, #1
	cmp	x3, x9
	b.eq	LBB5_7
LBB5_5:                                 ; =>This Inner Loop Header: Depth=1
	ldrb	w10, [x2, x9]
	ldrh	w11, [x1, x10, lsl #1]
	add	w11, w11, #1
	strh	w11, [x1, x10, lsl #1]
	cmp	w8, w10
	b.hs	LBB5_4
; %bb.6:                                ;   in Loop: Header=BB5_5 Depth=1
	str	w10, [x1, #608]
	mov	x8, x10
	b	LBB5_4
LBB5_7:
	cbz	w8, LBB5_24
; %bb.8:
	ldrh	w9, [x1, #2]
	cmp	w9, #2
	b.hi	LBB5_23
; %bb.9:
	ldrh	w10, [x1, #4]
	add	w12, w10, w9, lsl #1
	cmp	w12, #4
	b.hi	LBB5_23
; %bb.10:
	ldrh	w11, [x1, #6]
	add	w13, w11, w12, lsl #1
	cmp	w13, #8
	b.hi	LBB5_23
; %bb.11:
	ldrh	w12, [x1, #8]
	add	w14, w12, w13, lsl #1
	cmp	w14, #16
	b.hi	LBB5_23
; %bb.12:
	ldrh	w13, [x1, #10]
	add	w15, w13, w14, lsl #1
	cmp	w15, #32
	b.hi	LBB5_23
; %bb.13:
	ldrh	w14, [x1, #12]
	add	w15, w14, w15, lsl #1
	cmp	w15, #64
	b.hi	LBB5_23
; %bb.14:
	ldrh	w16, [x1, #14]
	add	w15, w16, w15, lsl #1
	cmp	w15, #128
	b.hi	LBB5_23
; %bb.15:
	ldrh	w17, [x1, #16]
	add	w15, w17, w15, lsl #1
	cmp	w15, #256
	b.hi	LBB5_23
; %bb.16:
	ldrh	w5, [x1, #18]
	add	w15, w5, w15, lsl #1
	cmp	w15, #512
	b.hi	LBB5_23
; %bb.17:
	ldrh	w6, [x1, #20]
	add	w15, w6, w15, lsl #1
	cmp	w15, #1024
	b.hi	LBB5_23
; %bb.18:
	ldrh	w7, [x1, #22]
	add	w15, w7, w15, lsl #1
	cmp	w15, #2048
	b.hi	LBB5_23
; %bb.19:
	ldrh	w19, [x1, #24]
	add	w15, w19, w15, lsl #1
	cmp	w15, #1, lsl #12                ; =4096
	b.hi	LBB5_23
; %bb.20:
	ldrh	w20, [x1, #26]
	add	w15, w20, w15, lsl #1
	cmp	w15, #2, lsl #12                ; =8192
	b.hi	LBB5_23
; %bb.21:
	ldrh	w21, [x1, #28]
	add	w15, w21, w15, lsl #1
	cmp	w15, #4, lsl #12                ; =16384
	b.hi	LBB5_23
; %bb.22:
	ldrh	w22, [x1, #30]
	add	w15, w22, w15, lsl #1
	cmp	w15, #8, lsl #12                ; =32768
	b.ls	LBB5_26
LBB5_23:
	mov	w8, #0                          ; =0x0
Lloh20:
	adrp	x9, lCPI5_1@PAGE
Lloh21:
	ldr	d0, [x9, lCPI5_1@PAGEOFF]
	str	d0, [x0, #32]
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #96
	mov	x0, x8
	ret
LBB5_24:
	cbz	w4, LBB5_53
LBB5_25:
	mov	w8, #0                          ; =0x0
Lloh22:
	adrp	x9, lCPI5_0@PAGE
Lloh23:
	ldr	d0, [x9, lCPI5_0@PAGEOFF]
	str	d0, [x0, #32]
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #96
	mov	x0, x8
	ret
LBB5_26:
	cmp	w8, #1
	csinc	w8, w4, wzr, eq
	cmp	w15, #8, lsl #12                ; =32768
	b.eq	LBB5_28
; %bb.27:
	tbnz	w8, #0, LBB5_25
LBB5_28:
	mov	x15, #0                         ; =0x0
	str	xzr, [sp]
	add	w8, w9, w10
	add	w10, w8, w11
	stp	w9, w8, [sp, #8]
	add	w8, w10, w12
	add	w9, w8, w13
	stp	w10, w8, [sp, #16]
	add	w8, w9, w14
	add	w10, w8, w16
	stp	w9, w8, [sp, #24]
	add	w8, w10, w17
	add	w9, w8, w5
	stp	w10, w8, [sp, #32]
	add	w8, w9, w6
	add	w10, w8, w7
	stp	w9, w8, [sp, #40]
	add	w8, w10, w19
	add	w9, w8, w20
	stp	w10, w8, [sp, #48]
	add	w8, w9, w21
	stp	w9, w8, [sp, #56]
	add	x8, x1, #32
	and	x9, x3, #0x7
	cmp	x3, #8
	b.lo	LBB5_49
; %bb.29:
	and	x10, x3, #0xfffffffffffffff8
	neg	x11, x10
	mov	w10, #3                         ; =0x3
	mov	x12, sp
	b	LBB5_31
LBB5_30:                                ;   in Loop: Header=BB5_31 Depth=1
	add	x10, x10, #8
	add	x13, x11, x10
	cmp	x13, #3
	b.eq	LBB5_47
LBB5_31:                                ; =>This Inner Loop Header: Depth=1
	add	x13, x2, x10
	ldurb	w14, [x13, #-3]
	cbz	x14, LBB5_33
; %bb.32:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w15, [x12, x14, lsl #2]
	sub	x16, x10, #3
	strh	w16, [x8, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x12, x14, lsl #2]
LBB5_33:                                ;   in Loop: Header=BB5_31 Depth=1
	ldurb	w13, [x13, #-2]
	cbz	x13, LBB5_35
; %bb.34:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w14, [x12, x13, lsl #2]
	sub	x15, x10, #2
	strh	w15, [x8, x14, lsl #1]
	add	w14, w14, #1
	str	w14, [x12, x13, lsl #2]
LBB5_35:                                ;   in Loop: Header=BB5_31 Depth=1
	add	x13, x2, x10
	ldurb	w13, [x13, #-1]
	cbz	x13, LBB5_37
; %bb.36:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w14, [x12, x13, lsl #2]
	sub	x15, x10, #1
	strh	w15, [x8, x14, lsl #1]
	add	w14, w14, #1
	str	w14, [x12, x13, lsl #2]
LBB5_37:                                ;   in Loop: Header=BB5_31 Depth=1
	ldrb	w13, [x2, x10]
	cbz	x13, LBB5_39
; %bb.38:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w14, [x12, x13, lsl #2]
	strh	w10, [x8, x14, lsl #1]
	add	w14, w14, #1
	str	w14, [x12, x13, lsl #2]
LBB5_39:                                ;   in Loop: Header=BB5_31 Depth=1
	add	x13, x2, x10
	ldrb	w14, [x13, #1]
	cbz	x14, LBB5_41
; %bb.40:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w15, [x12, x14, lsl #2]
	add	x16, x10, #1
	strh	w16, [x8, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x12, x14, lsl #2]
LBB5_41:                                ;   in Loop: Header=BB5_31 Depth=1
	ldrb	w13, [x13, #2]
	cbz	x13, LBB5_43
; %bb.42:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w14, [x12, x13, lsl #2]
	add	x15, x10, #2
	strh	w15, [x8, x14, lsl #1]
	add	w14, w14, #1
	str	w14, [x12, x13, lsl #2]
LBB5_43:                                ;   in Loop: Header=BB5_31 Depth=1
	add	x13, x2, x10
	ldrb	w14, [x13, #3]
	cbz	x14, LBB5_45
; %bb.44:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w15, [x12, x14, lsl #2]
	add	x16, x10, #3
	strh	w16, [x8, x15, lsl #1]
	add	w15, w15, #1
	str	w15, [x12, x14, lsl #2]
LBB5_45:                                ;   in Loop: Header=BB5_31 Depth=1
	ldrb	w13, [x13, #4]
	cbz	x13, LBB5_30
; %bb.46:                               ;   in Loop: Header=BB5_31 Depth=1
	ldr	w14, [x12, x13, lsl #2]
	add	x15, x10, #4
	strh	w15, [x8, x14, lsl #1]
	add	w14, w14, #1
	str	w14, [x12, x13, lsl #2]
	b	LBB5_30
LBB5_47:
	cbz	x9, LBB5_53
; %bb.48:
	sub	x15, x10, #3
LBB5_49:
	mov	x10, sp
	b	LBB5_51
LBB5_50:                                ;   in Loop: Header=BB5_51 Depth=1
	add	x15, x15, #1
	subs	x9, x9, #1
	b.eq	LBB5_53
LBB5_51:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w11, [x2, x15]
	cbz	x11, LBB5_50
; %bb.52:                               ;   in Loop: Header=BB5_51 Depth=1
	ldr	w12, [x10, x11, lsl #2]
	strh	w15, [x8, x12, lsl #1]
	add	w12, w12, #1
	str	w12, [x10, x11, lsl #2]
	b	LBB5_50
LBB5_53:
	mov	w8, #1                          ; =0x1
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #96
	mov	x0, x8
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
	mov	w8, #34692                      ; =0x8784
	add	x19, x0, x8
	movi.16b	v0, #8
	stp	q0, q0, [x19]
	stp	q0, q0, [x19, #32]
	stp	q0, q0, [x19, #64]
	stp	q0, q0, [x19, #96]
	mov	w8, #34972                      ; =0x889c
	mov	x9, #578721382704613384         ; =0x808080808080808
	str	x9, [x0, x8]
	str	q0, [x19, #128]
	mov	w8, #34948                      ; =0x8884
	add	x8, x0, x8
	mov	x9, #506381209866536711         ; =0x707070707070707
	stp	x9, x9, [x8]
	str	x9, [x8, #16]
	mov	w8, #34836                      ; =0x8814
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
	mov	w8, #33468                      ; =0x82bc
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
	add	x8, x0, #8, lsl #12             ; =32768
	ldr	x9, [x1, #40]
	ldr	x10, [x1, #16]
	strb	w2, [x10, x9]
	ldr	x9, [x1, #40]
	add	x9, x9, #1
	str	x9, [x1, #40]
	ldr	x9, [x0, #48]
	add	x9, x9, #1
	str	x9, [x0, #48]
	ldr	x9, [x8, #64]
	add	x9, x0, x9
	strb	w2, [x9, #64]
	ldr	w9, [x8, #64]
	add	w9, w9, #1
	and	x9, x9, #0x7fff
	str	x9, [x8, #64]
	ldr	x9, [x8, #72]
	lsr	x10, x9, #15
	cbnz	x10, LBB8_2
; %bb.1:
	add	x9, x9, #1
	str	x9, [x8, #72]
LBB8_2:
	ldr	w9, [x8, #2332]
	mov	w10, #-65521                    ; =0xffff000f
	add	w9, w9, w2
	add	w11, w9, w10
	mov	w12, #65520                     ; =0xfff0
	cmp	w9, w12
	csel	w9, w11, w9, hi
	str	w9, [x8, #2332]
	ldr	w11, [x8, #2336]
	add	w9, w9, w11
	add	w10, w9, w10
	cmp	w9, w12
	csel	w9, w10, w9, hi
	str	w9, [x8, #2336]
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN9gui_forms11compression13InflateStream9end_blockEv ; -- Begin function _ZN9gui_forms11compression13InflateStream9end_blockEv
	.p2align	2
__ZN9gui_forms11compression13InflateStream9end_blockEv: ; @_ZN9gui_forms11compression13InflateStream9end_blockEv
	.cfi_startproc
; %bb.0:
	add	x8, x0, #8, lsl #12             ; =32768
	mov	w9, #3                          ; =0x3
	str	w9, [x0, #32]
	ldrb	w9, [x8, #2328]
	cmp	w9, #1
	b.ne	LBB9_2
; %bb.1:
	movi.2d	v0, #0000000000000000
	str	d0, [x8, #2280]
	ldr	w8, [x0]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	str	w8, [x0, #32]
LBB9_2:
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
lCPI10_0:
	.long	19                              ; 0x13
	.long	2                               ; 0x2
lCPI10_1:
	.long	19                              ; 0x13
	.long	23                              ; 0x17
lCPI10_2:
	.long	19                              ; 0x13
	.long	5                               ; 0x5
lCPI10_3:
	.long	19                              ; 0x13
	.long	3                               ; 0x3
lCPI10_4:
	.long	19                              ; 0x13
	.long	6                               ; 0x6
lCPI10_5:
	.long	19                              ; 0x13
	.long	4                               ; 0x4
lCPI10_6:
	.long	19                              ; 0x13
	.long	21                              ; 0x15
lCPI10_7:
	.long	19                              ; 0x13
	.long	22                              ; 0x16
lCPI10_8:
	.long	19                              ; 0x13
	.long	20                              ; 0x14
lCPI10_9:
	.long	19                              ; 0x13
	.long	17                              ; 0x11
lCPI10_10:
	.long	19                              ; 0x13
	.long	16                              ; 0x10
lCPI10_11:
	.long	4294967291                      ; 0xfffffffb
	.long	4294967286                      ; 0xfffffff6
lCPI10_12:
	.long	31                              ; 0x1f
	.long	15                              ; 0xf
lCPI10_13:
	.long	1                               ; 0x1
	.long	4                               ; 0x4
lCPI10_14:
	.long	19                              ; 0x13
	.long	13                              ; 0xd
lCPI10_15:
	.long	19                              ; 0x13
	.long	12                              ; 0xc
lCPI10_16:
	.long	19                              ; 0x13
	.long	11                              ; 0xb
lCPI10_17:
	.long	19                              ; 0x13
	.long	10                              ; 0xa
lCPI10_18:
	.long	19                              ; 0x13
	.long	7                               ; 0x7
lCPI10_19:
	.long	19                              ; 0x13
	.long	9                               ; 0x9
lCPI10_20:
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
	b.hi	LBB10_3
; %bb.1:
	mov	w8, #0                          ; =0x0
	add	x21, x0, #8, lsl #12            ; =32768
Lloh30:
	adrp	x10, LJTI10_0@PAGE
Lloh31:
	add	x10, x10, LJTI10_0@PAGEOFF
	adr	x11, LBB10_2
	ldrh	w12, [x10, x9, lsl #1]
	add	x11, x11, x12, lsl #2
	br	x11
LBB10_2:
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_3:
	mov	w8, #0                          ; =0x0
Lloh32:
	adrp	x9, lCPI10_0@PAGE
Lloh33:
	ldr	d0, [x9, lCPI10_0@PAGEOFF]
	b	LBB10_209
LBB10_4:
	ldr	w3, [x21, #2288]
	ldr	w8, [x21, #2292]
	ldr	w9, [x21, #2300]
	add	w8, w8, w3
	cmp	w9, w8
	b.ne	LBB10_101
; %bb.5:
	ldrb	w8, [x21, #2180]
	cbz	w8, LBB10_171
; %bb.6:
	mov	w8, #34692                      ; =0x8784
	add	x19, x0, x8
	mov	w8, #32856                      ; =0x8058
	add	x1, x0, x8
	mov	x20, x0
	mov	x2, x19
	mov	w4, #0                          ; =0x0
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB10_195
; %bb.7:
	ldr	w8, [x21, #2288]
	ldr	w3, [x21, #2292]
	mov	w9, #33468                      ; =0x82bc
	mov	x0, x20
	add	x1, x20, x9
	add	x2, x19, x8
	mov	w4, #0                          ; =0x0
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB10_195
; %bb.8:
	mov	w8, #11                         ; =0xb
	str	w8, [x20, #32]
	b	LBB10_201
LBB10_9:
	ldr	w9, [x21, #2304]
	cmp	w9, #16
	b.ne	LBB10_104
; %bb.10:
	ldr	w8, [x21, #2300]
	sub	w8, w8, #1
	add	x8, x0, x8
	mov	w10, #34692                     ; =0x8784
	ldrb	w8, [x8, x10]
	b	LBB10_105
LBB10_11:
	ldr	w9, [x21, #2284]
	cmp	w9, #14
	b.hs	LBB10_113
; %bb.12:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB10_107
; %bb.13:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB10_158
; %bb.14:
	ldr	x8, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x8, x14
	b.eq	LBB10_208
; %bb.15:
	ldr	x16, [x1]
	ldr	w17, [x21, #2280]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w9
	orr	w2, w2, w17
	str	w2, [x21, #2280]
	add	w17, w9, #8
	str	w17, [x21, #2284]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w9, #5
	b.hi	LBB10_204
; %bb.16:
	cmp	x3, x13
	b.eq	LBB10_107
; %bb.17:
	cmp	x4, x15
	b.eq	LBB10_158
; %bb.18:
	cmp	x8, x10
	b.eq	LBB10_208
; %bb.19:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	str	w13, [x21, #2280]
	orr	w9, w9, #0x10
	str	w9, [x21, #2284]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x8, x10
	b.ne	LBB10_114
	b	LBB10_208
LBB10_20:
	ldr	w8, [x21, #2284]
	cmp	w8, #16
	b.hs	LBB10_118
; %bb.21:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB10_107
; %bb.22:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB10_158
; %bb.23:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB10_208
; %bb.24:
	ldr	x16, [x1]
	ldr	w17, [x21, #2280]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	str	w2, [x21, #2280]
	add	w17, w8, #8
	str	w17, [x21, #2284]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB10_205
; %bb.25:
	cmp	x3, x13
	b.eq	LBB10_107
; %bb.26:
	cmp	x4, x15
	b.eq	LBB10_158
; %bb.27:
	cmp	x9, x10
	b.eq	LBB10_208
; %bb.28:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	str	w13, [x21, #2280]
	orr	w8, w8, #0x10
	str	w8, [x21, #2284]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB10_119
	b	LBB10_208
LBB10_29:
	ldr	w8, [x21, #2316]
	ldr	w9, [x21, #2284]
	cmp	w9, w8
	b.hs	LBB10_121
; %bb.30:
	mov	x11, #0                         ; =0x0
	ldr	x17, [x0, #8]
	ldr	x10, [x0, #24]
	ldp	x2, x3, [x1]
	ldr	x12, [x1, #32]
	ldr	x14, [x0, #40]
	ldr	x13, [x0, #56]
	ldr	w15, [x21, #2280]
	sub	x16, x13, x10
	sub	x17, x14, x17
	add	x2, x2, x12
	sub	x3, x12, x3
LBB10_31:                               ; =>This Inner Loop Header: Depth=1
	cmn	x3, x11
	b.eq	LBB10_107
; %bb.32:                               ;   in Loop: Header=BB10_31 Depth=1
	add	x4, x17, x11
	cbz	x4, LBB10_158
; %bb.33:                               ;   in Loop: Header=BB10_31 Depth=1
	add	x4, x16, x11
	cbz	x4, LBB10_208
; %bb.34:                               ;   in Loop: Header=BB10_31 Depth=1
	add	x4, x13, x11
	add	x4, x4, #1
	str	x4, [x0, #56]
	ldrb	w4, [x2, x11]
	lsl	w4, w4, w9
	orr	w15, w4, w15
	str	w15, [x21, #2280]
	add	w9, w9, #8
	str	w9, [x21, #2284]
	add	x4, x12, x11
	add	x4, x4, #1
	str	x4, [x1, #32]
	add	x4, x14, x11
	add	x4, x4, #1
	str	x4, [x0, #40]
	add	x11, x11, #1
	cmp	w9, w8
	b.lo	LBB10_31
; %bb.35:
	add	x11, x13, x11
	b	LBB10_122
LBB10_36:
	ldr	w8, [x21, #2284]
	cmp	w8, #3
	b.hs	LBB10_124
; %bb.37:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB10_107
; %bb.38:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB10_158
; %bb.39:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.40:
	ldr	x13, [x1]
	ldr	w14, [x21, #2280]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	str	w13, [x21, #2280]
	orr	w8, w8, #0x8
	str	w8, [x21, #2284]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB10_125
LBB10_41:
	mov	w8, #32856                      ; =0x8058
	add	x2, x0, x8
	add	x3, sp, #12
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB10_195
; %bb.42:
	ldr	w8, [sp, #12]
	cmp	w8, #255
	b.hi	LBB10_159
; %bb.43:
	str	w8, [x21, #2320]
	mov	w8, #12                         ; =0xc
	b	LBB10_200
LBB10_44:
	ldr	w8, [x21, #2284]
	cmp	w8, #16
	b.hs	LBB10_129
; %bb.45:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB10_107
; %bb.46:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB10_158
; %bb.47:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB10_208
; %bb.48:
	ldr	x16, [x1]
	ldr	w17, [x21, #2280]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	str	w2, [x21, #2280]
	add	w17, w8, #8
	str	w17, [x21, #2284]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB10_206
; %bb.49:
	cmp	x3, x13
	b.eq	LBB10_107
; %bb.50:
	cmp	x4, x15
	b.eq	LBB10_158
; %bb.51:
	cmp	x9, x10
	b.eq	LBB10_208
; %bb.52:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	str	w13, [x21, #2280]
	orr	w8, w8, #0x10
	str	w8, [x21, #2284]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB10_130
	b	LBB10_208
LBB10_53:
	ldr	w8, [x21, #2300]
	ldr	w9, [x21, #2296]
	cmp	w8, w9
	b.hs	LBB10_131
; %bb.54:
	ldr	w9, [x21, #2284]
	cmp	w9, #3
	b.hs	LBB10_162
; %bb.55:
	ldr	x10, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x10
	b.eq	LBB10_107
; %bb.56:
	ldr	x13, [x0, #40]
	ldr	x10, [x0, #8]
	cmp	x13, x10
	b.eq	LBB10_158
; %bb.57:
	ldr	x10, [x0, #24]
	ldr	x12, [x0, #56]
	cmp	x10, x12
	b.eq	LBB10_208
; %bb.58:
	ldr	x14, [x1]
	ldr	w15, [x21, #2280]
	add	x12, x12, #1
	str	x12, [x0, #56]
	ldrb	w14, [x14, x11]
	lsl	w14, w14, w9
	orr	w14, w14, w15
	str	w14, [x21, #2280]
	orr	w9, w9, #0x8
	str	w9, [x21, #2284]
	add	x11, x11, #1
	str	x11, [x1, #32]
	add	x11, x13, #1
	str	x11, [x0, #40]
	b	LBB10_163
LBB10_59:
	ldr	w8, [x21, #2324]
	cbz	w8, LBB10_154
; %bb.60:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.eq	LBB10_92
; %bb.61:
	ldr	x8, [x1, #40]
	ldr	x9, [x1, #24]
	cmp	x8, x9
	b.eq	LBB10_148
; %bb.62:
	ldr	x9, [x0, #24]
	ldr	x8, [x0, #56]
	cmp	x9, x8
	b.eq	LBB10_208
; %bb.63:
	add	x8, x8, #1
	str	x8, [x0, #56]
	add	x3, sp, #12
	mov	x19, x0
	mov	x20, x1
	mov	w2, #8                          ; =0x8
	bl	__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
	cbz	w0, LBB10_195
; %bb.64:
	ldrb	w2, [sp, #12]
	mov	x0, x19
	mov	x1, x20
	bl	__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	ldr	w8, [x21, #2324]
	sub	w8, w8, #1
	str	w8, [x21, #2324]
	b	LBB10_201
LBB10_65:
	ldr	w8, [x21, #2284]
	cmp	w8, #8
	b.hs	LBB10_133
; %bb.66:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB10_107
; %bb.67:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB10_158
; %bb.68:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.69:
	ldr	x13, [x1]
	ldr	w14, [x21, #2280]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	str	w13, [x21, #2280]
	orr	w8, w8, #0x8
	str	w8, [x21, #2284]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB10_134
LBB10_70:
	ldr	w8, [x21, #2284]
	cmp	w8, #16
	b.hs	LBB10_137
; %bb.71:
	ldr	x13, [x1, #8]
	ldr	x11, [x1, #32]
	cmp	x11, x13
	b.eq	LBB10_107
; %bb.72:
	ldr	x15, [x0, #8]
	ldr	x12, [x0, #40]
	cmp	x12, x15
	b.eq	LBB10_158
; %bb.73:
	ldr	x9, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x9, x14
	b.eq	LBB10_208
; %bb.74:
	ldr	x16, [x1]
	ldr	w17, [x21, #2280]
	add	x10, x14, #1
	str	x10, [x0, #56]
	ldrb	w2, [x16, x11]
	lsl	w2, w2, w8
	orr	w2, w2, w17
	str	w2, [x21, #2280]
	add	w17, w8, #8
	str	w17, [x21, #2284]
	add	x3, x11, #1
	str	x3, [x1, #32]
	add	x4, x12, #1
	str	x4, [x0, #40]
	cmp	w8, #7
	b.hi	LBB10_207
; %bb.75:
	cmp	x3, x13
	b.eq	LBB10_107
; %bb.76:
	cmp	x4, x15
	b.eq	LBB10_158
; %bb.77:
	cmp	x9, x10
	b.eq	LBB10_208
; %bb.78:
	add	x10, x14, #2
	str	x10, [x0, #56]
	ldrb	w13, [x16, x3]
	lsl	w13, w13, w17
	orr	w13, w13, w2
	str	w13, [x21, #2280]
	orr	w8, w8, #0x10
	str	w8, [x21, #2284]
	add	x11, x11, #2
	str	x11, [x1, #32]
	add	x11, x12, #2
	str	x11, [x0, #40]
	cmp	x9, x10
	b.ne	LBB10_138
	b	LBB10_208
LBB10_79:
	ldr	w8, [x21, #2316]
	ldr	w9, [x21, #2284]
	cmp	w9, w8
	b.hs	LBB10_143
; %bb.80:
	mov	x11, #0                         ; =0x0
	ldr	x17, [x0, #8]
	ldr	x10, [x0, #24]
	ldp	x2, x3, [x1]
	ldr	x12, [x1, #32]
	ldr	x14, [x0, #40]
	ldr	x13, [x0, #56]
	ldr	w15, [x21, #2280]
	sub	x16, x13, x10
	sub	x17, x14, x17
	add	x2, x2, x12
	sub	x3, x12, x3
LBB10_81:                               ; =>This Inner Loop Header: Depth=1
	cmn	x3, x11
	b.eq	LBB10_107
; %bb.82:                               ;   in Loop: Header=BB10_81 Depth=1
	add	x4, x17, x11
	cbz	x4, LBB10_158
; %bb.83:                               ;   in Loop: Header=BB10_81 Depth=1
	add	x4, x16, x11
	cbz	x4, LBB10_208
; %bb.84:                               ;   in Loop: Header=BB10_81 Depth=1
	add	x4, x13, x11
	add	x4, x4, #1
	str	x4, [x0, #56]
	ldrb	w4, [x2, x11]
	lsl	w4, w4, w9
	orr	w15, w4, w15
	str	w15, [x21, #2280]
	add	w9, w9, #8
	str	w9, [x21, #2284]
	add	x4, x12, x11
	add	x4, x4, #1
	str	x4, [x1, #32]
	add	x4, x14, x11
	add	x4, x4, #1
	str	x4, [x0, #40]
	add	x11, x11, #1
	cmp	w9, w8
	b.lo	LBB10_81
; %bb.85:
	add	x11, x13, x11
	b	LBB10_144
LBB10_86:
	ldr	w8, [x21, #2308]
	cbz	w8, LBB10_156
; %bb.87:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.eq	LBB10_92
; %bb.88:
	ldr	x8, [x1, #40]
	ldr	x9, [x1, #24]
	cmp	x8, x9
	b.eq	LBB10_148
; %bb.89:
	ldr	x9, [x0, #24]
	ldr	x8, [x0, #56]
	cmp	x9, x8
	b.eq	LBB10_208
; %bb.90:
	add	x8, x8, #1
	str	x8, [x0, #56]
	ldr	w8, [x21, #64]
	ldr	w9, [x21, #2312]
	sub	w8, w8, w9
	and	x8, x8, #0x7fff
	add	x8, x0, x8
	ldrb	w2, [x8, #64]
	bl	__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	ldr	w8, [x21, #2308]
	sub	w8, w8, #1
	str	w8, [x21, #2308]
	b	LBB10_201
LBB10_91:
	ldr	x8, [x0, #48]
	ldr	x9, [x0, #16]
	cmp	x8, x9
	b.ne	LBB10_147
LBB10_92:
	mov	w8, #0                          ; =0x0
Lloh34:
	adrp	x9, lCPI10_5@PAGE
Lloh35:
	ldr	d0, [x9, lCPI10_5@PAGEOFF]
	b	LBB10_209
LBB10_93:
	ldr	w8, [x21, #2284]
	cmp	w8, #8
	b.hs	LBB10_149
; %bb.94:
	ldr	x9, [x1, #8]
	ldr	x10, [x1, #32]
	cmp	x10, x9
	b.eq	LBB10_107
; %bb.95:
	ldr	x12, [x0, #40]
	ldr	x9, [x0, #8]
	cmp	x12, x9
	b.eq	LBB10_158
; %bb.96:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.97:
	ldr	x13, [x1]
	ldr	w14, [x21, #2280]
	add	x11, x11, #1
	str	x11, [x0, #56]
	ldrb	w13, [x13, x10]
	lsl	w13, w13, w8
	orr	w13, w13, w14
	str	w13, [x21, #2280]
	orr	w8, w8, #0x8
	str	w8, [x21, #2284]
	add	x10, x10, #1
	str	x10, [x1, #32]
	add	x10, x12, #1
	str	x10, [x0, #40]
	b	LBB10_150
LBB10_98:
	mov	w8, #33468                      ; =0x82bc
	add	x2, x0, x8
	add	x3, sp, #12
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB10_195
; %bb.99:
	ldr	w8, [sp, #12]
	cmp	w8, #30
	b.lo	LBB10_170
; %bb.100:
	mov	w8, #0                          ; =0x0
Lloh36:
	adrp	x9, lCPI10_8@PAGE
Lloh37:
	ldr	d0, [x9, lCPI10_8@PAGEOFF]
	b	LBB10_186
LBB10_101:
	mov	w8, #34080                      ; =0x8520
	add	x2, x0, x8
	add	x3, sp, #12
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	cbz	w0, LBB10_195
; %bb.102:
	ldr	w8, [sp, #12]
	cmp	w8, #15
	b.hi	LBB10_183
; %bb.103:
	ldr	w9, [x21, #2300]
	add	x9, x19, x9
	mov	w10, #34692                     ; =0x8784
	strb	w8, [x9, x10]
	b	LBB10_165
LBB10_104:
	mov	w8, #0                          ; =0x0
LBB10_105:
	cmp	w9, #17
	mov	w10, #2                         ; =0x2
	cinc	w10, w10, eq
	mov	w11, #3                         ; =0x3
	mov	w12, #11                        ; =0xb
	cmp	w9, #18
	csel	w9, w12, w11, eq
	mov	w11, #7                         ; =0x7
	csel	w10, w11, w10, eq
	ldr	w11, [x21, #2284]
	cmp	w11, w10
	b.hs	LBB10_110
; %bb.106:
	ldr	x12, [x1, #8]
	ldr	x13, [x1, #32]
	cmp	x13, x12
	b.ne	LBB10_157
LBB10_107:
	ldrb	w8, [x1, #48]
	cmp	w8, #1
	b.ne	LBB10_109
; %bb.108:
Lloh38:
	adrp	x8, lCPI10_4@PAGE
Lloh39:
	ldr	d0, [x8, lCPI10_4@PAGEOFF]
	str	d0, [x0, #32]
LBB10_109:
	mov	w8, #0                          ; =0x0
	str	wzr, [x1, #52]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_110:
	ldr	x12, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x12, x14
	b.eq	LBB10_208
LBB10_111:
	add	x13, x14, #1
	str	x13, [x0, #56]
	mov	w14, #-1                        ; =0xffffffff
	lsl	w14, w14, w10
	ldr	w15, [x21, #2280]
	bic	w14, w15, w14
	lsr	w15, w15, w10
	str	w15, [x21, #2280]
	sub	w10, w11, w10
	str	w10, [x21, #2284]
	ldr	w11, [x21, #2288]
	ldr	w15, [x21, #2292]
	ldr	w10, [x21, #2300]
	add	w11, w15, w11
	sub	w11, w11, w10
	add	w9, w14, w9
	cmp	w9, w11
	b.ls	LBB10_174
; %bb.112:
	mov	w8, #0                          ; =0x0
Lloh40:
	adrp	x9, lCPI10_9@PAGE
Lloh41:
	ldr	d0, [x9, lCPI10_9@PAGEOFF]
	b	LBB10_209
LBB10_113:
	ldr	x8, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x8, x10
	b.eq	LBB10_208
LBB10_114:
	add	x11, x10, #1
	str	x11, [x0, #56]
	ldr	w12, [x21, #2280]
	lsr	w13, w12, #14
	str	w13, [x21, #2280]
	sub	w9, w9, #14
	str	w9, [x21, #2284]
	and	w9, w12, #0x1f
	add	w13, w9, #257
	str	w13, [x21, #2288]
	dup.2s	v0, w12
	add	x12, x21, #2292
Lloh42:
	adrp	x13, lCPI10_11@PAGE
Lloh43:
	ldr	d1, [x13, lCPI10_11@PAGEOFF]
	ushl.2s	v0, v0, v1
Lloh44:
	adrp	x13, lCPI10_12@PAGE
Lloh45:
	ldr	d1, [x13, lCPI10_12@PAGEOFF]
	and.8b	v0, v0, v1
Lloh46:
	adrp	x13, lCPI10_13@PAGE
Lloh47:
	ldr	d1, [x13, lCPI10_13@PAGEOFF]
	add.2s	v1, v0, v1
	str	d1, [x12]
	cmp	w9, #29
	b.hi	LBB10_180
; %bb.115:
	fmov	w9, s0
	cmp	w9, #30
	b.hs	LBB10_180
; %bb.116:
	sub	x8, x8, x11
	cmp	x8, #18
	b.ls	LBB10_208
; %bb.117:
	add	x8, x10, #20
	str	x8, [x0, #56]
	mov	w8, #35012                      ; =0x88c4
	add	x8, x0, x8
	movi.2d	v0, #0000000000000000
	str	q0, [x8]
	stur	wzr, [x8, #15]
	str	wzr, [x21, #2300]
	mov	w8, #8                          ; =0x8
	b	LBB10_192
LBB10_118:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB10_208
LBB10_119:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	and	w10, w9, #0xffff
	lsr	w9, w9, #16
	str	w9, [x21, #2280]
	sub	w8, w8, #16
	str	w8, [x21, #2284]
	ldr	w8, [x21, #2324]
	eor	w8, w10, w8
	mov	w9, #65535                      ; =0xffff
	cmp	w8, w9
	b.ne	LBB10_181
; %bb.120:
	mov	w8, #6                          ; =0x6
	b	LBB10_192
LBB10_121:
	ldr	x10, [x0, #24]
	ldr	x11, [x0, #56]
LBB10_122:
	cmp	x10, x11
	b.eq	LBB10_208
; %bb.123:
	add	x10, x11, #1
	str	x10, [x0, #56]
	mov	w10, #-1                        ; =0xffffffff
	lsl	w10, w10, w8
	ldr	w11, [x21, #2280]
	bic	w10, w11, w10
	lsr	w11, w11, w8
	str	w11, [x21, #2280]
	sub	w8, w9, w8
	str	w8, [x21, #2284]
	ldr	w8, [x21, #2308]
	add	w8, w8, w10
	str	w8, [x21, #2308]
	mov	w8, #14                         ; =0xe
	b	LBB10_192
LBB10_124:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB10_125:
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.126:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	lsr	w10, w9, #3
	str	w10, [x21, #2280]
	sub	w8, w8, #3
	str	w8, [x21, #2284]
	and	w8, w9, #0x1
	strb	w8, [x21, #2328]
	ubfx	w8, w9, #1, #2
	cmp	w8, #1
	b.gt	LBB10_178
; %bb.127:
	cbnz	w8, LBB10_193
; %bb.128:
	movi.2d	v0, #0000000000000000
	str	d0, [x21, #2280]
	mov	w8, #4                          ; =0x4
	b	LBB10_192
LBB10_129:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB10_208
LBB10_130:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	and	w10, w9, #0xffff
	str	w10, [x21, #2324]
	lsr	w9, w9, #16
	str	w9, [x21, #2280]
	sub	w8, w8, #16
	str	w8, [x21, #2284]
	mov	w8, #5                          ; =0x5
	b	LBB10_192
LBB10_131:
	mov	w8, #34080                      ; =0x8520
	mov	w9, #35012                      ; =0x88c4
	add	x1, x0, x8
	add	x2, x0, x9
	mov	x19, x0
	mov	w3, #19                         ; =0x13
	mov	w4, #1                          ; =0x1
	bl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	cbz	w0, LBB10_195
; %bb.132:
	str	wzr, [x21, #2300]
	mov	w8, #9                          ; =0x9
	b	LBB10_200
LBB10_133:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB10_134:
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.135:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	lsr	w10, w9, #8
	str	w10, [x21, #2280]
	sub	w8, w8, #8
	str	w8, [x21, #2284]
	ldr	w8, [x21, #2348]
	bfi	w9, w8, #8, #24
	str	w9, [x21, #2348]
	ldr	w8, [x21, #2344]
	add	w8, w8, #1
	str	w8, [x21, #2344]
	cmp	w8, #4
	b.ne	LBB10_201
; %bb.136:
	mov	w8, #0                          ; =0x0
Lloh48:
	adrp	x9, lCPI10_17@PAGE
Lloh49:
	ldr	d0, [x9, lCPI10_17@PAGEOFF]
	b	LBB10_209
LBB10_137:
	ldr	x9, [x0, #24]
	ldr	x10, [x0, #56]
	cmp	x9, x10
	b.eq	LBB10_208
LBB10_138:
	add	x9, x10, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	lsr	w10, w9, #16
	str	w10, [x21, #2280]
	sub	w8, w8, #16
	str	w8, [x21, #2284]
	and	w8, w9, #0xf
	cmp	w8, #8
	b.ne	LBB10_182
; %bb.139:
	ubfx	x8, x9, #4, #4
	add	x10, x8, #8
	ldr	w8, [x0, #4]
	cmp	w10, w8
	b.hi	LBB10_198
; %bb.140:
	and	x8, x9, #0xf8
	cmp	w8, #128
	b.hs	LBB10_198
; %bb.141:
	rev	w8, w9
	lsr	w8, w8, #16
	mov	w11, #31711                     ; =0x7bdf
	mul	w8, w8, w11
	and	w8, w8, #0xffff
	cmp	w8, #2115
	b.lo	LBB10_210
; %bb.142:
	mov	w8, #0                          ; =0x0
Lloh50:
	adrp	x9, lCPI10_19@PAGE
Lloh51:
	ldr	d0, [x9, lCPI10_19@PAGEOFF]
	b	LBB10_209
LBB10_143:
	ldr	x10, [x0, #24]
	ldr	x11, [x0, #56]
LBB10_144:
	cmp	x10, x11
	b.eq	LBB10_208
; %bb.145:
	add	x10, x11, #1
	str	x10, [x0, #56]
	mov	w10, #-1                        ; =0xffffffff
	lsl	w10, w10, w8
	ldr	w11, [x21, #2280]
	bic	w10, w11, w10
	lsr	w11, w11, w8
	str	w11, [x21, #2280]
	sub	w8, w9, w8
	str	w8, [x21, #2284]
	ldr	w8, [x21, #2312]
	ldr	x9, [x21, #80]
	add	w8, w8, w10
	cmp	x9, x8
	str	w8, [x21, #2312]
	b.hs	LBB10_172
; %bb.146:
	mov	w8, #0                          ; =0x0
Lloh52:
	adrp	x9, lCPI10_7@PAGE
Lloh53:
	ldr	d0, [x9, lCPI10_7@PAGEOFF]
	b	LBB10_209
LBB10_147:
	ldr	x9, [x1, #40]
	ldr	x8, [x1, #24]
	cmp	x9, x8
	b.ne	LBB10_166
LBB10_148:
	mov	w8, #0                          ; =0x0
	mov	w9, #1                          ; =0x1
	str	w9, [x1, #52]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_149:
	ldr	x9, [x0, #24]
	ldr	x11, [x0, #56]
LBB10_150:
	cmp	x9, x11
	b.eq	LBB10_208
; %bb.151:
	add	x9, x11, #1
	str	x9, [x0, #56]
	ldr	w9, [x21, #2280]
	lsr	w10, w9, #8
	str	w10, [x21, #2280]
	sub	w8, w8, #8
	str	w8, [x21, #2284]
	ldr	w8, [x21, #2340]
	bfi	w9, w8, #8, #24
	str	w9, [x21, #2340]
	ldr	w8, [x21, #2344]
	add	w8, w8, #1
	str	w8, [x21, #2344]
	cmp	w8, #4
	b.ne	LBB10_201
; %bb.152:
	ldr	w8, [x21, #2336]
	ldr	w10, [x21, #2332]
	orr	w8, w10, w8, lsl #16
	cmp	w9, w8
	b.ne	LBB10_197
; %bb.153:
	mov	w8, #18                         ; =0x12
	b	LBB10_192
LBB10_154:
	mov	w8, #3                          ; =0x3
	str	w8, [x0, #32]
	ldrb	w8, [x21, #2328]
	cmp	w8, #1
	b.ne	LBB10_201
; %bb.155:
	movi.2d	v0, #0000000000000000
	str	d0, [x21, #2280]
	ldr	w8, [x0]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	b	LBB10_192
LBB10_156:
	mov	w8, #11                         ; =0xb
	b	LBB10_192
LBB10_157:
	ldr	x15, [x0, #40]
	ldr	x12, [x0, #8]
	cmp	x15, x12
	b.ne	LBB10_187
LBB10_158:
	mov	w8, #0                          ; =0x0
Lloh54:
	adrp	x9, lCPI10_3@PAGE
Lloh55:
	ldr	d0, [x9, lCPI10_3@PAGEOFF]
	b	LBB10_209
LBB10_159:
	cmp	w8, #256
	mov	x9, x19
	b.ne	LBB10_189
; %bb.160:
	mov	w8, #3                          ; =0x3
	str	w8, [x9, #32]
	ldrb	w8, [x21, #2328]
	cmp	w8, #1
	b.ne	LBB10_201
; %bb.161:
	movi.2d	v0, #0000000000000000
	str	d0, [x21, #2280]
	ldr	w8, [x9]
	cmp	w8, #1
	mov	w8, #17                         ; =0x11
	cinc	w8, w8, eq
	b	LBB10_203
LBB10_162:
	ldr	x10, [x0, #24]
	ldr	x12, [x0, #56]
LBB10_163:
	cmp	x10, x12
	b.eq	LBB10_208
; %bb.164:
	add	x10, x12, #1
	str	x10, [x0, #56]
	ldr	w10, [x21, #2280]
	lsr	w11, w10, #3
	str	w11, [x21, #2280]
	sub	w9, w9, #3
Lloh56:
	adrp	x11, __ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE@PAGE
Lloh57:
	add	x11, x11, __ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE@PAGEOFF
	str	w9, [x21, #2284]
	ldr	w8, [x11, x8, lsl #2]
	and	w9, w10, #0x7
	add	x8, x0, x8
	mov	w10, #35012                     ; =0x88c4
	strb	w9, [x8, x10]
LBB10_165:
	ldr	w8, [x21, #2300]
	add	w8, w8, #1
	str	w8, [x21, #2300]
	b	LBB10_201
LBB10_166:
	ldr	x10, [x0, #24]
	ldr	x8, [x0, #56]
	cmp	x10, x8
	b.eq	LBB10_208
; %bb.167:
	add	x8, x8, #1
	str	x8, [x0, #56]
	ldr	w10, [x21, #2320]
	and	w8, w10, #0xff
	ldr	x11, [x1, #16]
	strb	w10, [x11, x9]
	ldr	x9, [x1, #40]
	add	x9, x9, #1
	str	x9, [x1, #40]
	ldr	x9, [x0, #48]
	add	x9, x9, #1
	str	x9, [x0, #48]
	ldr	x9, [x21, #64]
	add	x9, x0, x9
	strb	w10, [x9, #64]
	ldr	w9, [x21, #64]
	add	w9, w9, #1
	and	x9, x9, #0x7fff
	str	x9, [x21, #64]
	ldr	x9, [x21, #72]
	lsr	x10, x9, #15
	cbnz	x10, LBB10_169
; %bb.168:
	add	x9, x9, #1
	str	x9, [x21, #72]
LBB10_169:
	ldr	w9, [x21, #2332]
	mov	w10, #-65521                    ; =0xffff000f
	add	w8, w9, w8
	add	w9, w8, w10
	mov	w11, #65520                     ; =0xfff0
	cmp	w8, w11
	csel	w8, w9, w8, hi
	str	w8, [x21, #2332]
	ldr	w9, [x21, #2336]
	add	w8, w8, w9
	add	w9, w8, w10
	cmp	w8, w11
	csel	w8, w9, w8, hi
	str	w8, [x21, #2336]
	mov	w8, #11                         ; =0xb
	b	LBB10_192
LBB10_170:
Lloh58:
	adrp	x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGE
Lloh59:
	add	x9, x9, __ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE@PAGEOFF
	ldr	w9, [x9, x8, lsl #2]
	str	w9, [x21, #2312]
Lloh60:
	adrp	x9, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGE
Lloh61:
	add	x9, x9, __ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE@PAGEOFF
	ldr	w8, [x9, x8, lsl #2]
	str	w8, [x21, #2316]
	mov	w8, #15                         ; =0xf
	b	LBB10_200
LBB10_171:
Lloh62:
	adrp	x9, lCPI10_10@PAGE
Lloh63:
	ldr	d0, [x9, lCPI10_10@PAGEOFF]
	b	LBB10_209
LBB10_172:
	ldr	x9, [x21, #72]
	cmp	x9, x8
	b.hs	LBB10_191
; %bb.173:
	mov	w8, #0                          ; =0x0
Lloh64:
	adrp	x9, lCPI10_6@PAGE
Lloh65:
	ldr	d0, [x9, lCPI10_6@PAGEOFF]
	b	LBB10_209
LBB10_174:
	sub	x11, x12, x13
	cmp	x11, x9
	b.lo	LBB10_208
; %bb.175:
	add	x11, x13, x9
	str	x11, [x0, #56]
	mov	w11, #34692                     ; =0x8784
	add	x11, x0, x11
LBB10_176:                              ; =>This Inner Loop Header: Depth=1
	strb	w8, [x11, w10, uxtw]
	ldr	w10, [x21, #2300]
	add	w10, w10, #1
	str	w10, [x21, #2300]
	subs	w9, w9, #1
	b.ne	LBB10_176
; %bb.177:
	mov	w8, #9                          ; =0x9
	b	LBB10_192
LBB10_178:
	cmp	w8, #2
	b.ne	LBB10_196
; %bb.179:
	mov	w8, #7                          ; =0x7
	b	LBB10_192
LBB10_180:
	mov	w8, #0                          ; =0x0
Lloh66:
	adrp	x9, lCPI10_14@PAGE
Lloh67:
	ldr	d0, [x9, lCPI10_14@PAGEOFF]
	b	LBB10_209
LBB10_181:
	mov	w8, #0                          ; =0x0
Lloh68:
	adrp	x9, lCPI10_15@PAGE
Lloh69:
	ldr	d0, [x9, lCPI10_15@PAGEOFF]
	b	LBB10_209
LBB10_182:
	mov	w8, #0                          ; =0x0
Lloh70:
	adrp	x9, lCPI10_18@PAGE
Lloh71:
	ldr	d0, [x9, lCPI10_18@PAGEOFF]
	b	LBB10_209
LBB10_183:
	cmp	w8, #16
	b.ne	LBB10_199
; %bb.184:
	ldr	w9, [x21, #2300]
	cbnz	w9, LBB10_199
; %bb.185:
	mov	w8, #0                          ; =0x0
Lloh72:
	adrp	x9, lCPI10_9@PAGE
Lloh73:
	ldr	d0, [x9, lCPI10_9@PAGEOFF]
LBB10_186:
	str	d0, [x19, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_187:
	ldr	x12, [x0, #24]
	ldr	x14, [x0, #56]
	cmp	x12, x14
	b.eq	LBB10_208
; %bb.188:
	ldr	x16, [x1]
	ldr	w17, [x21, #2280]
	add	x14, x14, #1
	str	x14, [x0, #56]
	ldrb	w16, [x16, x13]
	lsl	w16, w16, w11
	orr	w16, w16, w17
	str	w16, [x21, #2280]
	add	w11, w11, #8
	str	w11, [x21, #2284]
	add	x13, x13, #1
	str	x13, [x1, #32]
	add	x13, x15, #1
	str	x13, [x0, #40]
	cmp	x12, x14
	b.ne	LBB10_111
	b	LBB10_208
LBB10_189:
	cmp	w8, #286
	b.lo	LBB10_202
; %bb.190:
	mov	w8, #0                          ; =0x0
	movi.2s	v0, #19
	str	d0, [x9, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_191:
	mov	w8, #16                         ; =0x10
LBB10_192:
	str	w8, [x0, #32]
	b	LBB10_201
LBB10_193:
	mov	x19, x0
	bl	__ZN9gui_forms11compression13InflateStream11fixed_treesEv
	cbz	w0, LBB10_195
; %bb.194:
	mov	w8, #11                         ; =0xb
	b	LBB10_200
LBB10_195:
	mov	w8, #0                          ; =0x0
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_196:
	mov	w8, #0                          ; =0x0
Lloh74:
	adrp	x9, lCPI10_16@PAGE
Lloh75:
	ldr	d0, [x9, lCPI10_16@PAGEOFF]
	b	LBB10_209
LBB10_197:
	mov	w8, #0                          ; =0x0
Lloh76:
	adrp	x9, lCPI10_1@PAGE
Lloh77:
	ldr	d0, [x9, lCPI10_1@PAGEOFF]
	b	LBB10_209
LBB10_198:
	mov	w8, #0                          ; =0x0
Lloh78:
	adrp	x9, lCPI10_20@PAGE
Lloh79:
	ldr	d0, [x9, lCPI10_20@PAGEOFF]
	b	LBB10_209
LBB10_199:
	str	w8, [x21, #2304]
	mov	w8, #10                         ; =0xa
LBB10_200:
	str	w8, [x19, #32]
LBB10_201:
	mov	w8, #1                          ; =0x1
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_202:
Lloh80:
	adrp	x10, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGE
Lloh81:
	add	x10, x10, __ZN9gui_forms11compression12_GLOBAL__N_111length_baseE@PAGEOFF
	sub	w8, w8, #257
	ldr	w10, [x10, w8, uxtw #2]
	str	w10, [x21, #2308]
Lloh82:
	adrp	x10, __ZN9gui_forms11compression12_GLOBAL__N_112length_extraE@PAGE
Lloh83:
	add	x10, x10, __ZN9gui_forms11compression12_GLOBAL__N_112length_extraE@PAGEOFF
	ldr	w8, [x10, w8, uxtw #2]
	str	w8, [x21, #2316]
	mov	w8, #13                         ; =0xd
LBB10_203:
	str	w8, [x9, #32]
	b	LBB10_201
LBB10_204:
	mov	x9, x17
	cmp	x8, x10
	b.ne	LBB10_114
	b	LBB10_208
LBB10_205:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB10_119
	b	LBB10_208
LBB10_206:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB10_130
	b	LBB10_208
LBB10_207:
	mov	x8, x17
	cmp	x9, x10
	b.ne	LBB10_138
LBB10_208:
	mov	w8, #0                          ; =0x0
Lloh84:
	adrp	x9, lCPI10_2@PAGE
Lloh85:
	ldr	d0, [x9, lCPI10_2@PAGEOFF]
LBB10_209:
	str	d0, [x0, #32]
	mov	x0, x8
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB10_210:
	mov	w8, #1                          ; =0x1
	lsl	x10, x8, x10
	str	x10, [x21, #80]
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
	.loh AdrpAdd	Lloh30, Lloh31
	.loh AdrpLdr	Lloh32, Lloh33
	.loh AdrpLdr	Lloh34, Lloh35
	.loh AdrpLdr	Lloh36, Lloh37
	.loh AdrpLdr	Lloh38, Lloh39
	.loh AdrpLdr	Lloh40, Lloh41
	.loh AdrpLdr	Lloh46, Lloh47
	.loh AdrpAdrp	Lloh44, Lloh46
	.loh AdrpLdr	Lloh44, Lloh45
	.loh AdrpAdrp	Lloh42, Lloh44
	.loh AdrpLdr	Lloh42, Lloh43
	.loh AdrpLdr	Lloh48, Lloh49
	.loh AdrpLdr	Lloh50, Lloh51
	.loh AdrpLdr	Lloh52, Lloh53
	.loh AdrpLdr	Lloh54, Lloh55
	.loh AdrpAdd	Lloh56, Lloh57
	.loh AdrpAdd	Lloh60, Lloh61
	.loh AdrpAdd	Lloh58, Lloh59
	.loh AdrpLdr	Lloh62, Lloh63
	.loh AdrpLdr	Lloh64, Lloh65
	.loh AdrpLdr	Lloh66, Lloh67
	.loh AdrpLdr	Lloh68, Lloh69
	.loh AdrpLdr	Lloh70, Lloh71
	.loh AdrpLdr	Lloh72, Lloh73
	.loh AdrpLdr	Lloh74, Lloh75
	.loh AdrpLdr	Lloh76, Lloh77
	.loh AdrpLdr	Lloh78, Lloh79
	.loh AdrpAdd	Lloh82, Lloh83
	.loh AdrpAdd	Lloh80, Lloh81
	.loh AdrpLdr	Lloh84, Lloh85
	.cfi_endproc
	.section	__TEXT,__const
	.p2align	1, 0x0
LJTI10_0:
	.short	(LBB10_3-LBB10_2)>>2
	.short	(LBB10_70-LBB10_2)>>2
	.short	(LBB10_65-LBB10_2)>>2
	.short	(LBB10_36-LBB10_2)>>2
	.short	(LBB10_44-LBB10_2)>>2
	.short	(LBB10_20-LBB10_2)>>2
	.short	(LBB10_59-LBB10_2)>>2
	.short	(LBB10_11-LBB10_2)>>2
	.short	(LBB10_53-LBB10_2)>>2
	.short	(LBB10_4-LBB10_2)>>2
	.short	(LBB10_9-LBB10_2)>>2
	.short	(LBB10_41-LBB10_2)>>2
	.short	(LBB10_91-LBB10_2)>>2
	.short	(LBB10_29-LBB10_2)>>2
	.short	(LBB10_98-LBB10_2)>>2
	.short	(LBB10_79-LBB10_2)>>2
	.short	(LBB10_86-LBB10_2)>>2
	.short	(LBB10_93-LBB10_2)>>2
	.short	(LBB10_2-LBB10_2)>>2
	.short	(LBB10_2-LBB10_2)>>2
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
lCPI11_0:
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
	cbz	x2, LBB11_3
; %bb.1:
	cbz	x4, LBB11_3
; %bb.2:
	sub	x8, x3, x1
	cmp	x8, x2
	cset	w8, lo
	subs	x9, x1, x3
	cmp	x9, x4
	cset	w9, lo
	cmp	x1, x3
	csel	w9, w9, w8, hi
	b	LBB11_4
LBB11_3:
	mov	w9, #0                          ; =0x0
	mov	w8, #0                          ; =0x0
	cbz	x2, LBB11_5
LBB11_4:
	subs	x8, x20, x1
	cmp	x8, x2
	cset	w8, lo
	sub	x10, x1, x20
	lsr	x10, x10, #4
	cmp	x10, #2195
	cset	w10, lo
	cmp	x20, x1
	csel	w8, w10, w8, lo
	orr	w8, w8, w9
LBB11_5:
	subs	x9, x20, x3
	cmp	x9, x4
	cset	w9, lo
	sub	x10, x3, x20
	lsr	x10, x10, #4
	cmp	x10, #2195
	cset	w10, lo
	cmp	x20, x3
	csel	w9, w10, w9, lo
	cmp	x4, #0
	csel	w10, wzr, w9, eq
	ldr	w9, [x20, #32]
	orr	w8, w8, w10
	cmp	w8, #1
	b.ne	LBB11_8
; %bb.6:
	and	w8, w9, #0xfffffffe
	cmp	w8, #18
	b.eq	LBB11_8
; %bb.7:
	mov	w8, #1                          ; =0x1
	b	LBB11_10
LBB11_8:
	cbnz	w9, LBB11_11
; %bb.9:
	mov	w8, #2                          ; =0x2
LBB11_10:
	mov	w9, #19                         ; =0x13
	stp	w9, w8, [x20, #32]
LBB11_11:                               ; =>This Inner Loop Header: Depth=1
	ldr	w8, [x20, #32]
	and	w9, w8, #0xfffffffe
	cmp	w9, #18
	b.eq	LBB11_15
; %bb.12:                               ;   in Loop: Header=BB11_11 Depth=1
	ldr	x9, [x20, #24]
	ldr	x8, [x20, #56]
	cmp	x9, x8
	b.eq	LBB11_18
; %bb.13:                               ;   in Loop: Header=BB11_11 Depth=1
	add	x8, x8, #1
	str	x8, [x20, #56]
	add	x1, sp, #8
	mov	x0, x20
	bl	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	tbnz	w0, #0, LBB11_11
; %bb.14:
	ldr	w8, [x20, #32]
LBB11_15:
	cmp	w8, #18
	b.eq	LBB11_20
; %bb.16:
	cmp	w8, #19
	b.eq	LBB11_19
; %bb.17:
	ldr	w8, [sp, #60]
	b	LBB11_21
LBB11_18:
Lloh86:
	adrp	x8, lCPI11_0@PAGE
Lloh87:
	ldr	d0, [x8, lCPI11_0@PAGEOFF]
	str	d0, [x20, #32]
LBB11_19:
	mov	w8, #3                          ; =0x3
	b	LBB11_21
LBB11_20:
	mov	w8, #2                          ; =0x2
LBB11_21:
	ldr	w9, [x20, #36]
	stp	w8, w9, [x19]
	ldur	q0, [sp, #40]
	stur	q0, [x19, #8]
	ldur	q0, [x20, #40]
	stur	q0, [x19, #24]
	ldr	x8, [x20, #56]
	str	x8, [x19, #40]
	mov	w8, #35116                      ; =0x892c
	ldr	w8, [x20, x8]
	str	w8, [x19, #48]
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #96
	ret
	.loh AdrpLdr	Lloh86, Lloh87
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
