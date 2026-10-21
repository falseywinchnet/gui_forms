	.build_version macos, 14, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE ## -- Begin function _ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE
	.p2align	4
__ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE: ## @_ZN9gui_forms5image19inspect_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	subq	$2392, %rsp                     ## imm = 0x958
	.cfi_offset %rbx, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdx, %r14
	movq	%rsi, %r15
	movq	%rdi, %rbx
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -2416(%rbp)
	movb	$0, -2400(%rbp)
	movups	%xmm0, -2392(%rbp)
	movups	%xmm0, -2376(%rbp)
	movq	$0, -2360(%rbp)
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	movq	%rax, -2352(%rbp)
	movups	%xmm0, -2344(%rbp)
	movq	$0, -2328(%rbp)
	movq	%rax, -2320(%rbp)
	movups	%xmm0, -2312(%rbp)
	movq	$0, -2296(%rbp)
	movq	%rax, -2288(%rbp)
	movups	%xmm0, -2280(%rbp)
	movq	$0, -2264(%rbp)
	movq	%rax, -2256(%rbp)
	movups	%xmm0, -2248(%rbp)
	movq	$0, -2232(%rbp)
	movq	%rax, -2224(%rbp)
	movups	%xmm0, -2216(%rbp)
	movq	$0, -2200(%rbp)
	movq	%rax, -2192(%rbp)
	movups	%xmm0, -2184(%rbp)
	movq	$0, -2168(%rbp)
	movq	%rax, -2160(%rbp)
	leaq	-2152(%rbp), %rdi
	movl	$2093, %esi                     ## imm = 0x82D
	callq	___bzero
	xorps	%xmm0, %xmm0
	movups	%xmm0, -56(%rbp)
	movq	$0, -40(%rbp)
	movups	%xmm0, 9(%rbx)
	movups	%xmm0, (%rbx)
	movups	%xmm0, 32(%rbx)
	movups	%xmm0, 48(%rbx)
	movups	%xmm0, 64(%rbx)
	leaq	16(%rbp), %rdx
	leaq	-2416(%rbp), %rcx
	movq	%r15, %rdi
	movq	%r14, %rsi
	callq	__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	movl	%eax, (%rbx)
	testl	%eax, %eax
	jne	LBB0_2
## %bb.1:
	movq	-2368(%rbp), %rax
	movq	%rax, 56(%rbx)
	movaps	-2416(%rbp), %xmm0
	movaps	-2400(%rbp), %xmm1
	movaps	-2384(%rbp), %xmm2
	movups	%xmm2, 40(%rbx)
	movups	%xmm1, 24(%rbx)
	movups	%xmm0, 8(%rbx)
LBB0_2:
	movq	%rbx, %rax
	addq	$2392, %rsp                     ## imm = 0x958
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE: ## @_ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	.cfi_startproc
## %bb.0:
	movq	(%rdx), %r8
	movl	$1, %eax
	cmpq	$-1, %r8
	je	LBB1_19
## %bb.1:
	cmpq	$-1, 8(%rdx)
	je	LBB1_19
## %bb.2:
	cmpq	$-1, 16(%rdx)
	je	LBB1_19
## %bb.3:
	cmpq	$-1, 24(%rdx)
	je	LBB1_19
## %bb.4:
	cmpq	$-1, 32(%rdx)
	je	LBB1_19
## %bb.5:
	cmpl	$0, 40(%rdx)
	js	LBB1_19
## %bb.6:
	cmpq	%r8, %rsi
	jbe	LBB1_8
## %bb.7:
	movl	$2, %eax
LBB1_19:
	retq
LBB1_8:
	movl	$3, %eax
	cmpq	$8, %rsi
	jb	LBB1_19
## %bb.9:
	cmpb	$-119, (%rdi)
	jne	LBB1_19
## %bb.10:
	cmpb	$80, 1(%rdi)
	jne	LBB1_19
## %bb.11:
	cmpb	$78, 2(%rdi)
	jne	LBB1_19
## %bb.12:
	cmpb	$71, 3(%rdi)
	jne	LBB1_19
## %bb.13:
	cmpb	$13, 4(%rdi)
	jne	LBB1_19
## %bb.14:
	cmpb	$10, 5(%rdi)
	jne	LBB1_19
## %bb.15:
	cmpb	$26, 6(%rdi)
	jne	LBB1_19
## %bb.16:
	cmpb	$10, 7(%rdi)
	jne	LBB1_19
## %bb.17:
	cmpq	$8, %rsi
	jne	LBB1_20
## %bb.18:
	movl	$13, %eax
	retq
LBB1_20:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$232, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	298(%rcx), %rax
	movq	%rax, -184(%rbp)                ## 8-byte Spill
	leaq	292(%rcx), %rax
	movq	%rax, -216(%rbp)                ## 8-byte Spill
	leaq	8(%rdi), %rax
	movq	%rax, -240(%rbp)                ## 8-byte Spill
	leaq	11(%rdi), %rax
	movq	%rax, -224(%rbp)                ## 8-byte Spill
	leaq	13(%rdi), %rax
	movq	%rax, -232(%rbp)                ## 8-byte Spill
	movl	$8, %r14d
	movq	$0, -144(%rbp)                  ## 8-byte Folded Spill
	movq	$0, -160(%rbp)                  ## 8-byte Folded Spill
	movq	$0, -120(%rbp)                  ## 8-byte Folded Spill
	movq	$0, -176(%rbp)                  ## 8-byte Folded Spill
	movl	$0, -108(%rbp)                  ## 4-byte Folded Spill
	movq	$0, -192(%rbp)                  ## 8-byte Folded Spill
	xorl	%r9d, %r9d
	movq	%rsi, -88(%rbp)                 ## 8-byte Spill
	movq	%rdx, -72(%rbp)                 ## 8-byte Spill
	movq	%rdi, -80(%rbp)                 ## 8-byte Spill
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
LBB1_21:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB1_112 Depth 2
                                        ##     Child Loop BB1_62 Depth 2
                                        ##     Child Loop BB1_65 Depth 2
                                        ##     Child Loop BB1_107 Depth 2
	cmpl	44(%rdx), %r9d
	je	LBB1_114
## %bb.22:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	%rsi, %r8
	subq	%r14, %r8
	movl	$4, %eax
	cmpq	$12, %r8
	jb	LBB1_141
## %bb.23:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%r9d, -60(%rbp)                 ## 4-byte Spill
	movzbl	(%rdi,%r14), %r9d
	movl	%r9d, %r11d
	shll	$24, %r11d
	js	LBB1_141
## %bb.24:                              ##   in Loop: Header=BB1_21 Depth=1
	movzbl	1(%rdi,%r14), %r10d
	movl	%r10d, %r15d
	shll	$16, %r15d
	movq	%r11, -256(%rbp)                ## 8-byte Spill
	addl	%r15d, %r11d
	movzbl	2(%rdi,%r14), %r13d
	movq	%r13, -200(%rbp)                ## 8-byte Spill
                                        ## kill: def $r13d killed $r13d killed $r13 def $r13
	shll	$8, %r13d
	movzbl	3(%rdi,%r14), %ebx
	movq	%rbx, -168(%rbp)                ## 8-byte Spill
	addl	%r13d, %ebx
	movl	%ebx, -124(%rbp)                ## 4-byte Spill
	orl	%ebx, %r11d
	movl	%r11d, -44(%rbp)                ## 4-byte Spill
	movl	%r11d, %r11d
	addq	$-12, %r8
	movq	%r11, -208(%rbp)                ## 8-byte Spill
	cmpq	%r11, %r8
	jb	LBB1_141
## %bb.25:                              ##   in Loop: Header=BB1_21 Depth=1
	movzbl	4(%rdi,%r14), %eax
	movzbl	5(%rdi,%r14), %r12d
	movzbl	6(%rdi,%r14), %r11d
	movzbl	7(%rdi,%r14), %r8d
	movl	%r8d, -148(%rbp)                ## 4-byte Spill
	leaq	8(%r14), %rbx
	leaq	4(%r14), %r8
	cmpq	%rbx, %r8
	movl	%eax, -128(%rbp)                ## 4-byte Spill
	movq	%rbx, -96(%rbp)                 ## 8-byte Spill
	jae	LBB1_31
## %bb.26:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%r11d, %ebx
	movl	%eax, %r11d
	andb	$-33, %r11b
	addb	$-65, %r11b
	movl	$5, %eax
	cmpb	$25, %r11b
	ja	LBB1_141
## %bb.27:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%r12d, %r11d
	andb	$-33, %r11b
	addb	$-65, %r11b
	cmpb	$25, %r11b
	ja	LBB1_141
## %bb.28:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%ebx, %r11d
	andb	$-33, %r11b
	addb	$-65, %r11b
	cmpb	$25, %r11b
	ja	LBB1_141
## %bb.29:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	-148(%rbp), %r11d               ## 4-byte Reload
                                        ## kill: def $r11b killed $r11b killed $r11d
	andb	$-33, %r11b
	addb	$-65, %r11b
	cmpb	$25, %r11b
	ja	LBB1_141
## %bb.30:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%ebx, %r11d
	andb	$32, %r11b
	je	LBB1_32
	jmp	LBB1_141
LBB1_31:                                ##   in Loop: Header=BB1_21 Depth=1
	movl	%r11d, %ebx
	testb	$32, %r11b
	jne	LBB1_117
LBB1_32:                                ##   in Loop: Header=BB1_21 Depth=1
	movq	%r13, -248(%rbp)                ## 8-byte Spill
	shll	$24, %r9d
	shll	$16, %r10d
	orl	%r9d, %r10d
	movq	-200(%rbp), %r13                ## 8-byte Reload
                                        ## kill: def $r13d killed $r13d killed $r13 def $r13
	shll	$8, %r13d
	orl	%r10d, %r13d
	addq	-168(%rbp), %r13                ## 8-byte Folded Reload
	movq	-208(%rbp), %rax                ## 8-byte Reload
	leaq	4(%rax), %rsi
	addq	%rdi, %r8
	movq	%r8, %rdi
	callq	__ZN9gui_forms5image9png_crc32ENSt3__14spanIKSt4byteLm18446744073709551615EEE
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	leaq	(%r14,%r13), %rcx
	addq	$12, %rcx
	movq	%rcx, -104(%rbp)                ## 8-byte Spill
	leaq	(%r14,%r13), %rcx
	addq	$12, %rcx
	movl	-4(%rdi,%rcx), %ecx
	bswapl	%ecx
	cmpl	%ecx, %eax
	jne	LBB1_115
## %bb.33:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	-128(%rbp), %r8d                ## 4-byte Reload
	movl	%r8d, %eax
	shll	$24, %eax
	shll	$16, %r12d
	orl	%eax, %r12d
	shll	$8, %ebx
	addl	-148(%rbp), %r12d               ## 4-byte Folded Reload
	orl	%ebx, %r12d
	cmpl	$1229472850, %r12d              ## imm = 0x49484452
	sete	%cl
	orb	-160(%rbp), %cl                 ## 1-byte Folded Reload
	movl	$8, %eax
	testb	$1, %cl
	je	LBB1_141
## %bb.34:                              ##   in Loop: Header=BB1_21 Depth=1
	addq	%rdi, -96(%rbp)                 ## 8-byte Folded Spill
	cmpl	$1229209940, %r12d              ## imm = 0x49444154
	setne	%bl
	testb	$32, %r8b
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	je	LBB1_38
## %bb.35:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	24(%rdx), %rax
	subq	-192(%rbp), %rax                ## 8-byte Folded Reload
	cmpq	%rax, %r13
	ja	LBB1_114
## %bb.36:                              ##   in Loop: Header=BB1_21 Depth=1
	incq	48(%rcx)
	movl	%r12d, %edi
	movq	-96(%rbp), %rsi                 ## 8-byte Reload
	movq	%r13, %rdx
	leaq	-129(%rbp), %rcx
	leaq	-264(%rbp), %r8
	callq	__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	testl	%eax, %eax
	jne	LBB1_141
## %bb.37:                              ##   in Loop: Header=BB1_21 Depth=1
	addq	%r13, -192(%rbp)                ## 8-byte Folded Spill
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	$8, %eax
	movl	-128(%rbp), %r8d                ## 4-byte Reload
LBB1_38:                                ##   in Loop: Header=BB1_21 Depth=1
	andb	-120(%rbp), %bl                 ## 1-byte Folded Reload
	movq	-176(%rbp), %rsi                ## 8-byte Reload
	orb	%bl, %sil
	movq	%rsi, -176(%rbp)                ## 8-byte Spill
	cmpl	$1732332864, %r12d              ## imm = 0x67414D40
	jg	LBB1_44
## %bb.39:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1347179588, %r12d              ## imm = 0x504C5444
	jg	LBB1_47
## %bb.40:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1229209940, %r12d              ## imm = 0x49444154
	je	LBB1_85
## %bb.41:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1229472850, %r12d              ## imm = 0x49484452
	jne	LBB1_104
## %bb.42:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$13, -44(%rbp)                  ## 4-byte Folded Reload
	setne	%al
	movq	-160(%rbp), %rcx                ## 8-byte Reload
	orb	%al, %cl
	testb	$1, %cl
	jne	LBB1_124
## %bb.43:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rdi                 ## 8-byte Reload
	movq	-72(%rbp), %r15                 ## 8-byte Reload
	movq	%r15, %rsi
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	callq	__ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	movq	%r15, %rdx
	movb	$1, %sil
	movq	%rsi, -160(%rbp)                ## 8-byte Spill
	testl	%eax, %eax
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	-60(%rbp), %r9d                 ## 4-byte Reload
	movq	-104(%rbp), %r14                ## 8-byte Reload
	je	LBB1_96
	jmp	LBB1_141
LBB1_44:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1934772033, %r12d              ## imm = 0x73524741
	jg	LBB1_49
## %bb.45:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1732332865, %r12d              ## imm = 0x67414D41
	je	LBB1_66
## %bb.46:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1766015824, %r12d              ## imm = 0x69434350
	je	LBB1_66
	jmp	LBB1_105
LBB1_47:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1347179589, %r12d              ## imm = 0x504C5445
	je	LBB1_75
## %bb.48:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1665684045, %r12d              ## imm = 0x6348524D
	je	LBB1_66
	jmp	LBB1_105
LBB1_49:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1934772034, %r12d              ## imm = 0x73524742
	je	LBB1_66
## %bb.50:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1951551059, %r12d              ## imm = 0x74524E53
	jne	LBB1_105
## %bb.51:                              ##   in Loop: Header=BB1_21 Depth=1
	movzbl	2356(%rcx), %eax
	orb	-120(%rbp), %al                 ## 1-byte Folded Reload
	jne	LBB1_126
## %bb.52:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	movl	12(%rax), %edx
	cmpl	$3, %edx
	jne	LBB1_97
## %bb.53:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$0, -44(%rbp)                   ## 4-byte Folded Reload
	je	LBB1_126
## %bb.54:                              ##   in Loop: Header=BB1_21 Depth=1
	testb	$1, -144(%rbp)                  ## 1-byte Folded Reload
	je	LBB1_126
## %bb.55:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	movl	-44(%rbp), %ecx                 ## 4-byte Reload
	cmpl	2340(%rax), %ecx
	movl	$10, %eax
	movq	-248(%rbp), %rcx                ## 8-byte Reload
	ja	LBB1_141
## %bb.56:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-256(%rbp), %rax                ## 8-byte Reload
	orl	%r15d, %eax
	orl	-168(%rbp), %ecx                ## 4-byte Folded Reload
	orl	%eax, %ecx
	movq	%rcx, %r13
	cmpl	$8, %ecx
	jb	LBB1_59
## %bb.57:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-240(%rbp), %rax                ## 8-byte Reload
	addq	%r14, %rax
	addq	%r13, %rax
	cmpq	%rax, -184(%rbp)                ## 8-byte Folded Reload
	jae	LBB1_111
## %bb.58:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-216(%rbp), %rax                ## 8-byte Reload
	leaq	(%rax,%r13,8), %rax
	cmpq	%rax, -96(%rbp)                 ## 8-byte Folded Reload
	jae	LBB1_111
LBB1_59:                                ##   in Loop: Header=BB1_21 Depth=1
	xorl	%eax, %eax
	movq	%r13, %r8
LBB1_60:                                ##   in Loop: Header=BB1_21 Depth=1
	movq	%r8, %rcx
	movq	%rax, %r8
	andq	$3, %rcx
	movq	-184(%rbp), %rdi                ## 8-byte Reload
	movq	-96(%rbp), %r9                  ## 8-byte Reload
	je	LBB1_63
## %bb.61:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	%rax, %r8
LBB1_62:                                ##   Parent Loop BB1_21 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movzbl	(%r9,%r8), %edx
	movl	%edx, %esi
	shll	$8, %esi
	orl	%edx, %esi
	movw	%si, (%rdi,%r8,8)
	incq	%r8
	decq	%rcx
	jne	LBB1_62
LBB1_63:                                ##   in Loop: Header=BB1_21 Depth=1
	subq	%r13, %rax
	cmpq	$-4, %rax
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	ja	LBB1_103
## %bb.64:                              ##   in Loop: Header=BB1_21 Depth=1
	addq	-224(%rbp), %r14                ## 8-byte Folded Reload
LBB1_65:                                ##   Parent Loop BB1_21 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movzbl	-3(%r14,%r8), %eax
	movl	%eax, %r9d
	shll	$8, %r9d
	orl	%eax, %r9d
	movw	%r9w, 298(%rcx,%r8,8)
	movzbl	-2(%r14,%r8), %eax
	movl	%eax, %r9d
	shll	$8, %r9d
	orl	%eax, %r9d
	movw	%r9w, 306(%rcx,%r8,8)
	movzbl	-1(%r14,%r8), %eax
	movl	%eax, %r9d
	shll	$8, %r9d
	orl	%eax, %r9d
	movw	%r9w, 314(%rcx,%r8,8)
	movzbl	(%r14,%r8), %eax
	movl	%eax, %r9d
	shll	$8, %r9d
	orl	%eax, %r9d
	movw	%r9w, 322(%rcx,%r8,8)
	addq	$4, %r8
	cmpq	%r8, %r13
	jne	LBB1_65
	jmp	LBB1_103
LBB1_66:                                ##   in Loop: Header=BB1_21 Depth=1
	xorl	%r8d, %r8d
	cmpl	$1732332865, %r12d              ## imm = 0x67414D41
	sete	%r8b
	incl	%r8d
	cmpl	$1665684045, %r12d              ## imm = 0x6348524D
	movl	$4, %eax
	cmovel	%eax, %r8d
	cmpl	$1766015824, %r12d              ## imm = 0x69434350
	movl	$8, %eax
	cmovel	%eax, %r8d
	movq	-144(%rbp), %rax                ## 8-byte Reload
                                        ## kill: def $al killed $al killed $rax
	orb	-120(%rbp), %al                 ## 1-byte Folded Reload
	movl	$11, %eax
	jne	LBB1_141
## %bb.67:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	%r8d, %ecx
	andl	-108(%rbp), %ecx                ## 4-byte Folded Reload
	jne	LBB1_141
## %bb.68:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1934772034, %r12d              ## imm = 0x73524742
	jne	LBB1_72
## %bb.69:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1, -44(%rbp)                   ## 4-byte Folded Reload
	jne	LBB1_141
## %bb.70:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	cmpb	$3, (%rax)
	ja	LBB1_130
## %bb.71:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	-108(%rbp), %eax                ## 4-byte Reload
	andl	$8, %eax
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	$11, %eax
	je	LBB1_94
	jmp	LBB1_141
LBB1_72:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1732332865, %r12d              ## imm = 0x67414D41
	jne	LBB1_90
## %bb.73:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$4, -44(%rbp)                   ## 4-byte Folded Reload
	jne	LBB1_141
## %bb.74:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	cmpl	$0, (%rax)
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	$11, %eax
	jne	LBB1_94
	jmp	LBB1_141
LBB1_75:                                ##   in Loop: Header=BB1_21 Depth=1
	movq	-144(%rbp), %rax                ## 8-byte Reload
	orb	-120(%rbp), %al                 ## 1-byte Folded Reload
	jne	LBB1_125
## %bb.76:                              ##   in Loop: Header=BB1_21 Depth=1
	imull	$-1431655765, -44(%rbp), %eax   ## 4-byte Folded Reload
                                        ## imm = 0xAAAAAAAB
	cmpl	$1431655765, %eax               ## imm = 0x55555555
	ja	LBB1_125
