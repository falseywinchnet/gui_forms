	.build_version macos, 26, 0	sdk_version 26, 2
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI0_0:
	.long	209                             ; 0xd1
	.long	52                              ; 0x34
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0
lCPI0_1:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI0_2:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-80]!           ; 16-byte Folded Spill
	stp	x24, x23, [sp, #16]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #32]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #48]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #64]             ; 16-byte Folded Spill
	add	x29, sp, #64
	sub	sp, sp, #1056
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w27, -72
	.cfi_offset w28, -80
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #3
	b.hi	LBB0_5
; %bb.1:
	mvn	w0, w21
	cbz	x20, LBB0_14
; %bb.2:
	mov	w8, #33568                      ; =0x8320
	movk	w8, #60856, lsl #16
	mov	w9, #16784                      ; =0x4190
	movk	w9, #30428, lsl #16
	ldrb	w10, [x19]
	eor	w10, w0, w10
	sbfx	w11, w10, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w10, lsr #1
	lsl	w12, w10, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v1, w10
Lloh0:
	adrp	x10, lCPI0_1@PAGE
Lloh1:
	ldr	q0, [x10, lCPI0_1@PAGEOFF]
	and.16b	v1, v1, v0
	cmeq.4s	v2, v1, #0
Lloh2:
	adrp	x10, lCPI0_2@PAGE
Lloh3:
	ldr	q1, [x10, lCPI0_2@PAGEOFF]
	bic.16b	v2, v1, v2
	lsl	w10, w11, #26
	and	w10, w9, w10, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w10, w13, w10
	eor	w10, w10, w14
	eor	w11, w11, w12, lsr #6
	eor	w0, w10, w11
	cmp	x20, #1
	b.eq	LBB0_14
; %bb.3:
	ldrb	w10, [x19, #1]
	eor	w10, w0, w10
	sbfx	w11, w10, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w10, lsr #1
	lsl	w12, w10, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w10
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w10, w11, #26
	and	w10, w9, w10, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w10, w13, w10
	eor	w10, w10, w14
	eor	w11, w11, w12, lsr #6
	eor	w0, w10, w11
	cmp	x20, #2
	b.eq	LBB0_14
; %bb.4:
	ldrb	w10, [x19, #2]
	eor	w10, w0, w10
	sbfx	w11, w10, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w10, lsr #1
	lsl	w12, w10, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w10
	and.16b	v0, v2, v0
	cmeq.4s	v0, v0, #0
	bic.16b	v0, v1, v0
	lsl	w10, w11, #26
	and	w9, w9, w10, asr #31
	lsl	w10, w12, #26
	and	w8, w8, w10, asr #31
	ext.16b	v1, v0, v0, #8
	eor.8b	v0, v0, v1
	fmov	x10, d0
	lsr	x11, x10, #32
	eor	w9, w10, w9
	eor	w9, w9, w11
	eor	w8, w8, w12, lsr #6
	eor	w0, w9, w8
	b	LBB0_14
LBB0_5:
	add	x22, sp, #4
	add	x23, x22, #836
	add	x24, x22, #1044
	add	x0, x22, #4
	mov	w1, #1040                       ; =0x410
	bl	_bzero
	ldr	w8, [x19]
	eon	w8, w8, w21
	str	wzr, [sp, #4]
	str	w8, [sp, #840]
	movi.2s	v0, #1
	str	d0, [x24]
	adrp	x8, lCPI0_0@PAGE
	cmp	x20, #8
	b.hs	LBB0_7
; %bb.6:
	mov	w9, #1                          ; =0x1
	mov	w21, #4                         ; =0x4
	mov	w10, #1                         ; =0x1
	b	LBB0_9
LBB0_7:
	sub	x11, x20, #4
	mov	w9, #1                          ; =0x1
	mov	w21, #4                         ; =0x4
	mov	w12, #209                       ; =0xd1
	ldr	d1, [x8, lCPI0_0@PAGEOFF]
	mov	w10, #1                         ; =0x1
LBB0_8:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w13, [x19, x21]
	cmp	w10, #12
	csel	w14, w12, wzr, lo
	add	w14, w10, w14
	sub	w14, w14, #12
	cmp	w10, #150
	csel	w15, w12, wzr, lo
	ldr	w9, [x23, w9, uxtw #2]
	add	w15, w10, w15
	ldr	w14, [x22, w14, uxtw #2]
	eor	w9, w14, w9
	ldr	w14, [x22, w10, uxtw #2]
	sub	w15, w15, #150
	ldr	w15, [x22, w15, uxtw #2]
	eor	w15, w15, w9
	str	w15, [x22, w10, uxtw #2]
	eor	w10, w14, w13
	eor	w9, w10, w9
	ldr	w10, [sp, #1052]
	str	w9, [x23, x10, lsl #2]
	ldr	d2, [x24]
	add.2s	v2, v2, v0
	cmeq.2s	v3, v2, v1
	bic.8b	v2, v2, v3
	str	d2, [x24]
	add	x21, x21, #4
	mov.s	w9, v2[1]
	fmov	w10, s2
	sub	x11, x11, #4
	cmp	x11, #3
	b.hi	LBB0_8
LBB0_9:
	mov	w11, #52                        ; =0x34
	mov	w12, #209                       ; =0xd1
	ldr	d1, [x8, lCPI0_0@PAGEOFF]
LBB0_10:                                ; =>This Inner Loop Header: Depth=1
	cmp	w10, #12
	csel	w8, w12, wzr, lo
	add	w8, w10, w8
	sub	w8, w8, #12
	cmp	w10, #150
	csel	w13, w12, wzr, lo
	add	w13, w10, w13
	ldr	w9, [x23, w9, uxtw #2]
	sub	w13, w13, #150
	ldr	w8, [x22, w8, uxtw #2]
	eor	w8, w8, w9
	ldr	w9, [x22, w10, uxtw #2]
	ldr	w13, [x22, w13, uxtw #2]
	eor	w13, w13, w8
	str	w13, [x22, w10, uxtw #2]
	eor	w8, w9, w8
	ldr	w9, [sp, #1052]
	str	w8, [x23, x9, lsl #2]
	ldr	d2, [x24]
	add.2s	v2, v2, v0
	cmeq.2s	v3, v2, v1
	bic.8b	v2, v2, v3
	str	d2, [x24]
	mov.s	w9, v2[1]
	fmov	w10, s2
	subs	w11, w11, #1
	b.ne	LBB0_10
; %bb.11:
	add	x0, sp, #4
	bl	__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	subs	x8, x20, x21
	b.ls	LBB0_14
; %bb.12:
	mov	w9, #33568                      ; =0x8320
	movk	w9, #60856, lsl #16
	mov	w10, #16784                     ; =0x4190
	movk	w10, #30428, lsl #16
Lloh4:
	adrp	x11, lCPI0_1@PAGE
Lloh5:
	ldr	q0, [x11, lCPI0_1@PAGEOFF]
Lloh6:
	adrp	x11, lCPI0_2@PAGE
Lloh7:
	ldr	q1, [x11, lCPI0_2@PAGEOFF]
	add	x11, x19, x21
LBB0_13:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w12, [x11], #1
	eor	w12, w0, w12
	sbfx	w13, w12, #0, #1
	and	w13, w13, w9
	eor	w13, w13, w12, lsr #1
	lsl	w14, w12, #30
	and	w14, w9, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w12
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w12, w13, #26
	and	w12, w10, w12, asr #31
	lsl	w13, w14, #26
	and	w13, w9, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w12, w15, w12
	eor	w12, w12, w16
	eor	w13, w13, w14, lsr #6
	eor	w0, w12, w13
	subs	x8, x8, #1
	b.ne	LBB0_13
LBB0_14:
	mvn	w0, w0
	add	sp, sp, #1056
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #80             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh2, Lloh3
	.loh AdrpAdrp	Lloh0, Lloh2
	.loh AdrpLdr	Lloh0, Lloh1
	.loh AdrpLdr	Lloh6, Lloh7
	.loh AdrpAdrp	Lloh4, Lloh6
	.loh AdrpLdr	Lloh4, Lloh5
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI1_0:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI1_1:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	sub	sp, sp, #2112
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
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #7
	b.hi	LBB1_9
; %bb.1:
	mvn	w9, w21
	cbz	x20, LBB1_36
; %bb.2:
	mov	w8, #33568                      ; =0x8320
	movk	w8, #60856, lsl #16
	mov	w10, #16784                     ; =0x4190
	movk	w10, #30428, lsl #16
	ldrb	w11, [x19]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v1, w9
Lloh8:
	adrp	x9, lCPI1_0@PAGE
Lloh9:
	ldr	q0, [x9, lCPI1_0@PAGEOFF]
	and.16b	v1, v1, v0
	cmeq.4s	v2, v1, #0
Lloh10:
	adrp	x9, lCPI1_1@PAGE
Lloh11:
	ldr	q1, [x9, lCPI1_1@PAGEOFF]
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #1
	b.eq	LBB1_36
; %bb.3:
	ldrb	w11, [x19, #1]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #2
	b.eq	LBB1_36
; %bb.4:
	ldrb	w11, [x19, #2]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #3
	b.eq	LBB1_36
; %bb.5:
	ldrb	w11, [x19, #3]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #4
	b.eq	LBB1_36
; %bb.6:
	ldrb	w11, [x19, #4]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #5
	b.eq	LBB1_36
; %bb.7:
	ldrb	w11, [x19, #5]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #6
	b.eq	LBB1_36
; %bb.8:
	ldrb	w11, [x19, #6]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v0, v2, v0
	cmeq.4s	v0, v0, #0
	bic.16b	v0, v1, v0
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w10, w12, #26
	and	w8, w8, w10, asr #31
	ext.16b	v1, v0, v0, #8
	eor.8b	v0, v0, v1
	fmov	x10, d0
	lsr	x11, x10, #32
	eor	w9, w10, w9
	eor	w9, w9, w11
	eor	w8, w8, w12, lsr #6
	eor	w9, w9, w8
	b	LBB1_36
LBB1_9:
	add	x23, sp, #8
	add	x22, x23, #1672
	add	x0, sp, #8
	mov	w1, #2096                       ; =0x830
	bl	_bzero
	ldp	w8, w9, [x19]
	eon	w8, w21, w8
	str	w8, [sp, #1680]
	str	wzr, [sp, #12]
	str	w9, [sp, #1684]
	movi.2s	v0, #1
	str	d0, [sp, #2096]
	cmp	x20, #16
	b.hs	LBB1_11
; %bb.10:
	mov	w12, #1                         ; =0x1
	mov	w8, #8                          ; =0x8
	mov	w13, #1                         ; =0x1
	b	LBB1_13
LBB1_11:
	add	x9, x19, #12
	sub	x10, x20, #8
	mov	w12, #1                         ; =0x1
	mov	w8, #8                          ; =0x8
	mov	w11, #209                       ; =0xd1
	mov	w13, #1                         ; =0x1
LBB1_12:                                ; =>This Inner Loop Header: Depth=1
	ldur	w14, [x9, #-4]
	cmp	w13, #12
	csel	w15, w11, wzr, lo
	add	w15, w13, w15
	sub	w15, w15, #12
	cmp	w13, #150
	csel	w16, w11, wzr, lo
	add	w16, w13, w16
	sub	w16, w16, #150
	add	x15, x23, w15, uxtw #3
	add	x16, x23, w16, uxtw #3
	add	x12, x22, w12, uxtw #3
	ldp	w17, w0, [x12]
	ldp	w1, w15, [x15]
	eor	w17, w1, w17
	ubfiz	x13, x13, #3, #32
	ldr	w1, [x23, x13]
	ldp	w2, w16, [x16]
	eor	w2, w2, w17
	str	w2, [x23, x13]
	ldr	w13, [x9], #8
	eor	w14, w14, w1
	eor	w14, w14, w17
	str	w14, [x12]
	eor	w12, w15, w0
	ldr	w14, [sp, #2096]
	add	x15, x23, x14, lsl #3
	ldr	w17, [x15, #4]
	eor	w16, w16, w12
	str	w16, [x15, #4]
	eor	w13, w13, w17
	eor	w12, w13, w12
	ldr	w13, [sp, #2100]
	add	x13, x22, x13, lsl #3
	str	w12, [x13, #4]
	add	w12, w14, #1
	cmp	w12, #209
	csinc	w13, wzr, w14, eq
	str	w13, [sp, #2096]
	ldr	w12, [sp, #2100]
	add	w14, w12, #1
	cmp	w14, #52
	csinc	w12, wzr, w12, eq
	str	w12, [sp, #2100]
	add	x8, x8, #8
	sub	x10, x10, #8
	cmp	x10, #7
	b.hi	LBB1_12
LBB1_13:
	mov	w9, #52                         ; =0x34
	mov	w10, #209                       ; =0xd1
                                        ; kill: def $w13 killed $w13 killed $x13 def $x13
LBB1_14:                                ; =>This Inner Loop Header: Depth=1
	cmp	w13, #12
	csel	w11, w10, wzr, lo
	add	w11, w13, w11
	sub	w11, w11, #12
	cmp	w13, #150
	csel	w14, w10, wzr, lo
	add	w14, w13, w14
	sub	w14, w14, #150
	add	x11, x23, w11, uxtw #3
	add	x14, x23, w14, uxtw #3
	add	x12, x22, w12, uxtw #3
	ldp	w15, w16, [x12]
	ldp	w17, w11, [x11]
	eor	w15, w17, w15
	ubfiz	x13, x13, #3, #32
	ldr	w17, [x23, x13]
	ldp	w0, w14, [x14]
	eor	w0, w0, w15
	str	w0, [x23, x13]
	eor	w13, w17, w15
	str	w13, [x12]
	eor	w11, w11, w16
	ldr	w12, [sp, #2096]
	add	x13, x23, x12, lsl #3
	ldr	w15, [x13, #4]
	eor	w14, w14, w11
	str	w14, [x13, #4]
	eor	w11, w15, w11
	ldr	w13, [sp, #2100]
	add	x13, x22, x13, lsl #3
	str	w11, [x13, #4]
	add	w11, w12, #1
	cmp	w11, #209
	csinc	w13, wzr, w12, eq
	str	w13, [sp, #2096]
	ldr	w11, [sp, #2100]
	add	w12, w11, #1
	cmp	w12, #52
	csinc	w12, wzr, w11, eq
	str	w12, [sp, #2100]
	subs	w9, w9, #1
	b.ne	LBB1_14
; %bb.15:
	str	x19, [sp]                       ; 8-byte Spill
	mov	w9, #1                          ; =0x1
	mov	w10, #-1                        ; =0xffffffff
	mov	w11, #52                        ; =0x34
	mov	w12, #209                       ; =0xd1
	add	x13, sp, #8
LBB1_16:                                ; =>This Inner Loop Header: Depth=1
	ldr	w14, [sp, #2100]
	cmp	w14, w9
	csel	w15, w11, wzr, lo
	add	w14, w10, w14
	add	w14, w14, w15
	ldr	w15, [sp, #2096]
	cmp	w15, w9
	csel	w16, w12, wzr, lo
	add	w17, w10, w15
	add	w16, w17, w16
	add	w0, w9, #150
	cmp	w15, w0
	csel	w15, w12, wzr, lo
	add	w15, w17, w15
	sub	w15, w15, #150
	ldr	d0, [x13, w16, uxtw #3]
	ldr	d1, [x13, w15, uxtw #3]
	ldr	d2, [x22, w14, uxtw #3]
	eor3.16b	v0, v1, v0, v2
	str	d0, [x22, w14, uxtw #3]
	sub	w10, w10, #1
	add	w9, w9, #1
	cmp	w9, #53
	b.ne	LBB1_16
; %bb.17:
	mov	x10, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	ldr	w11, [sp, #2096]
	sub	w12, w11, #53
	sub	w13, w11, #151
	ldr	w14, [sp, #2100]
	sub	w15, w11, #203
	sub	w16, w14, #53
	sub	w17, w11, #1
	mov	w0, #-248                       ; =0xffffff08
	mov	w1, #261                        ; =0x105
	mov	w2, #1                          ; =0x1
	mov	w3, #209                        ; =0xd1
	add	x4, sp, #8
	mov	w5, #53                         ; =0x35
	mov	w6, #151                        ; =0x97
	mov	w7, #203                        ; =0xcb
	mov	w21, #52                        ; =0x34
Lloh12:
	adrp	x23, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGE
Lloh13:
	add	x23, x23, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGEOFF
	b	LBB1_21
LBB1_18:                                ;   in Loop: Header=BB1_21 Depth=1
	movi.2d	v0, #0000000000000000
LBB1_19:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	x2, x11
	csel	x24, x3, xzr, hi
	add	w25, w17, w10
	add	w24, w25, w24
	add	x25, sp, #8
LBB1_20:                                ;   in Loop: Header=BB1_21 Depth=1
	add	x24, x25, w24, uxtw #3
	mov.s	w25, v0[1]
	ldp	w24, w26, [x24]
	fmov	w27, s0
	eor	w24, w24, w27
	and	w27, w9, #0xff
	ldr	w27, [x23, w27, uxtw #2]
	eor	w25, w25, w27
	ubfx	w27, w9, #8, #8
	add	x28, x23, #1024
	ldr	w27, [x28, w27, uxtw #2]
	eor	w25, w25, w27
	ubfx	w27, w9, #16, #8
	add	x30, x23, #2048
	ldr	w27, [x30, w27, uxtw #2]
	lsr	w9, w9, #24
	add	x19, x23, #3072
	ldr	w9, [x19, w9, uxtw #2]
	eor	w25, w26, w25
	eor	w9, w27, w9
	eor	w9, w25, w9
	and	w25, w9, #0xff
	ldr	w25, [x23, w25, uxtw #2]
	ubfx	w26, w9, #8, #8
	ldr	w26, [x28, w26, uxtw #2]
	ubfx	w27, w9, #16, #8
	ldr	w27, [x30, w27, uxtw #2]
	eor	w24, w24, w25
	eor	w25, w26, w27
	eor	w24, w24, w25
	lsr	w9, w9, #24
	ldr	w9, [x19, w9, uxtw #2]
	eor	w9, w24, w9
	sub	x10, x10, #1
	add	w0, w0, #1
	add	x2, x2, #1
	sub	x1, x1, #1
	cmn	x10, #261
	b.eq	LBB1_33
LBB1_21:                                ; =>This Inner Loop Header: Depth=1
	add	x24, x10, #261
	cmp	x24, #12
	b.hi	LBB1_23
; %bb.22:                               ;   in Loop: Header=BB1_21 Depth=1
	cmp	w11, w0
	csel	w25, w3, wzr, lo
	add	w26, w11, w10
	add	w25, w26, w25
	add	w25, w25, #248
	ldr	d0, [x4, w25, uxtw #3]
	mov	x25, x1
	b	LBB1_25
LBB1_23:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	x24, #52
	b.hi	LBB1_29
; %bb.24:                               ;   in Loop: Header=BB1_21 Depth=1
	add	w25, w10, #261
	movi.2d	v0, #0000000000000000
LBB1_25:                                ;   in Loop: Header=BB1_21 Depth=1
	sub	w26, w5, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w27, w25, w12
	add	w26, w27, w26
	ldr	d1, [x4, w26, uxtw #3]
	eor.8b	v0, v1, v0
LBB1_26:                                ;   in Loop: Header=BB1_21 Depth=1
	sub	w26, w6, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w27, w25, w13
	add	w26, w27, w26
	ldr	d1, [x4, w26, uxtw #3]
	eor.8b	v0, v1, v0
LBB1_27:                                ;   in Loop: Header=BB1_21 Depth=1
	sub	w26, w7, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w25, w25, w15
	add	w25, w25, w26
	ldr	d1, [x4, w25, uxtw #3]
	eor.8b	v0, v1, v0
	cmp	x24, #52
	b.hi	LBB1_19
; %bb.28:                               ;   in Loop: Header=BB1_21 Depth=1
	add	w24, w0, #40
	cmp	w14, w24
	csel	w24, w21, wzr, lo
	add	w25, w16, w1
	add	w24, w25, w24
	mov	x25, x22
	b	LBB1_20
LBB1_29:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	x24, #150
	b.hi	LBB1_31
; %bb.30:                               ;   in Loop: Header=BB1_21 Depth=1
	add	w25, w10, #261
	movi.2d	v0, #0000000000000000
	b	LBB1_26
LBB1_31:                                ;   in Loop: Header=BB1_21 Depth=1
	cmp	x24, #202
	b.hi	LBB1_18
; %bb.32:                               ;   in Loop: Header=BB1_21 Depth=1
	add	w25, w10, #261
	movi.2d	v0, #0000000000000000
	b	LBB1_27
LBB1_33:
	subs	x10, x20, x8
	b.ls	LBB1_36
; %bb.34:
	mov	w11, #33568                     ; =0x8320
	movk	w11, #60856, lsl #16
	mov	w12, #16784                     ; =0x4190
	movk	w12, #30428, lsl #16
Lloh14:
	adrp	x13, lCPI1_0@PAGE
Lloh15:
	ldr	q0, [x13, lCPI1_0@PAGEOFF]
Lloh16:
	adrp	x13, lCPI1_1@PAGE
Lloh17:
	ldr	q1, [x13, lCPI1_1@PAGEOFF]
	ldr	x13, [sp]                       ; 8-byte Reload
	add	x8, x13, x8
LBB1_35:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x8], #1
	eor	w9, w9, w13
	sbfx	w13, w9, #0, #1
	and	w13, w13, w11
	eor	w13, w13, w9, lsr #1
	lsl	w14, w9, #30
	and	w14, w11, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w13, #26
	and	w9, w12, w9, asr #31
	lsl	w13, w14, #26
	and	w13, w11, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w9, w15, w9
	eor	w9, w9, w16
	eor	w13, w13, w14, lsr #6
	eor	w9, w9, w13
	subs	x10, x10, #1
	b.ne	LBB1_35
LBB1_36:
	mvn	w0, w9
	add	sp, sp, #2112
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh10, Lloh11
	.loh AdrpAdrp	Lloh8, Lloh10
	.loh AdrpLdr	Lloh8, Lloh9
	.loh AdrpAdd	Lloh12, Lloh13
	.loh AdrpLdr	Lloh16, Lloh17
	.loh AdrpAdrp	Lloh14, Lloh16
	.loh AdrpLdr	Lloh14, Lloh15
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI2_0:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI2_1:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	add	x29, sp, #80
	sub	sp, sp, #1, lsl #12             ; =4096
	sub	sp, sp, #96
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
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #15
	b.hi	LBB2_4
; %bb.1:
	mvn	w9, w21
	cbz	x20, LBB2_30
; %bb.2:
	mov	w8, #33568                      ; =0x8320
	movk	w8, #60856, lsl #16
	mov	w10, #16784                     ; =0x4190
	movk	w10, #30428, lsl #16
Lloh18:
	adrp	x11, lCPI2_0@PAGE
Lloh19:
	ldr	q0, [x11, lCPI2_0@PAGEOFF]
Lloh20:
	adrp	x11, lCPI2_1@PAGE
Lloh21:
	ldr	q1, [x11, lCPI2_1@PAGEOFF]
LBB2_3:                                 ; =>This Inner Loop Header: Depth=1
	ldrb	w11, [x19], #1
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	subs	x20, x20, #1
	b.ne	LBB2_3
	b	LBB2_30
LBB2_4:
	mov	x23, sp
	add	x22, x23, #3344
	mov	x0, sp
	mov	w1, #4184                       ; =0x1058
	bl	_bzero
	ldr	w8, [x19]
	ldr	w9, [x19, #12]
	eon	w8, w21, w8
	add	x10, x23, #3348
	ldur	d0, [x19, #4]
	str	xzr, [sp, #8]
	str	wzr, [sp, #4]
	str	d0, [x10]
	str	w8, [sp, #3344]
	str	w9, [sp, #3356]
	movi.2s	v0, #1
	str	d0, [sp, #4176]
	cmp	x20, #32
	b.hs	LBB2_6
; %bb.5:
	mov	w12, #1                         ; =0x1
	mov	w8, #16                         ; =0x10
	mov	w13, #1                         ; =0x1
	b	LBB2_8
LBB2_6:
	add	x9, x19, #24
	sub	x10, x20, #16
	mov	w12, #1                         ; =0x1
	mov	w8, #16                         ; =0x10
	mov	w11, #209                       ; =0xd1
	mov	w13, #1                         ; =0x1
LBB2_7:                                 ; =>This Inner Loop Header: Depth=1
	ldp	w14, w15, [x9, #-8]
	cmp	w13, #12
	csel	w16, w11, wzr, lo
	add	w16, w13, w16
	sub	w16, w16, #12
	cmp	w13, #150
	csel	w17, w11, wzr, lo
	add	w17, w13, w17
	sub	w17, w17, #150
	add	x16, x23, w16, uxtw #4
	add	x17, x23, w17, uxtw #4
	add	x12, x22, w12, uxtw #4
	ldp	w0, w1, [x12]
	ldp	w2, w3, [x16]
	eor	w0, w2, w0
	ubfiz	x13, x13, #4, #32
	ldr	w2, [x23, x13]
	eor	w14, w14, w2
	ldp	w2, w4, [x17]
	eor	w2, w2, w0
	eor	w14, w14, w0
	eor	w0, w3, w1
	eor	w1, w4, w0
	ldr	d0, [x9], #16
	str	w2, [x23, x13]
	str	w14, [x12]
	ldr	w12, [sp, #4176]
	add	x13, x23, x12, lsl #4
	ldr	w14, [x13, #4]
	str	w1, [x13, #4]
	eor	w14, w15, w14
	eor	w14, w14, w0
	ldr	w15, [sp, #4180]
	add	x15, x22, x15, lsl #4
	str	w14, [x15, #4]
	ldr	w14, [sp, #4180]
	add	x15, x22, x14, lsl #4
	ldr	d1, [x15, #8]
	ldr	d2, [x16, #8]
	eor.8b	v3, v2, v1
	ldr	d4, [x13, #8]
	ldr	d5, [x17, #8]
	eor3.16b	v1, v2, v1, v5
	str	d1, [x13, #8]
	eor3.16b	v0, v0, v4, v3
	str	d0, [x15, #8]
	add	w13, w12, #1
	cmp	w13, #209
	csinc	w13, wzr, w12, eq
	str	w13, [sp, #4176]
	add	w12, w14, #1
	cmp	w12, #52
	csinc	w12, wzr, w14, eq
	str	w12, [sp, #4180]
	add	x8, x8, #16
	sub	x10, x10, #16
	cmp	x10, #15
	b.hi	LBB2_7
LBB2_8:
	mov	w9, #52                         ; =0x34
	mov	w10, #209                       ; =0xd1
                                        ; kill: def $w13 killed $w13 killed $x13 def $x13
LBB2_9:                                 ; =>This Inner Loop Header: Depth=1
	cmp	w13, #12
	csel	w11, w10, wzr, lo
	add	w11, w13, w11
	sub	w11, w11, #12
	cmp	w13, #150
	csel	w14, w10, wzr, lo
	add	w14, w13, w14
	sub	w14, w14, #150
	add	x11, x23, w11, uxtw #4
	add	x14, x23, w14, uxtw #4
	add	x12, x22, w12, uxtw #4
	ldp	w15, w16, [x12]
	ldp	w17, w0, [x11]
	eor	w15, w17, w15
	ubfiz	x13, x13, #4, #32
	ldr	w17, [x23, x13]
	ldp	w1, w2, [x14]
	eor	w1, w1, w15
	str	w1, [x23, x13]
	eor	w13, w17, w15
	str	w13, [x12]
	eor	w12, w0, w16
	ldr	w13, [sp, #4176]
	add	x15, x23, x13, lsl #4
	ldr	w16, [x15, #4]
	eor	w17, w2, w12
	str	w17, [x15, #4]
	eor	w12, w16, w12
	ldr	w16, [sp, #4180]
	add	x16, x22, x16, lsl #4
	str	w12, [x16, #4]
	ldr	w12, [sp, #4180]
	add	x16, x22, x12, lsl #4
	ldr	d0, [x16, #8]
	ldr	d1, [x11, #8]
	ldr	d2, [x15, #8]
	ldr	d3, [x14, #8]
	eor3.16b	v3, v1, v0, v3
	str	d3, [x15, #8]
	eor3.16b	v0, v1, v0, v2
	str	d0, [x16, #8]
	add	w11, w13, #1
	cmp	w11, #209
	csinc	w13, wzr, w13, eq
	str	w13, [sp, #4176]
	add	w11, w12, #1
	cmp	w11, #52
	csinc	w12, wzr, w12, eq
	str	w12, [sp, #4180]
	subs	w9, w9, #1
	b.ne	LBB2_9
; %bb.10:
	mov	w9, #1                          ; =0x1
	mov	w10, #-1                        ; =0xffffffff
	mov	w11, #52                        ; =0x34
	mov	w12, #209                       ; =0xd1
	mov	x13, sp
LBB2_11:                                ; =>This Inner Loop Header: Depth=1
	ldr	w14, [sp, #4180]
	cmp	w14, w9
	csel	w15, w11, wzr, lo
	add	w14, w10, w14
	add	w14, w14, w15
	ldr	w15, [sp, #4176]
	cmp	w15, w9
	csel	w16, w12, wzr, lo
	add	w17, w10, w15
	add	w16, w17, w16
	add	w0, w9, #150
	cmp	w15, w0
	csel	w15, w12, wzr, lo
	add	w15, w17, w15
	sub	w15, w15, #150
	ubfiz	x14, x14, #4, #32
	ldr	q0, [x13, w16, uxtw #4]
	ldr	q1, [x13, w15, uxtw #4]
	ldr	q2, [x22, x14]
	eor3.16b	v0, v1, v0, v2
	str	q0, [x22, x14]
	sub	w10, w10, #1
	add	w9, w9, #1
	cmp	w9, #53
	b.ne	LBB2_11
; %bb.12:
	mov	x10, #0                         ; =0x0
	mov	w9, #0                          ; =0x0
	ldr	w11, [sp, #4176]
	sub	w12, w11, #53
	sub	w13, w11, #151
	ldr	w14, [sp, #4180]
	sub	w15, w11, #203
	sub	w17, w11, #1
	mov	w0, #-248                       ; =0xffffff08
	mov	w1, #261                        ; =0x105
	mov	w2, #1                          ; =0x1
	mov	w3, #209                        ; =0xd1
	mov	x4, sp
	mov	w5, #53                         ; =0x35
	mov	w6, #151                        ; =0x97
	mov	w7, #203                        ; =0xcb
Lloh22:
	adrp	x23, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGE
Lloh23:
	add	x23, x23, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGEOFF
	b	LBB2_15
LBB2_13:                                ;   in Loop: Header=BB2_15 Depth=1
	cmp	x2, x11
	csel	x24, x3, xzr, hi
	add	w25, w17, w10
	add	w24, w25, w24
	mov	x25, sp
LBB2_14:                                ;   in Loop: Header=BB2_15 Depth=1
	add	x24, x25, w24, uxtw #4
	mov.s	w25, v0[3]
	ldp	w27, w26, [x24, #8]
	mov.s	w28, v0[2]
	eor	w27, w27, w28
	mov.s	w28, v0[1]
	ldp	w24, w30, [x24]
	eor	w28, w30, w28
	fmov	w30, s0
	eor	w24, w24, w30
	and	w30, w9, #0xff
	ldr	w30, [x23, w30, uxtw #2]
	ubfx	w16, w9, #8, #8
	add	x21, x23, #1024
	ldr	w16, [x21, w16, uxtw #2]
	eor	w25, w25, w30
	eor	w16, w25, w16
	ubfx	w25, w9, #16, #8
	add	x30, x23, #2048
	ldr	w25, [x30, w25, uxtw #2]
	eor	w16, w26, w16
	lsr	w9, w9, #24
	add	x26, x23, #3072
	ldr	w9, [x26, w9, uxtw #2]
	eor	w9, w25, w9
	eor	w9, w16, w9
	and	w16, w9, #0xff
	ldr	w16, [x23, w16, uxtw #2]
	eor	w16, w27, w16
	ubfx	w25, w9, #8, #8
	ldr	w25, [x21, w25, uxtw #2]
	ubfx	w27, w9, #16, #8
	ldr	w27, [x30, w27, uxtw #2]
	eor	w25, w25, w27
	eor	w16, w16, w25
	lsr	w9, w9, #24
	ldr	w9, [x26, w9, uxtw #2]
	eor	w9, w16, w9
	and	w16, w9, #0xff
	ldr	w16, [x23, w16, uxtw #2]
	eor	w16, w28, w16
	ubfx	w25, w9, #8, #8
	ldr	w25, [x21, w25, uxtw #2]
	ubfx	w27, w9, #16, #8
	ldr	w27, [x30, w27, uxtw #2]
	eor	w25, w25, w27
	eor	w16, w16, w25
	lsr	w9, w9, #24
	ldr	w9, [x26, w9, uxtw #2]
	eor	w9, w16, w9
	and	w16, w9, #0xff
	ldr	w16, [x23, w16, uxtw #2]
	ubfx	w25, w9, #8, #8
	ldr	w21, [x21, w25, uxtw #2]
	ubfx	w25, w9, #16, #8
	ldr	w25, [x30, w25, uxtw #2]
	eor	w16, w24, w16
	eor	w21, w21, w25
	eor	w16, w16, w21
	lsr	w9, w9, #24
	ldr	w9, [x26, w9, uxtw #2]
	eor	w9, w16, w9
	sub	x10, x10, #1
	add	w0, w0, #1
	add	x2, x2, #1
	sub	x1, x1, #1
	cmn	x10, #261
	b.eq	LBB2_27
LBB2_15:                                ; =>This Inner Loop Header: Depth=1
	add	x24, x10, #261
	cmp	x24, #12
	b.hi	LBB2_17
; %bb.16:                               ;   in Loop: Header=BB2_15 Depth=1
	cmp	w11, w0
	csel	w25, w3, wzr, lo
	add	w26, w11, w10
	add	w25, w26, w25
	add	w25, w25, #248
	ldr	q0, [x4, w25, uxtw #4]
	mov	x25, x1
	b	LBB2_19
LBB2_17:                                ;   in Loop: Header=BB2_15 Depth=1
	cmp	x24, #52
	b.hi	LBB2_23
; %bb.18:                               ;   in Loop: Header=BB2_15 Depth=1
	add	w25, w10, #261
	movi.2d	v0, #0000000000000000
LBB2_19:                                ;   in Loop: Header=BB2_15 Depth=1
	sub	w26, w5, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w27, w25, w12
	add	w26, w27, w26
	ldr	q1, [x4, w26, uxtw #4]
	eor.16b	v0, v1, v0
LBB2_20:                                ;   in Loop: Header=BB2_15 Depth=1
	sub	w26, w6, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w27, w25, w13
	add	w26, w27, w26
	ldr	q1, [x4, w26, uxtw #4]
	eor.16b	v0, v1, v0
LBB2_21:                                ;   in Loop: Header=BB2_15 Depth=1
	sub	w26, w7, w25
	cmp	w11, w26
	csel	w26, w3, wzr, lo
	add	w25, w25, w15
	add	w25, w25, w26
	ldr	q1, [x4, w25, uxtw #4]
	eor.16b	v0, v1, v0
	cmp	x24, #52
	b.hi	LBB2_13
; %bb.22:                               ;   in Loop: Header=BB2_15 Depth=1
	add	w24, w0, #40
	cmp	w14, w24
	mov	w16, #52                        ; =0x34
	csel	w24, w16, wzr, lo
	sub	w16, w14, #53
	add	w25, w16, w1
	add	w24, w25, w24
	mov	x25, x22
	b	LBB2_14
LBB2_23:                                ;   in Loop: Header=BB2_15 Depth=1
	cmp	x24, #150
	b.hi	LBB2_25
; %bb.24:                               ;   in Loop: Header=BB2_15 Depth=1
	add	w25, w10, #261
	movi.2d	v0, #0000000000000000
	b	LBB2_20
LBB2_25:                                ;   in Loop: Header=BB2_15 Depth=1
	movi.2d	v0, #0000000000000000
	cmp	x24, #202
	b.hi	LBB2_13
; %bb.26:                               ;   in Loop: Header=BB2_15 Depth=1
	add	w25, w10, #261
	b	LBB2_21
LBB2_27:
	subs	x10, x20, x8
	b.ls	LBB2_30
; %bb.28:
	mov	w11, #33568                     ; =0x8320
	movk	w11, #60856, lsl #16
	mov	w12, #16784                     ; =0x4190
	movk	w12, #30428, lsl #16
Lloh24:
	adrp	x13, lCPI2_0@PAGE
Lloh25:
	ldr	q0, [x13, lCPI2_0@PAGEOFF]
Lloh26:
	adrp	x13, lCPI2_1@PAGE
Lloh27:
	ldr	q1, [x13, lCPI2_1@PAGEOFF]
	add	x8, x19, x8
LBB2_29:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x8], #1
	eor	w9, w9, w13
	sbfx	w13, w9, #0, #1
	and	w13, w13, w11
	eor	w13, w13, w9, lsr #1
	lsl	w14, w9, #30
	and	w14, w11, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w13, #26
	and	w9, w12, w9, asr #31
	lsl	w13, w14, #26
	and	w13, w11, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w9, w15, w9
	eor	w9, w9, w16
	eor	w13, w13, w14, lsr #6
	eor	w9, w9, w13
	subs	x10, x10, #1
	b.ne	LBB2_29
LBB2_30:
	mvn	w0, w9
	add	sp, sp, #1, lsl #12             ; =4096
	add	sp, sp, #96
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh20, Lloh21
	.loh AdrpAdrp	Lloh18, Lloh20
	.loh AdrpLdr	Lloh18, Lloh19
	.loh AdrpAdd	Lloh22, Lloh23
	.loh AdrpLdr	Lloh26, Lloh27
	.loh AdrpAdrp	Lloh24, Lloh26
	.loh AdrpLdr	Lloh24, Lloh25
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI3_0:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI3_1:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x26, x25, [sp, #-80]!           ; 16-byte Folded Spill
	stp	x24, x23, [sp, #16]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #32]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #48]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #64]             ; 16-byte Folded Spill
	add	x29, sp, #64
	sub	sp, sp, #2144
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
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #207
	b.hi	LBB3_2
; %bb.1:
	mov	x0, x19
	mov	x1, x20
	add	sp, sp, #2144
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	b	__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LBB3_2:
	mov	x22, sp
	add	x0, sp, #1308
	mov	w1, #836                        ; =0x344
	mov	x21, x2
	bl	_bzero
	movi.2d	v0, #0000000000000000
	str	q0, [sp, #1280]
	str	q0, [sp, #1264]
	str	q0, [sp, #1248]
	str	q0, [sp, #1232]
	str	q0, [sp, #1216]
	str	q0, [sp, #1200]
	str	q0, [sp, #1184]
	str	q0, [sp, #1168]
	str	q0, [sp, #1152]
	str	q0, [sp, #1136]
	str	q0, [sp, #1120]
	str	q0, [sp, #1104]
	str	q0, [sp, #1088]
	add	x23, sp, #1088
	str	wzr, [sp, #1084]
	ldr	q0, [x19]
	str	q0, [sp]
	ldr	w8, [sp]
	eon	w8, w21, w8
	str	w8, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, sp, #1088
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #16]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #16
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #32]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #32
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #48]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #48
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #64]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #64
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #80]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #80
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #96]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #96
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #112]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #112
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #128]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #128
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #144]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #144
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #160]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #160
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #176]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #176
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x19, #192]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #192
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	cmp	x20, #416
	b.lo	LBB3_8
; %bb.3:
	sub	x24, x20, #208
	mov	w21, #208                       ; =0xd0
LBB3_4:                                 ; =>This Inner Loop Header: Depth=1
	add	x25, x19, x21
	ldr	q0, [x25]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, sp, #1088
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #16]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #16
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #32]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #32
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #48]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #48
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #64]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #64
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #80]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #80
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #96]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #96
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #112]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #112
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #128]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #128
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #144]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #144
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #160]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #160
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #176]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #176
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	ldr	q0, [x25, #192]
	str	q0, [sp]
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x2, x23, #192
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	x21, x21, #208
	sub	x24, x24, #208
	cmp	x24, #207
	b.hi	LBB3_4
; %bb.5:
	add	x22, x22, #836
	sub	x23, x20, x21
	cmp	x23, #16
	b.lo	LBB3_9
LBB3_6:
	mov	w24, #0                         ; =0x0
	add	x25, sp, #1088
LBB3_7:                                 ; =>This Inner Loop Header: Depth=1
	ldr	q0, [x19, x21]
	str	q0, [sp]
	add	x2, x25, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	mov	x3, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w24, w24, #1
	add	x21, x21, #16
	sub	x23, x23, #16
	cmp	x23, #15
	b.hi	LBB3_7
	b	LBB3_10
LBB3_8:
	mov	w21, #208                       ; =0xd0
	add	x22, x22, #836
	sub	x23, x20, x21
	cmp	x23, #16
	b.hs	LBB3_6
LBB3_9:
	mov	w24, #0                         ; =0x0
LBB3_10:
	str	xzr, [sp, #1072]
	str	xzr, [sp, #1064]
	add	x23, sp, #1088
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	add	w8, w24, #1
	cmp	w8, #13
	csinc	w24, wzr, w24, eq
	add	x2, x23, w24, uxtw #4
	add	x0, sp, #1308
	add	x1, sp, #1084
	add	x3, sp, #1064
	bl	__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	mov	x0, sp
	add	x1, sp, #1308
	mov	w2, #836                        ; =0x344
	bl	_memcpy
	ldr	q0, [sp, #1088]
	ldr	q1, [sp, #1104]
	stp	q0, q1, [x22]
	ldr	q0, [sp, #1120]
	ldr	q1, [sp, #1136]
	stp	q0, q1, [x22, #32]
	ldr	q0, [sp, #1152]
	ldr	q1, [sp, #1168]
	stp	q0, q1, [x22, #64]
	ldr	q0, [sp, #1184]
	ldr	q1, [sp, #1200]
	stp	q0, q1, [x22, #96]
	ldr	q0, [sp, #1216]
	ldr	q1, [sp, #1232]
	stp	q0, q1, [x22, #128]
	ldr	q0, [sp, #1248]
	ldr	q1, [sp, #1264]
	stp	q0, q1, [x22, #160]
	ldr	q0, [sp, #1280]
	str	q0, [x22, #192]
	add	w8, w24, #1
	ldr	w9, [sp, #1084]
	str	w9, [sp, #1044]
	lsl	w9, w8, #2
	cmp	w8, #13
	csel	w8, wzr, w9, eq
	str	w8, [sp, #1048]
	mov	x0, sp
	bl	__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	subs	x8, x20, x21
	b.ls	LBB3_13
; %bb.11:
	mov	w9, #33568                      ; =0x8320
	movk	w9, #60856, lsl #16
	add	x10, x19, x21
Lloh28:
	adrp	x11, lCPI3_0@PAGE
Lloh29:
	ldr	q0, [x11, lCPI3_0@PAGEOFF]
Lloh30:
	adrp	x11, lCPI3_1@PAGE
Lloh31:
	ldr	q1, [x11, lCPI3_1@PAGEOFF]
	mov	w11, #16784                     ; =0x4190
	movk	w11, #30428, lsl #16
LBB3_12:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w12, [x10], #1
	eor	w12, w0, w12
	sbfx	w13, w12, #0, #1
	and	w13, w13, w9
	eor	w13, w13, w12, lsr #1
	lsl	w14, w12, #30
	and	w14, w9, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w12
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w12, w13, #26
	and	w12, w11, w12, asr #31
	lsl	w13, w14, #26
	and	w13, w9, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w12, w15, w12
	eor	w12, w12, w16
	eor	w13, w13, w14, lsr #6
	eor	w0, w12, w13
	subs	x8, x8, #1
	b.ne	LBB3_12
LBB3_13:
	mvn	w0, w0
	add	sp, sp, #2144
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh30, Lloh31
	.loh AdrpAdrp	Lloh28, Lloh30
	.loh AdrpLdr	Lloh28, Lloh29
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI4_0:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI4_1:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-64]!           ; 16-byte Folded Spill
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	sub	sp, sp, #1200
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w27, -56
	.cfi_offset w28, -64
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #3
	b.hi	LBB4_5
; %bb.1:
	mvn	w9, w21
	cbz	x20, LBB4_21
; %bb.2:
	mov	w8, #33568                      ; =0x8320
	movk	w8, #60856, lsl #16
	mov	w10, #16784                     ; =0x4190
	movk	w10, #30428, lsl #16
	ldrb	w11, [x19]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v1, w9
Lloh32:
	adrp	x9, lCPI4_0@PAGE
Lloh33:
	ldr	q0, [x9, lCPI4_0@PAGEOFF]
	and.16b	v1, v1, v0
	cmeq.4s	v2, v1, #0
Lloh34:
	adrp	x9, lCPI4_1@PAGE
Lloh35:
	ldr	q1, [x9, lCPI4_1@PAGEOFF]
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #1
	b.eq	LBB4_21
; %bb.3:
	ldrb	w11, [x19, #1]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w9, w13, w9
	eor	w9, w9, w14
	eor	w11, w11, w12, lsr #6
	eor	w9, w9, w11
	cmp	x20, #2
	b.eq	LBB4_21
; %bb.4:
	ldrb	w11, [x19, #2]
	eor	w9, w9, w11
	sbfx	w11, w9, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w9, lsr #1
	lsl	w12, w9, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w9
	and.16b	v0, v2, v0
	cmeq.4s	v0, v0, #0
	bic.16b	v0, v1, v0
	lsl	w9, w11, #26
	and	w9, w10, w9, asr #31
	lsl	w10, w12, #26
	and	w8, w8, w10, asr #31
	ext.16b	v1, v0, v0, #8
	eor.8b	v0, v0, v1
	fmov	x10, d0
	lsr	x11, x10, #32
	eor	w9, w10, w9
	eor	w9, w9, w11
	eor	w8, w8, w12, lsr #6
	eor	w9, w9, w8
	b	LBB4_21
LBB4_5:
	mov	x22, sp
	add	x0, x22, #4
	mov	w1, #1196                       ; =0x4ac
	bl	_bzero
	ldr	w8, [x19]
	eon	w8, w21, w8
	str	w8, [sp]
	and	x8, x20, #0xfffffffffffffffc
	cmp	x8, #4
	b.ne	LBB4_7
; %bb.6:
	mov	w10, #1                         ; =0x1
	b	LBB4_9
LBB4_7:
	sub	x9, x20, #4
	mov	w10, #1                         ; =0x1
	mov	w8, #4                          ; =0x4
	mov	w11, #300                       ; =0x12c
LBB4_8:                                 ; =>This Inner Loop Header: Depth=1
	cmp	w10, #145
	csel	w12, w11, wzr, lo
	add	w12, w10, w12
	sub	w12, w12, #145
	cmp	w10, #183
	csel	w13, w11, wzr, lo
	add	w13, w10, w13
	sub	w13, w13, #183
	cmp	w10, #211
	csel	w14, w11, wzr, lo
	add	w14, w10, w14
	sub	w14, w14, #211
	ldr	w15, [x19, x8]
	ldr	w16, [x22, w10, uxtw #2]
	ldr	w12, [x22, w12, uxtw #2]
	ldr	w13, [x22, w13, uxtw #2]
	ldr	w14, [x22, w14, uxtw #2]
	eor	w15, w15, w16
	eor	w12, w15, w12
	eor	w13, w13, w14
	eor	w12, w12, w13
	str	w12, [x22, w10, uxtw #2]
	add	w12, w10, #1
	cmp	w12, #300
	csinc	w10, wzr, w10, eq
	add	x8, x8, #4
	sub	x9, x9, #4
	cmp	x9, #3
	b.hi	LBB4_8
LBB4_9:
	mov	w9, #0                          ; =0x0
	mov	w11, #300                       ; =0x12c
	mov	w12, #1                         ; =0x1
	mov	w14, #300                       ; =0x12c
Lloh36:
	adrp	x13, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGE
Lloh37:
	add	x13, x13, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGEOFF
	b	LBB4_14
LBB4_10:                                ;   in Loop: Header=BB4_14 Depth=1
	sub	w15, w12, #155
	cmp	w10, w15
	csel	w15, w11, wzr, lo
	add	w16, w10, w14
	add	w15, w16, w15
	sub	w15, w15, #146
	ldr	w15, [x22, w15, uxtw #2]
LBB4_11:                                ;   in Loop: Header=BB4_14 Depth=1
	sub	w16, w12, #117
	cmp	w10, w16
	csel	w16, w11, wzr, lo
	add	w17, w10, w14
	add	w16, w17, w16
	sub	w16, w16, #184
	ldr	w16, [x22, w16, uxtw #2]
	eor	w15, w16, w15
LBB4_12:                                ;   in Loop: Header=BB4_14 Depth=1
	sub	w16, w12, #89
	cmp	w10, w16
	csel	w16, w11, wzr, lo
	add	w17, w10, w14
	add	w16, w17, w16
	sub	w16, w16, #212
	ldr	w16, [x22, w16, uxtw #2]
	eor	w16, w16, w15
LBB4_13:                                ;   in Loop: Header=BB4_14 Depth=1
	cmp	w10, w12
	csel	w15, w11, wzr, lo
	add	w17, w10, w14
	add	w15, w17, w15
	sub	w15, w15, #301
	ldr	w15, [x22, w15, uxtw #2]
	eor	w15, w15, w16
	and	w16, w9, #0xff
	ldr	w16, [x13, w16, uxtw #2]
	ubfx	w17, w9, #8, #8
	add	x17, x13, w17, uxtw #2
	ldr	w17, [x17, #1024]
	ubfx	w0, w9, #16, #8
	add	x0, x13, w0, uxtw #2
	ldr	w0, [x0, #2048]
	lsr	w9, w9, #24
	add	x9, x13, w9, uxtw #2
	ldr	w9, [x9, #3072]
	eor	w16, w16, w17
	eor	w9, w0, w9
	eor	w9, w16, w9
	eor	w9, w9, w15
	add	w12, w12, #1
	sub	w15, w14, #1
	mov	x14, x15
	cbz	w15, LBB4_18
LBB4_14:                                ; =>This Inner Loop Header: Depth=1
	cmp	w14, #145
	b.le	LBB4_10
; %bb.15:                               ;   in Loop: Header=BB4_14 Depth=1
	cmp	w14, #183
	b.hi	LBB4_17
; %bb.16:                               ;   in Loop: Header=BB4_14 Depth=1
	mov	w15, #0                         ; =0x0
	b	LBB4_11
LBB4_17:                                ;   in Loop: Header=BB4_14 Depth=1
	mov	w15, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
	cmp	w14, #211
	b.ls	LBB4_12
	b	LBB4_13
LBB4_18:
	subs	x10, x20, x8
	b.ls	LBB4_21
; %bb.19:
	mov	w11, #33568                     ; =0x8320
	movk	w11, #60856, lsl #16
	mov	w12, #16784                     ; =0x4190
	movk	w12, #30428, lsl #16
Lloh38:
	adrp	x13, lCPI4_0@PAGE
Lloh39:
	ldr	q0, [x13, lCPI4_0@PAGEOFF]
Lloh40:
	adrp	x13, lCPI4_1@PAGE
Lloh41:
	ldr	q1, [x13, lCPI4_1@PAGEOFF]
	add	x8, x19, x8
LBB4_20:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x8], #1
	eor	w9, w9, w13
	sbfx	w13, w9, #0, #1
	and	w13, w13, w11
	eor	w13, w13, w9, lsr #1
	lsl	w14, w9, #30
	and	w14, w11, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w9
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w9, w13, #26
	and	w9, w12, w9, asr #31
	lsl	w13, w14, #26
	and	w13, w11, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w9, w15, w9
	eor	w9, w9, w16
	eor	w13, w13, w14, lsr #6
	eor	w9, w9, w13
	subs	x10, x10, #1
	b.ne	LBB4_20
LBB4_21:
	mvn	w0, w9
	add	sp, sp, #1200
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #64             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh34, Lloh35
	.loh AdrpAdrp	Lloh32, Lloh34
	.loh AdrpLdr	Lloh32, Lloh33
	.loh AdrpAdd	Lloh36, Lloh37
	.loh AdrpLdr	Lloh40, Lloh41
	.loh AdrpAdrp	Lloh38, Lloh40
	.loh AdrpLdr	Lloh38, Lloh39
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
lCPI5_0:
	.long	8                               ; 0x8
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	32                              ; 0x20
lCPI5_1:
	.long	249268274                       ; 0xedb8832
	.long	124634137                       ; 0x76dc419
	.long	498536548                       ; 0x1db71064
	.long	997073096                       ; 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2
__ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ; @_ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-64]!           ; 16-byte Folded Spill
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	sub	sp, sp, #1, lsl #12             ; =4096
	sub	sp, sp, #704
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w27, -56
	.cfi_offset w28, -64
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
	cmp	x1, #15
	b.hi	LBB5_4
; %bb.1:
	mvn	w10, w21
	cbz	x20, LBB5_19
; %bb.2:
	mov	w8, #33568                      ; =0x8320
	movk	w8, #60856, lsl #16
	mov	w9, #16784                      ; =0x4190
	movk	w9, #30428, lsl #16
Lloh42:
	adrp	x11, lCPI5_0@PAGE
Lloh43:
	ldr	q0, [x11, lCPI5_0@PAGEOFF]
Lloh44:
	adrp	x11, lCPI5_1@PAGE
Lloh45:
	ldr	q1, [x11, lCPI5_1@PAGEOFF]
LBB5_3:                                 ; =>This Inner Loop Header: Depth=1
	ldrb	w11, [x19], #1
	eor	w10, w10, w11
	sbfx	w11, w10, #0, #1
	and	w11, w11, w8
	eor	w11, w11, w10, lsr #1
	lsl	w12, w10, #30
	and	w12, w8, w12, asr #31
	eor	w12, w12, w11, lsr #1
	dup.4s	v2, w10
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w10, w11, #26
	and	w10, w9, w10, asr #31
	lsl	w11, w12, #26
	and	w11, w8, w11, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x13, d2
	lsr	x14, x13, #32
	eor	w10, w13, w10
	eor	w10, w10, w14
	eor	w11, w11, w12, lsr #6
	eor	w10, w10, w11
	subs	x20, x20, #1
	b.ne	LBB5_3
	b	LBB5_19
LBB5_4:
	mov	x22, sp
	add	x0, x22, #16
	mov	w1, #4784                       ; =0x12b0
	bl	_bzero
	ldr	w8, [x19]
	ldr	w9, [x19, #12]
	eon	w8, w21, w8
	str	w8, [sp]
	ldur	d0, [x19, #4]
	stur	d0, [sp, #4]
	str	w9, [sp, #12]
	and	x8, x20, #0xfffffffffffffff0
	cmp	x8, #16
	b.ne	LBB5_6
; %bb.5:
	mov	w9, #1                          ; =0x1
	b	LBB5_8
LBB5_6:
	sub	x10, x20, #16
	mov	w9, #1                          ; =0x1
	mov	w8, #16                         ; =0x10
	mov	w11, #300                       ; =0x12c
LBB5_7:                                 ; =>This Inner Loop Header: Depth=1
	cmp	w9, #145
	csel	w12, w11, wzr, lo
	add	w12, w9, w12
	sub	w12, w12, #145
	cmp	w9, #183
	csel	w13, w11, wzr, lo
	add	w13, w9, w13
	sub	w13, w13, #183
	cmp	w9, #211
	csel	w14, w11, wzr, lo
	add	w14, w9, w14
	sub	w14, w14, #211
	ubfiz	x15, x9, #4, #32
	ldr	q0, [x19, x8]
	ldr	q1, [x22, x15]
	ldr	q2, [x22, w12, uxtw #4]
	ldr	q3, [x22, w13, uxtw #4]
	ldr	q4, [x22, w14, uxtw #4]
	eor3.16b	v0, v1, v0, v2
	eor3.16b	v0, v0, v3, v4
	str	q0, [x22, x15]
	add	w12, w9, #1
	cmp	w12, #300
	csinc	w9, wzr, w9, eq
	add	x8, x8, #16
	sub	x10, x10, #16
	cmp	x10, #15
	b.hi	LBB5_7
LBB5_8:
	mov	w10, #0                         ; =0x0
	mov	w11, #300                       ; =0x12c
	mov	w12, #1                         ; =0x1
	mov	w14, #300                       ; =0x12c
Lloh46:
	adrp	x13, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGE
Lloh47:
	add	x13, x13, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGEOFF
	b	LBB5_13
LBB5_9:                                 ;   in Loop: Header=BB5_13 Depth=1
	sub	w15, w12, #155
	cmp	w9, w15
	csel	w15, w11, wzr, lo
	add	w16, w9, w14
	add	w15, w16, w15
	sub	w15, w15, #146
	ldr	q0, [x22, w15, uxtw #4]
LBB5_10:                                ;   in Loop: Header=BB5_13 Depth=1
	sub	w15, w12, #117
	cmp	w9, w15
	csel	w15, w11, wzr, lo
	add	w16, w9, w14
	add	w15, w16, w15
	sub	w15, w15, #184
	ldr	q1, [x22, w15, uxtw #4]
	eor.16b	v0, v0, v1
LBB5_11:                                ;   in Loop: Header=BB5_13 Depth=1
	sub	w15, w12, #89
	cmp	w9, w15
	csel	w15, w11, wzr, lo
	add	w16, w9, w14
	add	w15, w16, w15
	sub	w15, w15, #212
	ldr	q1, [x22, w15, uxtw #4]
	eor.16b	v0, v0, v1
LBB5_12:                                ;   in Loop: Header=BB5_13 Depth=1
	cmp	w9, w12
	csel	w15, w11, wzr, lo
	add	w16, w9, w14
	add	w15, w16, w15
	sub	w15, w15, #301
	mov.s	w16, v0[3]
	add	x15, x22, w15, uxtw #4
	ldp	w0, w17, [x15, #8]
	mov.s	w1, v0[2]
	eor	w0, w1, w0
	mov.s	w1, v0[1]
	ldp	w15, w2, [x15]
	eor	w1, w1, w2
	fmov	w2, s0
	eor	w15, w2, w15
	and	w2, w10, #0xff
	ldr	w2, [x13, w2, uxtw #2]
	eor	w16, w2, w16
	ubfx	w2, w10, #8, #8
	add	x3, x13, #1024
	ldr	w2, [x3, w2, uxtw #2]
	eor	w16, w16, w2
	ubfx	w2, w10, #16, #8
	add	x4, x13, #2048
	ldr	w2, [x4, w2, uxtw #2]
	eor	w16, w16, w2
	eor	w16, w16, w17
	lsr	w10, w10, #24
	add	x17, x13, #3072
	ldr	w10, [x17, w10, uxtw #2]
	eor	w10, w16, w10
	and	w16, w10, #0xff
	ldr	w16, [x13, w16, uxtw #2]
	ubfx	w2, w10, #8, #8
	ldr	w2, [x3, w2, uxtw #2]
	ubfx	w5, w10, #16, #8
	ldr	w5, [x4, w5, uxtw #2]
	eor	w16, w16, w0
	eor	w0, w2, w5
	eor	w16, w16, w0
	lsr	w10, w10, #24
	ldr	w10, [x17, w10, uxtw #2]
	eor	w10, w16, w10
	and	w16, w10, #0xff
	ldr	w16, [x13, w16, uxtw #2]
	eor	w16, w16, w1
	ubfx	w0, w10, #8, #8
	ldr	w0, [x3, w0, uxtw #2]
	ubfx	w1, w10, #16, #8
	ldr	w1, [x4, w1, uxtw #2]
	eor	w0, w0, w1
	eor	w16, w16, w0
	lsr	w10, w10, #24
	ldr	w10, [x17, w10, uxtw #2]
	eor	w10, w16, w10
	and	w16, w10, #0xff
	ldr	w16, [x13, w16, uxtw #2]
	eor	w15, w16, w15
	ubfx	w16, w10, #8, #8
	ldr	w16, [x3, w16, uxtw #2]
	ubfx	w0, w10, #16, #8
	ldr	w0, [x4, w0, uxtw #2]
	eor	w16, w16, w0
	eor	w15, w15, w16
	lsr	w10, w10, #24
	ldr	w10, [x17, w10, uxtw #2]
	eor	w10, w15, w10
	add	w12, w12, #1
	sub	w15, w14, #1
	mov	x14, x15
	cbz	w15, LBB5_16
LBB5_13:                                ; =>This Inner Loop Header: Depth=1
	cmp	w14, #145
	b.le	LBB5_9
; %bb.14:                               ;   in Loop: Header=BB5_13 Depth=1
	movi.2d	v0, #0000000000000000
	cmp	w14, #183
	b.ls	LBB5_10
; %bb.15:                               ;   in Loop: Header=BB5_13 Depth=1
	cmp	w14, #211
	b.ls	LBB5_11
	b	LBB5_12
LBB5_16:
	subs	x9, x20, x8
	b.ls	LBB5_19
; %bb.17:
	mov	w11, #33568                     ; =0x8320
	movk	w11, #60856, lsl #16
	mov	w12, #16784                     ; =0x4190
	movk	w12, #30428, lsl #16
Lloh48:
	adrp	x13, lCPI5_0@PAGE
Lloh49:
	ldr	q0, [x13, lCPI5_0@PAGEOFF]
Lloh50:
	adrp	x13, lCPI5_1@PAGE
Lloh51:
	ldr	q1, [x13, lCPI5_1@PAGEOFF]
	add	x8, x19, x8
LBB5_18:                                ; =>This Inner Loop Header: Depth=1
	ldrb	w13, [x8], #1
	eor	w10, w10, w13
	sbfx	w13, w10, #0, #1
	and	w13, w13, w11
	eor	w13, w13, w10, lsr #1
	lsl	w14, w10, #30
	and	w14, w11, w14, asr #31
	eor	w14, w14, w13, lsr #1
	dup.4s	v2, w10
	and.16b	v2, v2, v0
	cmeq.4s	v2, v2, #0
	bic.16b	v2, v1, v2
	lsl	w10, w13, #26
	and	w10, w12, w10, asr #31
	lsl	w13, w14, #26
	and	w13, w11, w13, asr #31
	ext.16b	v3, v2, v2, #8
	eor.8b	v2, v2, v3
	fmov	x15, d2
	lsr	x16, x15, #32
	eor	w10, w15, w10
	eor	w10, w10, w16
	eor	w13, w13, w14, lsr #6
	eor	w10, w10, w13
	subs	x9, x9, #1
	b.ne	LBB5_18
LBB5_19:
	mvn	w0, w10
	add	sp, sp, #1, lsl #12             ; =4096
	add	sp, sp, #704
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #64             ; 16-byte Folded Reload
	ret
	.loh AdrpLdr	Lloh44, Lloh45
	.loh AdrpAdrp	Lloh42, Lloh44
	.loh AdrpLdr	Lloh42, Lloh43
	.loh AdrpAdd	Lloh46, Lloh47
	.loh AdrpLdr	Lloh50, Lloh51
	.loh AdrpAdrp	Lloh48, Lloh50
	.loh AdrpLdr	Lloh48, Lloh49
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
__ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_: ; @_ZN15crc_suitability12_GLOBAL__N_17quartetERNSt3__15arrayIjLm209EEERjRNS2_IjLm4EEERKS6_
	.cfi_startproc
; %bb.0:
	ldr	w12, [x1]
	mov	w8, #209                        ; =0xd1
	cmp	w12, #12
	csel	w8, w8, wzr, lo
	add	w8, w12, w8
	sub	w8, w8, #12
	cmp	w8, #206
	b.hs	LBB6_2
; %bb.1:
	add	x8, x0, w8, uxtw #2
	ldp	w10, w11, [x8]
	ldr	x9, [x8, #8]
	b	LBB6_3
LBB6_2:
	ldr	w10, [x0, w8, uxtw #2]
	add	w9, w8, #1
	cmp	w9, #209
	csinc	w8, wzr, w8, eq
	ldr	w11, [x0, w8, uxtw #2]
	add	w9, w8, #1
	cmp	w9, #209
	csinc	w8, wzr, w8, eq
	ldr	w9, [x0, w8, uxtw #2]
	add	w13, w8, #1
	cmp	w13, #209
	csinc	w8, wzr, w8, eq
	ldr	w8, [x0, w8, uxtw #2]
	orr	x9, x9, x8, lsl #32
LBB6_3:
	mov	w8, #209                        ; =0xd1
	cmp	w12, #150
	csel	w8, w8, wzr, lo
	add	w8, w12, w8
	sub	w8, w8, #150
	cmp	w8, #206
	b.hs	LBB6_6
; %bb.4:
	add	x8, x0, w8, uxtw #2
	ldp	w13, w14, [x8]
	ldr	x8, [x8, #8]
	cmp	w12, #206
	b.lo	LBB6_7
LBB6_5:
	ldr	w15, [x0, x12, lsl #2]
	add	w16, w12, #1
	cmp	w16, #209
	csinc	w12, wzr, w12, eq
	ldr	w16, [x0, w12, uxtw #2]
	add	w17, w12, #1
	cmp	w17, #209
	csinc	w12, wzr, w12, eq
	ldr	w17, [x0, w12, uxtw #2]
	add	w4, w12, #1
	cmp	w4, #209
	csinc	w12, wzr, w12, eq
	ldr	w12, [x0, w12, uxtw #2]
	orr	x12, x17, x12, lsl #32
	b	LBB6_8
LBB6_6:
	ldr	w13, [x0, w8, uxtw #2]
	add	w14, w8, #1
	cmp	w14, #209
	csinc	w8, wzr, w8, eq
	ldr	w14, [x0, w8, uxtw #2]
	add	w15, w8, #1
	cmp	w15, #209
	csinc	w8, wzr, w8, eq
	ldr	w15, [x0, w8, uxtw #2]
	add	w16, w8, #1
	cmp	w16, #209
	csinc	w8, wzr, w8, eq
	ldr	w8, [x0, w8, uxtw #2]
	orr	x8, x15, x8, lsl #32
	cmp	w12, #206
	b.hs	LBB6_5
LBB6_7:
	add	x12, x0, x12, lsl #2
	ldp	w15, w16, [x12]
	ldr	x12, [x12, #8]
LBB6_8:
	ldp	w17, w4, [x2]
	eor	w10, w10, w17
	ldr	w17, [x3]
	eor	w15, w17, w15
	eor	w13, w13, w10
	eor	w10, w15, w10
	str	w10, [x2]
	eor	w11, w11, w4
	ldr	w15, [x3, #4]
	eor	w10, w14, w11
	eor	w14, w15, w16
	eor	w11, w14, w11
	str	w11, [x2, #4]
	ldp	w11, w14, [x2, #8]
	eor	w15, w11, w9
	ldr	w16, [x3, #8]
	eor	w11, w15, w8
	eor	w16, w16, w12
	eor	w15, w16, w15
	str	w15, [x2, #8]
	lsr	x9, x9, #32
	eor	w9, w14, w9
	ldr	w14, [x3, #12]
	lsr	x12, x12, #32
	eor	w12, w14, w12
	eor	w12, w12, w9
	str	w12, [x2, #12]
	ldr	w14, [x1]
	str	w13, [x0, x14, lsl #2]
	ldr	w12, [x1]
	add	w12, w12, #1
	cmp	w14, #206
	b.hs	LBB6_10
; %bb.9:
	str	w10, [x0, w12, uxtw #2]
	ldr	w10, [x1]
	add	w10, w10, #2
	str	w11, [x0, w10, uxtw #2]
	ldr	w10, [x1]
	add	w10, w10, #3
	mov	w11, #4                         ; =0x4
	b	LBB6_11
LBB6_10:
	cmp	w12, #209
	csel	w12, wzr, w12, eq
	str	w12, [x1]
	str	w10, [x0, w12, uxtw #2]
	ldr	w10, [x1]
	add	w12, w10, #1
	cmp	w12, #209
	csinc	w10, wzr, w10, eq
	str	w10, [x1]
	str	w11, [x0, w10, uxtw #2]
	ldr	w10, [x1]
	add	w11, w10, #1
	cmp	w11, #209
	csinc	w10, wzr, w10, eq
	str	w10, [x1]
	mov	w11, #1                         ; =0x1
LBB6_11:
	lsr	x8, x8, #32
	eor	w8, w9, w8
	str	w8, [x0, w10, uxtw #2]
	ldr	w8, [x1]
	add	w8, w8, w11
	cmp	w8, #209
	csel	w8, wzr, w8, eq
	str	w8, [x1]
	ret
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE: ; @_ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	.cfi_startproc
; %bb.0:
	add	x9, x0, #836
	mov	w8, #1                          ; =0x1
	mov	w10, #-1                        ; =0xffffffff
	mov	w11, #52                        ; =0x34
	mov	w12, #209                       ; =0xd1
LBB7_1:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w13, [x0, #1048]
	cmp	w13, w8
	csel	w14, w11, wzr, lo
	add	w13, w10, w13
	add	w13, w13, w14
	ldr	w14, [x0, #1044]
	cmp	w14, w8
	csel	w15, w12, wzr, lo
	add	w16, w10, w14
	add	w15, w16, w15
	add	w17, w8, #150
	cmp	w14, w17
	csel	w14, w12, wzr, lo
	add	w14, w16, w14
	ldr	w15, [x0, w15, uxtw #2]
	sub	w14, w14, #150
	ldr	w14, [x0, w14, uxtw #2]
	ldr	w16, [x9, w13, uxtw #2]
	eor	w15, w15, w16
	eor	w14, w14, w15
	str	w14, [x9, w13, uxtw #2]
	sub	w10, w10, #1
	add	w8, w8, #1
	cmp	w8, #53
	b.ne	LBB7_1
; %bb.2:
	mov	w8, #0                          ; =0x0
	ldr	w10, [x0, #1048]
	ldr	w11, [x0, #1044]
	mov	w12, #-248                      ; =0xffffff08
	mov	w14, #261                       ; =0x105
	mov	w13, #1                         ; =0x1
	mov	w15, #209                       ; =0xd1
	mov	w16, #262                       ; =0x106
	mov	w17, #52                        ; =0x34
Lloh52:
	adrp	x1, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGE
Lloh53:
	add	x1, x1, __ZN15crc_suitability12_GLOBAL__N_19reductionE@PAGEOFF
LBB7_3:                                 ; =>This Inner Loop Header: Depth=1
	cmp	x14, #12
	b.gt	LBB7_5
; %bb.4:                                ;   in Loop: Header=BB7_3 Depth=1
	cmp	w11, w12
	csel	w2, w15, wzr, lo
	add	w3, w11, w14
	add	w2, w3, w2
	sub	w2, w2, #13
	ldr	w2, [x0, w2, uxtw #2]
	b	LBB7_7
LBB7_5:                                 ;   in Loop: Header=BB7_3 Depth=1
	cmp	x14, #52
	b.hi	LBB7_11
; %bb.6:                                ;   in Loop: Header=BB7_3 Depth=1
	mov	w2, #0                          ; =0x0
LBB7_7:                                 ;   in Loop: Header=BB7_3 Depth=1
	add	w3, w12, #40
	cmp	w11, w3
	csel	w3, w15, wzr, lo
	add	w4, w11, w14
	add	w3, w4, w3
	sub	w3, w3, #53
	ldr	w3, [x0, w3, uxtw #2]
	eor	w2, w3, w2
LBB7_8:                                 ;   in Loop: Header=BB7_3 Depth=1
	add	w3, w12, #138
	cmp	w11, w3
	csel	w3, w15, wzr, lo
	add	w4, w11, w14
	add	w3, w4, w3
	sub	w3, w3, #151
	ldr	w3, [x0, w3, uxtw #2]
	eor	w3, w3, w2
LBB7_9:                                 ;   in Loop: Header=BB7_3 Depth=1
	add	w2, w12, #190
	cmp	w11, w2
	csel	w2, w15, wzr, lo
	add	w4, w11, w14
	add	w2, w4, w2
	sub	w2, w2, #203
	ldr	w2, [x0, w2, uxtw #2]
	eor	w2, w2, w3
LBB7_10:                                ;   in Loop: Header=BB7_3 Depth=1
	cmp	x13, x11
	csel	w3, w15, wzr, hi
	sub	x4, x16, x14
	sub	w4, w11, w4
	add	w3, w4, w3
	add	w4, w12, #40
	cmp	w10, w4
	csel	x4, x17, xzr, lo
	add	w5, w10, w14
	add	w4, w5, w4
	sub	w4, w4, #53
	cmp	x13, #210
	csel	x5, x0, x9, lo
	csel	w3, w3, w4, lo
	ldr	w3, [x5, w3, uxtw #2]
	and	w4, w8, #0xff
	ldr	w4, [x1, w4, uxtw #2]
	eor	w2, w3, w2
	ubfx	w3, w8, #8, #8
	add	x3, x1, w3, uxtw #2
	ldr	w3, [x3, #1024]
	ubfx	w5, w8, #16, #8
	add	x5, x1, w5, uxtw #2
	ldr	w5, [x5, #2048]
	lsr	w8, w8, #24
	add	x8, x1, w8, uxtw #2
	ldr	w8, [x8, #3072]
	eor	w3, w4, w3
	eor	w8, w5, w8
	eor	w8, w3, w8
	eor	w8, w8, w2
	add	w12, w12, #1
	add	x13, x13, #1
	sub	x2, x14, #1
	mov	x14, x2
	cbnz	x2, LBB7_3
	b	LBB7_14
LBB7_11:                                ;   in Loop: Header=BB7_3 Depth=1
	cmp	x14, #150
	b.hi	LBB7_13
; %bb.12:                               ;   in Loop: Header=BB7_3 Depth=1
	mov	w2, #0                          ; =0x0
	b	LBB7_8
LBB7_13:                                ;   in Loop: Header=BB7_3 Depth=1
	mov	w3, #0                          ; =0x0
	mov	w2, #0                          ; =0x0
	cmp	x14, #202
	b.ls	LBB7_9
	b	LBB7_10
LBB7_14:
	mov	x0, x8
	ret
	.loh AdrpAdd	Lloh52, Lloh53
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"compact261-32"

l_.str.1:                               ; @.str.1
	.asciz	"compact261-64"

l_.str.2:                               ; @.str.2
	.asciz	"compact261-128"

l_.str.3:                               ; @.str.3
	.asciz	"quartet261"

l_.str.4:                               ; @.str.4
	.asciz	"direct300-32"

l_.str.5:                               ; @.str.5
	.asciz	"direct300-128"

	.section	__DATA,__const
	.globl	__ZN15crc_suitability8circuitsE ; @_ZN15crc_suitability8circuitsE
	.p2align	3, 0x0
__ZN15crc_suitability8circuitsE:
	.quad	l_.str
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	1044                            ; 0x414
	.byte	1                               ; 0x1
	.space	7
	.quad	l_.str.1
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	2088                            ; 0x828
	.byte	1                               ; 0x1
	.space	7
	.quad	l_.str.2
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	4176                            ; 0x1050
	.byte	1                               ; 0x1
	.space	7
	.quad	l_.str.3
	.quad	__ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	2088                            ; 0x828
	.byte	1                               ; 0x1
	.space	7
	.quad	l_.str.4
	.quad	__ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	1200                            ; 0x4b0
	.byte	1                               ; 0x1
	.space	7
	.quad	l_.str.5
	.quad	__ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ; 0x1000
	.quad	4800                            ; 0x12c0
	.byte	1                               ; 0x1
	.space	7

	.section	__TEXT,__const
	.p2align	2, 0x0                          ; @_ZN15crc_suitability12_GLOBAL__N_19reductionE
__ZN15crc_suitability12_GLOBAL__N_19reductionE:
	.long	0                               ; 0x0
	.long	3681617473                      ; 0xdb710641
	.long	1838353091                      ; 0x6d930ac3
	.long	3068267650                      ; 0xb6e20c82
	.long	3676706182                      ; 0xdb261586
	.long	5706695                         ; 0x5713c7
	.long	3065323333                      ; 0xb6b51f45
	.long	1841568004                      ; 0x6dc41904
	.long	1832725837                      ; 0x6d3d2d4d
	.long	3058445068                      ; 0xb64c2b0c
	.long	11413390                        ; 0xae278e
	.long	3688833487                      ; 0xdbdf21cf
	.long	3055237323                      ; 0xb61b38cb
	.long	1835679370                      ; 0x6d6a3e8a
	.long	3683136008                      ; 0xdb883208
	.long	16331849                        ; 0xf93449
	.long	3665451674                      ; 0xda7a5a9a
	.long	17521883                        ; 0x10b5cdb
	.long	3085520985                      ; 0xb7e95059
	.long	1821922840                      ; 0x6c985618
	.long	22826780                        ; 0x15c4f1c
	.long	3660400989                      ; 0xda2d495d
	.long	1825523167                      ; 0x6ccf45df
	.long	3082699678                      ; 0xb7be439e
	.long	3074914263                      ; 0xb74777d7
	.long	1815507350                      ; 0x6c367196
	.long	3671358740                      ; 0xdad47d14
	.long	27622229                        ; 0x1a57b55
	.long	1818321489                      ; 0x6c616251
	.long	3071304720                      ; 0xb7106410
	.long	32663698                        ; 0x1f26892
	.long	3666046675                      ; 0xda836ed3
	.long	1871033205                      ; 0x6f85b375
	.long	3035936052                      ; 0xb4f4b534
	.long	35043766                        ; 0x216b9b6
	.long	3647455223                      ; 0xd967bff7
	.long	3030623987                      ; 0xb4a3a6f3
	.long	1876074674                      ; 0x6fd2a0b2
	.long	3643845680                      ; 0xd930ac30
	.long	37857905                        ; 0x241aa71
	.long	45653560                        ; 0x2b89e38
	.long	3653867641                      ; 0xd9c99879
	.long	1865127163                      ; 0x6f2b94fb
	.long	3025834682                      ; 0xb45a92ba
	.long	3651046334                      ; 0xd99e8bbe
	.long	49253887                        ; 0x2ef8dff
	.long	3020783997                      ; 0xb40d817d
	.long	1870432060                      ; 0x6f7c873c
	.long	3053447663                      ; 0xb5ffe9ef
	.long	1854861230                      ; 0x6e8eefae
	.long	3631014700                      ; 0xd86ce32c
	.long	52290925                        ; 0x31de56d
	.long	1859779689                      ; 0x6ed9fc69
	.long	3047750184                      ; 0xb5a8fa28
	.long	55244458                        ; 0x34af6aa
	.long	3627806955                      ; 0xd83bf0eb
	.long	3636642978                      ; 0xd8c2c4a2
	.long	62112483                        ; 0x3b3c2e3
	.long	3042037345                      ; 0xb551ce61
	.long	1847642144                      ; 0x6e20c820
	.long	65327396                        ; 0x3e4d124
	.long	3633698661                      ; 0xd895d765
	.long	1853348839                      ; 0x6e77dbe7
	.long	3037126054                      ; 0xb506dda6
	.long	3742066410                      ; 0xdf0b66ea
	.long	75129003                        ; 0x47a60ab
	.long	2996333609                      ; 0xb2986c29
	.long	1776904808                      ; 0x69e96a68
	.long	70087532                        ; 0x42d736c
	.long	3747378477                      ; 0xdf5c752d
	.long	1774090671                      ; 0x69be79af
	.long	2999943150                      ; 0xb2cf7fee
	.long	2989902759                      ; 0xb2364ba7
	.long	1766280678                      ; 0x69474de6
	.long	3752149348                      ; 0xdfa54164
	.long	81020709                        ; 0x4d44725
	.long	1762680353                      ; 0x69105e21
	.long	2992724064                      ; 0xb2615860
	.long	75715810                        ; 0x48354e2
	.long	3757200035                      ; 0xdff252a3
	.long	91307120                        ; 0x5713c70
	.long	3724556849                      ; 0xde003a31
	.long	1759655603                      ; 0x68e236b3
	.long	3012767986                      ; 0xb39330f2
	.long	3730254326                      ; 0xde5729f6
	.long	86388663                        ; 0x5262fb7
	.long	3015975733                      ; 0xb3c42335
	.long	1756702068                      ; 0x68b52574
	.long	1749815613                      ; 0x684c113d
	.long	3007125372                      ; 0xb33d177c
	.long	98507774                        ; 0x5df1bfe
	.long	3735952831                      ; 0xdeae1dbf
	.long	3010069691                      ; 0xb36a04bb
	.long	1746600698                      ; 0x681b02fa
	.long	3740864120                      ; 0xdef90e78
	.long	92801081                        ; 0x5880839
	.long	2962150815                      ; 0xb08ed59f
	.long	1811928030                      ; 0x6bffd3de
	.long	3709722460                      ; 0xdd1ddf5c
	.long	107796765                       ; 0x66cd91d
	.long	1806221337                      ; 0x6ba8c019
	.long	2967062104                      ; 0xb0d9c658
	.long	104581850                       ; 0x63bcada
	.long	3712666779                      ; 0xdd4acc9b
	.long	3719559378                      ; 0xddb3f8d2
	.long	113442451                       ; 0x6c2fe93
	.long	2954949137                      ; 0xb020f211
	.long	1800533072                      ; 0x6b51f450
	.long	110488916                       ; 0x695ed54
	.long	3722767125                      ; 0xdde4eb15
	.long	1795614615                      ; 0x6b06e797
	.long	2960646614                      ; 0xb077e1d6
	.long	1794412293                      ; 0x6af48f05
	.long	2978318660                      ; 0xb1858944
	.long	124224966                       ; 0x76785c6
	.long	3692462983                      ; 0xdc168387
	.long	2983369347                      ; 0xb1d29a83
	.long	1789107394                      ; 0x6aa39cc2
	.long	3695284288                      ; 0xdc419040
	.long	120624641                       ; 0x7309601
	.long	130654792                       ; 0x7c9a248
	.long	3703088137                      ; 0xdcb8a409
	.long	1784326283                      ; 0x6a5aa88b
	.long	2972430026                      ; 0xb12baeca
	.long	3706697678                      ; 0xdcefb7ce
	.long	127840655                       ; 0x79eb18f
	.long	2977742093                      ; 0xb17cbd0d
	.long	1779284812                      ; 0x6a0dbb4c
	.long	1701301141                      ; 0x6567cb95
	.long	3189165524                      ; 0xbe16cdd4
	.long	150258006                       ; 0x8f4c156
	.long	3548759831                      ; 0xd385c717
	.long	3191987731                      ; 0xbe41de13
	.long	1697699922                      ; 0x6530d852
	.long	3553809616                      ; 0xd3d2d4d0
	.long	144954001                       ; 0x8a3d291
	.long	140175064                       ; 0x85ae6d8
	.long	3542868121                      ; 0xd32be099
	.long	1707731995                      ; 0x65c9ec1b
	.long	3199789658                      ; 0xbeb8ea5a
	.long	3548181342                      ; 0xd37cf35e
	.long	135132447                       ; 0x80df51f
	.long	3203398045                      ; 0xbeeff99d
	.long	1704919004                      ; 0x659effdc
	.long	3206385935                      ; 0xbf1d910f
	.long	1684838222                      ; 0x646c974e
	.long	3532561356                      ; 0xd28e9bcc
	.long	167746957                       ; 0x9ff9d8d
	.long	1681622153                      ; 0x643b8489
	.long	3209331400                      ; 0xbf4a82c8
	.long	162041418                       ; 0x9a88e4a
	.long	3537471499                      ; 0xd2d9880b
	.long	3525360706                      ; 0xd220bc42
	.long	156350979                       ; 0x951ba03
	.long	3216225921                      ; 0xbfb3b681
	.long	1690480832                      ; 0x64c2b0c0
	.long	151431620                       ; 0x906a9c4
	.long	3531059077                      ; 0xd277af85
	.long	1687528199                      ; 0x6495a307
	.long	3219432774                      ; 0xbfe4a546
	.long	182614240                       ; 0xae278e0
	.long	3516104353                      ; 0xd1937ea1
	.long	1735488035                      ; 0x67717223
	.long	3154146402                      ; 0xbc007462
	.long	3519311206                      ; 0xd1c46d66
	.long	179661607                       ; 0xab56b27
	.long	3159844773                      ; 0xbc5767a5
	.long	1730568676                      ; 0x672661e4
	.long	1742689709                      ; 0x67df55ad
	.long	3165541356                      ; 0xbcae53ec
	.long	172777326                       ; 0xa4c5f6e
	.long	3510458671                      ; 0xd13d592f
	.long	3170451499                      ; 0xbcf9402b
	.long	1736984170                      ; 0x6788466a
	.long	3513404136                      ; 0xd16a4ae8
	.long	169561257                       ; 0xa1b4ca9
	.long	3499631226                      ; 0xd098227a
	.long	199828539                       ; 0xbe9243b
	.long	3171625145                      ; 0xbd0b28b9
	.long	1719283448                      ; 0x667a2ef8
	.long	197015548                       ; 0xbbe37fc
	.long	3503239613                      ; 0xd0cf31bd
	.long	1714240831                      ; 0x662d3d3f
	.long	3176938366                      ; 0xbd5c3b7e
	.long	3181711159                      ; 0xbda50f37
	.long	1725172086                      ; 0x66d40976
	.long	3493201396                      ; 0xd03605f4
	.long	189203381                       ; 0xb4703b5
	.long	1719868081                      ; 0x66831ab1
	.long	3186760944                      ; 0xbdf21cf0
	.long	185602162                       ; 0xb101072
	.long	3496023603                      ; 0xd0611633
	.long	3127684479                      ; 0xba6cad7f
	.long	1629334334                      ; 0x611dab3e
	.long	3623856060                      ; 0xd7ffa7bc
	.long	210674173                       ; 0xc8ea1fd
	.long	1632286969                      ; 0x614ab8f9
	.long	3124477624                      ; 0xba3bbeb8
	.long	215593530                       ; 0xcd9b23a
	.long	3618157691                      ; 0xd7a8b47b
	.long	3612442674                      ; 0xd7518032
	.long	203458163                       ; 0xc208673
	.long	3133311729                      ; 0xbac28af1
	.long	1639156912                      ; 0x61b38cb0
	.long	209163700                       ; 0xc7795b4
	.long	3607532533                      ; 0xd70693f5
	.long	1642372983                      ; 0x61e49f77
	.long	3130366262                      ; 0xba959936
	.long	1612117989                      ; 0x6016f7e5
	.long	3144151460                      ; 0xbb67f1a4
	.long	226884902                       ; 0xd85fd26
	.long	3606379367                      ; 0xd6f4fb67
	.long	3140543075                      ; 0xbb30e263
	.long	1614930978                      ; 0x6041e422
	.long	3601066144                      ; 0xd6a3e8a0
	.long	231927521                       ; 0xdd2eee1
	.long	220977832                       ; 0xd2bdaa8
	.long	3596279017                      ; 0xd65adce9
	.long	1622724715                      ; 0x60b8d06b
	.long	3150566954                      ; 0xbbc9d62a
	.long	3591229230                      ; 0xd60dcf2e
	.long	226281839                       ; 0xd7cc96f
	.long	3147744749                      ; 0xbb9ec5ed
	.long	1626325932                      ; 0x60efc3ac
	.long	3588824586                      ; 0xd5e91e0a
	.long	244848715                       ; 0xe98184b
	.long	3095008457                      ; 0xb87a14c9
	.long	1661670024                      ; 0x630b1288
	.long	248449932                       ; 0xecf0b8c
	.long	3586002381                      ; 0xd5be0dcd
	.long	1666974031                      ; 0x635c014f
	.long	3089958670                      ; 0xb82d070e
	.long	3100914503                      ; 0xb8d43347
	.long	1671771398                      ; 0x63a53506
	.long	3578214788                      ; 0xd5473984
	.long	238436293                       ; 0xe363fc5
	.long	1676814017                      ; 0x63f226c1
	.long	3095601280                      ; 0xb8832080
	.long	241249282                       ; 0xe612c02
	.long	3574606403                      ; 0xd5102a43
	.long	261309584                       ; 0xf934490
	.long	3571598033                      ; 0xd4e242d1
	.long	1644187219                      ; 0x62004e53
	.long	3111208978                      ; 0xb9714812
	.long	3568652566                      ; 0xd4b55116
	.long	264525655                       ; 0xfc45757
	.long	3106298837                      ; 0xb9265bd5
	.long	1649892756                      ; 0x62575d94
	.long	1655597533                      ; 0x62ae69dd
	.long	3118428060                      ; 0xb9df6f9c
	.long	255681310                       ; 0xf3d631e
	.long	3561776479                      ; 0xd44c655f
	.long	3112729691                      ; 0xb9887c5b
	.long	1660516890                      ; 0x62f97a1a
	.long	3558569624                      ; 0xd41b7698
	.long	258633945                       ; 0xf6a70d9
	.long	0                               ; 0x0
	.long	3402602282                      ; 0xcacf972a
	.long	1324230677                      ; 0x4eee2815
	.long	2216804159                      ; 0x8421bf3f
	.long	2648461354                      ; 0x9ddc502a
	.long	1460913920                      ; 0x5713c700
	.long	3543300159                      ; 0xd332783f
	.long	436072213                       ; 0x19fdef15
	.long	3771311637                      ; 0xe0c9a615
	.long	705048895                       ; 0x2a06313f
	.long	2921827840                      ; 0xae278e00
	.long	1692932394                      ; 0x64e8192a
	.long	2098591295                      ; 0x7d15f63f
	.long	3084542229                      ; 0xb7da6115
	.long	872144426                       ; 0x33fbde2a
	.long	4180953344                      ; 0xf9344900
	.long	451037803                       ; 0x1ae24a6b
	.long	3492666689                      ; 0xd02ddd41
	.long	1410097790                      ; 0x540c627e
	.long	2663642452                      ; 0x9ec3f554
	.long	2268994113                      ; 0x873e1a41
	.long	1307676011                      ; 0x4df18d6b
	.long	3385864788                      ; 0xc9d03254
	.long	52405630                        ; 0x31fa57e
	.long	4197182590                      ; 0xfa2bec7e
	.long	820280148                       ; 0x30e47b54
	.long	3032859755                      ; 0xb4c5c46b
	.long	2114605889                      ; 0x7e0a5341
	.long	1744288852                      ; 0x67f7bc54
	.long	2906139518                      ; 0xad382b7e
	.long	689542209                       ; 0x29199441
	.long	3822453611                      ; 0xe3d6036b
	.long	902075606                       ; 0x35c494d6
	.long	4278911996                      ; 0xff0b03fc
	.long	2066398403                      ; 0x7b2abcc3
	.long	2984586217                      ; 0xb1e52be9
	.long	2820195580                      ; 0xa818c4fc
	.long	1658278870                      ; 0x62d753d6
	.long	3874942185                      ; 0xe6f6ece9
	.long	741964739                       ; 0x2c397bc3
	.long	3574411971                      ; 0xd50d32c3
	.long	532850153                       ; 0x1fc2a5e9
	.long	2615352022                      ; 0x9be31ad6
	.long	1361874428                      ; 0x512c8dfc
	.long	1221681897                      ; 0x48d162e9
	.long	2183067075                      ; 0x821ef5c3
	.long	104811260                       ; 0x63f4afc
	.long	3438337494                      ; 0xccf0ddd6
	.long	791076541                       ; 0x2f26debd
	.long	3857271191                      ; 0xe5e94997
	.long	1640560296                      ; 0x61c8f6a8
	.long	2869387650                      ; 0xab076182
	.long	3002764951                      ; 0xb2fa8e97
	.long	2016745917                      ; 0x783519bd
	.long	4229211778                      ; 0xfc14a682
	.long	920334760                       ; 0x36db31a8
	.long	3488577704                      ; 0xcfef78a8
	.long	86044546                        ; 0x520ef82
	.long	2164347069                      ; 0x810150bd
	.long	1271842711                      ; 0x4bcec797
	.long	1379084418                      ; 0x52332882
	.long	2566700968                      ; 0x98fcbfa8
	.long	484245655                       ; 0x1cdd0097
	.long	3591542717                      ; 0xd61297bd
	.long	1804151212                      ; 0x6b8929ac
	.long	2705768070                      ; 0xa146be86
	.long	627507641                       ; 0x256701b9
	.long	4020803219                      ; 0xefa89693
	.long	4132796806                      ; 0xf6557986
	.long	1016786604                      ; 0x3c9aeeac
	.long	3099283859                      ; 0xb8bb5193
	.long	1920255673                      ; 0x7274c6b9
	.long	2336264121                      ; 0x8b408fb9
	.long	1099896979                      ; 0x418f1893
	.long	3316557740                      ; 0xc5aea7ac
	.long	258027654                       ; 0xf613086
	.long	379379603                       ; 0x169cdf93
	.long	3696445625                      ; 0xdc5348b9
	.long	1483929478                      ; 0x5872f786
	.long	2461884588                      ; 0x92bd60ac
	.long	1902863303                      ; 0x716b63c7
	.long	3148149997                      ; 0xbba4f4ed
	.long	1065700306                      ; 0x3f854bd2
	.long	4115324152                      ; 0xf54adcf8
	.long	3971429357                      ; 0xecb733ed
	.long	645440711                       ; 0x2678a4c7
	.long	2723748856                      ; 0xa2591bf8
	.long	1754696914                      ; 0x68968cd2
	.long	2443363794                      ; 0x91a2c5d2
	.long	1533891320                      ; 0x5b6d52f8
	.long	3746360775                      ; 0xdf4cedc7
	.long	360938221                       ; 0x15837aed
	.long	209622520                       ; 0xc7e95f8
	.long	3333489362                      ; 0xc6b102d2
	.long	1116782061                      ; 0x4290bded
	.long	2287938247                      ; 0x885f2ac7
	.long	1582153082                      ; 0x5e4dbd7a
	.long	2491558480                      ; 0x94822a50
	.long	279156079                       ; 0x10a3956f
	.long	3664511557                      ; 0xda6c0245
	.long	3281120592                      ; 0xc391ed50
	.long	157186682                       ; 0x95e7a7a
	.long	2373961029                      ; 0x8d7fc545
	.long	1202737775                      ; 0x47b0526f
	.long	3196328815                      ; 0xbe841b6f
	.long	1951108165                      ; 0x744b8c45
	.long	4033491834                      ; 0xf06a337a
	.long	983934032                       ; 0x3aa5a450
	.long	592988997                       ; 0x23584b45
	.long	3919043695                      ; 0xe997dc6f
	.long	1840669520                      ; 0x6db66350
	.long	2809787514                      ; 0xa779f47a
	.long	1152382737                      ; 0x44aff711
	.long	2388680763                      ; 0x8e60603b
	.long	172089092                       ; 0xa41df04
	.long	3230550062                      ; 0xc08e482e
	.long	3648235323                      ; 0xd973a73b
	.long	331100177                       ; 0x13bc3011
	.long	2543685422                      ; 0x979d8f2e
	.long	1565661188                      ; 0x5d521804
	.long	2758168836                      ; 0xa4665104
	.long	1856620078                      ; 0x6ea9c62e
	.long	3934812433                      ; 0xea887911
	.long	541584955                       ; 0x2047ee3b
	.long	968491310                       ; 0x39ba012e
	.long	4084569604                      ; 0xf3759604
	.long	2002004283                      ; 0x7754293b
	.long	3181100561                      ; 0xbd9bbe11
	.long	3608302424                      ; 0xd7125358
	.long	501073010                       ; 0x1dddc472
	.long	2583460685                      ; 0x99fc7b4d
	.long	1395911783                      ; 0x5333ec67
	.long	1255015282                      ; 0x4ace0372
	.long	2147587160                      ; 0x80019458
	.long	69217127                        ; 0x4202b67
	.long	3471817805                      ; 0xceefbc4d
	.long	937162061                       ; 0x37dbf54d
	.long	4245971559                      ; 0xfd146267
	.long	2033573208                      ; 0x7935dd58
	.long	3019524722                      ; 0xb3fa4a72
	.long	2852627815                      ; 0xaa07a567
	.long	1623732813                      ; 0x60c8324d
	.long	3840511346                      ; 0xe4e98d72
	.long	774249048                       ; 0x2e261a58
	.long	3455064371                      ; 0xcdf01933
	.long	121605657                       ; 0x73f8e19
	.long	2199793958                      ; 0x831e3126
	.long	1238476300                      ; 0x49d1a60c
	.long	1345079577                      ; 0x502c4919
	.long	2598624819                      ; 0x9ae3de33
	.long	516055308                       ; 0x1ec2610c
	.long	3557684774                      ; 0xd40df626
	.long	758759206                       ; 0x2d39bf26
	.long	3891669004                      ; 0xe7f6280c
	.long	1675073331                      ; 0x63d79733
	.long	2836922393                      ; 0xa9180019
	.long	2967858956                      ; 0xb0e5ef0c
	.long	2049603622                      ; 0x7a2a7826
	.long	4262184729                      ; 0xfe0bc719
	.long	885280819                       ; 0x34c45033
	.long	3805726606                      ; 0xe2d6c78e
	.long	672747684                       ; 0x281950a4
	.long	2889412507                      ; 0xac38ef9b
	.long	1727494321                      ; 0x66f778b1
	.long	2131400612                      ; 0x7f0a97a4
	.long	3049586830                      ; 0xb5c5008e
	.long	837074865                       ; 0x31e4bfb1
	.long	4213909659                      ; 0xfb2b289b
	.long	35611035                        ; 0x21f619b
	.long	3369137841                      ; 0xc8d0f6b1
	.long	1290881422                      ; 0x4cf1498e
	.long	2252267172                      ; 0x863edea4
	.long	2680369585                      ; 0x9fc331b1
	.long	1426892443                      ; 0x550ca69b
	.long	3509393828                      ; 0xd12d19a4
	.long	467832462                       ; 0x1be28e8e
	.long	4164193765                      ; 0xf8348de5
	.long	855317199                       ; 0x32fb1acf
	.long	3067782640                      ; 0xb6daa5f0
	.long	2081764058                      ; 0x7c1532da
	.long	1709759951                      ; 0x65e8ddcf
	.long	2938587877                      ; 0xaf274ae5
	.long	721876442                       ; 0x2b06f5da
	.long	3788071664                      ; 0xe1c962f0
	.long	419245040                       ; 0x18fd2bf0
	.long	3526540506                      ; 0xd232bcda
	.long	1444086757                      ; 0x561303e5
	.long	2631701711                      ; 0x9cdc94cf
	.long	2233564122                      ; 0x85217bda
	.long	1341058288                      ; 0x4feeecf0
	.long	3419362255                      ; 0xcbcf53cf
	.long	16827621                        ; 0x100c4e5
	.long	3164306164                      ; 0xbc9b7af4
	.long	1985277406                      ; 0x7654edde
	.long	4067775201                      ; 0xf27552e1
	.long	951764427                       ; 0x38bac5cb
	.long	558312158                       ; 0x21472ade
	.long	3951607284                      ; 0xeb88bdf4
	.long	1873347275                      ; 0x6fa902cb
	.long	2774963681                      ; 0xa56695e1
	.long	1548934369                      ; 0x5c52dce1
	.long	2526890955                      ; 0x969d4bcb
	.long	314373364                       ; 0x12bcf4f4
	.long	3631440862                      ; 0xd87363de
	.long	3247344843                      ; 0xc18e8ccb
	.long	188816353                       ; 0xb411be1
	.long	2405475550                      ; 0x8f60a4de
	.long	1169110004                      ; 0x45af33f4
	.long	2792960159                      ; 0xa679309f
	.long	1823909813                      ; 0x6cb6a7b5
	.long	3902216330                      ; 0xe897188a
	.long	576229280                       ; 0x22588fa0
	.long	1000693941                      ; 0x3ba560b5
	.long	4050319263                      ; 0xf16af79f
	.long	1967868064                      ; 0x754b48a0
	.long	3213156234                      ; 0xbf84df8a
	.long	1185977994                      ; 0x46b0968a
	.long	2357133728                      ; 0x8c7f01a0
	.long	140426911                       ; 0x85ebe9f
	.long	3264293301                      ; 0xc29129b5
	.long	3681339040                      ; 0xdb6cc6a0
	.long	295915914                       ; 0x11a3518a
	.long	2508385973                      ; 0x9582eeb5
	.long	1598912927                      ; 0x5f4d799f
	.long	2304765474                      ; 0x895fee22
	.long	1133541640                      ; 0x43907908
	.long	3350316599                      ; 0xc7b1c637
	.long	226382109                       ; 0xd7e511d
	.long	344178184                       ; 0x1483be08
	.long	3729533218                      ; 0xde4c2922
	.long	1517131293                      ; 0x5a6d961d
	.long	2426536247                      ; 0x90a20137
	.long	1771456567                      ; 0x69964837
	.long	2740576029                      ; 0xa359df1d
	.long	662200354                       ; 0x27786022
	.long	3988256520                      ; 0xedb7f708
	.long	4098496541                      ; 0xf44a181d
	.long	1048940343                      ; 0x3e858f37
	.long	3131322376                      ; 0xbaa43008
	.long	1886103330                      ; 0x706ba722
	.long	2478679113                      ; 0x93bda449
	.long	1500656483                      ; 0x59723363
	.long	3713240156                      ; 0xdd538c5c
	.long	396106614                       ; 0x179c1b76
	.long	241300579                       ; 0xe61f463
	.long	3299763017                      ; 0xc4ae6349
	.long	1083169910                      ; 0x408fdc76
	.long	2319469404                      ; 0x8a404b5c
	.long	1936982620                      ; 0x7374025c
	.long	3116078454                      ; 0xb9bb9576
	.long	1033513545                      ; 0x3d9a2a49
	.long	4149591395                      ; 0xf755bd63
	.long	4004008566                      ; 0xeea85276
	.long	610780508                       ; 0x2467c55c
	.long	2688973411                      ; 0xa0467a63
	.long	1787424073                      ; 0x6a89ed49
	.long	0                               ; 0x0
	.long	1968546033                      ; 0x7555a0f1
	.long	3937092066                      ; 0xeaab41e2
	.long	2684281107                      ; 0x9ffee113
	.long	237471109                       ; 0xe278585
	.long	2071078260                      ; 0x7b722574
	.long	3834430567                      ; 0xe48cc467
	.long	2446943382                      ; 0x91d96496
	.long	474942218                       ; 0x1c4f0b0a
	.long	1763355643                      ; 0x691aabfb
	.long	4142156520                      ; 0xf6e44ae8
	.long	2209475097                      ; 0x83b1ea19
	.long	308842127                       ; 0x12688e8f
	.long	1732062846                      ; 0x673d2e7e
	.long	4173582189                      ; 0xf8c3cf6d
	.long	2375446428                      ; 0x8d966f9c
	.long	949884436                       ; 0x389e1614
	.long	1305196261                      ; 0x4dcbb6e5
	.long	3526711286                      ; 0xd23557f6
	.long	2808149767                      ; 0xa760f707
	.long	918131601                       ; 0x36b99391
	.long	1139553120                      ; 0x43ec3360
	.long	3692221043                      ; 0xdc12d273
	.long	2840031874                      ; 0xa9477282
	.long	617684254                       ; 0x24d11d1e
	.long	1367653871                      ; 0x5184bdef
	.long	3464125692                      ; 0xce7a5cfc
	.long	3140484109                      ; 0xbb2ffc0d
	.long	720803995                       ; 0x2af6989b
	.long	1604532330                      ; 0x5fa3386a
	.long	3227375993                      ; 0xc05dd979
	.long	3037231496                      ; 0xb5087988
	.long	1899768872                      ; 0x713c2c28
	.long	74026201                        ; 0x4698cd9
	.long	2610392522                      ; 0x9b976dca
	.long	4005743931                      ; 0xeec2cd3b
	.long	2132519341                      ; 0x7f1ba9ad
	.long	172886364                       ; 0xa4e095c
	.long	2511398991                      ; 0x95b0e84f
	.long	3773122750                      ; 0xe0e548be
	.long	1836263202                      ; 0x6d732722
	.long	405178323                       ; 0x182687d3
	.long	2279106240                      ; 0x87d866c0
	.long	4069377585                      ; 0xf28dc631
	.long	1666491047                      ; 0x6354a2a7
	.long	369164886                       ; 0x16010256
	.long	2315248453                      ; 0x89ffe345
	.long	4239016884                      ; 0xfcaa43b4
	.long	1235368508                      ; 0x49a23a3c
	.long	1022859981                      ; 0x3cf79acd
	.long	2735307742                      ; 0xa3097bde
	.long	3596409647                      ; 0xd65cdb2f
	.long	1199947705                      ; 0x4785bfb9
	.long	852500296                       ; 0x32d01f48
	.long	2905538139                      ; 0xad2efe5b
	.long	3631963818                      ; 0xd87b5eaa
	.long	1441607990                      ; 0x55ed3136
	.long	548966855                       ; 0x20b891c7
	.long	3209064660                      ; 0xbf4670d4
	.long	3390296101                      ; 0xca13d025
	.long	1540011187                      ; 0x5bcab4b3
	.long	782177346                       ; 0x2e9f1442
	.long	2975987025                      ; 0xb161f551
	.long	3291764128                      ; 0xc43455a0
	.long	3799537744                      ; 0xe2785850
	.long	2536372385                      ; 0x972df8a1
	.long	148052402                       ; 0x8d319b2
	.long	2105981251                      ; 0x7d86b943
	.long	3965705685                      ; 0xec5fddd5
	.long	2567601444                      ; 0x990a7d24
	.long	116694071                       ; 0x6f49c37
	.long	1939946694                      ; 0x73a13cc6
	.long	4265038682                      ; 0xfe37535a
	.long	2338517931                      ; 0x8b62f3ab
	.long	345772728                       ; 0x149c12b8
	.long	1640608329                      ; 0x61c9b249
	.long	4027635423                      ; 0xf010d6df
	.long	2235921966                      ; 0x8545762e
	.long	448501565                       ; 0x1abb973d
	.long	1877882828                      ; 0x6fee37cc
	.long	3672526404                      ; 0xdae64e44
	.long	2947804853                      ; 0xafb3eeb5
	.long	810356646                       ; 0x304d0fa6
	.long	1159245655                      ; 0x4518af57
	.long	3569470401                      ; 0xd4c1cbc1
	.long	2710858544                      ; 0xa1946b30
	.long	1047169571                      ; 0x3e6a8a23
	.long	1262430930                      ; 0x4b3f2ad2
	.long	3332982094                      ; 0xc6a9454e
	.long	3019695551                      ; 0xb3fce5bf
	.long	738329772                       ; 0x2c0204ac
	.long	1498915933                      ; 0x5957a45d
	.long	3364798667                      ; 0xc88ec0cb
	.long	3185270842                      ; 0xbddb603a
	.long	572883241                       ; 0x22258129
	.long	1466966488                      ; 0x577021d8
	.long	2470737016                      ; 0x93447478
	.long	3859928201                      ; 0xe611d489
	.long	2045719962                      ; 0x79ef359a
	.long	213554539                       ; 0xcba956b
	.long	2640572925                      ; 0x9d63f1fd
	.long	3895873804                      ; 0xe836510c
	.long	2009640991                      ; 0x77c8b01f
	.long	43847918                        ; 0x29d10ee
	.long	2399895410                      ; 0x8f0b7f72
	.long	4200521603                      ; 0xfa5edf83
	.long	1705000592                      ; 0x65a03e90
	.long	284532321                       ; 0x10f59e61
	.long	2167208695                      ; 0x812cfaf7
	.long	4101593606                      ; 0xf4795a06
	.long	1804057365                      ; 0x6b87bb15
	.long	517086180                       ; 0x1ed21be4
	.long	2883215980                      ; 0xabda626c
	.long	3733963421                      ; 0xde8fc29d
	.long	1097933710                      ; 0x4171238e
	.long	874808191                       ; 0x3424837f
	.long	2784880617                      ; 0xa5fde7e9
	.long	3500689176                      ; 0xd0a84718
	.long	1331078667                      ; 0x4f56a60b
	.long	973276922                       ; 0x3a0306fa
	.long	3080022374                      ; 0xb7956966
	.long	3267414423                      ; 0xc2c0c997
	.long	1564354692                      ; 0x5d3e2884
	.long	678135925                       ; 0x286b8875
	.long	3115511011                      ; 0xb9b2ece3
	.long	3437710354                      ; 0xcce74c12
	.long	1394191617                      ; 0x5319ad01
	.long	642518512                       ; 0x264c0df0
	.long	528594657                       ; 0x1f81b6e1
	.long	1792284176                      ; 0x6ad41610
	.long	4113233667                      ; 0xf52af703
	.long	2155829234                      ; 0x807f57f2
	.long	296104804                       ; 0x11a63364
	.long	1693684629                      ; 0x64f39395
	.long	4211962502                      ; 0xfb0d7286
	.long	2388185719                      ; 0x8e58d277
	.long	63880683                        ; 0x3cebdeb
	.long	1989877018                      ; 0x769b1d1a
	.long	3915774985                      ; 0xe965fc09
	.long	2620415224                      ; 0x9c305cf8
	.long	233388142                       ; 0xde9386e
	.long	2025625759                      ; 0x78bc989f
	.long	3879893388                      ; 0xe742798c
	.long	2451036541                      ; 0x9217d97d
	.long	656384245                       ; 0x271fa0f5
	.long	1380581380                      ; 0x524a0004
	.long	3451183383                      ; 0xcdb4e117
	.long	3101770214                      ; 0xb8e141e6
	.long	691545456                       ; 0x29382570
	.long	1550681473                      ; 0x5c6d8581
	.long	3281216658                      ; 0xc3936492
	.long	3066479715                      ; 0xb6c6c463
	.long	995142655                       ; 0x3b50abff
	.long	1308953358                      ; 0x4e050b0e
	.long	3522947613                      ; 0xd1fbea1d
	.long	2762885868                      ; 0xa4ae4aec
	.long	897003130                       ; 0x35772e7a
	.long	1076006539                      ; 0x40228e8b
	.long	3755765656                      ; 0xdfdc6f98
	.long	2861158249                      ; 0xaa89cf69
	.long	1857919689                      ; 0x6ebd9ac9
	.long	468204088                       ; 0x1be83a38
	.long	2216090411                      ; 0x8416db2b
	.long	4047731674                      ; 0xf1437bda
	.long	1620713292                      ; 0x609a1f4c
	.long	365936573                       ; 0x15cfbfbd
	.long	2318491310                      ; 0x8a315eae
	.long	4284808799                      ; 0xff64fe5f
	.long	1928499651                      ; 0x72f291c3
	.long	128397618                       ; 0x7a73132
	.long	2556022817                      ; 0x9859d021
	.long	3977015504                      ; 0xed0c70d0
	.long	2094339142                      ; 0x7cd51446
	.long	159429815                       ; 0x980b4b7
	.long	2524861860                      ; 0x967e55a4
	.long	3811308885                      ; 0xe32bf555
	.long	1445170397                      ; 0x56238cdd
	.long	594947116                       ; 0x23762c2c
	.long	3163082047                      ; 0xbc88cd3f
	.long	3386731982                      ; 0xc9dd6dce
	.long	1476659544                      ; 0x58040958
	.long	760326569                       ; 0x2d51a9a9
	.long	2997831866                      ; 0xb2af48ba
	.long	3355109451                      ; 0xc7fae84b
	.long	1248626647                      ; 0x4a6c87d7
	.long	1060710182                      ; 0x3f392726
	.long	2697446965                      ; 0xa0c7c635
	.long	3583141572                      ; 0xd59266c4
	.long	1145766482                      ; 0x444b0252
	.long	824091299                       ; 0x311ea2a3
	.long	2933932976                      ; 0xaee043b0
	.long	3686130497                      ; 0xdbb5e341
	.long	4261015217                      ; 0xfdf9eeb1
	.long	2292993600                      ; 0x88ac4e40
	.long	391294803                       ; 0x1752af53
	.long	1644629922                      ; 0x62070fa2
	.long	4091439924                      ; 0xf3de6b34
	.long	2257308613                      ; 0x868bcbc5
	.long	427109078                       ; 0x19752ad6
	.long	1814071847                      ; 0x6c208a27
	.long	3786859963                      ; 0xe1b6e5bb
	.long	2497922378                      ; 0x94e3454a
	.long	186491993                       ; 0xb1da459
	.long	2118649000                      ; 0x7e4804a8
	.long	4019281982                      ; 0xef91603e
	.long	2596585679                      ; 0x9ac4c0cf
	.long	87695836                        ; 0x53a21dc
	.long	1886355757                      ; 0x706f812d
	.long	3311925413                      ; 0xc567f8a5
	.long	2956089428                      ; 0xb0325854
	.long	801945927                       ; 0x2fccb947
	.long	1519983030                      ; 0x5a9919b6
	.long	3410001184                      ; 0xcb407d20
	.long	3189104081                      ; 0xbe15ddd1
	.long	569064642                       ; 0x21eb3cc2
	.long	1421777971                      ; 0x54be9c33
	.long	3643339695                      ; 0xd928f3af
	.long	2893894494                      ; 0xac7d535e
	.long	864268877                       ; 0x3383b24d
	.long	1188434620                      ; 0x46d612bc
	.long	3608114730                      ; 0xd70f762a
	.long	2723862235                      ; 0xa25ad6db
	.long	1034172360                      ; 0x3da437c8
	.long	1223792441                      ; 0x48f19739
	.long	2361770649                      ; 0x8cc5c299
	.long	4186989160                      ; 0xf9906268
	.long	1718518651                      ; 0x666e837b
	.long	322642826                       ; 0x133b238a
	.long	2195867420                      ; 0x82e2471c
	.long	4156024813                      ; 0xf7b7e7ed
	.long	1749616382                      ; 0x684906fe
	.long	488416783                       ; 0x1d1ca60f
	.long	2425014675                      ; 0x908ac993
	.long	3856623970                      ; 0xe5df6962
	.long	2049017969                      ; 0x7a218871
	.long	259270784                       ; 0xf742880
	.long	2662157334                      ; 0x9ead4c16
	.long	3958959335                      ; 0xebf8ece7
	.long	1946553844                      ; 0x74060df4
	.long	22260997                        ; 0x153ad05
	.long	3025917069                      ; 0xb45bd48d
	.long	3238950012                      ; 0xc10e747c
	.long	1592825199                      ; 0x5ef0956f
	.long	732247454                       ; 0x2ba5359e
	.long	3128709384                      ; 0xba7c5108
	.long	3475632633                      ; 0xcf29f1f9
	.long	1356271850                      ; 0x50d710ea
	.long	629321755                       ; 0x2582b01b
	.long	2819940231                      ; 0xa814df87
	.long	3712057206                      ; 0xdd417f76
	.long	1119854181                      ; 0x42bf9e65
	.long	938098324                       ; 0x37ea3e94
	.long	2788383234                      ; 0xa6335a02
	.long	3546741491                      ; 0xd366faf3
	.long	1285037024                      ; 0x4c981be0
	.long	969784081                       ; 0x39cdbb11
	.long	0                               ; 0x0
	.long	1057189314                      ; 0x3f036dc2
	.long	2114378628                      ; 0x7e06db84
	.long	1090893382                      ; 0x4105b646
	.long	4228757256                      ; 0xfc0db708
	.long	3272530634                      ; 0xc30edaca
	.long	2181786764                      ; 0x820b6c8c
	.long	3171418446                      ; 0xbd08014e
	.long	594176081                       ; 0x236a6851
	.long	476644755                       ; 0x1c690593
	.long	1567405013                      ; 0x5d6cb3d5
	.long	1651498519                      ; 0x626fde17
	.long	3748126553                      ; 0xdf67df59
	.long	3764695707                      ; 0xe064b29b
	.long	2707490013                      ; 0xa16104dd
	.long	2657249567                      ; 0x9e62691f
	.long	1188352162                      ; 0x46d4d0a2
	.long	2044181856                      ; 0x79d7bd60
	.long	953289510                       ; 0x38d20b26
	.long	131163876                       ; 0x7d166e4
	.long	3134810026                      ; 0xbad967aa
	.long	2245659240                      ; 0x85da0a68
	.long	3302997038                      ; 0xc4dfbc2e
	.long	4225552876                      ; 0xfbdcd1ec
	.long	1706998003                      ; 0x65beb8f3
	.long	1522390321                      ; 0x5abdd531
	.long	465068919                       ; 0x1bb86377
	.long	616238773                       ; 0x24bb0eb5
	.long	2578649083                      ; 0x99b30ffb
	.long	2796577337                      ; 0xa6b06239
	.long	3887453311                      ; 0xe7b5d47f
	.long	3635853757                      ; 0xd8b6b9bd
	.long	2376704324                      ; 0x8da9a144
	.long	2997537926                      ; 0xb2aacc86
	.long	4088363712                      ; 0xf3af7ac0
	.long	3433830146                      ; 0xccac1702
	.long	1906579020                      ; 0x71a4164c
	.long	1319598990                      ; 0x4ea77b8e
	.long	262327752                       ; 0xfa2cdc8
	.long	815898634                       ; 0x30a1a00a
	.long	2932066581                      ; 0xaec3c915
	.long	2445321431                      ; 0x91c0a4d7
	.long	3502576273                      ; 0xd0c51291
	.long	4022763347                      ; 0xefc67f53
	.long	1389264413                      ; 0x52ce7e1d
	.long	1842156511                      ; 0x6dcd13df
	.long	751347097                       ; 0x2cc8a599
	.long	332122203                       ; 0x13cbc85b
	.long	3413996006                      ; 0xcb7d71e6
	.long	4101905444                      ; 0xf47e1c24
	.long	3044780642                      ; 0xb57baa62
	.long	2323171232                      ; 0x8a78c7a0
	.long	930137838                       ; 0x3770c6ee
	.long	141798188                       ; 0x873ab2c
	.long	1232477546                      ; 0x49761d6a
	.long	1987408040                      ; 0x767570a8
	.long	3893828023                      ; 0xe81719b7
	.long	3608441973                      ; 0xd7147475
	.long	2517746227                      ; 0x9611c233
	.long	2836574193                      ; 0xa912aff1
	.long	337292991                       ; 0x141aaebf
	.long	723108733                       ; 0x2b19c37d
	.long	1780249915                      ; 0x6a1c753b
	.long	1428101369                      ; 0x551f18f9
	.long	3223471305                      ; 0xc02244c9
	.long	4280363275                      ; 0xff21290b
	.long	3190071117                      ; 0xbe249f4d
	.long	2166878863                      ; 0x8127f28f
	.long	1009775553                      ; 0x3c2ff3c1
	.long	53255683                        ; 0x32c9e03
	.long	1109993541                      ; 0x42292845
	.long	2099922311                      ; 0x7d2a4587
	.long	3813158040                      ; 0xe3482c98
	.long	3695919450                      ; 0xdc4b415a
	.long	2639197980                      ; 0x9d4ef71c
	.long	2722994910                      ; 0xa24d9ade
	.long	524655504                       ; 0x1f459b90
	.long	541521490                       ; 0x2046f652
	.long	1631797268                      ; 0x61434014
	.long	1581264342                      ; 0x5e402dd6
	.long	2264306795                      ; 0x86f6946b
	.long	3119905193                      ; 0xb9f5f9a9
	.long	4176498671                      ; 0xf8f04fef
	.long	3354599981                      ; 0xc7f3222d
	.long	2063278947                      ; 0x7afb2363
	.long	1173900961                      ; 0x45f84ea1
	.long	83753191                        ; 0x4fdf8e7
	.long	1006540069                      ; 0x3bfe9525
	.long	2778528826                      ; 0xa59cfc3a
	.long	2594148856                      ; 0x9a9f91f8
	.long	3684313022                      ; 0xdb9a27be
	.long	3835251324                      ; 0xe4994a7c
	.long	1502694194                      ; 0x59914b32
	.long	1720854256                      ; 0x669226f0
	.long	664244406                       ; 0x279790b6
	.long	412417396                       ; 0x1894fd74
	.long	1301013901                      ; 0x4d8be58d
	.long	1921550415                      ; 0x7288884f
	.long	864894473                       ; 0x338d3e09
	.long	210654155                       ; 0xc8e53cb
	.long	2978370181                      ; 0xb1865285
	.long	2391097159                      ; 0x8e853f47
	.long	3481307393                      ; 0xcf808901
	.long	4035175619                      ; 0xf083e4c3
	.long	1860275676                      ; 0x6ee18ddc
	.long	1373823006                      ; 0x51e2e01e
	.long	283596376                       ; 0x10e75658
	.long	803486618                       ; 0x2fe43b9a
	.long	2464955092                      ; 0x92ec3ad4
	.long	2918143766                      ; 0xadef5716
	.long	3974816080                      ; 0xeceae150
	.long	3555298450                      ; 0xd3e98c92
	.long	190788911                       ; 0xb5f352f
	.long	878467309                       ; 0x345c58ed
	.long	1968828075                      ; 0x7559eeab
	.long	1247445865                      ; 0x4a5a8369
	.long	4149379623                      ; 0xf7528227
	.long	3360813029                      ; 0xc851efe5
	.long	2304006563                      ; 0x895459a3
	.long	3059168353                      ; 0xb6573461
	.long	674585982                       ; 0x28355d7e
	.long	389427388                       ; 0x173630bc
	.long	1446217466                      ; 0x563386fa
	.long	1764813624                      ; 0x6930eb38
	.long	3560499830                      ; 0xd438ea76
	.long	3946547124                      ; 0xeb3b87b4
	.long	2856202738                      ; 0xaa3e31f2
	.long	2503826480                      ; 0x953d5c30
	.long	1530236883                      ; 0x5b358fd3
	.long	1681318417                      ; 0x6436e211
	.long	624120919                       ; 0x25335457
	.long	439368085                       ; 0x1a303995
	.long	2805479643                      ; 0xa73838db
	.long	2554025241                      ; 0x983b5519
	.long	3644777311                      ; 0xd93ee35f
	.long	3862793885                      ; 0xe63d8e9d
	.long	2019551106                      ; 0x785fe782
	.long	1197247040                      ; 0x475c8a40
	.long	106511366                       ; 0x6593c06
	.long	962220484                       ; 0x395a51c4
	.long	2219987082                      ; 0x8452508a
	.long	3142663496                      ; 0xbb513d48
	.long	4199844622                      ; 0xfa548b0e
	.long	3310872268                      ; 0xc557e6cc
	.long	501309297                       ; 0x1de15f71
	.long	585249459                       ; 0x22e232b3
	.long	1676117237                      ; 0x63e784f5
	.long	1558505783                      ; 0x5ce4e937
	.long	3790399609                      ; 0xe1ece879
	.long	3740239291                      ; 0xdeef85bb
	.long	2682926077                      ; 0x9fea33fd
	.long	2699648575                      ; 0xa0e95e3f
	.long	1049311008                      ; 0x3e8b3720
	.long	25713378                        ; 0x1885ae2
	.long	1083042980                      ; 0x408deca4
	.long	2140045670                      ; 0x7f8e8166
	.long	3263594536                      ; 0xc2868028
	.long	4253412842                      ; 0xfd85edea
	.long	3162528684                      ; 0xbc805bac
	.long	2206414446                      ; 0x8383366e
	.long	3600559767                      ; 0xd69c2e97
	.long	3919528789                      ; 0xe99f4355
	.long	2828727571                      ; 0xa89af513
	.long	2543425745                      ; 0x979998d1
	.long	714185119                       ; 0x2a91999f
	.long	361952349                       ; 0x1592f45d
	.long	1419199003                      ; 0x5497421b
	.long	1804873689                      ; 0x6b942fd9
	.long	4126557894                      ; 0xf5f646c6
	.long	3405064964                      ; 0xcaf52b04
	.long	2347801922                      ; 0x8bf09d42
	.long	3035885696                      ; 0xb4f3f080
	.long	167506382                       ; 0x9fbf1ce
	.long	922262540                       ; 0x36f89c0c
	.long	2013080138                      ; 0x77fd2a4a
	.long	1224624008                      ; 0x48fe4788
	.long	2420702773                      ; 0x9048fe35
	.long	2940965879                      ; 0xaf4b93f7
	.long	3998098865                      ; 0xee4e25b1
	.long	3511502963                      ; 0xd14d4873
	.long	1816480061                      ; 0x6c45493d
	.long	1397105919                      ; 0x534624ff
	.long	306418361                       ; 0x124392b9
	.long	759234427                       ; 0x2d40ff7b
	.long	3005388388                      ; 0xb3229664
	.long	2351037350                      ; 0x8c21fba6
	.long	3441708512                      ; 0xcd244de0
	.long	4062650402                      ; 0xf2272022
	.long	1328488812                      ; 0x4f2f216c
	.long	1881951406                      ; 0x702c4cae
	.long	824834792                       ; 0x3129fae8
	.long	237672234                       ; 0xe2a972a
	.long	2602027802                      ; 0x9b17cb1a
	.long	2752816856                      ; 0xa414a6d8
	.long	3843100830                      ; 0xe511109e
	.long	3658644828                      ; 0xda127d5c
	.long	1729788946                      ; 0x671a7c12
	.long	1478037968                      ; 0x581911d0
	.long	421308310                       ; 0x191ca796
	.long	639617620                       ; 0x261fca54
	.long	3095241547                      ; 0xb87da34b
	.long	2273234569                      ; 0x877ece89
	.long	3329980623                      ; 0xc67b78cf
	.long	4185396493                      ; 0xf978150d
	.long	1148195907                      ; 0x44701443
	.long	2071165313                      ; 0x7b737981
	.long	980864967                       ; 0x3a76cfc7
	.long	91595269                        ; 0x575a205
	.long	3720551352                      ; 0xddc31bb8
	.long	3804264058                      ; 0xe2c0767a
	.long	2747646012                      ; 0xa3c5c03c
	.long	2630266366                      ; 0x9cc6adfe
	.long	567192752                       ; 0x21ceacb0
	.long	516800882                       ; 0x1ecdc172
	.long	1606973236                      ; 0x5fc87734
	.long	1623923446                      ; 0x60cb1af6
	.long	4272518121                      ; 0xfea973e9
	.long	3249151531                      ; 0xc1aa1e2b
	.long	2158995565                      ; 0x80afa86d
	.long	3215771055                      ; 0xbfacc5af
	.long	44352737                        ; 0x2a4c4e1
	.long	1034397987                      ; 0x3da7a923
	.long	2090999653                      ; 0x7ca21f65
	.long	1134654119                      ; 0x43a172a7
	.long	381577822                       ; 0x16be6a5e
	.long	700254108                       ; 0x29bd079c
	.long	1756934618                      ; 0x68b8b1da
	.long	1471929368                      ; 0x57bbdc18
	.long	3937656150                      ; 0xeab3dd56
	.long	3585126548                      ; 0xd5b0b094
	.long	2494891730                      ; 0x94b506d2
	.long	2880858896                      ; 0xabb66b10
	.long	903086607                       ; 0x35d4020f
	.long	181891021                       ; 0xad76fcd
	.long	1272109451                      ; 0x4bd2d98b
	.long	1959900233                      ; 0x74d1b449
	.long	3386488071                      ; 0xc9d9b507
	.long	4141537477                      ; 0xf6dad8c5
	.long	3084873347                      ; 0xb7df6e83
	.long	2296120129                      ; 0x88dc0341
	.long	1349171964                      ; 0x506abafc
	.long	1869207358                      ; 0x6f69d73e
	.long	778854776                       ; 0x2e6c6178
	.long	292490426                       ; 0x116f0cba
	.long	2892434932                      ; 0xac670df4
	.long	2472828982                      ; 0x93646036
	.long	3529627248                      ; 0xd261d670
	.long	3982670770                      ; 0xed62bbb2
	.long	1929433773                      ; 0x7300d2ad
	.long	1275314031                      ; 0x4c03bf6f
	.long	218499369                       ; 0xd060929
	.long	839214315                       ; 0x320564eb
	.long	2400019877                      ; 0x8f0d65a5
	.long	2953709671                      ; 0xb00e0867
	.long	4044078625                      ; 0xf10bbe21
	.long	3456685027                      ; 0xce08d3e3

.subsections_via_symbols
