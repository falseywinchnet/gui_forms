	.build_version macos, 26, 0	sdk_version 26, 2
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE ; -- Begin function _ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE
	.p2align	2
__ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE: ; @_ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE
	.cfi_startproc
; %bb.0:
	mov	w8, #5                          ; =0x5
	stp	w8, w1, [x0, #48]
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream6chargeEy ; -- Begin function _ZN8stx_zlib13DeflateStream6chargeEy
	.p2align	2
__ZN8stx_zlib13DeflateStream6chargeEy:  ; @_ZN8stx_zlib13DeflateStream6chargeEy
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #40]
	ldr	x9, [x0, #72]
	sub	x8, x8, x9
	cmp	x1, x8
	b.hi	LBB1_2
; %bb.1:
	add	x9, x9, x1
	str	x9, [x0, #72]
	cmp	x1, x8
	cset	w0, ls
	ret
LBB1_2:
	movi.2s	v0, #5
	str	d0, [x0, #48]
	cmp	x1, x8
	cset	w0, ls
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE
lCPI2_0:
	.long	1                               ; 0x1
	.long	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE
	.p2align	2
__ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE: ; @_ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE
	.cfi_startproc
; %bb.0:
	ldp	w9, w8, [x1]
	cbz	w9, LBB2_2
; %bb.1:
	cmp	w8, #4
	ccmp	w9, #1, #0, ls
	b.eq	LBB2_3
	b	LBB2_12
LBB2_2:
	cmp	w8, #4
	b.hi	LBB2_12
LBB2_3:
	ldr	w8, [x1, #8]
	sub	w8, w8, #10
	cmn	w8, #11
	b.lo	LBB2_12
; %bb.4:
	ldr	w8, [x1, #12]
	and	w8, w8, #0xfffffff8
	cmp	w8, #8
	b.ne	LBB2_12
; %bb.5:
	mov	x8, x0
	ldr	w9, [x1, #16]
	sub	w9, w9, #10
	mov	w0, #1                          ; =0x1
	cmn	w9, #9
	b.lo	LBB2_11
; %bb.6:
	ldr	x9, [x1, #24]
	cmn	x9, #1
	b.eq	LBB2_11
; %bb.7:
	ldr	x9, [x1, #32]
	cmn	x9, #1
	b.eq	LBB2_11
; %bb.8:
	ldr	x9, [x1, #40]
	cmn	x9, #1
	b.eq	LBB2_11
; %bb.9:
	mov	w0, #0                          ; =0x0
	ldp	q0, q1, [x1]
	ldr	q2, [x1, #32]
	stp	q1, q2, [x8, #16]
	str	q0, [x8]
	ldr	w9, [x1, #8]
	cmn	w9, #1
	mov	w10, #6                         ; =0x6
	csel	w9, w10, w9, eq
Lloh0:
	adrp	x10, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains@PAGE
Lloh1:
	add	x10, x10, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains@PAGEOFF
	ldr	w10, [x10, w9, uxtw #2]
Lloh2:
	adrp	x11, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice@PAGE
Lloh3:
	add	x11, x11, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice@PAGEOFF
	ldr	w11, [x11, w9, uxtw #2]
Lloh4:
	adrp	x13, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good@PAGE
Lloh5:
	add	x13, x13, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good@PAGEOFF
	ldr	w13, [x13, w9, uxtw #2]
Lloh6:
	adrp	x14, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits@PAGE
Lloh7:
	add	x14, x14, l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits@PAGEOFF
	stp	w10, w11, [x8, #92]
	ldr	w10, [x14, w9, uxtw #2]
	stp	w13, w10, [x8, #100]
	ldr	w10, [x1, #16]
	add	w10, w10, #7
	mov	w13, #1                         ; =0x1
	lsl	w10, w13, w10
	stp	w9, w10, [x8, #80]
	ldr	w14, [x1]
	ldr	w10, [x1, #12]
	cmp	w10, #8
	ccmp	w14, #0, #0, eq
	mov	w11, #9                         ; =0x9
	csel	w11, w11, w10, eq
	lsl	x10, x13, x11
	str	x10, [x8, #112]
	add	x10, x8, #196, lsl #12          ; =802816
	add	x10, x10, #1384
	str	w13, [x8, #48]
	add	x13, x8, #16, lsl #12           ; =65536
	stur	xzr, [x8, #52]
	movi.2d	v0, #0000000000000000
	stur	q0, [x8, #60]
	str	wzr, [x8, #76]
	str	q0, [x13, #120]!
	strb	wzr, [x13, #16]
	stp	xzr, xzr, [x10, #8]
	str	xzr, [x10]
Lloh8:
	adrp	x12, lCPI2_0@PAGE
Lloh9:
	ldr	d0, [x12, lCPI2_0@PAGEOFF]
	str	d0, [x10, #24]
	cbnz	w14, LBB2_11
; %bb.10:
	mov	w0, #0                          ; =0x0
	lsl	w11, w11, #12
	mov	w12, #64                        ; =0x40
	mov	w13, #192                       ; =0xc0
	mov	w14, #128                       ; =0x80
	cmp	w9, #6
	csel	w13, w14, w13, eq
	csel	w12, w12, w13, lo
	cmp	w9, #2
	csel	w9, wzr, w12, lo
	mov	w12, #-30720                    ; =0xffff8800
	add	w11, w11, w12
	orr	w12, w9, w11
	mov	w13, #4229                      ; =0x1085
	movk	w13, #2114, lsl #16
	umull	x13, w12, w13
	lsr	x13, x13, #32
	sub	w14, w12, w13
	add	w13, w13, w14, lsr #1
	add	x8, x8, #180, lsl #12           ; =737280
	add	x8, x8, #1384
	lsr	w13, w13, #4
	sub	w13, w13, w13, lsl #5
	adds	w12, w12, w13
	eor	w12, w12, #0x1f
	csel	w12, wzr, w12, eq
	orr	w9, w12, w9
	lsr	w11, w11, #8
	strb	w11, [x8]
	strb	w9, [x8, #1]
	mov	w8, #2                          ; =0x2
	str	x8, [x10]
LBB2_11:
	ret
LBB2_12:
	mov	w0, #1                          ; =0x1
	ret
	.loh AdrpLdr	Lloh8, Lloh9
	.loh AdrpAdd	Lloh6, Lloh7
	.loh AdrpAdd	Lloh4, Lloh5
	.loh AdrpAdd	Lloh2, Lloh3
	.loh AdrpAdd	Lloh0, Lloh1
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
lCPI3_0:
	.long	12                              ; 0xc
	.long	13                              ; 0xd
	.long	14                              ; 0xe
	.long	15                              ; 0xf
lCPI3_1:
	.long	8                               ; 0x8
	.long	9                               ; 0x9
	.long	10                              ; 0xa
	.long	11                              ; 0xb
lCPI3_2:
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.long	7                               ; 0x7
lCPI3_3:
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	3                               ; 0x3
lCPI3_4:
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
lCPI3_5:
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
lCPI3_6:
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
lCPI3_7:
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
	.globl	__ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
	.p2align	2
__ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE: ; @_ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
	.cfi_startproc
; %bb.0:
	cbz	x2, LBB3_19
; %bb.1:
	stp	d15, d14, [sp, #-64]!           ; 16-byte Folded Spill
	stp	d13, d12, [sp, #16]             ; 16-byte Folded Spill
	stp	d11, d10, [sp, #32]             ; 16-byte Folded Spill
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	.cfi_def_cfa_offset 64
	.cfi_offset b8, -8
	.cfi_offset b9, -16
	.cfi_offset b10, -24
	.cfi_offset b11, -32
	.cfi_offset b12, -40
	.cfi_offset b13, -48
	.cfi_offset b14, -56
	.cfi_offset b15, -64
	mov	x8, #0                          ; =0x0
	add	x9, x0, #196, lsl #12           ; =802816
	ldr	w10, [x9, #1412]
Lloh10:
	adrp	x11, lCPI3_3@PAGE
Lloh11:
	ldr	q0, [x11, lCPI3_3@PAGEOFF]
	mov	w11, #5552                      ; =0x15b0
	mov	w12, #32881                     ; =0x8071
	movk	w12, #32775, lsl #16
	mov	w13, #65521                     ; =0xfff1
	movi.4s	v1, #4
Lloh12:
	adrp	x14, lCPI3_0@PAGE
Lloh13:
	ldr	q2, [x14, lCPI3_0@PAGEOFF]
Lloh14:
	adrp	x14, lCPI3_1@PAGE
Lloh15:
	ldr	q3, [x14, lCPI3_1@PAGEOFF]
Lloh16:
	adrp	x14, lCPI3_2@PAGE
Lloh17:
	ldr	q4, [x14, lCPI3_2@PAGEOFF]
Lloh18:
	adrp	x14, lCPI3_4@PAGE
Lloh19:
	ldr	q5, [x14, lCPI3_4@PAGEOFF]
Lloh20:
	adrp	x14, lCPI3_5@PAGE
Lloh21:
	ldr	q6, [x14, lCPI3_5@PAGEOFF]
Lloh22:
	adrp	x14, lCPI3_6@PAGE
Lloh23:
	ldr	q7, [x14, lCPI3_6@PAGEOFF]
Lloh24:
	adrp	x14, lCPI3_7@PAGE
Lloh25:
	ldr	q16, [x14, lCPI3_7@PAGEOFF]
	movi.4s	v17, #16
	ldr	w14, [x9, #1408]
	b	LBB3_4
LBB3_2:                                 ;   in Loop: Header=BB3_4 Depth=1
	mov	w3, #0                          ; =0x0
	mov	w0, #0                          ; =0x0
LBB3_3:                                 ;   in Loop: Header=BB3_4 Depth=1
	add	w10, w10, w3
	madd	w10, w14, w15, w10
	add	w14, w14, w0
	umull	x16, w14, w12
	lsr	x16, x16, #47
	msub	w14, w16, w13, w14
	str	w14, [x9, #1408]
	umull	x16, w10, w12
	lsr	x16, x16, #47
	msub	w10, w16, w13, w10
	str	w10, [x9, #1412]
	add	x8, x15, x8
	cmp	x8, x2
	b.hs	LBB3_18
LBB3_4:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_10 Depth 2
                                        ;     Child Loop BB3_14 Depth 2
                                        ;     Child Loop BB3_17 Depth 2
	subs	x15, x2, x8
	cmp	x15, x11
	csel	x15, x15, x11, lo
	subs	x16, x2, x8
	b.eq	LBB3_2
; %bb.5:                                ;   in Loop: Header=BB3_4 Depth=1
	cmp	x16, #4
	b.hs	LBB3_7
; %bb.6:                                ;   in Loop: Header=BB3_4 Depth=1
	mov	w0, #0                          ; =0x0
	mov	x16, #0                         ; =0x0
	mov	w3, #0                          ; =0x0
	b	LBB3_16
LBB3_7:                                 ;   in Loop: Header=BB3_4 Depth=1
	dup.4s	v18, w15
	cmp	x16, #16
	b.hs	LBB3_9
; %bb.8:                                ;   in Loop: Header=BB3_4 Depth=1
	mov	x16, #0                         ; =0x0
	mov	w0, #0                          ; =0x0
	mov	w3, #0                          ; =0x0
	b	LBB3_13
LBB3_9:                                 ;   in Loop: Header=BB3_4 Depth=1
	add	x0, x1, x8
	and	x17, x15, #0xc
	movi.2d	v19, #0000000000000000
	movi.2d	v20, #0000000000000000
	and	x16, x15, #0x1ff0
	and	x3, x15, #0x1ff0
	movi.2d	v21, #0000000000000000
	movi.2d	v22, #0000000000000000
	movi.2d	v23, #0000000000000000
	mov.16b	v27, v0
	movi.2d	v24, #0000000000000000
	mov.16b	v28, v4
	movi.2d	v25, #0000000000000000
	mov.16b	v29, v3
	movi.2d	v26, #0000000000000000
	mov.16b	v30, v2
LBB3_10:                                ;   Parent Loop BB3_4 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	q31, [x0], #16
	tbl.16b	v8, { v31 }, v5
	tbl.16b	v9, { v31 }, v6
	tbl.16b	v10, { v31 }, v7
	tbl.16b	v31, { v31 }, v16
	add.4s	v22, v22, v31
	add.4s	v21, v21, v10
	add.4s	v20, v20, v9
	add.4s	v23, v23, v8
	sub.4s	v11, v18, v28
	sub.4s	v12, v18, v27
	sub.4s	v13, v18, v29
	sub.4s	v14, v18, v30
	mla.4s	v19, v11, v10
	mla.4s	v24, v12, v9
	mla.4s	v25, v13, v31
	mla.4s	v26, v14, v8
	add.4s	v27, v27, v17
	add.4s	v28, v28, v17
	add.4s	v29, v29, v17
	add.4s	v30, v30, v17
	subs	x3, x3, #16
	b.ne	LBB3_10
; %bb.11:                               ;   in Loop: Header=BB3_4 Depth=1
	add.4s	v20, v20, v22
	add.4s	v21, v21, v23
	add.4s	v20, v20, v21
	addv.4s	s20, v20
	fmov	w0, s20
	add.4s	v20, v24, v25
	add.4s	v19, v19, v26
	add.4s	v19, v20, v19
	addv.4s	s19, v19
	fmov	w3, s19
	cmp	x15, x16
	b.eq	LBB3_3
; %bb.12:                               ;   in Loop: Header=BB3_4 Depth=1
	cbz	x17, LBB3_16
LBB3_13:                                ;   in Loop: Header=BB3_4 Depth=1
	mov	x4, x16
	and	x16, x15, #0x1ffc
	movi.2d	v20, #0000000000000000
	mov.s	v20[0], w0
	movi.2d	v19, #0000000000000000
	mov.s	v19[0], w3
	dup.4s	v21, w4
	orr.16b	v21, v21, v0
	sub	x17, x4, x16
	add	x0, x4, x8
	add	x0, x1, x0
LBB3_14:                                ;   Parent Loop BB3_4 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldr	s22, [x0], #4
	ushll.8h	v22, v22, #0
	ushll.4s	v23, v22, #0
	uaddw.4s	v20, v20, v22
	sub.4s	v22, v18, v21
	mla.4s	v19, v22, v23
	add.4s	v21, v21, v1
	adds	x17, x17, #4
	b.ne	LBB3_14
; %bb.15:                               ;   in Loop: Header=BB3_4 Depth=1
	addv.4s	s18, v20
	fmov	w0, s18
	addv.4s	s18, v19
	fmov	w3, s18
	cmp	x15, x16
	b.eq	LBB3_3
LBB3_16:                                ;   in Loop: Header=BB3_4 Depth=1
	sub	x17, x15, x16
	add	x16, x16, x8
	add	x16, x1, x16
LBB3_17:                                ;   Parent Loop BB3_4 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrb	w4, [x16], #1
	add	w0, w0, w4
	madd	w3, w17, w4, w3
	subs	x17, x17, #1
	b.ne	LBB3_17
	b	LBB3_3
LBB3_18:
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #32]             ; 16-byte Folded Reload
	ldp	d13, d12, [sp, #16]             ; 16-byte Folded Reload
	ldp	d15, d14, [sp], #64             ; 16-byte Folded Reload
LBB3_19:
	ret
	.loh AdrpLdr	Lloh24, Lloh25
	.loh AdrpAdrp	Lloh22, Lloh24
	.loh AdrpLdr	Lloh22, Lloh23
	.loh AdrpAdrp	Lloh20, Lloh22
	.loh AdrpLdr	Lloh20, Lloh21
	.loh AdrpAdrp	Lloh18, Lloh20
	.loh AdrpLdr	Lloh18, Lloh19
	.loh AdrpAdrp	Lloh16, Lloh18
	.loh AdrpLdr	Lloh16, Lloh17
	.loh AdrpAdrp	Lloh14, Lloh16
	.loh AdrpLdr	Lloh14, Lloh15
	.loh AdrpAdrp	Lloh12, Lloh14
	.loh AdrpLdr	Lloh12, Lloh13
	.loh AdrpLdr	Lloh10, Lloh11
	.cfi_endproc
                                        ; -- End function
	.private_extern	___clang_call_terminate ; -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	2
___clang_call_terminate:                ; @__clang_call_terminate
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	___cxa_begin_catch
	bl	__ZSt9terminatev
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNK8stx_zlib13DeflateStream9prefix_atEm ; -- Begin function _ZNK8stx_zlib13DeflateStream9prefix_atEm
	.p2align	2
__ZNK8stx_zlib13DeflateStream9prefix_atEm: ; @_ZNK8stx_zlib13DeflateStream9prefix_atEm
	.cfi_startproc
; %bb.0:
	add	x8, x0, x1
	ldrb	w9, [x8, #120]
	ldrb	w10, [x8, #121]
	ldrb	w8, [x8, #122]
	ldp	w12, w11, [x0, #84]
	lsl	w13, w9, w11
	eor	w13, w13, w10
	lsl	w11, w13, w11
	eor	w8, w11, w8
	sub	w11, w12, #1
	and	w8, w8, w11
	orr	w9, w9, w10, lsl #8
	orr	x0, x8, x9, lsl #32
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream6insertEm ; -- Begin function _ZN8stx_zlib13DeflateStream6insertEm
	.p2align	2
__ZN8stx_zlib13DeflateStream6insertEm:  ; @_ZN8stx_zlib13DeflateStream6insertEm
	.cfi_startproc
; %bb.0:
	add	x8, x0, x1
	ldrb	w9, [x8, #120]
	ldrb	w10, [x8, #121]
	ldrb	w8, [x8, #122]
	ldp	w12, w11, [x0, #84]
	lsl	w13, w9, w11
	eor	w13, w13, w10
	lsl	w11, w13, w11
	eor	w8, w11, w8
	sub	w11, w12, #1
	and	w8, w8, w11
	add	x8, x0, w8, uxtw #2
	mov	w11, #140                       ; =0x8c
	movk	w11, #1, lsl #16
	ldr	w12, [x8, x11]
	add	x13, x0, x1, lsl #2
	mov	w14, #140                       ; =0x8c
	movk	w14, #5, lsl #16
	str	w12, [x13, x14]
	orr	w9, w1, w9, lsl #16
	orr	w9, w9, w10, lsl #24
	str	w9, [x8, x11]
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj ; -- Begin function _ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj
	.p2align	2
__ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj: ; @_ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj
	.cfi_startproc
; %bb.0:
	cbz	x2, LBB7_8
; %bb.1:
	mov	x8, #0                          ; =0x0
	cmp	x2, #4
	b.lo	LBB7_6
; %bb.2:
	sub	x9, x3, x1
	cmp	x9, #16
	b.lo	LBB7_6
; %bb.3:
	dup.4s	v0, w5
	dup.4s	v1, w6
	and	x8, x2, #0xfffffffffffffffc
	movi.2d	v2, #0x00ffff0000ffff
	movi.2d	v3, #0xffff0000ffff0000
	mov	x9, x1
	mov	x10, x3
	and	x11, x2, #0xfffffffffffffffc
LBB7_4:                                 ; =>This Inner Loop Header: Depth=1
	ldr	q4, [x9], #16
	and.16b	v5, v4, v2
	sub.4s	v6, v5, v0
	cmeq.4s	v5, v5, v2
	cmhi.4s	v7, v1, v6
	bic.16b	v5, v7, v5
	and.16b	v4, v4, v3
	orr.16b	v4, v6, v4
	and.16b	v4, v4, v5
	orn.16b	v4, v4, v5
	str	q4, [x10], #16
	subs	x11, x11, #4
	b.ne	LBB7_4
; %bb.5:
	cmp	x2, x8
	b.eq	LBB7_8
LBB7_6:
	sub	x9, x2, x8
	lsl	x10, x8, #2
	add	x8, x1, x10
	add	x10, x3, x10
	mov	w11, #65535                     ; =0xffff
LBB7_7:                                 ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x8], #4
	and	w13, w12, #0xffff
	sub	w14, w13, w5
	cmp	w14, w6
	ccmp	w13, w11, #4, lo
	and	w12, w12, #0xffff0000
	orr	w12, w14, w12
	csinv	w12, w12, wzr, ne
	str	w12, [x10], #4
	subs	x9, x9, #1
	b.ne	LBB7_7
LBB7_8:
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy ; -- Begin function _ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	.p2align	2
__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy: ; @_ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	.cfi_startproc
; %bb.0:
	stp	x28, x27, [sp, #-96]!           ; 16-byte Folded Spill
	stp	x26, x25, [sp, #16]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #32]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #48]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #64]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #80]             ; 16-byte Folded Spill
	.cfi_def_cfa_offset 96
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
	mov	x25, #0                         ; =0x0
	mov	w8, #0                          ; =0x0
	ldr	x9, [x0, #112]
	subs	x9, x1, x9
	csel	x10, xzr, x9, lo
	cbz	w3, LBB8_31
; %bb.1:
	and	w22, w4, #0xffff
	mov	w11, #65535                     ; =0xffff
	cmp	w22, w11
	b.eq	LBB8_31
; %bb.2:
	mov	w9, #0                          ; =0x0
	mov	w12, #0                         ; =0x0
	mov	w8, #0                          ; =0x0
	add	x13, x0, #120
	sub	x20, x2, #11
	ldr	w15, [x0, #96]
	add	x16, x0, #80, lsl #12           ; =327680
	add	x16, x16, #140
	add	x17, x13, x1
	add	x19, x0, #123
	add	x21, x1, x0
	add	x0, x21, #123
	neg	x30, x2
	add	x21, x21, #120
LBB8_3:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB8_11 Depth 2
                                        ;     Child Loop BB8_21 Depth 2
	cmp	x7, w9, uxtw
	b.eq	LBB8_35
; %bb.4:                                ;   in Loop: Header=BB8_3 Depth=1
	add	w26, w9, #1
	mov	w23, w22
	cmp	x10, x23
	b.hi	LBB8_33
; %bb.5:                                ;   in Loop: Header=BB8_3 Depth=1
	cmp	w5, w4, lsr #16
	b.ne	LBB8_16
; %bb.6:                                ;   in Loop: Header=BB8_3 Depth=1
	cbz	w6, LBB8_9
; %bb.7:                                ;   in Loop: Header=BB8_3 Depth=1
	add	w4, w22, w6
	ldrb	w25, [x13, w4, uxtw]
	add	x24, x17, w6, uxtw
	ldrb	w27, [x24]
	cmp	w25, w27
	b.ne	LBB8_16
; %bb.8:                                ;   in Loop: Header=BB8_3 Depth=1
	sub	w4, w4, #1
	ldrb	w4, [x13, w4, uxtw]
	ldurb	w24, [x24, #-1]
	cmp	w4, w24
	b.ne	LBB8_16
LBB8_9:                                 ;   in Loop: Header=BB8_3 Depth=1
	cmn	x20, #9
	b.hi	LBB8_19
; %bb.10:                               ;   in Loop: Header=BB8_3 Depth=1
	mov	x4, #0                          ; =0x0
	add	x24, x19, x23
	sub	x25, x2, #3
	mov	w26, w9
LBB8_11:                                ;   Parent Loop BB8_3 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	w27, w26, w4
	add	w27, w27, #1
	sub	x27, x7, x27
	cmp	x27, #8
	b.lo	LBB8_32
; %bb.12:                               ;   in Loop: Header=BB8_11 Depth=2
	ldr	x27, [x24, x4]
	ldr	x28, [x0, x4]
	cmp	x27, x28
	b.ne	LBB8_25
; %bb.13:                               ;   in Loop: Header=BB8_11 Depth=2
	add	x4, x4, #8
	sub	x25, x25, #8
	cmp	x25, #7
	b.hi	LBB8_11
; %bb.14:                               ;   in Loop: Header=BB8_3 Depth=1
	add	x24, x4, #3
	add	w9, w9, w4
	add	w26, w9, #1
	cmp	x24, x2
	b.lo	LBB8_20
LBB8_15:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	x9, x26
	cmp	x24, #3
	b.hs	LBB8_27
	b	LBB8_30
LBB8_16:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	x24, x6
	mov	x9, x26
LBB8_17:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	x25, #0                         ; =0x0
	add	w12, w12, #1
	cmp	w12, w3
	b.hs	LBB8_36
; %bb.18:                               ;   in Loop: Header=BB8_3 Depth=1
	ldr	w4, [x16, x23, lsl #2]
	and	w22, w4, #0xffff
	mov	x6, x24
	cmp	w22, w11
	b.ne	LBB8_3
	b	LBB8_36
LBB8_19:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	w24, #3                         ; =0x3
	cmp	x24, x2
	b.hs	LBB8_15
LBB8_20:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	x4, #0                          ; =0x0
	add	w9, w26, w2
	sub	w9, w9, w24
	add	x25, x24, x23
	add	x25, x13, x25
	add	x27, x30, x24
	add	x28, x21, x24
	mov	w30, w26
LBB8_21:                                ;   Parent Loop BB8_3 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	w20, w30, w4
	cmp	x7, x20
	b.eq	LBB8_34
; %bb.22:                               ;   in Loop: Header=BB8_21 Depth=2
	ldrb	w20, [x25, x4]
	ldrb	w14, [x28, x4]
	cmp	w20, w14
	b.ne	LBB8_26
; %bb.23:                               ;   in Loop: Header=BB8_21 Depth=2
	add	x4, x4, #1
	cmn	x27, x4
	b.ne	LBB8_21
; %bb.24:                               ;   in Loop: Header=BB8_3 Depth=1
	mov	x24, x2
	sub	x20, x2, #11
	neg	x30, x2
	cmp	x2, #3
	b.hs	LBB8_27
	b	LBB8_30
LBB8_25:                                ;   in Loop: Header=BB8_3 Depth=1
	eor	x24, x28, x27
	rbit	x24, x24
	clz	x24, x24
	add	x24, x4, x24, lsr #3
	add	x24, x24, #3
	add	w9, w9, w4
	add	w9, w9, #9
	cmp	x24, #3
	b.hs	LBB8_27
	b	LBB8_30
LBB8_26:                                ;   in Loop: Header=BB8_3 Depth=1
	add	x24, x24, x4
	add	w9, w26, w4
	add	w9, w9, #1
	sub	x20, x2, #11
	neg	x30, x2
	cmp	x24, #3
	b.lo	LBB8_30
LBB8_27:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	w14, w6
	cmp	x24, x14
	b.ls	LBB8_30
; %bb.28:                               ;   in Loop: Header=BB8_3 Depth=1
	mov	x25, #0                         ; =0x0
	sub	w8, w1, w22
	cmp	x24, x2
	b.eq	LBB8_36
; %bb.29:                               ;   in Loop: Header=BB8_3 Depth=1
	cmp	x24, x15
	b.lo	LBB8_17
	b	LBB8_36
LBB8_30:                                ;   in Loop: Header=BB8_3 Depth=1
	mov	x24, x6
	b	LBB8_17
LBB8_31:
	mov	x24, x6
	mov	x9, x8
	b	LBB8_36
LBB8_32:
	add	w9, w9, w4
	add	w9, w9, #1
	b	LBB8_35
LBB8_33:
	mov	x25, #0                         ; =0x0
	mov	x24, x6
	mov	x9, x26
	b	LBB8_36
LBB8_34:
	add	w9, w26, w4
LBB8_35:
	mov	x25, #4294967296                ; =0x100000000
	mov	x24, x6
LBB8_36:
	mov	w10, w24
	orr	x0, x10, x8, lsl #32
	mov	w8, w9
	orr	x1, x25, x8
	ldp	x29, x30, [sp, #80]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #64]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #48]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #32]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #16]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp], #96             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream12append_tokenEjjRm ; -- Begin function _ZN8stx_zlib13DeflateStream12append_tokenEjjRm
	.p2align	2
__ZN8stx_zlib13DeflateStream12append_tokenEjjRm: ; @_ZN8stx_zlib13DeflateStream12append_tokenEjjRm
	.cfi_startproc
; %bb.0:
	ldr	x8, [x3]
	add	x9, x0, x8, lsl #2
	add	x9, x9, #144, lsl #12           ; =589824
	strh	w1, [x9, #140]!
	strh	w2, [x9, #2]
	add	x8, x8, #1
	str	x8, [x3]
	cbz	w2, LBB9_2
; %bb.1:
Lloh26:
	adrp	x8, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGE
Lloh27:
	add	x8, x8, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGEOFF
	ldr	w8, [x8, w1, uxtw #2]
	add	w9, w8, #257
	add	x9, x0, w9, uxtw #2
	mov	w10, #152                       ; =0x98
	movk	w10, #11, lsl #16
	ldr	w11, [x9, x10]
	add	w11, w11, #1
	str	w11, [x9, x10]
	sub	w9, w2, #1
	clz	w10, w9
	mov	w11, #30                        ; =0x1e
	sub	w11, w11, w10
	lsr	w11, w9, w11
	and	w11, w11, #0x1
	sub	w10, w11, w10, lsl #1
	add	w10, w10, #62
	cmp	w2, #5
	csel	w9, w9, w10, lo
	add	x10, x0, w9, uxtw #2
	mov	w11, #1296                      ; =0x510
	movk	w11, #11, lsl #16
	ldr	w12, [x10, x11]
	add	w12, w12, #1
	str	w12, [x10, x11]
Lloh28:
	adrp	x10, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGE
Lloh29:
	add	x10, x10, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGEOFF
	ldr	w8, [x10, x8, lsl #2]
	mov	w10, #6232                      ; =0x1858
	movk	w10, #11, lsl #16
	ldr	x11, [x0, x10]
	lsr	w12, w9, #1
	sub	w12, w12, #1
	cmp	w9, #3
	csel	w9, w12, wzr, hi
	add	x9, x9, x11
	add	x8, x8, x9
	str	x8, [x0, x10]
	ret
LBB9_2:
	add	x8, x0, w1, uxtw #2
	mov	w9, #152                        ; =0x98
	movk	w9, #11, lsl #16
	ldr	w10, [x8, x9]
	add	w10, w10, #1
	str	w10, [x8, x9]
	ret
	.loh AdrpAdd	Lloh28, Lloh29
	.loh AdrpAdd	Lloh26, Lloh27
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream8tokenizeEv ; -- Begin function _ZN8stx_zlib13DeflateStream8tokenizeEv
	.p2align	2
__ZN8stx_zlib13DeflateStream8tokenizeEv: ; @_ZN8stx_zlib13DeflateStream8tokenizeEv
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #272
	stp	x28, x27, [sp, #176]            ; 16-byte Folded Spill
	stp	x26, x25, [sp, #192]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #208]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #224]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #240]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #256]            ; 16-byte Folded Spill
	add	x29, sp, #256
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
	mov	x20, x0
	mov	w23, #6232                      ; =0x1858
	movk	w23, #11, lsl #16
	add	x8, x0, #176, lsl #12           ; =720896
	add	x22, x8, #144
	add	x8, x0, #16, lsl #12            ; =65536
	add	x21, x8, #120
	str	xzr, [x0, x23]
	mov	x0, x22
	mov	w1, #1272                       ; =0x4f8
	bl	_bzero
	mov	w8, #1                          ; =0x1
	str	w8, [x22, #1032]
	ldr	x27, [x21, #8]
	ldr	x19, [x20, #40]
	ldr	x8, [x20, #72]
	sub	x9, x19, x8
	cmp	x27, x9
	b.ls	LBB10_3
LBB10_1:
	mov	w0, #0                          ; =0x0
LBB10_2:
	movi.2s	v0, #5
	str	d0, [x20, #48]
	b	LBB10_146
LBB10_3:
	add	x9, x20, #176, lsl #12          ; =720896
	add	x25, x9, #152
	ldr	x26, [x21]
	add	x30, x27, x26
	add	x8, x8, x27
	str	x8, [x20, #72]
	ldr	w12, [x20, #4]
	cmp	w12, #2
	b.eq	LBB10_18
; %bb.4:
	add	x9, x20, #176, lsl #12          ; =720896
	add	x24, x9, #1296
	cmp	w12, #3
	b.ne	LBB10_28
; %bb.5:
	cmp	x26, x30
	b.hs	LBB10_27
; %bb.6:
	mov	x9, #0                          ; =0x0
	mov	x10, #0                         ; =0x0
	add	x11, x20, #120
	add	x12, x20, #144, lsl #12         ; =589824
	add	x12, x12, #140
	mov	w13, #258                       ; =0x102
	mov	w14, #1                         ; =0x1
Lloh30:
	adrp	x15, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGE
Lloh31:
	add	x15, x15, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGEOFF
Lloh32:
	adrp	x16, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGE
Lloh33:
	add	x16, x16, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGEOFF
	b	LBB10_8
LBB10_7:                                ;   in Loop: Header=BB10_8 Depth=1
	ldrb	w17, [x11, x26]
	add	x0, x12, x9, lsl #2
	strh	w17, [x0]
	strh	wzr, [x0, #2]
	ldr	w0, [x25, x17, lsl #2]
	add	w0, w0, #1
	str	w0, [x25, x17, lsl #2]
	mov	w17, #1                         ; =0x1
	add	x9, x9, #1
	add	x26, x17, x26
	cmp	x26, x30
	b.hs	LBB10_17
LBB10_8:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB10_11 Depth 2
	sub	x17, x30, x26
	cmp	x17, #258
	csel	x17, x17, x13, lo
	cbz	x26, LBB10_7
; %bb.9:                                ;   in Loop: Header=BB10_8 Depth=1
	cmp	x30, x26
	b.eq	LBB10_7
; %bb.10:                               ;   in Loop: Header=BB10_8 Depth=1
	mov	x4, #0                          ; =0x0
	add	x0, x11, x26
	add	x2, x8, #1
	sub	x3, x19, x8
LBB10_11:                               ;   Parent Loop BB10_8 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cmp	x3, x4
	b.eq	LBB10_1
; %bb.12:                               ;   in Loop: Header=BB10_11 Depth=2
	mov	x1, x4
	add	x4, x2, x4
	str	x4, [x20, #72]
	ldrb	w4, [x0, x1]
	ldurb	w5, [x0, #-1]
	cmp	w4, w5
	b.ne	LBB10_15
; %bb.13:                               ;   in Loop: Header=BB10_11 Depth=2
	add	x4, x1, #1
	cmp	x17, x4
	b.ne	LBB10_11
; %bb.14:                               ;   in Loop: Header=BB10_8 Depth=1
	add	x8, x8, x1
	add	x8, x8, #1
	cmp	x17, #3
	b.lo	LBB10_7
	b	LBB10_16
LBB10_15:                               ;   in Loop: Header=BB10_8 Depth=1
	mov	x17, x1
	add	x8, x8, x1
	add	x8, x8, #1
	cmp	x1, #3
	b.lo	LBB10_7
LBB10_16:                               ;   in Loop: Header=BB10_8 Depth=1
	add	x0, x12, x9, lsl #2
	strh	w17, [x0]
	ldr	w1, [x15, w17, uxtw #2]
	add	w2, w1, #257
	ldr	w3, [x25, w2, uxtw #2]
	strh	w14, [x0, #2]
	add	w0, w3, #1
	str	w0, [x25, w2, uxtw #2]
	ldr	w0, [x24]
	add	w0, w0, #1
	str	w0, [x24]
	ldr	w0, [x16, x1, lsl #2]
	add	x10, x10, x0
	str	x10, [x20, x23]
	add	x9, x9, #1
	add	x26, x17, x26
	cmp	x26, x30
	b.lo	LBB10_8
LBB10_17:
	str	x9, [x22]
	b	LBB10_145
LBB10_18:
	cmp	x26, x30
	b.hs	LBB10_27
; %bb.19:
	mov	x9, #0                          ; =0x0
	sub	x11, x27, #1
	mov	x8, #-6148914691236517206       ; =0xaaaaaaaaaaaaaaaa
	movk	x8, #43691
	umulh	x8, x11, x8
	lsr	x8, x8, #1
	add	x8, x8, x8, lsl #1
	sub	x8, x11, x8
	add	x10, x8, #1
	cmp	x10, #3
	csinc	x8, xzr, x8, eq
	cmp	x11, #2
	b.lo	LBB10_24
; %bb.20:
	mov	x9, #0                          ; =0x0
	add	x11, x20, #144, lsl #12         ; =589824
	add	x12, x11, #146
	neg	x11, x26
	add	x13, x26, x20
	add	x13, x13, #122
	mov	x14, x8
LBB10_21:                               ; =>This Inner Loop Header: Depth=1
	ldurb	w15, [x13, #-2]
	sturh	w15, [x12, #-6]
	sturh	wzr, [x12, #-4]
	ldr	w16, [x25, x15, lsl #2]
	add	w16, w16, #1
	str	w16, [x25, x15, lsl #2]
	ldurb	w15, [x13, #-1]
	sturh	w15, [x12, #-2]
	ldr	w16, [x25, x15, lsl #2]
	strh	wzr, [x12]
	add	w16, w16, #1
	str	w16, [x25, x15, lsl #2]
	ldrb	w15, [x13], #3
	add	x9, x9, #3
	ldr	w16, [x25, x15, lsl #2]
	add	w16, w16, #1
	str	w16, [x25, x15, lsl #2]
	add	x14, x14, #3
	strh	w15, [x12, #2]
	strh	wzr, [x12, #4]
	sub	x11, x11, #3
	add	x12, x12, #12
	cmp	x27, x14
	b.ne	LBB10_21
; %bb.22:
	cmp	x10, #3
	b.eq	LBB10_26
; %bb.23:
	neg	x26, x11
LBB10_24:
	add	x10, x26, x20
	add	x10, x10, #120
	add	x9, x20, x9, lsl #2
	add	x9, x9, #144, lsl #12           ; =589824
	add	x9, x9, #142
LBB10_25:                               ; =>This Inner Loop Header: Depth=1
	ldrb	w11, [x10], #1
	sturh	w11, [x9, #-2]
	ldr	w12, [x25, x11, lsl #2]
	strh	wzr, [x9], #4
	add	w12, w12, #1
	str	w12, [x25, x11, lsl #2]
	subs	x8, x8, #1
	b.ne	LBB10_25
LBB10_26:
	str	x27, [x22]
	b	LBB10_145
LBB10_27:
	str	xzr, [x22]
	b	LBB10_145
LBB10_28:
	subs	x9, x30, #2
	csel	x9, xzr, x9, lo
	stur	x9, [x29, #-112]                ; 8-byte Folded Spill
	mov	w9, #2                          ; =0x2
	cmp	x26, #2
	csel	x9, x26, x9, lo
	add	x10, x27, x9
	sub	x11, x19, x8
	cmp	x10, x11
	b.hi	LBB10_1
; %bb.29:
	add	x28, x8, x10
	str	x28, [x20, #72]
	sub	x8, x26, x9
	ldr	w9, [x20, #16]
	add	w9, w9, #7
	lsl	w10, w30, #1
	mov	w11, #256                       ; =0x100
	cmp	w10, #256
	csel	w10, w10, w11, hi
	sub	w10, w10, #1
	clz	w10, w10
	neg	w10, w10
	and	w10, w10, #0x1f
	cmp	w10, w9
	csel	w9, w10, w9, lo
	cmp	x30, #3
	str	w12, [sp, #116]                 ; 4-byte Spill
	b.lo	LBB10_34
; %bb.30:
	mov	w10, #1                         ; =0x1
	lsl	w9, w10, w9
	ldrb	w10, [x21, #16]
	cmp	w10, #1
	b.ne	LBB10_32
; %bb.31:
	ldr	w10, [x20, #84]
	cmp	w9, w10
	b.ls	LBB10_34
LBB10_32:
	sub	w8, w9, #1
	clz	w10, w8
	mov	w11, #34                        ; =0x22
	sub	w10, w11, w10
	and	w10, w10, #0xff
	mov	w11, #86                        ; =0x56
	mul	w10, w10, w11
	lsr	w10, w10, #8
	stp	w9, w10, [x20, #84]
	add	x9, x26, w9, uxtw
	sub	x10, x19, x28
	cmp	x9, x10
	b.hi	LBB10_1
; %bb.33:
	add	x28, x28, x9
	str	x28, [x20, #72]
	ubfiz	x8, x8, #2, #32
	add	x9, x20, #16, lsl #12           ; =65536
	add	x0, x9, #140
	add	x2, x8, #4
	mov	w1, #255                        ; =0xff
	bl	_memset
	add	x30, x27, x26
	mov	x8, #0                          ; =0x0
	mov	w9, #1                          ; =0x1
	strb	w9, [x21, #16]
LBB10_34:
	ldur	x9, [x29, #-112]                ; 8-byte Folded Reload
	cmp	x9, x26
	csel	x9, x9, x26, lo
	subs	x9, x8, x9
	b.hs	LBB10_37
; %bb.35:
	ldp	w11, w10, [x20, #84]
	sub	w11, w11, #1
	add	x12, x20, #16, lsl #12          ; =65536
	add	x12, x12, #140
	add	x13, x20, x8, lsl #2
	add	x13, x13, #80, lsl #12          ; =327680
	add	x13, x13, #140
	add	x14, x8, x20
	add	x14, x14, #122
LBB10_36:                               ; =>This Inner Loop Header: Depth=1
	ldurb	w15, [x14, #-2]
	ldurb	w16, [x14, #-1]
	ldrb	w17, [x14], #1
	lsl	w0, w15, w10
	eor	w0, w0, w16
	lsl	w0, w0, w10
	eor	w17, w0, w17
	and	w17, w17, w11
	ldr	w0, [x12, w17, uxtw #2]
	str	w0, [x13], #4
	orr	w15, w8, w15, lsl #16
	orr	w15, w15, w16, lsl #24
	str	w15, [x12, w17, uxtw #2]
	add	w8, w8, #1
	adds	x9, x9, #1
	b.lo	LBB10_36
LBB10_37:
	str	x22, [sp, #24]                  ; 8-byte Spill
	str	x28, [sp, #40]                  ; 8-byte Spill
	sub	x9, x19, x28
	ldur	x8, [x29, #-112]                ; 8-byte Folded Reload
	cmp	x26, x8
	str	x9, [sp, #32]                   ; 8-byte Spill
	b.hs	LBB10_138
; %bb.38:
	str	x24, [sp, #64]                  ; 8-byte Spill
	mov	x24, #0                         ; =0x0
	mov	x22, #0                         ; =0x0
	ldr	x8, [x20, x23]
	str	x8, [sp, #120]                  ; 8-byte Spill
	add	x19, x20, #120
	ldp	w15, w8, [x20, #80]
	sub	w6, w8, #1
	add	x8, x20, #16, lsl #12           ; =65536
	add	x28, x8, #140
	add	x8, x20, #80, lsl #12           ; =327680
	add	x8, x8, #140
	stur	x8, [x29, #-104]                ; 8-byte Folded Spill
	ldp	w21, w8, [x20, #88]
	lsr	w10, w8, #2
	ldp	w12, w11, [x20, #96]
	str	x12, [sp, #88]                  ; 8-byte Spill
	stp	w11, w10, [sp, #76]             ; 8-byte Folded Spill
	cmp	w11, #0
	str	w8, [sp, #84]                   ; 4-byte Spill
	csel	w8, w10, w8, eq
	str	x8, [sp, #48]                   ; 8-byte Spill
	ldr	x8, [x20, #112]
	str	x8, [sp, #56]                   ; 8-byte Spill
	add	x8, x20, #144, lsl #12          ; =589824
	add	x3, x8, #140
	add	x8, x20, #80, lsl #12           ; =327680
	add	x8, x8, #144
	str	x8, [sp, #8]                    ; 8-byte Spill
	ldr	w8, [x20, #104]
	stur	w8, [x29, #-116]                ; 4-byte Folded Spill
	mov	x23, x26
	mov	x7, x9
	ldr	w17, [sp, #116]                 ; 4-byte Reload
	stp	w15, w6, [x29, #-124]           ; 8-byte Folded Spill
	str	x3, [sp, #96]                   ; 8-byte Spill
	b	LBB10_42
LBB10_39:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x24, #0                         ; =0x0
LBB10_40:                               ;   in Loop: Header=BB10_42 Depth=1
	ldur	x8, [x29, #-96]                 ; 8-byte Folded Reload
	ldrb	w8, [x8]
	add	x9, x3, x22, lsl #2
	strh	w8, [x9]
	strh	wzr, [x9, #2]
	ldr	w9, [x25, x8, lsl #2]
	add	w9, w9, #1
	str	w9, [x25, x8, lsl #2]
	mov	w8, #1                          ; =0x1
LBB10_41:                               ;   in Loop: Header=BB10_42 Depth=1
	add	x22, x22, #1
	add	x23, x8, x23
	ldur	x8, [x29, #-112]                ; 8-byte Folded Reload
	cmp	x23, x8
	b.hs	LBB10_139
LBB10_42:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB10_51 Depth 2
                                        ;       Child Loop BB10_58 Depth 3
                                        ;       Child Loop BB10_66 Depth 3
                                        ;     Child Loop BB10_98 Depth 2
                                        ;       Child Loop BB10_105 Depth 3
                                        ;       Child Loop BB10_114 Depth 3
                                        ;     Child Loop BB10_135 Depth 2
	add	x11, x19, x23
	ldrb	w8, [x11]
	ldrb	w9, [x11, #1]
	add	x10, x19, x23
	stur	x10, [x29, #-96]                ; 8-byte Folded Spill
	ldrb	w10, [x11, #2]
	lsl	w11, w8, w21
	eor	w11, w11, w9
	lsl	w11, w11, w21
	eor	w10, w11, w10
	and	w10, w10, w6
	ldr	w4, [x28, w10, uxtw #2]
	orr	w5, w8, w9, lsl #8
	ldur	x8, [x29, #-104]                ; 8-byte Folded Reload
	str	w4, [x8, x23, lsl #2]
	orr	w8, w23, w5, lsl #16
	str	w8, [x28, w10, uxtw #2]
	cbz	w24, LBB10_44
; %bb.43:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x0, x24
	b	LBB10_81
LBB10_44:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x0, #0                          ; =0x0
	sub	x8, x30, x23
	cmp	x8, #258
	mov	w9, #258                        ; =0x102
	csel	x2, x8, x9, lo
	cmp	x8, #3
	b.lo	LBB10_81
; %bb.45:                               ;   in Loop: Header=BB10_42 Depth=1
	and	w15, w4, #0xffff
	mov	w8, #65535                      ; =0xffff
	cmp	w15, w8
	b.eq	LBB10_48
; %bb.46:                               ;   in Loop: Header=BB10_42 Depth=1
	add	x8, x2, #9
	ldr	x3, [sp, #48]                   ; 8-byte Reload
	mul	x8, x8, x3
	cmp	x7, x8
	b.hs	LBB10_49
; %bb.47:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x19, x7
	mov	x0, x20
	mov	x1, x23
                                        ; kill: def $w3 killed $w3 killed $x3
	mov	w6, #0                          ; =0x0
	bl	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	ldp	w15, w6, [x29, #-124]           ; 8-byte Folded Reload
	ldr	w17, [sp, #116]                 ; 4-byte Reload
	add	x30, x27, x26
	mov	x7, x19
	add	x19, x20, #120
	ldr	x3, [sp, #96]                   ; 8-byte Reload
	sub	x7, x7, w1, uxtw
	tbz	x1, #32, LBB10_80
	b	LBB10_147
LBB10_48:                               ;   in Loop: Header=BB10_42 Depth=1
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
	b	LBB10_81
LBB10_49:                               ;   in Loop: Header=BB10_42 Depth=1
	ldr	x8, [sp, #56]                   ; 8-byte Reload
	subs	x8, x23, x8
	csel	x9, xzr, x8, lo
	cbz	w3, LBB10_77
; %bb.50:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	w14, #0                         ; =0x0
	mov	w8, #0                          ; =0x0
	mov	w10, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
	sub	x11, x2, #11
	add	x19, x20, #120
	add	x12, x19, x23
	add	x13, x20, #123
	add	x13, x13, x23
LBB10_51:                               ;   Parent Loop BB10_42 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB10_58 Depth 3
                                        ;       Child Loop BB10_66 Depth 3
	add	w0, w14, #1
	mov	w15, w15
	cmp	x9, x15
	b.hi	LBB10_78
; %bb.52:                               ;   in Loop: Header=BB10_51 Depth=2
	cmp	w5, w4, lsr #16
	b.ne	LBB10_62
; %bb.53:                               ;   in Loop: Header=BB10_51 Depth=2
	cbz	w16, LBB10_56
; %bb.54:                               ;   in Loop: Header=BB10_51 Depth=2
	add	w17, w15, w16
	ldrb	w6, [x19, w17, uxtw]
	add	x1, x12, w16, uxtw
	ldrb	w4, [x1]
	cmp	w6, w4
	b.ne	LBB10_63
; %bb.55:                               ;   in Loop: Header=BB10_51 Depth=2
	sub	w17, w17, #1
	ldrb	w17, [x19, w17, uxtw]
	ldurb	w1, [x1, #-1]
	cmp	w17, w1
	ldur	w6, [x29, #-120]                ; 4-byte Folded Reload
	b.ne	LBB10_62
LBB10_56:                               ;   in Loop: Header=BB10_51 Depth=2
	cmn	x11, #9
	b.hi	LBB10_64
; %bb.57:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x17, #0                         ; =0x0
	add	x0, x20, #123
	add	x0, x0, x15
	sub	x1, x2, #3
LBB10_58:                               ;   Parent Loop BB10_42 Depth=1
                                        ;     Parent Loop BB10_51 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	x6, [x0, x17]
	ldr	x4, [x13, x17]
	cmp	x6, x4
	b.ne	LBB10_69
; %bb.59:                               ;   in Loop: Header=BB10_58 Depth=3
	add	x17, x17, #8
	sub	x1, x1, #8
	cmp	x1, #7
	b.hi	LBB10_58
; %bb.60:                               ;   in Loop: Header=BB10_51 Depth=2
	add	w14, w14, w17
	add	w0, w14, #1
	add	x17, x17, #3
	ldur	w6, [x29, #-120]                ; 4-byte Folded Reload
	cmp	x17, x2
	b.lo	LBB10_65
LBB10_61:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x14, x0
	cmp	x17, #3
	b.hs	LBB10_71
	b	LBB10_76
LBB10_62:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x17, x16
	mov	x14, x0
	add	w10, w10, #1
	cmp	w10, w3
	b.lo	LBB10_75
	b	LBB10_79
LBB10_63:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x17, x16
	mov	x14, x0
	ldur	w6, [x29, #-120]                ; 4-byte Folded Reload
	add	w10, w10, #1
	cmp	w10, w3
	b.lo	LBB10_75
	b	LBB10_79
LBB10_64:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	w17, #3                         ; =0x3
	cmp	x17, x2
	b.hs	LBB10_61
LBB10_65:                               ;   in Loop: Header=BB10_51 Depth=2
	add	x1, x19, x15
	add	w14, w0, w2
	sub	w24, w14, w17
	add	w14, w0, #1
LBB10_66:                               ;   Parent Loop BB10_42 Depth=1
                                        ;     Parent Loop BB10_51 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w0, [x1, x17]
	ldur	x4, [x29, #-96]                 ; 8-byte Folded Reload
	ldrb	w4, [x4, x17]
	cmp	w0, w4
	b.ne	LBB10_70
; %bb.67:                               ;   in Loop: Header=BB10_66 Depth=3
	add	x17, x17, #1
	add	w14, w14, #1
	cmp	x2, x17
	b.ne	LBB10_66
; %bb.68:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x17, x2
	mov	x14, x24
	b	LBB10_70
LBB10_69:                               ;   in Loop: Header=BB10_51 Depth=2
	eor	x0, x4, x6
	rbit	x0, x0
	clz	x0, x0
	add	w14, w14, w17
	add	w14, w14, #9
	add	x17, x17, x0, lsr #3
	add	x17, x17, #3
LBB10_70:                               ;   in Loop: Header=BB10_51 Depth=2
	ldur	w6, [x29, #-120]                ; 4-byte Folded Reload
	cmp	x17, #3
	b.lo	LBB10_76
LBB10_71:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	w0, w16
	cmp	x17, x0
	b.ls	LBB10_76
; %bb.72:                               ;   in Loop: Header=BB10_51 Depth=2
	sub	w8, w23, w15
	cmp	x17, x2
	b.eq	LBB10_79
; %bb.73:                               ;   in Loop: Header=BB10_51 Depth=2
	ldr	x16, [sp, #88]                  ; 8-byte Reload
	cmp	x17, x16
	b.hs	LBB10_79
; %bb.74:                               ;   in Loop: Header=BB10_51 Depth=2
	add	w10, w10, #1
	cmp	w10, w3
	b.hs	LBB10_79
LBB10_75:                               ;   in Loop: Header=BB10_51 Depth=2
	ldur	x16, [x29, #-104]               ; 8-byte Folded Reload
	ldr	w4, [x16, x15, lsl #2]
	and	w15, w4, #0xffff
	mov	x16, x17
	mov	w0, #65535                      ; =0xffff
	cmp	w15, w0
	b.ne	LBB10_51
	b	LBB10_79
LBB10_76:                               ;   in Loop: Header=BB10_51 Depth=2
	mov	x17, x16
	add	w10, w10, #1
	cmp	w10, w3
	b.lo	LBB10_75
	b	LBB10_79
LBB10_77:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x1, #0                          ; =0x0
	mov	x0, #0                          ; =0x0
	add	x19, x20, #120
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
	ldr	x3, [sp, #96]                   ; 8-byte Reload
	sub	x7, x7, w1, uxtw
	tbz	x1, #32, LBB10_80
	b	LBB10_147
LBB10_78:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x17, x16
	mov	x14, x0
LBB10_79:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	w9, w17
	orr	x0, x9, x8, lsl #32
	mov	w1, w14
	ldr	w17, [sp, #116]                 ; 4-byte Reload
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
	ldr	x3, [sp, #96]                   ; 8-byte Reload
	sub	x7, x7, w1, uxtw
	tbnz	x1, #32, LBB10_147
LBB10_80:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x24, x0
LBB10_81:                               ;   in Loop: Header=BB10_42 Depth=1
	cmp	w17, #1
	b.ne	LBB10_83
; %bb.82:                               ;   in Loop: Header=BB10_42 Depth=1
	cmp	w24, #6
	b.lo	LBB10_85
LBB10_83:                               ;   in Loop: Header=BB10_42 Depth=1
	lsr	x1, x0, #32
	cmp	w24, #3
	b.ne	LBB10_86
; %bb.84:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x8, #17596481011712             ; =0x100100000000
	cmp	x0, x8
	b.lo	LBB10_86
LBB10_85:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	w24, #0                         ; =0x0
	mov	w1, #0                          ; =0x0
LBB10_86:                               ;   in Loop: Header=BB10_42 Depth=1
	cmp	w15, #4
	b.lo	LBB10_128
; %bb.87:                               ;   in Loop: Header=BB10_42 Depth=1
	cmp	w24, #3
	b.lo	LBB10_128
; %bb.88:                               ;   in Loop: Header=BB10_42 Depth=1
	ldur	w8, [x29, #-116]                ; 4-byte Folded Reload
	cmp	w24, w8
	b.hs	LBB10_128
; %bb.89:                               ;   in Loop: Header=BB10_42 Depth=1
	sub	x8, x30, x23
	cmp	x8, #4
	b.lo	LBB10_128
; %bb.90:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x10, #0                         ; =0x0
	add	x8, x23, #1
	sub	x9, x30, x8
	cmp	x9, #258
	mov	w11, #258                       ; =0x102
	csel	x2, x9, x11, lo
	mov	w0, w24
	cmp	x2, x0
	b.ls	LBB10_94
; %bb.91:                               ;   in Loop: Header=BB10_42 Depth=1
	add	x9, x19, x8
	ldrb	w11, [x9]
	ldrb	w12, [x9, #1]
	ldrb	w13, [x9, #2]
	lsl	w14, w11, w21
	eor	w14, w14, w12
	lsl	w14, w14, w21
	eor	w13, w14, w13
	and	w13, w13, w6
	ldr	w4, [x28, w13, uxtw #2]
	and	w16, w4, #0xffff
	mov	w13, #65535                     ; =0xffff
	cmp	w16, w13
	b.eq	LBB10_94
; %bb.92:                               ;   in Loop: Header=BB10_42 Depth=1
	orr	w5, w11, w12, lsl #8
	ldp	w10, w11, [sp, #76]             ; 8-byte Folded Reload
	cmp	w24, w10
	ldr	w10, [sp, #84]                  ; 4-byte Reload
	csel	w3, w10, w11, lo
	add	x10, x2, #9
	mul	x10, x10, x3
	cmp	x7, x10
	str	x1, [sp, #104]                  ; 8-byte Spill
	b.hs	LBB10_96
; %bb.93:                               ;   in Loop: Header=BB10_42 Depth=1
	add	x1, x23, #1
	mov	x0, x20
                                        ; kill: def $w3 killed $w3 killed $x3
	mov	x6, x24
	mov	x19, x27
	mov	x27, x7
	bl	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
	mov	x7, x27
	mov	x27, x19
	ldr	w17, [sp, #116]                 ; 4-byte Reload
	add	x30, x19, x26
	b	LBB10_126
LBB10_94:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	x0, x24
	cmp	w24, w0
	b.hs	LBB10_128
LBB10_95:                               ;   in Loop: Header=BB10_42 Depth=1
	mov	w8, w0
	orr	x24, x10, x8
	b	LBB10_40
LBB10_96:                               ;   in Loop: Header=BB10_42 Depth=1
	ldr	x10, [sp, #56]                  ; 8-byte Reload
	subs	x10, x8, x10
	csel	x11, xzr, x10, lo
	cbz	w3, LBB10_123
; %bb.97:                               ;   in Loop: Header=BB10_42 Depth=1
	str	x7, [sp, #16]                   ; 8-byte Spill
	mov	w15, #0                         ; =0x0
	mov	w10, #0                         ; =0x0
	mov	w12, #0                         ; =0x0
	sub	x13, x2, #11
	add	x14, x20, #124
	add	x14, x14, x23
	mov	x17, x24
LBB10_98:                               ;   Parent Loop BB10_42 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB10_105 Depth 3
                                        ;       Child Loop BB10_114 Depth 3
	add	w1, w15, #1
	mov	w16, w16
	cmp	x11, x16
	b.hi	LBB10_124
; %bb.99:                               ;   in Loop: Header=BB10_98 Depth=2
	cmp	w5, w4, lsr #16
	b.ne	LBB10_109
; %bb.100:                              ;   in Loop: Header=BB10_98 Depth=2
	cbz	w17, LBB10_103
; %bb.101:                              ;   in Loop: Header=BB10_98 Depth=2
	add	w0, w16, w17
	ldrb	w4, [x19, w0, uxtw]
	ldrb	w6, [x9, w17, uxtw]
	cmp	w4, w6
	b.ne	LBB10_109
; %bb.102:                              ;   in Loop: Header=BB10_98 Depth=2
	sub	w0, w0, #1
	ldrb	w0, [x19, w0, uxtw]
	ldur	x4, [x29, #-96]                 ; 8-byte Folded Reload
	ldrb	w4, [x4, w17, uxtw]
	cmp	w0, w4
	b.ne	LBB10_109
LBB10_103:                              ;   in Loop: Header=BB10_98 Depth=2
	cmn	x13, #9
	b.hi	LBB10_112
; %bb.104:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	x0, #0                          ; =0x0
	add	x1, x20, #123
	add	x1, x1, x16
	sub	x4, x2, #3
LBB10_105:                              ;   Parent Loop BB10_42 Depth=1
                                        ;     Parent Loop BB10_98 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	x6, [x1, x0]
	ldr	x7, [x14, x0]
	cmp	x6, x7
	b.ne	LBB10_117
; %bb.106:                              ;   in Loop: Header=BB10_105 Depth=3
	add	x0, x0, #8
	sub	x4, x4, #8
	cmp	x4, #7
	b.hi	LBB10_105
; %bb.107:                              ;   in Loop: Header=BB10_98 Depth=2
	add	w15, w15, w0
	add	w1, w15, #1
	add	x0, x0, #3
	cmp	x0, x2
	b.lo	LBB10_113
LBB10_108:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	x15, x1
	b	LBB10_118
LBB10_109:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	x0, x17
	mov	x15, x1
LBB10_110:                              ;   in Loop: Header=BB10_98 Depth=2
	add	w12, w12, #1
	cmp	w12, w3
	b.hs	LBB10_125
; %bb.111:                              ;   in Loop: Header=BB10_98 Depth=2
	ldur	x17, [x29, #-104]               ; 8-byte Folded Reload
	ldr	w4, [x17, x16, lsl #2]
	and	w16, w4, #0xffff
	mov	x17, x0
	mov	w1, #65535                      ; =0xffff
	cmp	w16, w1
	b.ne	LBB10_98
	b	LBB10_125
LBB10_112:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	w0, #3                          ; =0x3
	cmp	x0, x2
	b.hs	LBB10_108
LBB10_113:                              ;   in Loop: Header=BB10_98 Depth=2
	add	x4, x19, x16
	add	w15, w1, w2
	sub	w6, w15, w0
	add	w15, w1, #1
LBB10_114:                              ;   Parent Loop BB10_42 Depth=1
                                        ;     Parent Loop BB10_98 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w1, [x4, x0]
	ldrb	w7, [x9, x0]
	cmp	w1, w7
	b.ne	LBB10_118
; %bb.115:                              ;   in Loop: Header=BB10_114 Depth=3
	add	x0, x0, #1
	add	w15, w15, #1
	cmp	x2, x0
	b.ne	LBB10_114
; %bb.116:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	x0, x2
	mov	x15, x6
	b	LBB10_118
LBB10_117:                              ;   in Loop: Header=BB10_98 Depth=2
	eor	x1, x7, x6
	rbit	x1, x1
	clz	x1, x1
	add	w15, w15, w0
	add	w15, w15, #9
	add	x0, x0, x1, lsr #3
	add	x0, x0, #3
LBB10_118:                              ;   in Loop: Header=BB10_98 Depth=2
	cmp	x0, #3
	b.lo	LBB10_122
; %bb.119:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	w1, w17
	cmp	x0, x1
	b.ls	LBB10_122
; %bb.120:                              ;   in Loop: Header=BB10_98 Depth=2
	sub	w10, w8, w16
	cmp	x0, x2
	b.eq	LBB10_125
; %bb.121:                              ;   in Loop: Header=BB10_98 Depth=2
	ldr	x17, [sp, #88]                  ; 8-byte Reload
	cmp	x0, x17
	b.lo	LBB10_110
	b	LBB10_125
LBB10_122:                              ;   in Loop: Header=BB10_98 Depth=2
	mov	x0, x17
	b	LBB10_110
LBB10_123:                              ;   in Loop: Header=BB10_42 Depth=1
	mov	x1, #0                          ; =0x0
	b	LBB10_126
LBB10_124:                              ;   in Loop: Header=BB10_42 Depth=1
	mov	x0, x17
	mov	x15, x1
LBB10_125:                              ;   in Loop: Header=BB10_42 Depth=1
	mov	w8, w0
	orr	x0, x8, x10, lsl #32
	mov	w1, w15
	ldr	w17, [sp, #116]                 ; 4-byte Reload
	ldr	x7, [sp, #16]                   ; 8-byte Reload
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
LBB10_126:                              ;   in Loop: Header=BB10_42 Depth=1
	ldur	w6, [x29, #-120]                ; 4-byte Folded Reload
	ldr	x3, [sp, #96]                   ; 8-byte Reload
	sub	x7, x7, w1, uxtw
	tbnz	x1, #32, LBB10_147
; %bb.127:                              ;   in Loop: Header=BB10_42 Depth=1
	and	x10, x0, #0xffffffff00000000
	add	x19, x20, #120
	ldr	x1, [sp, #104]                  ; 8-byte Reload
	cmp	w24, w0
	b.lo	LBB10_95
LBB10_128:                              ;   in Loop: Header=BB10_42 Depth=1
	cmp	w24, #2
	b.ls	LBB10_39
; %bb.129:                              ;   in Loop: Header=BB10_42 Depth=1
	add	x8, x3, x22, lsl #2
	strh	w24, [x8]
	strh	w1, [x8, #2]
	mov	w8, w24
	cbz	w1, LBB10_131
; %bb.130:                              ;   in Loop: Header=BB10_42 Depth=1
Lloh34:
	adrp	x9, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGE
Lloh35:
	add	x9, x9, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGEOFF
	ldr	w9, [x9, x8, lsl #2]
	sub	w10, w1, #1
	clz	w11, w10
	mov	w12, #30                        ; =0x1e
	sub	w12, w12, w11
	lsr	w12, w10, w12
	and	w12, w12, #0x1
	sub	w11, w12, w11, lsl #1
	add	w11, w11, #62
	cmp	w1, #5
	csel	w10, w10, w11, lo
	add	w11, w9, #257
	ldr	w12, [x25, w11, uxtw #2]
	add	w12, w12, #1
	str	w12, [x25, w11, uxtw #2]
	ldr	x12, [sp, #64]                  ; 8-byte Reload
	ldr	w11, [x12, w10, uxtw #2]
	add	w11, w11, #1
	str	w11, [x12, w10, uxtw #2]
Lloh36:
	adrp	x11, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGE
Lloh37:
	add	x11, x11, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGEOFF
	ldr	w9, [x11, x9, lsl #2]
	lsr	w11, w10, #1
	sub	w11, w11, #1
	cmp	w10, #3
	csel	w10, w11, wzr, hi
	ldr	x11, [sp, #120]                 ; 8-byte Reload
	add	x10, x10, x11
	add	x10, x9, x10
	mov	w9, #6232                       ; =0x1858
	movk	w9, #11, lsl #16
	str	x10, [sp, #120]                 ; 8-byte Spill
	str	x10, [x20, x9]
	cmp	w15, #3
	b.ls	LBB10_132
	b	LBB10_133
LBB10_131:                              ;   in Loop: Header=BB10_42 Depth=1
	ldr	w9, [x25, x8, lsl #2]
	add	w9, w9, #1
	str	w9, [x25, x8, lsl #2]
	cmp	w15, #3
	b.hi	LBB10_133
LBB10_132:                              ;   in Loop: Header=BB10_42 Depth=1
	ldur	w9, [x29, #-116]                ; 4-byte Folded Reload
	cmp	w24, w9
	b.hi	LBB10_137
LBB10_133:                              ;   in Loop: Header=BB10_42 Depth=1
	add	x9, x23, x8
	ldur	x10, [x29, #-112]               ; 8-byte Folded Reload
	cmp	x10, x9
	csel	x9, x10, x9, lo
	add	x10, x23, #1
	cmp	x10, x9
	b.hs	LBB10_137
; %bb.134:                              ;   in Loop: Header=BB10_42 Depth=1
	sub	x9, x23, x9
	add	x9, x9, #1
	add	w10, w23, #1
	ldr	x11, [sp, #8]                   ; 8-byte Reload
	add	x11, x11, x23, lsl #2
	add	x12, x20, #123
	add	x12, x12, x23
LBB10_135:                              ;   Parent Loop BB10_42 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldurb	w13, [x12, #-2]
	ldurb	w14, [x12, #-1]
	ldrb	w15, [x12], #1
	lsl	w16, w13, w21
	eor	w16, w16, w14
	lsl	w16, w16, w21
	eor	w15, w16, w15
	and	w15, w15, w6
	ldr	w16, [x28, w15, uxtw #2]
	str	w16, [x11], #4
	orr	w13, w10, w13, lsl #16
	orr	w13, w13, w14, lsl #24
	str	w13, [x28, w15, uxtw #2]
	add	w10, w10, #1
	adds	x9, x9, #1
	b.lo	LBB10_135
; %bb.136:                              ;   in Loop: Header=BB10_42 Depth=1
	mov	x24, #0                         ; =0x0
	ldur	w15, [x29, #-124]               ; 4-byte Folded Reload
	b	LBB10_41
LBB10_137:                              ;   in Loop: Header=BB10_42 Depth=1
	mov	x24, #0                         ; =0x0
	b	LBB10_41
LBB10_138:
	mov	x22, #0                         ; =0x0
	mov	x7, x9
	mov	x23, x26
LBB10_139:
	cmp	x23, x30
	b.hs	LBB10_143
; %bb.140:
	add	x11, x27, x26
	sub	x8, x11, x23
	add	x9, x23, x20
	add	x9, x9, #120
	add	x10, x20, x22, lsl #2
	add	x10, x10, #144, lsl #12         ; =589824
	add	x10, x10, #142
	add	x11, x11, x22
	sub	x22, x11, x23
	ldp	x14, x13, [sp, #32]             ; 16-byte Folded Reload
LBB10_141:                              ; =>This Inner Loop Header: Depth=1
	ldrb	w11, [x9], #1
	sturh	w11, [x10, #-2]
	ldr	w12, [x25, x11, lsl #2]
	strh	wzr, [x10], #4
	add	w12, w12, #1
	str	w12, [x25, x11, lsl #2]
	subs	x8, x8, #1
	b.ne	LBB10_141
; %bb.142:
	ldr	x8, [sp, #24]                   ; 8-byte Reload
	b	LBB10_144
LBB10_143:
	ldp	x8, x14, [sp, #24]              ; 16-byte Folded Reload
	ldr	x13, [sp, #40]                  ; 8-byte Reload
LBB10_144:
	str	x22, [x8]
	sub	x8, x14, x7
	add	x8, x8, x13
	str	x8, [x20, #72]
LBB10_145:
	mov	w0, #1                          ; =0x1
LBB10_146:
	ldp	x29, x30, [sp, #256]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #240]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #224]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #208]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #192]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #176]            ; 16-byte Folded Reload
	add	sp, sp, #272
	ret
LBB10_147:
	mov	w0, #0                          ; =0x0
	ldp	x8, x9, [sp, #32]               ; 16-byte Folded Reload
	sub	x8, x8, x7
	add	x8, x8, x9
	str	x8, [x20, #72]
	b	LBB10_2
	.loh AdrpAdd	Lloh32, Lloh33
	.loh AdrpAdd	Lloh30, Lloh31
	.loh AdrpAdd	Lloh36, Lloh37
	.loh AdrpAdd	Lloh34, Lloh35
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_ ; -- Begin function _ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_
	.p2align	2
__ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_: ; @_ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_
	.cfi_startproc
; %bb.0:
	ldp	w8, w9, [x1]
	ldp	w10, w11, [x2]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w0, w9, w8, eq
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
lCPI12_0:
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	.p2align	2
__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj: ; @_ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	sub	sp, sp, #224
	stp	x28, x27, [sp, #128]            ; 16-byte Folded Spill
	stp	x26, x25, [sp, #144]            ; 16-byte Folded Spill
	stp	x24, x23, [sp, #160]            ; 16-byte Folded Spill
	stp	x22, x21, [sp, #176]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #192]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #208]            ; 16-byte Folded Spill
	add	x29, sp, #208
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
	mov	x23, x0
	lsl	w8, w21, #6
	add	w8, w8, #512
	ldr	x10, [x0, #40]
	ldr	x9, [x0, #72]
	sub	x10, x10, x9
	cmp	x8, x10
	b.ls	LBB12_3
; %bb.1:
	mov	w0, #0                          ; =0x0
	movi.2s	v0, #5
LBB12_2:
	str	d0, [x23, #48]
	b	LBB12_92
LBB12_3:
	mov	x22, x7
	mov	x19, x5
	mov	x20, x3
	mov	x25, x1
	add	x8, x9, x8
	str	x8, [x23, #72]
	cmp	x4, #1
	b.lt	LBB12_5
; %bb.4:
	mov	x0, x20
	mov	x1, x4
	mov	x24, x6
	bl	_bzero
	mov	x6, x24
LBB12_5:
	lsl	x1, x6, #1
	cmp	x1, #1
	b.lt	LBB12_7
; %bb.6:
	mov	x0, x19
	bl	_bzero
LBB12_7:
	add	x8, x23, #177, lsl #12          ; =724992
	add	x27, x8, #2160
	add	x8, x23, #179, lsl #12          ; =733184
	add	x24, x8, #3176
	mov	w26, w21
	cbz	x26, LBB12_42
; %bb.8:
	mov	w8, w21
	sub	x9, x8, #1
	and	x8, x21, #0x7
	cmp	x9, #7
	b.hs	LBB12_10
; %bb.9:
	mov	x9, #0                          ; =0x0
	mov	w28, #0                         ; =0x0
	b	LBB12_40
LBB12_10:
	mov	x9, #0                          ; =0x0
	mov	w28, #0                         ; =0x0
	and	x10, x21, #0xfffffff8
	add	x11, x25, #16
	b	LBB12_12
LBB12_11:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x9, x9, #8
	mov	x11, x12
	cmp	x10, x9
	b.eq	LBB12_28
LBB12_12:                               ; =>This Inner Loop Header: Depth=1
	ldur	w12, [x11, #-16]
	cbz	w12, LBB12_14
; %bb.13:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x13, x24, w28, uxtw #3
	stp	w12, w9, [x13]
	add	w28, w28, #1
LBB12_14:                               ;   in Loop: Header=BB12_12 Depth=1
	ldur	w12, [x11, #-12]
	cbz	w12, LBB12_16
; %bb.15:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x13, x24, w28, uxtw #3
	add	w14, w9, #1
	stp	w12, w14, [x13]
	add	w28, w28, #1
LBB12_16:                               ;   in Loop: Header=BB12_12 Depth=1
	ldur	w12, [x11, #-8]
	cbz	w12, LBB12_18
; %bb.17:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x13, x24, w28, uxtw #3
	add	w14, w9, #2
	stp	w12, w14, [x13]
	add	w28, w28, #1
LBB12_18:                               ;   in Loop: Header=BB12_12 Depth=1
	ldur	w12, [x11, #-4]
	cbz	w12, LBB12_20
; %bb.19:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x13, x24, w28, uxtw #3
	add	w14, w9, #3
	stp	w12, w14, [x13]
	add	w28, w28, #1
LBB12_20:                               ;   in Loop: Header=BB12_12 Depth=1
	mov	x12, x11
	ldr	w13, [x12], #32
	cbz	w13, LBB12_22
; %bb.21:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x14, x24, w28, uxtw #3
	add	w15, w9, #4
	stp	w13, w15, [x14]
	add	w28, w28, #1
LBB12_22:                               ;   in Loop: Header=BB12_12 Depth=1
	ldr	w13, [x11, #4]
	cbz	w13, LBB12_24
; %bb.23:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x14, x24, w28, uxtw #3
	add	w15, w9, #5
	stp	w13, w15, [x14]
	add	w28, w28, #1
LBB12_24:                               ;   in Loop: Header=BB12_12 Depth=1
	ldr	w13, [x11, #8]
	cbz	w13, LBB12_26
; %bb.25:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x14, x24, w28, uxtw #3
	add	w15, w9, #6
	stp	w13, w15, [x14]
	add	w28, w28, #1
LBB12_26:                               ;   in Loop: Header=BB12_12 Depth=1
	ldr	w11, [x11, #12]
	cbz	w11, LBB12_11
; %bb.27:                               ;   in Loop: Header=BB12_12 Depth=1
	add	x13, x24, w28, uxtw #3
	add	w14, w9, #7
	stp	w11, w14, [x13]
	add	w28, w28, #1
	b	LBB12_11
LBB12_28:
	cbnz	x8, LBB12_40
LBB12_29:
	cmp	w28, #1
	b.hi	LBB12_35
; %bb.30:
	mov	x8, #0                          ; =0x0
	add	x9, x23, #179, lsl #12          ; =733184
	add	x9, x9, #3176
	mov	w10, w21
	mov	w11, #1                         ; =0x1
LBB12_31:                               ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x25, x8, lsl #2]
	cbnz	w12, LBB12_33
; %bb.32:                               ;   in Loop: Header=BB12_31 Depth=1
	add	x12, x9, w28, uxtw #3
	stp	w11, w8, [x12]
	add	w28, w28, #1
LBB12_33:                               ;   in Loop: Header=BB12_31 Depth=1
	cmp	w28, #1
	b.hi	LBB12_35
; %bb.34:                               ;   in Loop: Header=BB12_31 Depth=1
	add	x8, x8, #1
	cmp	x8, x10
	b.lo	LBB12_31
LBB12_35:
	add	x8, x23, #179, lsl #12          ; =733184
	add	x24, x8, #3176
	cbz	w28, LBB12_42
; %bb.36:
	mov	w25, w28
	add	x1, x24, w28, uxtw #3
	clz	x8, x25
	lsl	x8, x8, #1
Ltmp0:                                  ; EH_LABEL
	add	x2, sp, #64
	eor	x3, x8, #0x7e
	mov	x0, x24
	mov	w4, #1                          ; =0x1
	bl	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
Ltmp1:                                  ; EH_LABEL
; %bb.37:
	cmp	w28, #1
	b.ne	LBB12_43
; %bb.38:
	mov	x9, #0                          ; =0x0
	b	LBB12_48
LBB12_39:                               ;   in Loop: Header=BB12_40 Depth=1
	add	x9, x9, #1
	subs	x8, x8, #1
	b.eq	LBB12_29
LBB12_40:                               ; =>This Inner Loop Header: Depth=1
	ldr	w10, [x25, x9, lsl #2]
	cbz	w10, LBB12_39
; %bb.41:                               ;   in Loop: Header=BB12_40 Depth=1
	add	x11, x24, w28, uxtw #3
	stp	w10, w9, [x11]
	add	w28, w28, #1
	b	LBB12_39
LBB12_42:
	mov	w28, #0                         ; =0x0
	mov	w8, #1                          ; =0x1
	b	LBB12_49
LBB12_43:
	mov	x8, #0                          ; =0x0
	add	x9, x23, #179, lsl #12          ; =733184
	add	x9, x9, #3184
	add	x10, x27, #16
	and	x11, x25, #0xfffffffe
	neg	x11, x11
	movi	d0, #0xffffffff00000000
LBB12_44:                               ; =>This Inner Loop Header: Depth=1
	ldur	w12, [x9, #-8]
	stur	x12, [x10, #-24]
	stur	d0, [x10, #-16]
	ldr	w12, [x9], #16
	stur	x12, [x10, #-8]
	str	d0, [x10], #32
	sub	x8, x8, #2
	cmp	x11, x8
	b.ne	LBB12_44
; %bb.45:
	tbnz	w25, #0, LBB12_47
; %bb.46:
	mov	w8, #0                          ; =0x0
	b	LBB12_49
LBB12_47:
	neg	x9, x8
LBB12_48:
	mov	w8, #0                          ; =0x0
	add	x10, x23, #177, lsl #12         ; =724992
	add	x10, x10, #2152
	lsl	x11, x9, #3
	ldr	w11, [x24, x11]
	add	x9, x10, x9, lsl #4
	str	x11, [x9]
	movi	d0, #0xffffffff00000000
	str	d0, [x9, #8]
LBB12_49:
	mov	w9, #6244                       ; =0x1864
	movk	w9, #11, lsl #16
	lsl	w10, w28, #1
	sub	w11, w10, #1
	cmp	w28, w11
	b.hs	LBB12_61
; %bb.50:
	mov	w13, #0                         ; =0x0
	add	x12, x23, #177, lsl #12         ; =724992
	add	x12, x12, #2152
	mov	w14, w28
	add	x15, x9, w28, uxtw #4
	add	x15, x15, x23
	add	x15, x15, #12
	movi	d0, #0xffffffff00000000
	mov	x16, x28
	b	LBB12_53
LBB12_51:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	x0, x16
	add	w16, w16, #1
LBB12_52:                               ;   in Loop: Header=BB12_53 Depth=1
	add	x17, x12, w17, uxtw #4
	ldr	x1, [x17]
	add	x0, x12, w0, uxtw #4
	ldr	x2, [x0]
	add	x1, x2, x1
	stur	x1, [x15, #-8]
	str	d0, [x15], #16
	str	w14, [x17, #12]
	str	w14, [x0, #12]
	add	x14, x14, #1
	cmp	x11, x14
	b.eq	LBB12_61
LBB12_53:                               ; =>This Inner Loop Header: Depth=1
	cmp	w13, w28
	b.hs	LBB12_56
; %bb.54:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	w17, w16
	cmp	x14, x17
	b.eq	LBB12_57
; %bb.55:                               ;   in Loop: Header=BB12_53 Depth=1
	ubfiz	x0, x13, #4, #32
	ldr	x0, [x12, x0]
	lsl	x17, x17, #4
	ldr	x17, [x12, x17]
	cmp	x0, x17
	b.ls	LBB12_57
LBB12_56:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	x17, x16
	add	w16, w16, #1
	cmp	w13, w28
	b.hs	LBB12_51
	b	LBB12_58
LBB12_57:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	x17, x13
	add	w13, w13, #1
                                        ; kill: def $w13 killed $w13 def $x13
	cmp	w13, w28
	b.hs	LBB12_51
LBB12_58:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	w0, w16
	cmp	x14, x0
	b.eq	LBB12_60
; %bb.59:                               ;   in Loop: Header=BB12_53 Depth=1
	ubfiz	x1, x13, #4, #32
	ldr	x1, [x12, x1]
	lsl	x0, x0, #4
	ldr	x0, [x12, x0]
	cmp	x1, x0
	b.hi	LBB12_51
LBB12_60:                               ;   in Loop: Header=BB12_53 Depth=1
	mov	x0, x13
	add	w13, w13, #1
                                        ; kill: def $w13 killed $w13 def $x13
	b	LBB12_52
LBB12_61:
	subs	w10, w10, #2
	b.eq	LBB12_64
; %bb.62:
	add	x11, x23, #177, lsl #12         ; =724992
	add	x11, x11, #2152
	add	x9, x9, w10, uxtw #4
	add	x9, x23, x9
LBB12_63:                               ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x9]
	add	x12, x11, x12, lsl #4
	ldr	w12, [x12, #8]
	add	w12, w12, #1
	stur	w12, [x9, #-4]
	ldur	w12, [x9, #-16]
	add	x12, x11, x12, lsl #4
	ldr	w12, [x12, #8]
	add	w12, w12, #1
	stur	w12, [x9, #-20]
	sub	x9, x9, #32
	sub	x10, x10, #2
	cbnz	x10, LBB12_63
LBB12_64:
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [sp, #96]
	stp	q0, q0, [sp, #64]
	tbnz	w8, #0, LBB12_74
; %bb.65:
	mov	w8, #0                          ; =0x0
	add	x9, sp, #64
	mov	w10, #1                         ; =0x1
	mov	w11, w28
LBB12_66:                               ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x27], #16
	cmp	w22, w12
	csel	w12, w22, w12, lo
	ldr	w13, [x9, w12, uxtw #2]
	add	w13, w13, #1
	str	w13, [x9, w12, uxtw #2]
	sub	w12, w22, w12
	lsl	w12, w10, w12
	add	w8, w12, w8
	subs	x11, x11, #1
	b.ne	LBB12_66
; %bb.67:
	mov	w9, #1                          ; =0x1
	lsl	w9, w9, w22
	cmp	w8, w9
	b.ls	LBB12_74
; %bb.68:
	sub	w11, w22, #1
	add	x10, sp, #64
	add	x11, x10, w11, uxtw #2
	add	x11, x11, #4
LBB12_69:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB12_70 Depth 2
	mov	x13, x11
	add	w12, w22, #1
LBB12_70:                               ;   Parent Loop BB12_69 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cmp	w12, #2
	b.eq	LBB12_93
; %bb.71:                               ;   in Loop: Header=BB12_70 Depth=2
	ldr	w14, [x13, #-4]!
	sub	w12, w12, #1
	cbz	w14, LBB12_70
; %bb.72:                               ;   in Loop: Header=BB12_69 Depth=1
	ldr	w15, [x10, w22, uxtw #2]
	cbz	w15, LBB12_93
; %bb.73:                               ;   in Loop: Header=BB12_69 Depth=1
	sub	w14, w14, #1
	str	w14, [x13]
	ldr	w13, [x10, w12, uxtw #2]
	add	w13, w13, #2
	str	w13, [x10, w12, uxtw #2]
	ldr	w12, [x10, w22, uxtw #2]
	sub	w12, w12, #1
	str	w12, [x10, w22, uxtw #2]
	sub	w8, w8, #1
	cmp	w8, w9
	b.hi	LBB12_69
LBB12_74:
	cbz	w22, LBB12_84
; %bb.75:
	mov	w9, #0                          ; =0x0
	mov	w8, w22
	add	x10, sp, #64
	b	LBB12_77
LBB12_76:                               ;   in Loop: Header=BB12_77 Depth=1
	sub	x8, x8, #1
	cbz	w8, LBB12_81
LBB12_77:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB12_79 Depth 2
	ldr	w11, [x10, x8, lsl #2]
	cbz	w11, LBB12_76
; %bb.78:                               ;   in Loop: Header=BB12_77 Depth=1
	add	w12, w9, w11
LBB12_79:                               ;   Parent Loop BB12_77 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x13, x24, w9, uxtw #3
	ldr	w13, [x13, #4]
	strb	w8, [x20, x13]
	add	w9, w9, #1
	subs	w11, w11, #1
	b.ne	LBB12_79
; %bb.80:                               ;   in Loop: Header=BB12_77 Depth=1
	mov	x9, x12
	b	LBB12_76
LBB12_81:
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [sp, #32]
	stp	q0, q0, [sp]
	add	w9, w22, #1
	cmp	w9, #2
	mov	w9, #2                          ; =0x2
	csinc	w10, w9, w22, ls
	mov	x9, sp
	orr	x9, x9, #0x4
	sub	x10, x10, #1
	add	x11, sp, #64
LBB12_82:                               ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x11], #4
	add	w8, w12, w8
	lsl	w8, w8, #1
	str	w8, [x9], #4
	subs	x10, x10, #1
	b.ne	LBB12_82
; %bb.83:
	cbnz	x26, LBB12_85
	b	LBB12_91
LBB12_84:
	movi.2d	v0, #0000000000000000
	stp	q0, q0, [sp, #32]
	stp	q0, q0, [sp]
	cbz	x26, LBB12_91
LBB12_85:
	mov	x8, #0                          ; =0x0
	mov	w9, w21
	mov	x10, sp
	b	LBB12_87
LBB12_86:                               ;   in Loop: Header=BB12_87 Depth=1
	add	x8, x8, #1
	cmp	x8, x9
	b.eq	LBB12_91
LBB12_87:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB12_89 Depth 2
	ldrb	w11, [x20, x8]
	cbz	w11, LBB12_86
; %bb.88:                               ;   in Loop: Header=BB12_87 Depth=1
	mov	w13, #0                         ; =0x0
	ldr	w12, [x10, w11, uxtw #2]
	mov	x14, x11
	mov	x15, x12
LBB12_89:                               ;   Parent Loop BB12_87 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x16, x13
	mov	x13, x15
	lsr	w15, w15, #1
	bfi	w13, w16, #1, #31
	subs	w14, w14, #1
	b.ne	LBB12_89
; %bb.90:                               ;   in Loop: Header=BB12_87 Depth=1
	strh	w13, [x19, x8, lsl #1]
	add	w12, w12, #1
	str	w12, [x10, w11, uxtw #2]
	b	LBB12_86
LBB12_91:
	mov	w0, #1                          ; =0x1
LBB12_92:
	ldp	x29, x30, [sp, #208]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #192]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #176]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #160]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #144]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #128]            ; 16-byte Folded Reload
	add	sp, sp, #224
	ret
LBB12_93:
	mov	w0, #0                          ; =0x0
Lloh38:
	adrp	x8, lCPI12_0@PAGE
Lloh39:
	ldr	d0, [x8, lCPI12_0@PAGEOFF]
	b	LBB12_2
LBB12_94:
Ltmp2:                                  ; EH_LABEL
	bl	___clang_call_terminate
	.loh AdrpLdr	Lloh38, Lloh39
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table12:
Lexception0:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
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
	.byte	1                               ;   On action: 1
Lcst_end0:
	.byte	1                               ; >> Action Record 1 <<
                                        ;   Catch TypeInfo 1
	.byte	0                               ;   No further actions
	.p2align	2, 0x0
                                        ; >> Catch TypeInfos <<
	.long	0                               ; TypeInfo 1
Lttbase0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream15prepare_dynamicEv ; -- Begin function _ZN8stx_zlib13DeflateStream15prepare_dynamicEv
	.p2align	2
__ZN8stx_zlib13DeflateStream15prepare_dynamicEv: ; @_ZN8stx_zlib13DeflateStream15prepare_dynamicEv
	.cfi_startproc
; %bb.0:
	stp	d11, d10, [sp, #-128]!          ; 16-byte Folded Spill
	stp	d9, d8, [sp, #16]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #32]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	sub	sp, sp, #400
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
	.cfi_offset b10, -120
	.cfi_offset b11, -128
	mov	x21, x0
Lloh40:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh41:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh42:
	ldr	x8, [x8]
	stur	x8, [x29, #-120]
	add	x8, x0, #176, lsl #12           ; =720896
	add	x19, x8, #152
	add	x8, x0, #176, lsl #12           ; =720896
	add	x20, x8, #1416
	add	x8, x0, #176, lsl #12           ; =720896
	add	x5, x8, #1732
	mov	x1, x19
	mov	w2, #286                        ; =0x11e
	mov	x3, x20
	mov	w4, #286                        ; =0x11e
	mov	w6, #286                        ; =0x11e
	mov	w7, #15                         ; =0xf
	bl	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	cbz	w0, LBB13_114
; %bb.1:
	add	x1, x19, #1144
	add	x3, x19, #1550
	add	x8, x21, #176, lsl #12          ; =720896
	add	x5, x8, #2304
	mov	x0, x21
	mov	w2, #30                         ; =0x1e
	mov	w4, #30                         ; =0x1e
	mov	w6, #30                         ; =0x1e
	mov	w7, #15                         ; =0xf
	bl	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	cbz	w0, LBB13_114
; %bb.2:
	add	x8, x21, #177, lsl #12          ; =724992
	add	x23, x8, #2120
	mov	w24, #286                       ; =0x11e
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1549]
	cbnz	w8, LBB13_31
; %bb.3:
	mov	w24, #285                       ; =0x11d
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1548]
	cbnz	w8, LBB13_31
; %bb.4:
	mov	w24, #284                       ; =0x11c
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1547]
	cbnz	w8, LBB13_31
; %bb.5:
	mov	w24, #283                       ; =0x11b
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1546]
	cbnz	w8, LBB13_31
; %bb.6:
	mov	w24, #282                       ; =0x11a
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1545]
	cbnz	w8, LBB13_31
; %bb.7:
	mov	w24, #281                       ; =0x119
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1544]
	cbnz	w8, LBB13_31
; %bb.8:
	mov	w24, #280                       ; =0x118
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1543]
	cbnz	w8, LBB13_31
; %bb.9:
	mov	w24, #279                       ; =0x117
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1542]
	cbnz	w8, LBB13_31
; %bb.10:
	mov	w24, #278                       ; =0x116
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1541]
	cbnz	w8, LBB13_31
; %bb.11:
	mov	w24, #277                       ; =0x115
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1540]
	cbnz	w8, LBB13_31
; %bb.12:
	mov	w24, #276                       ; =0x114
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1539]
	cbnz	w8, LBB13_31
; %bb.13:
	mov	w24, #275                       ; =0x113
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1538]
	cbnz	w8, LBB13_31
; %bb.14:
	mov	w24, #274                       ; =0x112
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1537]
	cbnz	w8, LBB13_31
; %bb.15:
	mov	w24, #273                       ; =0x111
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1536]
	cbnz	w8, LBB13_31
; %bb.16:
	mov	w24, #272                       ; =0x110
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1535]
	cbnz	w8, LBB13_31
; %bb.17:
	mov	w24, #271                       ; =0x10f
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1534]
	cbnz	w8, LBB13_31
; %bb.18:
	mov	w24, #270                       ; =0x10e
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1533]
	cbnz	w8, LBB13_31
; %bb.19:
	mov	w24, #269                       ; =0x10d
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1532]
	cbnz	w8, LBB13_31
; %bb.20:
	mov	w24, #268                       ; =0x10c
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1531]
	cbnz	w8, LBB13_31
; %bb.21:
	mov	w24, #267                       ; =0x10b
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1530]
	cbnz	w8, LBB13_31
; %bb.22:
	mov	w24, #266                       ; =0x10a
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1529]
	cbnz	w8, LBB13_31
; %bb.23:
	mov	w24, #265                       ; =0x109
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1528]
	cbnz	w8, LBB13_31
; %bb.24:
	mov	w24, #264                       ; =0x108
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1527]
	cbnz	w8, LBB13_31
; %bb.25:
	mov	w24, #263                       ; =0x107
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1526]
	cbnz	w8, LBB13_31
; %bb.26:
	mov	w24, #262                       ; =0x106
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1525]
	cbnz	w8, LBB13_31
; %bb.27:
	mov	w24, #261                       ; =0x105
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1524]
	cbnz	w8, LBB13_31
; %bb.28:
	mov	w24, #260                       ; =0x104
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1523]
	cbnz	w8, LBB13_31
; %bb.29:
	mov	w24, #259                       ; =0x103
	str	w24, [x23, #4]
	ldrb	w8, [x19, #1522]
	cbnz	w8, LBB13_31
; %bb.30:
	ldrb	w8, [x19, #1521]
	cmp	w8, #0
	mov	w8, #257                        ; =0x101
	cinc	w24, w8, ne
	str	w24, [x23, #4]
LBB13_31:
	mov	w26, #30                        ; =0x1e
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1579]
	cbnz	w8, LBB13_60
; %bb.32:
	mov	w26, #29                        ; =0x1d
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1578]
	cbnz	w8, LBB13_60
; %bb.33:
	mov	w26, #28                        ; =0x1c
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1577]
	cbnz	w8, LBB13_60
; %bb.34:
	mov	w26, #27                        ; =0x1b
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1576]
	cbnz	w8, LBB13_60
; %bb.35:
	mov	w26, #26                        ; =0x1a
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1575]
	cbnz	w8, LBB13_60
; %bb.36:
	mov	w26, #25                        ; =0x19
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1574]
	cbnz	w8, LBB13_60
; %bb.37:
	mov	w26, #24                        ; =0x18
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1573]
	cbnz	w8, LBB13_60
; %bb.38:
	mov	w26, #23                        ; =0x17
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1572]
	cbnz	w8, LBB13_60
; %bb.39:
	mov	w26, #22                        ; =0x16
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1571]
	cbnz	w8, LBB13_60
; %bb.40:
	mov	w26, #21                        ; =0x15
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1570]
	cbnz	w8, LBB13_60
; %bb.41:
	mov	w26, #20                        ; =0x14
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1569]
	cbnz	w8, LBB13_60
; %bb.42:
	mov	w26, #19                        ; =0x13
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1568]
	cbnz	w8, LBB13_60
; %bb.43:
	mov	w26, #18                        ; =0x12
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1567]
	cbnz	w8, LBB13_60
; %bb.44:
	mov	w26, #17                        ; =0x11
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1566]
	cbnz	w8, LBB13_60
; %bb.45:
	mov	w26, #16                        ; =0x10
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1565]
	cbnz	w8, LBB13_60
; %bb.46:
	mov	w26, #15                        ; =0xf
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1564]
	cbnz	w8, LBB13_60
; %bb.47:
	mov	w26, #14                        ; =0xe
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1563]
	cbnz	w8, LBB13_60
; %bb.48:
	mov	w26, #13                        ; =0xd
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1562]
	cbnz	w8, LBB13_60
; %bb.49:
	mov	w26, #12                        ; =0xc
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1561]
	cbnz	w8, LBB13_60
; %bb.50:
	mov	w26, #11                        ; =0xb
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1560]
	cbnz	w8, LBB13_60
; %bb.51:
	mov	w26, #10                        ; =0xa
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1559]
	cbnz	w8, LBB13_60
; %bb.52:
	mov	w26, #9                         ; =0x9
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1558]
	cbnz	w8, LBB13_60
; %bb.53:
	mov	w26, #8                         ; =0x8
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1557]
	cbnz	w8, LBB13_60
; %bb.54:
	mov	w26, #7                         ; =0x7
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1556]
	cbnz	w8, LBB13_60
; %bb.55:
	mov	w26, #6                         ; =0x6
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1555]
	cbnz	w8, LBB13_60
; %bb.56:
	mov	w26, #5                         ; =0x5
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1554]
	cbnz	w8, LBB13_60
; %bb.57:
	mov	w26, #4                         ; =0x4
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1553]
	cbnz	w8, LBB13_60
; %bb.58:
	mov	w26, #3                         ; =0x3
	str	w26, [x23, #8]
	ldrb	w8, [x19, #1552]
	cbnz	w8, LBB13_60
; %bb.59:
	ldrb	w8, [x19, #1551]
	cmp	w8, #0
	mov	w8, #1                          ; =0x1
	cinc	w26, w8, ne
	str	w26, [x23, #8]
LBB13_60:
	mov	w22, w24
	mov	w8, #316                        ; =0x13c
	sub	x1, x8, x22
	add	x25, sp, #76
	add	x0, x25, x22
	bl	_bzero
	add	x0, sp, #76
	mov	x1, x20
	mov	x2, x22
	bl	_memcpy
	mov	w2, w26
	add	x0, x25, x22
	add	x1, x19, #1550
	bl	_memcpy
	mov	w9, #0                          ; =0x0
	mov	w0, #0                          ; =0x0
	add	w8, w26, w24
	movi.2d	v0, #0000000000000000
	stur	q0, [sp, #60]
	add	x10, x21, #176, lsl #12         ; =720896
	stp	q0, q0, [sp, #32]
	add	x10, x10, #2424
	mov	w11, #12                        ; =0xc
	mov	x12, sp
	mov	w13, #138                       ; =0x8a
	stp	q0, q0, [sp]
	str	wzr, [x23]
	mov	w14, #10                        ; =0xa
	mov	w15, #6                         ; =0x6
	mov	w16, #2                         ; =0x2
	mov	w17, #16                        ; =0x10
	b	LBB13_62
LBB13_61:                               ;   in Loop: Header=BB13_62 Depth=1
	cmp	w0, w8
	b.hs	LBB13_81
LBB13_62:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB13_64 Depth 2
                                        ;     Child Loop BB13_72 Depth 2
	ldrb	w1, [x25, w0, uxtw]
	add	w3, w0, #1
	cmp	w3, w8
	b.hs	LBB13_67
; %bb.63:                               ;   in Loop: Header=BB13_62 Depth=1
	sub	w2, w8, w0
	neg	x4, x2
	mov	x0, #-1                         ; =0xffffffffffffffff
LBB13_64:                               ;   Parent Loop BB13_62 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldrb	w5, [x25, w3, uxtw]
	cmp	w5, w1
	b.ne	LBB13_80
; %bb.65:                               ;   in Loop: Header=BB13_64 Depth=2
	sub	x0, x0, #1
	add	w3, w3, #1
	cmp	x4, x0
	b.ne	LBB13_64
; %bb.66:                               ;   in Loop: Header=BB13_62 Depth=1
	mov	x0, x8
	cbnz	w1, LBB13_68
	b	LBB13_69
LBB13_67:                               ;   in Loop: Header=BB13_62 Depth=1
	mov	w2, #1                          ; =0x1
	mov	x0, x3
	cbz	w1, LBB13_69
LBB13_68:                               ;   in Loop: Header=BB13_62 Depth=1
	umaddl	x9, w9, w11, x10
	stp	wzr, wzr, [x9, #4]
	str	w1, [x9]
	ldr	w9, [x23]
	add	w9, w9, #1
	str	w9, [x23]
	ldr	w3, [x12, w1, uxtw #2]
	add	w3, w3, #1
	str	w3, [x12, w1, uxtw #2]
	sub	w2, w2, #1
LBB13_69:                               ;   in Loop: Header=BB13_62 Depth=1
	cbnz	w2, LBB13_72
	b	LBB13_61
LBB13_70:                               ;   in Loop: Header=BB13_72 Depth=2
	cmp	w2, #138
	csel	w3, w2, w13, lo
	sub	w4, w3, #11
	mov	w6, #7                          ; =0x7
	mov	w5, #18                         ; =0x12
LBB13_71:                               ;   in Loop: Header=BB13_72 Depth=2
	umaddl	x9, w9, w11, x10
	stp	w5, w4, [x9]
	str	w6, [x9, #8]
	ldr	w9, [x23]
	add	w9, w9, #1
	str	w9, [x23]
	ldr	w4, [x12, w5, uxtw #2]
	add	w4, w4, #1
	str	w4, [x12, w5, uxtw #2]
	subs	w2, w2, w3
	b.eq	LBB13_61
LBB13_72:                               ;   Parent Loop BB13_62 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cbnz	w1, LBB13_74
; %bb.73:                               ;   in Loop: Header=BB13_72 Depth=2
	cmp	w2, #11
	b.hs	LBB13_70
LBB13_74:                               ;   in Loop: Header=BB13_72 Depth=2
	cbnz	w1, LBB13_77
; %bb.75:                               ;   in Loop: Header=BB13_72 Depth=2
	cmp	w2, #3
	b.lo	LBB13_77
; %bb.76:                               ;   in Loop: Header=BB13_72 Depth=2
	cmp	w2, #10
	csel	w3, w2, w14, lo
	sub	w4, w3, #3
	mov	w6, #3                          ; =0x3
	mov	w5, #17                         ; =0x11
	b	LBB13_71
LBB13_77:                               ;   in Loop: Header=BB13_72 Depth=2
	cbz	w1, LBB13_79
; %bb.78:                               ;   in Loop: Header=BB13_72 Depth=2
	cmp	w2, #6
	csel	w3, w2, w15, lo
	sub	w4, w3, #3
	cmp	w2, #3
	csel	w5, w1, w17, lo
	csinc	w3, w3, wzr, hs
	csel	w4, wzr, w4, lo
	csel	w6, wzr, w16, lo
	b	LBB13_71
LBB13_79:                               ;   in Loop: Header=BB13_72 Depth=2
	mov	w4, #0                          ; =0x0
	mov	w6, #0                          ; =0x0
	mov	w3, #1                          ; =0x1
	mov	x5, x1
	b	LBB13_71
LBB13_80:                               ;   in Loop: Header=BB13_62 Depth=1
	neg	w2, w0
	mov	x0, x3
	cbnz	w1, LBB13_68
	b	LBB13_69
LBB13_81:
	add	x8, x21, #176, lsl #12          ; =720896
	add	x20, x8, #2364
	mov	w22, #19                        ; =0x13
	mov	x1, sp
	add	x8, x21, #176, lsl #12          ; =720896
	add	x5, x8, #2384
	mov	x0, x21
	mov	w2, #19                         ; =0x13
	mov	x3, x20
	mov	w4, #19                         ; =0x13
	mov	w6, #19                         ; =0x13
	mov	w7, #7                          ; =0x7
	bl	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	cbz	w0, LBB13_114
; %bb.82:
	str	w22, [x23, #12]
	ldrb	w8, [x19, #2227]
	cbz	w8, LBB13_84
; %bb.83:
	mov	w8, #74                         ; =0x4a
	b	LBB13_111
LBB13_84:
	mov	w8, #18                         ; =0x12
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2213]
	cbz	w8, LBB13_86
; %bb.85:
	mov	w8, #71                         ; =0x47
	b	LBB13_111
LBB13_86:
	mov	w8, #17                         ; =0x11
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2226]
	cbz	w8, LBB13_88
; %bb.87:
	mov	w8, #68                         ; =0x44
	b	LBB13_111
LBB13_88:
	mov	w8, #16                         ; =0x10
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2214]
	cbz	w8, LBB13_90
; %bb.89:
	mov	w8, #65                         ; =0x41
	b	LBB13_111
LBB13_90:
	mov	w8, #15                         ; =0xf
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2225]
	cbz	w8, LBB13_92
; %bb.91:
	mov	w8, #62                         ; =0x3e
	b	LBB13_111
LBB13_92:
	mov	w8, #14                         ; =0xe
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2215]
	cbz	w8, LBB13_94
; %bb.93:
	mov	w8, #59                         ; =0x3b
	b	LBB13_111
LBB13_94:
	mov	w8, #13                         ; =0xd
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2224]
	cbz	w8, LBB13_96
; %bb.95:
	mov	w8, #56                         ; =0x38
	b	LBB13_111
LBB13_96:
	mov	w8, #12                         ; =0xc
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2216]
	cbz	w8, LBB13_98
; %bb.97:
	mov	w8, #53                         ; =0x35
	b	LBB13_111
LBB13_98:
	mov	w8, #11                         ; =0xb
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2223]
	cbz	w8, LBB13_100
; %bb.99:
	mov	w8, #50                         ; =0x32
	b	LBB13_111
LBB13_100:
	mov	w8, #10                         ; =0xa
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2217]
	cbz	w8, LBB13_102
; %bb.101:
	mov	w8, #47                         ; =0x2f
	b	LBB13_111
LBB13_102:
	mov	w8, #9                          ; =0x9
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2222]
	cbz	w8, LBB13_104
; %bb.103:
	mov	w8, #44                         ; =0x2c
	b	LBB13_111
LBB13_104:
	mov	w8, #8                          ; =0x8
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2218]
	cbz	w8, LBB13_106
; %bb.105:
	mov	w8, #41                         ; =0x29
	b	LBB13_111
LBB13_106:
	mov	w8, #7                          ; =0x7
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2221]
	cbz	w8, LBB13_108
; %bb.107:
	mov	w8, #38                         ; =0x26
	b	LBB13_111
LBB13_108:
	mov	w8, #6                          ; =0x6
	str	w8, [x23, #12]
	ldrb	w8, [x19, #2219]
	cbz	w8, LBB13_110
; %bb.109:
	mov	w8, #35                         ; =0x23
	b	LBB13_111
LBB13_110:
	ldrb	w8, [x19, #2220]
	cmp	w8, #0
	mov	w8, #4                          ; =0x4
	cinc	w8, w8, ne
	str	w8, [x23, #12]
	mov	w8, #32                         ; =0x20
	mov	w9, #29                         ; =0x1d
	csel	x8, x9, x8, eq
LBB13_111:
	movi.2d	v16, #0000000000000000
	ldr	x9, [x23, #16]
	add	x8, x9, x8
	mov.d	v16[0], x8
	ldr	d0, [x19, #1264]
	ushll.8h	v0, v0, #0
	ushll2.4s	v1, v0, #0
	ushll.4s	v0, v0, #0
	ldr	d2, [x19, #1272]
	ushll.8h	v2, v2, #0
	ushll2.4s	v3, v2, #0
	ldr	d4, [x19, #1280]
	ushll.4s	v2, v2, #0
	ushll.8h	v4, v4, #0
	ldr	d5, [x19, #1288]
	ushll.4s	v17, v4, #0
	ushll2.4s	v4, v4, #0
	ushll.8h	v5, v5, #0
	ldp	q6, q7, [x19]
	ushll2.4s	v18, v5, #0
	ushll.4s	v5, v5, #0
	mul.4s	v6, v6, v0
	ldp	q0, q19, [x19, #32]
	mul.4s	v7, v7, v1
	mul.4s	v0, v0, v2
	mul.4s	v1, v19, v3
	ldp	q3, q2, [x19, #64]
	mul.4s	v2, v2, v4
	mul.4s	v3, v3, v17
	ldp	q4, q17, [x19, #96]
	mul.4s	v4, v4, v5
	mul.4s	v5, v17, v18
	uaddw.2d	v17, v16, v6
	ldr	d16, [x19, #1296]
	ushll.8h	v16, v16, #0
	ldr	d18, [x19, #1304]
	ushll2.4s	v19, v16, #0
	ushll.4s	v16, v16, #0
	ushll.8h	v18, v18, #0
	ldp	q21, q20, [x19, #128]
	ushll2.4s	v22, v18, #0
	ldr	d23, [x19, #1312]
	ushll.4s	v18, v18, #0
	ushll.8h	v23, v23, #0
	ldr	d24, [x19, #1320]
	ushll.4s	v25, v23, #0
	ushll2.4s	v23, v23, #0
	ushll.8h	v24, v24, #0
	ldp	q27, q26, [x19, #160]
	ushll2.4s	v28, v24, #0
	ushll.4s	v24, v24, #0
	mul.4s	v21, v21, v16
	ldp	q16, q29, [x19, #192]
	mul.4s	v19, v20, v19
	mul.4s	v18, v27, v18
	mul.4s	v26, v26, v22
	ldp	q22, q20, [x19, #224]
	mul.4s	v23, v29, v23
	mul.4s	v27, v16, v25
	mul.4s	v24, v22, v24
	mul.4s	v28, v20, v28
	uaddl2.2d	v6, v6, v21
	uaddl2.2d	v16, v7, v19
	uaddw.2d	v20, v17, v21
	uaddl.2d	v22, v7, v19
	uaddl2.2d	v7, v0, v18
	uaddl2.2d	v17, v1, v26
	uaddl.2d	v21, v0, v18
	uaddl.2d	v25, v1, v26
	uaddl.2d	v0, v2, v23
	uaddl.2d	v18, v3, v27
	uaddl2.2d	v23, v2, v23
	uaddl2.2d	v26, v3, v27
	uaddl2.2d	v1, v4, v24
	uaddl2.2d	v19, v5, v28
	uaddl.2d	v24, v4, v24
	uaddl.2d	v27, v5, v28
	ldp	q2, q3, [x19, #256]
	ldp	q4, q5, [x19, #288]
	ldp	q29, q28, [x19, #320]
	ldr	q30, [x19, #368]
	ldr	d31, [x19, #1328]
	ushll.8h	v31, v31, #0
	ushll2.4s	v8, v31, #0
	mul.4s	v3, v3, v8
	ldr	d8, [x19, #1336]
	ushll.4s	v31, v31, #0
	ushll.8h	v8, v8, #0
	mul.4s	v31, v2, v31
	ushll2.4s	v2, v8, #0
	mul.4s	v9, v5, v2
	ldr	d2, [x19, #1344]
	ushll.4s	v5, v8, #0
	ushll.8h	v2, v2, #0
	mul.4s	v4, v4, v5
	ushll.4s	v5, v2, #0
	mul.4s	v29, v29, v5
	ldr	d5, [x19, #1352]
	ushll2.4s	v2, v2, #0
	ushll.8h	v5, v5, #0
	mul.4s	v8, v28, v2
	ushll2.4s	v2, v5, #0
	mul.4s	v30, v30, v2
	ldr	q2, [x19, #352]
	ushll.4s	v5, v5, #0
	mul.4s	v10, v2, v5
	uaddw.2d	v2, v22, v3
	uaddw.2d	v5, v20, v31
	uaddw2.2d	v20, v16, v3
	uaddw2.2d	v6, v6, v31
	uaddw.2d	v3, v25, v9
	uaddw.2d	v22, v21, v4
	uaddw2.2d	v21, v17, v9
	uaddw2.2d	v28, v7, v4
	uaddw2.2d	v4, v26, v29
	uaddw2.2d	v7, v23, v8
	uaddw.2d	v25, v18, v29
	uaddw.2d	v29, v0, v8
	uaddw.2d	v0, v27, v30
	uaddw.2d	v17, v24, v10
	uaddw2.2d	v26, v19, v30
	uaddw2.2d	v1, v1, v10
	ldp	q18, q16, [x19, #384]
	ldp	q23, q19, [x19, #416]
	ldp	q24, q27, [x19, #448]
	ldr	q30, [x19, #480]
	ldr	d31, [x19, #1360]
	ushll.8h	v31, v31, #0
	ushll.4s	v8, v31, #0
	mul.4s	v18, v18, v8
	ldr	d8, [x19, #1368]
	ushll2.4s	v31, v31, #0
	ushll.8h	v8, v8, #0
	mul.4s	v31, v16, v31
	ushll.4s	v16, v8, #0
	mul.4s	v23, v23, v16
	ldr	d16, [x19, #1376]
	ushll2.4s	v8, v8, #0
	ushll.8h	v16, v16, #0
	mul.4s	v19, v19, v8
	ushll2.4s	v8, v16, #0
	mul.4s	v27, v27, v8
	ldr	d8, [x19, #1384]
	ushll.4s	v16, v16, #0
	ushll.8h	v8, v8, #0
	mul.4s	v9, v24, v16
	ushll.4s	v16, v8, #0
	mul.4s	v30, v30, v16
	ldr	q16, [x19, #496]
	ushll2.4s	v24, v8, #0
	mul.4s	v8, v16, v24
	uaddw2.2d	v16, v6, v18
	uaddw2.2d	v24, v20, v31
	uaddw.2d	v20, v5, v18
	uaddw.2d	v2, v2, v31
	uaddw2.2d	v6, v28, v23
	uaddw2.2d	v21, v21, v19
	uaddw.2d	v18, v22, v23
	uaddw.2d	v23, v3, v19
	uaddw.2d	v3, v29, v27
	uaddw.2d	v19, v25, v9
	uaddw2.2d	v7, v7, v27
	uaddw2.2d	v22, v4, v9
	uaddw2.2d	v1, v1, v30
	uaddw2.2d	v5, v26, v8
	uaddw.2d	v4, v17, v30
	uaddw.2d	v17, v0, v8
	ldp	q0, q25, [x19, #512]
	ldp	q26, q27, [x19, #544]
	ldp	q29, q28, [x19, #576]
	ldr	q30, [x19, #624]
	ldr	d31, [x19, #1392]
	ushll.8h	v31, v31, #0
	ushll2.4s	v8, v31, #0
	mul.4s	v25, v25, v8
	ldr	d8, [x19, #1400]
	ushll.4s	v31, v31, #0
	ushll.8h	v8, v8, #0
	mul.4s	v31, v0, v31
	ushll2.4s	v0, v8, #0
	mul.4s	v27, v27, v0
	ldr	d0, [x19, #1408]
	ushll.4s	v8, v8, #0
	ushll.8h	v0, v0, #0
	mul.4s	v8, v26, v8
	ushll.4s	v26, v0, #0
	mul.4s	v29, v29, v26
	ldr	d26, [x19, #1416]
	ushll2.4s	v0, v0, #0
	ushll.8h	v26, v26, #0
	mul.4s	v28, v28, v0
	ushll2.4s	v0, v26, #0
	mul.4s	v30, v30, v0
	ldr	q0, [x19, #608]
	ushll.4s	v26, v26, #0
	mul.4s	v9, v0, v26
	uaddw.2d	v0, v2, v25
	uaddw2.2d	v2, v24, v25
	ldp	q11, q10, [x19, #640]
	uaddw.2d	v25, v20, v31
	uaddw2.2d	v26, v16, v31
	uaddw.2d	v16, v23, v27
	uaddw2.2d	v20, v21, v27
	ldp	q31, q27, [x19, #672]
	uaddw.2d	v21, v18, v8
	uaddw2.2d	v23, v6, v8
	uaddw2.2d	v6, v22, v29
	uaddw.2d	v18, v19, v29
	ldp	q29, q8, [x19, #704]
	uaddw2.2d	v22, v7, v28
	uaddw.2d	v24, v3, v28
	ldr	q3, [x19, #736]
	uaddw.2d	v7, v17, v30
	uaddw2.2d	v5, v5, v30
	ldr	d17, [x19, #1424]
	uaddw.2d	v19, v4, v9
	uaddw2.2d	v4, v1, v9
	ushll.8h	v1, v17, #0
	ushll.4s	v17, v1, #0
	mul.4s	v17, v11, v17
	ldr	d28, [x19, #1432]
	ushll2.4s	v1, v1, #0
	ushll.8h	v28, v28, #0
	mul.4s	v30, v10, v1
	ushll.4s	v1, v28, #0
	mul.4s	v31, v31, v1
	ldr	d1, [x19, #1440]
	ushll2.4s	v28, v28, #0
	ushll.8h	v1, v1, #0
	mul.4s	v27, v27, v28
	ushll2.4s	v28, v1, #0
	mul.4s	v28, v8, v28
	ldr	d8, [x19, #1448]
	ushll.4s	v1, v1, #0
	ushll.8h	v8, v8, #0
	mul.4s	v29, v29, v1
	ushll.4s	v1, v8, #0
	mul.4s	v9, v3, v1
	ldp	q1, q10, [x19, #752]
	ushll2.4s	v3, v8, #0
	mul.4s	v8, v1, v3
	uaddw2.2d	v1, v26, v17
	uaddw.2d	v3, v25, v17
	uaddw2.2d	v25, v2, v30
	uaddw.2d	v26, v0, v30
	uaddw2.2d	v0, v23, v31
	uaddw.2d	v17, v21, v31
	ldp	q11, q30, [x19, #784]
	uaddw2.2d	v20, v20, v27
	uaddw.2d	v23, v16, v27
	uaddw.2d	v2, v24, v28
	uaddw2.2d	v16, v22, v28
	ldr	q24, [x19, #848]
	ldp	q31, q27, [x19, #816]
	uaddw.2d	v21, v18, v29
	uaddw2.2d	v22, v6, v29
	ldr	q28, [x19, #880]
	uaddw2.2d	v4, v4, v9
	uaddw.2d	v6, v19, v9
	ldr	d29, [x19, #1456]
	uaddw2.2d	v18, v5, v8
	uaddw.2d	v19, v7, v8
	ushll.8h	v5, v29, #0
	ushll2.4s	v7, v5, #0
	mul.4s	v7, v11, v7
	ldr	d29, [x19, #1464]
	ushll.4s	v5, v5, #0
	ushll.8h	v29, v29, #0
	mul.4s	v8, v10, v5
	ushll2.4s	v5, v29, #0
	mul.4s	v31, v31, v5
	ldr	d5, [x19, #1472]
	ushll.4s	v29, v29, #0
	ushll.8h	v5, v5, #0
	mul.4s	v29, v30, v29
	ushll.4s	v30, v5, #0
	mul.4s	v27, v27, v30
	ldr	d30, [x19, #1480]
	ushll2.4s	v5, v5, #0
	ushll.8h	v30, v30, #0
	mul.4s	v9, v24, v5
	ushll2.4s	v5, v30, #0
	mul.4s	v28, v28, v5
	ldr	q5, [x19, #864]
	ushll.4s	v24, v30, #0
	mul.4s	v30, v5, v24
	uaddw.2d	v5, v26, v7
	uaddw2.2d	v7, v25, v7
	ldp	q10, q26, [x19, #896]
	uaddw.2d	v24, v3, v8
	uaddw2.2d	v25, v1, v8
	uaddw.2d	v1, v23, v31
	uaddw2.2d	v3, v20, v31
	ldp	q8, q31, [x19, #928]
	uaddw.2d	v20, v17, v29
	uaddw2.2d	v23, v0, v29
	uaddw2.2d	v0, v22, v27
	uaddw.2d	v17, v21, v27
	ldp	q27, q29, [x19, #960]
	uaddw2.2d	v21, v16, v9
	uaddw.2d	v22, v2, v9
	ldr	q9, [x19, #992]
	uaddw.2d	v2, v19, v28
	uaddw2.2d	v16, v18, v28
	ldr	d18, [x19, #1488]
	uaddw.2d	v6, v6, v30
	uaddw2.2d	v4, v4, v30
	ushll.8h	v18, v18, #0
	ushll.4s	v19, v18, #0
	mul.4s	v19, v10, v19
	ldr	d28, [x19, #1496]
	ushll2.4s	v18, v18, #0
	ushll.8h	v28, v28, #0
	mul.4s	v18, v26, v18
	ushll.4s	v26, v28, #0
	mul.4s	v26, v8, v26
	ldr	d30, [x19, #1504]
	ushll2.4s	v28, v28, #0
	ushll.8h	v30, v30, #0
	mul.4s	v28, v31, v28
	ushll2.4s	v31, v30, #0
	mul.4s	v29, v29, v31
	ldr	d31, [x19, #1512]
	ushll.4s	v30, v30, #0
	ushll.8h	v31, v31, #0
	mul.4s	v27, v27, v30
	ushll.4s	v30, v31, #0
	mul.4s	v30, v9, v30
	ldr	q8, [x19, #1008]
	ushll2.4s	v31, v31, #0
	mul.4s	v31, v8, v31
	uaddw2.2d	v25, v25, v19
	uaddw.2d	v19, v24, v19
	uaddw2.2d	v7, v7, v18
	uaddw.2d	v5, v5, v18
	uaddw2.2d	v18, v23, v26
	uaddw.2d	v20, v20, v26
	uaddw2.2d	v3, v3, v28
	uaddw.2d	v1, v1, v28
	uaddw.2d	v22, v22, v29
	uaddw2.2d	v21, v21, v29
	uaddw.2d	v17, v17, v27
	uaddw2.2d	v0, v0, v27
	uaddw2.2d	v4, v4, v30
	uaddw.2d	v6, v6, v30
	uaddw2.2d	v16, v16, v31
	uaddw.2d	v2, v2, v31
	add.2d	v1, v1, v5
	add.2d	v3, v3, v7
	add.2d	v5, v18, v25
	add.2d	v7, v17, v20
	add.2d	v7, v7, v19
	add.2d	v2, v2, v22
	add.2d	v1, v2, v1
	add.2d	v2, v6, v7
	add.2d	v6, v16, v21
	add.2d	v3, v6, v3
	add.2d	v0, v4, v0
	add.2d	v0, v0, v5
	add.2d	v0, v0, v3
	ldr	q3, [x19, #1024]
	add.2d	v1, v2, v1
	ldr	s2, [x19, #1520]
	add.2d	v0, v1, v0
	movi.2d	v1, #0000000000000000
	addp.2d	d0, v0
	mov.d	v1[0], v0[0]
	ldr	q0, [x19, #1040]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	ldr	s3, [x19, #1524]
	ushll.8h	v3, v3, #0
	ushll.4s	v3, v3, #0
	mul.4s	v0, v0, v3
	ldr	q3, [x19, #1056]
	uaddw.2d	v1, v1, v2
	uaddw.2d	v1, v1, v0
	uaddl2.2d	v0, v2, v0
	ldr	s2, [x19, #1528]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	ldr	q3, [x19, #1072]
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ldr	s2, [x19, #1532]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	ldr	q3, [x19, #1088]
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ldr	s2, [x19, #1536]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	ldr	q3, [x19, #1104]
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ldr	s2, [x19, #1540]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	ldr	q3, [x19, #1120]
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ldr	s2, [x19, #1544]
	ushll.8h	v2, v2, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	add.2d	v0, v1, v0
	addp.2d	d0, v0
	fmov	x9, d0
	str	x8, [x23, #24]
	ldr	w8, [x19, #1136]
	ldrb	w10, [x19, #1548]
	ldr	w11, [x19, #1140]
	ldrb	w12, [x19, #1549]
	mul	w8, w8, w10
	mul	w10, w11, w12
	add	x9, x9, x8
	add	x8, x9, x10
	str	x9, [x23, #24]
	ldr	w9, [x19, #1144]
	ldrb	w10, [x19, #1550]
	ldr	w11, [x19, #1148]
	ldrb	w12, [x19, #1551]
	mul	w9, w9, w10
	ldr	w10, [x19, #1152]
	ldrb	w13, [x19, #1552]
	mul	w11, w11, w12
	ldr	w12, [x19, #1156]
	mul	w10, w10, w13
	ldrb	w13, [x19, #1553]
	add	x9, x9, x11
	add	x9, x9, x10
	ldr	w10, [x19, #1160]
	ldrb	w11, [x19, #1554]
	mul	w12, w12, w13
	ldr	w13, [x19, #1164]
	mul	w10, w10, w11
	ldrb	w11, [x19, #1555]
	add	x9, x9, x12
	add	x9, x9, x10
	ldr	w10, [x19, #1168]
	ldrb	w12, [x19, #1556]
	mul	w11, w13, w11
	ldr	w13, [x19, #1172]
	mul	w10, w10, w12
	ldrb	w12, [x19, #1557]
	add	x9, x9, x11
	add	x9, x9, x10
	ldr	w10, [x19, #1176]
	ldrb	w11, [x19, #1558]
	mul	w12, w13, w12
	ldr	w13, [x19, #1180]
	mul	w10, w10, w11
	ldrb	w11, [x19, #1559]
	add	x9, x9, x12
	add	x9, x9, x10
	ldr	w10, [x19, #1184]
	ldrb	w12, [x19, #1560]
	mul	w11, w13, w11
	ldr	w13, [x19, #1188]
	mul	w10, w10, w12
	ldrb	w12, [x19, #1561]
	add	x9, x9, x11
	add	x9, x9, x10
	ldr	w10, [x19, #1192]
	mul	w11, w13, w12
	ldrb	w12, [x19, #1562]
	mul	w10, w10, w12
	ldr	w12, [x19, #1196]
	add	x9, x9, x11
	add	x9, x9, x10
	ldrb	w10, [x19, #1563]
	mul	w10, w12, w10
	ldr	w11, [x19, #1200]
	ldrb	w12, [x19, #1564]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1204]
	ldrb	w11, [x19, #1565]
	mul	w10, w10, w11
	ldr	w11, [x19, #1208]
	ldrb	w12, [x19, #1566]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1212]
	ldrb	w11, [x19, #1567]
	mul	w10, w10, w11
	ldr	w11, [x19, #1216]
	ldrb	w12, [x19, #1568]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1220]
	ldrb	w11, [x19, #1569]
	mul	w10, w10, w11
	ldr	w11, [x19, #1224]
	ldrb	w12, [x19, #1570]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1228]
	ldrb	w11, [x19, #1571]
	mul	w10, w10, w11
	ldr	w11, [x19, #1232]
	ldrb	w12, [x19, #1572]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1236]
	ldrb	w11, [x19, #1573]
	mul	w10, w10, w11
	ldr	w11, [x19, #1240]
	ldrb	w12, [x19, #1574]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1244]
	ldrb	w11, [x19, #1575]
	mul	w10, w10, w11
	ldr	w11, [x19, #1248]
	ldrb	w12, [x19, #1576]
	mul	w11, w11, w12
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1252]
	ldrb	w11, [x19, #1577]
	mul	w10, w10, w11
	ldr	w11, [x19, #1256]
	ldrb	w12, [x19, #1578]
	mul	w11, w11, w12
	str	x8, [x23, #24]
	add	x9, x9, x10
	add	x9, x9, x11
	ldr	w10, [x19, #1260]
	ldrb	w11, [x19, #1579]
	mul	w10, w10, w11
	add	x9, x9, x10
	add	x8, x8, x9
	str	x8, [x23, #24]
	ldr	w9, [x23]
	cbz	w9, LBB13_114
; %bb.112:
	add	x10, x19, #2280
LBB13_113:                              ; =>This Inner Loop Header: Depth=1
	ldur	w11, [x10, #-8]
	ldrb	w11, [x20, x11]
	ldr	w12, [x10], #12
	add	w11, w12, w11
	add	x8, x8, x11
	str	x8, [x23, #24]
	subs	x9, x9, #1
	b.ne	LBB13_113
LBB13_114:
	ldur	x8, [x29, #-120]
Lloh43:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh44:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh45:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB13_116
; %bb.115:
	add	sp, sp, #400
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #32]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #16]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp], #128            ; 16-byte Folded Reload
	ret
LBB13_116:
	bl	___stack_chk_fail
	.loh AdrpLdrGotLdr	Lloh40, Lloh41, Lloh42
	.loh AdrpLdrGotLdr	Lloh43, Lloh44, Lloh45
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream10write_bitsEjj ; -- Begin function _ZN8stx_zlib13DeflateStream10write_bitsEjj
	.p2align	2
__ZN8stx_zlib13DeflateStream10write_bitsEjj: ; @_ZN8stx_zlib13DeflateStream10write_bitsEjj
	.cfi_startproc
; %bb.0:
	ldr	x8, [x0, #40]
	ldr	x11, [x0, #72]
	mov	w9, #5                          ; =0x5
	mov	w10, #5                         ; =0x5
	cmp	x8, x11
	b.eq	LBB14_7
; %bb.1:
	add	x8, x0, #196, lsl #12           ; =802816
	add	x10, x11, #1
	str	x10, [x0, #72]
	mov	w10, #-1                        ; =0xffffffff
	lsl	w10, w10, w2
	bic	w10, w1, w10
	ldr	w11, [x8, #1404]
	lsl	w10, w10, w11
	ldr	w12, [x8, #1400]
	orr	w10, w12, w10
	str	w10, [x8, #1400]
	add	w11, w11, w2
	str	w11, [x8, #1404]
	cmp	w11, #8
	b.lo	LBB14_5
; %bb.2:
	add	x11, x0, #180, lsl #12          ; =737280
	add	x11, x11, #1384
	ldr	x12, [x8, #1384]
LBB14_3:                                ; =>This Inner Loop Header: Depth=1
	cmp	x12, #16, lsl #12               ; =65536
	b.eq	LBB14_6
; %bb.4:                                ;   in Loop: Header=BB14_3 Depth=1
	strb	w10, [x11, x12]
	ldr	x10, [x8, #1384]
	add	x12, x10, #1
	str	x12, [x8, #1384]
	ldr	w10, [x8, #1400]
	lsr	w10, w10, #8
	str	w10, [x8, #1400]
	ldr	w13, [x8, #1404]
	sub	w13, w13, #8
	str	w13, [x8, #1404]
	cmp	w13, #7
	b.hi	LBB14_3
LBB14_5:
	mov	w0, #1                          ; =0x1
	ret
LBB14_6:
	mov	w10, #6                         ; =0x6
LBB14_7:
	stp	w9, w10, [x0, #48]
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream10align_bitsEv ; -- Begin function _ZN8stx_zlib13DeflateStream10align_bitsEv
	.p2align	2
__ZN8stx_zlib13DeflateStream10align_bitsEv: ; @_ZN8stx_zlib13DeflateStream10align_bitsEv
	.cfi_startproc
; %bb.0:
	add	x8, x0, #196, lsl #12           ; =802816
	ldr	w9, [x8, #1404]
	cbz	w9, LBB15_5
; %bb.1:
	ldr	x12, [x0, #40]
	ldr	x11, [x0, #72]
	mov	w9, #5                          ; =0x5
	mov	w10, #5                         ; =0x5
	cmp	x12, x11
	b.eq	LBB15_7
; %bb.2:
	add	x10, x11, #1
	str	x10, [x0, #72]
	ldr	w11, [x8, #1400]
	mov	w10, #8                         ; =0x8
	str	w10, [x8, #1404]
	add	x10, x0, #180, lsl #12          ; =737280
	add	x10, x10, #1384
	ldr	x12, [x8, #1384]
LBB15_3:                                ; =>This Inner Loop Header: Depth=1
	cmp	x12, #16, lsl #12               ; =65536
	b.eq	LBB15_6
; %bb.4:                                ;   in Loop: Header=BB15_3 Depth=1
	strb	w11, [x10, x12]
	ldr	x11, [x8, #1384]
	add	x12, x11, #1
	str	x12, [x8, #1384]
	ldr	w11, [x8, #1400]
	lsr	w11, w11, #8
	str	w11, [x8, #1400]
	ldr	w13, [x8, #1404]
	sub	w13, w13, #8
	str	w13, [x8, #1404]
	cmp	w13, #7
	b.hi	LBB15_3
LBB15_5:
	mov	w0, #1                          ; =0x1
	ret
LBB15_6:
	mov	w10, #6                         ; =0x6
LBB15_7:
	stp	w9, w10, [x0, #48]
	mov	w0, #0                          ; =0x0
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream11emit_tokensEb ; -- Begin function _ZN8stx_zlib13DeflateStream11emit_tokensEb
	.p2align	2
__ZN8stx_zlib13DeflateStream11emit_tokensEb: ; @_ZN8stx_zlib13DeflateStream11emit_tokensEb
	.cfi_startproc
; %bb.0:
	stp	d11, d10, [sp, #-112]!          ; 16-byte Folded Spill
	stp	d9, d8, [sp, #16]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #32]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	.cfi_def_cfa_offset 112
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w23, -40
	.cfi_offset w24, -48
	.cfi_offset w25, -56
	.cfi_offset w26, -64
	.cfi_offset w27, -72
	.cfi_offset w28, -80
	.cfi_offset b8, -88
	.cfi_offset b9, -96
	.cfi_offset b10, -104
	.cfi_offset b11, -112
	add	x8, x0, #176, lsl #12           ; =720896
	add	x8, x8, #1416
Lloh46:
	adrp	x9, __ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE@PAGE
Lloh47:
	add	x9, x9, __ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE@PAGEOFF
	cmp	w1, #0
	csel	x8, x8, x9, ne
	ldp	q0, q1, [x8]
	ushll2.8h	v2, v1, #0
	ushll.4s	v3, v2, #0
	ushll.8h	v1, v1, #0
	ushll.4s	v4, v1, #0
	ushll2.4s	v2, v2, #0
	ushll2.4s	v1, v1, #0
	add	x10, x0, #176, lsl #12          ; =720896
	add	x10, x10, #144
	ldur	q5, [x10, #88]
	mul.4s	v5, v5, v1
	ldur	q1, [x10, #120]
	mul.4s	v2, v1, v2
	ldur	q1, [x10, #72]
	mul.4s	v4, v1, v4
	ldur	q1, [x10, #104]
	mul.4s	v3, v1, v3
	ldp	q1, q6, [x8, #32]
	ushll2.8h	v7, v6, #0
	ushll.4s	v16, v7, #0
	ushll.8h	v6, v6, #0
	ushll.4s	v17, v6, #0
	ushll2.4s	v7, v7, #0
	ushll2.4s	v6, v6, #0
	ldur	q18, [x10, #216]
	mul.4s	v6, v18, v6
	ldur	q18, [x10, #248]
	mul.4s	v7, v18, v7
	ldur	q18, [x10, #200]
	mul.4s	v17, v18, v17
	ldur	q18, [x10, #232]
	mul.4s	v16, v18, v16
	uaddl2.2d	v18, v5, v6
	uaddl2.2d	v19, v2, v7
	uaddl2.2d	v20, v4, v17
	uaddl2.2d	v21, v3, v16
	uaddl.2d	v5, v5, v6
	uaddl.2d	v6, v2, v7
	uaddl.2d	v4, v4, v17
	uaddl.2d	v3, v3, v16
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #472
	ldp	q16, q7, [x11]
	ldp	q2, q17, [x8, #64]
	ushll.8h	v22, v17, #0
	ushll2.4s	v23, v22, #0
	ldp	q25, q24, [x11, #32]
	ushll2.8h	v17, v17, #0
	ushll2.4s	v26, v17, #0
	ushll.4s	v22, v22, #0
	ushll.4s	v17, v17, #0
	mul.4s	v17, v25, v17
	mul.4s	v16, v16, v22
	mul.4s	v22, v24, v26
	mul.4s	v7, v7, v23
	uaddw.2d	v23, v3, v17
	uaddw.2d	v4, v4, v16
	uaddw.2d	v6, v6, v22
	uaddw.2d	v5, v5, v7
	uaddw2.2d	v17, v21, v17
	uaddw2.2d	v16, v20, v16
	uaddw2.2d	v19, v19, v22
	uaddw2.2d	v7, v18, v7
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #600
	ldp	q18, q20, [x11, #32]
	ldp	q21, q22, [x11]
	ldp	q3, q24, [x8, #96]
	ushll2.8h	v25, v24, #0
	ushll.4s	v26, v25, #0
	ushll.8h	v24, v24, #0
	ushll.4s	v27, v24, #0
	ushll2.4s	v25, v25, #0
	ushll2.4s	v24, v24, #0
	mul.4s	v22, v22, v24
	mul.4s	v20, v20, v25
	mul.4s	v21, v21, v27
	mul.4s	v18, v18, v26
	uaddw2.2d	v7, v7, v22
	uaddw2.2d	v19, v19, v20
	uaddw2.2d	v16, v16, v21
	uaddw2.2d	v17, v17, v18
	uaddw.2d	v5, v5, v22
	uaddw.2d	v6, v6, v20
	uaddw.2d	v20, v4, v21
	uaddw.2d	v18, v23, v18
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #728
	ldp	q22, q21, [x11]
	ldp	q24, q23, [x11, #32]
	ldp	q4, q25, [x8, #128]
	ushll.8h	v26, v25, #0
	ushll2.4s	v27, v26, #0
	ushll2.8h	v25, v25, #0
	ushll2.4s	v28, v25, #0
	ushll.4s	v26, v26, #0
	ushll.4s	v25, v25, #0
	mul.4s	v24, v24, v25
	mul.4s	v22, v22, v26
	mul.4s	v23, v23, v28
	mul.4s	v21, v21, v27
	uaddw.2d	v18, v18, v24
	uaddw.2d	v20, v20, v22
	uaddw.2d	v6, v6, v23
	uaddw.2d	v25, v5, v21
	uaddw2.2d	v17, v17, v24
	uaddw2.2d	v16, v16, v22
	uaddw2.2d	v19, v19, v23
	uaddw2.2d	v7, v7, v21
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #856
	ldp	q21, q22, [x11, #32]
	ldp	q23, q24, [x11]
	ldp	q5, q26, [x8, #160]
	ushll2.8h	v27, v26, #0
	ushll.4s	v28, v27, #0
	ushll.8h	v26, v26, #0
	ushll.4s	v29, v26, #0
	ushll2.4s	v27, v27, #0
	ushll2.4s	v26, v26, #0
	mul.4s	v24, v24, v26
	mul.4s	v22, v22, v27
	mul.4s	v23, v23, v29
	mul.4s	v21, v21, v28
	uaddw2.2d	v7, v7, v24
	uaddw2.2d	v19, v19, v22
	uaddw2.2d	v16, v16, v23
	uaddw2.2d	v17, v17, v21
	uaddw.2d	v24, v25, v24
	uaddw.2d	v22, v6, v22
	uaddw.2d	v20, v20, v23
	uaddw.2d	v18, v18, v21
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #984
	ldp	q23, q21, [x11]
	ldp	q26, q25, [x11, #32]
	ldp	q6, q27, [x8, #192]
	ushll.8h	v28, v27, #0
	ushll2.4s	v29, v28, #0
	ushll2.8h	v27, v27, #0
	ushll2.4s	v30, v27, #0
	ushll.4s	v28, v28, #0
	ushll.4s	v27, v27, #0
	mul.4s	v26, v26, v27
	mul.4s	v23, v23, v28
	mul.4s	v25, v25, v30
	mul.4s	v21, v21, v29
	uaddw.2d	v27, v18, v26
	uaddw.2d	v28, v20, v23
	uaddw.2d	v20, v22, v25
	uaddw.2d	v22, v24, v21
	uaddw2.2d	v18, v17, v26
	uaddw2.2d	v17, v16, v23
	uaddw2.2d	v16, v19, v25
	uaddw2.2d	v7, v7, v21
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #1112
	ldp	q19, q21, [x11, #32]
	ldp	q24, q25, [x11]
	ldp	q23, q26, [x8, #224]
	ushll2.8h	v29, v26, #0
	ushll.4s	v30, v29, #0
	ushll.8h	v26, v26, #0
	ushll.4s	v31, v26, #0
	ushll2.4s	v29, v29, #0
	ushll2.4s	v26, v26, #0
	mul.4s	v25, v25, v26
	mul.4s	v21, v21, v29
	mul.4s	v24, v24, v31
	mul.4s	v26, v19, v30
	uaddw2.2d	v7, v7, v25
	uaddw2.2d	v16, v16, v21
	uaddw2.2d	v17, v17, v24
	uaddw2.2d	v18, v18, v26
	uaddw.2d	v19, v22, v25
	uaddw.2d	v20, v20, v21
	uaddw.2d	v21, v28, v24
	uaddw.2d	v22, v27, v26
	add	x11, x0, #176, lsl #12          ; =720896
	ldr	d24, [x11, #6232]
	ldur	q25, [x10, #40]
	ldur	q26, [x10, #56]
	ldur	q27, [x10, #24]
	ldur	q28, [x10, #8]
	ushll2.8h	v29, v0, #0
	ushll.4s	v30, v29, #0
	ushll2.4s	v29, v29, #0
	ushll.8h	v0, v0, #0
	ushll2.4s	v31, v0, #0
	ushll.4s	v0, v0, #0
	mul.4s	v0, v28, v0
	mul.4s	v27, v27, v31
	mul.4s	v26, v26, v29
	mul.4s	v25, v25, v30
	uaddw.2d	v24, v24, v0
	ldur	q28, [x10, #168]
	ldur	q29, [x10, #136]
	ldur	q30, [x10, #184]
	ldur	q31, [x10, #152]
	ushll2.8h	v8, v1, #0
	ushll.4s	v9, v8, #0
	ushll.8h	v1, v1, #0
	ushll.4s	v10, v1, #0
	ushll2.4s	v8, v8, #0
	ushll2.4s	v1, v1, #0
	mul.4s	v1, v31, v1
	mul.4s	v30, v30, v8
	mul.4s	v29, v29, v10
	mul.4s	v28, v28, v9
	uaddl2.2d	v31, v27, v1
	uaddl2.2d	v8, v26, v30
	uaddl2.2d	v0, v0, v29
	uaddl2.2d	v9, v25, v28
	uaddl.2d	v1, v27, v1
	uaddl.2d	v26, v26, v30
	uaddw.2d	v24, v24, v29
	uaddl.2d	v25, v25, v28
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #408
	ldp	q28, q27, [x11]
	ldp	q30, q29, [x11, #32]
	ushll.8h	v10, v2, #0
	ushll2.8h	v2, v2, #0
	ushll.4s	v11, v2, #0
	mul.4s	v30, v30, v11
	ushll.4s	v11, v10, #0
	mul.4s	v28, v28, v11
	ushll2.4s	v10, v10, #0
	ushll2.4s	v2, v2, #0
	mul.4s	v2, v29, v2
	mul.4s	v27, v27, v10
	uaddw.2d	v25, v25, v30
	uaddw.2d	v24, v24, v28
	uaddw.2d	v26, v26, v2
	uaddw.2d	v1, v1, v27
	uaddw2.2d	v29, v9, v30
	uaddw2.2d	v0, v0, v28
	uaddw2.2d	v2, v8, v2
	uaddw2.2d	v27, v31, v27
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #536
	ldp	q28, q30, [x11, #32]
	ldp	q31, q8, [x11]
	ushll2.8h	v9, v3, #0
	ushll.8h	v3, v3, #0
	ushll2.4s	v10, v3, #0
	mul.4s	v8, v8, v10
	ushll2.4s	v10, v9, #0
	mul.4s	v30, v30, v10
	ushll.4s	v9, v9, #0
	ushll.4s	v3, v3, #0
	mul.4s	v3, v31, v3
	mul.4s	v28, v28, v9
	uaddw2.2d	v27, v27, v8
	uaddw2.2d	v2, v2, v30
	uaddw2.2d	v0, v0, v3
	uaddw2.2d	v29, v29, v28
	uaddw.2d	v1, v1, v8
	uaddw.2d	v26, v26, v30
	uaddw.2d	v3, v24, v3
	uaddw.2d	v24, v25, v28
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #664
	ldp	q28, q25, [x11]
	ldp	q31, q30, [x11, #32]
	ushll.8h	v8, v4, #0
	ushll2.8h	v4, v4, #0
	ushll.4s	v9, v4, #0
	mul.4s	v31, v31, v9
	ushll.4s	v9, v8, #0
	mul.4s	v28, v28, v9
	ushll2.4s	v8, v8, #0
	ushll2.4s	v4, v4, #0
	mul.4s	v4, v30, v4
	mul.4s	v25, v25, v8
	uaddw.2d	v24, v24, v31
	uaddw.2d	v3, v3, v28
	uaddw.2d	v26, v26, v4
	uaddw.2d	v1, v1, v25
	uaddw2.2d	v29, v29, v31
	uaddw2.2d	v0, v0, v28
	uaddw2.2d	v2, v2, v4
	uaddw2.2d	v4, v27, v25
	add	x11, x0, #176, lsl #12          ; =720896
	add	x11, x11, #792
	ldp	q25, q27, [x11, #32]
	ldp	q28, q30, [x11]
	ushll2.8h	v31, v5, #0
	ushll.8h	v5, v5, #0
	ushll2.4s	v8, v5, #0
	mul.4s	v30, v30, v8
	ushll2.4s	v8, v31, #0
	mul.4s	v27, v27, v8
	add	x11, x0, #196, lsl #12          ; =802816
	add	x11, x11, #1384
	ushll.4s	v31, v31, #0
	ushll.4s	v5, v5, #0
	mul.4s	v5, v28, v5
	mul.4s	v25, v25, v31
	uaddw2.2d	v4, v4, v30
	uaddw2.2d	v2, v2, v27
	uaddw2.2d	v0, v0, v5
	uaddw2.2d	v28, v29, v25
	uaddw.2d	v1, v1, v30
	uaddw.2d	v26, v26, v27
	uaddw.2d	v3, v3, v5
	uaddw.2d	v5, v24, v25
	add	x12, x0, #176, lsl #12          ; =720896
	add	x12, x12, #920
	ldp	q25, q24, [x12]
	ldp	q29, q27, [x12, #32]
	ushll.8h	v30, v6, #0
	ushll2.8h	v6, v6, #0
	ushll.4s	v31, v6, #0
	mul.4s	v29, v29, v31
	ushll2.4s	v31, v30, #0
	ushll2.4s	v6, v6, #0
	ushll.4s	v30, v30, #0
	mul.4s	v25, v25, v30
	mul.4s	v6, v27, v6
	mul.4s	v24, v24, v31
	uaddw.2d	v5, v5, v29
	uaddw.2d	v3, v3, v25
	uaddw.2d	v26, v26, v6
	uaddw.2d	v1, v1, v24
	uaddw2.2d	v27, v28, v29
	uaddw2.2d	v0, v0, v25
	uaddw2.2d	v2, v2, v6
	uaddw2.2d	v4, v4, v24
	add	x12, x0, #176, lsl #12          ; =720896
	add	x12, x12, #1048
	ldp	q6, q24, [x12, #32]
	ldp	q25, q28, [x12]
	ushll2.8h	v29, v23, #0
	ushll.4s	v30, v29, #0
	ushll.8h	v23, v23, #0
	ushll.4s	v31, v23, #0
	ushll2.4s	v29, v29, #0
	ushll2.4s	v23, v23, #0
	mul.4s	v23, v28, v23
	mul.4s	v24, v24, v29
	mul.4s	v25, v25, v31
	mul.4s	v6, v6, v30
	uaddw2.2d	v4, v4, v23
	uaddw2.2d	v2, v2, v24
	uaddw2.2d	v0, v0, v25
	uaddw2.2d	v27, v27, v6
	uaddw.2d	v1, v1, v23
	uaddw.2d	v23, v26, v24
	uaddw.2d	v3, v3, v25
	uaddw.2d	v5, v5, v6
	add.2d	v5, v22, v5
	add.2d	v3, v21, v3
	add.2d	v6, v20, v23
	add.2d	v1, v19, v1
	add.2d	v18, v18, v27
	add.2d	v0, v17, v0
	add.2d	v2, v16, v2
	add.2d	v4, v7, v4
	add.2d	v2, v4, v2
	add.2d	v0, v0, v18
	add.2d	v0, v0, v2
	add.2d	v1, v1, v6
	add.2d	v2, v3, v5
	add.2d	v1, v2, v1
	mov	w12, #1176                      ; =0x498
	movk	w12, #11, lsl #16
	ldr	q2, [x0, x12]
	ldr	s3, [x8, #256]
	mov	w12, #1192                      ; =0x4a8
	movk	w12, #11, lsl #16
	add.2d	v0, v1, v0
	addp.2d	d0, v0
	movi.2d	v1, #0000000000000000
	mov.d	v1[0], v0[0]
	ushll.8h	v0, v3, #0
	ushll.4s	v0, v0, #0
	mul.4s	v0, v2, v0
	uaddw.2d	v1, v1, v0
	ldr	q2, [x0, x12]
	ldr	s3, [x8, #260]
	ushll.8h	v3, v3, #0
	mov	w12, #1208                      ; =0x4b8
	movk	w12, #11, lsl #16
	ldr	q4, [x0, x12]
	ldr	s5, [x8, #264]
	mov	w12, #1224                      ; =0x4c8
	movk	w12, #11, lsl #16
	ushll.4s	v3, v3, #0
	mul.4s	v2, v2, v3
	uaddw.2d	v1, v1, v2
	uaddl2.2d	v0, v0, v2
	ushll.8h	v2, v5, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v4, v2
	uaddw.2d	v1, v1, v2
	ldr	q3, [x0, x12]
	ldr	s4, [x8, #268]
	mov	w12, #1240                      ; =0x4d8
	movk	w12, #11, lsl #16
	ldr	q5, [x0, x12]
	ldr	s6, [x8, #272]
	mov	w12, #1256                      ; =0x4e8
	movk	w12, #11, lsl #16
	uaddw2.2d	v0, v0, v2
	ushll.8h	v2, v4, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ushll.8h	v2, v6, #0
	ldr	q3, [x0, x12]
	ldr	s4, [x8, #276]
	mov	w12, #1272                      ; =0x4f8
	movk	w12, #11, lsl #16
	ldr	q6, [x0, x12]
	ldr	s7, [x8, #280]
	ushll.4s	v2, v2, #0
	mul.4s	v2, v5, v2
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ushll.8h	v2, v4, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v3, v2
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	ushll.8h	v2, v7, #0
	ushll.4s	v2, v2, #0
	mul.4s	v2, v6, v2
	uaddw.2d	v1, v1, v2
	uaddw2.2d	v0, v0, v2
	add.2d	v0, v1, v0
	addp.2d	d0, v0
	fmov	x12, d0
	ldr	w13, [x10, #1144]
	ldrb	w14, [x8, #284]
	mul	w13, w13, w14
	ldr	w14, [x10, #1148]
	ldrb	w15, [x8, #285]
	mul	w14, w14, w15
	add	x12, x12, x13
	add	x15, x12, x14
	add	x12, x0, #176, lsl #12          ; =720896
	add	x12, x12, #1702
	add	x13, x9, #858
	csel	x12, x12, x13, ne
	ldr	q0, [x10, #1184]
	ldr	q1, [x10, #1200]
	ldr	q2, [x10, #1152]
	ldr	q3, [x10, #1168]
	ldr	q4, [x12]
	ushll2.8h	v5, v4, #0
	ushll.4s	v6, v5, #0
	ushll2.4s	v5, v5, #0
	ushll.8h	v4, v4, #0
	ushll.4s	v7, v4, #0
	ldr	q16, [x10, #1216]
	ldr	q17, [x10, #1232]
	ldr	d18, [x12, #16]
	ushll2.4s	v4, v4, #0
	mul.4s	v3, v3, v4
	mul.4s	v2, v2, v7
	mul.4s	v1, v1, v5
	mul.4s	v0, v0, v6
	ushll.8h	v4, v18, #0
	ldr	q5, [x10, #1248]
	ldr	s6, [x12, #24]
	ldr	w13, [x10, #1264]
	ldrb	w14, [x12, #28]
	mul	w16, w13, w14
	ldr	w13, [x10, #1268]
	ushll.4s	v7, v4, #0
	ushll2.4s	v4, v4, #0
	mul.4s	v4, v17, v4
	mul.4s	v7, v16, v7
	ushll.8h	v6, v6, #0
	ushll.4s	v6, v6, #0
	mul.4s	v5, v5, v6
	ldrb	w14, [x12, #29]
	mul	w17, w13, w14
	add	x13, x0, #176, lsl #12          ; =720896
	add	x13, x13, #1732
	add	x14, x9, #286
	csel	x13, x13, x14, ne
	add	x14, x0, #176, lsl #12          ; =720896
	add	x14, x14, #2304
	add	x9, x9, #888
	csel	x14, x14, x9, ne
	ldr	w19, [x11, #20]
	uaddl.2d	v6, v3, v4
	uaddl2.2d	v3, v3, v4
	uaddl2.2d	v4, v2, v7
	uaddl.2d	v2, v2, v7
	uaddw.2d	v2, v2, v5
	uaddw2.2d	v4, v4, v5
	uaddw2.2d	v3, v3, v1
	uaddw2.2d	v4, v4, v0
	add.2d	v3, v4, v3
	uaddw.2d	v1, v6, v1
	uaddw.2d	v0, v2, v0
	add.2d	v0, v0, v1
	add.2d	v0, v0, v3
	addp.2d	d0, v0
	fmov	x9, d0
	add	x9, x9, x16
	add	x16, x17, x19
	add	x9, x9, x16
	add	x15, x9, x15
	ldr	x9, [x11]
	mov	w16, #65536                     ; =0x10000
	sub	x16, x16, x9
	cmp	x16, x15, lsr #3
	b.hs	LBB16_2
LBB16_1:
	mov	w9, #6                          ; =0x6
	b	LBB16_13
LBB16_2:
	ldr	x1, [x10]
	lsl	x16, x1, #2
	ldr	x15, [x0, #40]
	ldr	x17, [x0, #72]
	sub	x10, x15, x17
	cmp	x16, x10
	b.hi	LBB16_12
; %bb.3:
	add	x10, x0, #180, lsl #12          ; =737280
	add	x10, x10, #1384
	add	x17, x17, x16
	str	x17, [x0, #72]
	ldr	w16, [x11, #16]
	cbz	x1, LBB16_11
; %bb.4:
	mov	x2, #0                          ; =0x0
	add	x3, x0, #144, lsl #12           ; =589824
	add	x3, x3, #140
Lloh48:
	adrp	x4, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGE
Lloh49:
	add	x4, x4, __ZN8stx_zlib12_GLOBAL__N_114length_symbolsE@PAGEOFF
Lloh50:
	adrp	x5, __ZN8stx_zlib12_GLOBAL__N_111length_baseE@PAGE
Lloh51:
	add	x5, x5, __ZN8stx_zlib12_GLOBAL__N_111length_baseE@PAGEOFF
Lloh52:
	adrp	x6, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGE
Lloh53:
	add	x6, x6, __ZN8stx_zlib12_GLOBAL__N_112length_extraE@PAGEOFF
	mov	w7, #30                         ; =0x1e
                                        ; kill: def $w19 killed $w19 killed $x19 def $x19
Lloh54:
	adrp	x20, __ZN8stx_zlib12_GLOBAL__N_113distance_baseE@PAGE
Lloh55:
	add	x20, x20, __ZN8stx_zlib12_GLOBAL__N_113distance_baseE@PAGEOFF
	b	LBB16_6
LBB16_5:                                ;   in Loop: Header=BB16_6 Depth=1
	add	x2, x2, #1
	cmp	x2, x1
	b.eq	LBB16_10
LBB16_6:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB16_9 Depth 2
	add	x21, x3, x2, lsl #2
	ldrh	w22, [x21]
	ldrh	w21, [x21, #2]
	cbz	w21, LBB16_8
; %bb.7:                                ;   in Loop: Header=BB16_6 Depth=1
	ldr	w23, [x4, x22, lsl #2]
	add	w24, w23, #257
	ldrh	w25, [x13, w24, uxtw #1]
	ldrb	w24, [x8, w24, uxtw]
	ldr	w26, [x5, x23, lsl #2]
	sub	w22, w22, w26
	lsl	x22, x22, x24
	orr	x22, x22, x25
	ldr	w23, [x6, x23, lsl #2]
	sub	w25, w21, #1
	clz	w26, w25
	sub	w27, w7, w26
	lsr	w27, w25, w27
	and	w27, w27, #0x1
	sub	w26, w27, w26, lsl #1
	add	w26, w26, #62
	cmp	w21, #5
	csel	w25, w25, w26, lo
	ldrh	w26, [x14, w25, uxtw #1]
	ldrb	w27, [x12, w25, uxtw]
	add	w23, w23, w24
	add	w24, w23, w27
	lsl	x23, x26, x23
	ldr	w26, [x20, w25, uxtw #2]
	sub	w21, w21, w26
	lsl	x21, x21, x24
	orr	x21, x23, x21
	orr	x21, x22, x21
	lsr	w22, w25, #1
	sub	w22, w22, #1
	cmp	w25, #4
	csel	w22, wzr, w22, lo
	add	w22, w24, w22
	lsl	x21, x21, x19
	orr	x16, x21, x16
	add	w19, w22, w19
	cmp	w19, #8
	b.hs	LBB16_9
	b	LBB16_5
LBB16_8:                                ;   in Loop: Header=BB16_6 Depth=1
	ldrh	w21, [x13, x22, lsl #1]
	ldrb	w22, [x8, x22]
	lsl	x21, x21, x19
	orr	x16, x21, x16
	add	w19, w22, w19
	cmp	w19, #8
	b.lo	LBB16_5
LBB16_9:                                ;   Parent Loop BB16_6 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	strb	w16, [x10, x9]
	add	x9, x9, #1
	lsr	x16, x16, #8
	sub	w19, w19, #8
	cmp	w19, #7
	b.hi	LBB16_9
	b	LBB16_5
LBB16_10:
                                        ; kill: def $w19 killed $w19 killed $x19 def $x19
LBB16_11:
	stp	w16, w19, [x11, #16]
	str	x9, [x11]
	cmp	x15, x17
	b.ne	LBB16_15
LBB16_12:
	mov	w9, #5                          ; =0x5
LBB16_13:
	mov	w8, #0                          ; =0x0
	mov	w10, #5                         ; =0x5
	stp	w10, w9, [x0, #48]
LBB16_14:
	mov	x0, x8
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #64]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #48]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #32]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #16]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp], #112            ; 16-byte Folded Reload
	ret
LBB16_15:
	ldrb	w12, [x8, #256]
	ldrh	w8, [x13, #512]
	add	x13, x17, #1
	str	x13, [x0, #72]
	mov	w13, #-1                        ; =0xffffffff
	lsl	w13, w13, w12
	bic	w8, w8, w13
	lsl	w8, w8, w19
	orr	w8, w8, w16
	add	w12, w19, w12
	stp	w8, w12, [x11, #16]
	cmp	w12, #8
	b.lo	LBB16_18
LBB16_16:                               ; =>This Inner Loop Header: Depth=1
	cmp	x9, #16, lsl #12                ; =65536
	b.eq	LBB16_1
; %bb.17:                               ;   in Loop: Header=BB16_16 Depth=1
	strb	w8, [x10, x9]
	ldr	x8, [x11]
	add	x9, x8, #1
	str	x9, [x11]
	ldp	w8, w12, [x11, #16]
	lsr	w8, w8, #8
	sub	w12, w12, #8
	stp	w8, w12, [x11, #16]
	cmp	w12, #7
	b.hi	LBB16_16
LBB16_18:
	mov	w8, #1                          ; =0x1
	b	LBB16_14
	.loh AdrpAdd	Lloh46, Lloh47
	.loh AdrpAdd	Lloh54, Lloh55
	.loh AdrpAdd	Lloh52, Lloh53
	.loh AdrpAdd	Lloh50, Lloh51
	.loh AdrpAdd	Lloh48, Lloh49
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal8,8byte_literals
	.p2align	3, 0x0                          ; -- Begin function _ZN8stx_zlib13DeflateStream12encode_blockEb
lCPI17_0:
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream12encode_blockEb
	.p2align	2
__ZN8stx_zlib13DeflateStream12encode_blockEb: ; @_ZN8stx_zlib13DeflateStream12encode_blockEb
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
	mov	x20, x1
	mov	x19, x0
	add	x8, x0, #196, lsl #12           ; =802816
	add	x23, x8, #1384
	add	x8, x0, #177, lsl #12           ; =724992
	add	x21, x8, #2120
	add	x8, x0, #16, lsl #12            ; =65536
	add	x22, x8, #120
	ldr	w8, [x0, #80]
	cbz	w8, LBB17_13
; %bb.1:
	ldr	w25, [x23, #20]
	ldr	x24, [x22, #8]
	mov	x0, x19
	bl	__ZN8stx_zlib13DeflateStream8tokenizeEv
	cbz	w0, LBB17_27
; %bb.2:
	mov	x11, #0                         ; =0x0
	add	x8, x19, #176, lsl #12          ; =720896
	add	x8, x8, #1296
	mov	w9, #5                          ; =0x5
	sub	w9, w9, w25
	and	w9, w9, #0x7
	add	w9, w9, #35
	add	x24, x9, x24, lsl #3
	ldr	x10, [x21, #16]
	add	x9, x10, #3
	add	x10, x10, #29
	add	x12, x19, #176, lsl #12         ; =720896
	add	x12, x12, #152
	mov	w13, #8                         ; =0x8
	mov	w14, #9                         ; =0x9
	mov	w15, #7                         ; =0x7
LBB17_3:                                ; =>This Inner Loop Header: Depth=1
	ldr	w16, [x12, x11, lsl #2]
	cmp	x11, #280
	cinc	w17, w15, hs
	cmp	x11, #256
	csel	w17, w14, w17, lo
	cmp	x11, #144
	csel	w17, w13, w17, lo
	mul	w17, w17, w16
	add	x9, x9, x17
	add	x10, x10, x16
	add	x11, x11, #1
	cmp	x11, #286
	b.ne	LBB17_3
; %bb.4:
	ldp	w14, w15, [x8]
	add	w16, w14, w14, lsl #2
	add	w17, w15, w15, lsl #2
	ldp	w0, w11, [x8, #8]
	add	w1, w0, w0, lsl #2
	add	w2, w11, w11, lsl #2
	ldp	w13, w3, [x8, #16]
	add	w4, w13, w13, lsl #2
	add	w5, w3, w3, lsl #2
	ldp	w6, w12, [x8, #24]
	add	w7, w6, w6, lsl #2
	add	w25, w12, w12, lsl #2
	add	x16, x16, x17
	add	x16, x16, x1
	add	x14, x14, x15
	add	x14, x14, x0
	ldp	w15, w17, [x8, #32]
	add	w0, w15, w15, lsl #2
	add	x10, x10, x14
	add	w14, w17, w17, lsl #2
	add	x16, x16, x2
	add	x16, x16, x4
	add	x9, x9, x16
	ldp	w16, w1, [x8, #40]
	add	w2, w16, w16, lsl #2
	add	x11, x11, x13
	add	x11, x11, x3
	add	w13, w1, w1, lsl #2
	add	x11, x11, x6
	add	x10, x10, x11
	add	x11, x5, x7
	add	x11, x11, x25
	ldp	w3, w4, [x8, #48]
	add	w5, w3, w3, lsl #2
	add	x11, x11, x0
	add	x11, x11, x14
	add	w14, w4, w4, lsl #2
	add	x12, x12, x15
	add	x12, x12, x17
	add	x11, x11, x2
	add	x9, x9, x11
	ldp	w11, w15, [x8, #56]
	add	w17, w11, w11, lsl #2
	add	x12, x12, x16
	add	x12, x12, x1
	add	w16, w15, w15, lsl #2
	add	x12, x10, x12
	add	x10, x13, x5
	add	x10, x10, x14
	ldp	w13, w14, [x8, #64]
	add	w0, w13, w13, lsl #2
	add	x1, x3, x4
	add	x11, x1, x11
	add	w1, w14, w14, lsl #2
	add	x10, x10, x17
	add	x10, x10, x16
	add	x11, x11, x15
	add	x11, x11, x13
	ldp	w13, w15, [x8, #72]
	add	w16, w13, w13, lsl #2
	add	x10, x10, x0
	add	x10, x10, x1
	add	w17, w15, w15, lsl #2
	add	x10, x9, x10
	add	x9, x11, x14
	add	x9, x12, x9
	ldp	w11, w12, [x8, #80]
	add	w14, w11, w11, lsl #2
	add	x16, x16, x17
	add	x14, x16, x14
	add	x13, x13, x15
	add	x11, x13, x11
	add	w13, w12, w12, lsl #2
	ldp	w15, w16, [x8, #88]
	add	w17, w15, w15, lsl #2
	add	x13, x14, x13
	add	x13, x13, x17
	add	x11, x11, x12
	add	x11, x11, x15
	ldp	w12, w14, [x8, #96]
	add	w15, w16, w16, lsl #2
	add	w17, w12, w12, lsl #2
	add	x13, x13, x15
	add	x13, x13, x17
	add	x11, x11, x16
	add	x11, x11, x12
	add	w12, w14, w14, lsl #2
	add	x12, x13, x12
	add	x10, x10, x12
	ldp	w12, w13, [x8, #104]
	add	x14, x14, x12
	add	x14, x14, x13
	ldp	w15, w8, [x8, #112]
	add	w12, w12, w12, lsl #2
	add	w13, w13, w13, lsl #2
	add	w16, w15, w15, lsl #2
	add	x12, x12, x13
	add	x12, x12, x16
	add	w13, w8, w8, lsl #2
	add	x12, x12, x13
	add	x25, x10, x12
	add	x10, x14, x15
	add	x8, x10, x8
	add	x9, x9, x11
	add	x8, x9, x8
	tbz	w20, #0, LBB17_6
; %bb.5:
	ldr	w9, [x23, #20]
	neg	w9, w9
	sub	w10, w9, w25
	and	x10, x10, #0x7
	add	x25, x10, x25
	sub	w9, w9, w8
	and	x9, x9, #0x7
	add	x8, x9, x8
LBB17_6:
	ldr	w11, [x19, #4]
	cmp	x24, x25
	csel	x10, x24, x25, lo
	mov	w9, #1                          ; =0x1
	cmp	w11, #4
	b.eq	LBB17_12
; %bb.7:
	cmp	x8, x10
	b.hs	LBB17_12
; %bb.8:
	mov	x0, x19
	bl	__ZN8stx_zlib13DeflateStream15prepare_dynamicEv
	cbz	w0, LBB17_27
; %bb.9:
	ldr	x8, [x21, #24]
	cbz	w20, LBB17_11
; %bb.10:
	ldr	w9, [x23, #20]
	add	w9, w8, w9
	neg	w9, w9
	and	x9, x9, #0x7
	add	x8, x9, x8
LBB17_11:
	cmp	x8, x25
	csel	x25, x8, x25, lo
	mov	w8, #1                          ; =0x1
	cinc	w9, w8, lo
LBB17_12:
	cmp	x24, x25
	csel	w12, w9, wzr, hi
	ldr	x8, [x19, #40]
	ldr	x9, [x19, #72]
	mov	w13, #5                         ; =0x5
	mov	w10, #5                         ; =0x5
	cmp	x8, x9
	b.ne	LBB17_14
	b	LBB17_26
LBB17_13:
	mov	w12, #0                         ; =0x0
	ldr	x8, [x19, #40]
	ldr	x9, [x19, #72]
	mov	w13, #5                         ; =0x5
	mov	w10, #5                         ; =0x5
	cmp	x8, x9
	b.eq	LBB17_26
LBB17_14:
	orr	w11, w20, w12, lsl #1
	add	x10, x9, #1
	str	x10, [x19, #72]
	ldp	w15, w14, [x23, #16]
	lsl	w11, w11, w14
	orr	w11, w15, w11
	add	w15, w14, #3
	stp	w11, w15, [x23, #16]
	cmp	w15, #8
	b.lo	LBB17_18
; %bb.15:
	add	x14, x19, #180, lsl #12         ; =737280
	add	x14, x14, #1384
	ldr	x16, [x23]
LBB17_16:                               ; =>This Inner Loop Header: Depth=1
	cmp	x16, #16, lsl #12               ; =65536
	b.eq	LBB17_25
; %bb.17:                               ;   in Loop: Header=BB17_16 Depth=1
	strb	w11, [x14, x16]
	ldr	x11, [x23]
	add	x16, x11, #1
	str	x16, [x23]
	ldp	w11, w15, [x23, #16]
	lsr	w11, w11, #8
	sub	w15, w15, #8
	stp	w11, w15, [x23, #16]
	cmp	w15, #7
	b.hi	LBB17_16
LBB17_18:
	cmp	w12, #1
	b.eq	LBB17_28
; %bb.19:
	cbnz	w12, LBB17_34
; %bb.20:
	cbz	w15, LBB17_52
; %bb.21:
	mov	w12, #5                         ; =0x5
	mov	w13, #5                         ; =0x5
	cmp	x8, x10
	b.eq	LBB17_64
; %bb.22:
	add	x10, x9, #2
	str	x10, [x19, #72]
	mov	w9, #8                          ; =0x8
	str	w9, [x23, #20]
	add	x9, x19, #180, lsl #12          ; =737280
	add	x9, x9, #1384
	ldr	x13, [x23]
LBB17_23:                               ; =>This Inner Loop Header: Depth=1
	cmp	x13, #16, lsl #12               ; =65536
	b.eq	LBB17_63
; %bb.24:                               ;   in Loop: Header=BB17_23 Depth=1
	strb	w11, [x9, x13]
	ldr	x11, [x23]
	add	x13, x11, #1
	str	x13, [x23]
	ldp	w11, w14, [x23, #16]
	lsr	w11, w11, #8
	sub	w14, w14, #8
	stp	w11, w14, [x23, #16]
	cmp	w14, #7
	b.hi	LBB17_23
	b	LBB17_53
LBB17_25:
	mov	w10, #6                         ; =0x6
LBB17_26:
	mov	w0, #0                          ; =0x0
	stp	w13, w10, [x19, #48]
LBB17_27:
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_28:
	mov	x0, x19
	mov	w1, #0                          ; =0x0
	bl	__ZN8stx_zlib13DeflateStream11emit_tokensEb
	tbz	w0, #0, LBB17_51
LBB17_29:
	tbnz	w20, #0, LBB17_121
; %bb.30:
	ldp	x8, x9, [x22]
	ldr	x10, [x19, #112]
	add	x11, x9, x8
	cmp	x11, x10
	csel	x20, x11, x10, lo
	cmp	x11, x10
	b.ls	LBB17_120
; %bb.31:
	ldr	x10, [x19, #40]
	ldr	x9, [x19, #72]
	sub	x8, x10, x9
	cmp	x20, x8
	b.ls	LBB17_73
LBB17_32:
	mov	w0, #0                          ; =0x0
	movi.2s	v0, #5
LBB17_33:
	str	d0, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_34:
	mov	w12, #5                         ; =0x5
	mov	w13, #5                         ; =0x5
	cmp	x8, x10
	b.eq	LBB17_64
; %bb.35:
	ldr	w10, [x21, #4]
	sub	w10, w10, #1
	add	x13, x9, #2
	str	x13, [x19, #72]
	and	w10, w10, #0x1f
	lsl	w10, w10, w15
	orr	w14, w11, w10
	add	w16, w15, #5
	stp	w14, w16, [x23, #16]
	cmp	w15, #3
	b.lo	LBB17_39
; %bb.36:
	add	x10, x19, #180, lsl #12         ; =737280
	add	x10, x10, #1384
	ldr	x11, [x23]
LBB17_37:                               ; =>This Inner Loop Header: Depth=1
	cmp	x11, #16, lsl #12               ; =65536
	b.eq	LBB17_63
; %bb.38:                               ;   in Loop: Header=BB17_37 Depth=1
	strb	w14, [x10, x11]
	ldr	x11, [x23]
	add	x11, x11, #1
	str	x11, [x23]
	ldp	w14, w15, [x23, #16]
	lsr	w14, w14, #8
	sub	w16, w15, #8
	stp	w14, w16, [x23, #16]
	cmp	w16, #7
	b.hi	LBB17_37
LBB17_39:
	mov	w10, #5                         ; =0x5
	mov	w11, #5                         ; =0x5
	cmp	x8, x13
	b.eq	LBB17_82
; %bb.40:
	ldr	w11, [x21, #8]
	sub	w12, w11, #1
	add	x11, x9, #3
	str	x11, [x19, #72]
	and	w12, w12, #0x1f
	lsl	w12, w12, w16
	orr	w13, w14, w12
	add	w14, w16, #5
	stp	w13, w14, [x23, #16]
	cmp	w16, #3
	b.lo	LBB17_44
; %bb.41:
	add	x12, x19, #180, lsl #12         ; =737280
	add	x12, x12, #1384
	ldr	x15, [x23]
LBB17_42:                               ; =>This Inner Loop Header: Depth=1
	cmp	x15, #16, lsl #12               ; =65536
	b.eq	LBB17_81
; %bb.43:                               ;   in Loop: Header=BB17_42 Depth=1
	strb	w13, [x12, x15]
	ldr	x13, [x23]
	add	x15, x13, #1
	str	x15, [x23]
	ldp	w13, w14, [x23, #16]
	lsr	w13, w13, #8
	sub	w14, w14, #8
	stp	w13, w14, [x23, #16]
	cmp	w14, #7
	b.hi	LBB17_42
LBB17_44:
	mov	w12, #5                         ; =0x5
	mov	w10, #5                         ; =0x5
	cmp	x8, x11
	b.eq	LBB17_103
; %bb.45:
	ldr	w11, [x21, #12]
	add	w10, w11, #12
	add	x9, x9, #4
	str	x9, [x19, #72]
	and	w10, w10, #0xf
	lsl	w10, w10, w14
	orr	w10, w13, w10
	add	w16, w14, #4
	stp	w10, w16, [x23, #16]
	cmp	w14, #4
	b.lo	LBB17_49
; %bb.46:
	add	x13, x19, #180, lsl #12         ; =737280
	add	x13, x13, #1384
	ldr	x14, [x23]
LBB17_47:                               ; =>This Inner Loop Header: Depth=1
	cmp	x14, #16, lsl #12               ; =65536
	b.eq	LBB17_102
; %bb.48:                               ;   in Loop: Header=BB17_47 Depth=1
	strb	w10, [x13, x14]
	ldr	x10, [x23]
	add	x14, x10, #1
	str	x14, [x23]
	ldp	w10, w15, [x23, #16]
	lsr	w10, w10, #8
	sub	w16, w15, #8
	stp	w10, w16, [x23, #16]
	cmp	w16, #7
	b.hi	LBB17_47
LBB17_49:
	cbz	w11, LBB17_87
; %bb.50:
	mov	x12, #0                         ; =0x0
	add	x13, x19, #176, lsl #12         ; =720896
	add	x13, x13, #2364
	add	x14, x19, #180, lsl #12         ; =737280
	add	x14, x14, #1384
Lloh56:
	adrp	x15, __ZN8stx_zlib12_GLOBAL__N_110code_orderE@PAGE
Lloh57:
	add	x15, x15, __ZN8stx_zlib12_GLOBAL__N_110code_orderE@PAGEOFF
	mov	w17, #5                         ; =0x5
	b	LBB17_68
LBB17_51:
	mov	w0, #0                          ; =0x0
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_52:
	mov	w14, #0                         ; =0x0
LBB17_53:
	mov	w9, #5                          ; =0x5
	mov	w12, #5                         ; =0x5
	cmp	x8, x10
	b.eq	LBB17_66
; %bb.54:
	ldr	x21, [x22, #8]
	add	x13, x10, #1
	str	x13, [x19, #72]
	and	w12, w21, #0xffff
	lsl	w15, w12, w14
	orr	w11, w11, w15
	orr	w14, w14, #0x10
	stp	w11, w14, [x23, #16]
	add	x14, x19, #180, lsl #12         ; =737280
	add	x14, x14, #1384
	ldr	x15, [x23]
LBB17_55:                               ; =>This Inner Loop Header: Depth=1
	cmp	x15, #16, lsl #12               ; =65536
	b.eq	LBB17_65
; %bb.56:                               ;   in Loop: Header=BB17_55 Depth=1
	strb	w11, [x14, x15]
	ldr	x11, [x23]
	add	x15, x11, #1
	str	x15, [x23]
	ldp	w11, w16, [x23, #16]
	lsr	w11, w11, #8
	sub	w17, w16, #8
	stp	w11, w17, [x23, #16]
	cmp	w17, #7
	b.hi	LBB17_55
; %bb.57:
	mov	w9, #5                          ; =0x5
	mov	w14, #5                         ; =0x5
	cmp	x8, x13
	b.eq	LBB17_84
; %bb.58:
	add	x10, x10, #2
	str	x10, [x19, #72]
	eor	w12, w12, #0xffff
	lsl	w12, w12, w17
	orr	w13, w11, w12
	add	w11, w16, #8
	stp	w13, w11, [x23, #16]
	add	x11, x19, #180, lsl #12         ; =737280
	add	x12, x11, #1384
	ldr	x11, [x23]
LBB17_59:                               ; =>This Inner Loop Header: Depth=1
	cmp	x11, #16, lsl #12               ; =65536
	b.eq	LBB17_83
; %bb.60:                               ;   in Loop: Header=BB17_59 Depth=1
	strb	w13, [x12, x11]
	ldr	x14, [x23]
	add	x11, x14, #1
	str	x11, [x23]
	ldp	w13, w15, [x23, #16]
	lsr	w13, w13, #8
	sub	w15, w15, #8
	stp	w13, w15, [x23, #16]
	cmp	w15, #7
	b.hi	LBB17_59
; %bb.61:
	mov	w9, #65535                      ; =0xffff
	sub	x9, x9, x14
	cmp	x21, x9
	b.ls	LBB17_104
; %bb.62:
	mov	w0, #0                          ; =0x0
Lloh58:
	adrp	x8, lCPI17_0@PAGE
Lloh59:
	ldr	d0, [x8, lCPI17_0@PAGEOFF]
	b	LBB17_33
LBB17_63:
	mov	w13, #6                         ; =0x6
LBB17_64:
	mov	w0, #0                          ; =0x0
	stp	w12, w13, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_65:
	mov	w12, #6                         ; =0x6
LBB17_66:
	mov	w0, #0                          ; =0x0
	stp	w9, w12, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_67:                               ;   in Loop: Header=BB17_68 Depth=1
	add	x12, x12, #1
	mov	x16, x1
	cmp	x12, x11
	b.eq	LBB17_88
LBB17_68:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB17_71 Depth 2
	cmp	x8, x9
	b.eq	LBB17_122
; %bb.69:                               ;   in Loop: Header=BB17_68 Depth=1
	ldr	w0, [x15, x12, lsl #2]
	ldrb	w0, [x13, x0]
	add	x9, x9, #1
	str	x9, [x19, #72]
	and	w0, w0, #0x7
	lsl	w0, w0, w16
	orr	w10, w10, w0
	add	w1, w16, #3
	stp	w10, w1, [x23, #16]
	cmp	w16, #5
	b.lo	LBB17_67
; %bb.70:                               ;   in Loop: Header=BB17_68 Depth=1
	ldr	x16, [x23]
LBB17_71:                               ;   Parent Loop BB17_68 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cmp	x16, #16, lsl #12               ; =65536
	b.eq	LBB17_85
; %bb.72:                               ;   in Loop: Header=BB17_71 Depth=2
	strb	w10, [x14, x16]
	ldr	x10, [x23]
	add	x16, x10, #1
	str	x16, [x23]
	ldp	w10, w0, [x23, #16]
	lsr	w10, w10, #8
	sub	w1, w0, #8
	stp	w10, w1, [x23, #16]
	cmp	w1, #7
	b.hi	LBB17_71
	b	LBB17_67
LBB17_73:
	sub	x8, x11, x20
	add	x12, x9, x20
	str	x12, [x19, #72]
	ldrb	w9, [x22, #16]
	cmp	w9, #1
	b.ne	LBB17_119
; %bb.74:
	ldr	w9, [x19, #84]
	add	x13, x20, x9
	sub	x10, x10, x12
	cmp	x13, x10
	b.hi	LBB17_32
; %bb.75:
	add	x10, x13, x12
	str	x10, [x19, #72]
	mov	w10, #65536                     ; =0x10000
	sub	x10, x10, x8
	cmn	x20, #1
	csel	x10, x10, x20, eq
	cbz	x10, LBB17_111
; %bb.76:
	cmp	x10, #4
	b.lo	LBB17_108
; %bb.77:
	sub	x11, x20, x11
	tst	x11, #0x3ffffffffffffffc
	b.eq	LBB17_108
; %bb.78:
	add	x11, x19, #80, lsl #12          ; =327680
	add	x12, x11, #140
	and	x11, x10, #0xfffffffffffffffc
	dup.4s	v0, w8
	dup.4s	v1, w20
	lsl	x13, x8, #2
	movi.2d	v2, #0x00ffff0000ffff
	movi.2d	v3, #0xffff0000ffff0000
	and	x14, x10, #0xfffffffffffffffc
LBB17_79:                               ; =>This Inner Loop Header: Depth=1
	ldr	q4, [x12, x13]
	and.16b	v5, v4, v2
	sub.4s	v6, v5, v0
	cmeq.4s	v5, v5, v2
	cmhi.4s	v7, v1, v6
	bic.16b	v5, v7, v5
	and.16b	v4, v4, v3
	orr.16b	v4, v6, v4
	and.16b	v4, v4, v5
	orn.16b	v4, v4, v5
	str	q4, [x12], #16
	subs	x14, x14, #4
	b.ne	LBB17_79
; %bb.80:
	cmp	x10, x11
	b.ne	LBB17_109
	b	LBB17_111
LBB17_81:
	mov	w11, #6                         ; =0x6
LBB17_82:
	mov	w0, #0                          ; =0x0
	stp	w10, w11, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_83:
	mov	w14, #6                         ; =0x6
LBB17_84:
	mov	w0, #0                          ; =0x0
	stp	w9, w14, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_85:
	mov	w8, #6                          ; =0x6
LBB17_86:
	mov	w0, #0                          ; =0x0
	stp	w17, w8, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_87:
	mov	x1, x16
LBB17_88:
	ldr	w11, [x21]
	cbz	w11, LBB17_101
; %bb.89:
	mov	x12, #0                         ; =0x0
	add	x13, x19, #176, lsl #12         ; =720896
	add	x13, x13, #2424
	add	x14, x19, #176, lsl #12         ; =720896
	add	x14, x14, #2364
	add	x15, x19, #176, lsl #12         ; =720896
	add	x15, x15, #2384
	add	x16, x19, #180, lsl #12         ; =737280
	add	x16, x16, #1384
	mov	w17, #12                        ; =0xc
	mov	w0, #-1                         ; =0xffffffff
LBB17_90:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB17_93 Depth 2
                                        ;     Child Loop BB17_98 Depth 2
	mov	w3, #5                          ; =0x5
	cmp	x8, x9
	b.eq	LBB17_123
; %bb.91:                               ;   in Loop: Header=BB17_90 Depth=1
	madd	x5, x12, x17, x13
	ldp	w4, w2, [x5, #4]
	ldr	w5, [x5]
	ldrb	w6, [x14, x5]
	ldrh	w7, [x15, x5, lsl #1]
	add	x5, x9, #1
	str	x5, [x19, #72]
	lsl	w21, w0, w6
	bic	w7, w7, w21
	lsl	w7, w7, w1
	orr	w10, w10, w7
	add	w1, w1, w6
	stp	w10, w1, [x23, #16]
	cmp	w1, #8
	b.lo	LBB17_95
; %bb.92:                               ;   in Loop: Header=BB17_90 Depth=1
	ldr	x6, [x23]
LBB17_93:                               ;   Parent Loop BB17_90 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cmp	x6, #16, lsl #12                ; =65536
	b.eq	LBB17_106
; %bb.94:                               ;   in Loop: Header=BB17_93 Depth=2
	strb	w10, [x16, x6]
	ldr	x10, [x23]
	add	x6, x10, #1
	str	x6, [x23]
	ldp	w10, w1, [x23, #16]
	lsr	w10, w10, #8
	sub	w1, w1, #8
	stp	w10, w1, [x23, #16]
	cmp	w1, #7
	b.hi	LBB17_93
LBB17_95:                               ;   in Loop: Header=BB17_90 Depth=1
	mov	w3, #5                          ; =0x5
	cmp	x8, x5
	b.eq	LBB17_123
; %bb.96:                               ;   in Loop: Header=BB17_90 Depth=1
	add	x9, x9, #2
	str	x9, [x19, #72]
	lsl	w5, w0, w2
	bic	w4, w4, w5
	lsl	w4, w4, w1
	orr	w10, w10, w4
	add	w1, w1, w2
	stp	w10, w1, [x23, #16]
	cmp	w1, #8
	b.lo	LBB17_100
; %bb.97:                               ;   in Loop: Header=BB17_90 Depth=1
	ldr	x2, [x23]
LBB17_98:                               ;   Parent Loop BB17_90 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	cmp	x2, #16, lsl #12                ; =65536
	b.eq	LBB17_106
; %bb.99:                               ;   in Loop: Header=BB17_98 Depth=2
	strb	w10, [x16, x2]
	ldr	x10, [x23]
	add	x2, x10, #1
	str	x2, [x23]
	ldp	w10, w1, [x23, #16]
	lsr	w10, w10, #8
	sub	w1, w1, #8
	stp	w10, w1, [x23, #16]
	cmp	w1, #7
	b.hi	LBB17_98
LBB17_100:                              ;   in Loop: Header=BB17_90 Depth=1
	add	x12, x12, #1
	cmp	x12, x11
	b.ne	LBB17_90
LBB17_101:
	mov	x0, x19
	mov	w1, #1                          ; =0x1
	bl	__ZN8stx_zlib13DeflateStream11emit_tokensEb
	cbz	w0, LBB17_27
	b	LBB17_29
LBB17_102:
	mov	w10, #6                         ; =0x6
LBB17_103:
	mov	w0, #0                          ; =0x0
	stp	w12, w10, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_104:
	sub	x8, x8, x10
	cmp	x21, x8
	b.hi	LBB17_32
; %bb.105:
	add	x8, x19, #180, lsl #12          ; =737280
	add	x8, x8, #1384
	add	x9, x10, x21
	str	x9, [x19, #72]
	ldr	x9, [x22]
	add	x9, x19, x9
	add	x0, x8, x11
	add	x1, x9, #120
	mov	x2, x21
	bl	_memcpy
	ldr	x8, [x23]
	add	x8, x8, x21
	str	x8, [x23]
	b	LBB17_29
LBB17_106:
	mov	w8, #6                          ; =0x6
LBB17_107:
	mov	w0, #0                          ; =0x0
	stp	w3, w8, [x19, #48]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_108:
	mov	x11, #0                         ; =0x0
LBB17_109:
	sub	x10, x10, x11
	add	x11, x19, x11, lsl #2
	add	x11, x11, #16, lsl #12          ; =65536
	add	x11, x11, #140
	add	x11, x11, #64, lsl #12          ; =262144
	mov	w12, #65535                     ; =0xffff
LBB17_110:                              ; =>This Inner Loop Header: Depth=1
	ldr	w13, [x11, x8, lsl #2]
	and	w14, w13, #0xffff
	sub	w15, w14, w8
	cmp	w15, w20
	ccmp	w14, w12, #4, lo
	and	w13, w13, #0xffff0000
	orr	w13, w15, w13
	csinv	w13, w13, wzr, ne
	str	w13, [x11], #4
	subs	x10, x10, #1
	b.ne	LBB17_110
LBB17_111:
	cbz	w9, LBB17_119
; %bb.112:
	cmp	w9, #4
	b.hs	LBB17_114
; %bb.113:
	mov	x10, #0                         ; =0x0
	b	LBB17_117
LBB17_114:
	add	x11, x22, #20
	dup.4s	v0, w8
	and	x10, x9, #0xfffffffc
	dup.4s	v1, w20
	movi.2d	v2, #0x00ffff0000ffff
	movi.2d	v3, #0xffff0000ffff0000
	and	x12, x9, #0xfffffffc
LBB17_115:                              ; =>This Inner Loop Header: Depth=1
	ldr	q4, [x11]
	and.16b	v5, v4, v2
	sub.4s	v6, v5, v0
	cmeq.4s	v5, v5, v2
	cmhi.4s	v7, v1, v6
	bic.16b	v5, v7, v5
	and.16b	v4, v4, v3
	orr.16b	v4, v6, v4
	and.16b	v4, v4, v5
	orn.16b	v4, v4, v5
	str	q4, [x11], #16
	subs	x12, x12, #4
	b.ne	LBB17_115
; %bb.116:
	cmp	x10, x9
	b.eq	LBB17_119
LBB17_117:
	sub	x9, x9, x10
	add	x10, x19, x10, lsl #2
	add	x10, x10, #16, lsl #12          ; =65536
	add	x10, x10, #140
	mov	w11, #65535                     ; =0xffff
LBB17_118:                              ; =>This Inner Loop Header: Depth=1
	ldr	w12, [x10]
	and	w13, w12, #0xffff
	sub	w14, w13, w8
	cmp	w14, w20
	ccmp	w13, w11, #4, lo
	and	w12, w12, #0xffff0000
	orr	w12, w14, w12
	csinv	w12, w12, wzr, ne
	str	w12, [x10], #4
	subs	x9, x9, #1
	b.ne	LBB17_118
LBB17_119:
	add	x0, x19, #120
	add	x1, x0, x8
	mov	x2, x20
	bl	_memmove
LBB17_120:
	str	x20, [x22]
LBB17_121:
	str	xzr, [x22, #8]
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB17_122:
	mov	w8, #5                          ; =0x5
	b	LBB17_86
LBB17_123:
	mov	w8, #5                          ; =0x5
	b	LBB17_107
	.loh AdrpAdd	Lloh56, Lloh57
	.loh AdrpLdr	Lloh58, Lloh59
	.cfi_endproc
                                        ; -- End function
	.globl	__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE ; -- Begin function _ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
	.p2align	2
__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE: ; @_ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
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
	mov	x19, x0
	add	x8, x0, #196, lsl #12           ; =802816
	add	x20, x8, #1384
	cmp	w1, #2
	b.ne	LBB18_8
; %bb.1:
	mov	x0, x19
	mov	w1, #1                          ; =0x1
	bl	__ZN8stx_zlib13DeflateStream12encode_blockEb
	cbz	w0, LBB18_38
; %bb.2:
	ldr	w8, [x20, #20]
	cbz	w8, LBB18_22
; %bb.3:
	ldr	x9, [x19, #40]
	ldr	x8, [x19, #72]
	cmp	x9, x8
	b.eq	LBB18_11
; %bb.4:
	add	x8, x8, #1
	str	x8, [x19, #72]
	ldr	w10, [x20, #16]
	mov	w8, #8                          ; =0x8
	str	w8, [x20, #20]
	add	x8, x19, #180, lsl #12          ; =737280
	add	x8, x8, #1384
	ldr	x9, [x20]
LBB18_5:                                ; =>This Inner Loop Header: Depth=1
	cmp	x9, #16, lsl #12                ; =65536
	b.eq	LBB18_34
; %bb.6:                                ;   in Loop: Header=BB18_5 Depth=1
	strb	w10, [x8, x9]
	ldr	x9, [x20]
	add	x9, x9, #1
	str	x9, [x20]
	ldp	w10, w11, [x20, #16]
	lsr	w10, w10, #8
	sub	w15, w11, #8
	stp	w10, w15, [x20, #16]
	cmp	w15, #7
	b.hi	LBB18_5
; %bb.7:
	ldr	w8, [x19]
	cbnz	w8, LBB18_23
	b	LBB18_24
LBB18_8:
	add	x8, x19, #16, lsl #12           ; =65536
	ldr	x8, [x8, #128]
	cbz	x8, LBB18_10
; %bb.9:
	mov	x0, x19
	mov	w1, #0                          ; =0x0
	bl	__ZN8stx_zlib13DeflateStream12encode_blockEb
	cbz	w0, LBB18_38
LBB18_10:
	ldr	x8, [x19, #40]
	ldr	x11, [x19, #72]
	cmp	x8, x11
	b.ne	LBB18_12
LBB18_11:
	mov	w8, #5                          ; =0x5
	b	LBB18_35
LBB18_12:
	add	x9, x11, #1
	str	x9, [x19, #72]
	ldp	w10, w12, [x20, #16]
	add	w14, w12, #3
	str	w14, [x20, #20]
	cmp	w14, #8
	b.lo	LBB18_16
; %bb.13:
	add	x12, x19, #180, lsl #12         ; =737280
	add	x12, x12, #1384
	ldr	x13, [x20]
LBB18_14:                               ; =>This Inner Loop Header: Depth=1
	cmp	x13, #16, lsl #12               ; =65536
	b.eq	LBB18_34
; %bb.15:                               ;   in Loop: Header=BB18_14 Depth=1
	strb	w10, [x12, x13]
	ldr	x10, [x20]
	add	x13, x10, #1
	str	x13, [x20]
	ldp	w10, w14, [x20, #16]
	lsr	w10, w10, #8
	sub	w14, w14, #8
	stp	w10, w14, [x20, #16]
	cmp	w14, #7
	b.hi	LBB18_14
LBB18_16:
	cbz	w14, LBB18_26
; %bb.17:
	cmp	x8, x9
	b.eq	LBB18_11
; %bb.18:
	add	x9, x11, #2
	str	x9, [x19, #72]
	mov	w11, #8                         ; =0x8
	str	w11, [x20, #20]
	add	x11, x19, #180, lsl #12         ; =737280
	add	x11, x11, #1384
	ldr	x12, [x20]
LBB18_19:                               ; =>This Inner Loop Header: Depth=1
	cmp	x12, #16, lsl #12               ; =65536
	b.eq	LBB18_34
; %bb.20:                               ;   in Loop: Header=BB18_19 Depth=1
	strb	w10, [x11, x12]
	ldr	x10, [x20]
	add	x12, x10, #1
	str	x12, [x20]
	ldp	w10, w13, [x20, #16]
	lsr	w10, w10, #8
	sub	w14, w13, #8
	stp	w10, w14, [x20, #16]
	cmp	w14, #7
	b.hi	LBB18_19
; %bb.21:
	add	w12, w13, #8
	cmp	x8, x9
	b.eq	LBB18_11
	b	LBB18_27
LBB18_22:
	mov	w15, #0                         ; =0x0
	ldr	w8, [x19]
	cbz	w8, LBB18_24
LBB18_23:
	mov	w0, #1                          ; =0x1
	mov	w8, #3                          ; =0x3
	b	LBB18_37
LBB18_24:
	ldr	x10, [x19, #40]
	ldr	x13, [x19, #72]
	cmp	x10, x13
	b.ne	LBB18_39
; %bb.25:
	mov	w9, #5                          ; =0x5
	b	LBB18_58
LBB18_26:
	mov	w12, #16                        ; =0x10
	cmp	x8, x9
	b.eq	LBB18_11
LBB18_27:
	add	x11, x9, #1
	str	x11, [x19, #72]
	str	w12, [x20, #20]
	add	x12, x19, #180, lsl #12         ; =737280
	add	x12, x12, #1384
	ldr	x13, [x20]
LBB18_28:                               ; =>This Inner Loop Header: Depth=1
	cmp	x13, #16, lsl #12               ; =65536
	b.eq	LBB18_34
; %bb.29:                               ;   in Loop: Header=BB18_28 Depth=1
	strb	w10, [x12, x13]
	ldr	x10, [x20]
	add	x13, x10, #1
	str	x13, [x20]
	ldp	w10, w14, [x20, #16]
	lsr	w10, w10, #8
	sub	w15, w14, #8
	stp	w10, w15, [x20, #16]
	cmp	w15, #7
	b.hi	LBB18_28
; %bb.30:
	cmp	x8, x11
	b.eq	LBB18_11
; %bb.31:
	add	x8, x9, #2
	str	x8, [x19, #72]
	mov	w8, #65535                      ; =0xffff
	lsl	w8, w8, w15
	orr	w10, w10, w8
	add	w8, w14, #8
	stp	w10, w8, [x20, #16]
	add	x8, x19, #180, lsl #12          ; =737280
	add	x9, x8, #1384
	ldr	x11, [x20]
	mov	w0, #1                          ; =0x1
LBB18_32:                               ; =>This Inner Loop Header: Depth=1
	cmp	x11, #16, lsl #12               ; =65536
	b.eq	LBB18_34
; %bb.33:                               ;   in Loop: Header=BB18_32 Depth=1
	strb	w10, [x9, x11]
	ldr	x8, [x20]
	add	x11, x8, #1
	str	x11, [x20]
	ldp	w8, w12, [x20, #16]
	lsr	w10, w8, #8
	sub	w12, w12, #8
	stp	w10, w12, [x20, #16]
	mov	w8, #2                          ; =0x2
	cmp	w12, #7
	b.hi	LBB18_32
	b	LBB18_37
LBB18_34:
	mov	w8, #6                          ; =0x6
LBB18_35:
	mov	w0, #0                          ; =0x0
	str	w8, [x19, #52]
LBB18_36:
	mov	w8, #5                          ; =0x5
LBB18_37:
	str	w8, [x19, #48]
LBB18_38:
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB18_39:
	ldp	w12, w8, [x20, #24]
	orr	w14, w12, w8, lsl #16
	add	x8, x19, #180, lsl #12          ; =737280
	add	x9, x8, #1384
	add	x8, x13, #4
	lsr	w16, w14, #24
	add	x11, x13, #1
	lsl	w16, w16, w15
	ldr	w17, [x20, #16]
	orr	w16, w17, w16
	orr	w15, w15, #0x8
	stp	w16, w15, [x20, #16]
	ldr	x15, [x20]
LBB18_40:                               ; =>This Inner Loop Header: Depth=1
	cmp	x15, #16, lsl #12               ; =65536
	b.eq	LBB18_56
; %bb.41:                               ;   in Loop: Header=BB18_40 Depth=1
	strb	w16, [x9, x15]
	ldr	x15, [x20]
	add	x15, x15, #1
	str	x15, [x20]
	ldp	w16, w17, [x20, #16]
	lsr	w16, w16, #8
	sub	w0, w17, #8
	stp	w16, w0, [x20, #16]
	cmp	w0, #7
	b.hi	LBB18_40
; %bb.42:
	cmp	x10, x11
	b.ne	LBB18_44
LBB18_43:
	mov	w9, #5                          ; =0x5
	mov	x11, x10
	b	LBB18_57
LBB18_44:
	ubfx	w14, w14, #16, #8
	add	x11, x13, #2
	lsl	w14, w14, w0
	orr	w14, w16, w14
	stp	w14, w17, [x20, #16]
	ldr	x15, [x20]
LBB18_45:                               ; =>This Inner Loop Header: Depth=1
	cmp	x15, #16, lsl #12               ; =65536
	b.eq	LBB18_56
; %bb.46:                               ;   in Loop: Header=BB18_45 Depth=1
	strb	w14, [x9, x15]
	ldr	x14, [x20]
	add	x15, x14, #1
	str	x15, [x20]
	ldp	w14, w16, [x20, #16]
	lsr	w14, w14, #8
	sub	w17, w16, #8
	stp	w14, w17, [x20, #16]
	cmp	w17, #7
	b.hi	LBB18_45
; %bb.47:
	cmp	x10, x11
	b.eq	LBB18_43
; %bb.48:
	ubfx	w15, w12, #8, #8
	add	x11, x13, #3
	lsl	w13, w15, w17
	orr	w13, w14, w13
	stp	w13, w16, [x20, #16]
	ldr	x14, [x20]
LBB18_49:                               ; =>This Inner Loop Header: Depth=1
	cmp	x14, #16, lsl #12               ; =65536
	b.eq	LBB18_56
; %bb.50:                               ;   in Loop: Header=BB18_49 Depth=1
	strb	w13, [x9, x14]
	ldr	x13, [x20]
	add	x14, x13, #1
	str	x14, [x20]
	ldp	w13, w15, [x20, #16]
	lsr	w13, w13, #8
	sub	w16, w15, #8
	stp	w13, w16, [x20, #16]
	cmp	w16, #7
	b.hi	LBB18_49
; %bb.51:
	cmp	x10, x11
	b.eq	LBB18_43
; %bb.52:
	and	w10, w12, #0xff
	lsl	w10, w10, w16
	orr	w11, w13, w10
	stp	w11, w15, [x20, #16]
	ldr	x10, [x20]
LBB18_53:                               ; =>This Inner Loop Header: Depth=1
	cmp	x10, #16, lsl #12               ; =65536
	b.eq	LBB18_59
; %bb.54:                               ;   in Loop: Header=BB18_53 Depth=1
	strb	w11, [x9, x10]
	ldr	x10, [x20]
	add	x10, x10, #1
	str	x10, [x20]
	ldp	w11, w12, [x20, #16]
	lsr	w11, w11, #8
	sub	w12, w12, #8
	stp	w11, w12, [x20, #16]
	cmp	w12, #7
	b.hi	LBB18_53
; %bb.55:
	str	x8, [x19, #72]
	b	LBB18_23
LBB18_56:
	mov	w9, #6                          ; =0x6
LBB18_57:
	str	x11, [x19, #72]
LBB18_58:
	mov	w0, #0                          ; =0x0
	str	w9, [x19, #52]
	b	LBB18_36
LBB18_59:
	mov	w9, #6                          ; =0x6
	mov	x11, x8
	b	LBB18_57
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
lCPI19_0:
	.long	12                              ; 0xc
	.long	13                              ; 0xd
	.long	14                              ; 0xe
	.long	15                              ; 0xf
lCPI19_1:
	.long	8                               ; 0x8
	.long	9                               ; 0x9
	.long	10                              ; 0xa
	.long	11                              ; 0xb
lCPI19_2:
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.long	7                               ; 0x7
lCPI19_3:
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	3                               ; 0x3
lCPI19_4:
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
lCPI19_5:
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
lCPI19_6:
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
lCPI19_7:
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
	.globl	__ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
	.p2align	2
__ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE: ; @_ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #336
	stp	d11, d10, [sp, #208]            ; 16-byte Folded Spill
	stp	d9, d8, [sp, #224]              ; 16-byte Folded Spill
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
	.cfi_offset b8, -104
	.cfi_offset b9, -112
	.cfi_offset b10, -120
	.cfi_offset b11, -128
	mov	x21, x4
	mov	x24, x3
	mov	x22, x2
	mov	x20, x0
	mov	x19, x8
	ldr	w26, [x0, #48]
	sub	w8, w26, #4
	cmp	w8, #2
	stur	w5, [x29, #-124]                ; 4-byte Folded Spill
	b.hs	LBB19_3
LBB19_1:
	and	w8, w26, #0xfffffffe
	cmp	w8, #4
	b.ne	LBB19_5
; %bb.2:
	mov	x23, #0                         ; =0x0
	mov	w8, #0                          ; =0x0
	mov	x28, #0                         ; =0x0
	b	LBB19_67
LBB19_3:
	cbnz	w26, LBB19_45
; %bb.4:
	mov	x23, #0                         ; =0x0
	mov	x28, #0                         ; =0x0
	mov	w8, #2                          ; =0x2
	b	LBB19_54
LBB19_5:
	str	x1, [sp, #104]                  ; 8-byte Spill
	mov	x23, #0                         ; =0x0
	mov	x28, #0                         ; =0x0
	add	x8, x20, #196, lsl #12          ; =802816
	add	x25, x8, #1384
	add	x8, x20, #16, lsl #12           ; =65536
	add	x27, x8, #120
Lloh60:
	adrp	x8, lCPI19_3@PAGE
Lloh61:
	ldr	q0, [x8, lCPI19_3@PAGEOFF]
	str	q0, [sp, #160]                  ; 16-byte Spill
Lloh62:
	adrp	x8, lCPI19_0@PAGE
Lloh63:
	ldr	q0, [x8, lCPI19_0@PAGEOFF]
	str	q0, [sp, #144]                  ; 16-byte Spill
	add	x8, x20, #180, lsl #12          ; =737280
Lloh64:
	adrp	x9, lCPI19_1@PAGE
Lloh65:
	ldr	q0, [x9, lCPI19_1@PAGEOFF]
	str	q0, [sp, #128]                  ; 16-byte Spill
	add	x8, x8, #1384
	stur	x8, [x29, #-136]                ; 8-byte Folded Spill
Lloh66:
	adrp	x8, lCPI19_2@PAGE
Lloh67:
	ldr	q0, [x8, lCPI19_2@PAGEOFF]
	str	q0, [sp, #112]                  ; 16-byte Spill
Lloh68:
	adrp	x8, lCPI19_4@PAGE
Lloh69:
	ldr	q0, [x8, lCPI19_4@PAGEOFF]
	str	q0, [sp, #80]                   ; 16-byte Spill
Lloh70:
	adrp	x8, lCPI19_5@PAGE
Lloh71:
	ldr	q0, [x8, lCPI19_5@PAGEOFF]
	str	q0, [sp, #64]                   ; 16-byte Spill
Lloh72:
	adrp	x8, lCPI19_6@PAGE
Lloh73:
	ldr	q0, [x8, lCPI19_6@PAGEOFF]
	str	q0, [sp, #48]                   ; 16-byte Spill
Lloh74:
	adrp	x8, lCPI19_7@PAGE
Lloh75:
	ldr	q0, [x8, lCPI19_7@PAGEOFF]
	str	q0, [sp, #32]                   ; 16-byte Spill
	stur	x22, [x29, #-144]               ; 8-byte Folded Spill
	b	LBB19_8
LBB19_6:                                ;   in Loop: Header=BB19_8 Depth=1
	mov	x0, x20
	mov	w1, #0                          ; =0x0
	bl	__ZN8stx_zlib13DeflateStream12encode_blockEb
	cbz	w0, LBB19_57
LBB19_7:                                ;   in Loop: Header=BB19_8 Depth=1
	ldr	w26, [x20, #48]
	and	w8, w26, #0xfffffffe
	cmp	w8, #4
	b.eq	LBB19_57
LBB19_8:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB19_31 Depth 2
                                        ;       Child Loop BB19_37 Depth 3
                                        ;       Child Loop BB19_41 Depth 3
                                        ;       Child Loop BB19_44 Depth 3
	ldp	x9, x8, [x25]
	subs	x9, x9, x8
	b.eq	LBB19_14
; %bb.9:                                ;   in Loop: Header=BB19_8 Depth=1
	ldr	x10, [x20, #32]
	ldr	x11, [x20, #64]
	subs	x10, x10, x11
	b.eq	LBB19_61
; %bb.10:                               ;   in Loop: Header=BB19_8 Depth=1
	subs	x11, x21, x28
	b.eq	LBB19_62
; %bb.11:                               ;   in Loop: Header=BB19_8 Depth=1
	cmp	x11, x9
	csel	x9, x11, x9, lo
	cmp	x9, x10
	csel	x26, x9, x10, lo
	ldr	x10, [x20, #40]
	ldr	x9, [x20, #72]
	sub	x10, x10, x9
	cmp	x26, x10
	b.hi	LBB19_63
; %bb.12:                               ;   in Loop: Header=BB19_8 Depth=1
	add	x9, x9, x26
	str	x9, [x20, #72]
	add	x0, x24, x28
	ldur	x9, [x29, #-136]                ; 8-byte Folded Reload
	add	x1, x9, x8
	mov	x2, x26
	bl	_memcpy
	ldp	x9, x8, [x25]
	add	x8, x8, x26
	cmp	x8, x9
	str	x8, [x25, #8]
	add	x28, x26, x28
	ldr	x8, [x20, #64]
	add	x8, x8, x26
	str	x8, [x20, #64]
	b.ne	LBB19_7
; %bb.13:                               ;   in Loop: Header=BB19_8 Depth=1
	ldr	w26, [x20, #48]
LBB19_14:                               ;   in Loop: Header=BB19_8 Depth=1
	stp	xzr, xzr, [x25]
	cmp	w26, #2
	b.eq	LBB19_59
; %bb.15:                               ;   in Loop: Header=BB19_8 Depth=1
	cmp	w26, #3
	b.eq	LBB19_58
; %bb.16:                               ;   in Loop: Header=BB19_8 Depth=1
	ldr	x8, [x27, #8]
	cmp	x8, #8, lsl #12                 ; =32768
	b.ne	LBB19_20
; %bb.17:                               ;   in Loop: Header=BB19_8 Depth=1
	cmp	x22, x23
	b.ne	LBB19_6
; %bb.18:                               ;   in Loop: Header=BB19_8 Depth=1
	ldur	w1, [x29, #-124]                ; 4-byte Folded Reload
	cbz	w1, LBB19_64
; %bb.19:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	x0, x20
	bl	__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
	tbnz	w0, #0, LBB19_7
	b	LBB19_64
LBB19_20:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	x9, x22
	mov	x22, x21
	mov	x21, x24
	subs	x24, x9, x23
	b.ne	LBB19_24
; %bb.21:                               ;   in Loop: Header=BB19_8 Depth=1
	ldur	w1, [x29, #-124]                ; 4-byte Folded Reload
	cbz	w1, LBB19_60
; %bb.22:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	x0, x20
	bl	__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
	tbz	w0, #0, LBB19_60
; %bb.23:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	x24, x21
	mov	x21, x22
	ldur	x22, [x29, #-144]               ; 8-byte Folded Reload
	b	LBB19_7
LBB19_24:                               ;   in Loop: Header=BB19_8 Depth=1
	ldr	x9, [x20, #24]
	ldr	x12, [x20, #56]
	subs	x9, x9, x12
	b.eq	LBB19_69
; %bb.25:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	w10, #32768                     ; =0x8000
	sub	x10, x10, x8
	cmp	x24, x10
	csel	x10, x24, x10, lo
	cmp	x10, x9
	csel	x13, x10, x9, lo
	lsl	x9, x13, #1
	ldr	x11, [x20, #40]
	ldr	x10, [x20, #72]
	sub	x11, x11, x10
	cmp	x9, x11
	b.hi	LBB19_63
; %bb.26:                               ;   in Loop: Header=BB19_8 Depth=1
	stp	x12, x13, [sp, #16]             ; 16-byte Folded Spill
	add	x9, x10, x9
	str	x9, [x20, #72]
	ldr	x9, [x27]
	add	x10, x20, #120
	add	x9, x10, x9
	ldr	x11, [sp, #104]                 ; 8-byte Reload
	add	x12, x11, x23
	str	x12, [sp, #8]                   ; 8-byte Spill
	add	x0, x9, x8
	add	x1, x11, x23
	mov	x2, x13
	bl	_memcpy
	ldr	x1, [sp, #8]                    ; 8-byte Reload
	ldr	x0, [sp, #24]                   ; 8-byte Reload
	movi.4s	v10, #16
	ldp	q9, q8, [sp, #32]               ; 32-byte Folded Reload
	ldp	q31, q30, [sp, #64]             ; 32-byte Folded Reload
	movi.4s	v29, #4
	ldr	w8, [x20]
	cbz	w8, LBB19_28
LBB19_27:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	w8, #0                          ; =0x0
	add	x23, x0, x23
	ldr	x9, [x27, #8]
	add	x9, x9, x0
	str	x9, [x27, #8]
	ldr	x9, [sp, #16]                   ; 8-byte Reload
	add	x9, x0, x9
	str	x9, [x20, #56]
	and	w9, w26, #0xfffffffe
	cmp	w9, #4
	mov	x24, x21
	mov	x21, x22
	ldur	x22, [x29, #-144]               ; 8-byte Folded Reload
	b.ne	LBB19_8
	b	LBB19_67
LBB19_28:                               ;   in Loop: Header=BB19_8 Depth=1
	mov	x8, #0                          ; =0x0
	cmn	x0, #1
	csel	x9, x24, x0, eq
	ldp	w10, w11, [x25, #24]
	b	LBB19_31
LBB19_29:                               ;   in Loop: Header=BB19_31 Depth=2
	mov	w16, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
LBB19_30:                               ;   in Loop: Header=BB19_31 Depth=2
	madd	w11, w10, w12, w11
	add	w11, w11, w16
	add	w10, w15, w10
	mov	w14, #32881                     ; =0x8071
	movk	w14, #32775, lsl #16
	umull	x13, w10, w14
	lsr	x13, x13, #47
	mov	w15, #65521                     ; =0xfff1
	msub	w10, w13, w15, w10
	umull	x13, w11, w14
	lsr	x13, x13, #47
	msub	w11, w13, w15, w11
	stp	w10, w11, [x25, #24]
	add	x8, x12, x8
	cmp	x8, x9
	b.hs	LBB19_27
LBB19_31:                               ;   Parent Loop BB19_8 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB19_37 Depth 3
                                        ;       Child Loop BB19_41 Depth 3
                                        ;       Child Loop BB19_44 Depth 3
	subs	x12, x9, x8
	mov	w13, #5552                      ; =0x15b0
	cmp	x12, x13
	csel	x12, x12, x13, lo
	subs	x13, x9, x8
	b.eq	LBB19_29
; %bb.32:                               ;   in Loop: Header=BB19_31 Depth=2
	cmp	x13, #4
	b.hs	LBB19_34
; %bb.33:                               ;   in Loop: Header=BB19_31 Depth=2
	mov	w15, #0                         ; =0x0
	mov	x13, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
	b	LBB19_43
LBB19_34:                               ;   in Loop: Header=BB19_31 Depth=2
	dup.4s	v0, w12
	cmp	x13, #16
	b.hs	LBB19_36
; %bb.35:                               ;   in Loop: Header=BB19_31 Depth=2
	mov	x13, #0                         ; =0x0
	mov	w15, #0                         ; =0x0
	mov	w16, #0                         ; =0x0
	b	LBB19_40
LBB19_36:                               ;   in Loop: Header=BB19_31 Depth=2
	add	x15, x1, x8
	and	x14, x12, #0xc
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
	and	x13, x12, #0x1ff0
	and	x16, x12, #0x1ff0
	movi.2d	v3, #0000000000000000
	movi.2d	v4, #0000000000000000
	movi.2d	v5, #0000000000000000
	ldp	q20, q17, [sp, #144]            ; 32-byte Folded Reload
	movi.2d	v6, #0000000000000000
	ldp	q18, q19, [sp, #112]            ; 32-byte Folded Reload
	movi.2d	v7, #0000000000000000
	movi.2d	v16, #0000000000000000
LBB19_37:                               ;   Parent Loop BB19_8 Depth=1
                                        ;     Parent Loop BB19_31 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	q21, [x15], #16
	tbl.16b	v22, { v21 }, v30
	tbl.16b	v23, { v21 }, v31
	tbl.16b	v24, { v21 }, v8
	tbl.16b	v21, { v21 }, v9
	add.4s	v4, v4, v21
	add.4s	v3, v3, v24
	add.4s	v2, v2, v23
	add.4s	v5, v5, v22
	sub.4s	v25, v0, v18
	sub.4s	v26, v0, v17
	sub.4s	v27, v0, v19
	sub.4s	v28, v0, v20
	mla.4s	v1, v25, v24
	mla.4s	v6, v26, v23
	mla.4s	v7, v27, v21
	mla.4s	v16, v28, v22
	add.4s	v17, v17, v10
	add.4s	v18, v18, v10
	add.4s	v19, v19, v10
	add.4s	v20, v20, v10
	subs	x16, x16, #16
	b.ne	LBB19_37
; %bb.38:                               ;   in Loop: Header=BB19_31 Depth=2
	add.4s	v2, v2, v4
	add.4s	v3, v3, v5
	add.4s	v2, v2, v3
	addv.4s	s2, v2
	fmov	w15, s2
	add.4s	v2, v6, v7
	add.4s	v1, v1, v16
	add.4s	v1, v2, v1
	addv.4s	s1, v1
	fmov	w16, s1
	cmp	x12, x13
	b.eq	LBB19_30
; %bb.39:                               ;   in Loop: Header=BB19_31 Depth=2
	cbz	x14, LBB19_43
LBB19_40:                               ;   in Loop: Header=BB19_31 Depth=2
	mov	x17, x13
	and	x13, x12, #0x1ffc
	movi.2d	v2, #0000000000000000
	mov.s	v2[0], w15
	movi.2d	v1, #0000000000000000
	mov.s	v1[0], w16
	dup.4s	v3, w17
	ldr	q4, [sp, #160]                  ; 16-byte Reload
	orr.16b	v3, v3, v4
	sub	x14, x17, x13
	add	x15, x17, x8
	add	x15, x1, x15
LBB19_41:                               ;   Parent Loop BB19_8 Depth=1
                                        ;     Parent Loop BB19_31 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldr	s4, [x15], #4
	ushll.8h	v4, v4, #0
	ushll.4s	v5, v4, #0
	uaddw.4s	v2, v2, v4
	sub.4s	v4, v0, v3
	mla.4s	v1, v4, v5
	add.4s	v3, v3, v29
	adds	x14, x14, #4
	b.ne	LBB19_41
; %bb.42:                               ;   in Loop: Header=BB19_31 Depth=2
	addv.4s	s0, v2
	fmov	w15, s0
	addv.4s	s0, v1
	fmov	w16, s0
	cmp	x12, x13
	b.eq	LBB19_30
LBB19_43:                               ;   in Loop: Header=BB19_31 Depth=2
	sub	x14, x12, x13
	add	x13, x13, x8
	add	x13, x1, x13
LBB19_44:                               ;   Parent Loop BB19_8 Depth=1
                                        ;     Parent Loop BB19_31 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldrb	w17, [x13], #1
	add	w15, w15, w17
	madd	w16, w14, w17, w16
	subs	x14, x14, #1
	b.ne	LBB19_44
	b	LBB19_30
LBB19_45:
	cmp	w5, #2
	b.hi	LBB19_53
; %bb.46:
	cbz	x22, LBB19_49
; %bb.47:
	cbz	x21, LBB19_49
; %bb.48:
	sub	x8, x24, x1
	cmp	x8, x22
	cset	w8, lo
	subs	x9, x1, x24
	cmp	x9, x21
	cset	w9, lo
	cmp	x1, x24
	csel	w8, w9, w8, hi
	tbz	w8, #0, LBB19_50
	b	LBB19_53
LBB19_49:
	cbz	x22, LBB19_51
LBB19_50:
	mov	w8, #17800                      ; =0x4588
	movk	w8, #12, lsl #16
	sub	x9, x1, x20
	cmp	x9, x8
	cset	w8, lo
	subs	x9, x20, x1
	cmp	x9, x22
	cset	w9, lo
	cmp	x20, x1
	csel	w8, w9, w8, hi
	tbnz	w8, #0, LBB19_53
LBB19_51:
	cbz	x21, LBB19_1
; %bb.52:
	mov	w8, #17800                      ; =0x4588
	movk	w8, #12, lsl #16
	sub	x9, x24, x20
	cmp	x9, x8
	cset	w8, lo
	subs	x9, x20, x24
	cmp	x9, x21
	cset	w9, lo
	cmp	x20, x24
	csel	w8, w9, w8, hi
	cmp	w8, #1
	b.ne	LBB19_1
LBB19_53:
	mov	x23, #0                         ; =0x0
	mov	x28, #0                         ; =0x0
	mov	w8, #1                          ; =0x1
LBB19_54:
	str	w8, [x20, #52]
	mov	w26, #4                         ; =0x4
LBB19_55:
	mov	w8, #5                          ; =0x5
LBB19_56:
	str	w8, [x20, #48]
	b	LBB19_68
LBB19_57:
	mov	w8, #0                          ; =0x0
	b	LBB19_65
LBB19_58:
	mov	w8, #4                          ; =0x4
	b	LBB19_56
LBB19_59:
	mov	w8, #1                          ; =0x1
	b	LBB19_56
LBB19_60:
	mov	w8, #0                          ; =0x0
	ldur	x23, [x29, #-144]               ; 8-byte Folded Reload
	b	LBB19_65
LBB19_61:
	mov	w26, #4                         ; =0x4
	str	w26, [x20, #52]
	b	LBB19_55
LBB19_62:
	mov	w8, #1                          ; =0x1
	b	LBB19_66
LBB19_63:
	mov	w8, #5                          ; =0x5
	str	w8, [x20, #52]
	mov	w26, #4                         ; =0x4
	b	LBB19_56
LBB19_64:
	mov	w8, #0                          ; =0x0
	mov	x23, x22
LBB19_65:
	mov	x21, x28
LBB19_66:
	ldr	w26, [x20, #48]
	mov	x28, x21
LBB19_67:
	mov	w9, #4                          ; =0x4
	cmp	w26, #5
	csel	w8, w9, w8, eq
	mov	w9, #3                          ; =0x3
	cmp	w26, #4
	csel	w26, w9, w8, eq
LBB19_68:
	ldr	w8, [x20, #52]
	stp	w26, w8, [x19]
	stp	x23, x28, [x19, #8]
	ldur	q0, [x20, #56]
	stur	q0, [x19, #24]
	ldr	x8, [x20, #72]
	str	x8, [x19, #40]
	ldp	x29, x30, [sp, #320]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #304]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #288]            ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #272]            ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #256]            ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #240]            ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #224]              ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #208]            ; 16-byte Folded Reload
	add	sp, sp, #336
	ret
LBB19_69:
	mov	w8, #3                          ; =0x3
	b	LBB19_54
	.loh AdrpLdr	Lloh74, Lloh75
	.loh AdrpAdrp	Lloh72, Lloh74
	.loh AdrpLdr	Lloh72, Lloh73
	.loh AdrpAdrp	Lloh70, Lloh72
	.loh AdrpLdr	Lloh70, Lloh71
	.loh AdrpAdrp	Lloh68, Lloh70
	.loh AdrpLdr	Lloh68, Lloh69
	.loh AdrpAdrp	Lloh66, Lloh68
	.loh AdrpLdr	Lloh66, Lloh67
	.loh AdrpLdr	Lloh64, Lloh65
	.loh AdrpLdr	Lloh62, Lloh63
	.loh AdrpAdrp	Lloh60, Lloh62
	.loh AdrpLdr	Lloh60, Lloh61
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb ; -- Begin function _ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	.weak_def_can_be_hidden	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	.p2align	2
__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb: ; @_ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
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
	mov	x23, x4
	mov	x22, x3
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
LBB20_1:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB20_2 Depth 2
                                        ;       Child Loop BB20_59 Depth 3
                                        ;       Child Loop BB20_61 Depth 3
                                        ;       Child Loop BB20_63 Depth 3
                                        ;       Child Loop BB20_67 Depth 3
                                        ;         Child Loop BB20_68 Depth 4
                                        ;         Child Loop BB20_69 Depth 4
	mov	x25, x19
LBB20_2:                                ;   Parent Loop BB20_1 Depth=1
                                        ; =>  This Loop Header: Depth=2
                                        ;       Child Loop BB20_59 Depth 3
                                        ;       Child Loop BB20_61 Depth 3
                                        ;       Child Loop BB20_63 Depth 3
                                        ;       Child Loop BB20_67 Depth 3
                                        ;         Child Loop BB20_68 Depth 4
                                        ;         Child Loop BB20_69 Depth 4
	mov	x19, x25
	sub	x8, x20, x25
	asr	x9, x8, #3
	cmp	x9, #2
	b.gt	LBB20_5
; %bb.3:                                ;   in Loop: Header=BB20_2 Depth=2
	b.lo	LBB20_91
; %bb.4:                                ;   in Loop: Header=BB20_2 Depth=2
	cmp	x9, #2
	b.ne	LBB20_8
	b	LBB20_86
LBB20_5:                                ;   in Loop: Header=BB20_2 Depth=2
	cmp	x9, #3
	b.eq	LBB20_88
; %bb.6:                                ;   in Loop: Header=BB20_2 Depth=2
	cmp	x9, #4
	b.eq	LBB20_92
; %bb.7:                                ;   in Loop: Header=BB20_2 Depth=2
	cmp	x9, #5
	b.eq	LBB20_81
LBB20_8:                                ;   in Loop: Header=BB20_2 Depth=2
	cmp	x9, #23
	b.le	LBB20_93
; %bb.9:                                ;   in Loop: Header=BB20_2 Depth=2
	cbz	x22, LBB20_104
; %bb.10:                               ;   in Loop: Header=BB20_2 Depth=2
	lsr	x8, x9, #1
	add	x8, x19, x8, lsl #3
	ldp	w10, w11, [x20, #-8]
	cmp	x9, #129
	b.lo	LBB20_14
; %bb.11:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w9, w12, [x8]
	ldp	w13, w14, [x19]
	cmp	w12, w14
	cset	w14, lo
	cmp	w9, w13
	cset	w13, lo
	csel	w13, w14, w13, eq
	cmp	w11, w12
	cset	w11, lo
	cmp	w10, w9
	cset	w9, lo
	csel	w10, w11, w9, eq
	tbz	w13, #0, LBB20_17
; %bb.12:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19]
	cbz	w10, LBB20_23
; %bb.13:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-8]
	str	x10, [x19]
	b	LBB20_25
LBB20_14:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w9, w12, [x19]
	ldp	w13, w14, [x8]
	cmp	w12, w14
	cset	w14, lo
	cmp	w9, w13
	cset	w13, lo
	csel	w13, w14, w13, eq
	cmp	w11, w12
	cset	w11, lo
	cmp	w10, w9
	cset	w9, lo
	csel	w10, w11, w9, eq
	tbz	w13, #0, LBB20_20
; %bb.15:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x8]
	cbz	w10, LBB20_32
; %bb.16:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-8]
	str	x10, [x8]
	b	LBB20_34
LBB20_17:                               ;   in Loop: Header=BB20_2 Depth=2
	cbz	w10, LBB20_26
; %bb.18:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x8]
	ldur	x10, [x20, #-8]
	str	x10, [x8]
	stur	x9, [x20, #-8]
	ldp	w9, w10, [x8]
	ldp	w11, w12, [x19]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB20_26
; %bb.19:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19]
	ldr	x10, [x8]
	str	x10, [x19]
	str	x9, [x8]
	b	LBB20_26
LBB20_20:                               ;   in Loop: Header=BB20_2 Depth=2
	cbz	w10, LBB20_35
; %bb.21:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19]
	ldur	x10, [x20, #-8]
	str	x10, [x19]
	stur	x9, [x20, #-8]
	ldp	w9, w10, [x19]
	ldp	w11, w12, [x8]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB20_35
; %bb.22:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x8]
	ldr	x10, [x19]
	str	x10, [x8]
	str	x9, [x19]
	sub	x22, x22, #1
	tbz	w23, #0, LBB20_57
	b	LBB20_58
LBB20_23:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x8]
	str	x10, [x19]
	str	x9, [x8]
	ldp	w10, w11, [x20, #-8]
	lsr	x12, x9, #32
	cmp	w11, w12
	cset	w11, lo
	cmp	w10, w9
	cset	w10, lo
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB20_26
; %bb.24:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-8]
	str	x10, [x8]
LBB20_25:                               ;   in Loop: Header=BB20_2 Depth=2
	stur	x9, [x20, #-8]
LBB20_26:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w9, w10, [x8, #-8]
	ldp	w11, w12, [x19, #8]
	cmp	w10, w12
	cset	w12, lo
	cmp	w9, w11
	cset	w11, lo
	csel	w11, w12, w11, eq
	ldp	w12, w13, [x20, #-16]
	cmp	w13, w10
	cset	w10, lo
	cmp	w12, w9
	cset	w9, lo
	csel	w10, w10, w9, eq
	tbz	w11, #0, LBB20_29
; %bb.27:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19, #8]
	cbz	w10, LBB20_36
; %bb.28:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-16]
	str	x10, [x19, #8]
	b	LBB20_38
LBB20_29:                               ;   in Loop: Header=BB20_2 Depth=2
	cbz	w10, LBB20_39
; %bb.30:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x9, [x8, #-8]
	ldur	x10, [x20, #-16]
	stur	x10, [x8, #-8]
	stur	x9, [x20, #-16]
	ldp	w9, w10, [x8, #-8]
	ldp	w11, w12, [x19, #8]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB20_39
; %bb.31:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19, #8]
	ldur	x10, [x8, #-8]
	str	x10, [x19, #8]
	stur	x9, [x8, #-8]
	b	LBB20_39
LBB20_32:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x19]
	str	x10, [x8]
	str	x9, [x19]
	ldp	w8, w10, [x20, #-8]
	lsr	x11, x9, #32
	cmp	w10, w11
	cset	w10, lo
	cmp	w8, w9
	cset	w8, lo
	csel	w8, w10, w8, eq
	cmp	w8, #1
	b.ne	LBB20_35
; %bb.33:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x8, [x20, #-8]
	str	x8, [x19]
LBB20_34:                               ;   in Loop: Header=BB20_2 Depth=2
	stur	x9, [x20, #-8]
LBB20_35:                               ;   in Loop: Header=BB20_2 Depth=2
	sub	x22, x22, #1
	tbz	w23, #0, LBB20_57
	b	LBB20_58
LBB20_36:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x8, #-8]
	str	x10, [x19, #8]
	stur	x9, [x8, #-8]
	ldp	w10, w11, [x20, #-16]
	lsr	x12, x9, #32
	cmp	w11, w12
	cset	w11, lo
	cmp	w10, w9
	cset	w10, lo
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB20_39
; %bb.37:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-16]
	stur	x10, [x8, #-8]
LBB20_38:                               ;   in Loop: Header=BB20_2 Depth=2
	stur	x9, [x20, #-16]
LBB20_39:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w9, w10, [x8, #8]
	ldp	w11, w12, [x19, #16]
	cmp	w10, w12
	cset	w12, lo
	cmp	w9, w11
	cset	w11, lo
	csel	w11, w12, w11, eq
	ldp	w12, w13, [x20, #-24]
	cmp	w13, w10
	cset	w10, lo
	cmp	w12, w9
	cset	w9, lo
	csel	w10, w10, w9, eq
	tbz	w11, #0, LBB20_42
; %bb.40:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19, #16]
	cbz	w10, LBB20_45
; %bb.41:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-24]
	str	x10, [x19, #16]
	b	LBB20_47
LBB20_42:                               ;   in Loop: Header=BB20_2 Depth=2
	cbz	w10, LBB20_48
; %bb.43:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x8, #8]
	ldur	x10, [x20, #-24]
	str	x10, [x8, #8]
	stur	x9, [x20, #-24]
	ldp	w9, w10, [x8, #8]
	ldp	w11, w12, [x19, #16]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB20_48
; %bb.44:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x19, #16]
	ldr	x10, [x8, #8]
	str	x10, [x19, #16]
	str	x9, [x8, #8]
	b	LBB20_48
LBB20_45:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x8, #8]
	str	x10, [x19, #16]
	str	x9, [x8, #8]
	ldp	w10, w11, [x20, #-24]
	lsr	x12, x9, #32
	cmp	w11, w12
	cset	w11, lo
	cmp	w10, w9
	cset	w10, lo
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB20_48
; %bb.46:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x10, [x20, #-24]
	str	x10, [x8, #8]
LBB20_47:                               ;   in Loop: Header=BB20_2 Depth=2
	stur	x9, [x20, #-24]
LBB20_48:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w9, w14, [x8]
	ldp	w10, w11, [x8, #-8]
	cmp	w14, w11
	cset	w12, lo
	cmp	w9, w10
	cset	w13, lo
	csel	w15, w12, w13, eq
	ldp	w12, w13, [x8, #8]
	cmp	w13, w14
	cset	w14, lo
	cmp	w12, w9
	cset	w9, lo
	csel	w14, w14, w9, eq
	tbz	w15, #0, LBB20_51
; %bb.49:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x9, [x8, #-8]
	cbz	w14, LBB20_54
; %bb.50:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x8, #8]
	stur	x10, [x8, #-8]
	str	x9, [x8, #8]
	ldr	x9, [x8]
	ldr	x10, [x19]
	str	x9, [x19]
	str	x10, [x8]
	sub	x22, x22, #1
	tbz	w23, #0, LBB20_57
	b	LBB20_58
LBB20_51:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x8]
	cbz	w14, LBB20_56
; %bb.52:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x12, [x8, #8]
	stp	x12, x9, [x8]
	lsr	x9, x12, #32
	cmp	w11, w9
	cset	w9, hi
	cmp	w10, w12
	cset	w10, hi
	csel	w9, w9, w10, eq
	cmp	w9, #1
	b.ne	LBB20_78
; %bb.53:                               ;   in Loop: Header=BB20_2 Depth=2
	ldur	x9, [x8, #-8]
	stp	x12, x9, [x8, #-8]
	ldr	x10, [x19]
	str	x9, [x19]
	str	x10, [x8]
	sub	x22, x22, #1
	tbz	w23, #0, LBB20_57
	b	LBB20_58
LBB20_54:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x8]
	stp	x10, x9, [x8, #-8]
	lsr	x10, x9, #32
	cmp	w13, w10
	cset	w10, lo
	cmp	w12, w9
	cset	w11, lo
	csel	w10, w10, w11, eq
	cmp	w10, #1
	b.ne	LBB20_56
; %bb.55:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x8, #8]
	stp	x10, x9, [x8]
	mov	x9, x10
LBB20_56:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x19]
	str	x9, [x19]
	str	x10, [x8]
	sub	x22, x22, #1
	tbnz	w23, #0, LBB20_58
LBB20_57:                               ;   in Loop: Header=BB20_2 Depth=2
	ldp	w8, w9, [x19, #-8]
	ldp	w10, w11, [x19]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	tbz	w8, #0, LBB20_79
LBB20_58:                               ;   in Loop: Header=BB20_2 Depth=2
	mov	x12, #0                         ; =0x0
	ldr	x8, [x19]
	lsr	x9, x8, #32
LBB20_59:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	add	x10, x19, x12
	ldp	w11, w10, [x10, #8]
	cmp	w10, w9
	cset	w10, lo
	cmp	w11, w8
	cset	w11, lo
	csel	w10, w10, w11, eq
	add	x12, x12, #8
	tbnz	w10, #0, LBB20_59
; %bb.60:                               ;   in Loop: Header=BB20_2 Depth=2
	add	x10, x19, x12
	mov	x11, x20
	cmp	x12, #8
	b.eq	LBB20_62
LBB20_61:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	ldp	w12, w13, [x11, #-8]!
	cmp	w13, w9
	cset	w13, lo
	cmp	w12, w8
	cset	w12, lo
	csel	w12, w13, w12, eq
	cmp	w12, #1
	b.ne	LBB20_61
	b	LBB20_65
LBB20_62:                               ;   in Loop: Header=BB20_2 Depth=2
	mov	x11, x20
LBB20_63:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ; =>    This Inner Loop Header: Depth=3
	cmp	x10, x11
	b.hs	LBB20_65
; %bb.64:                               ;   in Loop: Header=BB20_63 Depth=3
	ldp	w12, w13, [x11, #-8]!
	cmp	w13, w9
	cset	w13, lo
	cmp	w12, w8
	cset	w12, lo
	csel	w12, w13, w12, eq
	tbz	w12, #0, LBB20_63
LBB20_65:                               ;   in Loop: Header=BB20_2 Depth=2
	cmp	x10, x11
	b.hs	LBB20_72
; %bb.66:                               ;   in Loop: Header=BB20_2 Depth=2
	mov	x12, x10
	mov	x13, x11
LBB20_67:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ; =>    This Loop Header: Depth=3
                                        ;         Child Loop BB20_68 Depth 4
                                        ;         Child Loop BB20_69 Depth 4
	ldr	x14, [x12]
	ldr	x15, [x13]
	str	x15, [x12]
	str	x14, [x13]
LBB20_68:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ;       Parent Loop BB20_67 Depth=3
                                        ; =>      This Inner Loop Header: Depth=4
	ldp	w14, w15, [x12, #8]!
	cmp	w15, w9
	cset	w15, lo
	cmp	w14, w8
	cset	w14, lo
	csel	w14, w15, w14, eq
	tbnz	w14, #0, LBB20_68
LBB20_69:                               ;   Parent Loop BB20_1 Depth=1
                                        ;     Parent Loop BB20_2 Depth=2
                                        ;       Parent Loop BB20_67 Depth=3
                                        ; =>      This Inner Loop Header: Depth=4
	ldp	w14, w15, [x13, #-8]!
	cmp	w15, w9
	cset	w15, lo
	cmp	w14, w8
	cset	w14, lo
	csel	w14, w15, w14, eq
	cmp	w14, #1
	b.ne	LBB20_69
; %bb.70:                               ;   in Loop: Header=BB20_67 Depth=3
	cmp	x12, x13
	b.lo	LBB20_67
; %bb.71:                               ;   in Loop: Header=BB20_2 Depth=2
	sub	x24, x12, #8
	cmp	x19, x24
	b.ne	LBB20_73
	b	LBB20_74
LBB20_72:                               ;   in Loop: Header=BB20_2 Depth=2
	sub	x24, x10, #8
	cmp	x19, x24
	b.eq	LBB20_74
LBB20_73:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x9, [x24]
	str	x9, [x19]
LBB20_74:                               ;   in Loop: Header=BB20_2 Depth=2
	str	x8, [x24]
	cmp	x10, x11
	b.lo	LBB20_77
; %bb.75:                               ;   in Loop: Header=BB20_2 Depth=2
	mov	x0, x19
	mov	x1, x24
	mov	x2, x21
	bl	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	mov	x26, x0
	add	x25, x24, #8
	add	x0, x24, #8
	mov	x1, x20
	mov	x2, x21
	bl	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	cbnz	w0, LBB20_80
; %bb.76:                               ;   in Loop: Header=BB20_2 Depth=2
	tbnz	w26, #0, LBB20_2
LBB20_77:                               ;   in Loop: Header=BB20_2 Depth=2
	and	w4, w23, #0x1
	mov	x0, x19
	mov	x1, x24
	mov	x2, x21
	mov	x3, x22
	bl	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	mov	w23, #0                         ; =0x0
	add	x25, x24, #8
	b	LBB20_2
LBB20_78:                               ;   in Loop: Header=BB20_2 Depth=2
	ldr	x10, [x19]
	str	x12, [x19]
	str	x10, [x8]
	sub	x22, x22, #1
	tbz	w23, #0, LBB20_57
	b	LBB20_58
LBB20_79:                               ;   in Loop: Header=BB20_2 Depth=2
	mov	x0, x19
	mov	x1, x20
	mov	x2, x21
	bl	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	mov	x25, x0
	mov	w23, #0                         ; =0x0
	b	LBB20_2
LBB20_80:                               ;   in Loop: Header=BB20_1 Depth=1
	mov	x20, x24
	tbz	w26, #0, LBB20_1
	b	LBB20_91
LBB20_81:
	add	x1, x19, #8
	add	x2, x19, #16
	add	x3, x19, #24
	mov	x0, x19
	mov	x4, x21
	bl	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	ldp	w8, w9, [x20, #-8]
	ldp	w10, w11, [x19, #24]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB20_91
; %bb.82:
	ldr	x8, [x19, #24]
	ldur	x9, [x20, #-8]
	str	x9, [x19, #24]
	stur	x8, [x20, #-8]
	ldp	w8, w9, [x19, #24]
	ldp	w10, w11, [x19, #16]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB20_91
; %bb.83:
	ldp	x9, x8, [x19, #16]
	stp	x8, x9, [x19, #16]
	lsr	x9, x8, #32
	ldp	w10, w11, [x19, #8]
	cmp	w11, w9
	cset	w11, hi
	cmp	w10, w8
	cset	w10, hi
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB20_91
; %bb.84:
	ldr	x10, [x19, #8]
	stp	x8, x10, [x19, #8]
	ldp	w10, w11, [x19]
	cmp	w11, w9
	cset	w9, hi
	cmp	w10, w8
	cset	w10, hi
	csel	w9, w9, w10, eq
	cmp	w9, #1
	b.ne	LBB20_91
; %bb.85:
	ldr	x9, [x19]
	stp	x8, x9, [x19]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB20_86:
	ldp	w8, w9, [x20, #-8]
	ldp	w10, w11, [x19]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB20_91
; %bb.87:
	ldr	x8, [x19]
	b	LBB20_90
LBB20_88:
	ldp	w8, w9, [x19, #8]
	ldp	w10, w11, [x19]
	cmp	w9, w11
	cset	w11, lo
	cmp	w8, w10
	cset	w10, lo
	csel	w10, w11, w10, eq
	ldp	w11, w12, [x20, #-8]
	cmp	w12, w9
	cset	w9, lo
	cmp	w11, w8
	cset	w8, lo
	csel	w9, w9, w8, eq
	tbz	w10, #0, LBB20_106
; %bb.89:
	ldr	x8, [x19]
	cbz	w9, LBB20_115
LBB20_90:
	ldur	x9, [x20, #-8]
	str	x9, [x19]
	stur	x8, [x20, #-8]
LBB20_91:
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB20_92:
	sub	x3, x20, #8
	add	x1, x19, #8
	add	x2, x19, #16
	mov	x0, x19
	mov	x4, x21
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	b	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
LBB20_93:
	cmp	x19, x20
	add	x10, x19, #8
	ccmp	x10, x20, #4, ne
	cset	w8, eq
	tbz	w23, #0, LBB20_109
; %bb.94:
	tbnz	w8, #0, LBB20_91
; %bb.95:
	mov	x8, #0                          ; =0x0
	mov	x9, x19
	b	LBB20_99
LBB20_96:                               ;   in Loop: Header=BB20_99 Depth=1
	mov	x11, x19
LBB20_97:                               ;   in Loop: Header=BB20_99 Depth=1
	str	x10, [x11]
LBB20_98:                               ;   in Loop: Header=BB20_99 Depth=1
	add	x10, x9, #8
	add	x8, x8, #8
	cmp	x10, x20
	b.eq	LBB20_91
LBB20_99:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB20_101 Depth 2
	ldp	w11, w12, [x9, #8]
	ldp	w13, w14, [x9]
	mov	x9, x10
	cmp	w12, w14
	cset	w10, lo
	cmp	w11, w13
	cset	w11, lo
	csel	w10, w10, w11, eq
	cmp	w10, #1
	b.ne	LBB20_98
; %bb.100:                              ;   in Loop: Header=BB20_99 Depth=1
	ldr	x10, [x9]
	lsr	x11, x10, #32
	mov	x12, x8
LBB20_101:                              ;   Parent Loop BB20_99 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x13, x19, x12
	ldr	x14, [x13]
	str	x14, [x13, #8]
	cbz	x12, LBB20_96
; %bb.102:                              ;   in Loop: Header=BB20_101 Depth=2
	ldp	w14, w13, [x13, #-8]
	cmp	w13, w11
	cset	w13, hi
	cmp	w14, w10
	cset	w14, hi
	csel	w13, w13, w14, eq
	sub	x12, x12, #8
	tbnz	w13, #0, LBB20_101
; %bb.103:                              ;   in Loop: Header=BB20_99 Depth=1
	add	x11, x19, x12
	add	x11, x11, #8
	b	LBB20_97
LBB20_104:
	cmp	x19, x20
	b.eq	LBB20_91
; %bb.105:
	mov	x0, x19
	mov	x1, x20
	mov	x2, x20
	mov	x3, x21
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	b	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
LBB20_106:
	cbz	w9, LBB20_91
; %bb.107:
	ldr	x8, [x19, #8]
	ldur	x9, [x20, #-8]
	str	x9, [x19, #8]
	stur	x8, [x20, #-8]
	ldp	w8, w9, [x19, #8]
	ldp	w10, w11, [x19]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB20_91
; %bb.108:
	ldr	q0, [x19]
	ext.16b	v0, v0, v0, #8
	str	q0, [x19]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
LBB20_109:
	tbz	w8, #0, LBB20_111
	b	LBB20_91
LBB20_110:                              ;   in Loop: Header=BB20_111 Depth=1
	add	x10, x19, #8
	cmp	x10, x20
	b.eq	LBB20_91
LBB20_111:                              ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB20_113 Depth 2
	ldp	w8, w9, [x19, #8]
	ldp	w11, w12, [x19]
	mov	x19, x10
	cmp	w9, w12
	cset	w9, lo
	cmp	w8, w11
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB20_110
; %bb.112:                              ;   in Loop: Header=BB20_111 Depth=1
	ldr	x8, [x19]
	lsr	x9, x8, #32
	mov	x10, x19
LBB20_113:                              ;   Parent Loop BB20_111 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x11, x10
	ldr	x12, [x10, #-8]!
	str	x12, [x11]
	ldp	w12, w11, [x11, #-16]
	cmp	w11, w9
	cset	w11, hi
	cmp	w12, w8
	cset	w12, hi
	csel	w11, w11, w12, eq
	tbnz	w11, #0, LBB20_113
; %bb.114:                              ;   in Loop: Header=BB20_111 Depth=1
	str	x8, [x10]
	b	LBB20_110
LBB20_115:
	ldr	x9, [x19, #8]
	stp	x9, x8, [x19]
	ldp	w9, w10, [x20, #-8]
	lsr	x11, x8, #32
	cmp	w10, w11
	cset	w10, lo
	cmp	w9, w8
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB20_91
; %bb.116:
	ldur	x9, [x20, #-8]
	str	x9, [x19, #8]
	stur	x8, [x20, #-8]
	ldp	x29, x30, [sp, #64]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #48]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #32]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #16]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp], #80             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_ ; -- Begin function _ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.globl	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.weak_def_can_be_hidden	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.p2align	2
__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_: ; @_ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.cfi_startproc
; %bb.0:
	ldp	w10, w11, [x1]
	ldp	w8, w9, [x0]
	cmp	w11, w9
	cset	w9, lo
	cmp	w10, w8
	cset	w8, lo
	csel	w12, w9, w8, eq
	ldp	w8, w9, [x2]
	cmp	w9, w11
	cset	w11, lo
	cmp	w8, w10
	cset	w10, lo
	csel	w10, w11, w10, eq
	tbz	w12, #0, LBB21_3
; %bb.1:
	ldr	x8, [x0]
	cbz	w10, LBB21_6
; %bb.2:
	ldr	x9, [x2]
	str	x9, [x0]
	str	x8, [x2]
	lsr	x9, x8, #32
                                        ; kill: def $w9 killed $w9 killed $x9 def $x9
	b	LBB21_10
LBB21_3:
	cbz	w10, LBB21_10
; %bb.4:
	ldr	x8, [x1]
	ldr	x9, [x2]
	str	x9, [x1]
	str	x8, [x2]
	ldp	w9, w10, [x1]
	ldp	w11, w12, [x0]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB21_8
; %bb.5:
	ldr	x8, [x0]
	ldr	x9, [x1]
	str	x9, [x0]
	str	x8, [x1]
	ldp	w8, w9, [x2]
	b	LBB21_10
LBB21_6:
	ldr	x9, [x1]
	str	x9, [x0]
	str	x8, [x1]
	ldp	w10, w11, [x2]
	lsr	x9, x8, #32
	cmp	w11, w9
	cset	w12, lo
	cmp	w10, w8
	cset	w13, lo
	csel	w12, w12, w13, eq
	cmp	w12, #1
	b.ne	LBB21_9
; %bb.7:
	ldr	x10, [x2]
	str	x10, [x1]
	str	x8, [x2]
	b	LBB21_10
LBB21_8:
	lsr	x9, x8, #32
                                        ; kill: def $w9 killed $w9 killed $x9 def $x9
                                        ; kill: def $w8 killed $w8 killed $x8 def $x8
	b	LBB21_10
LBB21_9:
	mov	x9, x11
	mov	x8, x10
LBB21_10:
	ldp	w10, w11, [x3]
	cmp	w11, w9
	cset	w9, lo
	cmp	w10, w8
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB21_14
; %bb.11:
	ldr	x8, [x2]
	ldr	x9, [x3]
	str	x9, [x2]
	str	x8, [x3]
	ldp	w8, w9, [x2]
	ldp	w10, w11, [x1]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB21_14
; %bb.12:
	ldr	x8, [x1]
	ldr	x9, [x2]
	str	x9, [x1]
	str	x8, [x2]
	ldp	w8, w9, [x1]
	ldp	w10, w11, [x0]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB21_14
; %bb.13:
	ldr	x8, [x0]
	ldr	x9, [x1]
	str	x9, [x0]
	str	x8, [x1]
LBB21_14:
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_ ; -- Begin function _ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.globl	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.weak_def_can_be_hidden	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.p2align	2
__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_: ; @_ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.cfi_startproc
; %bb.0:
	ldr	x9, [x0]
	lsr	x10, x9, #32
	ldp	w8, w11, [x1, #-8]
	cmp	w11, w10
	cset	w11, hi
	cmp	w8, w9
	cset	w8, hi
	csel	w8, w11, w8, eq
	tbz	w8, #0, LBB22_3
; %bb.1:
	mov	x8, x0
LBB22_2:                                ; =>This Inner Loop Header: Depth=1
	ldp	w11, w12, [x8, #8]!
	cmp	w12, w10
	cset	w12, hi
	cmp	w11, w9
	cset	w11, hi
	csel	w11, w12, w11, eq
	tbz	w11, #0, LBB22_2
	b	LBB22_6
LBB22_3:
	add	x11, x0, #8
LBB22_4:                                ; =>This Inner Loop Header: Depth=1
	mov	x8, x11
	cmp	x11, x1
	b.hs	LBB22_6
; %bb.5:                                ;   in Loop: Header=BB22_4 Depth=1
	ldp	w11, w12, [x8]
	cmp	w12, w10
	cset	w12, hi
	cmp	w11, w9
	cset	w11, hi
	csel	w12, w12, w11, eq
	add	x11, x8, #8
	cmp	w12, #1
	b.ne	LBB22_4
LBB22_6:
	cmp	x8, x1
	b.hs	LBB22_11
LBB22_7:                                ; =>This Inner Loop Header: Depth=1
	ldp	w11, w12, [x1, #-8]!
	cmp	w12, w10
	cset	w12, hi
	cmp	w11, w9
	cset	w11, hi
	csel	w11, w12, w11, eq
	tbnz	w11, #0, LBB22_7
	b	LBB22_11
LBB22_8:                                ;   in Loop: Header=BB22_11 Depth=1
	ldr	x11, [x8]
	ldr	x12, [x1]
	str	x12, [x8]
	str	x11, [x1]
LBB22_9:                                ;   Parent Loop BB22_11 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	w11, w12, [x8, #8]!
	cmp	w12, w10
	cset	w12, hi
	cmp	w11, w9
	cset	w11, hi
	csel	w11, w12, w11, eq
	cmp	w11, #1
	b.ne	LBB22_9
LBB22_10:                               ;   Parent Loop BB22_11 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ldp	w11, w12, [x1, #-8]!
	cmp	w12, w10
	cset	w12, hi
	cmp	w11, w9
	cset	w11, hi
	csel	w11, w12, w11, eq
	tbnz	w11, #0, LBB22_10
LBB22_11:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB22_9 Depth 2
                                        ;     Child Loop BB22_10 Depth 2
	cmp	x8, x1
	b.lo	LBB22_8
; %bb.12:
	sub	x10, x8, #8
	cmp	x0, x10
	b.eq	LBB22_14
; %bb.13:
	ldr	x10, [x10]
	str	x10, [x0]
LBB22_14:
	stur	x9, [x8, #-8]
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_ ; -- Begin function _ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.globl	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.weak_def_can_be_hidden	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.p2align	2
__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_: ; @_ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
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
	sub	x8, x1, x0
	asr	x8, x8, #3
	cmp	x8, #2
	b.gt	LBB23_5
; %bb.1:
	b.lo	LBB23_40
; %bb.2:
	cmp	x8, #2
	b.ne	LBB23_16
; %bb.3:
	ldp	w8, w9, [x1, #-8]
	ldp	w10, w11, [x0]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB23_40
; %bb.4:
	ldr	x8, [x0]
	b	LBB23_15
LBB23_5:
	cmp	x8, #3
	b.eq	LBB23_13
; %bb.6:
	mov	x4, x2
	cmp	x8, #4
	b.eq	LBB23_19
; %bb.7:
	cmp	x8, #5
	b.ne	LBB23_16
; %bb.8:
	mov	x20, x1
	add	x1, x0, #8
	add	x2, x0, #16
	add	x3, x0, #24
	mov	x19, x0
	bl	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	ldp	w9, w10, [x20, #-8]
	ldp	w11, w12, [x19, #24]
	cmp	w10, w12
	cset	w10, lo
	cmp	w9, w11
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB23_40
; %bb.9:
	ldr	x9, [x19, #24]
	ldur	x10, [x20, #-8]
	str	x10, [x19, #24]
	stur	x9, [x20, #-8]
	ldp	w8, w9, [x19, #24]
	ldp	w10, w11, [x19, #16]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB23_40
; %bb.10:
	ldp	x9, x8, [x19, #16]
	stp	x8, x9, [x19, #16]
	lsr	x9, x8, #32
	ldp	w10, w11, [x19, #8]
	cmp	w11, w9
	cset	w11, hi
	cmp	w10, w8
	cset	w10, hi
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB23_40
; %bb.11:
	ldr	x10, [x19, #8]
	stp	x8, x10, [x19, #8]
	ldp	w10, w11, [x19]
	cmp	w11, w9
	cset	w9, hi
	cmp	w10, w8
	cset	w10, hi
	csel	w9, w9, w10, eq
	cmp	w9, #1
	b.ne	LBB23_40
; %bb.12:
	ldr	x9, [x19]
	stp	x8, x9, [x19]
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_13:
	ldp	w8, w9, [x0, #8]
	ldp	w10, w11, [x0]
	cmp	w9, w11
	cset	w11, lo
	cmp	w8, w10
	cset	w10, lo
	csel	w10, w11, w10, eq
	ldp	w11, w12, [x1, #-8]
	cmp	w12, w9
	cset	w9, lo
	cmp	w11, w8
	cset	w8, lo
	csel	w9, w9, w8, eq
	tbz	w10, #0, LBB23_20
; %bb.14:
	ldr	x8, [x0]
	cbz	w9, LBB23_26
LBB23_15:
	ldur	x9, [x1, #-8]
	str	x9, [x0]
	stur	x8, [x1, #-8]
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_16:
	mov	x8, x0
	ldr	w9, [x8, #16]!
	ldp	w13, w14, [x0, #8]
	ldp	w10, w11, [x0]
	cmp	w14, w11
	cset	w12, lo
	cmp	w13, w10
	cset	w15, lo
	csel	w15, w12, w15, eq
	ldr	w12, [x0, #20]
	cmp	w12, w14
	cset	w14, lo
	cmp	w9, w13
	cset	w13, lo
	csel	w13, w14, w13, eq
	tbz	w15, #0, LBB23_23
; %bb.17:
	ldr	x10, [x0]
	cbz	w13, LBB23_28
; %bb.18:
	ldr	x9, [x0, #16]
	str	x9, [x0]
	b	LBB23_30
LBB23_19:
	sub	x3, x1, #8
	add	x1, x0, #8
	add	x2, x0, #16
	bl	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_20:
	cbz	w9, LBB23_40
; %bb.21:
	ldr	x8, [x0, #8]
	ldur	x9, [x1, #-8]
	str	x9, [x0, #8]
	stur	x8, [x1, #-8]
	ldp	w8, w9, [x0, #8]
	ldp	w10, w11, [x0]
	cmp	w9, w11
	cset	w9, lo
	cmp	w8, w10
	cset	w8, lo
	csel	w8, w9, w8, eq
	cmp	w8, #1
	b.ne	LBB23_40
; %bb.22:
	ldr	q0, [x0]
	ext.16b	v0, v0, v0, #8
	str	q0, [x0]
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_23:
	cbz	w13, LBB23_31
; %bb.24:
	ldp	x12, x9, [x0, #8]
	stp	x9, x12, [x0, #8]
	lsr	x12, x9, #32
	cmp	w11, w12
	cset	w11, hi
	cmp	w10, w9
	cset	w10, hi
	csel	w10, w11, w10, eq
	cmp	w10, #1
	b.ne	LBB23_31
; %bb.25:
	ldr	x10, [x0]
	stp	x9, x10, [x0]
	b	LBB23_31
LBB23_26:
	ldr	x9, [x0, #8]
	stp	x9, x8, [x0]
	ldp	w9, w10, [x1, #-8]
	lsr	x11, x8, #32
	cmp	w10, w11
	cset	w10, lo
	cmp	w9, w8
	cset	w9, lo
	csel	w9, w10, w9, eq
	cmp	w9, #1
	b.ne	LBB23_40
; %bb.27:
	ldur	x9, [x1, #-8]
	str	x9, [x0, #8]
	stur	x8, [x1, #-8]
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_28:
	ldr	x11, [x0, #8]
	stp	x11, x10, [x0]
	lsr	x11, x10, #32
	cmp	w12, w11
	cset	w11, lo
	cmp	w9, w10
	cset	w9, lo
	csel	w9, w11, w9, eq
	cmp	w9, #1
	b.ne	LBB23_31
; %bb.29:
	ldr	x9, [x0, #16]
	str	x9, [x0, #8]
LBB23_30:
	str	x10, [x0, #16]
LBB23_31:
	add	x9, x0, #24
	cmp	x9, x1
	b.eq	LBB23_40
; %bb.32:
	mov	x10, #0                         ; =0x0
	mov	w11, #0                         ; =0x0
	b	LBB23_34
LBB23_33:                               ;   in Loop: Header=BB23_34 Depth=1
	str	x8, [x0]
	add	w11, w11, #1
	cmp	w11, #8
	b.ne	LBB23_39
	b	LBB23_41
LBB23_34:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB23_36 Depth 2
	ldp	w12, w13, [x9]
	ldp	w14, w8, [x8]
	cmp	w13, w8
	cset	w8, lo
	cmp	w12, w14
	cset	w12, lo
	csel	w8, w8, w12, eq
	cmp	w8, #1
	b.ne	LBB23_39
; %bb.35:                               ;   in Loop: Header=BB23_34 Depth=1
	ldr	x8, [x9]
	lsr	x12, x8, #32
	mov	x13, x10
LBB23_36:                               ;   Parent Loop BB23_34 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x14, x0, x13
	ldr	x15, [x14, #16]
	str	x15, [x14, #24]
	cmn	x13, #16
	b.eq	LBB23_33
; %bb.37:                               ;   in Loop: Header=BB23_36 Depth=2
	ldp	w15, w14, [x14, #8]
	cmp	w14, w12
	cset	w14, hi
	cmp	w15, w8
	cset	w15, hi
	csel	w14, w14, w15, eq
	sub	x13, x13, #8
	tbnz	w14, #0, LBB23_36
; %bb.38:                               ;   in Loop: Header=BB23_34 Depth=1
	add	x12, x0, x13
	str	x8, [x12, #24]!
	add	w11, w11, #1
	cmp	w11, #8
	b.eq	LBB23_41
LBB23_39:                               ;   in Loop: Header=BB23_34 Depth=1
	mov	x8, x9
	add	x9, x9, #8
	add	x10, x10, #8
	cmp	x9, x1
	b.ne	LBB23_34
LBB23_40:
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB23_41:
	add	x8, x9, #8
	cmp	x8, x1
	cset	w0, eq
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_ ; -- Begin function _ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.globl	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.weak_def_can_be_hidden	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.p2align	2
__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_: ; @_ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.cfi_startproc
; %bb.0:
	cmp	x0, x1
	b.eq	LBB24_35
; %bb.1:
	sub	x10, x1, x0
	asr	x8, x10, #3
	subs	x9, x8, #2
	b.lt	LBB24_14
; %bb.2:
	lsr	x9, x9, #1
	mov	x11, x9
	b	LBB24_5
LBB24_3:                                ;   in Loop: Header=BB24_5 Depth=1
	str	x13, [x0, x15, lsl #3]
LBB24_4:                                ;   in Loop: Header=BB24_5 Depth=1
	cmp	x11, #0
	sub	x11, x11, #1
	b.le	LBB24_14
LBB24_5:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB24_11 Depth 2
	cmp	x9, x11
	b.lo	LBB24_4
; %bb.6:                                ;   in Loop: Header=BB24_5 Depth=1
	lsl	x13, x11, #1
	mov	w12, #1                         ; =0x1
	bfi	x12, x11, #1, #62
	add	x13, x13, #2
	cmp	x13, x8
	b.hs	LBB24_8
; %bb.7:                                ;   in Loop: Header=BB24_5 Depth=1
	add	x14, x0, x12, lsl #3
	add	x15, x0, x13, lsl #3
	ldp	w16, w14, [x14]
	ldp	w17, w15, [x15]
	cmp	w14, w15
	cset	w14, lo
	cmp	w16, w17
	cset	w15, lo
	csel	w14, w14, w15, eq
	cmp	w14, #0
	csel	x12, x13, x12, ne
LBB24_8:                                ;   in Loop: Header=BB24_5 Depth=1
	add	x14, x0, x12, lsl #3
	add	x13, x0, x11, lsl #3
	ldp	w15, w14, [x14]
	ldp	w16, w17, [x13]
	cmp	w14, w17
	cset	w14, lo
	cmp	w15, w16
	cset	w15, lo
	csel	w14, w14, w15, eq
	tbnz	w14, #0, LBB24_4
; %bb.9:                                ;   in Loop: Header=BB24_5 Depth=1
	ldr	x13, [x13]
	lsr	x14, x13, #32
	mov	x16, x11
	b	LBB24_11
LBB24_10:                               ;   in Loop: Header=BB24_11 Depth=2
	add	x16, x0, x12, lsl #3
	ldp	w17, w16, [x16]
	cmp	w16, w14
	cset	w16, lo
	cmp	w17, w13
	cset	w17, lo
	csel	w17, w16, w17, eq
	mov	x16, x15
	cmp	w17, #1
	b.eq	LBB24_3
LBB24_11:                               ;   Parent Loop BB24_5 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x15, x12
	ldr	x12, [x0, x12, lsl #3]
	str	x12, [x0, x16, lsl #3]
	cmp	x9, x15
	b.lt	LBB24_3
; %bb.12:                               ;   in Loop: Header=BB24_11 Depth=2
	lsl	x16, x15, #1
	mov	w12, #1                         ; =0x1
	bfi	x12, x15, #1, #62
	add	x16, x16, #2
	cmp	x16, x8
	b.ge	LBB24_10
; %bb.13:                               ;   in Loop: Header=BB24_11 Depth=2
	add	x17, x0, x12, lsl #3
	add	x3, x0, x16, lsl #3
	ldp	w4, w17, [x17]
	ldp	w5, w3, [x3]
	cmp	w17, w3
	cset	w17, lo
	cmp	w4, w5
	cset	w3, lo
	csel	w17, w17, w3, eq
	cmp	w17, #0
	csel	x12, x16, x12, ne
	b	LBB24_10
LBB24_14:
	mov	x9, x1
	cmp	x1, x2
	b.eq	LBB24_33
; %bb.15:
	subs	x9, x8, #2
	b.ge	LBB24_20
; %bb.16:
	ldp	w10, w11, [x0]
	mov	x9, x1
	b	LBB24_18
LBB24_17:                               ;   in Loop: Header=BB24_18 Depth=1
	add	x9, x9, #8
	cmp	x9, x2
	b.eq	LBB24_33
LBB24_18:                               ; =>This Inner Loop Header: Depth=1
	ldp	w12, w13, [x9]
	cmp	w13, w11
	cset	w13, lo
	cmp	w12, w10
	cset	w12, lo
	csel	w12, w13, w12, eq
	cmp	w12, #1
	b.ne	LBB24_17
; %bb.19:                               ;   in Loop: Header=BB24_18 Depth=1
	ldr	x10, [x9]
	ldr	x11, [x0]
	str	x11, [x9]
	str	x10, [x0]
	lsr	x11, x10, #32
	b	LBB24_17
LBB24_20:
	lsr	x11, x9, #1
	mov	w12, #1                         ; =0x1
	mov	x9, x1
	b	LBB24_23
LBB24_21:                               ;   in Loop: Header=BB24_23 Depth=1
	str	x13, [x0, x16, lsl #3]
LBB24_22:                               ;   in Loop: Header=BB24_23 Depth=1
	add	x9, x9, #8
	cmp	x9, x2
	b.eq	LBB24_33
LBB24_23:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB24_30 Depth 2
	ldp	w13, w14, [x9]
	ldp	w15, w16, [x0]
	cmp	w14, w16
	cset	w14, lo
	cmp	w13, w15
	cset	w13, lo
	csel	w13, w14, w13, eq
	cmp	w13, #1
	b.ne	LBB24_22
; %bb.24:                               ;   in Loop: Header=BB24_23 Depth=1
	ldr	x13, [x9]
	ldr	x14, [x0]
	str	x14, [x9]
	str	x13, [x0]
	cmp	x10, #16
	b.ne	LBB24_26
; %bb.25:                               ;   in Loop: Header=BB24_23 Depth=1
	mov	w15, #1                         ; =0x1
	b	LBB24_27
LBB24_26:                               ;   in Loop: Header=BB24_23 Depth=1
	ldp	w14, w15, [x0, #8]
	ldp	w16, w17, [x0, #16]
	cmp	w15, w17
	cset	w15, lo
	cmp	w14, w16
	cset	w14, lo
	csel	w14, w15, w14, eq
	cmp	w14, #0
	cinc	x15, x12, ne
LBB24_27:                               ;   in Loop: Header=BB24_23 Depth=1
	lsr	x14, x13, #32
	add	x16, x0, x15, lsl #3
	ldp	w17, w16, [x16]
	cmp	w16, w14
	cset	w16, lo
	cmp	w17, w13
	cset	w17, lo
	csel	w16, w16, w17, eq
	tbnz	w16, #0, LBB24_22
; %bb.28:                               ;   in Loop: Header=BB24_23 Depth=1
	mov	x17, #0                         ; =0x0
	b	LBB24_30
LBB24_29:                               ;   in Loop: Header=BB24_30 Depth=2
	add	x17, x0, x15, lsl #3
	ldp	w3, w17, [x17]
	cmp	w17, w14
	cset	w17, lo
	cmp	w3, w13
	cset	w3, lo
	csel	w3, w17, w3, eq
	mov	x17, x16
	cmp	w3, #1
	b.eq	LBB24_21
LBB24_30:                               ;   Parent Loop BB24_23 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x16, x15
	ldr	x15, [x0, x15, lsl #3]
	str	x15, [x0, x17, lsl #3]
	cmp	x11, x16
	b.lt	LBB24_21
; %bb.31:                               ;   in Loop: Header=BB24_30 Depth=2
	lsl	x17, x16, #1
	mov	w15, #1                         ; =0x1
	bfi	x15, x16, #1, #63
	add	x17, x17, #2
	cmp	x17, x8
	b.ge	LBB24_29
; %bb.32:                               ;   in Loop: Header=BB24_30 Depth=2
	add	x3, x0, x15, lsl #3
	add	x4, x0, x17, lsl #3
	ldp	w5, w3, [x3]
	ldp	w6, w4, [x4]
	cmp	w3, w4
	cset	w3, lo
	cmp	w5, w6
	cset	w4, lo
	csel	w3, w3, w4, eq
	cmp	w3, #0
	csel	x15, x17, x15, ne
	b	LBB24_29
LBB24_33:
	cmp	x8, #2
	b.ge	LBB24_38
LBB24_34:
	mov	x2, x9
LBB24_35:
	mov	x0, x2
	ret
LBB24_36:                               ;   in Loop: Header=BB24_38 Depth=1
	str	x11, [x10]
LBB24_37:                               ;   in Loop: Header=BB24_38 Depth=1
	cmp	x8, #2
	sub	x8, x8, #1
	b.le	LBB24_34
LBB24_38:                               ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB24_40 Depth 2
                                        ;     Child Loop BB24_46 Depth 2
	mov	x14, #0                         ; =0x0
	ldr	x11, [x0]
	sub	x10, x8, #2
	lsr	x12, x10, #1
	mov	x13, x0
	b	LBB24_40
LBB24_39:                               ;   in Loop: Header=BB24_40 Depth=2
	ldr	w17, [x16, #16]!
	ldp	w2, w3, [x16, #-8]
	ldr	w4, [x16, #4]
	cmp	w3, w4
	cset	w3, lo
	cmp	w2, w17
	cset	w17, lo
	csel	w17, w3, w17, eq
	cmp	w17, #0
	csel	x10, x16, x10, ne
	csel	x14, x14, x15, ne
	ldr	x15, [x10]
	str	x15, [x13]
	mov	x13, x10
	cmp	x14, x12
	b.gt	LBB24_42
LBB24_40:                               ;   Parent Loop BB24_38 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x16, x13, x14, lsl #3
	add	x10, x16, #8
	lsl	x17, x14, #1
	mov	w15, #1                         ; =0x1
	bfi	x15, x14, #1, #63
	add	x14, x17, #2
	cmp	x14, x8
	b.lt	LBB24_39
; %bb.41:                               ;   in Loop: Header=BB24_40 Depth=2
	mov	x14, x15
	ldr	x15, [x10]
	str	x15, [x13]
	mov	x13, x10
	cmp	x14, x12
	b.le	LBB24_40
LBB24_42:                               ;   in Loop: Header=BB24_38 Depth=1
	sub	x1, x1, #8
	cmp	x10, x1
	b.eq	LBB24_36
; %bb.43:                               ;   in Loop: Header=BB24_38 Depth=1
	ldr	x12, [x1]
	str	x12, [x10]
	str	x11, [x1]
	sub	x11, x10, x0
	add	x11, x11, #8
	asr	x11, x11, #3
	subs	x11, x11, #2
	b.lt	LBB24_37
; %bb.44:                               ;   in Loop: Header=BB24_38 Depth=1
	lsr	x11, x11, #1
	add	x12, x0, x11, lsl #3
	ldp	w13, w14, [x12]
	ldp	w15, w16, [x10]
	cmp	w14, w16
	cset	w14, lo
	cmp	w13, w15
	cset	w13, lo
	csel	w13, w14, w13, eq
	cmp	w13, #1
	b.ne	LBB24_37
; %bb.45:                               ;   in Loop: Header=BB24_38 Depth=1
	ldr	x13, [x10]
	lsr	x14, x13, #32
LBB24_46:                               ;   Parent Loop BB24_38 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	mov	x15, x10
	mov	x10, x12
	ldr	x12, [x12]
	str	x12, [x15]
	cbz	x11, LBB24_48
; %bb.47:                               ;   in Loop: Header=BB24_46 Depth=2
	sub	x11, x11, #1
	lsr	x11, x11, #1
	add	x12, x0, x11, lsl #3
	ldp	w15, w16, [x12]
	cmp	w16, w14
	cset	w16, lo
	cmp	w15, w13
	cset	w15, lo
	csel	w15, w16, w15, eq
	tbnz	w15, #0, LBB24_46
LBB24_48:                               ;   in Loop: Header=BB24_38 Depth=1
	str	x13, [x10]
	b	LBB24_37
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ; @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains:
	.long	0                               ; 0x0
	.long	4                               ; 0x4
	.long	8                               ; 0x8
	.long	32                              ; 0x20
	.long	16                              ; 0x10
	.long	32                              ; 0x20
	.long	128                             ; 0x80
	.long	256                             ; 0x100
	.long	1024                            ; 0x400
	.long	4096                            ; 0x1000

	.p2align	2, 0x0                          ; @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice:
	.long	0                               ; 0x0
	.long	8                               ; 0x8
	.long	16                              ; 0x10
	.long	32                              ; 0x20
	.long	16                              ; 0x10
	.long	32                              ; 0x20
	.long	128                             ; 0x80
	.long	128                             ; 0x80
	.long	258                             ; 0x102
	.long	258                             ; 0x102

	.p2align	2, 0x0                          ; @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good:
	.long	0                               ; 0x0
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	4                               ; 0x4
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	8                               ; 0x8
	.long	32                              ; 0x20
	.long	32                              ; 0x20

	.p2align	2, 0x0                          ; @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits:
	.long	0                               ; 0x0
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	6                               ; 0x6
	.long	4                               ; 0x4
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	32                              ; 0x20
	.long	128                             ; 0x80
	.long	258                             ; 0x102

	.p2align	2, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_114length_symbolsE
__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE:
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	0                               ; 0x0
	.long	1                               ; 0x1
	.long	2                               ; 0x2
	.long	3                               ; 0x3
	.long	4                               ; 0x4
	.long	5                               ; 0x5
	.long	6                               ; 0x6
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
	.long	12                              ; 0xc
	.long	12                              ; 0xc
	.long	13                              ; 0xd
	.long	13                              ; 0xd
	.long	13                              ; 0xd
	.long	13                              ; 0xd
	.long	14                              ; 0xe
	.long	14                              ; 0xe
	.long	14                              ; 0xe
	.long	14                              ; 0xe
	.long	15                              ; 0xf
	.long	15                              ; 0xf
	.long	15                              ; 0xf
	.long	15                              ; 0xf
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	16                              ; 0x10
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	17                              ; 0x11
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	18                              ; 0x12
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	19                              ; 0x13
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	20                              ; 0x14
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	21                              ; 0x15
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	22                              ; 0x16
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	23                              ; 0x17
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	24                              ; 0x18
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	25                              ; 0x19
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	26                              ; 0x1a
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	27                              ; 0x1b
	.long	28                              ; 0x1c

	.p2align	2, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_112length_extraE
__ZN8stx_zlib12_GLOBAL__N_112length_extraE:
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

	.p2align	2, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_110code_orderE
__ZN8stx_zlib12_GLOBAL__N_110code_orderE:
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

	.p2align	1, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE
__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE:
	.ascii	"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\b\b\b\b\b\b"
	.short	12                              ; 0xc
	.short	140                             ; 0x8c
	.short	76                              ; 0x4c
	.short	204                             ; 0xcc
	.short	44                              ; 0x2c
	.short	172                             ; 0xac
	.short	108                             ; 0x6c
	.short	236                             ; 0xec
	.short	28                              ; 0x1c
	.short	156                             ; 0x9c
	.short	92                              ; 0x5c
	.short	220                             ; 0xdc
	.short	60                              ; 0x3c
	.short	188                             ; 0xbc
	.short	124                             ; 0x7c
	.short	252                             ; 0xfc
	.short	2                               ; 0x2
	.short	130                             ; 0x82
	.short	66                              ; 0x42
	.short	194                             ; 0xc2
	.short	34                              ; 0x22
	.short	162                             ; 0xa2
	.short	98                              ; 0x62
	.short	226                             ; 0xe2
	.short	18                              ; 0x12
	.short	146                             ; 0x92
	.short	82                              ; 0x52
	.short	210                             ; 0xd2
	.short	50                              ; 0x32
	.short	178                             ; 0xb2
	.short	114                             ; 0x72
	.short	242                             ; 0xf2
	.short	10                              ; 0xa
	.short	138                             ; 0x8a
	.short	74                              ; 0x4a
	.short	202                             ; 0xca
	.short	42                              ; 0x2a
	.short	170                             ; 0xaa
	.short	106                             ; 0x6a
	.short	234                             ; 0xea
	.short	26                              ; 0x1a
	.short	154                             ; 0x9a
	.short	90                              ; 0x5a
	.short	218                             ; 0xda
	.short	58                              ; 0x3a
	.short	186                             ; 0xba
	.short	122                             ; 0x7a
	.short	250                             ; 0xfa
	.short	6                               ; 0x6
	.short	134                             ; 0x86
	.short	70                              ; 0x46
	.short	198                             ; 0xc6
	.short	38                              ; 0x26
	.short	166                             ; 0xa6
	.short	102                             ; 0x66
	.short	230                             ; 0xe6
	.short	22                              ; 0x16
	.short	150                             ; 0x96
	.short	86                              ; 0x56
	.short	214                             ; 0xd6
	.short	54                              ; 0x36
	.short	182                             ; 0xb6
	.short	118                             ; 0x76
	.short	246                             ; 0xf6
	.short	14                              ; 0xe
	.short	142                             ; 0x8e
	.short	78                              ; 0x4e
	.short	206                             ; 0xce
	.short	46                              ; 0x2e
	.short	174                             ; 0xae
	.short	110                             ; 0x6e
	.short	238                             ; 0xee
	.short	30                              ; 0x1e
	.short	158                             ; 0x9e
	.short	94                              ; 0x5e
	.short	222                             ; 0xde
	.short	62                              ; 0x3e
	.short	190                             ; 0xbe
	.short	126                             ; 0x7e
	.short	254                             ; 0xfe
	.short	1                               ; 0x1
	.short	129                             ; 0x81
	.short	65                              ; 0x41
	.short	193                             ; 0xc1
	.short	33                              ; 0x21
	.short	161                             ; 0xa1
	.short	97                              ; 0x61
	.short	225                             ; 0xe1
	.short	17                              ; 0x11
	.short	145                             ; 0x91
	.short	81                              ; 0x51
	.short	209                             ; 0xd1
	.short	49                              ; 0x31
	.short	177                             ; 0xb1
	.short	113                             ; 0x71
	.short	241                             ; 0xf1
	.short	9                               ; 0x9
	.short	137                             ; 0x89
	.short	73                              ; 0x49
	.short	201                             ; 0xc9
	.short	41                              ; 0x29
	.short	169                             ; 0xa9
	.short	105                             ; 0x69
	.short	233                             ; 0xe9
	.short	25                              ; 0x19
	.short	153                             ; 0x99
	.short	89                              ; 0x59
	.short	217                             ; 0xd9
	.short	57                              ; 0x39
	.short	185                             ; 0xb9
	.short	121                             ; 0x79
	.short	249                             ; 0xf9
	.short	5                               ; 0x5
	.short	133                             ; 0x85
	.short	69                              ; 0x45
	.short	197                             ; 0xc5
	.short	37                              ; 0x25
	.short	165                             ; 0xa5
	.short	101                             ; 0x65
	.short	229                             ; 0xe5
	.short	21                              ; 0x15
	.short	149                             ; 0x95
	.short	85                              ; 0x55
	.short	213                             ; 0xd5
	.short	53                              ; 0x35
	.short	181                             ; 0xb5
	.short	117                             ; 0x75
	.short	245                             ; 0xf5
	.short	13                              ; 0xd
	.short	141                             ; 0x8d
	.short	77                              ; 0x4d
	.short	205                             ; 0xcd
	.short	45                              ; 0x2d
	.short	173                             ; 0xad
	.short	109                             ; 0x6d
	.short	237                             ; 0xed
	.short	29                              ; 0x1d
	.short	157                             ; 0x9d
	.short	93                              ; 0x5d
	.short	221                             ; 0xdd
	.short	61                              ; 0x3d
	.short	189                             ; 0xbd
	.short	125                             ; 0x7d
	.short	253                             ; 0xfd
	.short	19                              ; 0x13
	.short	275                             ; 0x113
	.short	147                             ; 0x93
	.short	403                             ; 0x193
	.short	83                              ; 0x53
	.short	339                             ; 0x153
	.short	211                             ; 0xd3
	.short	467                             ; 0x1d3
	.short	51                              ; 0x33
	.short	307                             ; 0x133
	.short	179                             ; 0xb3
	.short	435                             ; 0x1b3
	.short	115                             ; 0x73
	.short	371                             ; 0x173
	.short	243                             ; 0xf3
	.short	499                             ; 0x1f3
	.short	11                              ; 0xb
	.short	267                             ; 0x10b
	.short	139                             ; 0x8b
	.short	395                             ; 0x18b
	.short	75                              ; 0x4b
	.short	331                             ; 0x14b
	.short	203                             ; 0xcb
	.short	459                             ; 0x1cb
	.short	43                              ; 0x2b
	.short	299                             ; 0x12b
	.short	171                             ; 0xab
	.short	427                             ; 0x1ab
	.short	107                             ; 0x6b
	.short	363                             ; 0x16b
	.short	235                             ; 0xeb
	.short	491                             ; 0x1eb
	.short	27                              ; 0x1b
	.short	283                             ; 0x11b
	.short	155                             ; 0x9b
	.short	411                             ; 0x19b
	.short	91                              ; 0x5b
	.short	347                             ; 0x15b
	.short	219                             ; 0xdb
	.short	475                             ; 0x1db
	.short	59                              ; 0x3b
	.short	315                             ; 0x13b
	.short	187                             ; 0xbb
	.short	443                             ; 0x1bb
	.short	123                             ; 0x7b
	.short	379                             ; 0x17b
	.short	251                             ; 0xfb
	.short	507                             ; 0x1fb
	.short	7                               ; 0x7
	.short	263                             ; 0x107
	.short	135                             ; 0x87
	.short	391                             ; 0x187
	.short	71                              ; 0x47
	.short	327                             ; 0x147
	.short	199                             ; 0xc7
	.short	455                             ; 0x1c7
	.short	39                              ; 0x27
	.short	295                             ; 0x127
	.short	167                             ; 0xa7
	.short	423                             ; 0x1a7
	.short	103                             ; 0x67
	.short	359                             ; 0x167
	.short	231                             ; 0xe7
	.short	487                             ; 0x1e7
	.short	23                              ; 0x17
	.short	279                             ; 0x117
	.short	151                             ; 0x97
	.short	407                             ; 0x197
	.short	87                              ; 0x57
	.short	343                             ; 0x157
	.short	215                             ; 0xd7
	.short	471                             ; 0x1d7
	.short	55                              ; 0x37
	.short	311                             ; 0x137
	.short	183                             ; 0xb7
	.short	439                             ; 0x1b7
	.short	119                             ; 0x77
	.short	375                             ; 0x177
	.short	247                             ; 0xf7
	.short	503                             ; 0x1f7
	.short	15                              ; 0xf
	.short	271                             ; 0x10f
	.short	143                             ; 0x8f
	.short	399                             ; 0x18f
	.short	79                              ; 0x4f
	.short	335                             ; 0x14f
	.short	207                             ; 0xcf
	.short	463                             ; 0x1cf
	.short	47                              ; 0x2f
	.short	303                             ; 0x12f
	.short	175                             ; 0xaf
	.short	431                             ; 0x1af
	.short	111                             ; 0x6f
	.short	367                             ; 0x16f
	.short	239                             ; 0xef
	.short	495                             ; 0x1ef
	.short	31                              ; 0x1f
	.short	287                             ; 0x11f
	.short	159                             ; 0x9f
	.short	415                             ; 0x19f
	.short	95                              ; 0x5f
	.short	351                             ; 0x15f
	.short	223                             ; 0xdf
	.short	479                             ; 0x1df
	.short	63                              ; 0x3f
	.short	319                             ; 0x13f
	.short	191                             ; 0xbf
	.short	447                             ; 0x1bf
	.short	127                             ; 0x7f
	.short	383                             ; 0x17f
	.short	255                             ; 0xff
	.short	511                             ; 0x1ff
	.short	0                               ; 0x0
	.short	64                              ; 0x40
	.short	32                              ; 0x20
	.short	96                              ; 0x60
	.short	16                              ; 0x10
	.short	80                              ; 0x50
	.short	48                              ; 0x30
	.short	112                             ; 0x70
	.short	8                               ; 0x8
	.short	72                              ; 0x48
	.short	40                              ; 0x28
	.short	104                             ; 0x68
	.short	24                              ; 0x18
	.short	88                              ; 0x58
	.short	56                              ; 0x38
	.short	120                             ; 0x78
	.short	4                               ; 0x4
	.short	68                              ; 0x44
	.short	36                              ; 0x24
	.short	100                             ; 0x64
	.short	20                              ; 0x14
	.short	84                              ; 0x54
	.short	52                              ; 0x34
	.short	116                             ; 0x74
	.short	3                               ; 0x3
	.short	131                             ; 0x83
	.short	67                              ; 0x43
	.short	195                             ; 0xc3
	.short	35                              ; 0x23
	.short	163                             ; 0xa3
	.space	30,5
	.short	0                               ; 0x0
	.short	16                              ; 0x10
	.short	8                               ; 0x8
	.short	24                              ; 0x18
	.short	4                               ; 0x4
	.short	20                              ; 0x14
	.short	12                              ; 0xc
	.short	28                              ; 0x1c
	.short	2                               ; 0x2
	.short	18                              ; 0x12
	.short	10                              ; 0xa
	.short	26                              ; 0x1a
	.short	6                               ; 0x6
	.short	22                              ; 0x16
	.short	14                              ; 0xe
	.short	30                              ; 0x1e
	.short	1                               ; 0x1
	.short	17                              ; 0x11
	.short	9                               ; 0x9
	.short	25                              ; 0x19
	.short	5                               ; 0x5
	.short	21                              ; 0x15
	.short	13                              ; 0xd
	.short	29                              ; 0x1d
	.short	3                               ; 0x3
	.short	19                              ; 0x13
	.short	11                              ; 0xb
	.short	27                              ; 0x1b
	.short	7                               ; 0x7
	.short	23                              ; 0x17

	.p2align	2, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_111length_baseE
__ZN8stx_zlib12_GLOBAL__N_111length_baseE:
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

	.p2align	2, 0x0                          ; @_ZN8stx_zlib12_GLOBAL__N_113distance_baseE
__ZN8stx_zlib12_GLOBAL__N_113distance_baseE:
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

.subsections_via_symbols