## %bb.77:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	-44(%rbp), %eax                 ## 4-byte Reload
	addl	$-769, %eax                     ## imm = 0xFCFF
	cmpl	$-768, %eax                     ## imm = 0xFD00
	jb	LBB1_125
## %bb.78:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	cmpb	$0, 2356(%rax)
	jne	LBB1_125
## %bb.79:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	movl	12(%rax), %eax
	testl	$-5, %eax
	je	LBB1_125
## %bb.80:                              ##   in Loop: Header=BB1_21 Depth=1
	imull	$43691, -124(%rbp), %edx        ## 4-byte Folded Reload
                                        ## imm = 0xAAAB
	shrl	$17, %edx
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movl	%edx, 2340(%rcx)
	cmpl	$3, %eax
	movl	$9, %eax
	jne	LBB1_82
## %bb.81:                              ##   in Loop: Header=BB1_21 Depth=1
	movzbl	8(%rcx), %ecx
	movl	$1, %esi
	shll	%cl, %esi
	cmpl	%edx, %esi
	jb	LBB1_141
LBB1_82:                                ##   in Loop: Header=BB1_21 Depth=1
	movb	$1, %al
	movq	%rax, -144(%rbp)                ## 8-byte Spill
	cmpl	$3, -124(%rbp)                  ## 4-byte Folded Reload
	jb	LBB1_110
## %bb.83:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpw	$1, %dx
	adcl	$0, %edx
	cmpl	$6, -124(%rbp)                  ## 4-byte Folded Reload
	jae	LBB1_106
## %bb.84:                              ##   in Loop: Header=BB1_21 Depth=1
	xorl	%eax, %eax
	jmp	LBB1_109
LBB1_85:                                ##   in Loop: Header=BB1_21 Depth=1
	testb	$1, -176(%rbp)                  ## 1-byte Folded Reload
	jne	LBB1_141
## %bb.86:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$3, 12(%rcx)
	setne	%al
	orb	-144(%rbp), %al                 ## 1-byte Folded Reload
	testb	$1, %al
	je	LBB1_125
## %bb.87:                              ##   in Loop: Header=BB1_21 Depth=1
	testb	$1, -120(%rbp)                  ## 1-byte Folded Reload
	jne	LBB1_89
## %bb.88:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	movq	%r14, 2360(%rax)
LBB1_89:                                ##   in Loop: Header=BB1_21 Depth=1
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-104(%rbp), %r14                ## 8-byte Reload
	movq	%r14, 2368(%rcx)
	movq	-208(%rbp), %rax                ## 8-byte Reload
	addq	%rax, 2376(%rcx)
	movb	$1, %al
	movq	%rax, -120(%rbp)                ## 8-byte Spill
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	-60(%rbp), %r9d                 ## 4-byte Reload
	jmp	LBB1_96
LBB1_90:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$32, -44(%rbp)                  ## 4-byte Folded Reload
	je	LBB1_92
## %bb.91:                              ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1665684045, %r12d              ## imm = 0x6348524D
	je	LBB1_141
LBB1_92:                                ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1766015824, %r12d              ## imm = 0x69434350
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	jne	LBB1_94
## %bb.93:                              ##   in Loop: Header=BB1_21 Depth=1
	movl	-108(%rbp), %r9d                ## 4-byte Reload
	andl	$1, %r9d
	jne	LBB1_141
LBB1_94:                                ##   in Loop: Header=BB1_21 Depth=1
	orl	%r8d, -108(%rbp)                ## 4-byte Folded Spill
LBB1_95:                                ##   in Loop: Header=BB1_21 Depth=1
	movl	-60(%rbp), %r9d                 ## 4-byte Reload
	movq	-104(%rbp), %r14                ## 8-byte Reload
LBB1_96:                                ##   in Loop: Header=BB1_21 Depth=1
	incl	%r9d
	cmpq	%rsi, %r14
	jb	LBB1_21
	jmp	LBB1_140
LBB1_97:                                ##   in Loop: Header=BB1_21 Depth=1
	xorl	%eax, %eax
	testl	%edx, %edx
	setne	%cl
	testl	$-3, %edx
	jne	LBB1_126
## %bb.98:                              ##   in Loop: Header=BB1_21 Depth=1
	movb	%cl, %al
	leal	2(,%rax,4), %eax
	cmpl	%eax, -44(%rbp)                 ## 4-byte Folded Reload
	jne	LBB1_126
## %bb.99:                              ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	movzwl	(%rax), %ecx
	rolw	$8, %cx
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
	movl	8(%rsi), %eax
	movzwl	%cx, %edi
	movl	%edi, 2344(%rsi)
	movl	%eax, %ecx
	shrl	%cl, %edi
	testl	%edi, %edi
	jne	LBB1_126
## %bb.100:                             ##   in Loop: Header=BB1_21 Depth=1
	testl	%edx, %edx
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	je	LBB1_103
## %bb.101:                             ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rdx                 ## 8-byte Reload
	movzwl	2(%rdx), %edx
	rolw	$8, %dx
	movzwl	%dx, %edx
	movl	%edx, 2348(%rcx)
	movl	%eax, %ecx
	shrl	%cl, %edx
	testl	%edx, %edx
	jne	LBB1_126
## %bb.102:                             ##   in Loop: Header=BB1_21 Depth=1
	movq	-96(%rbp), %rcx                 ## 8-byte Reload
	movzwl	4(%rcx), %ecx
	rolw	$8, %cx
	movzwl	%cx, %edx
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
	movl	%edx, 2352(%rsi)
	movl	%eax, %ecx
	shrl	%cl, %edx
	movq	%rsi, %rcx
	testl	%edx, %edx
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	$10, %eax
	jne	LBB1_141
LBB1_103:                               ##   in Loop: Header=BB1_21 Depth=1
	movb	$1, 2356(%rcx)
	jmp	LBB1_95
LBB1_104:                               ##   in Loop: Header=BB1_21 Depth=1
	cmpl	$1229278788, %r12d              ## imm = 0x49454E44
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	movl	-128(%rbp), %r8d                ## 4-byte Reload
	je	LBB1_120
LBB1_105:                               ##   in Loop: Header=BB1_21 Depth=1
	testb	$32, %r8b
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movl	-60(%rbp), %r9d                 ## 4-byte Reload
	movq	-104(%rbp), %r14                ## 8-byte Reload
	jne	LBB1_96
	jmp	LBB1_127
