	.build_version macos, 26, 0	sdk_version 26, 2
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE ; -- Begin function _ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE
	.p2align	2
__ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE: ; @_ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE
	.cfi_startproc
; %bb.0:
	stp	x24, x23, [sp, #-64]!           ; 16-byte Folded Spill
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	sub	sp, sp, #2384
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	mov	x20, x2
	mov	x21, x1
	mov	x22, x0
	mov	x19, x8
	movi.2d	v0, #0000000000000000
	strb	wzr, [sp, #16]
	movi.2d	v2, #0000000000000000
	str	q2, [sp]
	stur	q2, [sp, #24]
	mov	x8, sp
	stur	q2, [sp, #40]
	str	xzr, [sp, #56]
	movi.2s	v1, #1
	str	d1, [sp, #64]
	str	xzr, [sp, #72]
	str	q2, [sp, #80]
	str	d1, [sp, #96]
	str	xzr, [sp, #104]
	mov.d	v0[1], v0[0]
	str	q0, [sp, #112]
	str	d1, [sp, #128]
	str	xzr, [sp, #136]
	str	q2, [sp, #144]
	str	d1, [sp, #160]
	str	xzr, [sp, #184]
	stur	q2, [x8, #168]
	str	d1, [sp, #192]
	str	xzr, [sp, #216]
	stur	q2, [x8, #200]
	str	d1, [sp, #224]
	str	xzr, [sp, #248]
	stur	q2, [x8, #232]
	add	x23, x8, #2360
	str	d1, [sp, #256]
	add	x0, x8, #264
	mov	w1, #2093                       ; =0x82d
	bl	_bzero
	movi.2d	v0, #0000000000000000
	str	q0, [x23]
	str	xzr, [sp, #2376]
	stp	xzr, xzr, [x19, #8]
	str	xzr, [x19]
	strb	wzr, [x19, #24]
	stp	q0, q0, [x19, #32]
	str	q0, [x19, #64]
	mov	x3, sp
	mov	x0, x22
	mov	x1, x21
	mov	x2, x20
	bl	__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	str	w0, [x19]
	cbz	w0, LBB0_2
; %bb.1:
	add	sp, sp, #2384
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp], #64             ; 16-byte Folded Reload
	ret
LBB0_2:
	ldp	q0, q1, [sp]
	stur	q0, [x19, #8]
	stur	q1, [x19, #24]
	ldr	q0, [sp, #32]
	stur	q0, [x19, #40]
	ldr	x8, [sp, #48]
	str	x8, [x19, #56]
	add	sp, sp, #2384
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp], #64             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE: ; @_ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	.cfi_startproc
; %bb.0:
	ldp	q1, q0, [x2]
	movi.2d	v2, #0xffffffffffffffff
	cmeq.2d	v0, v0, v2
	cmeq.2d	v1, v1, v2
	uzp1.4s	v0, v1, v0
	umaxv.4s	s0, v0
	fmov	w9, s0
	mov	w8, #1                          ; =0x1
	tbnz	w9, #0, LBB1_5
; %bb.1:
	ldr	x9, [x2, #32]
	cmn	x9, #1
	b.eq	LBB1_5
; %bb.2:
	ldr	w9, [x2, #40]
	tbnz	w9, #31, LBB1_5
; %bb.3:
	ldr	x8, [x2]
	cmp	x1, x8
	b.ls	LBB1_6
; %bb.4:
	mov	w8, #2                          ; =0x2
LBB1_5:
	mov	x0, x8
	ret
LBB1_6:
	cmp	x1, #8
	b.hs	LBB1_8
; %bb.7:
	mov	w0, #3                          ; =0x3
	ret
LBB1_8:
	sub	sp, sp, #336
	stp	x28, x27, [sp, #240]            ; 16-byte Folded Spill
	stp	x26, x25, [sp, #256]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #272]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #288]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #304]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #320]            ; 16-byte Folded Spill
	add	x29, sp, #320
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
	ldrb	w8, [x0]
	cmp	w8, #137
	b.ne	LBB1_18
; %bb.9:
	ldrb	w8, [x0, #1]
	cmp	w8, #80
	b.ne	LBB1_18
; %bb.10:
	ldrb	w8, [x0, #2]
	cmp	w8, #78
	b.ne	LBB1_18
; %bb.11:
	ldrb	w8, [x0, #3]
	cmp	w8, #71
	b.ne	LBB1_18
; %bb.12:
	ldrb	w8, [x0, #4]
	cmp	w8, #13
	b.ne	LBB1_18
; %bb.13:
	ldrb	w8, [x0, #5]
	cmp	w8, #10
	b.ne	LBB1_18
; %bb.14:
	ldrb	w8, [x0, #6]
	cmp	w8, #26
	b.ne	LBB1_18
; %bb.15:
	ldrb	w8, [x0, #7]
	cmp	w8, #10
	b.ne	LBB1_18
; %bb.16:
	cmp	x1, #8
	b.ne	LBB1_20
LBB1_17:
	mov	w8, #13                         ; =0xd
	b	LBB1_19
LBB1_18:
	mov	w8, #3                          ; =0x3
LBB1_19:
	ldp	x29, x30, [sp, #320]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #304]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #288]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #272]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #256]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #240]            ; 16-byte Folded Reload
	add	sp, sp, #336
	mov	x0, x8
	ret
LBB1_20:
	stp	xzr, xzr, [sp, #64]             ; 16-byte Folded Spill
	mov	w26, #0                         ; =0x0
	str	wzr, [sp, #88]                  ; 4-byte Spill
	str	wzr, [sp, #80]                  ; 4-byte Spill
	mov	w12, #0                         ; =0x0
	add	x9, x3, #292
	add	x8, x0, #8
	stp	x9, x8, [sp, #24]               ; 16-byte Folded Spill
	add	x8, x3, #298
	str	x8, [sp, #40]                   ; 8-byte Spill
	add	x9, x3, #354
	add	x8, x0, #10
	stp	x9, x8, [sp, #8]                ; 16-byte Folded Spill
	movi.2d	v3, #0xffffffffffffffff
	mov	w11, #8                         ; =0x8
	stp	x1, x2, [sp, #112]              ; 16-byte Folded Spill
	str	x3, [sp, #128]                  ; 8-byte Spill
	stur	x0, [x29, #-112]                ; 8-byte Folded Spill
LBB1_21:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_118 Depth 2
                                        ;     Child Loop BB1_110 Depth 2
                                        ;     Child Loop BB1_61 Depth 2
                                        ;     Child Loop BB1_122 Depth 2
                                        ;     Child Loop BB1_115 Depth 2
                                        ;     Child Loop BB1_84 Depth 2
	ldr	w8, [x2, #44]
	cmp	w12, w8
	b.eq	LBB1_127
; %bb.22:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	x13, x11
	sub	x8, x1, x11
	subs	x10, x8, #12
	b.lo	LBB1_125
; %bb.23:                               ;   in Loop: Header=BB1_21 Depth=1
	add	x8, x0, x13
	ldrb	w9, [x8]
	lsl	w27, w9, #24
	tbnz	w27, #31, LBB1_125
; %bb.24:                               ;   in Loop: Header=BB1_21 Depth=1
	ldrb	w14, [x8, #1]
	mov	x11, x27
	bfi	w11, w14, #16, #8
	ldrb	w21, [x8, #2]
	ldrb	w25, [x8, #3]
	mov	x19, x25
	bfi	w19, w21, #8, #8
	orr	w11, w11, w19
	str	x11, [sp, #136]                 ; 8-byte Spill
	cmp	x10, x11
	b.lo	LBB1_125
; %bb.25:                               ;   in Loop: Header=BB1_21 Depth=1
	ldrb	w11, [x8, #4]
	ldrb	w28, [x8, #5]
	ldrb	w22, [x8, #6]
	ldrb	w20, [x8, #7]
	add	x8, x13, #8
	add	x10, x13, #4
	cmp	x10, x8
	str	w11, [sp, #108]                 ; 4-byte Spill
	b.hs	LBB1_31
; %bb.26:                               ;   in Loop: Header=BB1_21 Depth=1
	and	w8, w11, #0xffffffdf
	sub	w8, w8, #65
	cmp	w8, #25
	b.hi	LBB1_126
; %bb.27:                               ;   in Loop: Header=BB1_21 Depth=1
	and	w8, w28, #0xffffffdf
	sub	w8, w8, #65
	cmp	w8, #25
	b.hi	LBB1_126
; %bb.28:                               ;   in Loop: Header=BB1_21 Depth=1
	and	w8, w22, #0xffffffdf
	sub	w8, w8, #65
	cmp	w8, #25
	b.hi	LBB1_126
; %bb.29:                               ;   in Loop: Header=BB1_21 Depth=1
	and	w8, w20, #0xffffffdf
	sub	w11, w8, #65
	mov	w8, #5                          ; =0x5
	cmp	w11, #25
	b.hi	LBB1_19
; %bb.30:                               ;   in Loop: Header=BB1_21 Depth=1
	add	x24, x13, #8
	tbz	w22, #5, LBB1_32
	b	LBB1_19
LBB1_31:                                ;   in Loop: Header=BB1_21 Depth=1
	add	x24, x13, #8
	tbnz	w22, #5, LBB1_126
LBB1_32:                                ;   in Loop: Header=BB1_21 Depth=1
	str	w26, [sp, #84]                  ; 4-byte Spill
	add	x8, sp, #144
	st1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Spill
	str	w12, [sp, #92]                  ; 4-byte Spill
	and	x8, x9, #0xff
	lsl	x8, x8, #24
	stp	x14, x13, [sp, #48]             ; 16-byte Folded Spill
	orr	x8, x8, x14, lsl #16
	orr	x8, x8, x21, lsl #8
	orr	x23, x8, x25
	add	x8, x13, x23
	add	x26, x8, #12
	ldur	x8, [x29, #-112]                ; 8-byte Folded Reload
	add	x0, x8, x10
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	add	x1, x8, #4
	bl	__ZN9gui_forms5image9png_crc32ENSt3__14spanIKSt4byteLm18446744073709551615EEE
	mov	x8, x0
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	str	x26, [sp, #96]                  ; 8-byte Spill
	add	x9, x0, x26
	ldur	w9, [x9, #-4]
	rev	w9, w9
	cmp	w8, w9
	b.ne	LBB1_128
; %bb.33:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w10, [sp, #108]                 ; 4-byte Reload
	lsl	w8, w10, #24
	orr	w8, w8, w28, lsl #16
	orr	w8, w8, w20
	orr	w22, w8, w22, lsl #8
	mov	w8, #17490                      ; =0x4452
	movk	w8, #18760, lsl #16
	cmp	w22, w8
	cset	w8, eq
	ldr	w9, [sp, #68]                   ; 4-byte Reload
	orr	w8, w9, w8
	tbz	w8, #0, LBB1_134
; %bb.34:                               ;   in Loop: Header=BB1_21 Depth=1
	add	x28, x0, x24
	mov	w8, #16724                      ; =0x4154
	movk	w8, #18756, lsl #16
	cmp	w22, w8
	cset	w20, ne
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldr	x3, [sp, #128]                  ; 8-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	w26, [sp, #84]                  ; 4-byte Reload
	tbz	w10, #5, LBB1_38
; %bb.35:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [x2, #24]
	ldr	x9, [sp, #72]                   ; 8-byte Reload
	sub	x8, x8, x9
	cmp	x23, x8
	b.hi	LBB1_127
; %bb.36:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [x3, #48]
	add	x8, x8, #1
	str	x8, [x3, #48]
	sub	x3, x29, #89
	sub	x4, x29, #104
	mov	x0, x22
	mov	x1, x28
	mov	x2, x23
	bl	__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	cbnz	w0, LBB1_130
; %bb.37:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #72]                   ; 8-byte Reload
	add	x8, x23, x8
	str	x8, [sp, #72]                   ; 8-byte Spill
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldr	x3, [sp, #128]                  ; 8-byte Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	w10, [sp, #108]                 ; 4-byte Reload
LBB1_38:                                ;   in Loop: Header=BB1_21 Depth=1
	and	w8, w26, w20
	ldr	w9, [sp, #88]                   ; 4-byte Reload
	orr	w9, w8, w9
	mov	w8, #19776                      ; =0x4d40
	movk	w8, #26433, lsl #16
	cmp	w22, w8
	str	w9, [sp, #88]                   ; 4-byte Spill
	b.gt	LBB1_45
; %bb.39:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #21572                      ; =0x5444
	movk	w8, #20556, lsl #16
	cmp	w22, w8
	b.gt	LBB1_48
; %bb.40:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #16724                      ; =0x4154
	movk	w8, #18756, lsl #16
	cmp	w22, w8
	b.eq	LBB1_86
; %bb.41:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #17490                      ; =0x4452
	movk	w8, #18760, lsl #16
	cmp	w22, w8
	b.ne	LBB1_104
; %bb.42:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cmp	w8, #13
	cset	w8, ne
	ldr	w9, [sp, #68]                   ; 4-byte Reload
	orr	w8, w9, w8
	tbnz	w8, #0, LBB1_135
; %bb.43:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	x0, x28
	mov	x1, x23
	ldp	x2, x3, [sp, #120]              ; 16-byte Folded Reload
	bl	__ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	cbnz	w0, LBB1_130
; %bb.44:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #1                          ; =0x1
	str	w8, [sp, #68]                   ; 4-byte Spill
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	ldr	w26, [sp, #84]                  ; 4-byte Reload
	b	LBB1_106
LBB1_45:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #18241                      ; =0x4741
	movk	w8, #29522, lsl #16
	cmp	w22, w8
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	b.gt	LBB1_50
; %bb.46:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #19777                      ; =0x4d41
	movk	w8, #26433, lsl #16
	cmp	w22, w8
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	b.eq	LBB1_62
; %bb.47:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #17232                      ; =0x4350
	movk	w8, #26947, lsl #16
	cmp	w22, w8
	b.eq	LBB1_62
	b	LBB1_105
LBB1_48:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #21573                      ; =0x5445
	movk	w8, #20556, lsl #16
	cmp	w22, w8
	b.eq	LBB1_71
; %bb.49:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #21069                      ; =0x524d
	movk	w8, #25416, lsl #16
	cmp	w22, w8
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	b.eq	LBB1_62
	b	LBB1_105
LBB1_50:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #18242                      ; =0x4742
	movk	w8, #29522, lsl #16
	cmp	w22, w8
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	b.eq	LBB1_62
; %bb.51:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #20051                      ; =0x4e53
	movk	w8, #29778, lsl #16
	cmp	w22, w8
	b.ne	LBB1_105
; %bb.52:                               ;   in Loop: Header=BB1_21 Depth=1
	ldrb	w8, [x3, #2356]
	orr	w8, w8, w26
	tbnz	w8, #0, LBB1_132
; %bb.53:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #128]                  ; 8-byte Reload
	ldr	w9, [x8, #12]
	cmp	w9, #3
	b.ne	LBB1_96
; %bb.54:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cmp	w8, #0
	cset	w9, eq
	mov	w8, #10                         ; =0xa
	ldr	w10, [sp, #64]                  ; 4-byte Reload
	orn	w9, w9, w10
	tbnz	w9, #0, LBB1_19
; %bb.55:                               ;   in Loop: Header=BB1_21 Depth=1
	ldp	x9, x10, [sp, #128]             ; 16-byte Folded Reload
	ldr	w9, [x9, #2340]
	cmp	w10, w9
	ldr	x9, [sp, #48]                   ; 8-byte Reload
	b.hi	LBB1_19
; %bb.56:                               ;   in Loop: Header=BB1_21 Depth=1
	lsl	w8, w9, #16
	lsl	w9, w21, #8
	orr	w8, w27, w8
	orr	w9, w9, w25
	orr	w8, w8, w9
	cmp	w8, #8
	b.lo	LBB1_59
; %bb.57:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x9, [sp, #32]                   ; 8-byte Reload
	ldr	x10, [sp, #56]                  ; 8-byte Reload
	add	x9, x9, x10
	add	x9, x9, x8
	ldr	x10, [sp, #40]                  ; 8-byte Reload
	cmp	x10, x9
	b.hs	LBB1_107
; %bb.58:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x9, [sp, #24]                   ; 8-byte Reload
	add	x9, x9, x8, lsl #3
	cmp	x28, x9
	b.hs	LBB1_107
LBB1_59:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	x9, #0                          ; =0x0
LBB1_60:                                ;   in Loop: Header=BB1_21 Depth=1
	ldr	x10, [sp, #56]                  ; 8-byte Reload
	add	x10, x9, x10
	ldr	x11, [sp, #32]                  ; 8-byte Reload
	add	x10, x11, x10
	sub	x8, x8, x9
	ldr	x11, [sp, #40]                  ; 8-byte Reload
	add	x9, x11, x9, lsl #3
LBB1_61:                                ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrb	w11, [x10], #1
	orr	w11, w11, w11, lsl #8
	strh	w11, [x9], #8
	subs	x8, x8, #1
	b.ne	LBB1_61
	b	LBB1_102
LBB1_62:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #19777                      ; =0x4d41
	movk	w8, #26433, lsl #16
	cmp	w22, w8
	mov	w8, #1                          ; =0x1
	cinc	w8, w8, eq
	mov	w9, #21069                      ; =0x524d
	movk	w9, #25416, lsl #16
	cmp	w22, w9
	mov	w9, #4                          ; =0x4
	csel	w8, w9, w8, eq
	mov	w9, #17232                      ; =0x4350
	movk	w9, #26947, lsl #16
	cmp	w22, w9
	mov	w9, #8                          ; =0x8
	csel	w9, w9, w8, eq
	ldr	w8, [sp, #64]                   ; 4-byte Reload
	orr	w10, w8, w26
	mov	w8, #11                         ; =0xb
	tbnz	w10, #0, LBB1_19
; %bb.63:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w10, [sp, #80]                  ; 4-byte Reload
	and	w10, w9, w10
	cbnz	w10, LBB1_19
; %bb.64:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #18242                      ; =0x4742
	movk	w8, #29522, lsl #16
	cmp	w22, w8
	b.ne	LBB1_68
; %bb.65:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cmp	w8, #1
	b.ne	LBB1_129
; %bb.66:                               ;   in Loop: Header=BB1_21 Depth=1
	ldrb	w10, [x28]
	mov	w8, #11                         ; =0xb
	cmp	w10, #3
	b.hi	LBB1_19
; %bb.67:                               ;   in Loop: Header=BB1_21 Depth=1
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w10, [sp, #80]                  ; 4-byte Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x11, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x11]       ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	tbz	w10, #3, LBB1_95
	b	LBB1_19
LBB1_68:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #19777                      ; =0x4d41
	movk	w8, #26433, lsl #16
	cmp	w22, w8
	b.ne	LBB1_91
; %bb.69:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cmp	w8, #4
	b.ne	LBB1_129
; %bb.70:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	s0, [x28]
	ushll.8h	v0, v0, #0
	fmov	x8, d0
	lsr	x10, x8, #32
	orr	w8, w8, w10
	orr	w8, w8, w8, lsr #16
	tst	w8, #0xff
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	b.ne	LBB1_95
	b	LBB1_129
LBB1_71:                                ;   in Loop: Header=BB1_21 Depth=1
	ldr	w8, [sp, #64]                   ; 4-byte Reload
	orr	w8, w8, w26
	tbnz	w8, #0, LBB1_131
; %bb.72:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #9                          ; =0x9
	mov	w9, #43691                      ; =0xaaab
	movk	w9, #43690, lsl #16
	ldr	x10, [sp, #136]                 ; 8-byte Reload
	mul	w9, w10, w9
	mov	w10, #1431655765                ; =0x55555555
	cmp	w9, w10
	b.hi	LBB1_19
; %bb.73:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x9, [sp, #136]                  ; 8-byte Reload
	sub	w9, w9, #769
	cmn	w9, #768
	b.lo	LBB1_19
; %bb.74:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x9, [sp, #128]                  ; 8-byte Reload
	ldrb	w10, [x9, #2356]
	tbnz	w10, #0, LBB1_19
; %bb.75:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w9, [x9, #12]
	tst	w9, #0xfffffffb
	b.eq	LBB1_19
; %bb.76:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #43691                      ; =0xaaab
	mul	w8, w19, w8
	lsr	w8, w8, #17
	ldr	x10, [sp, #128]                 ; 8-byte Reload
	str	w8, [x10, #2340]
	cmp	w9, #3
	b.ne	LBB1_78
; %bb.77:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x9, [sp, #128]                  ; 8-byte Reload
	ldr	w9, [x9, #8]
	mov	w10, #1                         ; =0x1
	lsl	w9, w10, w9
	cmp	w9, w8
	b.lo	LBB1_131
LBB1_78:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	w19, #3
	b.lo	LBB1_85
; %bb.79:                               ;   in Loop: Header=BB1_21 Depth=1
	cmp	x8, #1
	csinc	x10, x8, xzr, hi
	cmp	w19, #24
	b.lo	LBB1_82
; %bb.80:                               ;   in Loop: Header=BB1_21 Depth=1
	cmp	x8, #1
	csinc	x9, x8, xzr, hi
	add	x11, x9, x9, lsl #1
	ldr	x12, [sp, #32]                  ; 8-byte Reload
	ldr	x13, [sp, #56]                  ; 8-byte Reload
	add	x12, x12, x13
	add	x11, x12, x11
	ldr	x12, [sp, #24]                  ; 8-byte Reload
	cmp	x12, x11
	b.hs	LBB1_112
; %bb.81:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x11, [sp, #24]                  ; 8-byte Reload
	add	x9, x11, x9, lsl #3
	cmp	x28, x9
	b.hs	LBB1_112
LBB1_82:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	x9, #0                          ; =0x0
LBB1_83:                                ;   in Loop: Header=BB1_21 Depth=1
	add	x10, x9, x9, lsl #1
	ldr	x11, [sp, #16]                  ; 8-byte Reload
	ldr	x12, [sp, #56]                  ; 8-byte Reload
	add	x11, x11, x12
	add	x10, x11, x10
	ldr	x11, [sp, #40]                  ; 8-byte Reload
	add	x11, x11, x9, lsl #3
	mov	w13, #65535                     ; =0xffff
LBB1_84:                                ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldurb	w12, [x10, #-2]
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x11, #-6]
	ldurb	w12, [x10, #-1]
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x11, #-4]
	ldrb	w12, [x10], #3
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x11, #-2]
	strh	w13, [x11], #8
	add	x9, x9, #1
	cmp	x9, x8
	b.lo	LBB1_84
LBB1_85:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w26, #0                         ; =0x0
	mov	w8, #1                          ; =0x1
	str	w8, [sp, #64]                   ; 4-byte Spill
	b	LBB1_103
LBB1_86:                                ;   in Loop: Header=BB1_21 Depth=1
	tbnz	w9, #0, LBB1_134
; %bb.87:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #128]                  ; 8-byte Reload
	ldr	w8, [x8, #12]
	cmp	w8, #3
	cset	w8, ne
	ldr	w9, [sp, #64]                   ; 4-byte Reload
	orr	w8, w8, w9
	tbz	w8, #0, LBB1_131
; %bb.88:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w8, [sp, #84]                   ; 4-byte Reload
	tbnz	w8, #0, LBB1_90
; %bb.89:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #128]                  ; 8-byte Reload
	ldr	x9, [sp, #56]                   ; 8-byte Reload
	str	x9, [x8, #2360]
LBB1_90:                                ;   in Loop: Header=BB1_21 Depth=1
	ldp	x3, x9, [sp, #128]              ; 16-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	str	x11, [x3, #2368]
	ldr	x8, [x3, #2376]
	add	x8, x8, x9
	str	x8, [x3, #2376]
	mov	w26, #1                         ; =0x1
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	b	LBB1_106
LBB1_91:                                ;   in Loop: Header=BB1_21 Depth=1
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cmp	w8, #32
	b.eq	LBB1_93
; %bb.92:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #21069                      ; =0x524d
	movk	w8, #25416, lsl #16
	cmp	w22, w8
	b.eq	LBB1_129
LBB1_93:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #17232                      ; =0x4350
	movk	w8, #26947, lsl #16
	cmp	w22, w8
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	b.ne	LBB1_95
; %bb.94:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w8, [sp, #80]                   ; 4-byte Reload
	tbnz	w8, #0, LBB1_129
LBB1_95:                                ;   in Loop: Header=BB1_21 Depth=1
	mov	w26, #0                         ; =0x0
	str	wzr, [sp, #64]                  ; 4-byte Spill
	ldr	w8, [sp, #80]                   ; 4-byte Reload
	orr	w8, w9, w8
	str	w8, [sp, #80]                   ; 4-byte Spill
	b	LBB1_106
LBB1_96:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	w9, #0
	mov	w8, #6                          ; =0x6
	mov	w10, #2                         ; =0x2
	csel	w10, w10, w8, eq
	mov	w8, #10                         ; =0xa
	tst	w9, #0xfffffffd
	b.ne	LBB1_19
; %bb.97:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	x11, [sp, #136]                 ; 8-byte Reload
	cmp	w11, w10
	ldr	x10, [sp, #128]                 ; 8-byte Reload
	b.ne	LBB1_19
; %bb.98:                               ;   in Loop: Header=BB1_21 Depth=1
	ldr	w8, [x10, #8]
	ldrh	w11, [x28]
	rev16	w11, w11
	str	w11, [x10, #2344]
	lsr	w10, w11, w8
	cbnz	w10, LBB1_132
; %bb.99:                               ;   in Loop: Header=BB1_21 Depth=1
	cbz	w9, LBB1_102
; %bb.100:                              ;   in Loop: Header=BB1_21 Depth=1
	ldrh	w9, [x28, #2]
	rev16	w10, w9
	ldr	x9, [sp, #128]                  ; 8-byte Reload
	str	w10, [x9, #2348]
	lsr	w9, w10, w8
	cbnz	w9, LBB1_132
; %bb.101:                              ;   in Loop: Header=BB1_21 Depth=1
	ldrh	w9, [x28, #4]
	rev16	w10, w9
	ldr	x9, [sp, #128]                  ; 8-byte Reload
	str	w10, [x9, #2352]
	lsr	w8, w10, w8
	cbnz	w8, LBB1_132
LBB1_102:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w26, #0                         ; =0x0
	ldr	x3, [sp, #128]                  ; 8-byte Reload
	mov	w8, #1                          ; =0x1
	strb	w8, [x3, #2356]
LBB1_103:                               ;   in Loop: Header=BB1_21 Depth=1
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	b	LBB1_106
LBB1_104:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	w8, #20036                      ; =0x4e44
	movk	w8, #18757, lsl #16
	cmp	w22, w8
	ldp	x1, x2, [sp, #112]              ; 16-byte Folded Reload
	ldur	x0, [x29, #-112]                ; 8-byte Folded Reload
	ldr	w12, [sp, #92]                  ; 4-byte Reload
	add	x8, sp, #144
	ld1.2d	{ v0, v1, v2, v3 }, [x8]        ; 64-byte Folded Reload
	ldr	x11, [sp, #96]                  ; 8-byte Reload
	ldr	w26, [sp, #84]                  ; 4-byte Reload
	ldr	w10, [sp, #108]                 ; 4-byte Reload
	b.eq	LBB1_133
LBB1_105:                               ;   in Loop: Header=BB1_21 Depth=1
	tbz	w10, #5, LBB1_136
LBB1_106:                               ;   in Loop: Header=BB1_21 Depth=1
	add	w12, w12, #1
	mov	w8, #13                         ; =0xd
	cmp	x11, x1
	b.lo	LBB1_21
	b	LBB1_19
LBB1_107:                               ;   in Loop: Header=BB1_21 Depth=1
	cmp	w8, #16
	b.hs	LBB1_117
; %bb.108:                              ;   in Loop: Header=BB1_21 Depth=1
	mov	x9, #0                          ; =0x0
LBB1_109:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	x12, x9
	and	x9, x8, #0x7ffffff8
	sub	x10, x12, x9
	ldr	x11, [sp, #8]                   ; 8-byte Reload
	add	x11, x11, x12, lsl #3
	ldr	x13, [sp, #56]                  ; 8-byte Reload
	add	x12, x12, x13
	ldr	x13, [sp, #32]                  ; 8-byte Reload
	add	x12, x13, x12
	movi.16b	v2, #1
LBB1_110:                               ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	d0, [x12], #8
	ushll.8h	v0, v0, #0
	mul.8h	v0, v0, v2
	stur	h0, [x11, #-56]
	mov	h1, v0[1]
	stur	h1, [x11, #-48]
	mov	h1, v0[2]
	stur	h1, [x11, #-40]
	mov	h1, v0[3]
	stur	h1, [x11, #-32]
	mov	h1, v0[4]
	stur	h1, [x11, #-24]
	mov	h1, v0[5]
	stur	h1, [x11, #-16]
	mov	h1, v0[6]
	stur	h1, [x11, #-8]
	st1.h	{ v0 }[7], [x11]
	add	x11, x11, #64
	adds	x10, x10, #8
	b.ne	LBB1_110
; %bb.111:                              ;   in Loop: Header=BB1_21 Depth=1
	cmp	x9, x8
	b.ne	LBB1_60
	b	LBB1_102
LBB1_112:                               ;   in Loop: Header=BB1_21 Depth=1
	cmp	w19, #48
	b.hs	LBB1_121
; %bb.113:                              ;   in Loop: Header=BB1_21 Depth=1
	mov	x9, #0                          ; =0x0
LBB1_114:                               ;   in Loop: Header=BB1_21 Depth=1
	mov	x12, x9
	and	x9, x10, #0x7ff8
	sub	x10, x12, x9
	ldp	x11, x13, [sp, #24]             ; 16-byte Folded Reload
	add	x11, x11, x12, lsl #3
	add	x12, x12, x12, lsl #1
	ldr	x14, [sp, #56]                  ; 8-byte Reload
	add	x13, x13, x14
	add	x12, x13, x12
	movi.16b	v16, #1
	add	x13, sp, #144
	ld1.2d	{ v17, v18, v19, v20 }, [x13]   ; 64-byte Folded Reload
LBB1_115:                               ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ld3.8b	{ v0, v1, v2 }, [x12], #24
	ushll.8h	v3, v2, #0
	mul.8h	v5, v3, v16
	ushll.8h	v7, v0, #0
	ushll.8h	v0, v1, #0
	mul.8h	v4, v0, v16
	mul.8h	v3, v7, v16
	mov.16b	v6, v20
	st4.8h	{ v3, v4, v5, v6 }, [x11], #64
	adds	x10, x10, #8
	b.ne	LBB1_115
; %bb.116:                              ;   in Loop: Header=BB1_21 Depth=1
	cmp	x9, x8
	b.ne	LBB1_83
	b	LBB1_85
LBB1_117:                               ;   in Loop: Header=BB1_21 Depth=1
	and	x9, x8, #0x7ffffff0
	ldr	x10, [sp, #128]                 ; 8-byte Reload
	add	x10, x10, #418
	and	x11, x8, #0x7ffffff0
	movi.16b	v3, #1
LBB1_118:                               ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	q0, [x28], #16
	ushll.8h	v1, v0, #0
	mul.8h	v1, v1, v3
	stur	h1, [x10, #-120]
	ushll2.8h	v0, v0, #0
	mov	h2, v1[1]
	stur	h2, [x10, #-112]
	mul.8h	v0, v0, v3
	mov	h2, v1[2]
	stur	h2, [x10, #-104]
	mov	h2, v1[3]
	stur	h2, [x10, #-96]
	mov	h2, v1[4]
	stur	h2, [x10, #-88]
	mov	h2, v1[5]
	stur	h2, [x10, #-80]
	mov	h2, v1[6]
	stur	h2, [x10, #-72]
	mov	h1, v1[7]
	stur	h1, [x10, #-64]
	stur	h0, [x10, #-56]
	mov	h1, v0[1]
	stur	h1, [x10, #-48]
	mov	h1, v0[2]
	stur	h1, [x10, #-40]
	mov	h1, v0[3]
	stur	h1, [x10, #-32]
	mov	h1, v0[4]
	stur	h1, [x10, #-24]
	mov	h1, v0[5]
	stur	h1, [x10, #-16]
	mov	h1, v0[6]
	stur	h1, [x10, #-8]
	st1.h	{ v0 }[7], [x10]
	add	x10, x10, #128
	subs	x11, x11, #16
	b.ne	LBB1_118
; %bb.119:                              ;   in Loop: Header=BB1_21 Depth=1
	cmp	x9, x8
	b.eq	LBB1_102
; %bb.120:                              ;   in Loop: Header=BB1_21 Depth=1
	tbnz	w8, #3, LBB1_109
	b	LBB1_60
LBB1_121:                               ;   in Loop: Header=BB1_21 Depth=1
	and	x9, x10, #0x7ff0
	ldr	x11, [sp, #128]                 ; 8-byte Reload
	add	x12, x11, #292
	and	x11, x10, #0x7ff0
	movi.16b	v18, #1
	add	x13, sp, #144
	ld1.2d	{ v19, v20, v21, v22 }, [x13]   ; 64-byte Folded Reload
LBB1_122:                               ;   Parent Loop BB1_21 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ld3.16b	{ v0, v1, v2 }, [x28], #48
	ushll2.8h	v3, v2, #0
	ushll.8h	v4, v2, #0
	mul.8h	v6, v4, v18
	mul.8h	v21, v3, v18
	ushll.8h	v3, v0, #0
	ushll.8h	v16, v1, #0
	ushll2.8h	v17, v0, #0
	ushll2.8h	v0, v1, #0
	mul.8h	v20, v0, v18
	mul.8h	v19, v17, v18
	mul.8h	v5, v16, v18
	mul.8h	v4, v3, v18
	mov.16b	v7, v22
	add	x13, x12, #128
	st4.8h	{ v4, v5, v6, v7 }, [x12], #64
	st4.8h	{ v19, v20, v21, v22 }, [x12]
	mov	x12, x13
	subs	x11, x11, #16
	b.ne	LBB1_122
; %bb.123:                              ;   in Loop: Header=BB1_21 Depth=1
	add	x11, sp, #144
	st1.2d	{ v19, v20, v21, v22 }, [x11]   ; 64-byte Folded Spill
	cmp	x9, x8
	b.eq	LBB1_85
; %bb.124:                              ;   in Loop: Header=BB1_21 Depth=1
	tbnz	w10, #3, LBB1_114
	b	LBB1_83
LBB1_125:
	mov	w8, #4                          ; =0x4
	b	LBB1_19
LBB1_126:
	mov	w8, #5                          ; =0x5
	b	LBB1_19
LBB1_127:
	mov	w8, #2                          ; =0x2
	b	LBB1_19
LBB1_128:
	mov	w8, #6                          ; =0x6
	b	LBB1_19
LBB1_129:
	mov	w8, #11                         ; =0xb
	b	LBB1_19
LBB1_130:
	mov	x8, x0
	b	LBB1_19
LBB1_131:
	mov	w8, #9                          ; =0x9
	b	LBB1_19
LBB1_132:
	mov	w8, #10                         ; =0xa
	b	LBB1_19
LBB1_133:
	ldr	x8, [sp, #136]                  ; 8-byte Reload
	cbz	w8, LBB1_137
LBB1_134:
	mov	w8, #8                          ; =0x8
	b	LBB1_19
LBB1_135:
	mov	w8, #7                          ; =0x7
	b	LBB1_19
LBB1_136:
	mov	w8, #12                         ; =0xc
	b	LBB1_19
LBB1_137:
	ldr	x8, [sp, #128]                  ; 8-byte Reload
	ldr	x8, [x8, #2376]
	cmp	x8, #0
	cset	w8, ne
	ldr	w9, [sp, #84]                   ; 4-byte Reload
	and	w8, w9, w8
	cmp	w8, #1
	b.ne	LBB1_17
; %bb.138:
	ldr	x8, [sp, #112]                  ; 8-byte Reload
	ldr	x9, [sp, #96]                   ; 8-byte Reload
	cmp	x9, x8
	mov	w8, #14                         ; =0xe
	csel	w8, wzr, w8, eq
	b	LBB1_19
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
lCPI2_0:
	.long	0                               ; 0x0
	.long	15                              ; 0xf
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0
lCPI2_1:
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
lCPI2_2:
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
lCPI2_3:
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
lCPI2_4:
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
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
	.p2align	2
__ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE: ; @_ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	sub	sp, sp, #2672
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
	ldp	x20, x19, [x29, #32]
	ldp	x21, x22, [x29, #16]
	mov	x26, x8
	str	xzr, [x26, #8]!
	lsl	x10, x7, #1
	add	x9, x19, x19, lsl #1
	lsl	x9, x9, #4
	str	xzr, [x8]
	strb	wzr, [x8, #24]
	str	xzr, [x8, #16]
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [x8, #32]
	str	q0, [x8, #64]
	cbz	x1, LBB2_20
; %bb.1:
	subs	x11, x0, x3
	b.ls	LBB2_3
; %bb.2:
	lsr	x11, x11, #4
	cmp	x11, #2387
	b.hs	LBB2_4
	b	LBB2_69
LBB2_3:
	sub	x11, x3, x0
	cmp	x11, x1
	b.lo	LBB2_69
LBB2_4:
	cbz	x5, LBB2_8
; %bb.5:
	subs	x11, x0, x4
	b.ls	LBB2_7
; %bb.6:
	cmp	x11, x5
	b.hs	LBB2_8
	b	LBB2_69
LBB2_7:
	sub	x11, x4, x0
	cmp	x11, x1
	b.lo	LBB2_69
LBB2_8:
	cbz	x10, LBB2_12
; %bb.9:
	subs	x11, x0, x6
	b.ls	LBB2_11
; %bb.10:
	cmp	x11, x10
	b.hs	LBB2_12
	b	LBB2_69
LBB2_11:
	sub	x11, x6, x0
	cmp	x11, x1
	b.lo	LBB2_69
LBB2_12:
	cbz	x22, LBB2_16
; %bb.13:
	subs	x11, x0, x21
	b.ls	LBB2_15
; %bb.14:
	cmp	x11, x22
	b.hs	LBB2_16
	b	LBB2_69
LBB2_15:
	sub	x11, x21, x0
	cmp	x11, x1
	b.lo	LBB2_69
LBB2_16:
	cbz	x9, LBB2_20
; %bb.17:
	subs	x11, x0, x20
	b.ls	LBB2_19
; %bb.18:
	cmp	x11, x9
	b.hs	LBB2_20
	b	LBB2_69
LBB2_19:
	sub	x11, x20, x0
	cmp	x11, x1
	b.lo	LBB2_69
LBB2_20:
	cbz	x5, LBB2_24
; %bb.21:
	subs	x11, x4, x3
	b.hs	LBB2_23
; %bb.22:
	sub	x11, x3, x4
	cmp	x11, x5
	b.hs	LBB2_24
	b	LBB2_69
LBB2_23:
	lsr	x11, x11, #4
	cmp	x11, #2387
	b.lo	LBB2_69
LBB2_24:
	cbz	x10, LBB2_28
; %bb.25:
	subs	x11, x6, x3
	b.hs	LBB2_27
; %bb.26:
	sub	x11, x3, x6
	cmp	x11, x10
	b.hs	LBB2_28
	b	LBB2_69
LBB2_27:
	lsr	x11, x11, #4
	cmp	x11, #2387
	b.lo	LBB2_69
LBB2_28:
	cbz	x22, LBB2_32
; %bb.29:
	subs	x11, x21, x3
	b.hs	LBB2_31
; %bb.30:
	sub	x11, x3, x21
	cmp	x11, x22
	b.hs	LBB2_32
	b	LBB2_69
LBB2_31:
	lsr	x11, x11, #4
	cmp	x11, #2387
	b.lo	LBB2_69
LBB2_32:
	cbz	x9, LBB2_36
; %bb.33:
	subs	x11, x20, x3
	b.hs	LBB2_35
; %bb.34:
	sub	x11, x3, x20
	cmp	x11, x9
	b.hs	LBB2_36
	b	LBB2_69
LBB2_35:
	lsr	x11, x11, #4
	cmp	x11, #2387
	b.lo	LBB2_69
LBB2_36:
	cbz	x5, LBB2_49
; %bb.37:
	cbz	x10, LBB2_41
; %bb.38:
	subs	x11, x4, x6
	b.ls	LBB2_40
; %bb.39:
	cmp	x11, x10
	b.hs	LBB2_41
	b	LBB2_69
LBB2_40:
	sub	x11, x6, x4
	cmp	x11, x5
	b.lo	LBB2_69
LBB2_41:
	cbz	x22, LBB2_45
; %bb.42:
	subs	x11, x4, x21
	b.ls	LBB2_44
; %bb.43:
	cmp	x11, x22
	b.hs	LBB2_45
	b	LBB2_69
LBB2_44:
	sub	x11, x21, x4
	cmp	x11, x5
	b.lo	LBB2_69
LBB2_45:
	cbz	x9, LBB2_49
; %bb.46:
	subs	x11, x4, x20
	b.ls	LBB2_48
; %bb.47:
	cmp	x11, x9
	b.hs	LBB2_49
	b	LBB2_69
LBB2_48:
	sub	x11, x20, x4
	cmp	x11, x5
	b.lo	LBB2_69
LBB2_49:
	cbz	x10, LBB2_58
; %bb.50:
	cbz	x22, LBB2_54
; %bb.51:
	subs	x11, x6, x21
	b.ls	LBB2_53
; %bb.52:
	cmp	x11, x22
	b.hs	LBB2_54
	b	LBB2_69
LBB2_53:
	sub	x11, x21, x6
	cmp	x11, x10
	b.lo	LBB2_69
LBB2_54:
	cbz	x9, LBB2_62
; %bb.55:
	subs	x11, x6, x20
	b.ls	LBB2_57
; %bb.56:
	cmp	x11, x9
	b.hs	LBB2_58
	b	LBB2_69
LBB2_57:
	sub	x11, x20, x6
	cmp	x11, x10
	b.lo	LBB2_69
LBB2_58:
	cbz	x22, LBB2_62
; %bb.59:
	cbz	x9, LBB2_62
; %bb.60:
	subs	x10, x21, x20
	b.ls	LBB2_68
; %bb.61:
	cmp	x10, x9
	b.lo	LBB2_69
LBB2_62:
	stp	x4, x5, [sp, #160]              ; 16-byte Folded Spill
	str	x6, [sp, #176]                  ; 8-byte Spill
	stp	x3, x7, [sp, #200]              ; 16-byte Folded Spill
	movi.2d	v0, #0000000000000000
	strb	wzr, [sp, #296]
	add	x9, sp, #280
	movi.2d	v2, #0000000000000000
	add	x10, sp, #25
	stur	q2, [x10, #255]
	add	x23, sp, #280
	add	x10, sp, #49
	stur	q2, [x10, #255]
	add	x10, sp, #65
	stur	q2, [x10, #255]
	str	xzr, [sp, #336]
	movi.2s	v1, #1
	str	d1, [sp, #344]
	str	xzr, [sp, #352]
	str	q2, [x9, #80]
	str	d1, [sp, #376]
	str	xzr, [sp, #384]
	mov.d	v0[1], v0[0]
	str	q0, [x9, #112]
	str	d1, [sp, #408]
	str	xzr, [sp, #416]
	str	q2, [x9, #144]
	str	d1, [sp, #440]
	str	xzr, [sp, #464]
	stur	q2, [x9, #168]
	str	d1, [sp, #472]
	str	xzr, [sp, #496]
	stur	q2, [x9, #200]
	str	d1, [sp, #504]
	str	xzr, [sp, #528]
	stur	q2, [x9, #232]
	add	x28, x23, #2360
	str	d1, [sp, #536]
	mov	x24, x0
	add	x0, x23, #264
	mov	x25, x1
	mov	w1, #2093                       ; =0x82d
	str	x8, [sp, #216]                  ; 8-byte Spill
	mov	x27, x2
	bl	_bzero
	movi.2d	v0, #0000000000000000
	str	q0, [x28]
	str	xzr, [sp, #2656]
	add	x3, sp, #280
	stp	x24, x25, [sp, #184]            ; 16-byte Folded Spill
	mov	x0, x24
	mov	x1, x25
	str	x27, [sp, #152]                 ; 8-byte Spill
	mov	x2, x27
	bl	__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	ldr	x8, [sp, #216]                  ; 8-byte Reload
	str	w0, [x8]
	cbnz	w0, LBB2_71
; %bb.63:
	add	x9, sp, #25
	ldur	q0, [x9, #255]
	add	x9, sp, #41
	ldur	q1, [x9, #255]
	stp	q0, q1, [x26]
	add	x9, sp, #57
	ldur	q0, [x9, #255]
	str	q0, [x26, #32]
	ldr	x9, [sp, #328]
	ldr	x11, [sp, #312]
	str	x9, [x26, #48]
	ldr	x10, [sp, #168]                 ; 8-byte Reload
	cmp	x10, x11
	ldr	x10, [sp, #208]                 ; 8-byte Reload
	b.lo	LBB2_72
; %bb.64:
	ldr	x11, [sp, #304]
	cmp	x10, x11
	b.lo	LBB2_72
; %bb.65:
	cmp	x19, x9
	b.lo	LBB2_72
; %bb.66:
Lloh0:
	adrp	x8, lCPI2_0@PAGE
Lloh1:
	ldr	d0, [x8, lCPI2_0@PAGEOFF]
	str	d0, [sp, #248]
	ldr	x8, [sp, #2656]
	ldr	x9, [sp, #320]
	stp	x8, x9, [sp, #256]
	ldr	x8, [sp, #152]                  ; 8-byte Reload
	ldr	x8, [x8, #32]
	str	x8, [sp, #272]
	add	x1, sp, #248
	ldr	x0, [sp, #200]                  ; 8-byte Reload
	bl	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	ldr	x8, [sp, #216]                  ; 8-byte Reload
	str	w0, [x8, #4]
	cbz	w0, LBB2_73
; %bb.67:
	mov	w9, #16                         ; =0x10
	b	LBB2_70
LBB2_68:
	sub	x9, x20, x21
	cmp	x9, x22
	b.hs	LBB2_62
LBB2_69:
	mov	w9, #1                          ; =0x1
LBB2_70:
	str	w9, [x8]
LBB2_71:
	add	sp, sp, #2672
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
LBB2_72:
	mov	w9, #15                         ; =0xf
	b	LBB2_70
LBB2_73:
	ldr	x8, [sp, #2640]
	stp	x8, xzr, [sp, #224]
	strb	wzr, [sp, #240]
	add	x0, sp, #280
	bl	__ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE
	ldr	x8, [sp, #216]                  ; 8-byte Reload
	mov	x25, x0
	ldr	w9, [sp, #560]
	ldp	x10, x3, [sp, #192]             ; 16-byte Folded Reload
	ldr	x0, [sp, #184]                  ; 8-byte Reload
	cbz	w9, LBB2_249
; %bb.74:
	mov	x11, #0                         ; =0x0
	add	x12, x23, #56
Lloh2:
	adrp	x9, lCPI2_1@PAGE
Lloh3:
	ldr	q0, [x9, lCPI2_1@PAGEOFF]
	str	q0, [sp, #128]                  ; 16-byte Spill
Lloh4:
	adrp	x9, lCPI2_2@PAGE
Lloh5:
	ldr	q0, [x9, lCPI2_2@PAGEOFF]
	str	q0, [sp, #112]                  ; 16-byte Spill
	ldr	x9, [sp, #168]                  ; 8-byte Reload
	add	x13, x9, #1
Lloh6:
	adrp	x9, lCPI2_3@PAGE
Lloh7:
	ldr	q0, [x9, lCPI2_3@PAGEOFF]
	str	q0, [sp, #96]                   ; 16-byte Spill
Lloh8:
	adrp	x9, lCPI2_4@PAGE
Lloh9:
	ldr	q0, [x9, lCPI2_4@PAGEOFF]
	str	q0, [sp, #80]                   ; 16-byte Spill
	stp	x13, x12, [sp, #56]             ; 16-byte Folded Spill
	b	LBB2_76
LBB2_75:                                ;   in Loop: Header=BB2_76 Depth=1
	ldp	x12, x11, [sp, #64]             ; 16-byte Folded Reload
	add	x11, x11, #1
	ldr	w9, [sp, #560]
	cmp	x11, x9
	ldr	x13, [sp, #56]                  ; 8-byte Reload
	b.hs	LBB2_249
LBB2_76:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_80 Depth 2
                                        ;       Child Loop BB2_169 Depth 3
                                        ;       Child Loop BB2_173 Depth 3
                                        ;       Child Loop BB2_91 Depth 3
                                        ;       Child Loop BB2_190 Depth 3
                                        ;       Child Loop BB2_194 Depth 3
                                        ;       Child Loop BB2_95 Depth 3
                                        ;       Child Loop BB2_176 Depth 3
                                        ;       Child Loop BB2_180 Depth 3
                                        ;       Child Loop BB2_128 Depth 3
                                        ;       Child Loop BB2_197 Depth 3
                                        ;       Child Loop BB2_201 Depth 3
                                        ;       Child Loop BB2_132 Depth 3
                                        ;       Child Loop BB2_183 Depth 3
                                        ;       Child Loop BB2_187 Depth 3
                                        ;       Child Loop BB2_118 Depth 3
                                        ;       Child Loop BB2_125 Depth 3
                                        ;       Child Loop BB2_233 Depth 3
                                        ;       Child Loop BB2_236 Depth 3
                                        ;       Child Loop BB2_166 Depth 3
                                        ;       Child Loop BB2_243 Depth 3
                                        ;       Child Loop BB2_246 Depth 3
                                        ;       Child Loop BB2_156 Depth 3
                                        ;       Child Loop BB2_228 Depth 3
                                        ;       Child Loop BB2_231 Depth 3
                                        ;       Child Loop BB2_105 Depth 3
                                        ;       Child Loop BB2_214 Depth 3
                                        ;       Child Loop BB2_161 Depth 3
                                        ;       Child Loop BB2_238 Depth 3
                                        ;       Child Loop BB2_241 Depth 3
                                        ;       Child Loop BB2_151 Depth 3
                                        ;       Child Loop BB2_206 Depth 3
	str	x11, [sp, #72]                  ; 8-byte Spill
	add	x24, x12, x11, lsl #5
	ldr	x9, [x24, #24]
	add	x26, x9, #1
	cmn	x26, #1
	csinc	x23, x13, x9, eq
	ldr	x9, [sp, #160]                  ; 8-byte Reload
	add	x27, x9, x26
	cmp	x23, #1
	b.lt	LBB2_78
; %bb.77:                               ;   in Loop: Header=BB2_76 Depth=1
	ldr	x8, [sp, #160]                  ; 8-byte Reload
	add	x0, x8, x26
	mov	x1, x23
	bl	_bzero
	ldp	x0, x10, [sp, #184]             ; 16-byte Folded Reload
	ldr	x3, [sp, #200]                  ; 8-byte Reload
	ldr	x8, [sp, #216]                  ; 8-byte Reload
LBB2_78:                                ;   in Loop: Header=BB2_76 Depth=1
	ldr	w9, [x24, #20]
	cbz	w9, LBB2_75
; %bb.79:                               ;   in Loop: Header=BB2_76 Depth=1
	mov	w28, #0                         ; =0x0
	ldr	x9, [sp, #160]                  ; 8-byte Reload
LBB2_80:                                ;   Parent Loop BB2_76 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB2_169 Depth 3
                                        ;       Child Loop BB2_173 Depth 3
                                        ;       Child Loop BB2_91 Depth 3
                                        ;       Child Loop BB2_190 Depth 3
                                        ;       Child Loop BB2_194 Depth 3
                                        ;       Child Loop BB2_95 Depth 3
                                        ;       Child Loop BB2_176 Depth 3
                                        ;       Child Loop BB2_180 Depth 3
                                        ;       Child Loop BB2_128 Depth 3
                                        ;       Child Loop BB2_197 Depth 3
                                        ;       Child Loop BB2_201 Depth 3
                                        ;       Child Loop BB2_132 Depth 3
                                        ;       Child Loop BB2_183 Depth 3
                                        ;       Child Loop BB2_187 Depth 3
                                        ;       Child Loop BB2_118 Depth 3
                                        ;       Child Loop BB2_125 Depth 3
                                        ;       Child Loop BB2_233 Depth 3
                                        ;       Child Loop BB2_236 Depth 3
                                        ;       Child Loop BB2_166 Depth 3
                                        ;       Child Loop BB2_243 Depth 3
                                        ;       Child Loop BB2_246 Depth 3
                                        ;       Child Loop BB2_156 Depth 3
                                        ;       Child Loop BB2_228 Depth 3
                                        ;       Child Loop BB2_231 Depth 3
                                        ;       Child Loop BB2_105 Depth 3
                                        ;       Child Loop BB2_214 Depth 3
                                        ;       Child Loop BB2_161 Depth 3
                                        ;       Child Loop BB2_238 Depth 3
                                        ;       Child Loop BB2_241 Depth 3
                                        ;       Child Loop BB2_151 Depth 3
                                        ;       Child Loop BB2_206 Depth 3
	str	x23, [sp, #168]                 ; 8-byte Spill
	mov	x23, x26
	mov	x26, x9
	str	x8, [sp]
	add	x2, sp, #280
	add	x4, sp, #224
	mov	x1, x10
	mov	x5, x9
	mov	x6, x23
	mov	w7, #0                          ; =0x0
	bl	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	ldr	x11, [sp, #216]                 ; 8-byte Reload
	str	w0, [x11]
	cbnz	w0, LBB2_71
; %bb.81:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x3, x26
	ldrb	w12, [x3], #1
	sub	x8, x23, #1
	ldr	w9, [sp, #568]
	cmp	w12, #1
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b.le	LBB2_96
; %bb.82:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x10, x27, #1
	cmp	w12, #2
	ldp	q28, q27, [sp, #112]            ; 32-byte Folded Reload
	ldp	q30, q29, [sp, #80]             ; 32-byte Folded Reload
	b.eq	LBB2_112
; %bb.83:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w12, #3
	b.eq	LBB2_107
; %bb.84:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w12, #4
	b.ne	LBB2_251
; %bb.85:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	csel	x11, x8, x9, lo
	cbz	x11, LBB2_92
; %bb.86:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, #8
	b.lo	LBB2_89
; %bb.87:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x12, x27, x11
	add	x12, x12, #1
	cmp	x3, x12
	b.hs	LBB2_144
; %bb.88:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x12, x26, x11
	add	x12, x12, #1
	cmp	x10, x12
	b.hs	LBB2_144
LBB2_89:                                ;   in Loop: Header=BB2_80 Depth=2
	mov	x12, #0                         ; =0x0
LBB2_90:                                ;   in Loop: Header=BB2_80 Depth=2
	sub	x13, x11, x12
	add	x14, x26, x12
	add	x14, x14, #1
	add	x12, x27, x12
	add	x12, x12, #1
LBB2_91:                                ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w15, [x12], #1
	ldrb	w16, [x14]
	add	w15, w16, w15
	strb	w15, [x14], #1
	subs	x13, x13, #1
	b.ne	LBB2_91
LBB2_92:                                ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	b.ls	LBB2_133
; %bb.93:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x12, x11
	add	x12, x23, x12
	cmp	x12, #4
	b.hs	LBB2_139
LBB2_94:                                ;   in Loop: Header=BB2_80 Depth=2
	sub	x12, x27, x9
	add	x12, x12, #1
	sub	x9, x26, x9
	add	x9, x9, #1
	mov	x13, x3
LBB2_95:                                ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w14, [x9, x11]
	ldrb	w15, [x10, x11]
	ldrb	w16, [x12, x11]
	add	w17, w15, w14
	sub	w17, w17, w16
	subs	w0, w17, w14
	sub	w1, w17, w15
	sub	w17, w17, w16
	cneg	w0, w0, mi
	cmp	w1, #0
	cneg	w1, w1, mi
	cmp	w17, #0
	cneg	w17, w17, mi
	cmp	w1, w17
	csel	w15, w16, w15, hi
	cmp	w0, w17
	ccmp	w0, w1, #2, ls
	csel	w14, w15, w14, hi
	ldrb	w15, [x13, x11]
	add	w14, w14, w15
	strb	w14, [x13, x11]
	sub	x8, x8, #1
	add	x13, x13, #1
	add	x10, x10, #1
	add	x12, x12, #1
	add	x9, x9, #1
	cmp	x11, x8
	b.ne	LBB2_95
	b	LBB2_133
LBB2_96:                                ;   in Loop: Header=BB2_80 Depth=2
	cbz	w12, LBB2_133
; %bb.97:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w12, #1
	b.ne	LBB2_251
; %bb.98:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #3
	str	x26, [sp, #144]                 ; 8-byte Spill
	b.gt	LBB2_119
; %bb.99:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #1
	b.eq	LBB2_148
; %bb.100:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #2
	b.eq	LBB2_158
; %bb.101:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #3
	b.ne	LBB2_133
; %bb.102:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #4
	b.lo	LBB2_133
; %bb.103:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #19
	b.lo	LBB2_211
; %bb.104:                              ;   in Loop: Header=BB2_80 Depth=2
	str	w28, [sp, #28]                  ; 4-byte Spill
	stp	x27, x25, [sp, #32]             ; 16-byte Folded Spill
	str	x24, [sp, #48]                  ; 8-byte Spill
	sub	x10, x23, #4
	str	x23, [sp, #16]                  ; 8-byte Spill
	sub	x9, x23, #20
	and	x9, x9, #0xfffffffffffffff0
	add	x9, x9, #19
	mov	x11, x26
LBB2_105:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldur	x13, [x11, #4]
	lsr	x2, x13, #24
	lsr	x0, x13, #32
	lsr	x5, x13, #40
	lsr	x4, x13, #48
	lsr	x6, x13, #56
	ldur	x1, [x11, #12]
	ldrb	w15, [x11, #1]
	ldrb	w12, [x11, #2]
	add	x12, x12, x13, lsr #8
	ldrb	w14, [x11, #3]
	add	x14, x14, x13, lsr #16
	lsr	x7, x1, #8
	lsr	x23, x1, #16
	lsr	x24, x1, #24
	lsr	x25, x1, #32
	add	w16, w15, w13
	add	w17, w16, w2
	add	w13, w12, w0
	add	w15, w14, w5
	add	w26, w6, w0
	add	w0, w12, w26
	add	w5, w1, w5
	add	w27, w14, w5
	and	x27, x27, #0xff
	add	w28, w7, w4
	add	w30, w17, w28
	bfi	x27, x30, #8, #8
	add	w6, w23, w6
	add	w6, w13, w6
	bfi	x27, x6, #16, #8
	add	w6, w24, w1
	add	w6, w15, w6
	lsl	w6, w6, #24
	add	w23, w23, w26
	add	x23, x1, x23, lsl #40
	add	w2, w4, w2
	add	w4, w16, w2
	add	w7, w25, w7
	add	w2, w7, w2
	and	x7, x23, #0xff0000000000
	add	w5, w24, w5
	add	x5, x1, x5, lsl #48
	bfi	x7, x2, #32, #8
	add	w2, w28, w25
	add	x1, x1, x2, lsl #56
	orr	x2, x27, x6
	orr	x6, x7, x2
	add	x7, x16, x6, lsr #32
	and	x5, x5, #0xff000000000000
	add	x23, x12, x6, lsr #40
	and	x1, x1, #0xff00000000000000
	orr	x1, x1, x5
	orr	x1, x1, x6
	add	x5, x14, x1, lsr #48
	and	w6, w23, #0xff
	lsl	x6, x6, #40
	and	w7, w7, #0xff
	orr	x6, x6, x7, lsl #32
	and	w5, w5, #0xff
	orr	x5, x6, x5, lsl #48
	add	x1, x17, x1, lsr #56
	orr	x1, x5, x1, lsl #56
	orr	x1, x1, x2
	lsl	w17, w17, #24
	bfi	x17, x14, #16, #8
	bfi	x17, x12, #8, #8
	bfxil	x17, x16, #0, #8
	ubfiz	w12, w4, #16, #8
	orr	w12, w12, w0, lsl #24
	bfi	w12, w15, #8, #8
	bfxil	w12, w13, #0, #8
	orr	x12, x17, x12, lsl #32
	stur	x12, [x11, #4]
	stur	x1, [x11, #12]
	sub	x10, x10, #16
	add	x11, x11, #16
	cmp	x10, #15
	b.hi	LBB2_105
; %bb.106:                              ;   in Loop: Header=BB2_80 Depth=2
	ldp	x25, x24, [sp, #40]             ; 16-byte Folded Reload
	ldr	x27, [sp, #32]                  ; 8-byte Reload
	ldr	w28, [sp, #28]                  ; 4-byte Reload
	ldr	x26, [sp, #144]                 ; 8-byte Reload
	ldr	x23, [sp, #16]                  ; 8-byte Reload
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b	LBB2_212
LBB2_107:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	csel	x11, x8, x9, lo
	cbz	x11, LBB2_129
; %bb.108:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, #8
	b.lo	LBB2_126
; %bb.109:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x12, x26, x11
	add	x12, x12, #1
	add	x13, x27, x11
	add	x13, x13, #1
	cmp	x3, x13
	ccmp	x10, x12, #2, lo
	b.lo	LBB2_126
; %bb.110:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, #64
	b.hs	LBB2_175
; %bb.111:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x10, #0                         ; =0x0
	b	LBB2_179
LBB2_112:                               ;   in Loop: Header=BB2_80 Depth=2
	cbz	x8, LBB2_133
; %bb.113:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #8
	b.lo	LBB2_116
; %bb.114:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x9, x27, x23
	cmp	x3, x9
	b.hs	LBB2_146
; %bb.115:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x9, x26, x23
	cmp	x10, x9
	b.hs	LBB2_146
LBB2_116:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x9, #0                          ; =0x0
LBB2_117:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x9
	add	x8, x8, x23
	add	x10, x27, x9
	add	x10, x10, #1
	add	x9, x26, x9
	add	x9, x9, #1
LBB2_118:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w11, [x9]
	ldrb	w12, [x10], #1
	add	w11, w12, w11
	strb	w11, [x9], #1
	subs	x8, x8, #1
	b.ne	LBB2_118
	b	LBB2_133
LBB2_119:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #4
	b.eq	LBB2_153
; %bb.120:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #6
	b.eq	LBB2_163
; %bb.121:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	w9, #8
	b.ne	LBB2_133
; %bb.122:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #9
	b.lo	LBB2_133
; %bb.123:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #24
	b.lo	LBB2_215
; %bb.124:                              ;   in Loop: Header=BB2_80 Depth=2
	sub	x10, x23, #9
	mov	w9, #8                          ; =0x8
LBB2_125:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	x11, x26, x9
	ldur	x16, [x11, #1]
	ldurb	w12, [x11, #-6]
	add	w17, w12, w16, lsr #8
	ldur	x12, [x11, #9]
	ldurb	w13, [x11, #-5]
	add	w0, w13, w16, lsr #16
	ldurb	w1, [x11, #-7]
	ldurb	w13, [x11, #-4]
	add	w2, w13, w16, lsr #24
	ldurb	w13, [x11, #-3]
	add	x4, x13, x16, lsr #32
	ldurb	w13, [x11, #-2]
	add	x15, x13, x16, lsr #40
	ldurb	w13, [x11, #-1]
	add	x14, x13, x16, lsr #48
	ldrb	w13, [x11]
	add	x13, x13, x16, lsr #56
	and	w30, w14, #0xff
	and	w7, w15, #0xff
	lsl	x7, x7, #40
	orr	x7, x7, x13, lsl #56
	orr	x30, x7, x30, lsl #48
	and	w7, w4, #0xff
	lsl	w2, w2, #24
	ubfiz	x17, x17, #8, #8
	orr	x7, x17, x7, lsl #32
	orr	x30, x30, x7
	ubfiz	x7, x0, #16, #8
	add	w16, w1, w16
	add	w1, w12, w16
	bfxil	x30, x16, #0, #8
	bfi	x30, x0, #16, #8
	orr	x16, x30, x2
	and	x0, x1, #0xff
	and	w1, w12, #0xff00
	add	w17, w17, w1
	and	x17, x17, #0xff00
	and	w1, w12, #0xff0000
	add	w1, w7, w1
	and	x1, x1, #0xff0000
	and	w7, w12, #0xff000000
	add	x4, x4, x12, lsr #32
	and	w4, w4, #0xff
	add	x15, x15, x12, lsr #40
	and	w15, w15, #0xff
	add	x14, x14, x12, lsr #48
	and	w14, w14, #0xff
	add	w2, w2, w7
	orr	x1, x1, x2
	orr	x1, x1, x4, lsl #32
	orr	x15, x1, x15, lsl #40
	orr	x14, x15, x14, lsl #48
	orr	x15, x17, x0
	orr	x14, x14, x15
	add	x12, x13, x12, lsr #56
	orr	x12, x14, x12, lsl #56
	stur	x16, [x11, #1]
	stur	x12, [x11, #9]
	add	x9, x9, #16
	sub	x10, x10, #16
	cmp	x10, #15
	b.hi	LBB2_125
	b	LBB2_216
LBB2_126:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x10, #0                         ; =0x0
LBB2_127:                               ;   in Loop: Header=BB2_80 Depth=2
	sub	x12, x11, x10
	add	x13, x26, x10
	add	x13, x13, #1
	add	x10, x27, x10
	add	x10, x10, #1
LBB2_128:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w14, [x10], #1
	ldrb	w15, [x13]
	add	w14, w15, w14, lsr #1
	strb	w14, [x13], #1
	subs	x12, x12, #1
	b.ne	LBB2_128
LBB2_129:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	b.ls	LBB2_133
; %bb.130:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x11
	add	x8, x23, x8
	cmp	x8, #8
	b.hs	LBB2_135
LBB2_131:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x11
	add	x8, x8, x23
	add	x10, x26, x11
	add	x10, x10, #1
	add	x11, x27, x11
	add	x11, x11, #1
	neg	x9, x9
LBB2_132:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w12, [x10, x9]
	ldrb	w13, [x11], #1
	add	w12, w13, w12
	ldrb	w13, [x10]
	add	w12, w13, w12, lsr #1
	strb	w12, [x10], #1
	subs	x8, x8, #1
	b.ne	LBB2_132
LBB2_133:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x0, sp, #280
	sub	x4, x23, #1
	mov	x1, x24
	mov	x2, x28
	blr	x25
	ldr	x8, [sp, #216]                  ; 8-byte Reload
	str	w0, [x8]
	cbnz	w0, LBB2_71
; %bb.134:                              ;   in Loop: Header=BB2_80 Depth=2
	add	w28, w28, #1
	mov	x11, x26
	ldr	x26, [sp, #168]                 ; 8-byte Reload
	ldr	w10, [x24, #20]
	mov	x9, x27
	mov	x27, x11
	cmp	w28, w10
	ldp	x10, x3, [sp, #192]             ; 16-byte Folded Reload
	ldr	x0, [sp, #184]                  ; 8-byte Reload
	b.lo	LBB2_80
	b	LBB2_75
LBB2_135:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x10, x26, #1
	add	x12, x10, x11
	sub	x14, x11, x9
	add	x13, x10, x14
	add	x15, x26, x23
	sub	x10, x23, x9
	add	x16, x26, x10
	add	x10, x27, x11
	add	x10, x10, #1
	add	x17, x27, x23
	cmp	x12, x17
	ccmp	x10, x15, #2, lo
	cset	w10, lo
	cmp	x13, x15
	ccmp	x12, x16, #2, lo
	b.lo	LBB2_131
; %bb.136:                              ;   in Loop: Header=BB2_80 Depth=2
	tbnz	w10, #0, LBB2_131
; %bb.137:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #64
	b.hs	LBB2_196
; %bb.138:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x10, #0                         ; =0x0
	b	LBB2_200
LBB2_139:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x13, x26, #1
	add	x16, x13, x11
	sub	x14, x11, x9
	add	x17, x13, x14
	add	x0, x26, x23
	sub	x15, x23, x9
	add	x1, x26, x15
	add	x13, x27, #1
	add	x2, x13, x11
	add	x4, x13, x14
	add	x13, x27, x23
	cmp	x16, x13
	ccmp	x2, x0, #2, lo
	cset	w13, lo
	add	x15, x27, x15
	cmp	x16, x15
	ccmp	x4, x0, #2, lo
	cset	w15, lo
	cmp	x17, x0
	ccmp	x16, x1, #2, lo
	b.lo	LBB2_94
; %bb.140:                              ;   in Loop: Header=BB2_80 Depth=2
	tbnz	w13, #0, LBB2_94
; %bb.141:                              ;   in Loop: Header=BB2_80 Depth=2
	tbnz	w15, #0, LBB2_94
; %bb.142:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x12, #16
	b.hs	LBB2_189
; %bb.143:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x13, #0                         ; =0x0
	b	LBB2_193
LBB2_144:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, #64
	b.hs	LBB2_168
; %bb.145:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x12, #0                         ; =0x0
	b	LBB2_172
LBB2_146:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #64
	b.hs	LBB2_182
; %bb.147:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x9, #0                          ; =0x0
	b	LBB2_186
LBB2_148:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #2
	b.lo	LBB2_133
; %bb.149:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #17
	b.lo	LBB2_203
; %bb.150:                              ;   in Loop: Header=BB2_80 Depth=2
	str	w28, [sp, #28]                  ; 4-byte Spill
	stp	x27, x25, [sp, #32]             ; 16-byte Folded Spill
	str	x24, [sp, #48]                  ; 8-byte Spill
	sub	x15, x23, #2
	sub	x9, x23, #18
	and	x9, x9, #0xfffffffffffffff0
	add	x9, x9, #17
	stp	x9, x23, [sp, #8]               ; 16-byte Folded Spill
	mov	x11, x26
LBB2_151:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldur	x12, [x11, #2]
	lsr	x30, x12, #8
	lsr	x9, x12, #16
	lsr	x25, x12, #24
	lsr	x26, x12, #32
	lsr	x23, x12, #40
	lsr	x7, x12, #48
	lsr	x2, x12, #56
	ldur	x14, [x11, #10]
	lsr	x4, x14, #8
	lsr	x1, x14, #16
	lsr	x0, x14, #24
	lsr	x17, x14, #32
	lsr	x16, x14, #40
	lsr	x5, x14, #48
	ldrb	w13, [x11, #1]
	add	w13, w13, w12
	add	w12, w13, w30
	add	w30, w9, w30
	add	w9, w25, w9
	lsl	w9, w9, #24
	lsr	x28, x9, #24
	bfi	x9, x30, #16, #8
	lsr	x9, x9, #16
	add	w25, w26, w25
	add	w30, w9, w25
	add	w26, w23, w26
	add	w24, w28, w26
	add	w23, w7, w23
	add	w25, w23, w25
	add	w7, w2, w7
	add	w26, w7, w26
	add	w2, w14, w2
	add	w23, w2, w23
	add	w27, w4, w14
	add	w7, w27, w7
	add	w4, w1, w4
	add	w2, w4, w2
	add	w1, w0, w1
	add	w27, w1, w27
	add	w0, w17, w0
	add	w4, w0, w4
	add	w10, w16, w17
	add	w1, w10, w1
	add	w16, w5, w16
	add	w0, w16, w0
	add	w17, w13, w30
	add	w16, w13, w9
	add	w9, w16, w25
	add	w30, w30, w23
	add	w30, w13, w30
	and	x30, x30, #0xff
	add	w24, w12, w24
	add	w6, w24, w7
	bfi	x30, x6, #8, #8
	add	w6, w12, w28
	add	w28, w6, w26
	add	w25, w2, w25
	add	w25, w16, w25
	bfi	x30, x25, #16, #8
	add	w25, w27, w26
	add	w25, w6, w25
	lsl	w25, w25, #24
	orr	x25, x30, x25
	add	w4, w4, w23
	add	w4, w17, w4
	and	w4, w4, #0xff
	add	w1, w1, w7
	add	w1, w24, w1
	and	w1, w1, #0xff
	orr	x4, x25, x4, lsl #32
	add	w0, w0, w2
	add	w0, w9, w0
	and	w0, w0, #0xff
	orr	x1, x4, x1, lsl #40
	orr	x0, x1, x0, lsl #48
	add	x14, x5, x14, lsr #56
	add	w10, w14, w10
	add	w10, w10, w27
	add	w10, w28, w10
	lsl	x14, x28, #56
	and	w9, w9, #0xff
	orr	x9, x14, x9, lsl #48
	and	w14, w24, #0xff
	orr	x9, x9, x14, lsl #40
	and	w14, w17, #0xff
	orr	x9, x9, x14, lsl #32
	lsl	w14, w6, #24
	orr	x9, x9, x14
	bfi	x9, x16, #16, #8
	and	x13, x13, #0xff
	bfi	x9, x12, #8, #8
	orr	x10, x0, x10, lsl #56
	orr	x9, x9, x13
	stur	x9, [x11, #2]
	stur	x10, [x11, #10]
	sub	x15, x15, #16
	add	x11, x11, #16
	cmp	x15, #15
	b.hi	LBB2_151
; %bb.152:                              ;   in Loop: Header=BB2_80 Depth=2
	ldp	x25, x24, [sp, #40]             ; 16-byte Folded Reload
	ldr	x27, [sp, #32]                  ; 8-byte Reload
	ldr	w28, [sp, #28]                  ; 4-byte Reload
	ldr	x26, [sp, #144]                 ; 8-byte Reload
	ldp	x9, x23, [sp, #8]               ; 16-byte Folded Reload
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b	LBB2_204
LBB2_153:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #5
	b.lo	LBB2_133
; %bb.154:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #20
	b.lo	LBB2_207
; %bb.155:                              ;   in Loop: Header=BB2_80 Depth=2
	str	x24, [sp, #48]                  ; 8-byte Spill
	mov	x9, #0                          ; =0x0
	mov	x30, x23
	sub	x10, x23, #5
LBB2_156:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	x11, x26, x9
	ldur	x14, [x11, #5]
	lsr	x5, x14, #32
	lsr	x2, x14, #40
	lsr	x0, x14, #48
	lsr	x15, x14, #56
	ldur	x17, [x11, #13]
	ldrb	w1, [x11, #1]
	ldrb	w12, [x11, #2]
	add	x12, x12, x14, lsr #8
	ldrb	w13, [x11, #3]
	add	x13, x13, x14, lsr #16
	ldrb	w16, [x11, #4]
	add	x16, x16, x14, lsr #24
	lsr	x6, x17, #8
	lsr	x7, x17, #16
	lsr	x4, x17, #24
	add	w14, w1, w14
	add	w1, w14, w5
	add	x23, x17, x17, lsl #32
	add	w5, w17, w5
	add	w5, w14, w5
	and	x5, x5, #0xff
	add	w24, w6, w2
	add	w24, w24, w12
	bfi	x5, x24, #8, #8
	add	w24, w7, w0
	add	w24, w13, w24
	bfi	x5, x24, #16, #8
	add	w24, w4, w15
	add	w24, w16, w24
	lsl	w24, w24, #24
	orr	x5, x5, x24
	add	x23, x1, x23, lsr #32
	and	w23, w23, #0xff
	add	x6, x6, x17, lsr #40
	add	w2, w12, w2
	add	w6, w2, w6
	and	w6, w6, #0xff
	orr	x5, x5, x23, lsl #32
	ldr	x26, [sp, #144]                 ; 8-byte Reload
	add	x7, x7, x17, lsr #48
	add	w0, w13, w0
	add	w7, w0, w7
	and	w7, w7, #0xff
	orr	x5, x5, x6, lsl #40
	add	x17, x4, x17, lsr #56
	add	w15, w16, w15
	add	w17, w15, w17
	and	w0, w0, #0xff
	lsl	x0, x0, #48
	orr	x15, x0, x15, lsl #56
	and	w0, w2, #0xff
	orr	x15, x15, x0, lsl #40
	and	w0, w1, #0xff
	orr	x15, x15, x0, lsl #32
	lsl	w16, w16, #24
	orr	x15, x15, x16
	bfi	x15, x13, #16, #8
	and	x13, x14, #0xff
	bfi	x15, x12, #8, #8
	orr	x12, x15, x13
	orr	x13, x5, x7, lsl #48
	stur	x12, [x11, #5]
	orr	x12, x13, x17, lsl #56
	stur	x12, [x11, #13]
	add	x9, x9, #16
	sub	x10, x10, #16
	cmp	x10, #15
	b.hi	LBB2_156
; %bb.157:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x11, x9, #4
	ldr	x24, [sp, #48]                  ; 8-byte Reload
	mov	x23, x30
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b	LBB2_208
LBB2_158:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #3
	b.lo	LBB2_133
; %bb.159:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #18
	b.lo	LBB2_219
; %bb.160:                              ;   in Loop: Header=BB2_80 Depth=2
	str	w28, [sp, #28]                  ; 4-byte Spill
	stp	x27, x25, [sp, #32]             ; 16-byte Folded Spill
	str	x24, [sp, #48]                  ; 8-byte Spill
	sub	x10, x23, #3
	sub	x9, x23, #19
	and	x9, x9, #0xfffffffffffffff0
	add	x9, x9, #18
	stp	x9, x23, [sp, #8]               ; 16-byte Folded Spill
	mov	x11, x26
LBB2_161:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldur	x13, [x11, #3]
	lsr	x1, x13, #16
	lsr	x2, x13, #24
	lsr	x4, x13, #32
	lsr	x5, x13, #40
	lsr	x6, x13, #48
	lsr	x7, x13, #56
	ldur	x15, [x11, #11]
	ldrb	w12, [x11, #2]
	add	x12, x12, x13, lsr #8
	lsr	x23, x15, #8
	lsr	x24, x15, #16
	lsr	x25, x15, #24
	lsr	x0, x15, #32
	lsr	x17, x15, #40
	ldrb	w14, [x11, #1]
	add	w13, w14, w13
	add	w14, w13, w1
	add	w16, w12, w2
	add	w26, w4, w1
	add	w1, w13, w26
	add	w27, w5, w2
	add	w2, w12, w27
	add	w4, w6, w4
	add	w28, w14, w4
	add	w5, w7, w5
	add	w30, w16, w5
	add	w6, w15, w6
	add	w26, w6, w26
	add	w7, w23, w7
	add	w27, w7, w27
	add	w9, w24, w15
	add	w4, w9, w4
	add	w23, w25, w23
	add	w5, w23, w5
	add	w24, w0, w24
	add	w6, w24, w6
	add	w24, w17, w25
	add	w7, w24, w7
	add	w24, w13, w26
	and	x24, x24, #0xff
	add	w25, w27, w12
	bfi	x24, x25, #8, #8
	add	w4, w14, w4
	bfi	x24, x4, #16, #8
	add	w4, w16, w5
	lsl	w4, w4, #24
	orr	x4, x24, x4
	add	w5, w1, w6
	and	w5, w5, #0xff
	add	w6, w2, w7
	and	w6, w6, #0xff
	orr	x4, x4, x5, lsl #32
	add	x0, x0, x15, lsr #48
	add	w9, w0, w9
	add	w9, w28, w9
	and	w9, w9, #0xff
	orr	x0, x4, x6, lsl #40
	orr	x9, x0, x9, lsl #48
	add	x15, x17, x15, lsr #56
	add	w15, w15, w23
	add	w15, w30, w15
	and	w17, w28, #0xff
	lsl	x17, x17, #48
	orr	x17, x17, x30, lsl #56
	and	w0, w2, #0xff
	orr	x17, x17, x0, lsl #40
	and	w0, w1, #0xff
	orr	x17, x17, x0, lsl #32
	lsl	w16, w16, #24
	orr	x16, x17, x16
	bfi	x16, x14, #16, #8
	and	x13, x13, #0xff
	bfi	x16, x12, #8, #8
	orr	x9, x9, x15, lsl #56
	orr	x12, x16, x13
	stur	x12, [x11, #3]
	stur	x9, [x11, #11]
	sub	x10, x10, #16
	add	x11, x11, #16
	cmp	x10, #15
	b.hi	LBB2_161
; %bb.162:                              ;   in Loop: Header=BB2_80 Depth=2
	ldp	x25, x24, [sp, #40]             ; 16-byte Folded Reload
	ldr	x27, [sp, #32]                  ; 8-byte Reload
	ldr	w28, [sp, #28]                  ; 4-byte Reload
	ldr	x26, [sp, #144]                 ; 8-byte Reload
	ldp	x9, x23, [sp, #8]               ; 16-byte Folded Reload
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b	LBB2_220
LBB2_163:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #7
	b.lo	LBB2_133
; %bb.164:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, #22
	b.lo	LBB2_223
; %bb.165:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x9, #0                          ; =0x0
	mov	x30, x23
	sub	x10, x23, #7
LBB2_166:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	x11, x26, x9
	ldur	x17, [x11, #7]
	lsr	x0, x17, #48
	lsr	x1, x17, #56
	ldur	x2, [x11, #15]
	ldrb	w4, [x11, #1]
	ldrb	w12, [x11, #2]
	add	x12, x12, x17, lsr #8
	ldrb	w13, [x11, #3]
	add	x14, x13, x17, lsr #16
	ldrb	w13, [x11, #4]
	add	x16, x13, x17, lsr #24
	ldrb	w13, [x11, #5]
	add	x13, x13, x17, lsr #32
	ldrb	w15, [x11, #6]
	add	x15, x15, x17, lsr #40
	lsr	x5, x2, #8
	add	w17, w4, w17
	add	w4, w17, w0
	add	w6, w12, w1
	add	w7, w14, w2
	and	x7, x7, #0xff
	add	w23, w16, w5
	bfi	x7, x23, #8, #8
	add	w23, w13, w2, lsr #16
	bfi	x7, x23, #16, #8
	add	w23, w15, w2, lsr #24
	lsl	w23, w23, #24
	orr	x7, x7, x23
	add	x0, x2, x0, lsl #32
	and	x0, x0, #0xff00000000
	add	x1, x2, x1, lsl #40
	and	x1, x1, #0xff0000000000
	add	x23, x2, x2, lsl #48
	and	x23, x23, #0xff000000000000
	orr	x0, x0, x1
	orr	x0, x0, x7
	add	x1, x2, x5, lsl #56
	and	x1, x1, #0xff00000000000000
	orr	x1, x1, x23
	ldr	x26, [sp, #144]                 ; 8-byte Reload
	orr	x1, x1, x0
	add	x2, x17, x0, lsr #32
	and	w2, w2, #0xff
	add	x0, x12, x0, lsr #40
	and	w0, w0, #0xff
	lsl	x0, x0, #40
	add	x5, x14, x1, lsr #48
	and	w5, w5, #0xff
	orr	x0, x0, x2, lsl #32
	orr	x0, x0, x5, lsl #48
	add	x1, x16, x1, lsr #56
	orr	x0, x0, x1, lsl #56
	orr	x0, x0, x7
	lsl	w16, w16, #24
	bfi	x16, x14, #16, #8
	bfi	x16, x12, #8, #8
	bfxil	x16, x17, #0, #8
	ubfiz	w12, w4, #16, #8
	orr	w12, w12, w6, lsl #24
	bfi	w12, w15, #8, #8
	bfxil	w12, w13, #0, #8
	orr	x12, x16, x12, lsl #32
	stur	x12, [x11, #7]
	stur	x0, [x11, #15]
	add	x9, x9, #16
	sub	x10, x10, #16
	cmp	x10, #15
	b.hi	LBB2_166
; %bb.167:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x11, x9, #6
	mov	x23, x30
	ldr	x6, [sp, #208]                  ; 8-byte Reload
	ldr	x5, [sp, #176]                  ; 8-byte Reload
	b	LBB2_224
LBB2_168:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x13, x11, #0x38
	and	x12, x11, #0xffffffc0
	add	x14, x27, #33
	add	x15, x26, #33
	and	x16, x11, #0xffffffc0
LBB2_169:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldp	q0, q1, [x14, #-32]
	ldp	q2, q3, [x14], #64
	ldp	q4, q5, [x15, #-32]
	ldp	q6, q7, [x15]
	add.16b	v0, v4, v0
	add.16b	v1, v5, v1
	add.16b	v2, v6, v2
	add.16b	v3, v7, v3
	stp	q0, q1, [x15, #-32]
	stp	q2, q3, [x15], #64
	subs	x16, x16, #64
	b.ne	LBB2_169
; %bb.170:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x12
	b.eq	LBB2_92
; %bb.171:                              ;   in Loop: Header=BB2_80 Depth=2
	cbz	x13, LBB2_90
LBB2_172:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x15, x12
	and	x12, x11, #0xfffffff8
	sub	x13, x15, x12
	add	x14, x26, x15
	add	x14, x14, #1
	add	x15, x27, x15
	add	x15, x15, #1
LBB2_173:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	d0, [x15], #8
	ldr	d1, [x14]
	add.8b	v0, v1, v0
	str	d0, [x14], #8
	adds	x13, x13, #8
	b.ne	LBB2_173
; %bb.174:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x12
	b.eq	LBB2_92
	b	LBB2_90
LBB2_175:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x12, x11, #0x38
	and	x10, x11, #0xffffffc0
	add	x13, x27, #33
	add	x14, x26, #33
	and	x15, x11, #0xffffffc0
LBB2_176:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldp	q0, q1, [x13, #-32]
	ldp	q2, q3, [x13], #64
	ldp	q4, q5, [x14, #-32]
	ldp	q6, q7, [x14]
	usra.16b	v4, v0, #1
	usra.16b	v5, v1, #1
	usra.16b	v6, v2, #1
	usra.16b	v7, v3, #1
	stp	q4, q5, [x14, #-32]
	stp	q6, q7, [x14], #64
	subs	x15, x15, #64
	b.ne	LBB2_176
; %bb.177:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x10
	b.eq	LBB2_129
; %bb.178:                              ;   in Loop: Header=BB2_80 Depth=2
	cbz	x12, LBB2_127
LBB2_179:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x14, x10
	and	x10, x11, #0xfffffff8
	sub	x12, x14, x10
	add	x13, x26, x14
	add	x13, x13, #1
	add	x14, x27, x14
	add	x14, x14, #1
LBB2_180:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	d0, [x14], #8
	ldr	d1, [x13]
	usra.8b	v1, v0, #1
	str	d1, [x13], #8
	adds	x12, x12, #8
	b.ne	LBB2_180
; %bb.181:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x10
	b.eq	LBB2_129
	b	LBB2_127
LBB2_182:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x10, x8, #0x38
	and	x9, x8, #0xffffffffffffffc0
	add	x11, x26, #33
	add	x12, x27, #33
	and	x13, x8, #0xffffffffffffffc0
LBB2_183:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldp	q0, q1, [x11, #-32]
	ldp	q2, q3, [x11]
	ldp	q4, q5, [x12, #-32]
	ldp	q6, q7, [x12], #64
	add.16b	v0, v4, v0
	add.16b	v1, v5, v1
	add.16b	v2, v6, v2
	add.16b	v3, v7, v3
	stp	q0, q1, [x11, #-32]
	stp	q2, q3, [x11], #64
	subs	x13, x13, #64
	b.ne	LBB2_183
; %bb.184:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	b.eq	LBB2_133
; %bb.185:                              ;   in Loop: Header=BB2_80 Depth=2
	cbz	x10, LBB2_117
LBB2_186:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x12, x9
	and	x9, x8, #0xfffffffffffffff8
	sub	x10, x12, x9
	add	x11, x27, x12
	add	x11, x11, #1
	add	x12, x26, x12
	add	x12, x12, #1
LBB2_187:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	d0, [x12]
	ldr	d1, [x11], #8
	add.8b	v0, v1, v0
	str	d0, [x12], #8
	adds	x10, x10, #8
	b.ne	LBB2_187
; %bb.188:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x9
	b.eq	LBB2_133
	b	LBB2_117
LBB2_189:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x15, x12, #0xc
	and	x13, x12, #0xfffffffffffffff0
	add	x16, x26, x11
	add	x16, x16, #1
	add	x17, x27, x11
	add	x17, x17, #1
	neg	x0, x9
	and	x1, x12, #0xfffffffffffffff0
LBB2_190:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	q0, [x16, x0]
	tbl.16b	v1, { v0 }, v27
	tbl.16b	v2, { v0 }, v28
	tbl.16b	v3, { v0 }, v29
	tbl.16b	v4, { v0 }, v30
	ldr	q5, [x17]
	tbl.16b	v6, { v5 }, v27
	tbl.16b	v7, { v5 }, v28
	tbl.16b	v16, { v5 }, v29
	tbl.16b	v17, { v5 }, v30
	ldr	q18, [x17, x0]
	tbl.16b	v19, { v18 }, v30
	tbl.16b	v20, { v18 }, v29
	tbl.16b	v21, { v18 }, v28
	tbl.16b	v22, { v18 }, v27
	add.4s	v23, v17, v4
	add.4s	v24, v16, v3
	add.4s	v25, v7, v2
	add.4s	v26, v6, v1
	sub.4s	v26, v26, v22
	sub.4s	v25, v25, v21
	sub.4s	v24, v24, v20
	sub.4s	v23, v23, v19
	sub.4s	v4, v23, v4
	sub.4s	v3, v24, v3
	sub.4s	v2, v25, v2
	sub.4s	v1, v26, v1
	sub.4s	v19, v23, v19
	sub.4s	v20, v24, v20
	sub.4s	v21, v25, v21
	sub.4s	v22, v26, v22
	abs.4s	v1, v1
	abs.4s	v2, v2
	abs.4s	v3, v3
	abs.4s	v4, v4
	sabd.4s	v6, v26, v6
	sabd.4s	v7, v25, v7
	sabd.4s	v16, v24, v16
	sabd.4s	v17, v23, v17
	abs.4s	v22, v22
	abs.4s	v21, v21
	abs.4s	v20, v20
	abs.4s	v19, v19
	umin.4s	v23, v17, v19
	cmhi.4s	v4, v4, v23
	umin.4s	v23, v16, v20
	cmhi.4s	v3, v3, v23
	uzp1.8h	v3, v3, v4
	umin.4s	v4, v7, v21
	cmhi.4s	v2, v2, v4
	umin.4s	v4, v6, v22
	cmhi.4s	v1, v1, v4
	uzp1.8h	v1, v1, v2
	uzp1.16b	v1, v1, v3
	cmhi.4s	v2, v17, v19
	cmhi.4s	v3, v16, v20
	uzp1.8h	v2, v3, v2
	cmhi.4s	v3, v7, v21
	cmhi.4s	v4, v6, v22
	uzp1.8h	v3, v4, v3
	uzp1.16b	v2, v3, v2
	bsl.16b	v2, v18, v5
	bit.16b	v0, v2, v1
	ldr	q1, [x16]
	add.16b	v0, v0, v1
	str	q0, [x16], #16
	add	x17, x17, #16
	subs	x1, x1, #16
	b.ne	LBB2_190
; %bb.191:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x12, x13
	b.eq	LBB2_133
; %bb.192:                              ;   in Loop: Header=BB2_80 Depth=2
	cbz	x15, LBB2_247
LBB2_193:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x15, x12, #0xfffffffffffffffc
	add	x16, x11, x15
	add	x1, x26, #1
	add	x17, x1, x11
	add	x0, x27, #1
	add	x11, x0, x11
	add	x0, x0, x14
	add	x14, x1, x14
	and	x1, x12, #0xfffffffffffffffc
LBB2_194:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	s0, [x14, x13]
	ushll.8h	v0, v0, #0
	ldr	s1, [x11, x13]
	ushll.8h	v1, v1, #0
	ushll.4s	v2, v1, #0
	ldr	s3, [x0, x13]
	ushll.8h	v3, v3, #0
	uaddl.4s	v4, v1, v0
	usubw.4s	v4, v4, v3
	usubw.4s	v5, v4, v0
	usubw.4s	v6, v4, v3
	abs.4s	v5, v5
	sabd.4s	v2, v4, v2
	abs.4s	v4, v6
	umin.4s	v6, v2, v4
	cmhi.4s	v5, v5, v6
	xtn.4h	v5, v5
	cmhi.4s	v2, v2, v4
	xtn.4h	v2, v2
	bit.8b	v1, v3, v2
	bit.8b	v0, v1, v5
	ldr	s1, [x17, x13]
	uaddw.8h	v0, v0, v1
	uzp1.8b	v0, v0, v0
	str	s0, [x17, x13]
	sub	x1, x1, #4
	add	x17, x17, #4
	add	x11, x11, #4
	add	x0, x0, #4
	add	x14, x14, #4
	cmp	x13, x1
	b.ne	LBB2_194
; %bb.195:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x11, x16
	cmp	x12, x15
	b.ne	LBB2_94
	b	LBB2_133
LBB2_196:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	x13, #0                         ; =0x0
	and	x12, x8, #0x38
	and	x10, x8, #0xffffffffffffffc0
	add	x14, x26, x14
	add	x15, x26, x11
	add	x16, x27, x11
	add	x16, x16, #33
LBB2_197:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	x17, x14, x13
	ldur	q0, [x17, #1]
	ldur	q1, [x17, #17]
	ldur	q2, [x17, #33]
	ldur	q3, [x17, #49]
	add	x17, x16, x13
	ldp	q4, q5, [x17, #-32]
	ldp	q6, q7, [x17]
	uhadd.16b	v0, v4, v0
	uhadd.16b	v1, v5, v1
	uhadd.16b	v2, v6, v2
	uhadd.16b	v3, v7, v3
	add	x17, x15, x13
	ldur	q4, [x17, #1]
	ldur	q5, [x17, #17]
	ldur	q6, [x17, #33]
	ldur	q7, [x17, #49]
	add.16b	v0, v4, v0
	add.16b	v1, v5, v1
	add.16b	v2, v6, v2
	add.16b	v3, v7, v3
	stur	q0, [x17, #1]
	stur	q1, [x17, #17]
	stur	q2, [x17, #33]
	stur	q3, [x17, #49]
	add	x13, x13, #64
	cmp	x10, x13
	b.ne	LBB2_197
; %bb.198:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x8, x10
	b.eq	LBB2_133
; %bb.199:                              ;   in Loop: Header=BB2_80 Depth=2
	cbz	x12, LBB2_248
LBB2_200:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x12, x8, #0xfffffffffffffff8
	add	x13, x11, x12
	sub	x14, x10, x12
	add	x11, x10, x11
	add	x10, x26, x11
	add	x10, x10, #1
	add	x11, x27, x11
	add	x11, x11, #1
	neg	x15, x9
LBB2_201:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	d0, [x10, x15]
	ldr	d1, [x11], #8
	uhadd.8b	v0, v1, v0
	ldr	d1, [x10]
	add.8b	v0, v1, v0
	str	d0, [x10], #8
	adds	x14, x14, #8
	b.ne	LBB2_201
; %bb.202:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x11, x13
	cmp	x8, x12
	b.ne	LBB2_131
	b	LBB2_133
LBB2_203:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w9, #1                          ; =0x1
LBB2_204:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x8
	b.hs	LBB2_133
; %bb.205:                              ;   in Loop: Header=BB2_80 Depth=2
	add	x8, x26, x9
	ldrb	w10, [x8], #1
	mvn	x9, x9
	add	x9, x9, x23
LBB2_206:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w11, [x8]
	add	w10, w10, w11
	strb	w10, [x8], #1
	subs	x9, x9, #1
	b.ne	LBB2_206
	b	LBB2_133
LBB2_207:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w11, #4                         ; =0x4
LBB2_208:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x8
	b.hs	LBB2_133
; %bb.209:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x11
	add	x9, x23, x8
	cmp	x9, #4
	b.hs	LBB2_227
; %bb.210:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x8, x11
	b	LBB2_230
LBB2_211:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w9, #3                          ; =0x3
LBB2_212:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x8
	b.hs	LBB2_133
; %bb.213:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x9
	add	x8, x8, x23
	add	x9, x26, x9
	add	x9, x9, #1
LBB2_214:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w10, [x9]
	ldurb	w11, [x9, #-3]
	add	w10, w11, w10
	strb	w10, [x9], #1
	subs	x8, x8, #1
	b.ne	LBB2_214
	b	LBB2_133
LBB2_215:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w9, #8                          ; =0x8
LBB2_216:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x8
	b.hs	LBB2_133
; %bb.217:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x9
	add	x10, x23, x8
	cmp	x10, #8
	b.hs	LBB2_232
; %bb.218:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x8, x9
	b	LBB2_235
LBB2_219:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w9, #2                          ; =0x2
LBB2_220:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x8
	b.hs	LBB2_133
; %bb.221:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x9
	add	x10, x23, x8
	cmp	x10, #2
	b.hs	LBB2_237
; %bb.222:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x8, x9
	b	LBB2_240
LBB2_223:                               ;   in Loop: Header=BB2_80 Depth=2
	mov	w11, #6                         ; =0x6
LBB2_224:                               ;   in Loop: Header=BB2_80 Depth=2
	cmp	x11, x8
	b.hs	LBB2_133
; %bb.225:                              ;   in Loop: Header=BB2_80 Depth=2
	mvn	x8, x11
	add	x9, x23, x8
	cmp	x9, #2
	b.hs	LBB2_242
; %bb.226:                              ;   in Loop: Header=BB2_80 Depth=2
	mov	x8, x11
	b	LBB2_245
LBB2_227:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x10, x9, #0xfffffffffffffffc
	add	x8, x11, x10
	add	x12, x26, x11
	ldur	s0, [x12, #-3]
	ushll.8h	v0, v0, #0
	add	x11, x3, x11
	and	x12, x9, #0xfffffffffffffffc
LBB2_228:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	s1, [x11]
	uaddw.8h	v0, v0, v1
	uzp1.8b	v1, v0, v0
	st1.s	{ v1 }[0], [x11], #4
	subs	x12, x12, #4
	b.ne	LBB2_228
; %bb.229:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x10
	b.eq	LBB2_133
LBB2_230:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x9, x8
	add	x9, x9, x23
	add	x8, x26, x8
	add	x8, x8, #1
LBB2_231:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w10, [x8]
	ldurb	w11, [x8, #-4]
	add	w10, w11, w10
	strb	w10, [x8], #1
	subs	x9, x9, #1
	b.ne	LBB2_231
	b	LBB2_133
LBB2_232:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x11, x10, #0xfffffffffffffff8
	add	x8, x9, x11
	add	x12, x26, x9
	ldur	d0, [x12, #-7]
	add	x9, x3, x9
	and	x12, x10, #0xfffffffffffffff8
LBB2_233:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	d1, [x9]
	add.8b	v0, v0, v1
	str	d0, [x9], #8
	subs	x12, x12, #8
	b.ne	LBB2_233
; %bb.234:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x10, x11
	b.eq	LBB2_133
LBB2_235:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x9, x8
	add	x9, x9, x23
	add	x8, x26, x8
	add	x8, x8, #1
LBB2_236:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w10, [x8]
	ldurb	w11, [x8, #-8]
	add	w10, w11, w10
	strb	w10, [x8], #1
	subs	x9, x9, #1
	b.ne	LBB2_236
	b	LBB2_133
LBB2_237:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x11, x10, #0xfffffffffffffffe
	add	x8, x9, x11
	add	x12, x26, x9
	ldur	h0, [x12, #-1]
	ushll.8h	v0, v0, #0
	ushll.4s	v0, v0, #0
	add	x9, x3, x9
	and	x12, x10, #0xfffffffffffffffe
LBB2_238:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	h1, [x9]
	ushll.8h	v1, v1, #0
	uaddw.4s	v0, v0, v1
	str	b0, [x9]
	mov	s1, v0[1]
	stur	b1, [x9, #1]
	add	x9, x9, #2
	subs	x12, x12, #2
	b.ne	LBB2_238
; %bb.239:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x10, x11
	b.eq	LBB2_133
LBB2_240:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x9, x8
	add	x9, x9, x23
	add	x8, x26, x8
	add	x8, x8, #1
LBB2_241:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w10, [x8]
	ldurb	w11, [x8, #-2]
	add	w10, w11, w10
	strb	w10, [x8], #1
	subs	x9, x9, #1
	b.ne	LBB2_241
	b	LBB2_133
LBB2_242:                               ;   in Loop: Header=BB2_80 Depth=2
	and	x10, x9, #0xfffffffffffffffe
	add	x8, x11, x10
	add	x11, x3, x11
	and	x12, x9, #0xfffffffffffffffe
LBB2_243:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	h0, [x11]
	ushll.8h	v0, v0, #0
	ldur	h1, [x11, #-6]
	ushll.8h	v1, v1, #0
	uaddl.4s	v0, v1, v0
	str	b0, [x11]
	mov	s0, v0[1]
	stur	b0, [x11, #1]
	add	x11, x11, #2
	subs	x12, x12, #2
	b.ne	LBB2_243
; %bb.244:                              ;   in Loop: Header=BB2_80 Depth=2
	cmp	x9, x10
	b.eq	LBB2_133
LBB2_245:                               ;   in Loop: Header=BB2_80 Depth=2
	mvn	x9, x8
	add	x9, x9, x23
	add	x8, x26, x8
	add	x8, x8, #1
LBB2_246:                               ;   Parent Loop BB2_76 Depth=1
                                        ;     Parent Loop BB2_80 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w10, [x8]
	ldurb	w11, [x8, #-6]
	add	w10, w11, w10
	strb	w10, [x8], #1
	subs	x9, x9, #1
	b.ne	LBB2_246
	b	LBB2_133
LBB2_247:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x11, x11, x13
	b	LBB2_94
LBB2_248:                               ;   in Loop: Header=BB2_80 Depth=2
	add	x11, x11, x10
	b	LBB2_131
LBB2_249:
	str	x8, [sp]
	add	x2, sp, #280
	add	x4, sp, #224
	mov	x1, x10
	mov	x5, #0                          ; =0x0
	mov	x6, #0                          ; =0x0
	mov	w7, #1                          ; =0x1
	bl	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	ldr	x8, [sp, #216]                  ; 8-byte Reload
	str	w0, [x8]
	cbnz	w0, LBB2_71
; %bb.250:
	str	x8, [sp]
	ldp	x0, x1, [sp, #184]              ; 16-byte Folded Reload
	ldr	x2, [sp, #152]                  ; 8-byte Reload
	ldr	x3, [sp, #200]                  ; 8-byte Reload
	mov	x4, x21
	mov	x5, x22
	mov	x6, x20
	mov	x7, x19
	bl	__ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
	ldr	x11, [sp, #216]                 ; 8-byte Reload
	b	LBB2_252
LBB2_251:
	mov	w0, #18                         ; =0x12
LBB2_252:
	str	w0, [x11]
	b	LBB2_71
	.loh AdrpLdr	Lloh0, Lloh1
	.loh AdrpLdr	Lloh8, Lloh9
	.loh AdrpAdrp	Lloh6, Lloh8
	.loh AdrpLdr	Lloh6, Lloh7
	.loh AdrpLdr	Lloh4, Lloh5
	.loh AdrpAdrp	Lloh2, Lloh4
	.loh AdrpLdr	Lloh2, Lloh3
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE
__ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE: ; @_ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE
	.cfi_startproc
; %bb.0:
	mov	x8, x0
	mov	x0, #0                          ; =0x0
	ldr	w9, [x8, #12]
	cmp	w9, #2
	b.le	LBB3_5
; %bb.1:
	cmp	w9, #3
	b.eq	LBB3_8
; %bb.2:
	cmp	w9, #4
	b.eq	LBB3_11
; %bb.3:
	cmp	w9, #6
	b.ne	LBB3_20
; %bb.4:
	ldr	w8, [x8, #8]
Lloh10:
	adrp	x9, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh11:
	add	x9, x9, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #16
	csel	x9, x9, xzr, eq
Lloh12:
	adrp	x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh13:
	add	x10, x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #8
	csel	x0, x10, x9, eq
	ret
LBB3_5:
	cbz	w9, LBB3_12
; %bb.6:
	cmp	w9, #2
	b.ne	LBB3_20
; %bb.7:
	ldrb	w9, [x8, #2356]
	ldr	w8, [x8, #8]
Lloh14:
	adrp	x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh15:
	add	x10, x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #16
	csel	x10, x10, xzr, eq
Lloh16:
	adrp	x11, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh17:
	add	x11, x11, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #8
	csel	x10, x11, x10, eq
Lloh18:
	adrp	x11, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh19:
	add	x11, x11, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #16
	csel	x11, x11, xzr, eq
Lloh20:
	adrp	x12, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh21:
	add	x12, x12, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #8
	csel	x8, x12, x11, eq
	cmp	w9, #1
	csel	x0, x10, x8, ne
	ret
LBB3_8:
	ldr	w8, [x8, #8]
	sub	w9, w8, #1
	eor	w10, w8, w9
	cmp	w10, w9
	b.ls	LBB3_21
; %bb.9:
	rbit	w8, w8
	clz	w8, w8
	cmp	w8, #4
	b.hs	LBB3_21
; %bb.10:
Lloh22:
	adrp	x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.2@PAGE
Lloh23:
	add	x9, x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.2@PAGEOFF
	b	LBB3_19
LBB3_11:
	ldr	w8, [x8, #8]
Lloh24:
	adrp	x9, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh25:
	add	x9, x9, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #16
	csel	x9, x9, xzr, eq
Lloh26:
	adrp	x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGE
Lloh27:
	add	x10, x10, __ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE@PAGEOFF
	cmp	w8, #8
	csel	x0, x10, x9, eq
	ret
LBB3_12:
	ldrb	w9, [x8, #2356]
	ldr	w8, [x8, #8]
	cmp	w9, #1
	b.ne	LBB3_16
; %bb.13:
	sub	w9, w8, #1
	eor	w10, w8, w9
	cmp	w10, w9
	b.ls	LBB3_21
; %bb.14:
	rbit	w8, w8
	clz	w8, w8
	cmp	w8, #5
	b.hs	LBB3_21
; %bb.15:
Lloh28:
	adrp	x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE@PAGE
Lloh29:
	add	x9, x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE@PAGEOFF
	b	LBB3_19
LBB3_16:
	sub	w9, w8, #1
	eor	w10, w8, w9
	cmp	w10, w9
	b.ls	LBB3_21
; %bb.17:
	rbit	w8, w8
	clz	w8, w8
	cmp	w8, #5
	b.hs	LBB3_21
; %bb.18:
Lloh30:
	adrp	x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.1@PAGE
Lloh31:
	add	x9, x9, l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.1@PAGEOFF
LBB3_19:
	ldr	x0, [x9, w8, uxtw #3]
LBB3_20:
	ret
LBB3_21:
	mov	x0, #0                          ; =0x0
	ret
	.loh AdrpAdd	Lloh12, Lloh13
	.loh AdrpAdd	Lloh10, Lloh11
	.loh AdrpAdd	Lloh20, Lloh21
	.loh AdrpAdd	Lloh18, Lloh19
	.loh AdrpAdd	Lloh16, Lloh17
	.loh AdrpAdd	Lloh14, Lloh15
	.loh AdrpAdd	Lloh22, Lloh23
	.loh AdrpAdd	Lloh26, Lloh27
	.loh AdrpAdd	Lloh24, Lloh25
	.loh AdrpAdd	Lloh28, Lloh29
	.loh AdrpAdd	Lloh30, Lloh31
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE: ; @_ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #160
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #80]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #96]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
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
	mov	x20, x7
	mov	x19, x6
	str	x1, [sp]                        ; 8-byte Spill
	ldrb	w8, [x4, #16]
	tbz	w8, #0, LBB4_5
; %bb.1:
	mov	x27, #0                         ; =0x0
LBB4_2:
	tbnz	w20, #0, LBB4_4
LBB4_3:
	cmp	x27, x19
	b.ne	LBB4_26
LBB4_4:
	mov	w0, #0                          ; =0x0
	b	LBB4_27
LBB4_5:
	mov	x23, x5
	mov	x22, x4
	mov	x24, x3
	mov	x21, x2
	mov	x26, x0
	mov	x27, #0                         ; =0x0
	ldr	x28, [x29, #16]
	add	x25, x0, #15
	b	LBB4_7
LBB4_6:                                 ;   in Loop: Header=BB4_7 Depth=1
	ldrb	w8, [x22, #16]
	cmp	w8, #1
	b.eq	LBB4_2
LBB4_7:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB4_12 Depth 2
	tbnz	w20, #0, LBB4_9
; %bb.8:                                ;   in Loop: Header=BB4_7 Depth=1
	cmp	x19, x27
	b.ls	LBB4_3
LBB4_9:                                 ;   in Loop: Header=BB4_7 Depth=1
	ldr	x8, [x21, #2368]
	ldr	x9, [x22]
	cmp	x9, x8
	b.hs	LBB4_14
; %bb.10:                               ;   in Loop: Header=BB4_7 Depth=1
	ldr	x11, [x22, #8]
	ldr	w10, [x26, x9]
	rev	w10, w10
	cmp	x11, x10
	b.lo	LBB4_14
; %bb.11:                               ;   in Loop: Header=BB4_7 Depth=1
	add	x9, x9, x10
LBB4_12:                                ;   Parent Loop BB4_7 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x9, x9, #12
	stp	x9, xzr, [x22]
	cmp	x9, x8
	b.hs	LBB4_14
; %bb.13:                               ;   in Loop: Header=BB4_12 Depth=2
	add	x10, x25, x9
	ldur	w10, [x10, #-15]
	rev	w10, w10
	cbz	w10, LBB4_12
LBB4_14:                                ;   in Loop: Header=BB4_7 Depth=1
	cmp	x9, x8
	b.hs	LBB4_16
; %bb.15:                               ;   in Loop: Header=BB4_7 Depth=1
	ldr	w10, [x26, x9]
	rev	w10, w10
	ldr	x11, [x22, #8]
	add	x12, x9, x11
	add	x9, x9, x10
	add	x9, x9, #12
	add	x12, x12, #8
	sub	x10, x10, x11
	ldr	x11, [sp]                       ; 8-byte Reload
	sub	x11, x11, x12
	cmn	x10, #1
	csel	x2, x11, x10, eq
	add	x1, x26, x12
	cmp	x9, x8
	cset	w5, eq
	b	LBB4_17
LBB4_16:                                ;   in Loop: Header=BB4_7 Depth=1
	mov	x2, #0                          ; =0x0
	mov	x1, #0                          ; =0x0
	mov	w5, #1                          ; =0x1
LBB4_17:                                ;   in Loop: Header=BB4_7 Depth=1
	add	x8, sp, #8
	add	x3, x27, x23
	sub	x4, x19, x27
	mov	x0, x24
	bl	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	ldr	x10, [x22, #8]
	ldp	x8, x9, [sp, #16]
	add	x10, x10, x8
	str	x10, [x22, #8]
	ldr	x10, [sp, #48]
	str	x10, [x28, #72]
	ldp	w10, w11, [sp, #8]
	str	w11, [x28, #4]
	cmp	w10, #3
	b.eq	LBB4_28
; %bb.18:                               ;   in Loop: Header=BB4_7 Depth=1
	add	x27, x9, x27
	cmp	w10, #2
	b.eq	LBB4_24
; %bb.19:                               ;   in Loop: Header=BB4_7 Depth=1
	cbz	w20, LBB4_21
; %bb.20:                               ;   in Loop: Header=BB4_7 Depth=1
	cbnz	x9, LBB4_26
LBB4_21:                                ;   in Loop: Header=BB4_7 Depth=1
	cbnz	x8, LBB4_6
; %bb.22:                               ;   in Loop: Header=BB4_7 Depth=1
	cbnz	x9, LBB4_6
; %bb.23:                               ;   in Loop: Header=BB4_7 Depth=1
	cbz	w10, LBB4_6
	b	LBB4_26
LBB4_24:
	mov	w8, #1                          ; =0x1
	strb	w8, [x22, #16]
	ldr	x8, [sp, #32]
	ldr	x9, [x21, #2376]
	cmp	x8, x9
	b.ne	LBB4_29
; %bb.25:
	ldr	x8, [sp, #40]
	ldr	x9, [x21, #40]
	cmp	x8, x9
	b.eq	LBB4_2
LBB4_26:
	mov	w0, #17                         ; =0x11
LBB4_27:
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #96]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
LBB4_28:
	mov	w0, #16                         ; =0x10
	b	LBB4_27
LBB4_29:
	mov	w0, #14                         ; =0xe
	b	LBB4_27
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
lCPI5_0:
	.long	0                               ; 0x0
	.long	15                              ; 0xf
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE: ; @_ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #304
	stp	d9, d8, [sp, #192]              ; 16-byte Folded Spill
	stp	x28, x27, [sp, #208]            ; 16-byte Folded Spill
	stp	x26, x25, [sp, #224]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #240]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #256]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #272]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #288]            ; 16-byte Folded Spill
	add	x29, sp, #288
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
	stp	x4, x5, [sp, #16]               ; 16-byte Folded Spill
	str	x2, [sp, #32]                   ; 8-byte Spill
	cmp	x1, #9
	b.lo	LBB5_11
; %bb.1:
	mov	x22, x6
	mov	x23, x1
	mov	x24, x0
	str	x3, [sp, #8]                    ; 8-byte Spill
	mov	x20, #0                         ; =0x0
	ldr	x21, [x29, #16]
	add	x25, sp, #136
	mov	w19, #8                         ; =0x8
Lloh32:
	adrp	x8, lCPI5_0@PAGE
Lloh33:
	ldr	d8, [x8, lCPI5_0@PAGEOFF]
	mov	w27, #48                        ; =0x30
	str	x0, [sp]                        ; 8-byte Spill
	b	LBB5_4
LBB5_2:                                 ;   in Loop: Header=BB5_4 Depth=1
	madd	x8, x20, x27, x22
	ldur	q0, [sp, #136]
	ldur	q1, [sp, #152]
	stp	q0, q1, [x8]
	ldur	q0, [sp, #168]
	str	q0, [x8, #32]
	add	x20, x20, #1
LBB5_3:                                 ;   in Loop: Header=BB5_4 Depth=1
	add	x8, x19, x26
	add	x19, x8, #12
	cmp	x19, x23
	b.hs	LBB5_11
LBB5_4:                                 ; =>This Inner Loop Header: Depth=1
	add	x8, x24, x19
	ldrb	w9, [x8, #4]
	ldr	w10, [x8]
	rev	w26, w10
	tbz	w9, #5, LBB5_3
; %bb.5:                                ;   in Loop: Header=BB5_4 Depth=1
	lsl	w9, w9, #24
	ldrb	w10, [x8, #5]
	ldrb	w11, [x8, #6]
	ldrb	w8, [x8, #7]
	orr	w9, w9, w10, lsl #16
	orr	w8, w9, w8
	orr	w0, w8, w11, lsl #8
	stp	xzr, xzr, [x25, #32]
	add	x8, x19, #8
	str	w0, [sp, #136]
	stp	x8, x26, [sp, #144]
	add	x28, x24, x8
	add	x3, x25, #24
	add	x4, sp, #128
	mov	x1, x28
	mov	x2, x26
	bl	__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	cbnz	w0, LBB5_12
; %bb.6:                                ;   in Loop: Header=BB5_4 Depth=1
	ldrb	w8, [sp, #160]
	cmp	w8, #1
	b.ne	LBB5_2
; %bb.7:                                ;   in Loop: Header=BB5_4 Depth=1
	mov	x25, x20
	mov	x20, x23
	mov	x23, x22
	str	d8, [sp, #96]
	ldr	x22, [sp, #128]
	sub	x27, x26, x22
	str	x27, [sp, #104]
	ldr	x8, [sp, #32]                   ; 8-byte Reload
	ldur	q0, [x8, #24]
	ldr	q1, [x21, #64]
	sub.2d	v0, v0, v1
	str	q0, [sp, #112]
	add	x1, sp, #96
	ldr	x24, [sp, #8]                   ; 8-byte Reload
	mov	x0, x24
	bl	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	str	w0, [x21, #4]
	cbnz	w0, LBB5_14
; %bb.8:                                ;   in Loop: Header=BB5_4 Depth=1
	ldr	x9, [x21, #64]
	ldp	x10, x8, [sp, #16]              ; 16-byte Folded Reload
	sub	x4, x8, x9
	add	x8, sp, #40
	add	x1, x28, x22
	add	x3, x9, x10
	mov	x0, x24
	mov	x2, x27
	mov	w5, #1                          ; =0x1
	bl	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	ldp	w8, w9, [sp, #40]
	str	w9, [x21, #4]
	ldr	x9, [sp, #80]
	ldr	x10, [x21, #72]
	add	x9, x10, x9
	str	x9, [x21, #72]
	cmp	w8, #2
	b.ne	LBB5_13
; %bb.9:                                ;   in Loop: Header=BB5_4 Depth=1
	ldr	x8, [sp, #48]
	cmp	x8, x27
	b.ne	LBB5_16
; %bb.10:                               ;   in Loop: Header=BB5_4 Depth=1
	ldr	x8, [x21, #64]
	ldr	x9, [sp, #56]
	stp	x8, x9, [sp, #168]
	add	x8, x9, x8
	str	x8, [x21, #64]
	mov	x22, x23
	mov	x23, x20
	mov	x20, x25
	ldr	x24, [sp]                       ; 8-byte Reload
	add	x25, sp, #136
	mov	w27, #48                        ; =0x30
	b	LBB5_2
LBB5_11:
	mov	w0, #0                          ; =0x0
LBB5_12:
	ldp	x29, x30, [sp, #288]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #272]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #256]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #240]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #224]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #208]            ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #192]              ; 16-byte Folded Reload
	add	sp, sp, #304
	ret
LBB5_13:
	cmp	w8, #3
	b.ne	LBB5_15
LBB5_14:
	mov	w0, #16                         ; =0x10
	b	LBB5_12
LBB5_15:
	mov	w0, #15                         ; =0xf
	b	LBB5_12
LBB5_16:
	mov	w0, #14                         ; =0xe
	b	LBB5_12
	.loh AdrpLdr	Lloh32, Lloh33
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm: ; @_ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	.cfi_startproc
; %bb.0:
	mov	x8, x0
	mov	w0, #0                          ; =0x0
	strb	wzr, [x3]
	str	xzr, [x4]
	mov	w9, #22643                      ; =0x5873
	movk	w9, #29765, lsl #16
	cmp	w8, w9
	b.gt	LBB6_4
; %bb.1:
	mov	w9, #17232                      ; =0x4350
	movk	w9, #26947, lsl #16
	cmp	w8, w9
	b.eq	LBB6_6
; %bb.2:
	mov	w9, #22644                      ; =0x5874
	movk	w9, #26964, lsl #16
	cmp	w8, w9
	b.eq	LBB6_6
LBB6_3:
	ret
LBB6_4:
	mov	w9, #22644                      ; =0x5874
	movk	w9, #31316, lsl #16
	cmp	w8, w9
	b.eq	LBB6_6
; %bb.5:
	mov	w9, #22644                      ; =0x5874
	movk	w9, #29765, lsl #16
	cmp	w8, w9
	b.ne	LBB6_3
LBB6_6:
	mov	w9, #80                         ; =0x50
	cmp	x2, #80
	csel	x10, x2, x9, lo
	cbz	x2, LBB6_14
; %bb.7:
	mov	w9, #1                          ; =0x1
	mov	w11, #1                         ; =0x1
LBB6_8:                                 ; =>This Inner Loop Header: Depth=1
	add	x12, x1, x9
	ldurb	w12, [x12, #-1]
	cbz	w12, LBB6_13
; %bb.9:                                ;   in Loop: Header=BB6_8 Depth=1
	mov	w0, #11                         ; =0xb
	cmp	w12, #32
	b.lo	LBB6_3
; %bb.10:                               ;   in Loop: Header=BB6_8 Depth=1
	sub	w13, w12, #127
	cmp	w13, #34
	b.lo	LBB6_3
; %bb.11:                               ;   in Loop: Header=BB6_8 Depth=1
	cmp	w12, #32
	cset	w12, eq
	and	w11, w11, w12
	tbnz	w11, #0, LBB6_3
; %bb.12:                               ;   in Loop: Header=BB6_8 Depth=1
	mov	x11, x12
	cmp	x10, x9
	add	x9, x9, #1
	b.ne	LBB6_8
	b	LBB6_3
LBB6_13:
	cmp	x9, #1
	cset	w10, eq
	orr	w10, w11, w10
	tbz	w10, #0, LBB6_15
LBB6_14:
	mov	w0, #11                         ; =0xb
	ret
LBB6_15:
	str	x9, [x4]
	mov	w10, #22644                     ; =0x5874
	movk	w10, #31316, lsl #16
	cmp	w8, w10
	b.eq	LBB6_18
; %bb.16:
	mov	w10, #22644                     ; =0x5874
	movk	w10, #29765, lsl #16
	cmp	w8, w10
	b.eq	LBB6_21
; %bb.17:
	mov	w10, #17232                     ; =0x4350
	movk	w10, #26947, lsl #16
	cmp	w8, w10
	b.ne	LBB6_22
LBB6_18:
	cmp	x2, x9
	b.eq	LBB6_14
; %bb.19:
	ldrb	w8, [x1, x9]
	cbnz	w8, LBB6_14
; %bb.20:
	mov	w0, #0                          ; =0x0
	add	x8, x9, #1
	str	x8, [x4]
	mov	w8, #1                          ; =0x1
	strb	w8, [x3]
	ret
LBB6_21:
	mov	w0, #0                          ; =0x0
	ret
LBB6_22:
	sub	x8, x2, x9
	cmp	x8, #2
	b.lo	LBB6_14
; %bb.23:
	ldrb	w8, [x1, x9]
	cmp	w8, #1
	b.hi	LBB6_14
; %bb.24:
	add	x10, x1, x9
	ldrb	w10, [x10, #1]
	cbnz	w10, LBB6_14
; %bb.25:
	cmp	w8, #1
	cset	w8, eq
	strb	w8, [x3]
	add	x8, x9, #2
	str	x8, [x4]
	cmp	x8, x2
	b.hs	LBB6_29
; %bb.26:
	mov	w0, #11                         ; =0xb
LBB6_27:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w9, [x1, x8]
	cbz	w9, LBB6_29
; %bb.28:                               ;   in Loop: Header=BB6_27 Depth=1
	add	x8, x8, #1
	str	x8, [x4]
	cmp	x2, x8
	b.ne	LBB6_27
	b	LBB6_3
LBB6_29:
	cmp	x8, x2
	b.eq	LBB6_14
; %bb.30:
	add	x8, x8, #1
	str	x8, [x4]
	cmp	x8, x2
	b.hs	LBB6_34
; %bb.31:
	mov	w0, #11                         ; =0xb
LBB6_32:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w9, [x1, x8]
	cbz	w9, LBB6_34
; %bb.33:                               ;   in Loop: Header=BB6_32 Depth=1
	add	x8, x8, #1
	str	x8, [x4]
	cmp	x2, x8
	b.ne	LBB6_32
	b	LBB6_3
LBB6_34:
	cmp	x8, x2
	b.eq	LBB6_14
; %bb.35:
	mov	w0, #0                          ; =0x0
	add	x8, x8, #1
	str	x8, [x4]
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
__ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE: ; @_ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	.cfi_startproc
; %bb.0:
	stp	x26, x25, [sp, #-64]!           ; 16-byte Folded Spill
	stp	x24, x23, [sp, #16]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #32]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #48]             ; 16-byte Folded Spill
	.cfi_def_cfa_offset 64
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w25, -56
	.cfi_offset w26, -64
	ldrb	w13, [x0]
	ldrb	w8, [x0, #1]
	ldrb	w9, [x0, #2]
	ldrb	w10, [x0, #3]
	orr	w8, w10, w8, lsl #16
	orr	w9, w8, w9, lsl #8
	bfi	w9, w13, #24, #8
	str	w9, [x3]
	ldrb	w15, [x0, #4]
	ldrb	w8, [x0, #5]
	ldrb	w10, [x0, #6]
	ldrb	w11, [x0, #7]
	orr	w8, w11, w8, lsl #16
	orr	w10, w8, w10, lsl #8
	bfi	w10, w15, #24, #8
	str	w10, [x3, #4]
	ldrb	w11, [x0, #8]
	str	w11, [x3, #8]
	ldrb	w16, [x0, #9]
	str	w16, [x3, #12]
	ldrb	w12, [x0, #12]
	cmp	w12, #0
	cset	w8, ne
	strb	w8, [x3, #16]
	mov	w8, #7                          ; =0x7
	cmp	w16, #2
	b.le	LBB7_5
; %bb.1:
	cmp	w16, #3
	b.eq	LBB7_8
; %bb.2:
	cmp	w16, #4
	b.eq	LBB7_11
; %bb.3:
	cmp	w16, #6
	b.ne	LBB7_37
; %bb.4:
	mov	w14, #4                         ; =0x4
	str	w14, [x3, #284]
	cmp	w11, #8
	b.ne	LBB7_13
	b	LBB7_16
LBB7_5:
	cbz	w16, LBB7_12
; %bb.6:
	cmp	w16, #2
	b.ne	LBB7_37
; %bb.7:
	mov	w14, #3                         ; =0x3
	str	w14, [x3, #284]
	cmp	w11, #8
	b.ne	LBB7_13
	b	LBB7_16
LBB7_8:
	mov	w14, #1                         ; =0x1
	str	w14, [x3, #284]
	sub	w16, w11, #1
	eor	w17, w11, w16
	cmp	w17, w16
	b.ls	LBB7_37
; %bb.9:
	and	w16, w11, #0xf
	cbz	w16, LBB7_37
; %bb.10:
	cbnz	w9, LBB7_17
	b	LBB7_37
LBB7_11:
	mov	w14, #2                         ; =0x2
	str	w14, [x3, #284]
	cmp	w11, #8
	b.ne	LBB7_13
	b	LBB7_16
LBB7_12:
	mov	w14, #1                         ; =0x1
	str	w14, [x3, #284]
	cmp	w11, #8
	b.eq	LBB7_16
LBB7_13:
	cmp	w11, #16
	b.eq	LBB7_16
; %bb.14:
	sub	w8, w11, #3
	cmp	w11, #4
	ccmn	w8, #2, #2, ne
	cset	w17, lo
	mov	w8, #7                          ; =0x7
	cbnz	w16, LBB7_37
; %bb.15:
	tbnz	w17, #0, LBB7_37
LBB7_16:
	cbz	w9, LBB7_37
LBB7_17:
	lsl	w8, w13, #24
	lsl	w13, w15, #24
	orr	w13, w13, w8
	mov	w8, #7                          ; =0x7
	tbnz	w13, #31, LBB7_37
; %bb.18:
	cbz	w10, LBB7_37
; %bb.19:
	ldrb	w8, [x0, #10]
	cbnz	w8, LBB7_22
; %bb.20:
	ldrb	w8, [x0, #11]
	cbnz	w8, LBB7_22
; %bb.21:
	ldrb	w8, [x0, #12]
	cmp	w8, #1
	b.ls	LBB7_23
LBB7_22:
	mov	w8, #7                          ; =0x7
	mov	x0, x8
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #64             ; 16-byte Folded Reload
	ret
LBB7_23:
	ldr	w13, [x2, #40]
	mov	w8, #2                          ; =0x2
	cmp	w9, w13
	b.hi	LBB7_37
; %bb.24:
	cmp	w10, w13
	b.hi	LBB7_37
; %bb.25:
	umull	x15, w10, w9
	lsr	x13, x15, #61
	cbnz	x13, LBB7_37
; %bb.26:
	ldr	x13, [x2, #8]
	cmp	x15, x13
	b.hi	LBB7_37
; %bb.27:
	mov	x13, #0                         ; =0x0
	lsl	x8, x15, #2
	str	x8, [x3, #24]
	mul	w8, w14, w11
	lsr	w8, w8, #3
	cmp	w8, #1
	csinc	w8, w8, wzr, hi
	str	w8, [x3, #288]
	ldr	x14, [x2, #16]
	add	x15, x3, #56
Lloh34:
	adrp	x16, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x@PAGE
Lloh35:
	add	x16, x16, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x@PAGEOFF
Lloh36:
	adrp	x17, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y@PAGE
Lloh37:
	add	x17, x17, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y@PAGEOFF
	ldp	x1, x0, [x3, #32]
Lloh38:
	adrp	x2, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x@PAGE
Lloh39:
	add	x2, x2, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x@PAGEOFF
Lloh40:
	adrp	x4, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y@PAGE
Lloh41:
	add	x4, x4, l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y@PAGEOFF
	mov	x5, #7                          ; =0x7
LBB7_28:                                ; =>This Inner Loop Header: Depth=1
	cbz	w12, LBB7_30
; %bb.29:                               ;   in Loop: Header=BB7_28 Depth=1
	ldr	w6, [x16, x13, lsl #2]
	ldr	w8, [x17, x13, lsl #2]
	ldr	w19, [x2, x13, lsl #2]
	ldr	w7, [x4, x13, lsl #2]
	cmp	w9, w6
	b.hi	LBB7_31
	b	LBB7_35
LBB7_30:                                ;   in Loop: Header=BB7_28 Depth=1
	mov	w8, #0                          ; =0x0
	mov	w6, #0                          ; =0x0
	mov	w7, #1                          ; =0x1
	mov	w19, #1                         ; =0x1
	cmp	w9, w6
	b.ls	LBB7_35
LBB7_31:                                ;   in Loop: Header=BB7_28 Depth=1
	cmp	w10, w8
	b.ls	LBB7_35
; %bb.32:                               ;   in Loop: Header=BB7_28 Depth=1
	mvn	w20, w6
	add	w20, w9, w20
	udiv	w20, w20, w19
	add	w20, w20, #1
	mvn	w21, w8
	add	w21, w10, w21
	udiv	w21, w21, w7
	add	w21, w21, #1
	ldr	w22, [x3, #284]
	umull	x23, w11, w20
	madd	x22, x23, x22, x5
	lsr	x22, x22, #3
	udiv	x23, x14, x21
	cmp	x22, x23
	b.hs	LBB7_38
; %bb.33:                               ;   in Loop: Header=BB7_28 Depth=1
	add	x23, x22, #1
	mul	x24, x23, x21
	sub	x25, x14, x0
	cmp	x24, x25
	b.hi	LBB7_38
; %bb.34:                               ;   in Loop: Header=BB7_28 Depth=1
	add	x0, x0, x24
	lsl	x23, x23, #1
	cmp	x1, x23
	csel	x1, x1, x23, hi
	stp	x1, x0, [x3, #32]
	ldr	w23, [x3, #280]
	add	x23, x15, x23, lsl #5
	stp	w6, w8, [x23]
	stp	w19, w7, [x23, #8]
	stp	w20, w21, [x23, #16]
	str	x22, [x23, #24]
	ldr	w8, [x3, #280]
	add	w8, w8, #1
	str	w8, [x3, #280]
LBB7_35:                                ;   in Loop: Header=BB7_28 Depth=1
	mov	w8, #0                          ; =0x0
	cbz	w12, LBB7_37
; %bb.36:                               ;   in Loop: Header=BB7_28 Depth=1
	cmp	x13, #6
	add	x13, x13, #1
	b.lo	LBB7_28
LBB7_37:
	mov	x0, x8
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #64             ; 16-byte Folded Reload
	ret
LBB7_38:
	mov	w8, #2                          ; =0x2
	mov	x0, x8
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #64             ; 16-byte Folded Reload
	ret
	.loh AdrpAdd	Lloh40, Lloh41
	.loh AdrpAdd	Lloh38, Lloh39
	.loh AdrpAdd	Lloh36, Lloh37
	.loh AdrpAdd	Lloh34, Lloh35
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB8_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldr	w9, [x0, #2344]
	ldp	w13, w12, [x1]
	madd	w10, w10, w2, w12
	ldr	w12, [x0]
	umaddl	x10, w12, w10, x13
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
LBB8_2:                                 ; =>This Inner Loop Header: Depth=1
	ldrb	w12, [x3], #1
	cmp	w9, w12
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x10, #-4]
	csetm	w13, ne
	sturh	w12, [x10, #-2]
	strh	w12, [x10]
	strh	w13, [x10, #2]
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB8_2
LBB8_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB9_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w12, w9, #2
	ldr	w9, [x0, #2344]
	ldp	w13, w11, [x1]
	madd	w10, w10, w2, w11
	ldr	w11, [x0]
	umaddl	x11, w11, w10, x13
	add	x10, x3, #1
	add	x11, x5, x11, lsl #3
	add	x11, x11, #4
	lsl	x12, x12, #1
LBB9_2:                                 ; =>This Inner Loop Header: Depth=1
	ldurh	w13, [x10, #-1]
	rev16	w13, w13
	cmp	w13, w9
	csetm	w14, ne
	sturh	w13, [x11, #-4]
	sturh	w13, [x11, #-2]
	strh	w13, [x11]
	strh	w14, [x11, #2]
	add	x10, x10, #2
	add	x11, x11, x12
	subs	x8, x8, #1
	b.ne	LBB9_2
LBB9_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB10_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	ldp	w10, w11, [x1, #8]
	lsl	w12, w10, #2
	ldr	w10, [x0, #2344]
	ldp	w14, w13, [x1]
	madd	w11, w11, w2, w13
	ldr	w13, [x0]
	umaddl	x11, w13, w11, x14
	add	x11, x5, x11, lsl #3
	add	x11, x11, #4
	lsl	x12, x12, #1
	mov	w13, #7                         ; =0x7
LBB10_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w14, w13, w9
	lsr	x15, x9, #3
	ldrb	w15, [x3, x15]
	lsr	w14, w15, w14
	and	w14, w14, #0x1
	cmp	w14, w10
	csetm	w15, ne
	neg	w14, w14
	sturh	w14, [x11, #-4]
	sturh	w14, [x11, #-2]
	strh	w14, [x11]
	strh	w15, [x11, #2]
	add	x9, x9, #1
	add	x11, x11, x12
	cmp	x8, x9
	b.ne	LBB10_2
LBB10_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB11_3
; %bb.1:
	mov	w9, #0                          ; =0x0
	mov	x10, #0                         ; =0x0
	ldp	w11, w12, [x1, #8]
	ldp	w14, w13, [x1]
	madd	w12, w12, w2, w13
	lsl	w13, w11, #2
	ldr	w11, [x0]
	umaddl	x11, w11, w12, x14
	add	x12, x5, x11, lsl #3
	ldr	w11, [x0, #2344]
	add	x12, x12, #4
	lsl	x13, x13, #1
	mov	w14, #6                         ; =0x6
	mov	w15, #21845                     ; =0x5555
LBB11_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w16, w14, w9
	lsr	x17, x10, #2
	ldrb	w17, [x3, x17]
	lsr	w16, w17, w16
	and	w16, w16, #0x3
	cmp	w16, w11
	csetm	w17, ne
	mul	w16, w16, w15
	sturh	w16, [x12, #-4]
	sturh	w16, [x12, #-2]
	strh	w16, [x12]
	strh	w17, [x12, #2]
	add	x10, x10, #1
	add	w9, w9, #2
	add	x12, x12, x13
	cmp	x8, x10
	b.ne	LBB11_2
LBB11_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB12_3
; %bb.1:
	mov	w9, #0                          ; =0x0
	mov	x10, #0                         ; =0x0
	ldp	w11, w12, [x1, #8]
	ldp	w14, w13, [x1]
	madd	w12, w12, w2, w13
	lsl	w13, w11, #2
	ldr	w11, [x0]
	umaddl	x11, w11, w12, x14
	add	x12, x5, x11, lsl #3
	ldr	w11, [x0, #2344]
	add	x12, x12, #4
	lsl	x13, x13, #1
	mov	w14, #4                         ; =0x4
	mov	w15, #4369                      ; =0x1111
LBB12_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w16, w14, w9
	lsr	x17, x10, #1
	ldrb	w17, [x3, x17]
	lsr	w16, w17, w16
	and	w16, w16, #0xf
	cmp	w16, w11
	csetm	w17, ne
	mul	w16, w16, w15
	sturh	w16, [x12, #-4]
	sturh	w16, [x12, #-2]
	strh	w16, [x12]
	strh	w17, [x12, #2]
	add	x10, x10, #1
	add	w9, w9, #4
	add	x12, x12, x13
	cmp	x8, x10
	b.ne	LBB12_2
LBB12_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB13_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x9, w10, w9, x12
	add	x9, x5, x9, lsl #3
	add	x9, x9, #4
	lsl	x10, x11, #1
	mov	w11, #65535                     ; =0xffff
LBB13_2:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w12, [x3], #1
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x9, #-4]
	sturh	w12, [x9, #-2]
	strh	w12, [x9]
	strh	w11, [x9, #2]
	add	x9, x9, x10
	subs	x8, x8, #1
	b.ne	LBB13_2
LBB13_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB14_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #1
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
	mov	w12, #65535                     ; =0xffff
LBB14_2:                                ; =>This Inner Loop Header: Depth=1
	ldurh	w13, [x9, #-1]
	rev	w13, w13
	lsr	w13, w13, #16
	sturh	w13, [x10, #-4]
	sturh	w13, [x10, #-2]
	strh	w13, [x10]
	strh	w12, [x10, #2]
	add	x9, x9, #2
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB14_2
LBB14_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB15_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	ldp	w10, w11, [x1, #8]
	ldp	w13, w12, [x1]
	madd	w11, w11, w2, w12
	ldr	w12, [x0]
	umaddl	x11, w12, w11, x13
	add	x11, x5, x11, lsl #3
	lsl	w12, w10, #2
	add	x10, x11, #4
	lsl	x11, x12, #1
	mov	w12, #7                         ; =0x7
	mov	w13, #65535                     ; =0xffff
LBB15_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w14, w12, w9
	lsr	x15, x9, #3
	ldrb	w15, [x3, x15]
	lsr	w14, w15, w14
	sbfx	w14, w14, #0, #1
	sturh	w14, [x10, #-4]
	sturh	w14, [x10, #-2]
	strh	w14, [x10]
	strh	w13, [x10, #2]
	add	x9, x9, #1
	add	x10, x10, x11
	cmp	x8, x9
	b.ne	LBB15_2
LBB15_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB16_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	ldp	w10, w11, [x1, #8]
	ldp	w13, w12, [x1]
	madd	w11, w11, w2, w12
	ldr	w12, [x0]
	umaddl	x11, w12, w11, x13
	add	x11, x5, x11, lsl #3
	lsl	w12, w10, #2
	add	x10, x11, #4
	lsl	x11, x12, #1
	mov	w12, #6                         ; =0x6
	mov	w13, #21845                     ; =0x5555
	mov	w14, #65535                     ; =0xffff
LBB16_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w15, w12, w9, lsl #1
	lsr	x16, x9, #2
	ldrb	w16, [x3, x16]
	lsr	w15, w16, w15
	and	w15, w15, #0x3
	mul	w15, w15, w13
	sturh	w15, [x10, #-4]
	sturh	w15, [x10, #-2]
	strh	w15, [x10]
	strh	w14, [x10, #2]
	add	x9, x9, #1
	add	x10, x10, x11
	cmp	x8, x9
	b.ne	LBB16_2
LBB16_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB17_3
; %bb.1:
	mov	x9, #0                          ; =0x0
	ldp	w10, w11, [x1, #8]
	ldp	w13, w12, [x1]
	madd	w11, w11, w2, w12
	ldr	w12, [x0]
	umaddl	x11, w12, w11, x13
	add	x11, x5, x11, lsl #3
	lsl	w12, w10, #2
	add	x10, x11, #4
	lsl	x11, x12, #1
	mov	w12, #4                         ; =0x4
	mov	w13, #4369                      ; =0x1111
	mov	w14, #65535                     ; =0xffff
LBB17_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w15, w12, w9, lsl #2
	lsr	x16, x9, #1
	ldrb	w16, [x3, x16]
	lsr	w15, w16, w15
	and	w15, w15, #0xf
	mul	w15, w15, w13
	sturh	w15, [x10, #-4]
	sturh	w15, [x10, #-2]
	strh	w15, [x10]
	strh	w14, [x10, #2]
	add	x9, x9, #1
	add	x10, x10, x11
	cmp	x8, x9
	b.ne	LBB17_2
LBB17_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB18_3
; %bb.1:
	ldp	w9, w12, [x1, #8]
	lsl	w14, w9, #2
	ldr	w9, [x0, #2344]
	ldr	w10, [x0, #2348]
	ldr	w11, [x0, #2352]
	ldp	w15, w13, [x1]
	madd	w12, w12, w2, w13
	ldr	w13, [x0]
	umaddl	x13, w13, w12, x15
	add	x12, x3, #2
	add	x13, x5, x13, lsl #3
	add	x13, x13, #4
	lsl	x14, x14, #1
LBB18_2:                                ; =>This Inner Loop Header: Depth=1
	ldurb	w15, [x12, #-2]
	ldurb	w16, [x12, #-1]
	and	w17, w16, #0xff
	ldrb	w0, [x12], #3
	and	w1, w0, #0xff
	cmp	w9, w15
	ccmp	w10, w17, #0, eq
	ccmp	w11, w1, #0, eq
	csetm	w17, ne
	orr	w15, w15, w15, lsl #8
	sturh	w15, [x13, #-4]
	add	w15, w16, w16, lsl #8
	sturh	w15, [x13, #-2]
	add	w15, w0, w0, lsl #8
	strh	w15, [x13]
	strh	w17, [x13, #2]
	add	x13, x13, x14
	subs	x8, x8, #1
	b.ne	LBB18_2
LBB18_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB19_3
; %bb.1:
	ldp	w9, w12, [x1, #8]
	lsl	w14, w9, #2
	ldr	w9, [x0, #2344]
	ldr	w10, [x0, #2348]
	ldr	w11, [x0, #2352]
	ldp	w15, w13, [x1]
	madd	w12, w12, w2, w13
	ldr	w13, [x0]
	umaddl	x13, w13, w12, x15
	add	x12, x3, #2
	add	x13, x5, x13, lsl #3
	add	x13, x13, #4
	lsl	x14, x14, #1
LBB19_2:                                ; =>This Inner Loop Header: Depth=1
	ldurh	w15, [x12, #-2]
	rev16	w15, w15
	ldrh	w16, [x12]
	rev16	w16, w16
	ldrh	w17, [x12, #2]
	rev16	w17, w17
	cmp	w15, w9
	ccmp	w16, w10, #0, eq
	ccmp	w17, w11, #0, eq
	csetm	w0, ne
	sturh	w15, [x13, #-4]
	sturh	w16, [x13, #-2]
	strh	w17, [x13]
	strh	w0, [x13, #2]
	add	x12, x12, #6
	add	x13, x13, x14
	subs	x8, x8, #1
	b.ne	LBB19_2
LBB19_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB20_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #2
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
	mov	w12, #65535                     ; =0xffff
LBB20_2:                                ; =>This Inner Loop Header: Depth=1
	ldurb	w13, [x9, #-2]
	ldurb	w14, [x9, #-1]
	ldrb	w15, [x9], #3
	orr	w13, w13, w13, lsl #8
	sturh	w13, [x10, #-4]
	orr	w13, w14, w14, lsl #8
	sturh	w13, [x10, #-2]
	orr	w13, w15, w15, lsl #8
	strh	w13, [x10]
	strh	w12, [x10, #2]
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB20_2
LBB20_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB21_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #2
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
	mov	w12, #65535                     ; =0xffff
LBB21_2:                                ; =>This Inner Loop Header: Depth=1
	ldurh	w13, [x9, #-2]
	rev	w13, w13
	lsr	w13, w13, #16
	ldrh	w14, [x9]
	rev	w14, w14
	lsr	w14, w14, #16
	ldrh	w15, [x9, #2]
	rev	w15, w15
	lsr	w15, w15, #16
	sturh	w13, [x10, #-4]
	sturh	w14, [x10, #-2]
	strh	w15, [x10]
	strh	w12, [x10, #2]
	add	x9, x9, #6
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB21_2
LBB21_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB22_4
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w12, w9, #2
	ldp	w11, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x11, w10, w9, x11
	ldr	w9, [x0, #2340]
	add	x10, x0, #292
	add	x11, x5, x11, lsl #3
	add	x11, x11, #4
	lsl	x12, x12, #1
LBB22_2:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x3], #1
	cmp	w9, w13
	b.ls	LBB22_5
; %bb.3:                                ;   in Loop: Header=BB22_2 Depth=1
	add	x13, x10, x13, lsl #3
	ldrh	w14, [x13]
	sturh	w14, [x11, #-4]
	ldrh	w14, [x13, #2]
	sturh	w14, [x11, #-2]
	ldrh	w14, [x13, #4]
	strh	w14, [x11]
	ldrh	w13, [x13, #6]
	strh	w13, [x11, #2]
	add	x11, x11, x12
	subs	x8, x8, #1
	b.ne	LBB22_2
LBB22_4:
	mov	w0, #0                          ; =0x0
	ret
LBB22_5:
	mov	w0, #19                         ; =0x13
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB23_4
; %bb.1:
	mov	x9, #0                          ; =0x0
	ldp	w10, w11, [x1, #8]
	ldp	w13, w12, [x1]
	madd	w11, w11, w2, w12
	lsl	w14, w10, #2
	ldr	w10, [x0]
	umaddl	x12, w10, w11, x13
	ldr	w10, [x0, #2340]
	add	x11, x0, #292
	add	x12, x5, x12, lsl #3
	add	x12, x12, #4
	lsl	x13, x14, #1
	mov	w14, #7                         ; =0x7
LBB23_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w15, w14, w9
	lsr	x16, x9, #3
	ldrb	w16, [x3, x16]
	lsr	w15, w16, w15
	and	w15, w15, #0x1
	cmp	w15, w10
	b.hs	LBB23_5
; %bb.3:                                ;   in Loop: Header=BB23_2 Depth=1
	add	x15, x11, w15, uxtw #3
	ldrh	w16, [x15]
	sturh	w16, [x12, #-4]
	ldrh	w16, [x15, #2]
	sturh	w16, [x12, #-2]
	ldrh	w16, [x15, #4]
	strh	w16, [x12]
	ldrh	w15, [x15, #6]
	strh	w15, [x12, #2]
	add	x9, x9, #1
	add	x12, x12, x13
	cmp	x8, x9
	b.ne	LBB23_2
LBB23_4:
	mov	w0, #0                          ; =0x0
	ret
LBB23_5:
	mov	w0, #19                         ; =0x13
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB24_4
; %bb.1:
	mov	w9, #0                          ; =0x0
	mov	x10, #0                         ; =0x0
	ldp	w11, w12, [x1, #8]
	ldp	w14, w13, [x1]
	madd	w12, w12, w2, w13
	ldr	w13, [x0]
	umaddl	x13, w13, w12, x14
	lsl	w14, w11, #2
	ldr	w11, [x0, #2340]
	add	x12, x0, #292
	add	x13, x5, x13, lsl #3
	add	x13, x13, #4
	lsl	x14, x14, #1
	mov	w15, #6                         ; =0x6
LBB24_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w16, w15, w9
	lsr	x17, x10, #2
	ldrb	w17, [x3, x17]
	lsr	w16, w17, w16
	and	w16, w16, #0x3
	cmp	w16, w11
	b.hs	LBB24_5
; %bb.3:                                ;   in Loop: Header=BB24_2 Depth=1
	add	x16, x12, w16, uxtw #3
	ldrh	w17, [x16]
	sturh	w17, [x13, #-4]
	ldrh	w17, [x16, #2]
	sturh	w17, [x13, #-2]
	ldrh	w17, [x16, #4]
	strh	w17, [x13]
	ldrh	w16, [x16, #6]
	strh	w16, [x13, #2]
	add	x10, x10, #1
	add	w9, w9, #2
	add	x13, x13, x14
	cmp	x8, x10
	b.ne	LBB24_2
LBB24_4:
	mov	w0, #0                          ; =0x0
	ret
LBB24_5:
	mov	w0, #19                         ; =0x13
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB25_4
; %bb.1:
	mov	w9, #0                          ; =0x0
	mov	x10, #0                         ; =0x0
	ldp	w11, w12, [x1, #8]
	ldp	w14, w13, [x1]
	madd	w12, w12, w2, w13
	ldr	w13, [x0]
	umaddl	x13, w13, w12, x14
	lsl	w14, w11, #2
	ldr	w11, [x0, #2340]
	add	x12, x0, #292
	add	x13, x5, x13, lsl #3
	add	x13, x13, #4
	lsl	x14, x14, #1
	mov	w15, #4                         ; =0x4
LBB25_2:                                ; =>This Inner Loop Header: Depth=1
	bic	w16, w15, w9
	lsr	x17, x10, #1
	ldrb	w17, [x3, x17]
	lsr	w16, w17, w16
	and	w16, w16, #0xf
	cmp	w16, w11
	b.hs	LBB25_5
; %bb.3:                                ;   in Loop: Header=BB25_2 Depth=1
	add	x16, x12, w16, uxtw #3
	ldrh	w17, [x16]
	sturh	w17, [x13, #-4]
	ldrh	w17, [x16, #2]
	sturh	w17, [x13, #-2]
	ldrh	w17, [x16, #4]
	strh	w17, [x13]
	ldrh	w16, [x16, #6]
	strh	w16, [x13, #2]
	add	x10, x10, #1
	add	w9, w9, #4
	add	x13, x13, x14
	cmp	x8, x10
	b.ne	LBB25_2
LBB25_4:
	mov	w0, #0                          ; =0x0
	ret
LBB25_5:
	mov	w0, #19                         ; =0x13
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB26_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #1
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
LBB26_2:                                ; =>This Inner Loop Header: Depth=1
	ldurb	w12, [x9, #-1]
	ldrb	w13, [x9], #2
	orr	w12, w12, w12, lsl #8
	sturh	w12, [x10, #-4]
	sturh	w12, [x10, #-2]
	strh	w12, [x10]
	orr	w12, w13, w13, lsl #8
	strh	w12, [x10, #2]
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB26_2
LBB26_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB27_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #3
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
LBB27_2:                                ; =>This Inner Loop Header: Depth=1
	ldurh	w12, [x9, #-3]
	rev	w12, w12
	lsr	w12, w12, #16
	ldurh	w13, [x9, #-1]
	rev	w13, w13
	lsr	w13, w13, #16
	sturh	w12, [x10, #-4]
	sturh	w12, [x10, #-2]
	strh	w12, [x10]
	strh	w13, [x10, #2]
	add	x9, x9, #4
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB27_2
LBB27_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB28_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x9, w10, w9, x12
	add	x9, x5, x9, lsl #3
	lsl	x10, x11, #1
	movi.8b	v0, #1
LBB28_2:                                ; =>This Inner Loop Header: Depth=1
	ldr	s1, [x3], #4
	ushll.8h	v1, v1, #0
	mul.4h	v1, v1, v0
	str	d1, [x9]
	add	x9, x9, x10
	subs	x8, x8, #1
	b.ne	LBB28_2
LBB28_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ; @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	ldr	w8, [x1, #16]
	cbz	w8, LBB29_3
; %bb.1:
	ldp	w9, w10, [x1, #8]
	lsl	w11, w9, #2
	ldp	w12, w9, [x1]
	madd	w9, w10, w2, w9
	ldr	w10, [x0]
	umaddl	x10, w10, w9, x12
	add	x9, x3, #3
	add	x10, x5, x10, lsl #3
	add	x10, x10, #4
	lsl	x11, x11, #1
LBB29_2:                                ; =>This Inner Loop Header: Depth=1
	ldurh	w12, [x9, #-3]
	rev	w12, w12
	lsr	w12, w12, #16
	ldurh	w13, [x9, #-1]
	rev	w13, w13
	lsr	w13, w13, #16
	ldurh	w14, [x9, #1]
	rev	w14, w14
	lsr	w14, w14, #16
	ldurh	w15, [x9, #3]
	rev	w15, w15
	lsr	w15, w15, #16
	sturh	w12, [x10, #-4]
	sturh	w13, [x10, #-2]
	strh	w14, [x10]
	strh	w15, [x10, #2]
	add	x9, x9, #8
	add	x10, x10, x11
	subs	x8, x8, #1
	b.ne	LBB29_2
LBB29_3:
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ; @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x:
	.long	0                               ; 0x0
	.long	4                               ; 0x4
	.long	0                               ; 0x0
	.long	2                               ; 0x2
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	0                               ; 0x0

	.p2align	2, 0x0                          ; @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y:
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	4                               ; 0x4
	.long	0                               ; 0x0
	.long	2                               ; 0x2
	.long	0                               ; 0x0
	.long	1                               ; 0x1

	.p2align	2, 0x0                          ; @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x:
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	2                               ; 0x2
	.long	2                               ; 0x2
	.long	1                               ; 0x1

	.p2align	2, 0x0                          ; @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y:
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	2                               ; 0x2
	.long	2                               ; 0x2

	.section	__DATA,__const
	.p2align	3, 0x0                          ; @switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE
l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

	.p2align	3, 0x0                          ; @switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.1
l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.1:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

	.p2align	3, 0x0                          ; @switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.2
l_switch.table._ZN9gui_forms5image12_GLOBAL__N_114select_scatterERKNS1_4PlanE.2:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

.subsections_via_symbols