LBB1_106:                               ##   in Loop: Header=BB1_21 Depth=1
	movzwl	%dx, %ecx
	addq	-232(%rbp), %r14                ## 8-byte Folded Reload
	movq	-200(%rbp), %rax                ## 8-byte Reload
	shll	$8, %eax
	addl	-168(%rbp), %eax                ## 4-byte Folded Reload
	imulq	$1431655766, %rax, %rdx         ## imm = 0x55555556
	shrq	$32, %rdx
	cmpq	$1, %rdx
	adcq	$0, %rdx
	andl	$32766, %edx                    ## imm = 0x7FFE
	xorl	%eax, %eax
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
LBB1_107:                               ##   Parent Loop BB1_21 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movzbl	-5(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 292(%rsi,%rax,8)
	movzbl	-4(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 294(%rsi,%rax,8)
	movzbl	-3(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 296(%rsi,%rax,8)
	movw	$-1, 298(%rsi,%rax,8)
	movzbl	-2(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 300(%rsi,%rax,8)
	movzbl	-1(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 302(%rsi,%rax,8)
	movzbl	(%r14), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, 304(%rsi,%rax,8)
	movw	$-1, 306(%rsi,%rax,8)
	addq	$2, %rax
	addq	$6, %r14
	cmpq	%rax, %rdx
	jne	LBB1_107
## %bb.108:                             ##   in Loop: Header=BB1_21 Depth=1
	testb	$1, %cl
	je	LBB1_110
LBB1_109:                               ##   in Loop: Header=BB1_21 Depth=1
	leaq	(%rax,%rax,2), %rdx
	movq	-96(%rbp), %r8                  ## 8-byte Reload
	movzbl	(%r8,%rdx), %ecx
	movl	%ecx, %esi
	shll	$8, %esi
	orl	%ecx, %esi
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movw	%si, 292(%rcx,%rax,8)
	movzbl	1(%r8,%rdx), %esi
	movl	%esi, %edi
	shll	$8, %edi
	orl	%esi, %edi
	movw	%di, 294(%rcx,%rax,8)
	movzbl	2(%r8,%rdx), %edx
	movl	%edx, %esi
	shll	$8, %esi
	orl	%edx, %esi
	movw	%si, 296(%rcx,%rax,8)
	movw	$-1, 298(%rcx,%rax,8)
LBB1_110:                               ##   in Loop: Header=BB1_21 Depth=1
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	jmp	LBB1_95
LBB1_111:                               ##   in Loop: Header=BB1_21 Depth=1
	movl	%r13d, %eax
	andl	$2147483640, %eax               ## imm = 0x7FFFFFF8
	xorl	%edx, %edx
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	-96(%rbp), %r12                 ## 8-byte Reload
LBB1_112:                               ##   Parent Loop BB1_21 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	(%r12,%rdx), %xmm0              ## xmm0 = mem[0],zero
	punpcklbw	%xmm0, %xmm0            ## xmm0 = xmm0[0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7]
	movd	%xmm0, %esi
	pextrw	$1, %xmm0, %edi
	pextrw	$2, %xmm0, %r8d
	pextrw	$3, %xmm0, %r9d
	pextrw	$4, %xmm0, %r10d
	pextrw	$5, %xmm0, %r11d
	pextrw	$6, %xmm0, %ebx
	pextrw	$7, %xmm0, %r15d
	movw	%si, 298(%rcx,%rdx,8)
	movw	%di, 306(%rcx,%rdx,8)
	movw	%r8w, 314(%rcx,%rdx,8)
	movw	%r9w, 322(%rcx,%rdx,8)
	movw	%r10w, 330(%rcx,%rdx,8)
	movw	%r11w, 338(%rcx,%rdx,8)
	movw	%bx, 346(%rcx,%rdx,8)
	movw	%r15w, 354(%rcx,%rdx,8)
	addq	$8, %rdx
	cmpq	%rdx, %rax
	jne	LBB1_112
## %bb.113:                             ##   in Loop: Header=BB1_21 Depth=1
	movq	%r13, %r8
	cmpl	%r8d, %eax
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	movq	-72(%rbp), %rdx                 ## 8-byte Reload
	movq	-80(%rbp), %rdi                 ## 8-byte Reload
	jne	LBB1_60
	jmp	LBB1_103
LBB1_114:
	movl	$2, %eax
	jmp	LBB1_141
LBB1_115:
	movl	$6, %eax
	jmp	LBB1_141
LBB1_117:
	movl	$5, %eax
	jmp	LBB1_141
LBB1_125:
	movl	$9, %eax
	jmp	LBB1_141
LBB1_126:
	movl	$10, %eax
	jmp	LBB1_141
LBB1_120:
	cmpl	$0, -44(%rbp)                   ## 4-byte Folded Reload
	jne	LBB1_141
## %bb.121:
	cmpb	$1, -120(%rbp)                  ## 1-byte Folded Reload
	jne	LBB1_140
## %bb.122:
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	cmpq	$0, 2376(%rax)
	movl	$13, %eax
	movq	-88(%rbp), %rcx                 ## 8-byte Reload
	je	LBB1_141
## %bb.123:
	xorl	%edx, %edx
	cmpq	%rcx, -104(%rbp)                ## 8-byte Folded Reload
	movl	$14, %eax
	cmovel	%edx, %eax
	jmp	LBB1_141
LBB1_140:
	movl	$13, %eax
LBB1_141:
	addq	$232, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB1_124:
	movl	$7, %eax
	jmp	LBB1_141
LBB1_127:
	movl	$12, %eax
	jmp	LBB1_141
LBB1_130:
	movl	$11, %eax
	jmp	LBB1_141
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
LCPI2_0:
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
LCPI2_1:
	.space	16,127
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
	.p2align	4
__ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE: ## @_ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$2664, %rsp                     ## imm = 0xA68
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	pxor	%xmm0, %xmm0
	movdqu	%xmm0, 9(%rdi)
	movdqu	%xmm0, (%rdi)
	movdqu	%xmm0, 32(%rdi)
	movdqu	%xmm0, 48(%rdi)
	movq	%rdi, -64(%rbp)                 ## 8-byte Spill
	movdqu	%xmm0, 64(%rdi)
	movq	64(%rbp), %r14
	movq	72(%rbp), %r13
	movq	96(%rbp), %rax
	movq	104(%rbp), %r12
	movq	%r12, %r10
	shlq	$4, %r10
	leaq	(%r10,%r10,2), %r10
	movq	80(%rbp), %r11
	movq	88(%rbp), %rbx
	testq	%rdx, %rdx
	je	LBB2_20
## %bb.1:
	movq	%rsi, %r15
	subq	%rcx, %r15
	jbe	LBB2_3
## %bb.2:
	cmpq	$38192, %r15                    ## imm = 0x9530
	jae	LBB2_4
	jmp	LBB2_69
LBB2_3:
	movq	%rcx, %r15
	subq	%rsi, %r15
	cmpq	%rdx, %r15
	jb	LBB2_69
LBB2_4:
	testq	%r9, %r9
	je	LBB2_8
## %bb.5:
	movq	%rsi, %r15
	subq	%r8, %r15
	jbe	LBB2_7
## %bb.6:
	cmpq	%r9, %r15
	jae	LBB2_8
	jmp	LBB2_69
LBB2_7:
	movq	%r8, %r15
	subq	%rsi, %r15
	cmpq	%rdx, %r15
	jb	LBB2_69
LBB2_8:
	leaq	(,%r13,2), %r15
	testq	%r15, %r15
	je	LBB2_12
## %bb.9:
	movq	%rsi, %r15
	subq	%r14, %r15
	jbe	LBB2_11
## %bb.10:
	leaq	(,%r13,2), %rdi
	cmpq	%rdi, %r15
	jae	LBB2_12
	jmp	LBB2_69
LBB2_11:
	movq	%r14, %r15
	subq	%rsi, %r15
	cmpq	%rdx, %r15
	jb	LBB2_69
LBB2_12:
	testq	%rbx, %rbx
	je	LBB2_16
## %bb.13:
	movq	%rsi, %r15
	subq	%r11, %r15
	jbe	LBB2_15
## %bb.14:
	cmpq	%rbx, %r15
	jae	LBB2_16
	jmp	LBB2_69
LBB2_15:
	movq	%r11, %r15
	subq	%rsi, %r15
	cmpq	%rdx, %r15
	jb	LBB2_69
LBB2_16:
	testq	%r10, %r10
	je	LBB2_20
## %bb.17:
	movq	%rsi, %r15
	subq	%rax, %r15
	jbe	LBB2_19
## %bb.18:
	cmpq	%r10, %r15
	jae	LBB2_20
	jmp	LBB2_69
LBB2_19:
	movq	%rax, %r15
	subq	%rsi, %r15
	cmpq	%rdx, %r15
	jb	LBB2_69
LBB2_20:
	testq	%r9, %r9
	je	LBB2_24
## %bb.21:
	movq	%rcx, %r15
	subq	%r8, %r15
	jbe	LBB2_23
## %bb.22:
	cmpq	%r9, %r15
	jae	LBB2_24
	jmp	LBB2_69
LBB2_23:
	movq	%r8, %r15
	subq	%rcx, %r15
	cmpq	$38192, %r15                    ## imm = 0x9530
	jb	LBB2_69
LBB2_24:
	leaq	(,%r13,2), %rdi
	testq	%rdi, %rdi
	je	LBB2_28
## %bb.25:
	movq	%rcx, %r15
	subq	%r14, %r15
	jbe	LBB2_27
## %bb.26:
	leaq	(,%r13,2), %rdi
	cmpq	%rdi, %r15
	jae	LBB2_28
	jmp	LBB2_69
LBB2_27:
	movq	%r14, %r15
	subq	%rcx, %r15
	cmpq	$38192, %r15                    ## imm = 0x9530
	jb	LBB2_69
LBB2_28:
	testq	%rbx, %rbx
	je	LBB2_32
## %bb.29:
	movq	%rcx, %r15
	subq	%r11, %r15
	jbe	LBB2_31
## %bb.30:
	cmpq	%rbx, %r15
	jae	LBB2_32
	jmp	LBB2_69
LBB2_31:
	movq	%r11, %r15
	subq	%rcx, %r15
	cmpq	$38192, %r15                    ## imm = 0x9530
	jb	LBB2_69
LBB2_32:
	testq	%r10, %r10
	je	LBB2_36
## %bb.33:
	movq	%rcx, %r15
	subq	%rax, %r15
	jbe	LBB2_35
## %bb.34:
	cmpq	%r10, %r15
	jae	LBB2_36
	jmp	LBB2_69
LBB2_35:
	movq	%rax, %r15
	subq	%rcx, %r15
	cmpq	$38192, %r15                    ## imm = 0x9530
	jb	LBB2_69
LBB2_36:
	testq	%r9, %r9
	je	LBB2_49
## %bb.37:
	leaq	(,%r13,2), %rdi
	testq	%rdi, %rdi
	je	LBB2_41
## %bb.38:
	movq	%r8, %r15
	subq	%r14, %r15
	jbe	LBB2_40
## %bb.39:
	leaq	(,%r13,2), %rdi
	cmpq	%rdi, %r15
	jae	LBB2_41
	jmp	LBB2_69
LBB2_40:
	movq	%r14, %r15
	subq	%r8, %r15
	cmpq	%r9, %r15
	jb	LBB2_69
LBB2_41:
	testq	%rbx, %rbx
	je	LBB2_45
## %bb.42:
	movq	%r8, %r15
	subq	%r11, %r15
	jbe	LBB2_44
## %bb.43:
	cmpq	%rbx, %r15
	jae	LBB2_45
	jmp	LBB2_69
LBB2_44:
	movq	%r11, %r15
	subq	%r8, %r15
	cmpq	%r9, %r15
	jb	LBB2_69
LBB2_45:
	testq	%r10, %r10
	je	LBB2_49
## %bb.46:
	movq	%r8, %r15
	subq	%rax, %r15
	jbe	LBB2_48
## %bb.47:
	cmpq	%r10, %r15
	jae	LBB2_49
	jmp	LBB2_69
LBB2_48:
	movq	%rax, %r15
	subq	%r8, %r15
	cmpq	%r9, %r15
	jb	LBB2_69
LBB2_49:
	leaq	(,%r13,2), %rdi
	testq	%rdi, %rdi
	je	LBB2_58
## %bb.50:
	testq	%rbx, %rbx
	je	LBB2_54
## %bb.51:
	movq	%r14, %r15
	subq	%r11, %r15
	jbe	LBB2_53
## %bb.52:
	cmpq	%rbx, %r15
	jae	LBB2_54
	jmp	LBB2_69
LBB2_53:
	movq	%r11, %r15
	subq	%r14, %r15
	leaq	(,%r13,2), %rdi
	cmpq	%rdi, %r15
	jb	LBB2_69
LBB2_54:
	testq	%r10, %r10
	je	LBB2_62
## %bb.55:
	movq	%r14, %r15
	subq	%rax, %r15
	jbe	LBB2_57
## %bb.56:
	cmpq	%r10, %r15
	jae	LBB2_58
	jmp	LBB2_69
LBB2_57:
	movq	%rax, %r15
	subq	%r14, %r15
	leaq	(,%r13,2), %rdi
	cmpq	%rdi, %r15
	jb	LBB2_69
LBB2_58:
	testq	%rbx, %rbx
	je	LBB2_62
## %bb.59:
	testq	%r10, %r10
	je	LBB2_62
## %bb.60:
	movq	%r11, %r14
	subq	%rax, %r14
	jbe	LBB2_68
## %bb.61:
	cmpq	%r10, %r14
	jb	LBB2_69
LBB2_62:
	movq	%r8, -144(%rbp)                 ## 8-byte Spill
	movq	%r9, -96(%rbp)                  ## 8-byte Spill
	movq	%rcx, -104(%rbp)                ## 8-byte Spill
	leaq	16(%rbp), %rbx
	movdqa	%xmm0, -2704(%rbp)
	movb	$0, -2688(%rbp)
	movdqu	%xmm0, -2680(%rbp)
	movdqu	%xmm0, -2664(%rbp)
	movq	$0, -2648(%rbp)
	movabsq	$4294967297, %rcx               ## imm = 0x100000001
	movq	%rcx, -2640(%rbp)
	movdqu	%xmm0, -2632(%rbp)
	movq	$0, -2616(%rbp)
	movq	%rcx, -2608(%rbp)
	movdqu	%xmm0, -2600(%rbp)
	movq	$0, -2584(%rbp)
	movq	%rcx, -2576(%rbp)
	movdqu	%xmm0, -2568(%rbp)
	movq	$0, -2552(%rbp)
	movq	%rcx, -2544(%rbp)
	movdqu	%xmm0, -2536(%rbp)
	movq	$0, -2520(%rbp)
	movq	%rcx, -2512(%rbp)
	movdqu	%xmm0, -2504(%rbp)
	movq	$0, -2488(%rbp)
	movq	%rcx, -2480(%rbp)
	movdqu	%xmm0, -2472(%rbp)
	movq	$0, -2456(%rbp)
	movq	%rcx, -2448(%rbp)
	leaq	-2440(%rbp), %rdi
	movq	%rsi, %r14
	movl	$2093, %esi                     ## imm = 0x82D
	movq	%rdx, %r15
	callq	___bzero
	pxor	%xmm0, %xmm0
	movdqu	%xmm0, -344(%rbp)
	movq	$0, -328(%rbp)
	leaq	-2704(%rbp), %rcx
	movq	%r14, -112(%rbp)                ## 8-byte Spill
	movq	%r14, %rdi
	movq	%r15, -120(%rbp)                ## 8-byte Spill
	movq	%r15, %rsi
	movq	%rbx, %r15
	movq	%rbx, %rdx
	callq	__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	movl	%eax, %ecx
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.63:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	leaq	8(%rax), %rcx
	movq	-2656(%rbp), %rdx
	movq	%rdx, 48(%rcx)
	movdqa	-2704(%rbp), %xmm0
	movdqa	-2688(%rbp), %xmm1
	movdqa	-2672(%rbp), %xmm2
	movdqu	%xmm2, 32(%rcx)
	movdqu	%xmm1, 16(%rcx)
	movdqu	%xmm0, (%rcx)
	movq	-96(%rbp), %rcx                 ## 8-byte Reload
	cmpq	-2672(%rbp), %rcx
	jb	LBB2_71
## %bb.64:
	cmpq	-2680(%rbp), %r13
	jb	LBB2_71
## %bb.65:
	cmpq	-2656(%rbp), %r12
	jb	LBB2_71
## %bb.66:
	movabsq	$64424509440, %rax              ## imm = 0xF00000000
	movq	%rax, -272(%rbp)
	movq	-328(%rbp), %rax
	movq	%rax, -264(%rbp)
	movq	-2664(%rbp), %rax
	movq	%rax, -256(%rbp)
	movq	32(%r15), %rax
	movq	%rax, -248(%rbp)
	leaq	-272(%rbp), %rsi
	movq	-104(%rbp), %rdi                ## 8-byte Reload
	callq	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	movl	%eax, %ecx
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, 4(%rax)
	testl	%ecx, %ecx
	je	LBB2_72
## %bb.67:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	$16, (%rax)
	jmp	LBB2_70
LBB2_68:
	subq	%r11, %rax
	cmpq	%rbx, %rax
	jae	LBB2_62
LBB2_69:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	$1, (%rax)
LBB2_70:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	addq	$2664, %rsp                     ## imm = 0xA68
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB2_71:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	$15, (%rax)
	jmp	LBB2_70
LBB2_72:
	movq	-344(%rbp), %rcx
	movq	%rcx, -240(%rbp)
	movq	$0, -232(%rbp)
	movb	$0, -224(%rbp)
	movl	-2424(%rbp), %eax
	testl	%eax, %eax
	movq	-104(%rbp), %rcx                ## 8-byte Reload
	movq	-120(%rbp), %rsi                ## 8-byte Reload
	movq	-112(%rbp), %rdi                ## 8-byte Reload
	movq	-96(%rbp), %r8                  ## 8-byte Reload
	movq	-144(%rbp), %rdx                ## 8-byte Reload
	je	LBB2_235
## %bb.73:
	leaq	-2648(%rbp), %r11
	incq	%r8
	xorl	%ebx, %ebx
	leaq	-2704(%rbp), %r12
	movq	%r8, -96(%rbp)                  ## 8-byte Spill
	jmp	LBB2_76
LBB2_74:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2424(%rbp), %eax
	movq	-96(%rbp), %r8                  ## 8-byte Reload
	movq	-144(%rbp), %rdx                ## 8-byte Reload
LBB2_75:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	-208(%rbp), %rbx                ## 8-byte Reload
	incq	%rbx
	movl	%eax, %r9d
	cmpq	%r9, %rbx
	leaq	-2648(%rbp), %r11
	jae	LBB2_235
LBB2_76:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB2_103 Depth 2
                                        ##       Child Loop BB2_138 Depth 3
                                        ##       Child Loop BB2_142 Depth 3
                                        ##       Child Loop BB2_146 Depth 3
                                        ##       Child Loop BB2_148 Depth 3
                                        ##       Child Loop BB2_197 Depth 3
                                        ##       Child Loop BB2_201 Depth 3
                                        ##       Child Loop BB2_154 Depth 3
                                        ##       Child Loop BB2_174 Depth 3
                                        ##       Child Loop BB2_178 Depth 3
                                        ##       Child Loop BB2_183 Depth 3
                                        ##       Child Loop BB2_221 Depth 3
                                        ##       Child Loop BB2_225 Depth 3
                                        ##       Child Loop BB2_190 Depth 3
                                        ##       Child Loop BB2_162 Depth 3
                                        ##       Child Loop BB2_166 Depth 3
                                        ##       Child Loop BB2_170 Depth 3
                                        ##       Child Loop BB2_172 Depth 3
                                        ##       Child Loop BB2_206 Depth 3
                                        ##       Child Loop BB2_210 Depth 3
                                        ##       Child Loop BB2_216 Depth 3
	movq	%rbx, %r10
	shlq	$5, %r10
	leaq	(%r11,%r10), %r13
	movl	-2692(%rbp), %r9d
	cmpl	$6, %r9d
	jne	LBB2_79
## %bb.77:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$1, 8(%r11,%r10)
	jne	LBB2_79
## %bb.78:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$8, -2696(%rbp)
	leaq	__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj16EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r10
	leaq	__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj8EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	cmoveq	%r9, %r10
	movq	%r10, -48(%rbp)                 ## 8-byte Spill
	jmp	LBB2_99
LBB2_79:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	$0, -48(%rbp)                   ## 8-byte Folded Spill
	cmpl	$6, %r9d
	ja	LBB2_99
## %bb.80:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%r9d, %r9d
	leaq	LJTI2_0(%rip), %r10
	movslq	(%r10,%r9,4), %r9
	addq	%r10, %r9
	jmpq	*%r9
LBB2_81:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2696(%rbp), %r9d
	cmpb	$1, -348(%rbp)
	jne	LBB2_93
## %bb.82:                              ##   in Loop: Header=BB2_76 Depth=1
	leal	-1(%r9), %r10d
	movl	%r9d, %r11d
	xorl	%r10d, %r11d
	cmpl	%r10d, %r11d
	jbe	LBB2_231
## %bb.83:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %r9d
	cmpl	$5, %r9d
	jae	LBB2_231
## %bb.84:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%r9d, %r9d
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE(%rip), %r10
	jmp	LBB2_96
LBB2_85:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2696(%rbp), %r9d
	leal	-1(%r9), %r10d
	movl	%r9d, %r11d
	xorl	%r10d, %r11d
	cmpl	%r10d, %r11d
	jbe	LBB2_231
## %bb.86:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %r9d
	cmpl	$4, %r9d
	jae	LBB2_231
## %bb.87:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%r9d, %r9d
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.6(%rip), %r10
	jmp	LBB2_96
LBB2_88:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2696(%rbp), %r9d
	cmpb	$1, -348(%rbp)
	jne	LBB2_97
## %bb.89:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$16, %r9d
	movl	$0, %r11d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r10
	cmoveq	%r10, %r11
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	jmp	LBB2_98
LBB2_90:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2696(%rbp), %r9d
	cmpl	$16, %r9d
	movl	$0, %r11d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r10
	cmoveq	%r10, %r11
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	jmp	LBB2_98
LBB2_91:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2696(%rbp), %r9d
	cmpl	$16, %r9d
	movl	$0, %r11d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r10
	cmoveq	%r10, %r11
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	jmp	LBB2_98
LBB2_93:                                ##   in Loop: Header=BB2_76 Depth=1
	leal	-1(%r9), %r10d
	movl	%r9d, %r11d
	xorl	%r10d, %r11d
	cmpl	%r10d, %r11d
	jbe	LBB2_231
## %bb.94:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %r9d
	cmpl	$5, %r9d
	jae	LBB2_231
## %bb.95:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%r9d, %r9d
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5(%rip), %r10
LBB2_96:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	(%r10,%r9,8), %r9
	movq	%r9, -48(%rbp)                  ## 8-byte Spill
	jmp	LBB2_99
LBB2_231:                               ##   in Loop: Header=BB2_76 Depth=1
	movq	$0, -48(%rbp)                   ## 8-byte Folded Spill
	jmp	LBB2_99
LBB2_97:                                ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$16, %r9d
	movl	$0, %r11d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r10
	cmoveq	%r10, %r11
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
LBB2_98:                                ##   in Loop: Header=BB2_76 Depth=1
	cmoveq	%r9, %r11
	movq	%r11, -48(%rbp)                 ## 8-byte Spill
LBB2_99:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	%rbx, -208(%rbp)                ## 8-byte Spill
	movq	24(%r13), %r9
	leaq	1(%r9), %rbx
	cmpq	$-1, %rbx
	movq	%rbx, %r10
	cmoveq	%r8, %r10
	leaq	(%rdx,%r9), %r14
	incq	%r14
	testq	%r10, %r10
	jle	LBB2_101
## %bb.100:                             ##   in Loop: Header=BB2_76 Depth=1
	movq	%r14, %rdi
	movq	%r10, %rsi
	movq	%r14, -56(%rbp)                 ## 8-byte Spill
	movq	%r10, -160(%rbp)                ## 8-byte Spill
	movl	%eax, %r14d
	callq	___bzero
	movl	%r14d, %eax
	movq	-160(%rbp), %r10                ## 8-byte Reload
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-144(%rbp), %rdx                ## 8-byte Reload
	movq	-96(%rbp), %r8                  ## 8-byte Reload
	movq	-112(%rbp), %rdi                ## 8-byte Reload
	movq	-120(%rbp), %rsi                ## 8-byte Reload
	movq	-104(%rbp), %rcx                ## 8-byte Reload
LBB2_101:                               ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$0, 20(%r13)
	je	LBB2_75
## %bb.102:                             ##   in Loop: Header=BB2_76 Depth=1
	movl	$0, -84(%rbp)                   ## 4-byte Folded Spill
	movq	%r13, -216(%rbp)                ## 8-byte Spill
LBB2_103:                               ##   Parent Loop BB2_76 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB2_138 Depth 3
                                        ##       Child Loop BB2_142 Depth 3
                                        ##       Child Loop BB2_146 Depth 3
                                        ##       Child Loop BB2_148 Depth 3
                                        ##       Child Loop BB2_197 Depth 3
                                        ##       Child Loop BB2_201 Depth 3
                                        ##       Child Loop BB2_154 Depth 3
                                        ##       Child Loop BB2_174 Depth 3
                                        ##       Child Loop BB2_178 Depth 3
                                        ##       Child Loop BB2_183 Depth 3
                                        ##       Child Loop BB2_221 Depth 3
                                        ##       Child Loop BB2_225 Depth 3
                                        ##       Child Loop BB2_190 Depth 3
                                        ##       Child Loop BB2_162 Depth 3
                                        ##       Child Loop BB2_166 Depth 3
                                        ##       Child Loop BB2_170 Depth 3
                                        ##       Child Loop BB2_172 Depth 3
                                        ##       Child Loop BB2_206 Depth 3
                                        ##       Child Loop BB2_210 Depth 3
                                        ##       Child Loop BB2_216 Depth 3
	movq	%r10, -160(%rbp)                ## 8-byte Spill
	movq	%rbx, %r10
	movq	%rdx, %rbx
	subq	$8, %rsp
	movq	%r12, %rdx
	leaq	-240(%rbp), %r8
	movq	%rbx, %r9
	pushq	-64(%rbp)                       ## 8-byte Folded Reload
	pushq	$0
	movq	%r10, -72(%rbp)                 ## 8-byte Spill
	pushq	%r10
	callq	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.104:                             ##   in Loop: Header=BB2_103 Depth=2
	movzbl	(%rbx), %edi
	cmpq	$4, %rdi
	ja	LBB2_237
## %bb.105:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	leaq	-1(%rcx), %r15
	leaq	1(%rbx), %rcx
	movq	%rcx, -152(%rbp)                ## 8-byte Spill
	leaq	1(%r14), %r13
	movl	-2416(%rbp), %edx
	leaq	LJTI2_1(%rip), %rcx
	movslq	(%rcx,%rdi,4), %rdi
	addq	%rcx, %rdi
	movdqa	LCPI2_1(%rip), %xmm5            ## xmm5 = [127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127]
	movq	%r14, -56(%rbp)                 ## 8-byte Spill
	movq	%rbx, -80(%rbp)                 ## 8-byte Spill
	jmpq	*%rdi
LBB2_106:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	jbe	LBB2_217
## %bb.107:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdx, %rsi
	notq	%rsi
	movq	-72(%rbp), %r8                  ## 8-byte Reload
	addq	%r8, %rsi
	cmpq	$4, %rsi
	jae	LBB2_127
## %bb.108:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdx, %r8
	jmp	LBB2_212
LBB2_109:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	movq	%rdx, %rdi
	cmovbq	%r15, %rdi
	testq	%rdi, %rdi
	leaq	1(%rbx), %rcx
	je	LBB2_149
## %bb.110:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %rdi
	jb	LBB2_111
## %bb.118:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%r14,%rdi), %r8
	incq	%r8
	cmpq	%r8, %rcx
	jae	LBB2_131
## %bb.119:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdi), %r8
	incq	%r8
	cmpq	%r8, %r13
	jae	LBB2_131
LBB2_111:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
LBB2_144:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %r10
	movq	%r8, %r9
	andq	$3, %r10
	je	LBB2_147
## %bb.145:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r9
LBB2_146:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r13,%r9), %r11d
	addb	%r11b, (%rcx,%r9)
	incq	%r9
	decq	%r10
	jne	LBB2_146
LBB2_147:                               ##   in Loop: Header=BB2_103 Depth=2
	subq	%rdi, %r8
	cmpq	$-4, %r8
	ja	LBB2_149
LBB2_148:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r14,%r9), %r8d
	addb	%r8b, 1(%rbx,%r9)
	movzbl	2(%r14,%r9), %r8d
	addb	%r8b, 2(%rbx,%r9)
	movzbl	3(%r14,%r9), %r8d
	addb	%r8b, 3(%rbx,%r9)
	movzbl	4(%r14,%r9), %r8d
	addb	%r8b, 4(%rbx,%r9)
	addq	$4, %r9
	cmpq	%r9, %rdi
	jne	LBB2_148
LBB2_149:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	jbe	LBB2_217
## %bb.150:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rcx
	notq	%rcx
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
	addq	%rsi, %rcx
	cmpq	$4, %rcx
	movq	%r15, -128(%rbp)                ## 8-byte Spill
	jae	LBB2_156
## %bb.151:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rax
LBB2_152:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r14, %rdi
	subq	%rdx, %rdi
	incq	%rdi
	movq	%rbx, %r8
	subq	%rdx, %r8
	incq	%r8
	movq	-152(%rbp), %rdx                ## 8-byte Reload
	movq	%r15, %r10
	jmp	LBB2_154
LBB2_153:                               ##   in Loop: Header=BB2_154 Depth=3
	cmpl	%r13d, %r15d
	cmoval	%ebx, %r11d
	cmpl	%r12d, %r15d
	cmoval	%ebx, %r11d
	addb	%r11b, (%rdx,%rax)
	decq	%r10
	incq	%rdx
	movq	%r9, %r13
	incq	%r13
	incq	%rdi
	incq	%r8
	cmpq	%r10, %rax
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	movq	-128(%rbp), %r15                ## 8-byte Reload
	je	LBB2_217
LBB2_154:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r8,%rax), %r11d
	movq	%r13, %r9
	movzbl	(%r13,%rax), %r14d
	movzbl	(%rdi,%rax), %ebx
	leal	(%r14,%r11), %r13d
	subl	%ebx, %r13d
	movl	%r13d, %r12d
	subl	%r11d, %r12d
	movl	%r13d, %esi
	subl	%ebx, %esi
	movl	%r12d, %r15d
	negl	%r15d
	cmovsl	%r12d, %r15d
	movl	%r13d, %ecx
	subl	%r14d, %ecx
	movl	%r14d, %r12d
	subl	%r13d, %r12d
	cmovll	%ecx, %r12d
	movl	%esi, %r13d
	negl	%r13d
	cmovsl	%esi, %r13d
	cmpl	%r13d, %r12d
	ja	LBB2_153
## %bb.155:                             ##   in Loop: Header=BB2_154 Depth=3
	movl	%r14d, %ebx
	jmp	LBB2_153
LBB2_112:                               ##   in Loop: Header=BB2_103 Depth=2
	testq	%r15, %r15
	je	LBB2_217
## %bb.113:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %r15
	jb	LBB2_114
## %bb.121:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	leaq	(%r14,%rcx), %rdx
	leaq	1(%rbx), %rsi
	cmpq	%rdx, %rsi
	jae	LBB2_133
## %bb.122:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rcx), %rdx
	cmpq	%rdx, %r13
	jae	LBB2_133
LBB2_114:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%edx, %edx
LBB2_168:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
	movl	%esi, %r8d
	andl	$3, %r8d
	movq	%rdx, %rdi
	cmpl	$1, %r8d
	leaq	1(%rbx), %rcx
	je	LBB2_171
## %bb.169:                             ##   in Loop: Header=BB2_103 Depth=2
	leal	-1(%rsi), %r8d
	andl	$3, %r8d
	movq	%rdx, %rdi
LBB2_170:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r13,%rdi), %r9d
	addb	%r9b, (%rcx,%rdi)
	incq	%rdi
	decq	%r8
	jne	LBB2_170
LBB2_171:                               ##   in Loop: Header=BB2_103 Depth=2
	subq	%rdx, %rsi
	addq	$-2, %rsi
	cmpq	$3, %rsi
	jb	LBB2_217
LBB2_172:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r14,%rdi), %edx
	addb	%dl, 1(%rbx,%rdi)
	movzbl	2(%r14,%rdi), %edx
	addb	%dl, 2(%rbx,%rdi)
	movzbl	3(%r14,%rdi), %edx
	addb	%dl, 3(%rbx,%rdi)
	movzbl	4(%r14,%rdi), %edx
	addb	%dl, 4(%rbx,%rdi)
	addq	$4, %rdi
	cmpq	%rdi, %r15
	jne	LBB2_172
	jmp	LBB2_217
LBB2_115:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	movq	%rdx, %rdi
	cmovbq	%r15, %rdi
	testq	%rdi, %rdi
	leaq	1(%rbx), %rcx
	je	LBB2_184
## %bb.116:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %rdi
	jb	LBB2_117
## %bb.124:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%r14,%rdi), %r8
	incq	%r8
	cmpq	%r8, %rcx
	jae	LBB2_135
## %bb.125:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdi), %r8
	incq	%r8
	cmpq	%r8, %r13
	jae	LBB2_135
LBB2_117:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
LBB2_180:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r9
	orq	$1, %r9
	testb	$1, %dil
	je	LBB2_182
## %bb.181:                             ##   in Loop: Header=BB2_103 Depth=2
	movzbl	(%r13,%r8), %r10d
	shrb	%r10b
	addb	%r10b, (%rcx,%r8)
	movq	%r9, %r8
LBB2_182:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r9, %rdi
	je	LBB2_184
LBB2_183:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r14,%r8), %r9d
	shrb	%r9b
	addb	%r9b, 1(%rbx,%r8)
	movzbl	2(%r14,%r8), %r9d
	shrb	%r9b
	addb	%r9b, 2(%rbx,%r8)
	addq	$2, %r8
	cmpq	%r8, %rdi
	jne	LBB2_183
LBB2_184:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	jbe	LBB2_217
## %bb.185:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %r8
	notq	%r8
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
	addq	%rsi, %r8
	cmpq	$8, %r8
	jae	LBB2_191
LBB2_186:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %r9d
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	subl	%ecx, %r9d
	movq	%rdi, %r8
	testb	$1, %r9b
	jne	LBB2_188
## %bb.187:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %r8
	subq	%rdx, %r8
	movq	-152(%rbp), %r9                 ## 8-byte Reload
	movzbl	(%r9,%r8), %r8d
	movzbl	(%r13,%rdi), %esi
	addl	%r8d, %esi
	shrl	%esi
	addb	%sil, (%r9,%rdi)
	leaq	1(%rdi), %r8
LBB2_188:                               ##   in Loop: Header=BB2_103 Depth=2
	leaq	-2(%rcx), %rsi
	cmpq	%rdi, %rsi
	je	LBB2_217
## %bb.189:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rsi
	notq	%rsi
	addq	-72(%rbp), %rsi                 ## 8-byte Folded Reload
	leaq	(%rbx,%r8), %rdi
	addq	$2, %rdi
	addq	%r14, %r8
	addq	$2, %r8
	negq	%rdx
LBB2_190:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-1(%rdi,%rdx), %r9d
	movzbl	-1(%r8), %r10d
	addl	%r9d, %r10d
	shrl	%r10d
	addb	%r10b, -1(%rdi)
	movzbl	(%rdi,%rdx), %r9d
	movzbl	(%r8), %r10d
	addl	%r9d, %r10d
	shrl	%r10d
	addb	%r10b, (%rdi)
	addq	$2, %rdi
	addq	$2, %r8
	addq	$-2, %rsi
	jne	LBB2_190
	jmp	LBB2_217
LBB2_156:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, -200(%rbp)                ## 8-byte Spill
	movq	%r13, -136(%rbp)                ## 8-byte Spill
	movq	%rbx, %rcx
	addq	%rdi, %rbx
	incq	%rbx
	leaq	(%rcx,%rsi), %rax
	movq	%rdi, %r8
	subq	%rdx, %r8
	leaq	(%rcx,%r8), %r10
	incq	%r10
	movq	%rsi, %r9
	subq	%rdx, %r9
	leaq	(%rcx,%r9), %r15
	movq	%r14, %rcx
	addq	%rdi, %r14
	incq	%r14
	leaq	(%rcx,%rsi), %r13
	leaq	(%rcx,%r8), %r11
	incq	%r11
	addq	%rcx, %r9
	cmpq	%r15, %rbx
	setb	%r12b
	cmpq	%rax, %r10
	setb	%r15b
	cmpq	%r13, %rbx
	setb	%r8b
	cmpq	%rax, %r14
	setb	%r13b
	cmpq	%r9, %rbx
	setb	%r9b
	cmpq	%rax, %r11
	setb	%al
	testb	%r15b, %r12b
	jne	LBB2_203
## %bb.157:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%r13b, %r8b
	jne	LBB2_203
## %bb.158:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%al, %r9b
	movq	-128(%rbp), %r15                ## 8-byte Reload
	movq	-136(%rbp), %r13                ## 8-byte Reload
	movq	-200(%rbp), %rsi                ## 8-byte Reload
	jne	LBB2_195
## %bb.159:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$16, %rsi
	jae	LBB2_196
## %bb.160:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jmp	LBB2_200
LBB2_191:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r15, %r12
	leaq	(%rbx,%rdi), %r9
	incq	%r9
	leaq	(%rbx,%rsi), %r10
	movq	%rdi, %r11
	subq	%rdx, %r11
	addq	%rbx, %r11
	incq	%r11
	movq	%rbx, %rcx
	movq	%rsi, %rbx
	subq	%rdx, %rbx
	addq	%rcx, %rbx
	movq	%r14, %rcx
	addq	%rdi, %r14
	incq	%r14
	leaq	(%rcx,%rsi), %r15
	cmpq	%rbx, %r9
	setb	%bl
	cmpq	%r10, %r11
	setb	%r11b
	cmpq	%r15, %r9
	setb	%r9b
	cmpq	%r10, %r14
	setb	%r10b
	testb	%r11b, %bl
	jne	LBB2_219
## %bb.192:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%r10b, %r9b
	movq	%r12, %r15
	jne	LBB2_204
## %bb.193:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %r8
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jae	LBB2_220
## %bb.194:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r9d, %r9d
	jmp	LBB2_224
LBB2_127:                               ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdx), %rdi
	incq	%rdi
	subq	%rdx, %r8
	addq	%rbx, %r8
	cmpq	%r8, %rdi
	jae	LBB2_129
## %bb.128:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	leaq	(%rbx,%rcx), %rdi
	movq	%rdx, %r8
	leaq	1(%rbx), %rcx
	cmpq	%rdi, %rcx
	jb	LBB2_212
LBB2_129:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rsi
	jae	LBB2_205
## %bb.130:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%edi, %edi
	jmp	LBB2_209
LBB2_131:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rdi
	jae	LBB2_137
## %bb.132:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
	jmp	LBB2_141
LBB2_133:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %r15
	jae	LBB2_161
## %bb.134:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%edx, %edx
	jmp	LBB2_165
LBB2_135:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rdi
	jae	LBB2_173
## %bb.136:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
	jmp	LBB2_177
LBB2_137:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %r8d
	andl	$-32, %r8d
	xorl	%r9d, %r9d
LBB2_138:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%r14,%r9), %xmm0
	movdqu	17(%r14,%r9), %xmm1
	movdqu	1(%rbx,%r9), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%rbx,%r9), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%r9)
	movdqu	%xmm0, 17(%rbx,%r9)
	addq	$32, %r9
	cmpq	%r9, %r8
	jne	LBB2_138
## %bb.139:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rdi
	je	LBB2_149
## %bb.140:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %dil
	je	LBB2_144
LBB2_141:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r9
	movl	%edi, %r8d
	andl	$-4, %r8d
LBB2_142:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%r13,%r9), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	movd	(%rcx,%r9), %xmm1               ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%rcx,%r9)
	addq	$4, %r9
	cmpq	%r9, %r8
	jne	LBB2_142
## %bb.143:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rdi
	je	LBB2_149
	jmp	LBB2_144
LBB2_161:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r15, %rdx
	andq	$-32, %rdx
	xorl	%edi, %edi
LBB2_162:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%rbx,%rdi), %xmm0
	movdqu	17(%rbx,%rdi), %xmm1
	movdqu	1(%r14,%rdi), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%r14,%rdi), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%rdi)
	movdqu	%xmm0, 17(%rbx,%rdi)
	addq	$32, %rdi
	cmpq	%rdi, %rdx
	jne	LBB2_162
## %bb.163:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	je	LBB2_217
## %bb.164:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %r15b
	je	LBB2_168
LBB2_165:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdx, %rdi
	movq	%r15, %rdx
	andq	$-4, %rdx
	leaq	1(%rbx), %rcx
LBB2_166:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rcx,%rdi), %xmm0              ## xmm0 = mem[0],zero,zero,zero
	movd	(%r13,%rdi), %xmm1              ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%rcx,%rdi)
	addq	$4, %rdi
	cmpq	%rdi, %rdx
	jne	LBB2_166
## %bb.167:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdx, %r15
	je	LBB2_217
	jmp	LBB2_168
LBB2_173:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %r8d
	andl	$-32, %r8d
	xorl	%r9d, %r9d
LBB2_174:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%r14,%r9), %xmm0
	movdqu	17(%r14,%r9), %xmm1
	psrlw	$1, %xmm0
	pand	%xmm5, %xmm0
	psrlw	$1, %xmm1
	pand	%xmm5, %xmm1
	movdqu	1(%rbx,%r9), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%rbx,%r9), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%r9)
	movdqu	%xmm0, 17(%rbx,%r9)
	addq	$32, %r9
	cmpq	%r9, %r8
	jne	LBB2_174
## %bb.175:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rdi
	je	LBB2_184
## %bb.176:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %dil
	je	LBB2_180
LBB2_177:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r9
	movl	%edi, %r8d
	andl	$-4, %r8d
LBB2_178:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%r13,%r9), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	psrlw	$1, %xmm0
	pand	%xmm5, %xmm0
	movd	(%rcx,%r9), %xmm1               ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%rcx,%r9)
	addq	$4, %r9
	cmpq	%r9, %r8
	jne	LBB2_178
## %bb.179:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rdi
	je	LBB2_184
	jmp	LBB2_180
LBB2_195:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rax
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jmp	LBB2_152
LBB2_196:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %r8
	andq	$-16, %r8
	movq	%rdx, %rcx
	negq	%rcx
	movq	%r8, %r9
LBB2_197:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	(%rbx,%rcx), %xmm8
	movdqa	%xmm8, -320(%rbp)               ## 16-byte Spill
	movdqa	%xmm8, %xmm10
	pxor	%xmm11, %xmm11
	punpckhbw	%xmm11, %xmm10          ## xmm10 = xmm10[8],xmm11[8],xmm10[9],xmm11[9],xmm10[10],xmm11[10],xmm10[11],xmm11[11],xmm10[12],xmm11[12],xmm10[13],xmm11[13],xmm10[14],xmm11[14],xmm10[15],xmm11[15]
	movdqa	%xmm10, %xmm6
	punpcklwd	%xmm11, %xmm10          ## xmm10 = xmm10[0],xmm11[0],xmm10[1],xmm11[1],xmm10[2],xmm11[2],xmm10[3],xmm11[3]
	punpcklbw	%xmm11, %xmm8           ## xmm8 = xmm8[0],xmm11[0],xmm8[1],xmm11[1],xmm8[2],xmm11[2],xmm8[3],xmm11[3],xmm8[4],xmm11[4],xmm8[5],xmm11[5],xmm8[6],xmm11[6],xmm8[7],xmm11[7]
	movdqa	%xmm8, %xmm7
	punpckhwd	%xmm11, %xmm7           ## xmm7 = xmm7[4],xmm11[4],xmm7[5],xmm11[5],xmm7[6],xmm11[6],xmm7[7],xmm11[7]
	punpcklwd	%xmm11, %xmm8           ## xmm8 = xmm8[0],xmm11[0],xmm8[1],xmm11[1],xmm8[2],xmm11[2],xmm8[3],xmm11[3]
	movdqu	(%r14), %xmm2
	movdqa	%xmm2, -304(%rbp)               ## 16-byte Spill
	movdqa	%xmm2, %xmm14
	punpckhbw	%xmm11, %xmm14          ## xmm14 = xmm14[8],xmm11[8],xmm14[9],xmm11[9],xmm14[10],xmm11[10],xmm14[11],xmm11[11],xmm14[12],xmm11[12],xmm14[13],xmm11[13],xmm14[14],xmm11[14],xmm14[15],xmm11[15]
	movdqa	%xmm14, -176(%rbp)              ## 16-byte Spill
	punpcklwd	%xmm11, %xmm14          ## xmm14 = xmm14[0],xmm11[0],xmm14[1],xmm11[1],xmm14[2],xmm11[2],xmm14[3],xmm11[3]
	punpcklbw	%xmm11, %xmm2           ## xmm2 = xmm2[0],xmm11[0],xmm2[1],xmm11[1],xmm2[2],xmm11[2],xmm2[3],xmm11[3],xmm2[4],xmm11[4],xmm2[5],xmm11[5],xmm2[6],xmm11[6],xmm2[7],xmm11[7]
	movdqa	%xmm2, %xmm12
	punpckhwd	%xmm11, %xmm12          ## xmm12 = xmm12[4],xmm11[4],xmm12[5],xmm11[5],xmm12[6],xmm11[6],xmm12[7],xmm11[7]
	punpcklwd	%xmm11, %xmm2           ## xmm2 = xmm2[0],xmm11[0],xmm2[1],xmm11[1],xmm2[2],xmm11[2],xmm2[3],xmm11[3]
	movdqu	(%r14,%rcx), %xmm0
	movdqa	%xmm0, %xmm13
	movdqa	%xmm0, -288(%rbp)               ## 16-byte Spill
	punpcklbw	%xmm11, %xmm13          ## xmm13 = xmm13[0],xmm11[0],xmm13[1],xmm11[1],xmm13[2],xmm11[2],xmm13[3],xmm11[3],xmm13[4],xmm11[4],xmm13[5],xmm11[5],xmm13[6],xmm11[6],xmm13[7],xmm11[7]
	movdqa	%xmm13, %xmm15
	punpcklwd	%xmm11, %xmm15          ## xmm15 = xmm15[0],xmm11[0],xmm15[1],xmm11[1],xmm15[2],xmm11[2],xmm15[3],xmm11[3]
	punpckhwd	%xmm11, %xmm13          ## xmm13 = xmm13[4],xmm11[4],xmm13[5],xmm11[5],xmm13[6],xmm11[6],xmm13[7],xmm11[7]
	movdqa	%xmm2, %xmm5
	paddd	%xmm8, %xmm5
	movdqa	%xmm12, %xmm4
	paddd	%xmm7, %xmm4
	psubd	%xmm13, %xmm4
	psubd	%xmm15, %xmm5
	movdqa	%xmm5, %xmm3
	psubd	%xmm8, %xmm3
	movdqa	%xmm4, %xmm8
	psubd	%xmm7, %xmm8
	punpckhbw	%xmm11, %xmm0           ## xmm0 = xmm0[8],xmm11[8],xmm0[9],xmm11[9],xmm0[10],xmm11[10],xmm0[11],xmm11[11],xmm0[12],xmm11[12],xmm0[13],xmm11[13],xmm0[14],xmm11[14],xmm0[15],xmm11[15]
	movdqa	%xmm0, %xmm1
	punpcklwd	%xmm11, %xmm1           ## xmm1 = xmm1[0],xmm11[0],xmm1[1],xmm11[1],xmm1[2],xmm11[2],xmm1[3],xmm11[3]
	movdqa	%xmm14, %xmm9
	paddd	%xmm10, %xmm9
	psubd	%xmm1, %xmm9
	movdqa	%xmm9, %xmm7
	psubd	%xmm10, %xmm7
	punpckhwd	%xmm11, %xmm6           ## xmm6 = xmm6[4],xmm11[4],xmm6[5],xmm11[5],xmm6[6],xmm11[6],xmm6[7],xmm11[7]
	movdqa	-176(%rbp), %xmm10              ## 16-byte Reload
	punpckhwd	%xmm11, %xmm10          ## xmm10 = xmm10[4],xmm11[4],xmm10[5],xmm11[5],xmm10[6],xmm11[6],xmm10[7],xmm11[7]
	movdqa	%xmm10, -176(%rbp)              ## 16-byte Spill
	punpckhwd	%xmm11, %xmm0           ## xmm0 = xmm0[4],xmm11[4],xmm0[5],xmm11[5],xmm0[6],xmm11[6],xmm0[7],xmm11[7]
	movdqa	%xmm10, %xmm11
	paddd	%xmm6, %xmm11
	psubd	%xmm0, %xmm11
	movdqa	%xmm11, %xmm10
	psubd	%xmm6, %xmm10
	movdqa	%xmm5, %xmm6
	psubd	%xmm2, %xmm6
	movdqa	%xmm4, %xmm2
	psubd	%xmm12, %xmm2
	movdqa	%xmm2, -192(%rbp)               ## 16-byte Spill
	movdqa	%xmm9, %xmm12
	psubd	%xmm14, %xmm12
	movdqa	%xmm11, %xmm2
	psubd	-176(%rbp), %xmm2               ## 16-byte Folded Reload
	psubd	%xmm15, %xmm5
	psubd	%xmm13, %xmm4
	psubd	%xmm1, %xmm9
	psubd	%xmm0, %xmm11
	movdqa	%xmm10, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm10
	psubd	%xmm0, %xmm10
	movdqa	%xmm7, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm7
	psubd	%xmm0, %xmm7
	movdqa	%xmm8, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm8
	psubd	%xmm0, %xmm8
	movdqa	%xmm3, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm3
	psubd	%xmm0, %xmm3
	movdqa	%xmm2, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm2
	psubd	%xmm0, %xmm2
	movdqa	%xmm12, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm12
	psubd	%xmm0, %xmm12
	movdqa	-192(%rbp), %xmm1               ## 16-byte Reload
	movdqa	%xmm1, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm1
	psubd	%xmm0, %xmm1
	movdqa	%xmm1, %xmm13
	movdqa	%xmm1, -192(%rbp)               ## 16-byte Spill
	movdqa	%xmm6, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm6
	psubd	%xmm0, %xmm6
	movdqa	%xmm11, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm11
	psubd	%xmm0, %xmm11
	movdqa	%xmm9, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm9
	psubd	%xmm0, %xmm9
	movdqa	%xmm4, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm4
	psubd	%xmm0, %xmm4
	movdqa	%xmm5, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm5
	psubd	%xmm0, %xmm5
	movdqa	%xmm6, %xmm0
	movdqa	LCPI2_0(%rip), %xmm1            ## xmm1 = [2147483648,2147483648,2147483648,2147483648]
	pxor	%xmm1, %xmm0
	pxor	%xmm1, %xmm3
	movdqa	%xmm3, %xmm14
	pcmpgtd	%xmm0, %xmm14
	movdqa	%xmm14, -176(%rbp)              ## 16-byte Spill
	movdqa	%xmm13, %xmm0
	pxor	%xmm1, %xmm0
	pxor	%xmm1, %xmm8
	movdqa	%xmm8, %xmm15
	pcmpgtd	%xmm0, %xmm15
	movdqa	%xmm12, %xmm0
	pxor	%xmm1, %xmm0
	pxor	%xmm1, %xmm7
	movdqa	%xmm7, %xmm13
	pcmpgtd	%xmm0, %xmm13
	movdqa	%xmm2, %xmm14
	pxor	%xmm1, %xmm14
	pxor	%xmm1, %xmm10
	movdqa	%xmm10, %xmm0
	pcmpgtd	%xmm14, %xmm0
	pcmpgtd	%xmm5, %xmm6
	pxor	%xmm1, %xmm5
	pcmpgtd	%xmm5, %xmm3
	por	-176(%rbp), %xmm3               ## 16-byte Folded Reload
	movdqa	-192(%rbp), %xmm5               ## 16-byte Reload
	pcmpgtd	%xmm4, %xmm5
	pxor	%xmm1, %xmm4
	pcmpgtd	%xmm4, %xmm8
	por	%xmm15, %xmm8
	packssdw	%xmm8, %xmm3
	pcmpgtd	%xmm9, %xmm12
	pxor	%xmm1, %xmm9
	pcmpgtd	%xmm9, %xmm7
	por	%xmm13, %xmm7
	pcmpgtd	%xmm11, %xmm2
	pxor	%xmm1, %xmm11
	pcmpgtd	%xmm11, %xmm10
	por	%xmm0, %xmm10
	packssdw	%xmm10, %xmm7
	packsswb	%xmm7, %xmm3
	packssdw	%xmm2, %xmm12
	packssdw	%xmm5, %xmm6
	packsswb	%xmm12, %xmm6
	movdqa	-288(%rbp), %xmm0               ## 16-byte Reload
	pand	%xmm6, %xmm0
	pandn	-304(%rbp), %xmm6               ## 16-byte Folded Reload
	por	%xmm0, %xmm6
	pand	%xmm3, %xmm6
	pandn	-320(%rbp), %xmm3               ## 16-byte Folded Reload
	por	%xmm6, %xmm3
	movdqu	(%rbx), %xmm0
	paddb	%xmm3, %xmm0
	movdqu	%xmm0, (%rbx)
	addq	$16, %rbx
	addq	$16, %r14
	addq	$-16, %r9
	jne	LBB2_197
## %bb.198:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rsi
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	je	LBB2_217
## %bb.199:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$12, %sil
	je	LBB2_227
LBB2_200:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %r9
	andq	$-4, %r9
	leaq	(%rdi,%r9), %rax
	leaq	(%rbx,%rdi), %r14
	incq	%r14
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	addq	%rcx, %rdi
	incq	%rdi
	movq	%r9, %rbx
	pxor	%xmm8, %xmm8
	movdqa	LCPI2_0(%rip), %xmm9            ## xmm9 = [2147483648,2147483648,2147483648,2147483648]
LBB2_201:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%r10,%r8), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	movdqa	%xmm0, %xmm4
	punpcklbw	%xmm8, %xmm4            ## xmm4 = xmm4[0],xmm8[0],xmm4[1],xmm8[1],xmm4[2],xmm8[2],xmm4[3],xmm8[3],xmm4[4],xmm8[4],xmm4[5],xmm8[5],xmm4[6],xmm8[6],xmm4[7],xmm8[7]
	punpcklwd	%xmm8, %xmm4            ## xmm4 = xmm4[0],xmm8[0],xmm4[1],xmm8[1],xmm4[2],xmm8[2],xmm4[3],xmm8[3]
	movd	(%rdi,%r8), %xmm1               ## xmm1 = mem[0],zero,zero,zero
	movdqa	%xmm1, %xmm6
	punpcklbw	%xmm8, %xmm6            ## xmm6 = xmm6[0],xmm8[0],xmm6[1],xmm8[1],xmm6[2],xmm8[2],xmm6[3],xmm8[3],xmm6[4],xmm8[4],xmm6[5],xmm8[5],xmm6[6],xmm8[6],xmm6[7],xmm8[7]
	punpcklwd	%xmm8, %xmm6            ## xmm6 = xmm6[0],xmm8[0],xmm6[1],xmm8[1],xmm6[2],xmm8[2],xmm6[3],xmm8[3]
	movd	(%r11,%r8), %xmm3               ## xmm3 = mem[0],zero,zero,zero
	movdqa	%xmm3, %xmm7
	punpcklbw	%xmm8, %xmm7            ## xmm7 = xmm7[0],xmm8[0],xmm7[1],xmm8[1],xmm7[2],xmm8[2],xmm7[3],xmm8[3],xmm7[4],xmm8[4],xmm7[5],xmm8[5],xmm7[6],xmm8[6],xmm7[7],xmm8[7]
	punpcklwd	%xmm8, %xmm7            ## xmm7 = xmm7[0],xmm8[0],xmm7[1],xmm8[1],xmm7[2],xmm8[2],xmm7[3],xmm8[3]
	movdqa	%xmm6, %xmm5
	paddd	%xmm4, %xmm5
	psubd	%xmm7, %xmm5
	movdqa	%xmm5, %xmm2
	psubd	%xmm4, %xmm2
	movdqa	%xmm5, %xmm4
	psubd	%xmm6, %xmm4
	psubd	%xmm7, %xmm5
	movdqa	%xmm2, %xmm6
	psrad	$31, %xmm6
	pxor	%xmm6, %xmm2
	psubd	%xmm6, %xmm2
	movdqa	%xmm4, %xmm6
	psrad	$31, %xmm6
	pxor	%xmm6, %xmm4
	psubd	%xmm6, %xmm4
	movdqa	%xmm5, %xmm6
	psrad	$31, %xmm6
	pxor	%xmm6, %xmm5
	psubd	%xmm6, %xmm5
	movdqa	%xmm4, %xmm6
	pxor	%xmm9, %xmm6
	pxor	%xmm9, %xmm2
	movdqa	%xmm2, %xmm7
	pcmpgtd	%xmm6, %xmm7
	pcmpgtd	%xmm5, %xmm4
	pxor	%xmm9, %xmm5
	pcmpgtd	%xmm5, %xmm2
	por	%xmm7, %xmm2
	packssdw	%xmm2, %xmm2
	packsswb	%xmm2, %xmm2
	packssdw	%xmm4, %xmm4
	packsswb	%xmm4, %xmm4
	pand	%xmm4, %xmm3
	pandn	%xmm1, %xmm4
	por	%xmm3, %xmm4
	pand	%xmm2, %xmm4
	pandn	%xmm0, %xmm2
	por	%xmm4, %xmm2
	movd	(%r14,%r8), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	paddb	%xmm2, %xmm0
	movd	%xmm0, (%r14,%r8)
	addq	$-4, %rbx
	addq	$4, %r14
	addq	$4, %rdi
	addq	$4, %r11
	addq	$4, %r10
	cmpq	%rbx, %r8
	jne	LBB2_201
## %bb.202:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r9, %rsi
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jne	LBB2_152
	jmp	LBB2_217
LBB2_203:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rax
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	movq	-128(%rbp), %r15                ## 8-byte Reload
	movq	-136(%rbp), %r13                ## 8-byte Reload
	jmp	LBB2_152
LBB2_204:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jmp	LBB2_186
LBB2_219:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	movq	%r12, %r15
	jmp	LBB2_186
LBB2_205:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %rdi
	andq	$-32, %rdi
	leaq	17(%rbx), %r8
	leaq	(%rbx,%rdx), %r9
	addq	$17, %r9
	xorl	%r10d, %r10d
LBB2_206:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	-16(%r8,%r10), %xmm0
	movdqu	(%r8,%r10), %xmm1
	movdqu	-16(%r9,%r10), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	(%r9,%r10), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, -16(%r9,%r10)
	movdqu	%xmm0, (%r9,%r10)
	addq	$32, %r10
	cmpq	%r10, %rdi
	jne	LBB2_206
## %bb.207:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdi, %rsi
	je	LBB2_217
## %bb.208:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %sil
	je	LBB2_229
LBB2_209:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %r9
	andq	$-4, %r9
	leaq	(%r9,%rdx), %r8
	leaq	1(%rbx), %rcx
	leaq	(%rcx,%rdx), %r10
LBB2_210:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rcx,%rdi), %xmm0              ## xmm0 = mem[0],zero,zero,zero
	movd	(%r10,%rdi), %xmm1              ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%r10,%rdi)
	addq	$4, %rdi
	cmpq	%rdi, %r9
	jne	LBB2_210
## %bb.211:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r9, %rsi
	je	LBB2_217
LBB2_212:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%r8d, %edi
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	subl	%ecx, %edi
	movq	%r8, %rsi
	testb	$1, %dil
	jne	LBB2_214
## %bb.213:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rsi
	subq	%rdx, %rsi
	leaq	1(%rbx), %rdi
	movzbl	(%rdi,%rsi), %esi
	addb	%sil, (%rdi,%r8)
	leaq	1(%r8), %rsi
LBB2_214:                               ##   in Loop: Header=BB2_103 Depth=2
	leaq	-2(%rcx), %rdi
	cmpq	%r8, %rdi
	je	LBB2_217
## %bb.215:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rsi), %rdi
	addq	$2, %rdi
	notq	%rsi
	addq	-72(%rbp), %rsi                 ## 8-byte Folded Reload
	negq	%rdx
LBB2_216:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-1(%rdi,%rdx), %r8d
	addb	%r8b, -1(%rdi)
	movzbl	(%rdi,%rdx), %r8d
	addb	%r8b, (%rdi)
	addq	$2, %rdi
	addq	$-2, %rsi
	jne	LBB2_216
LBB2_217:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	$0, (%rax)
	subq	$16, %rsp
	leaq	64(%rbp), %rax
	movdqu	(%rax), %xmm0
	movdqu	%xmm0, (%rsp)
	leaq	-2704(%rbp), %r12
	movq	%r12, %rdi
	movq	-216(%rbp), %r13                ## 8-byte Reload
	movq	%r13, %rsi
	movl	-84(%rbp), %edx                 ## 4-byte Reload
	movq	-152(%rbp), %rcx                ## 8-byte Reload
	movq	%r15, %r8
	callq	*-48(%rbp)                      ## 8-byte Folded Reload
	addq	$16, %rsp
	movl	%eax, %ecx
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	leaq	16(%rbp), %r15
	jne	LBB2_70
## %bb.218:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	-84(%rbp), %ecx                 ## 4-byte Reload
	incl	%ecx
	movq	%rbx, %rsi
	movq	-160(%rbp), %rbx                ## 8-byte Reload
	movq	%r14, %rdx
	movq	-72(%rbp), %r10                 ## 8-byte Reload
	movq	%rsi, %r14
	movl	%ecx, -84(%rbp)                 ## 4-byte Spill
	cmpl	20(%r13), %ecx
	movq	-104(%rbp), %rcx                ## 8-byte Reload
	movq	-120(%rbp), %rsi                ## 8-byte Reload
	movq	-112(%rbp), %rdi                ## 8-byte Reload
	jb	LBB2_103
	jmp	LBB2_74
LBB2_220:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r9
	andq	$-32, %r9
	leaq	(%rbx,%rdi), %r10
	addq	$17, %r10
	leaq	(%r14,%rdi), %r11
	addq	$17, %r11
	movq	%rdx, %rbx
	negq	%rbx
	movq	%r9, %r14
LBB2_221:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	-16(%r10,%rbx), %xmm0
	movdqu	(%r10,%rbx), %xmm1
	movdqu	-16(%r11), %xmm2
	movdqu	(%r11), %xmm3
	movdqa	%xmm2, %xmm4
	pand	%xmm0, %xmm4
	pxor	%xmm0, %xmm2
	psrlw	$1, %xmm2
	pand	%xmm5, %xmm2
	movdqa	%xmm3, %xmm0
	pand	%xmm1, %xmm0
	pxor	%xmm1, %xmm3
	psrlw	$1, %xmm3
	pand	%xmm5, %xmm3
	movdqu	-16(%r10), %xmm1
	paddb	%xmm4, %xmm1
	paddb	%xmm2, %xmm1
	movdqu	(%r10), %xmm2
	paddb	%xmm0, %xmm2
	paddb	%xmm3, %xmm2
	movdqu	%xmm1, -16(%r10)
	movdqu	%xmm2, (%r10)
	addq	$32, %r10
	addq	$32, %r11
	addq	$-32, %r14
	jne	LBB2_221
## %bb.222:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r9, %r8
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	je	LBB2_217
## %bb.223:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$24, %r8b
	je	LBB2_228
LBB2_224:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %r10
	andq	$-8, %r10
	movq	%r9, %r11
	subq	%r10, %r11
	addq	%rdi, %r9
	addq	%r10, %rdi
	addq	%r9, %rbx
	incq	%rbx
	addq	%r14, %r9
	incq	%r9
	movq	%rdx, %r14
	negq	%r14
LBB2_225:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	(%rbx,%r14), %xmm0              ## xmm0 = mem[0],zero
	movq	(%r9), %xmm1                    ## xmm1 = mem[0],zero
	movdqa	%xmm1, %xmm2
	pand	%xmm0, %xmm2
	pxor	%xmm0, %xmm1
	psrlw	$1, %xmm1
	pand	%xmm5, %xmm1
	movq	(%rbx), %xmm0                   ## xmm0 = mem[0],zero
	paddb	%xmm2, %xmm0
	paddb	%xmm1, %xmm0
	movq	%xmm0, (%rbx)
	addq	$8, %rbx
	addq	$8, %r9
	addq	$8, %r11
	jne	LBB2_225
## %bb.226:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r10, %r8
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	movq	-80(%rbp), %rbx                 ## 8-byte Reload
	jne	LBB2_186
	jmp	LBB2_217
LBB2_227:                               ##   in Loop: Header=BB2_103 Depth=2
	addq	%r8, %rdi
	movq	%rdi, %rax
	movq	-56(%rbp), %r14                 ## 8-byte Reload
	jmp	LBB2_152
LBB2_228:                               ##   in Loop: Header=BB2_103 Depth=2
	addq	%r9, %rdi
	jmp	LBB2_186
LBB2_229:                               ##   in Loop: Header=BB2_103 Depth=2
	addq	%rdx, %rdi
	movq	%rdi, %r8
	jmp	LBB2_212
LBB2_235:
	subq	$8, %rsp
	leaq	-2704(%rbp), %rdx
	leaq	-240(%rbp), %r8
	xorl	%r9d, %r9d
	pushq	-64(%rbp)                       ## 8-byte Folded Reload
	pushq	$1
	pushq	$0
	callq	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.236:
	leaq	96(%rbp), %r10
	leaq	80(%rbp), %r11
	movq	(%r11), %r9
	movq	24(%r15), %rdx
	movq	32(%r15), %rcx
	subq	$8, %rsp
	movq	-112(%rbp), %rdi                ## 8-byte Reload
	movq	-120(%rbp), %rsi                ## 8-byte Reload
	movq	-104(%rbp), %r8                 ## 8-byte Reload
	pushq	-64(%rbp)                       ## 8-byte Folded Reload
	pushq	(%r10)
	pushq	8(%r11)
	callq	__ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	jmp	LBB2_238
LBB2_237:
	movl	$18, %ecx
LBB2_238:
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	jmp	LBB2_70
	.cfi_endproc
	.p2align	2
	.data_region jt32
L2_0_set_81 = LBB2_81-LJTI2_0
L2_0_set_99 = LBB2_99-LJTI2_0
L2_0_set_88 = LBB2_88-LJTI2_0
L2_0_set_85 = LBB2_85-LJTI2_0
L2_0_set_90 = LBB2_90-LJTI2_0
L2_0_set_91 = LBB2_91-LJTI2_0
LJTI2_0:
	.long	L2_0_set_81
	.long	L2_0_set_99
	.long	L2_0_set_88
	.long	L2_0_set_85
	.long	L2_0_set_90
	.long	L2_0_set_99
	.long	L2_0_set_91
L2_1_set_217 = LBB2_217-LJTI2_1
L2_1_set_106 = LBB2_106-LJTI2_1
L2_1_set_112 = LBB2_112-LJTI2_1
L2_1_set_115 = LBB2_115-LJTI2_1
L2_1_set_109 = LBB2_109-LJTI2_1
LJTI2_1:
	.long	L2_1_set_217
	.long	L2_1_set_106
	.long	L2_1_set_112
	.long	L2_1_set_115
	.long	L2_1_set_109
	.end_data_region
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE: ## @_ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$88, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%r9, -64(%rbp)                  ## 8-byte Spill
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	movq	%rsi, -56(%rbp)                 ## 8-byte Spill
	movq	%rdi, %r13
	movzbl	24(%rbp), %edi
	movq	16(%rbp), %r9
	cmpb	$0, 16(%r8)
	je	LBB3_2
## %bb.1:
	xorl	%ebx, %ebx
LBB3_23:
	testb	%dil, %dil
	jne	LBB3_25
LBB3_24:
	movl	$17, %eax
	cmpq	%r9, %rbx
	jne	LBB3_26
LBB3_25:
	xorl	%eax, %eax
LBB3_26:
	addq	$88, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB3_2:
	movq	%r8, %r14
	movq	%rcx, %r12
	xorl	%ebx, %ebx
	.p2align	4
LBB3_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB3_9 Depth 2
	testb	%dil, %dil
	jne	LBB3_5
## %bb.4:                               ##   in Loop: Header=BB3_3 Depth=1
	cmpq	%rbx, %r9
	jbe	LBB3_24
LBB3_5:                                 ##   in Loop: Header=BB3_3 Depth=1
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	movq	2368(%rax), %rax
	movq	(%r14), %rdx
	cmpq	%rax, %rdx
	jae	LBB3_11
## %bb.6:                               ##   in Loop: Header=BB3_3 Depth=1
	movl	(%r13,%rdx), %ecx
	bswapl	%ecx
	cmpq	%rcx, 8(%r14)
	jae	LBB3_7
LBB3_11:                                ##   in Loop: Header=BB3_3 Depth=1
	cmpq	%rax, %rdx
	jae	LBB3_12
## %bb.13:                              ##   in Loop: Header=BB3_3 Depth=1
	movl	(%r13,%rdx), %esi
	bswapl	%esi
	movq	8(%r14), %rdi
	leaq	(%rdx,%rdi), %r8
	addq	$8, %r8
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	subq	%r8, %rcx
	leaq	(%rdx,%rsi), %r8
	addq	$12, %r8
	subq	%rdi, %rsi
	cmpq	$-1, %rsi
	cmovneq	%rsi, %rcx
	addq	%rdi, %rdx
	addq	%r13, %rdx
	addq	$8, %rdx
	cmpq	%rax, %r8
	sete	%al
	jmp	LBB3_14
	.p2align	4
LBB3_12:                                ##   in Loop: Header=BB3_3 Depth=1
	movb	$1, %al
	xorl	%ecx, %ecx
	xorl	%edx, %edx
LBB3_14:                                ##   in Loop: Header=BB3_3 Depth=1
	movq	%r9, %r15
	subq	%rbx, %r9
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
	leaq	(%rsi,%rbx), %r8
	movzbl	%al, %eax
	movl	%eax, (%rsp)
	leaq	-120(%rbp), %rdi
	movq	%r12, %rsi
	callq	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	movq	-112(%rbp), %rcx
	addq	%rcx, 8(%r14)
	movq	-104(%rbp), %rdx
	movq	-80(%rbp), %rax
	movq	32(%rbp), %rsi
	movq	%rax, 72(%rsi)
	movl	-116(%rbp), %eax
	movl	%eax, 4(%rsi)
	movl	-120(%rbp), %esi
	cmpl	$3, %esi
	je	LBB3_15
## %bb.16:                              ##   in Loop: Header=BB3_3 Depth=1
	addq	%rdx, %rbx
	cmpl	$2, %esi
	je	LBB3_17
## %bb.20:                              ##   in Loop: Header=BB3_3 Depth=1
	testq	%rdx, %rdx
	setne	%dil
	movl	$17, %eax
	movzbl	24(%rbp), %r8d
	testb	%dil, %r8b
	jne	LBB3_26
## %bb.21:                              ##   in Loop: Header=BB3_3 Depth=1
	orq	%rdx, %rcx
	sete	%cl
	testl	%esi, %esi
	setne	%dl
	testb	%dl, %cl
	jne	LBB3_26
## %bb.22:                              ##   in Loop: Header=BB3_3 Depth=1
	movl	%r8d, %edi
	movq	%r15, %r9
	cmpb	$1, 16(%r14)
	jne	LBB3_3
	jmp	LBB3_23
	.p2align	4
LBB3_7:                                 ##   in Loop: Header=BB3_3 Depth=1
	addq	%rcx, %rdx
	addq	$12, %rdx
	movq	%rdx, (%r14)
	movq	$0, 8(%r14)
	cmpq	%rax, %rdx
	jae	LBB3_11
	.p2align	4
LBB3_9:                                 ##   Parent Loop BB3_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	(%r13,%rdx), %ecx
	bswapl	%ecx
	testl	%ecx, %ecx
	jne	LBB3_11
## %bb.10:                              ##   in Loop: Header=BB3_9 Depth=2
	addq	$12, %rdx
	movq	%rdx, (%r14)
	movq	$0, 8(%r14)
	cmpq	%rax, %rdx
	jb	LBB3_9
	jmp	LBB3_11
LBB3_17:
	movb	$1, 16(%r14)
	movq	-96(%rbp), %rcx
	movl	$14, %eax
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	cmpq	2376(%rdx), %rcx
	movzbl	24(%rbp), %edi
	jne	LBB3_26
## %bb.18:
	movq	-88(%rbp), %rcx
	movl	$17, %eax
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	cmpq	40(%rdx), %rcx
	jne	LBB3_26
## %bb.19:
	movq	%r15, %r9
	jmp	LBB3_23
LBB3_15:
	movl	$16, %eax
	jmp	LBB3_26
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
__ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE: ## @_ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	subq	$216, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%r9, -64(%rbp)                  ## 8-byte Spill
	movq	%rcx, -80(%rbp)                 ## 8-byte Spill
	movq	%rdx, -72(%rbp)                 ## 8-byte Spill
	cmpq	$9, %rsi
	jb	LBB4_11
## %bb.1:
	movq	%r8, %rbx
	movq	%rsi, %r12
	movq	%rdi, %rdx
	movl	$8, %r13d
	xorl	%esi, %esi
	movq	%r12, -48(%rbp)                 ## 8-byte Spill
	movq	%rdi, -88(%rbp)                 ## 8-byte Spill
	movq	%r8, -56(%rbp)                  ## 8-byte Spill
	jmp	LBB4_4
	.p2align	4
LBB4_2:                                 ##   in Loop: Header=BB4_4 Depth=1
	movq	-96(%rbp), %rsi                 ## 8-byte Reload
	leaq	(%rsi,%rsi,2), %rax
	shlq	$4, %rax
	movups	-152(%rbp), %xmm0
	movups	-136(%rbp), %xmm1
	movups	-120(%rbp), %xmm2
	movq	24(%rbp), %rcx
	movups	%xmm2, 32(%rcx,%rax)
	movups	%xmm1, 16(%rcx,%rax)
	movups	%xmm0, (%rcx,%rax)
	incq	%rsi
	movq	-88(%rbp), %rdx                 ## 8-byte Reload
LBB4_3:                                 ##   in Loop: Header=BB4_4 Depth=1
	addq	%r14, %r13
	addq	$12, %r13
	cmpq	%r12, %r13
	jae	LBB4_11
LBB4_4:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	4(%rdx,%r13), %eax
	movl	(%rdx,%r13), %r14d
	bswapl	%r14d
	testb	$32, %al
	je	LBB4_3
## %bb.5:                               ##   in Loop: Header=BB4_4 Depth=1
	movq	%rsi, -96(%rbp)                 ## 8-byte Spill
	shll	$24, %eax
	movzbl	5(%rdx,%r13), %ecx
	shll	$16, %ecx
	orl	%eax, %ecx
	movzbl	6(%rdx,%r13), %eax
	shll	$8, %eax
	movzbl	7(%rdx,%r13), %edi
	orl	%ecx, %edi
	orl	%eax, %edi
	leaq	8(%r13), %rax
	leaq	-128(%rbp), %rcx
	xorps	%xmm0, %xmm0
	movups	%xmm0, 8(%rcx)
	movl	%edi, -152(%rbp)
	movq	%rax, -144(%rbp)
	movq	%r14, -136(%rbp)
	leaq	8(%rdx,%r13), %r15
	movq	%r15, %rsi
	movq	%r14, %rdx
	leaq	-104(%rbp), %r8
	callq	__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	testl	%eax, %eax
	jne	LBB4_12
## %bb.6:                               ##   in Loop: Header=BB4_4 Depth=1
	cmpb	$1, -128(%rbp)
	jne	LBB4_2
## %bb.7:                               ##   in Loop: Header=BB4_4 Depth=1
	movabsq	$64424509440, %rax              ## imm = 0xF00000000
	movq	%rax, -184(%rbp)
	movq	%rbx, %rdi
	movq	-104(%rbp), %rbx
	movq	%r14, %r12
	subq	%rbx, %r12
	movq	%r12, -176(%rbp)
	movq	-72(%rbp), %rax                 ## 8-byte Reload
	movq	32(%rbp), %rcx
	subq	64(%rcx), %rax
	movq	%rax, -168(%rbp)
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	subq	72(%rcx), %rax
	movq	%rax, -160(%rbp)
	leaq	-184(%rbp), %rsi
	callq	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	movq	32(%rbp), %rcx
	movl	%eax, 4(%rcx)
	testl	%eax, %eax
	jne	LBB4_15
## %bb.8:                               ##   in Loop: Header=BB4_4 Depth=1
	addq	%rbx, %r15
	movq	64(%rcx), %r8
	movq	16(%rbp), %r9
	subq	%r8, %r9
	addq	-64(%rbp), %r8                  ## 8-byte Folded Reload
	movl	$1, (%rsp)
	leaq	-240(%rbp), %rdi
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	%rbx, %rsi
	movq	%r15, %rdx
	movq	%r12, %rcx
	callq	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	movq	32(%rbp), %rdx
	movl	-236(%rbp), %eax
	movl	%eax, 4(%rdx)
	movq	-200(%rbp), %rax
	addq	%rax, 72(%rdx)
	movl	-240(%rbp), %ecx
	cmpl	$2, %ecx
	jne	LBB4_13
## %bb.9:                               ##   in Loop: Header=BB4_4 Depth=1
	cmpq	%r12, -232(%rbp)
	jne	LBB4_16
## %bb.10:                              ##   in Loop: Header=BB4_4 Depth=1
	movq	64(%rdx), %rax
	movq	%rax, -120(%rbp)
	movq	-224(%rbp), %rcx
	movq	%rcx, -112(%rbp)
	addq	%rax, %rcx
	movq	%rcx, 64(%rdx)
	movq	-48(%rbp), %r12                 ## 8-byte Reload
	jmp	LBB4_2
LBB4_11:
	xorl	%eax, %eax
LBB4_12:
	addq	$216, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB4_13:
	movl	$15, %eax
	cmpl	$3, %ecx
	jne	LBB4_12
LBB4_15:
	movl	$16, %eax
	jmp	LBB4_12
LBB4_16:
	movl	$14, %eax
	jmp	LBB4_12
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
__ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm: ## @_ZN9gui_forms5image12_GLOBAL__N_114metadata_fieldEjNSt3__14spanIKSt4byteLm18446744073709551615EEERbRm
	.cfi_startproc
## %bb.0:
	movb	$0, (%rcx)
	movq	$0, (%r8)
	cmpl	$1950701683, %edi               ## imm = 0x74455873
	jg	LBB5_13
## %bb.1:
	cmpl	$1766015824, %edi               ## imm = 0x69434350
	je	LBB5_3
## %bb.2:
	cmpl	$1767135348, %edi               ## imm = 0x69545874
	je	LBB5_3
	jmp	LBB5_15
LBB5_13:
	cmpl	$2052348020, %edi               ## imm = 0x7A545874
	je	LBB5_3
## %bb.14:
	cmpl	$1950701684, %edi               ## imm = 0x74455874
	jne	LBB5_15
LBB5_3:
	cmpq	$80, %rdx
	movl	$80, %r10d
	cmovbq	%rdx, %r10
	movl	$11, %eax
	testq	%rdx, %rdx
	je	LBB5_16
## %bb.4:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movb	$1, %r11b
	movl	$1, %r9d
	.p2align	4
LBB5_5:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	-1(%rsi,%r9), %ebx
	testb	%bl, %bl
	je	LBB5_6
## %bb.17:                              ##   in Loop: Header=BB5_5 Depth=1
	cmpb	$32, %bl
	jb	LBB5_34
## %bb.18:                              ##   in Loop: Header=BB5_5 Depth=1
	leal	-127(%rbx), %r14d
	cmpb	$34, %r14b
	jb	LBB5_34
## %bb.19:                              ##   in Loop: Header=BB5_5 Depth=1
	cmpb	$32, %bl
	sete	%bl
	testb	%bl, %r11b
	jne	LBB5_34
## %bb.20:                              ##   in Loop: Header=BB5_5 Depth=1
	movl	%ebx, %r11d
	cmpq	%r9, %r10
	leaq	1(%r9), %r9
	jne	LBB5_5
LBB5_34:
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB5_15:
	xorl	%eax, %eax
LBB5_16:
	retq
LBB5_6:
	cmpq	$1, %r9
	sete	%r10b
	orb	%r10b, %r11b
	testb	$1, %r11b
	jne	LBB5_34
## %bb.7:
	movq	%r9, (%r8)
	cmpl	$2052348020, %edi               ## imm = 0x7A545874
	je	LBB5_10
## %bb.8:
	cmpl	$1950701684, %edi               ## imm = 0x74455874
	je	LBB5_33
## %bb.9:
	cmpl	$1766015824, %edi               ## imm = 0x69434350
	jne	LBB5_21
LBB5_10:
	cmpq	%r9, %rdx
	je	LBB5_34
## %bb.11:
	cmpb	$0, (%rsi,%r9)
	jne	LBB5_34
## %bb.12:
	incq	%r9
	movq	%r9, (%r8)
	movb	$1, (%rcx)
LBB5_33:
	xorl	%eax, %eax
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB5_21:
	movq	%rdx, %rdi
	subq	%r9, %rdi
	cmpq	$2, %rdi
	jb	LBB5_34
## %bb.22:
	movzbl	(%rsi,%r9), %edi
	cmpb	$1, %dil
	ja	LBB5_34
## %bb.23:
	cmpb	$0, 1(%rsi,%r9)
	jne	LBB5_34
## %bb.24:
	cmpb	$1, %dil
	sete	(%rcx)
	addq	$2, %r9
	movq	%r9, (%r8)
	cmpq	%rdx, %r9
	jae	LBB5_27
LBB5_25:                                ## =>This Inner Loop Header: Depth=1
	cmpb	$0, (%rsi,%r9)
	je	LBB5_27
## %bb.26:                              ##   in Loop: Header=BB5_25 Depth=1
	incq	%r9
	movq	%r9, (%r8)
	cmpq	%r9, %rdx
	jne	LBB5_25
	jmp	LBB5_34
LBB5_27:
	cmpq	%rdx, %r9
	je	LBB5_34
## %bb.28:
	incq	%r9
	movq	%r9, (%r8)
	cmpq	%rdx, %r9
	jae	LBB5_31
LBB5_29:                                ## =>This Inner Loop Header: Depth=1
	cmpb	$0, (%rsi,%r9)
	je	LBB5_31
## %bb.30:                              ##   in Loop: Header=BB5_29 Depth=1
	incq	%r9
	movq	%r9, (%r8)
	cmpq	%r9, %rdx
	jne	LBB5_29
	jmp	LBB5_34
LBB5_31:
	cmpq	%rdx, %r9
	je	LBB5_34
## %bb.32:
	incq	%r9
	movq	%r9, (%r8)
	jmp	LBB5_33
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
__ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE: ## @_ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movzbl	(%rdi), %r11d
	shll	$24, %r11d
	movzbl	1(%rdi), %eax
	shll	$16, %eax
	movzbl	2(%rdi), %ecx
	shll	$8, %ecx
	movzbl	3(%rdi), %r8d
	orl	%eax, %r8d
	orl	%ecx, %r8d
	orl	%r11d, %r8d
	movl	%r8d, (%rdx)
	movzbl	4(%rdi), %ebx
	shll	$24, %ebx
	movzbl	5(%rdi), %eax
	shll	$16, %eax
	movzbl	6(%rdi), %ecx
	shll	$8, %ecx
	movzbl	7(%rdi), %r9d
	orl	%eax, %r9d
	orl	%ecx, %r9d
	orl	%ebx, %r9d
	movl	%r9d, 4(%rdx)
	movzbl	8(%rdi), %ecx
	movl	%ecx, 8(%rdx)
	movzbl	9(%rdi), %r15d
	movl	%r15d, 12(%rdx)
	movzbl	12(%rdi), %r10d
	testb	%r10b, %r10b
	setne	16(%rdx)
	movl	$7, %eax
	cmpq	$6, %r15
	ja	LBB6_18
## %bb.1:
	leaq	LJTI6_0(%rip), %r14
	movslq	(%r14,%r15,4), %r12
	addq	%r14, %r12
	jmpq	*%r12
LBB6_2:
	movl	$1, %r14d
	movl	%r14d, 284(%rdx)
	cmpl	$8, %ecx
	jne	LBB6_9
	jmp	LBB6_12
LBB6_3:
	movl	$1, 284(%rdx)
	leal	-1(%rcx), %r14d
	movl	%ecx, %r15d
	xorl	%r14d, %r15d
	cmpl	%r14d, %r15d
	jbe	LBB6_18
## %bb.4:
	movl	%ecx, %r14d
	andl	$15, %r14d
	je	LBB6_18
## %bb.5:
	movl	$1, %r14d
	jmp	LBB6_12
LBB6_6:
	movl	$3, %r14d
	movl	%r14d, 284(%rdx)
	cmpl	$8, %ecx
	jne	LBB6_9
	jmp	LBB6_12
LBB6_7:
	movl	$2, %r14d
	movl	%r14d, 284(%rdx)
	cmpl	$8, %ecx
	jne	LBB6_9
	jmp	LBB6_12
LBB6_8:
	movl	$4, %r14d
	movl	%r14d, 284(%rdx)
	cmpl	$8, %ecx
	je	LBB6_12
LBB6_9:
	cmpl	$16, %ecx
	je	LBB6_12
## %bb.10:
	leal	-3(%rcx), %r12d
	cmpb	$-2, %r12b
	setb	%r12b
	cmpl	$4, %ecx
	setne	%r13b
	testb	%r15b, %r15b
	jne	LBB6_18
## %bb.11:
	andb	%r12b, %r13b
	jne	LBB6_18
LBB6_12:
	testl	%r8d, %r8d
	je	LBB6_18
## %bb.13:
	orl	%r11d, %ebx
	js	LBB6_18
## %bb.14:
	testl	%r9d, %r9d
	je	LBB6_18
## %bb.15:
	cmpb	$0, 10(%rdi)
	jne	LBB6_18
## %bb.16:
	cmpb	$0, 11(%rdi)
	jne	LBB6_18
## %bb.17:
	cmpb	$1, 12(%rdi)
	ja	LBB6_18
## %bb.19:
	movl	40(%rsi), %eax
	cmpl	%eax, %r8d
	ja	LBB6_39
## %bb.20:
	cmpl	%eax, %r9d
	ja	LBB6_39
## %bb.21:
	movl	%r8d, %edi
	movl	%r9d, %eax
	imulq	%rdi, %rax
	movq	%rax, %rdi
	shrq	$61, %rdi
	jne	LBB6_39
## %bb.22:
	cmpq	8(%rsi), %rax
	ja	LBB6_39
## %bb.23:
	shlq	$2, %rax
	movq	%rax, 24(%rdx)
	imull	%ecx, %r14d
	shrl	$3, %r14d
	cmpl	$1, %r14d
	adcl	$0, %r14d
	movl	%r14d, 288(%rdx)
	movl	%ecx, %eax
	movq	%rax, -104(%rbp)                ## 8-byte Spill
	movq	16(%rsi), %rsi
	leaq	56(%rdx), %rax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	movq	32(%rdx), %rax
	movq	%rax, -72(%rbp)                 ## 8-byte Spill
	movq	40(%rdx), %rcx
	xorl	%edi, %edi
	movq	%rsi, -56(%rbp)                 ## 8-byte Spill
	movq	%rsi, %rax
	shrq	$32, %rax
	movq	%rax, -96(%rbp)                 ## 8-byte Spill
	xorl	%r11d, %r11d
	movq	%rdx, -88(%rbp)                 ## 8-byte Spill
LBB6_24:                                ## =>This Inner Loop Header: Depth=1
	testb	%r10b, %r10b
	je	LBB6_29
## %bb.25:                              ##   in Loop: Header=BB6_24 Depth=1
	leaq	l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x(%rip), %rax
	movl	(%r11,%rax), %r12d
	leaq	l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y(%rip), %rax
	movl	(%r11,%rax), %ebx
	leaq	l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x(%rip), %rax
	movl	(%r11,%rax), %r14d
	leaq	l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y(%rip), %rax
	movl	(%r11,%rax), %eax
	movl	%eax, -44(%rbp)                 ## 4-byte Spill
	cmpl	%r12d, %r8d
	ja	LBB6_26
	jmp	LBB6_31
LBB6_29:                                ##   in Loop: Header=BB6_24 Depth=1
	movl	$1, -44(%rbp)                   ## 4-byte Folded Spill
	movl	$1, %r14d
	xorl	%ebx, %ebx
	xorl	%r12d, %r12d
	cmpl	%r12d, %r8d
	jbe	LBB6_31
LBB6_26:                                ##   in Loop: Header=BB6_24 Depth=1
	cmpl	%ebx, %r9d
	jbe	LBB6_31
## %bb.27:                              ##   in Loop: Header=BB6_24 Depth=1
	movl	%r12d, %eax
	notl	%eax
	addl	%r8d, %eax
	movq	%rdx, %r15
	xorl	%edx, %edx
	divl	%r14d
	movl	%eax, %esi
	movl	%ebx, %eax
	notl	%eax
	addl	%r9d, %eax
	xorl	%edx, %edx
	divl	-44(%rbp)                       ## 4-byte Folded Reload
	movl	%eax, %r13d
	incl	%r13d
	movl	284(%r15), %r15d
	cmpq	$0, -96(%rbp)                   ## 8-byte Folded Reload
	movq	%r13, -120(%rbp)                ## 8-byte Spill
	je	LBB6_32
## %bb.28:                              ##   in Loop: Header=BB6_24 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	xorl	%edx, %edx
	divq	%r13
	jmp	LBB6_33
LBB6_31:                                ##   in Loop: Header=BB6_24 Depth=1
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	jmp	LBB6_36
LBB6_32:                                ##   in Loop: Header=BB6_24 Depth=1
	movq	-56(%rbp), %rax                 ## 8-byte Reload
                                        ## kill: def $eax killed $eax killed $rax
	xorl	%edx, %edx
	divl	%r13d
                                        ## kill: def $eax killed $eax def $rax
LBB6_33:                                ##   in Loop: Header=BB6_24 Depth=1
	incl	%esi
	imulq	-104(%rbp), %r15                ## 8-byte Folded Reload
	imulq	%rsi, %r15
	addq	$7, %r15
	shrq	$3, %r15
	cmpq	%rax, %r15
	jae	LBB6_39
## %bb.34:                              ##   in Loop: Header=BB6_24 Depth=1
	movq	%rdi, -112(%rbp)                ## 8-byte Spill
	leaq	1(%r15), %rax
	movq	%rax, %rdx
	movq	-120(%rbp), %rdi                ## 8-byte Reload
	imulq	%rdi, %rdx
	movq	-56(%rbp), %r13                 ## 8-byte Reload
	subq	%rcx, %r13
	cmpq	%r13, %rdx
	ja	LBB6_39
## %bb.35:                              ##   in Loop: Header=BB6_24 Depth=1
	addq	%rdx, %rcx
	movq	-88(%rbp), %rdx                 ## 8-byte Reload
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	movq	%rcx, 40(%rdx)
	addq	%rax, %rax
	movq	-72(%rbp), %r13                 ## 8-byte Reload
	cmpq	%rax, %r13
	cmovbeq	%rax, %r13
	movq	%r13, -72(%rbp)                 ## 8-byte Spill
	movq	%r13, 32(%rdx)
	movl	280(%rdx), %eax
	shlq	$5, %rax
	movq	-80(%rbp), %r13                 ## 8-byte Reload
	movl	%r12d, (%r13,%rax)
	movl	%ebx, 4(%r13,%rax)
	movl	%r14d, 8(%r13,%rax)
	movl	-44(%rbp), %ecx                 ## 4-byte Reload
	movl	%ecx, 12(%r13,%rax)
	movl	%esi, 16(%r13,%rax)
	movl	%edi, 20(%r13,%rax)
	movq	%r15, 24(%r13,%rax)
	incl	280(%rdx)
	movq	-112(%rbp), %rdi                ## 8-byte Reload
LBB6_36:                                ##   in Loop: Header=BB6_24 Depth=1
	xorl	%eax, %eax
	testb	%r10b, %r10b
	je	LBB6_18
## %bb.37:                              ##   in Loop: Header=BB6_24 Depth=1
	addq	$4, %r11
	cmpq	$6, %rdi
	leaq	1(%rdi), %rdi
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	jb	LBB6_24
	jmp	LBB6_18
LBB6_39:
	movl	$2, %eax
LBB6_18:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
	.p2align	2
	.data_region jt32
L6_0_set_2 = LBB6_2-LJTI6_0
L6_0_set_18 = LBB6_18-LJTI6_0
L6_0_set_6 = LBB6_6-LJTI6_0
L6_0_set_3 = LBB6_3-LJTI6_0
L6_0_set_7 = LBB6_7-LJTI6_0
L6_0_set_8 = LBB6_8-LJTI6_0
LJTI6_0:
	.long	L6_0_set_2
	.long	L6_0_set_18
	.long	L6_0_set_6
	.long	L6_0_set_3
	.long	L6_0_set_7
	.long	L6_0_set_18
	.long	L6_0_set_8
	.end_data_region
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj8EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI7_0:
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj8EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj8EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB7_15
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r9
	imull	12(%rsi), %edx
	leaq	(,%r8,4), %rax
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r9), %rdi
	leaq	(,%rdx,8), %r9
	leaq	(%rdi,%rdx,8), %rsi
	leaq	(%rcx,%rax), %r10
	cmpq	%r10, %rsi
	jae	LBB7_6
## %bb.2:
	addq	%r8, %rdx
	leaq	(%rdi,%rdx,8), %rdx
	cmpq	%rdx, %rcx
	jae	LBB7_6
## %bb.3:
	xorl	%edx, %edx
LBB7_4:
	leaq	(%r9,%rdi), %rsi
	addq	$6, %rsi
	.p2align	4
LBB7_5:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%rdx), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, -6(%rsi,%rdx,2)
	movzbl	1(%rcx,%rdx), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, -4(%rsi,%rdx,2)
	movzbl	2(%rcx,%rdx), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, -2(%rsi,%rdx,2)
	movzbl	3(%rcx,%rdx), %edi
	movl	%edi, %r8d
	shll	$8, %r8d
	orl	%edi, %r8d
	movw	%r8w, (%rsi,%rdx,2)
	addq	$4, %rdx
	cmpq	%rdx, %rax
	jne	LBB7_5
	jmp	LBB7_14
LBB7_6:
	cmpl	$4, %r8d
	jae	LBB7_8
## %bb.7:
	xorl	%edx, %edx
	jmp	LBB7_12
LBB7_8:
	movq	%rax, %rdx
	andq	$-16, %rdx
	leaq	(%r9,%rdi), %r10
	addq	$16, %r10
	xorl	%r11d, %r11d
	.p2align	4
LBB7_9:                                 ## =>This Inner Loop Header: Depth=1
	movq	(%rcx,%r11), %xmm0              ## xmm0 = mem[0],zero
	movq	8(%rcx,%r11), %xmm1             ## xmm1 = mem[0],zero
	punpcklbw	%xmm0, %xmm0            ## xmm0 = xmm0[0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7]
	punpcklbw	%xmm1, %xmm1            ## xmm1 = xmm1[0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7]
	movdqu	%xmm0, -16(%r10,%r11,2)
	movdqu	%xmm1, (%r10,%r11,2)
	addq	$16, %r11
	cmpq	%r11, %rdx
	jne	LBB7_9
## %bb.10:
	cmpq	%rdx, %rax
	je	LBB7_14
## %bb.11:
	testb	$3, %r8b
	je	LBB7_4
LBB7_12:
	pxor	%xmm0, %xmm0
	movdqa	LCPI7_0(%rip), %xmm1            ## xmm1 = [257,257,257,257,u,u,u,u]
	.p2align	4
LBB7_13:                                ## =>This Inner Loop Header: Depth=1
	movd	(%rcx,%rdx), %xmm2              ## xmm2 = mem[0],zero,zero,zero
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pmullw	%xmm1, %xmm2
	movq	%xmm2, (%rsi,%rdx,2)
	addq	$4, %rdx
	cmpq	%rdx, %rax
	jne	LBB7_13
LBB7_14:
	popq	%rbp
LBB7_15:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj16EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj16EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj16EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB8_12
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	16(%rbp), %r9
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	(,%r8,4), %rax
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r9), %rsi
	leaq	(,%rdx,8), %rdi
	cmpl	$4, %r8d
	jb	LBB8_2
## %bb.3:
	leaq	(%rsi,%rdx,8), %rdx
	leaq	(%rsi,%rdi), %r9
	leaq	(%rcx,%r8,8), %r10
	cmpq	%r10, %rdx
	jae	LBB8_6
## %bb.4:
	leaq	(%r9,%r8,8), %rdx
	cmpq	%rdx, %rcx
	jae	LBB8_6
LBB8_2:
	xorl	%edx, %edx
LBB8_9:
	addq	%rdi, %rsi
	addq	$2, %rsi
	.p2align	4
LBB8_10:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rdx,2), %edi
	rolw	$8, %di
	movw	%di, -2(%rsi,%rdx,2)
	movzwl	2(%rcx,%rdx,2), %edi
	rolw	$8, %di
	movw	%di, (%rsi,%rdx,2)
	addq	$2, %rdx
	cmpq	%rdx, %rax
	jne	LBB8_10
LBB8_11:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
LBB8_12:
	xorl	%eax, %eax
	retq
LBB8_6:
	movq	%rdi, -48(%rbp)                 ## 8-byte Spill
	movq	%rsi, -56(%rbp)                 ## 8-byte Spill
	movq	%rax, %rdx
	andq	$-8, %rdx
	shlq	$3, %r8
	andq	$-16, %r8
	xorl	%r10d, %r10d
	.p2align	4
LBB8_7:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%r10), %r11d
	movzbl	2(%rcx,%r10), %ebx
	movzbl	4(%rcx,%r10), %r14d
	movzbl	8(%rcx,%r10), %r15d
	movzbl	12(%rcx,%r10), %r12d
	movzbl	10(%rcx,%r10), %r13d
	shll	$8, %r13d
	orl	%r15d, %r13d
	shll	$8, %ebx
	orl	%r11d, %ebx
	shll	$16, %r14d
	orl	%ebx, %r14d
	movzbl	6(%rcx,%r10), %r11d
	shll	$24, %r11d
	orl	%r14d, %r11d
	movd	%r11d, %xmm0
	pinsrw	$2, %r13d, %xmm0
	movzbl	14(%rcx,%r10), %r11d
	shll	$8, %r11d
	orl	%r12d, %r11d
	pinsrw	$3, %r11d, %xmm0
	pxor	%xmm1, %xmm1
	punpcklbw	%xmm0, %xmm1            ## xmm1 = xmm1[0],xmm0[0],xmm1[1],xmm0[1],xmm1[2],xmm0[2],xmm1[3],xmm0[3],xmm1[4],xmm0[4],xmm1[5],xmm0[5],xmm1[6],xmm0[6],xmm1[7],xmm0[7]
	movzbl	1(%rcx,%r10), %r11d
	movzbl	3(%rcx,%r10), %ebx
	movzbl	5(%rcx,%r10), %r14d
	movzbl	7(%rcx,%r10), %r15d
	movzbl	9(%rcx,%r10), %r12d
	movzbl	11(%rcx,%r10), %r13d
	movzbl	13(%rcx,%r10), %edi
	movzbl	15(%rcx,%r10), %esi
	shll	$16, %ebx
	orl	%r11d, %ebx
	movd	%ebx, %xmm0
	pinsrw	$2, %r14d, %xmm0
	pinsrw	$3, %r15d, %xmm0
	pinsrw	$4, %r12d, %xmm0
	pinsrw	$5, %r13d, %xmm0
	pinsrw	$6, %edi, %xmm0
	pinsrw	$7, %esi, %xmm0
	por	%xmm1, %xmm0
	movdqu	%xmm0, (%r9,%r10)
	addq	$16, %r10
	cmpq	%r10, %r8
	jne	LBB8_7
## %bb.8:
	cmpq	%rdx, %rax
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
	movq	-48(%rbp), %rdi                 ## 8-byte Reload
	je	LBB8_11
	jmp	LBB8_9
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r9d
	testq	%r9, %r9
	je	LBB9_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	16(%rbp), %r10
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	2344(%rdi), %r8d
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r10), %rdx
	cmpl	$1, %r9d
	jne	LBB9_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB9_4
LBB9_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r9d, %r10d
	andl	$-2, %r10d
	leaq	(%rdx,%rdi,2), %r11
	addq	$6, %r11
	xorl	%esi, %esi
	.p2align	4
LBB9_8:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%rsi), %ebx
	xorl	%r14d, %r14d
	cmpl	%ebx, %r8d
	setne	%r14b
	negl	%r14d
	movl	%ebx, %r15d
	shll	$8, %r15d
	orl	%ebx, %r15d
	movw	%r15w, (%rdx,%rax,2)
	movw	%r15w, 2(%rdx,%rax,2)
	movw	%r15w, 4(%rdx,%rax,2)
	movw	%r14w, 6(%rdx,%rax,2)
	movzbl	1(%rcx,%rsi), %ebx
	xorl	%r14d, %r14d
	cmpl	%ebx, %r8d
	setne	%r14b
	negl	%r14d
	movl	%ebx, %r15d
	shll	$8, %r15d
	orl	%ebx, %r15d
	movw	%r15w, -6(%r11,%rax,2)
	movw	%r15w, -4(%r11,%rax,2)
	movw	%r15w, -2(%r11,%rax,2)
	movw	%r14w, (%r11,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r10
	jne	LBB9_8
## %bb.3:
	testb	$1, %r9b
	je	LBB9_5
LBB9_4:
	movzbl	(%rcx,%rsi), %ecx
	xorl	%esi, %esi
	cmpl	%ecx, %r8d
	setne	%sil
	negl	%esi
	movl	%ecx, %edi
	shll	$8, %edi
	orl	%ecx, %edi
	movw	%di, (%rdx,%rax,2)
	movw	%di, 2(%rdx,%rax,2)
	movw	%di, 4(%rdx,%rax,2)
	movw	%si, 6(%rdx,%rax,2)
LBB9_5:
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
LBB9_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB10_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r10
	movl	8(%rsi), %r8d
	shll	$2, %r8d
	movl	2344(%rdi), %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r10), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r8, %r8
	xorl	%esi, %esi
	.p2align	4
LBB10_2:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rsi,2), %edi
	rolw	$8, %di
	movzwl	%di, %edi
	xorl	%r10d, %r10d
	cmpl	%r9d, %edi
	setne	%r10b
	negl	%r10d
	movw	%di, -6(%rdx)
	movw	%di, -4(%rdx)
	movw	%di, -2(%rdx)
	movw	%r10w, (%rdx)
	incq	%rsi
	addq	%r8, %rdx
	cmpq	%rsi, %rax
	jne	LBB10_2
## %bb.3:
	popq	%rbp
LBB10_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB11_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r10
	movl	8(%rsi), %r8d
	shll	$2, %r8d
	movl	2344(%rdi), %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r10), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r8, %r8
	xorl	%esi, %esi
	.p2align	4
LBB11_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%rsi, %rdi
	shrq	$3, %rdi
	movzbl	(%rcx,%rdi), %edi
	movl	%esi, %r10d
	notb	%r10b
	movzbl	%r10b, %r10d
	andl	$7, %r10d
	xorl	%r11d, %r11d
	btl	%r10d, %edi
	setb	%r11b
	xorl	%edi, %edi
	cmpl	%r9d, %r11d
	setne	%dil
	negl	%edi
	negl	%r11d
	movw	%r11w, -6(%rdx)
	movw	%r11w, -4(%rdx)
	movw	%r11w, -2(%rdx)
	movw	%di, (%rdx)
	incq	%rsi
	addq	%r8, %rdx
	cmpq	%rsi, %rax
	jne	LBB11_2
## %bb.3:
	popq	%rbp
LBB11_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB12_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	shll	$2, %r9d
	imull	12(%rsi), %edx
	movl	2344(%rdi), %r10d
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%rcx), %rcx
	leaq	(%rcx,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r9, %r9
	xorl	%esi, %esi
	xorl	%edi, %edi
	.p2align	4
LBB12_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%rdi, %rcx
	shrq	$2, %rcx
	movzbl	(%rax,%rcx), %r11d
	movl	%esi, %ecx
	notb	%cl
	andb	$6, %cl
	shrl	%cl, %r11d
	andl	$3, %r11d
	xorl	%ecx, %ecx
	cmpl	%r10d, %r11d
	setne	%cl
	negl	%ecx
	imull	$21845, %r11d, %r11d            ## imm = 0x5555
	movw	%r11w, -6(%rdx)
	movw	%r11w, -4(%rdx)
	movw	%r11w, -2(%rdx)
	movw	%cx, (%rdx)
	incq	%rdi
	addq	%r9, %rdx
	addl	$2, %esi
	cmpq	%rdi, %r8
	jne	LBB12_2
## %bb.3:
	popq	%rbp
LBB12_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB13_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	shll	$2, %r9d
	imull	12(%rsi), %edx
	movl	2344(%rdi), %r10d
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%rcx), %rcx
	leaq	(%rcx,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r9, %r9
	xorl	%esi, %esi
	xorl	%edi, %edi
	.p2align	4
LBB13_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%rdi, %rcx
	shrq	%rcx
	movzbl	(%rax,%rcx), %r11d
	movl	%esi, %ecx
	notb	%cl
	andb	$4, %cl
	shrl	%cl, %r11d
	andl	$15, %r11d
	xorl	%ecx, %ecx
	cmpl	%r10d, %r11d
	setne	%cl
	negl	%ecx
	imull	$4369, %r11d, %r11d             ## imm = 0x1111
	movw	%r11w, -6(%rdx)
	movw	%r11w, -4(%rdx)
	movw	%r11w, -2(%rdx)
	movw	%cx, (%rdx)
	incq	%rdi
	addq	%r9, %rdx
	addl	$4, %esi
	cmpq	%rdi, %r8
	jne	LBB13_2
## %bb.3:
	popq	%rbp
LBB13_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI14_0:
	.space	2
	.short	65535                           ## 0xffff
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB14_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	16(%rbp), %r9
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	cmpl	$1, %r8d
	jne	LBB14_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB14_4
LBB14_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r8d, %r9d
	andl	$-2, %r9d
	leaq	(%rdx,%rdi,2), %r10
	xorl	%esi, %esi
	.p2align	4
LBB14_8:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%rsi), %r11d
	movl	%r11d, %ebx
	shll	$8, %ebx
	orl	%r11d, %ebx
	pcmpeqd	%xmm0, %xmm0
	pinsrw	$0, %ebx, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%rdx,%rax,2)
	movzbl	1(%rcx,%rsi), %r11d
	movl	%r11d, %ebx
	shll	$8, %ebx
	orl	%r11d, %ebx
	pcmpeqd	%xmm0, %xmm0
	pinsrw	$0, %ebx, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%r10,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r9
	jne	LBB14_8
## %bb.3:
	testb	$1, %r8b
	je	LBB14_5
LBB14_4:
	movzbl	(%rcx,%rsi), %ecx
	movl	%ecx, %esi
	shll	$8, %esi
	orl	%ecx, %esi
	movdqa	LCPI14_0(%rip), %xmm0           ## xmm0 = [u,65535,u,u,u,u,u,u]
	pinsrw	$0, %esi, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%rdx,%rax,2)
LBB14_5:
	popq	%rbx
	popq	%rbp
LBB14_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB15_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	16(%rbp), %r9
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	cmpl	$1, %r8d
	jne	LBB15_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB15_4
LBB15_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r8d, %r9d
	andl	$-2, %r9d
	leaq	(%rdx,%rdi,2), %r10
	addq	$6, %r10
	xorl	%esi, %esi
	.p2align	4
LBB15_8:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rsi,2), %r11d
	rolw	$8, %r11w
	movw	%r11w, (%rdx,%rax,2)
	movw	%r11w, 2(%rdx,%rax,2)
	movw	%r11w, 4(%rdx,%rax,2)
	movw	$-1, 6(%rdx,%rax,2)
	movzwl	2(%rcx,%rsi,2), %r11d
	rolw	$8, %r11w
	movw	%r11w, -6(%r10,%rax,2)
	movw	%r11w, -4(%r10,%rax,2)
	movw	%r11w, -2(%r10,%rax,2)
	movw	$-1, (%r10,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r9
	jne	LBB15_8
## %bb.3:
	testb	$1, %r8b
	je	LBB15_5
LBB15_4:
	movzwl	(%rcx,%rsi,2), %ecx
	rolw	$8, %cx
	movw	%cx, (%rdx,%rax,2)
	movw	%cx, 2(%rdx,%rax,2)
	movw	%cx, 4(%rdx,%rax,2)
	movw	$-1, 6(%rdx,%rax,2)
LBB15_5:
	popq	%rbp
LBB15_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI16_0:
	.space	2
	.short	65535                           ## 0xffff
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB16_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	16(%rbp), %r9
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	xorl	%edi, %edi
	cmpl	$1, %r8d
	jne	LBB16_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB16_4
LBB16_7:
	movl	8(%rsi), %r9d
	shll	$2, %r9d
	movl	%r8d, %r10d
	andl	$-2, %r10d
	leaq	(%rdx,%r9,2), %r11
	xorl	%esi, %esi
	.p2align	4
LBB16_8:                                ## =>This Inner Loop Header: Depth=1
	movl	%esi, %ebx
	andl	$6, %ebx
	movl	%ebx, %r14d
	xorl	$7, %r14d
	movq	%rsi, %r15
	shrq	$3, %r15
	movzbl	(%rcx,%r15), %r12d
	btl	%r14d, %r12d
	movl	$0, %r14d
	sbbl	%r14d, %r14d
	pcmpeqd	%xmm0, %xmm0
	pinsrw	$0, %r14d, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%rdx,%rax,2)
	xorl	$6, %ebx
	movzbl	(%rcx,%r15), %r14d
	btl	%ebx, %r14d
	movl	$0, %ebx
	sbbl	%ebx, %ebx
	pcmpeqd	%xmm0, %xmm0
	pinsrw	$0, %ebx, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%r11,%rax,2)
	addq	%r9, %rax
	addq	%r9, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r10
	jne	LBB16_8
## %bb.3:
	testb	$1, %r8b
	je	LBB16_5
LBB16_4:
	movl	%esi, %r8d
	notl	%r8d
	andl	$7, %r8d
	shrq	$3, %rsi
	movzbl	(%rcx,%rsi), %ecx
	btl	%r8d, %ecx
	sbbl	%edi, %edi
	movdqa	LCPI16_0(%rip), %xmm0           ## xmm0 = [u,65535,u,u,u,u,u,u]
	pinsrw	$0, %edi, %xmm0
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	movq	%xmm0, (%rdx,%rax,2)
LBB16_5:
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
LBB16_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI17_0:
	.space	2
	.short	65535                           ## 0xffff
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB17_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	shll	$2, %r9d
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	shlq	$3, %rdx
	addq	(%rcx), %rdx
	addq	%r9, %r9
	xorl	%esi, %esi
	movdqa	LCPI17_0(%rip), %xmm0           ## xmm0 = [u,65535,u,u,u,u,u,u]
	xorl	%edi, %edi
	.p2align	4
LBB17_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%rdi, %rcx
	shrq	$2, %rcx
	movzbl	(%rax,%rcx), %r10d
	movl	%esi, %ecx
	notb	%cl
	andb	$6, %cl
	shrl	%cl, %r10d
	andl	$3, %r10d
	imull	$21845, %r10d, %ecx             ## imm = 0x5555
	movdqa	%xmm0, %xmm1
	pinsrw	$0, %ecx, %xmm1
	pshuflw	$64, %xmm1, %xmm1               ## xmm1 = xmm1[0,0,0,1,4,5,6,7]
	movq	%xmm1, (%rdx)
	incq	%rdi
	addq	%r9, %rdx
	addl	$2, %esi
	cmpq	%rdi, %r8
	jne	LBB17_2
## %bb.3:
	popq	%rbp
LBB17_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI18_0:
	.space	2
	.short	65535                           ## 0xffff
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB18_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	shll	$2, %r9d
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	shlq	$3, %rdx
	addq	(%rcx), %rdx
	addq	%r9, %r9
	xorl	%esi, %esi
	movdqa	LCPI18_0(%rip), %xmm0           ## xmm0 = [u,65535,u,u,u,u,u,u]
	xorl	%edi, %edi
	.p2align	4
LBB18_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%rdi, %rcx
	shrq	%rcx
	movzbl	(%rax,%rcx), %r10d
	movl	%esi, %ecx
	notb	%cl
	andb	$4, %cl
	shrl	%cl, %r10d
	andl	$15, %r10d
	imull	$4369, %r10d, %ecx              ## imm = 0x1111
	movdqa	%xmm0, %xmm1
	pinsrw	$0, %ecx, %xmm1
	pshuflw	$64, %xmm1, %xmm1               ## xmm1 = xmm1[0,0,0,1,4,5,6,7]
	movq	%xmm1, (%rdx)
	incq	%rdi
	addq	%r9, %rdx
	addl	$4, %esi
	cmpq	%rdi, %r8
	jne	LBB18_2
## %bb.3:
	popq	%rbp
LBB18_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r11d
	testq	%r11, %r11
	je	LBB19_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	16(%rbp), %rbx
	movl	8(%rsi), %eax
	shll	$2, %eax
	movl	2344(%rdi), %r8d
	movl	2348(%rdi), %r9d
	imull	12(%rsi), %edx
	movl	2352(%rdi), %r10d
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%rbx), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%rax, %rax
	leaq	(%r11,%r11,2), %rsi
	xorl	%edi, %edi
	.p2align	4
LBB19_2:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%rdi), %r14d
	movzbl	1(%rcx,%rdi), %r11d
	movzbl	%r11b, %r15d
	xorl	%r9d, %r15d
	movzbl	2(%rcx,%rdi), %ebx
	movzbl	%bl, %r12d
	xorl	%r10d, %r12d
	movl	%r8d, %r13d
	xorl	%r14d, %r13d
	orl	%r15d, %r13d
	xorl	%r15d, %r15d
	orl	%r12d, %r13d
	setne	%r15b
	movl	%r14d, %r12d
	shll	$8, %r12d
	orl	%r14d, %r12d
	movw	%r12w, -6(%rdx)
	movl	%r11d, %r14d
	shll	$8, %r14d
	addl	%r11d, %r14d
	movw	%r14w, -4(%rdx)
	movl	%ebx, %r11d
	shll	$8, %r11d
	addl	%ebx, %r11d
	movw	%r11w, -2(%rdx)
	negl	%r15d
	movw	%r15w, (%rdx)
	addq	%rax, %rdx
	addq	$3, %rdi
	cmpq	%rdi, %rsi
	jne	LBB19_2
## %bb.3:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
LBB19_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r11d
	testq	%r11, %r11
	je	LBB20_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	leaq	16(%rbp), %rbx
	movl	8(%rsi), %eax
	shll	$2, %eax
	movl	2344(%rdi), %r8d
	movl	2348(%rdi), %r9d
	movl	2352(%rdi), %r10d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%rbx), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%rax, %rax
	addq	%r11, %r11
	leaq	(%r11,%r11,2), %rsi
	xorl	%edi, %edi
	.p2align	4
LBB20_2:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rdi), %r11d
	movzwl	2(%rcx,%rdi), %ebx
	rolw	$8, %r11w
	rolw	$8, %bx
	movzwl	%r11w, %r11d
	movzwl	4(%rcx,%rdi), %r14d
	rolw	$8, %r14w
	movzwl	%bx, %ebx
	movzwl	%r14w, %r14d
	movl	%r11d, %r15d
	xorl	%r8d, %r15d
	movl	%ebx, %r12d
	xorl	%r9d, %r12d
	orl	%r15d, %r12d
	movl	%r14d, %r15d
	xorl	%r10d, %r15d
	xorl	%r13d, %r13d
	orl	%r12d, %r15d
	movw	%r11w, -6(%rdx)
	movw	%bx, -4(%rdx)
	movw	%r14w, -2(%rdx)
	setne	%r13b
	negl	%r13d
	movw	%r13w, (%rdx)
	addq	%rax, %rdx
	addq	$6, %rdi
	cmpq	%rdi, %rsi
	jne	LBB20_2
## %bb.3:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
LBB20_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI21_0:
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB21_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	16(%rbp), %r9
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	cmpl	$1, %r8d
	jne	LBB21_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB21_4
LBB21_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r8d, %r9d
	andl	$-2, %r9d
	leaq	(%rdx,%rdi,2), %r10
	addq	$6, %r10
	leaq	5(%rcx), %r11
	xorl	%esi, %esi
	pxor	%xmm0, %xmm0
	movdqa	LCPI21_0(%rip), %xmm1           ## xmm1 = [257,257,u,u,u,u,u,u]
	.p2align	4
LBB21_8:                                ## =>This Inner Loop Header: Depth=1
	movzbl	-3(%r11), %ebx
	movzwl	-5(%r11), %r14d
	movd	%r14d, %xmm2
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pmullw	%xmm1, %xmm2
	movd	%xmm2, (%rdx,%rax,2)
	movl	%ebx, %r14d
	shll	$8, %r14d
	orl	%ebx, %r14d
	movw	%r14w, 4(%rdx,%rax,2)
	movw	$-1, 6(%rdx,%rax,2)
	movzbl	(%r11), %ebx
	movzwl	-2(%r11), %r14d
	movd	%r14d, %xmm2
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pmullw	%xmm1, %xmm2
	movd	%xmm2, -6(%r10,%rax,2)
	movl	%ebx, %r14d
	shll	$8, %r14d
	orl	%ebx, %r14d
	movw	%r14w, -2(%r10,%rax,2)
	movw	$-1, (%r10,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	addq	$6, %r11
	cmpq	%rsi, %r9
	jne	LBB21_8
## %bb.3:
	testb	$1, %r8b
	je	LBB21_5
LBB21_4:
	leaq	(%rsi,%rsi,2), %rsi
	movzbl	2(%rcx,%rsi), %edi
	movzwl	(%rcx,%rsi), %ecx
	movd	%ecx, %xmm0
	pxor	%xmm1, %xmm1
	punpcklbw	%xmm1, %xmm0            ## xmm0 = xmm0[0],xmm1[0],xmm0[1],xmm1[1],xmm0[2],xmm1[2],xmm0[3],xmm1[3],xmm0[4],xmm1[4],xmm0[5],xmm1[5],xmm0[6],xmm1[6],xmm0[7],xmm1[7]
	pmullw	LCPI21_0(%rip), %xmm0           ## [257,257,u,u,u,u,u,u]
	movd	%xmm0, (%rdx,%rax,2)
	movl	%edi, %ecx
	shll	$8, %ecx
	orl	%edi, %ecx
	movw	%cx, 4(%rdx,%rax,2)
	movw	$-1, 6(%rdx,%rax,2)
LBB21_5:
	popq	%rbx
	popq	%r14
	popq	%rbp
LBB21_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI22_0:
	.short	256                             ## 0x100
	.short	256                             ## 0x100
	.short	256                             ## 0x100
	.short	1                               ## 0x1
	.space	2
	.space	2
	.space	2
	.space	2
LCPI22_1:
	.short	256                             ## 0x100
	.short	256                             ## 0x100
	.short	256                             ## 0x100
	.short	1                               ## 0x1
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB22_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r9
	movl	8(%rsi), %eax
	imull	12(%rsi), %edx
	shll	$2, %eax
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	shlq	$3, %rdx
	addq	(%r9), %rdx
	addq	%rax, %rax
	addq	%r8, %r8
	leaq	(%r8,%r8,2), %rsi
	xorl	%edi, %edi
	movl	$65535, %r8d                    ## imm = 0xFFFF
	movq	LCPI22_1(%rip), %xmm0           ## xmm0 = [256,256,256,1,0,0,0,0]
	.p2align	4
LBB22_2:                                ## =>This Inner Loop Header: Depth=1
	movzbl	2(%rcx,%rdi), %r9d
	movzbl	(%rcx,%rdi), %r10d
	movd	%r10d, %xmm1
	pinsrw	$1, %r9d, %xmm1
	movzbl	4(%rcx,%rdi), %r9d
	pinsrw	$2, %r9d, %xmm1
	movzbl	1(%rcx,%rdi), %r9d
	pinsrw	$3, %r8d, %xmm1
	pmullw	%xmm0, %xmm1
	movzbl	3(%rcx,%rdi), %r10d
	movd	%r9d, %xmm2
	pinsrw	$1, %r10d, %xmm2
	movzbl	5(%rcx,%rdi), %r9d
	pinsrw	$2, %r9d, %xmm2
	por	%xmm1, %xmm2
	movq	%xmm2, (%rdx)
	addq	%rax, %rdx
	addq	$6, %rdi
	cmpq	%rdi, %rsi
	jne	LBB22_2
## %bb.3:
	popq	%rbp
LBB22_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB23_1
## %bb.3:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r9
	movl	8(%rsi), %r8d
	imull	12(%rsi), %edx
	shll	$2, %r8d
	addl	4(%rsi), %edx
	movl	(%rdi), %r10d
	imulq	%rdx, %r10
	movl	(%rsi), %esi
	addq	%r10, %rsi
	movl	2340(%rdi), %edx
	movq	(%r9), %r9
	leaq	(%r9,%rsi,8), %rsi
	addq	$6, %rsi
	addq	%r8, %r8
	xorl	%r9d, %r9d
	.p2align	4
LBB23_4:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rcx,%r9), %r10d
	cmpl	%r10d, %edx
	jbe	LBB23_5
## %bb.6:                               ##   in Loop: Header=BB23_4 Depth=1
	movzwl	292(%rdi,%r10,8), %r11d
	movw	%r11w, -6(%rsi)
	movzwl	294(%rdi,%r10,8), %r11d
	movw	%r11w, -4(%rsi)
	movzwl	296(%rdi,%r10,8), %r11d
	movw	%r11w, -2(%rsi)
	movzwl	298(%rdi,%r10,8), %r10d
	movw	%r10w, (%rsi)
	incq	%r9
	addq	%r8, %rsi
	cmpq	%r9, %rax
	jne	LBB23_4
## %bb.7:
	xorl	%eax, %eax
	popq	%rbp
	retq
LBB23_1:
	xorl	%eax, %eax
	retq
LBB23_5:
	movl	$19, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB24_1
## %bb.3:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	leaq	16(%rbp), %r9
	movl	8(%rsi), %r8d
	imull	12(%rsi), %edx
	shll	$2, %r8d
	addl	4(%rsi), %edx
	movl	(%rdi), %r10d
	imulq	%rdx, %r10
	movl	(%rsi), %esi
	addq	%r10, %rsi
	movl	2340(%rdi), %edx
	movq	(%r9), %r9
	leaq	(%r9,%rsi,8), %rsi
	addq	$6, %rsi
	addq	%r8, %r8
	xorl	%r9d, %r9d
	.p2align	4
LBB24_4:                                ## =>This Inner Loop Header: Depth=1
	movq	%r9, %r10
	shrq	$3, %r10
	movzbl	(%rcx,%r10), %r11d
	movl	%r9d, %r10d
	notb	%r10b
	movzbl	%r10b, %ebx
	andl	$7, %ebx
	xorl	%r10d, %r10d
	btl	%ebx, %r11d
	setb	%r10b
	cmpl	%edx, %r10d
	jae	LBB24_5
## %bb.6:                               ##   in Loop: Header=BB24_4 Depth=1
	movl	%r10d, %r10d
	movzwl	292(%rdi,%r10,8), %r11d
	movw	%r11w, -6(%rsi)
	movzwl	294(%rdi,%r10,8), %r11d
	movw	%r11w, -4(%rsi)
	movzwl	296(%rdi,%r10,8), %r11d
	movw	%r11w, -2(%rsi)
	movzwl	298(%rdi,%r10,8), %r10d
	movw	%r10w, (%rsi)
	incq	%r9
	addq	%r8, %rsi
	cmpq	%r9, %rax
	jne	LBB24_4
## %bb.7:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rbp
	retq
LBB24_1:
	xorl	%eax, %eax
	retq
LBB24_5:
	movl	$19, %eax
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB25_1
## %bb.3:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	shll	$2, %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %r10d
	imulq	%rdx, %r10
	movl	(%rsi), %esi
	addq	%r10, %rsi
	movl	2340(%rdi), %edx
	movq	(%rcx), %rcx
	leaq	(%rcx,%rsi,8), %rsi
	addq	$6, %rsi
	addq	%r9, %r9
	xorl	%r10d, %r10d
	xorl	%r11d, %r11d
	.p2align	4
LBB25_4:                                ## =>This Inner Loop Header: Depth=1
	movq	%r11, %rcx
	shrq	$2, %rcx
	movzbl	(%rax,%rcx), %ebx
	movl	%r10d, %ecx
	notb	%cl
	andb	$6, %cl
	shrl	%cl, %ebx
	andl	$3, %ebx
	cmpl	%edx, %ebx
	jae	LBB25_5
## %bb.6:                               ##   in Loop: Header=BB25_4 Depth=1
	movl	%ebx, %ecx
	movzwl	292(%rdi,%rcx,8), %ebx
	movw	%bx, -6(%rsi)
	movzwl	294(%rdi,%rcx,8), %ebx
	movw	%bx, -4(%rsi)
	movzwl	296(%rdi,%rcx,8), %ebx
	movw	%bx, -2(%rsi)
	movzwl	298(%rdi,%rcx,8), %ecx
	movw	%cx, (%rsi)
	incq	%r11
	addq	%r9, %rsi
	addl	$2, %r10d
	cmpq	%r11, %r8
	jne	LBB25_4
## %bb.7:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rbp
	retq
LBB25_1:
	xorl	%eax, %eax
	retq
LBB25_5:
	movl	$19, %eax
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB26_1
## %bb.3:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movq	%rcx, %rax
	leaq	16(%rbp), %rcx
	movl	8(%rsi), %r9d
	shll	$2, %r9d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %r10d
	imulq	%rdx, %r10
	movl	(%rsi), %esi
	addq	%r10, %rsi
	movl	2340(%rdi), %edx
	movq	(%rcx), %rcx
	leaq	(%rcx,%rsi,8), %rsi
	addq	$6, %rsi
	addq	%r9, %r9
	xorl	%r10d, %r10d
	xorl	%r11d, %r11d
	.p2align	4
LBB26_4:                                ## =>This Inner Loop Header: Depth=1
	movq	%r11, %rcx
	shrq	%rcx
	movzbl	(%rax,%rcx), %ebx
	movl	%r10d, %ecx
	notb	%cl
	andb	$4, %cl
	shrl	%cl, %ebx
	andl	$15, %ebx
	cmpl	%edx, %ebx
	jae	LBB26_5
## %bb.6:                               ##   in Loop: Header=BB26_4 Depth=1
	movl	%ebx, %ecx
	movzwl	292(%rdi,%rcx,8), %ebx
	movw	%bx, -6(%rsi)
	movzwl	294(%rdi,%rcx,8), %ebx
	movw	%bx, -4(%rsi)
	movzwl	296(%rdi,%rcx,8), %ebx
	movw	%bx, -2(%rsi)
	movzwl	298(%rdi,%rcx,8), %ecx
	movw	%cx, (%rsi)
	incq	%r11
	addq	%r9, %rsi
	addl	$4, %r10d
	cmpq	%r11, %r8
	jne	LBB26_4
## %bb.7:
	xorl	%eax, %eax
	popq	%rbx
	popq	%rbp
	retq
LBB26_1:
	xorl	%eax, %eax
	retq
LBB26_5:
	movl	$19, %eax
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI27_0:
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB27_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	16(%rbp), %r9
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	cmpl	$1, %r8d
	jne	LBB27_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB27_4
LBB27_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r8d, %r9d
	andl	$-2, %r9d
	leaq	(%rdx,%rdi,2), %r10
	xorl	%esi, %esi
	pxor	%xmm0, %xmm0
	movdqa	LCPI27_0(%rip), %xmm1           ## xmm1 = [257,257,257,257,u,u,u,u]
	.p2align	4
LBB27_8:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rsi,2), %r11d
	movd	%r11d, %xmm2
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pshuflw	$64, %xmm2, %xmm2               ## xmm2 = xmm2[0,0,0,1,4,5,6,7]
	pmullw	%xmm1, %xmm2
	movq	%xmm2, (%rdx,%rax,2)
	movzwl	2(%rcx,%rsi,2), %r11d
	movd	%r11d, %xmm2
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pshuflw	$64, %xmm2, %xmm2               ## xmm2 = xmm2[0,0,0,1,4,5,6,7]
	pmullw	%xmm1, %xmm2
	movq	%xmm2, (%r10,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r9
	jne	LBB27_8
## %bb.3:
	testb	$1, %r8b
	je	LBB27_5
LBB27_4:
	movzwl	(%rcx,%rsi,2), %ecx
	movd	%ecx, %xmm0
	pxor	%xmm1, %xmm1
	punpcklbw	%xmm1, %xmm0            ## xmm0 = xmm0[0],xmm1[0],xmm0[1],xmm1[1],xmm0[2],xmm1[2],xmm0[3],xmm1[3],xmm0[4],xmm1[4],xmm0[5],xmm1[5],xmm0[6],xmm1[6],xmm0[7],xmm1[7]
	pshuflw	$64, %xmm0, %xmm0               ## xmm0 = xmm0[0,0,0,1,4,5,6,7]
	pmullw	LCPI27_0(%rip), %xmm0           ## [257,257,257,257,u,u,u,u]
	movq	%xmm0, (%rdx,%rax,2)
LBB27_5:
	popq	%rbp
LBB27_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB28_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r9
	movl	8(%rsi), %r8d
	shll	$2, %r8d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r9), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r8, %r8
	xorl	%esi, %esi
	.p2align	4
LBB28_2:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rsi,4), %edi
	movzwl	2(%rcx,%rsi,4), %r9d
	rolw	$8, %di
	rolw	$8, %r9w
	movw	%di, -6(%rdx)
	movw	%di, -4(%rdx)
	movw	%di, -2(%rdx)
	movw	%r9w, (%rdx)
	incq	%rsi
	addq	%r8, %rdx
	cmpq	%rsi, %rax
	jne	LBB28_2
## %bb.3:
	popq	%rbp
LBB28_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
LCPI29_0:
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.short	257                             ## 0x101
	.space	2
	.space	2
	.space	2
	.space	2
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %r8d
	testq	%r8, %r8
	je	LBB29_6
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	leaq	16(%rbp), %r9
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %eax
	addq	%rdi, %rax
	shlq	$2, %rax
	movq	(%r9), %rdx
	cmpl	$1, %r8d
	jne	LBB29_7
## %bb.2:
	xorl	%esi, %esi
	jmp	LBB29_4
LBB29_7:
	movl	8(%rsi), %edi
	shll	$2, %edi
	movl	%r8d, %r9d
	andl	$-2, %r9d
	leaq	(%rdx,%rdi,2), %r10
	xorl	%esi, %esi
	pxor	%xmm0, %xmm0
	movdqa	LCPI29_0(%rip), %xmm1           ## xmm1 = [257,257,257,257,u,u,u,u]
	.p2align	4
LBB29_8:                                ## =>This Inner Loop Header: Depth=1
	movd	(%rcx,%rsi,4), %xmm2            ## xmm2 = mem[0],zero,zero,zero
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pmullw	%xmm1, %xmm2
	movq	%xmm2, (%rdx,%rax,2)
	movd	4(%rcx,%rsi,4), %xmm2           ## xmm2 = mem[0],zero,zero,zero
	punpcklbw	%xmm0, %xmm2            ## xmm2 = xmm2[0],xmm0[0],xmm2[1],xmm0[1],xmm2[2],xmm0[2],xmm2[3],xmm0[3],xmm2[4],xmm0[4],xmm2[5],xmm0[5],xmm2[6],xmm0[6],xmm2[7],xmm0[7]
	pmullw	%xmm1, %xmm2
	movq	%xmm2, (%r10,%rax,2)
	addq	%rdi, %rax
	addq	%rdi, %rax
	addq	$2, %rsi
	cmpq	%rsi, %r9
	jne	LBB29_8
## %bb.3:
	testb	$1, %r8b
	je	LBB29_5
LBB29_4:
	movd	(%rcx,%rsi,4), %xmm0            ## xmm0 = mem[0],zero,zero,zero
	pxor	%xmm1, %xmm1
	punpcklbw	%xmm1, %xmm0            ## xmm0 = xmm0[0],xmm1[0],xmm0[1],xmm1[1],xmm0[2],xmm1[2],xmm0[3],xmm1[3],xmm0[4],xmm1[4],xmm0[5],xmm1[5],xmm0[6],xmm1[6],xmm0[7],xmm1[7]
	pmullw	LCPI29_0(%rip), %xmm0           ## [257,257,257,257,u,u,u,u]
	movq	%xmm0, (%rdx,%rax,2)
LBB29_5:
	popq	%rbp
LBB29_6:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE: ## @_ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.cfi_startproc
## %bb.0:
                                        ## kill: def $edx killed $edx def $rdx
	movl	16(%rsi), %eax
	testq	%rax, %rax
	je	LBB30_4
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	leaq	16(%rbp), %r9
	movl	8(%rsi), %r8d
	shll	$2, %r8d
	imull	12(%rsi), %edx
	addl	4(%rsi), %edx
	movl	(%rdi), %edi
	imulq	%rdx, %rdi
	movl	(%rsi), %edx
	addq	%rdi, %rdx
	movq	(%r9), %rsi
	leaq	(%rsi,%rdx,8), %rdx
	addq	$6, %rdx
	addq	%r8, %r8
	xorl	%esi, %esi
	.p2align	4
LBB30_2:                                ## =>This Inner Loop Header: Depth=1
	movzwl	(%rcx,%rsi,8), %edi
	movzwl	2(%rcx,%rsi,8), %r9d
	rolw	$8, %di
	rolw	$8, %r9w
	movzwl	4(%rcx,%rsi,8), %r10d
	rolw	$8, %r10w
	movzwl	6(%rcx,%rsi,8), %r11d
	rolw	$8, %r11w
	movw	%di, -6(%rdx)
	movw	%r9w, -4(%rdx)
	movw	%r10w, -2(%rdx)
	movw	%r11w, (%rdx)
	incq	%rsi
	addq	%r8, %rdx
	cmpq	%rsi, %rax
	jne	LBB30_2
## %bb.3:
	popq	%rbp
LBB30_4:
	xorl	%eax, %eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ## @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_x:
	.long	0                               ## 0x0
	.long	4                               ## 0x4
	.long	0                               ## 0x0
	.long	2                               ## 0x2
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	0                               ## 0x0

	.p2align	2, 0x0                          ## @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.starts_y:
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	4                               ## 0x4
	.long	0                               ## 0x0
	.long	2                               ## 0x2
	.long	0                               ## 0x0
	.long	1                               ## 0x1

	.p2align	2, 0x0                          ## @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_x:
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	2                               ## 0x2
	.long	2                               ## 0x2
	.long	1                               ## 0x1

	.p2align	2, 0x0                          ## @__const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y
l___const._ZN9gui_forms5image12_GLOBAL__N_111make_layoutENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE.steps_y:
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	2                               ## 0x2
	.long	2                               ## 0x2

	.section	__DATA,__const
	.p2align	3, 0x0                          ## @switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE
l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

	.p2align	3, 0x0                          ## @switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5
l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

	.p2align	3, 0x0                          ## @switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.6
l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.6:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

.subsections_via_symbols
