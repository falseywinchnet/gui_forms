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
	subq	$2696, %rsp                     ## imm = 0xA88
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
	movq	%rdi, -80(%rbp)                 ## 8-byte Spill
	movdqu	%xmm0, 64(%rdi)
	movq	64(%rbp), %r14
	movq	72(%rbp), %r13
	leaq	(%r13,%r13), %rax
	movq	96(%rbp), %rdi
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
	testq	%rax, %rax
	je	LBB2_12
## %bb.9:
	movq	%rsi, %r15
	subq	%r14, %r15
	jbe	LBB2_11
## %bb.10:
	cmpq	%rax, %r15
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
	subq	%rdi, %r15
	jbe	LBB2_19
## %bb.18:
	cmpq	%r10, %r15
	jae	LBB2_20
	jmp	LBB2_69
LBB2_19:
	movq	%rdi, %r15
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
	testq	%rax, %rax
	je	LBB2_28
## %bb.25:
	movq	%rcx, %r15
	subq	%r14, %r15
	jbe	LBB2_27
## %bb.26:
	cmpq	%rax, %r15
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
	subq	%rdi, %r15
	jbe	LBB2_35
## %bb.34:
	cmpq	%r10, %r15
	jae	LBB2_36
	jmp	LBB2_69
LBB2_35:
	movq	%rdi, %r15
	subq	%rcx, %r15
	cmpq	$38192, %r15                    ## imm = 0x9530
	jb	LBB2_69
LBB2_36:
	testq	%r9, %r9
	je	LBB2_49
## %bb.37:
	testq	%rax, %rax
	je	LBB2_41
## %bb.38:
	movq	%r8, %r15
	subq	%r14, %r15
	jbe	LBB2_40
## %bb.39:
	cmpq	%rax, %r15
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
	subq	%rdi, %r15
	jbe	LBB2_48
## %bb.47:
	cmpq	%r10, %r15
	jae	LBB2_49
	jmp	LBB2_69
LBB2_48:
	movq	%rdi, %r15
	subq	%r8, %r15
	cmpq	%r9, %r15
	jb	LBB2_69
LBB2_49:
	testq	%rax, %rax
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
	cmpq	%rax, %r15
	jb	LBB2_69
LBB2_54:
	testq	%r10, %r10
	je	LBB2_62
## %bb.55:
	movq	%r14, %r15
	subq	%rdi, %r15
	jbe	LBB2_57
## %bb.56:
	cmpq	%r10, %r15
	jae	LBB2_58
	jmp	LBB2_69
LBB2_57:
	movq	%rdi, %r15
	subq	%r14, %r15
	cmpq	%rax, %r15
	jb	LBB2_69
LBB2_58:
	testq	%rbx, %rbx
	je	LBB2_62
## %bb.59:
	testq	%r10, %r10
	je	LBB2_62
## %bb.60:
	movq	%r11, %rax
	subq	%rdi, %rax
	jbe	LBB2_68
## %bb.61:
	cmpq	%r10, %rax
	jb	LBB2_69
LBB2_62:
	movq	%r8, -248(%rbp)                 ## 8-byte Spill
	movq	%r9, -208(%rbp)                 ## 8-byte Spill
	movq	%rcx, -216(%rbp)                ## 8-byte Spill
	leaq	16(%rbp), %rbx
	movdqa	%xmm0, -2736(%rbp)
	movb	$0, -2720(%rbp)
	movdqu	%xmm0, -2712(%rbp)
	movdqu	%xmm0, -2696(%rbp)
	movq	$0, -2680(%rbp)
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	movq	%rax, -2672(%rbp)
	movdqu	%xmm0, -2664(%rbp)
	movq	$0, -2648(%rbp)
	movq	%rax, -2640(%rbp)
	movdqu	%xmm0, -2632(%rbp)
	movq	$0, -2616(%rbp)
	movq	%rax, -2608(%rbp)
	movdqu	%xmm0, -2600(%rbp)
	movq	$0, -2584(%rbp)
	movq	%rax, -2576(%rbp)
	movdqu	%xmm0, -2568(%rbp)
	movq	$0, -2552(%rbp)
	movq	%rax, -2544(%rbp)
	movdqu	%xmm0, -2536(%rbp)
	movq	$0, -2520(%rbp)
	movq	%rax, -2512(%rbp)
	movdqu	%xmm0, -2504(%rbp)
	movq	$0, -2488(%rbp)
	movq	%rax, -2480(%rbp)
	leaq	-2472(%rbp), %rdi
	movq	%rsi, %r14
	movl	$2093, %esi                     ## imm = 0x82D
	movq	%rdx, %r15
	callq	___bzero
	pxor	%xmm0, %xmm0
	movdqu	%xmm0, -376(%rbp)
	movq	$0, -360(%rbp)
	leaq	-2736(%rbp), %rcx
	movq	%r14, -232(%rbp)                ## 8-byte Spill
	movq	%r14, %rdi
	movq	%r15, -240(%rbp)                ## 8-byte Spill
	movq	%r15, %rsi
	movq	%rbx, %r15
	movq	%rbx, %rdx
	callq	__ZN9gui_forms5image12_GLOBAL__N_15parseENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS1_4PlanE
	movl	%eax, %ecx
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.63:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	addq	$8, %rax
	movq	-2688(%rbp), %rcx
	movq	%rcx, 48(%rax)
	movdqa	-2736(%rbp), %xmm0
	movdqa	-2720(%rbp), %xmm1
	movdqa	-2704(%rbp), %xmm2
	movdqu	%xmm2, 32(%rax)
	movdqu	%xmm1, 16(%rax)
	movdqu	%xmm0, (%rax)
	movq	-208(%rbp), %rax                ## 8-byte Reload
	cmpq	-2704(%rbp), %rax
	jb	LBB2_71
## %bb.64:
	cmpq	-2712(%rbp), %r13
	jb	LBB2_71
## %bb.65:
	cmpq	-2688(%rbp), %r12
	jb	LBB2_71
## %bb.66:
	movabsq	$64424509440, %rax              ## imm = 0xF00000000
	movq	%rax, -352(%rbp)
	movq	-360(%rbp), %rax
	movq	%rax, -344(%rbp)
	movq	-2696(%rbp), %rax
	movq	%rax, -336(%rbp)
	movq	32(%r15), %rax
	movq	%rax, -328(%rbp)
	leaq	-352(%rbp), %rsi
	movq	-216(%rbp), %rdi                ## 8-byte Reload
	callq	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	movl	%eax, %ecx
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, 4(%rax)
	testl	%ecx, %ecx
	je	LBB2_72
## %bb.67:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	$16, (%rax)
	jmp	LBB2_70
LBB2_68:
	subq	%r11, %rdi
	cmpq	%rbx, %rdi
	jae	LBB2_62
LBB2_69:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	$1, (%rax)
LBB2_70:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	addq	$2696, %rsp                     ## imm = 0xA88
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB2_71:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	$15, (%rax)
	jmp	LBB2_70
LBB2_72:
	movq	-376(%rbp), %rax
	movq	%rax, -320(%rbp)
	movq	$0, -312(%rbp)
	movb	$0, -304(%rbp)
	movl	-2456(%rbp), %r14d
	testl	%r14d, %r14d
	movq	-216(%rbp), %rcx                ## 8-byte Reload
	movq	-240(%rbp), %rsi                ## 8-byte Reload
	movq	-232(%rbp), %rdi                ## 8-byte Reload
	movq	-208(%rbp), %r8                 ## 8-byte Reload
	movq	-248(%rbp), %rdx                ## 8-byte Reload
	je	LBB2_289
## %bb.73:
	leaq	-2680(%rbp), %rax
	incq	%r8
	xorl	%r10d, %r10d
	leaq	-2736(%rbp), %r12
	movq	%r8, -208(%rbp)                 ## 8-byte Spill
	jmp	LBB2_76
LBB2_74:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2456(%rbp), %r14d
	movq	-208(%rbp), %r8                 ## 8-byte Reload
	movq	-248(%rbp), %rdx                ## 8-byte Reload
LBB2_75:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	-264(%rbp), %r10                ## 8-byte Reload
	incq	%r10
	movl	%r14d, %eax
	cmpq	%rax, %r10
	leaq	-2680(%rbp), %rax
	jae	LBB2_289
LBB2_76:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB2_103 Depth 2
                                        ##       Child Loop BB2_189 Depth 3
                                        ##       Child Loop BB2_193 Depth 3
                                        ##       Child Loop BB2_197 Depth 3
                                        ##       Child Loop BB2_199 Depth 3
                                        ##       Child Loop BB2_246 Depth 3
                                        ##       Child Loop BB2_250 Depth 3
                                        ##       Child Loop BB2_256 Depth 3
                                        ##       Child Loop BB2_221 Depth 3
                                        ##       Child Loop BB2_225 Depth 3
                                        ##       Child Loop BB2_230 Depth 3
                                        ##       Child Loop BB2_261 Depth 3
                                        ##       Child Loop BB2_265 Depth 3
                                        ##       Child Loop BB2_237 Depth 3
                                        ##       Child Loop BB2_209 Depth 3
                                        ##       Child Loop BB2_213 Depth 3
                                        ##       Child Loop BB2_217 Depth 3
                                        ##       Child Loop BB2_219 Depth 3
                                        ##       Child Loop BB2_156 Depth 3
                                        ##       Child Loop BB2_268 Depth 3
                                        ##       Child Loop BB2_271 Depth 3
                                        ##       Child Loop BB2_179 Depth 3
                                        ##       Child Loop BB2_184 Depth 3
                                        ##       Child Loop BB2_187 Depth 3
                                        ##       Child Loop BB2_171 Depth 3
                                        ##       Child Loop BB2_278 Depth 3
                                        ##       Child Loop BB2_281 Depth 3
                                        ##       Child Loop BB2_145 Depth 3
                                        ##       Child Loop BB2_150 Depth 3
                                        ##       Child Loop BB2_152 Depth 3
                                        ##       Child Loop BB2_163 Depth 3
                                        ##       Child Loop BB2_273 Depth 3
                                        ##       Child Loop BB2_276 Depth 3
                                        ##       Child Loop BB2_111 Depth 3
                                        ##       Child Loop BB2_115 Depth 3
                                        ##       Child Loop BB2_117 Depth 3
	movq	%r10, -264(%rbp)                ## 8-byte Spill
	shlq	$5, %r10
	leaq	(%rax,%r10), %r9
	movq	%r9, -224(%rbp)                 ## 8-byte Spill
	movl	-2724(%rbp), %r9d
	cmpl	$6, %r9d
	jne	LBB2_79
## %bb.77:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$1, 8(%rax,%r10)
	jne	LBB2_79
## %bb.78:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$8, -2728(%rbp)
	leaq	__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj16EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	leaq	__ZN9gui_forms5image12_GLOBAL__N_111expand_rgbaILj8EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	cmoveq	%rax, %r9
	movq	%r9, -64(%rbp)                  ## 8-byte Spill
	jmp	LBB2_99
LBB2_79:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	$0, -64(%rbp)                   ## 8-byte Folded Spill
	cmpl	$6, %r9d
	ja	LBB2_99
## %bb.80:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%r9d, %eax
	leaq	LJTI2_0(%rip), %r9
	movslq	(%r9,%rax,4), %rax
	addq	%r9, %rax
	jmpq	*%rax
LBB2_81:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2728(%rbp), %r9d
	cmpb	$1, -380(%rbp)
	jne	LBB2_93
## %bb.82:                              ##   in Loop: Header=BB2_76 Depth=1
	leal	-1(%r9), %eax
	movl	%r9d, %r10d
	xorl	%eax, %r10d
	cmpl	%eax, %r10d
	jbe	LBB2_285
## %bb.83:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %eax
	cmpl	$5, %eax
	jae	LBB2_285
## %bb.84:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%eax, %eax
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE(%rip), %r9
	jmp	LBB2_96
LBB2_85:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2728(%rbp), %r9d
	leal	-1(%r9), %eax
	movl	%r9d, %r10d
	xorl	%eax, %r10d
	cmpl	%eax, %r10d
	jbe	LBB2_285
## %bb.86:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %eax
	cmpl	$4, %eax
	jae	LBB2_285
## %bb.87:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%eax, %eax
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5(%rip), %r9
	jmp	LBB2_96
LBB2_88:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2728(%rbp), %r9d
	cmpb	$1, -380(%rbp)
	jne	LBB2_97
## %bb.89:                              ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$16, %r9d
	movl	$0, %r10d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	cmoveq	%rax, %r10
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb1EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	jmp	LBB2_98
LBB2_90:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2728(%rbp), %eax
	cmpl	$16, %eax
	movl	$0, %r10d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	cmoveq	%r9, %r10
	cmpl	$8, %eax
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj4ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	jmp	LBB2_98
LBB2_91:                                ##   in Loop: Header=BB2_76 Depth=1
	movl	-2728(%rbp), %eax
	cmpl	$16, %eax
	movl	$0, %r10d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %r9
	cmoveq	%r9, %r10
	cmpl	$8, %eax
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj6ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	jmp	LBB2_98
LBB2_93:                                ##   in Loop: Header=BB2_76 Depth=1
	leal	-1(%r9), %eax
	movl	%r9d, %r10d
	xorl	%eax, %r10d
	cmpl	%eax, %r10d
	jbe	LBB2_285
## %bb.94:                              ##   in Loop: Header=BB2_76 Depth=1
	rep		bsfl	%r9d, %eax
	cmpl	$5, %eax
	jae	LBB2_285
## %bb.95:                              ##   in Loop: Header=BB2_76 Depth=1
	movl	%eax, %eax
	leaq	l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.4(%rip), %r9
LBB2_96:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	(%r9,%rax,8), %rax
	movq	%rax, -64(%rbp)                 ## 8-byte Spill
	jmp	LBB2_99
LBB2_285:                               ##   in Loop: Header=BB2_76 Depth=1
	movq	$0, -64(%rbp)                   ## 8-byte Folded Spill
	jmp	LBB2_99
LBB2_97:                                ##   in Loop: Header=BB2_76 Depth=1
	cmpl	$16, %r9d
	movl	$0, %r10d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
	cmoveq	%rax, %r10
	cmpl	$8, %r9d
	leaq	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj2ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE(%rip), %rax
LBB2_98:                                ##   in Loop: Header=BB2_76 Depth=1
	cmoveq	%rax, %r10
	movq	%r10, -64(%rbp)                 ## 8-byte Spill
LBB2_99:                                ##   in Loop: Header=BB2_76 Depth=1
	movq	-224(%rbp), %rax                ## 8-byte Reload
	movq	24(%rax), %rax
	leaq	1(%rax), %rbx
	cmpq	$-1, %rbx
	movq	%rbx, %r9
	cmoveq	%r8, %r9
	leaq	(%rdx,%rax), %r13
	incq	%r13
	testq	%r9, %r9
	jle	LBB2_101
## %bb.100:                             ##   in Loop: Header=BB2_76 Depth=1
	movq	%r13, %rdi
	movq	%r9, %rsi
	movq	%r13, -104(%rbp)                ## 8-byte Spill
	movq	%r9, %r13
	callq	___bzero
	movq	%r13, %r9
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-248(%rbp), %rdx                ## 8-byte Reload
	movq	-208(%rbp), %r8                 ## 8-byte Reload
	movq	-232(%rbp), %rdi                ## 8-byte Reload
	movq	-240(%rbp), %rsi                ## 8-byte Reload
	movq	-216(%rbp), %rcx                ## 8-byte Reload
LBB2_101:                               ##   in Loop: Header=BB2_76 Depth=1
	movq	-224(%rbp), %rax                ## 8-byte Reload
	cmpl	$0, 20(%rax)
	je	LBB2_75
## %bb.102:                             ##   in Loop: Header=BB2_76 Depth=1
	xorl	%r14d, %r14d
LBB2_103:                               ##   Parent Loop BB2_76 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB2_189 Depth 3
                                        ##       Child Loop BB2_193 Depth 3
                                        ##       Child Loop BB2_197 Depth 3
                                        ##       Child Loop BB2_199 Depth 3
                                        ##       Child Loop BB2_246 Depth 3
                                        ##       Child Loop BB2_250 Depth 3
                                        ##       Child Loop BB2_256 Depth 3
                                        ##       Child Loop BB2_221 Depth 3
                                        ##       Child Loop BB2_225 Depth 3
                                        ##       Child Loop BB2_230 Depth 3
                                        ##       Child Loop BB2_261 Depth 3
                                        ##       Child Loop BB2_265 Depth 3
                                        ##       Child Loop BB2_237 Depth 3
                                        ##       Child Loop BB2_209 Depth 3
                                        ##       Child Loop BB2_213 Depth 3
                                        ##       Child Loop BB2_217 Depth 3
                                        ##       Child Loop BB2_219 Depth 3
                                        ##       Child Loop BB2_156 Depth 3
                                        ##       Child Loop BB2_268 Depth 3
                                        ##       Child Loop BB2_271 Depth 3
                                        ##       Child Loop BB2_179 Depth 3
                                        ##       Child Loop BB2_184 Depth 3
                                        ##       Child Loop BB2_187 Depth 3
                                        ##       Child Loop BB2_171 Depth 3
                                        ##       Child Loop BB2_278 Depth 3
                                        ##       Child Loop BB2_281 Depth 3
                                        ##       Child Loop BB2_145 Depth 3
                                        ##       Child Loop BB2_150 Depth 3
                                        ##       Child Loop BB2_152 Depth 3
                                        ##       Child Loop BB2_163 Depth 3
                                        ##       Child Loop BB2_273 Depth 3
                                        ##       Child Loop BB2_276 Depth 3
                                        ##       Child Loop BB2_111 Depth 3
                                        ##       Child Loop BB2_115 Depth 3
                                        ##       Child Loop BB2_117 Depth 3
	movq	%r9, -296(%rbp)                 ## 8-byte Spill
	movq	%rbx, %rax
	movq	%rdx, %rbx
	subq	$8, %rsp
	movq	%r12, %rdx
	leaq	-320(%rbp), %r8
	movq	%rbx, %r9
	pushq	-80(%rbp)                       ## 8-byte Folded Reload
	pushq	$0
	movq	%rax, -48(%rbp)                 ## 8-byte Spill
	pushq	%rax
	callq	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.104:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	%r14d, -256(%rbp)               ## 4-byte Spill
	movzbl	(%rbx), %ecx
	cmpq	$4, %rcx
	ja	LBB2_291
## %bb.105:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-1(%rax), %r15
	leaq	1(%rbx), %rax
	movq	%rax, -72(%rbp)                 ## 8-byte Spill
	leaq	1(%r13), %rdx
	movl	-2448(%rbp), %esi
	leaq	LJTI2_1(%rip), %rdi
	movslq	(%rdi,%rcx,4), %rax
	addq	%rdi, %rax
	movdqa	LCPI2_1(%rip), %xmm5            ## xmm5 = [127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127]
	movq	%r13, -104(%rbp)                ## 8-byte Spill
	movq	%rbx, -56(%rbp)                 ## 8-byte Spill
	movq	%r15, -88(%rbp)                 ## 8-byte Spill
	jmpq	*%rax
LBB2_106:                               ##   in Loop: Header=BB2_103 Depth=2
	decl	%esi
	cmpl	$7, %esi
	ja	LBB2_238
## %bb.107:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	LJTI2_2(%rip), %rcx
	movslq	(%rcx,%rsi,4), %rax
	addq	%rcx, %rax
	jmpq	*%rax
LBB2_108:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$2, %r15
	jb	LBB2_238
## %bb.109:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$1, %r8d
	cmpq	$17, %r15
	movabsq	$71776119061217280, %rsi        ## imm = 0xFF000000000000
	jb	LBB2_112
## %bb.110:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	addq	$-2, %rax
LBB2_111:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	movq	%r8, -192(%rbp)                 ## 8-byte Spill
	movq	1(%rbx,%r8), %r9
	movq	9(%rbx,%r8), %r13
	movzbl	(%rbx,%r8), %eax
	addb	%r9b, %al
	movzbl	%al, %eax
	movq	%rax, -144(%rbp)                ## 8-byte Spill
	movq	%r9, %rbx
	shrq	$8, %rbx
	movq	%r9, %r10
	shrq	$16, %r10
	movq	%r9, %rcx
	shrq	$24, %rcx
	movq	%r9, %rdx
	shrq	$32, %rdx
	movq	%r9, %r11
	shrq	$40, %r11
	movq	%r13, %rax
	shrq	$8, %rax
	leal	(%rax,%r13), %edi
	movq	%r13, %r14
	shrq	$16, %r14
	addl	%r14d, %eax
	movzbl	%al, %r15d
	shll	$16, %r15d
	movq	%r13, %r8
	shrq	$48, %r8
	movq	%r8, -96(%rbp)                  ## 8-byte Spill
	movq	%r13, %r12
	shrq	$40, %r12
	addl	%r8d, %r12d
	shlq	$48, %r12
	andq	%rsi, %r12
	orq	%r15, %r12
	movq	%r13, %rax
	shrq	$24, %rax
	addl	%eax, %r14d
	shll	$24, %r14d
	orq	%r14, %r12
	movq	%r9, %r14
	shrq	$48, %r14
	shrq	$56, %r9
	shll	$8, %edi
	movzwl	%di, %edi
	orq	%rdi, %r12
	movq	%r13, %r8
	movq	%r13, -128(%rbp)                ## 8-byte Spill
	shrq	$32, %r8
	addl	%r8d, %eax
	shlq	$32, %rax
	movabsq	$1095216660480, %r15            ## imm = 0xFF00000000
	andq	%r15, %rax
	orq	%rax, %r12
	movq	-144(%rbp), %rdi                ## 8-byte Reload
	movl	%edi, %r15d
	addl	%ebx, %r15d
	addl	%r10d, %ebx
	addl	%ecx, %r10d
	addl	%edx, %ecx
	addl	%r11d, %edx
	addl	%r14d, %r11d
	addq	%r9, %r14
	addq	%r13, %r9
	shlq	$48, %r11
	andq	%rsi, %r11
	shlq	$40, %rdx
	movabsq	$280375465082880, %rax          ## imm = 0xFF0000000000
	andq	%rax, %rdx
	orq	%r11, %rdx
	shlq	$32, %rcx
	movabsq	$1095216660480, %rax            ## imm = 0xFF00000000
	andq	%rax, %rcx
	orq	%rdx, %rcx
	shll	$24, %r10d
	orq	%rcx, %r10
	movzbl	%bl, %ebx
	shll	$16, %ebx
	shll	$8, %r15d
	movzwl	%r15w, %eax
	orq	%rdi, %rax
	orq	%r10, %rbx
	orq	%rbx, %rax
	movq	%rax, %rsi
	movq	%rax, -144(%rbp)                ## 8-byte Spill
	shrq	$8, %rsi
	shrq	$16, %rbx
	shrq	$24, %r10
	shrq	$32, %rcx
	shrq	$40, %rdx
	shrq	$48, %r11
	addq	%r10, %rsi
	movq	%rsi, -176(%rbp)                ## 8-byte Spill
	addl	%ebx, %eax
	movl	%eax, -200(%rbp)                ## 4-byte Spill
	addl	%ecx, %ebx
	addl	%edx, %r10d
	leal	(%r11,%rcx), %esi
	addq	%r14, %rdx
	addq	%r9, %r11
	movq	%r12, %rcx
	shrq	$16, %rcx
	addl	%ecx, %r9d
	movq	%r12, %r15
	shrq	$32, %r15
	addl	%r15d, %ecx
	shlq	$48, %r15
	addq	%r12, %r15
	movq	%r12, %r13
	shrq	$8, %r12
	addl	%r12d, %r14d
	shrq	$24, %r13
	addl	%r13d, %r12d
	shll	$24, %r12d
	movzbl	%r11b, %eax
	orq	%rax, %r12
	movzbl	%r9b, %eax
	shll	$16, %eax
	orq	%rax, %r12
	shll	$8, %r14d
	movzwl	%r14w, %eax
	orq	%rax, %r12
	shlq	$32, %rcx
	movabsq	$1095216660480, %rax            ## imm = 0xFF00000000
	andq	%rax, %rcx
	orq	%r12, %rcx
	movabsq	$-72057594037927936, %r12       ## imm = 0xFF00000000000000
	addl	%r8d, %r13d
	shlq	$56, %r8
	movq	-128(%rbp), %rax                ## 8-byte Reload
	movq	%rax, %r14
	shlq	$16, %r14
	addq	%r8, %r14
	movq	-96(%rbp), %rdi                 ## 8-byte Reload
	shlq	$56, %rdi
	addq	%rax, %rdi
	andq	%r12, %r14
	addq	%rdi, %r14
	movabsq	$71776119061217280, %rax        ## imm = 0xFF000000000000
	andq	%rax, %r15
	andq	%r12, %r14
	orq	%r15, %r14
	movl	-200(%rbp), %r12d               ## 4-byte Reload
	movzbl	%r12b, %eax
	shll	$16, %eax
	movq	-144(%rbp), %r9                 ## 8-byte Reload
	movzwl	%r9w, %edi
	orq	%rax, %rdi
	orq	%rcx, %r14
	shlq	$48, %rsi
	movabsq	$71776119061217280, %rax        ## imm = 0xFF000000000000
	andq	%rax, %rsi
	shlq	$40, %r10
	movabsq	$280375465082880, %rax          ## imm = 0xFF0000000000
	andq	%rax, %r10
	orq	%rsi, %r10
	shlq	$32, %rbx
	movabsq	$1095216660480, %rax            ## imm = 0xFF00000000
	andq	%rax, %rbx
	orq	%r10, %rbx
	movq	-176(%rbp), %rax                ## 8-byte Reload
	addq	%rax, %rdx
                                        ## kill: def $eax killed $eax killed $rax def $rax
	shll	$24, %eax
	orq	%rbx, %rax
	orq	%rax, %rdi
	shrq	$40, %r10
	movl	%edi, %r8d
	shrl	$8, %r8d
	addl	%r10d, %r8d
	movq	%r14, %rax
	shrq	$8, %rax
	addl	%eax, %r10d
	addl	%eax, %r13d
	shll	$8, %r10d
	movl	%r9d, %r15d
	andl	$65280, %r15d                   ## imm = 0xFF00
	addl	%r10d, %r15d
	shrq	$48, %rsi
	movl	%edi, %r9d
	shrl	$16, %r9d
	addl	%esi, %r9d
	addl	%r12d, %esi
	movq	%r14, %rax
	shrq	$16, %rax
	movq	%rax, %r10
	addl	%esi, %eax
	shrq	$32, %rbx
	addq	%rdi, %rbx
	addl	%ebx, %r11d
	movzbl	%r11b, %esi
	movabsq	$4294967297, %r11               ## imm = 0x100000001
	movzbl	%al, %eax
	shll	$16, %eax
	orq	%rsi, %rax
	imulq	%r11, %rcx
	shlq	$32, %rbx
	movq	%rbx, %rsi
	addq	%rcx, %rbx
	movabsq	$1095216660480, %rcx            ## imm = 0xFF00000000
	andq	%rcx, %rbx
	orq	%rax, %rbx
	movq	%r14, %rax
	shrq	$24, %rax
	movq	%rdx, %rcx
	addl	%eax, %edx
	shll	$24, %edx
	orq	%rdx, %rbx
	shlq	$40, %r13
	addq	-128(%rbp), %r13                ## 8-byte Folded Reload
	shlq	$40, %r8
	movzwl	%r15w, %edx
	addq	%r8, %r13
	movabsq	$280375465082880, %r11          ## imm = 0xFF0000000000
	andq	%r11, %r13
	orq	%rdx, %r13
	movabsq	$280375465082880, %rdx          ## imm = 0xFF0000000000
	shlq	$48, %r10
	shlq	$56, %rax
	shlq	$56, %rcx
	addq	%r14, %r10
	addq	%rcx, %r14
	addq	%rax, %r14
	shlq	$48, %r9
	addq	%r9, %r10
	movabsq	$71776119061217280, %rax        ## imm = 0xFF000000000000
	andq	%rax, %r10
	movabsq	$-72057594037927936, %rax       ## imm = 0xFF00000000000000
	andq	%rax, %r14
	orq	%r10, %r14
	orq	%rbx, %r14
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	orq	%r13, %r14
	movabsq	$71776119061217280, %r13        ## imm = 0xFF000000000000
	movzbl	-144(%rbp), %eax                ## 1-byte Folded Reload
	andl	$-256, %edi
	orq	%rdi, %rax
	movabsq	$1095216660480, %rdi            ## imm = 0xFF00000000
	andq	%rdi, %rsi
	orq	%rsi, %rax
	andq	%rdx, %r8
	orq	%r8, %rax
	movq	-192(%rbp), %r8                 ## 8-byte Reload
	movq	%r13, %rsi
	andq	%r13, %r9
	orq	%r9, %rax
	orq	%rcx, %rax
	movq	%rax, 1(%rbx,%r8)
	movq	-160(%rbp), %rax                ## 8-byte Reload
	movq	%r14, 9(%rbx,%r8)
	addq	$16, %r8
	addq	$-16, %rax
	cmpq	$15, %rax
	ja	LBB2_111
LBB2_112:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-88(%rbp), %r15                 ## 8-byte Reload
	cmpq	%r15, %r8
	movq	-104(%rbp), %r13                ## 8-byte Reload
	jae	LBB2_238
## %bb.113:                             ##   in Loop: Header=BB2_103 Depth=2
	movzbl	(%rbx,%r8), %ecx
	movl	%r8d, %esi
	notl	%esi
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	addl	%eax, %esi
	movq	%r8, %rdx
	andl	$3, %esi
	je	LBB2_116
## %bb.114:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-72(%rbp), %rdi                 ## 8-byte Reload
LBB2_115:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	addb	(%rdi,%rdx), %cl
	movb	%cl, (%rdi,%rdx)
	incq	%rdx
	decq	%rsi
	jne	LBB2_115
LBB2_116:                               ##   in Loop: Header=BB2_103 Depth=2
	subq	%r8, %rax
	addq	$-2, %rax
	cmpq	$3, %rax
	jb	LBB2_238
LBB2_117:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	addb	1(%rbx,%rdx), %cl
	movb	%cl, 1(%rbx,%rdx)
	addb	2(%rbx,%rdx), %cl
	movb	%cl, 2(%rbx,%rdx)
	addb	3(%rbx,%rdx), %cl
	movb	%cl, 3(%rbx,%rdx)
	addb	4(%rbx,%rdx), %cl
	movb	%cl, 4(%rbx,%rdx)
	addq	$4, %rdx
	cmpq	%rdx, %r15
	jne	LBB2_117
	jmp	LBB2_238
LBB2_118:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %r15
	movq	%rsi, %rdi
	cmovbq	%r15, %rdi
	testq	%rdi, %rdi
	leaq	1(%rbx), %r10
	je	LBB2_200
## %bb.119:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %rdi
	jb	LBB2_120
## %bb.127:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rdi,%r13), %rax
	incq	%rax
	cmpq	%rax, %r10
	jae	LBB2_136
## %bb.128:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdi), %rax
	incq	%rax
	cmpq	%rax, %rdx
	jae	LBB2_136
LBB2_120:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
LBB2_195:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %r9
	movq	%rcx, %r8
	andq	$3, %r9
	je	LBB2_198
## %bb.196:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r8
LBB2_197:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%rdx,%r8), %eax
	addb	%al, (%r10,%r8)
	incq	%r8
	decq	%r9
	jne	LBB2_197
LBB2_198:                               ##   in Loop: Header=BB2_103 Depth=2
	subq	%rdi, %rcx
	cmpq	$-4, %rcx
	ja	LBB2_200
LBB2_199:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r13,%r8), %eax
	addb	%al, 1(%rbx,%r8)
	movzbl	2(%r13,%r8), %eax
	addb	%al, 2(%rbx,%r8)
	movzbl	3(%r13,%r8), %eax
	addb	%al, 3(%rbx,%r8)
	movzbl	4(%r13,%r8), %eax
	addb	%al, 4(%rbx,%r8)
	addq	$4, %r8
	cmpq	%r8, %rdi
	jne	LBB2_199
LBB2_200:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %r15
	jbe	LBB2_238
## %bb.201:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rax
	notq	%rax
	movq	-48(%rbp), %r11                 ## 8-byte Reload
	addq	%r11, %rax
	cmpq	$4, %rax
	jae	LBB2_203
LBB2_202:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rcx
	jmp	LBB2_254
LBB2_121:                               ##   in Loop: Header=BB2_103 Depth=2
	testq	%r15, %r15
	je	LBB2_238
## %bb.122:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %r15
	jb	LBB2_123
## %bb.130:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rcx                 ## 8-byte Reload
	leaq	(%rcx,%r13), %rax
	leaq	1(%rbx), %rsi
	cmpq	%rax, %rsi
	jae	LBB2_138
## %bb.131:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rcx), %rax
	cmpq	%rax, %rdx
	jae	LBB2_138
LBB2_123:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
LBB2_215:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %r8                  ## 8-byte Reload
	movl	%r8d, %eax
	andl	$3, %eax
	movq	%rcx, %rsi
	cmpl	$1, %eax
	leaq	1(%rbx), %r9
	je	LBB2_218
## %bb.216:                             ##   in Loop: Header=BB2_103 Depth=2
	leal	-1(%r8), %edi
	andl	$3, %edi
	movq	%rcx, %rsi
LBB2_217:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%rdx,%rsi), %eax
	addb	%al, (%r9,%rsi)
	incq	%rsi
	decq	%rdi
	jne	LBB2_217
LBB2_218:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rax
	subq	%rcx, %rax
	addq	$-2, %rax
	cmpq	$3, %rax
	jb	LBB2_238
LBB2_219:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r13,%rsi), %eax
	addb	%al, 1(%rbx,%rsi)
	movzbl	2(%r13,%rsi), %eax
	addb	%al, 2(%rbx,%rsi)
	movzbl	3(%r13,%rsi), %eax
	addb	%al, 3(%rbx,%rsi)
	movzbl	4(%r13,%rsi), %eax
	addb	%al, 4(%rbx,%rsi)
	addq	$4, %rsi
	cmpq	%rsi, %r15
	jne	LBB2_219
	jmp	LBB2_238
LBB2_124:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %r15
	movq	%rsi, %rdi
	cmovbq	%r15, %rdi
	testq	%rdi, %rdi
	leaq	1(%rbx), %r9
	je	LBB2_231
## %bb.125:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %rdi
	jb	LBB2_126
## %bb.133:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rdi,%r13), %rax
	incq	%rax
	cmpq	%rax, %r9
	jae	LBB2_140
## %bb.134:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdi), %rax
	incq	%rax
	cmpq	%rax, %rdx
	jae	LBB2_140
LBB2_126:                               ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
LBB2_227:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r8
	orq	$1, %r8
	testb	$1, %dil
	je	LBB2_229
## %bb.228:                             ##   in Loop: Header=BB2_103 Depth=2
	movzbl	(%rdx,%rcx), %eax
	shrb	%al
	addb	%al, (%r9,%rcx)
	movq	%r8, %rcx
LBB2_229:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rdi
	je	LBB2_231
LBB2_230:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	1(%r13,%rcx), %eax
	shrb	%al
	addb	%al, 1(%rbx,%rcx)
	movzbl	2(%r13,%rcx), %eax
	shrb	%al
	addb	%al, 2(%rbx,%rcx)
	addq	$2, %rcx
	cmpq	%rcx, %rdi
	jne	LBB2_230
LBB2_231:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %r15
	jbe	LBB2_238
## %bb.232:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rcx
	notq	%rcx
	movq	-48(%rbp), %r14                 ## 8-byte Reload
	addq	%r14, %rcx
	cmpq	$8, %rcx
	jb	LBB2_233
## %bb.240:                             ##   in Loop: Header=BB2_103 Depth=2
	leaq	(%rbx,%rdi), %rax
	incq	%rax
	leaq	(%rbx,%r14), %r9
	movq	%rdi, %r8
	subq	%rsi, %r8
	addq	%rbx, %r8
	incq	%r8
	movq	%r14, %r10
	subq	%rsi, %r10
	addq	%rbx, %r10
	leaq	(%rdi,%r13), %r11
	incq	%r11
	leaq	(%r14,%r13), %rbx
	cmpq	%r10, %rax
	setb	%r10b
	cmpq	%r9, %r8
	setb	%r14b
	cmpq	%rbx, %rax
	setb	%r8b
	cmpq	%r9, %r11
	setb	%al
	testb	%r14b, %r10b
	jne	LBB2_259
## %bb.241:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%al, %r8b
	jne	LBB2_259
## %bb.242:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rcx
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	jae	LBB2_260
## %bb.243:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r8d, %r8d
	jmp	LBB2_264
LBB2_203:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rax, -96(%rbp)                 ## 8-byte Spill
	leaq	(%rbx,%rdi), %r8
	incq	%r8
	leaq	(%rbx,%r11), %rax
	movq	%rdi, %rcx
	subq	%rsi, %rcx
	leaq	(%rbx,%rcx), %r10
	incq	%r10
	movq	%r11, %r9
	subq	%rsi, %r9
	leaq	(%rbx,%r9), %r14
	leaq	(%rdi,%r13), %rbx
	incq	%rbx
	leaq	(%r11,%r13), %r15
	leaq	(%rcx,%r13), %r11
	incq	%r11
	addq	%r13, %r9
	cmpq	%r14, %r8
	setb	%r12b
	cmpq	%rax, %r10
	setb	%r13b
	cmpq	%r15, %r8
	setb	%cl
	cmpq	%rax, %rbx
	setb	%r14b
	cmpq	%r9, %r8
	setb	%r9b
	cmpq	%rax, %r11
	setb	%r15b
	testb	%r13b, %r12b
	jne	LBB2_252
## %bb.204:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%r14b, %cl
	jne	LBB2_252
## %bb.205:                             ##   in Loop: Header=BB2_103 Depth=2
	andb	%r15b, %r9b
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	jne	LBB2_244
## %bb.206:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$16, %rax
	jae	LBB2_245
## %bb.207:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%r14d, %r14d
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
	jmp	LBB2_249
LBB2_136:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rdi
	jae	LBB2_188
## %bb.137:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
	jmp	LBB2_192
LBB2_138:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %r15
	jae	LBB2_208
## %bb.139:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
	jmp	LBB2_212
LBB2_140:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$32, %rdi
	jae	LBB2_220
## %bb.141:                             ##   in Loop: Header=BB2_103 Depth=2
	xorl	%ecx, %ecx
	jmp	LBB2_224
LBB2_142:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$4, %r15
	jb	LBB2_238
## %bb.143:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$3, %r8d
	cmpq	$19, %r15
	jb	LBB2_147
## %bb.144:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-4(%rax), %rcx
	xorl	%r8d, %r8d
	movabsq	$1095216660480, %r14            ## imm = 0xFF00000000
LBB2_145:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	%rcx, -144(%rbp)                ## 8-byte Spill
	movq	%r8, -128(%rbp)                 ## 8-byte Spill
	movq	-72(%rbp), %r13                 ## 8-byte Reload
	movq	3(%r13,%r8), %r9
	movq	11(%r13,%r8), %rbx
	movl	%r9d, %eax
	shrl	$8, %eax
	movl	%r9d, %ecx
	movzbl	(%r13,%r8), %edx
	addb	%r9b, %dl
	addb	1(%r13,%r8), %al
	movzbl	%al, %esi
	movl	%esi, %eax
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	movq	%r9, %rdi
	movq	%r9, %rax
	shrq	$32, %rax
	movq	%r9, %r12
	movq	%r9, %r10
	shrq	$56, %r9
	addl	%eax, %esi
	addl	%r9d, %eax
	movl	%eax, -192(%rbp)                ## 4-byte Spill
	movq	%rbx, %rax
	shrq	$8, %rax
	movq	%rax, %r11
	movq	%rax, -96(%rbp)                 ## 8-byte Spill
	movq	%rbx, %r8
	shrq	$16, %r8
	addq	%r8, %r9
	shlq	$16, %r9
	movl	%r9d, %eax
	andl	$16711680, %eax                 ## imm = 0xFF0000
	movq	%r11, %r15
	shlq	$32, %r15
	addq	%rbx, %r15
	andq	%r14, %r15
	orq	%rax, %r15
	movzbl	%dl, %eax
	movq	%rax, -200(%rbp)                ## 8-byte Spill
	shrq	$24, %rdi
	shrq	$48, %r10
	leal	(%rdi,%rax), %edx
	addl	%r10d, %edi
	addl	%r11d, %r10d
	shll	$8, %r10d
	movzwl	%r10w, %eax
	orq	%rax, %r15
	shrl	$16, %ecx
	movq	-128(%rbp), %rax                ## 8-byte Reload
	addb	2(%r13,%rax), %cl
	movzbl	%cl, %r14d
	shrq	$40, %r12
	movl	%r14d, %r11d
	addl	%r12d, %r14d
	addq	%rbx, %r12
	movq	%rbx, %r13
	shrq	$24, %r13
	leaq	(%rbx,%r13), %r10
	shlq	$48, %r13
	addq	%rbx, %r13
	movabsq	$71776119061217280, %rax        ## imm = 0xFF000000000000
	andq	%rax, %r13
	movzbl	%r12b, %eax
	orq	%rax, %r13
	shlq	$24, %r10
	movq	%rbx, %rcx
	shlq	$24, %rcx
	movabsq	$-72057594037927936, %rax       ## imm = 0xFF00000000000000
	andq	%rax, %rcx
	addq	%rbx, %rcx
	andq	%rax, %rcx
	movl	%r10d, %eax
	orq	%rax, %rcx
	orq	%r13, %rcx
	orq	%r15, %rcx
	movq	%r8, %rax
	shlq	$40, %rax
	addq	%rbx, %rax
	movabsq	$280375465082880, %r13          ## imm = 0xFF0000000000
	andq	%r13, %rax
	orq	%rax, %rcx
	shlq	$32, %rsi
	movabsq	$1095216660480, %rax            ## imm = 0xFF00000000
	andq	%rax, %rsi
	movl	%edx, %r15d
	shll	$24, %r15d
	orq	%rsi, %r15
	shlq	$40, %r14
	andq	%r13, %r14
	orq	%r14, %r15
	shll	$16, %r11d
	movq	%r11, -176(%rbp)                ## 8-byte Spill
	movq	-160(%rbp), %rax                ## 8-byte Reload
	shll	$8, %eax
	movq	-200(%rbp), %r14                ## 8-byte Reload
	addq	%r14, %rax
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	leaq	(%rax,%r11), %rsi
	orq	%r15, %rsi
	movq	%rsi, %r11
	shrq	$16, %r11
	addl	%r11d, %r12d
	movzbl	%r12b, %eax
	addl	%r11d, %r9d
	andl	$16711680, %r9d                 ## imm = 0xFF0000
	orl	%eax, %r9d
	addl	%r10d, %r11d
	andl	$-16777216, %r11d               ## imm = 0xFF000000
	orl	%r9d, %r11d
	movq	%rcx, %r9
	shrq	$8, %r9
	addl	%r9d, %edx
	shll	$8, %edx
	movzwl	%dx, %eax
	orl	%eax, %r11d
	movl	%esi, %edx
	shrl	$8, %edx
	movl	-192(%rbp), %eax                ## 4-byte Reload
	addl	%eax, %edx
	addl	%eax, %r8d
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	leal	(%rax,%rdi), %r10d
	shlq	$32, %r10
	addq	%rbx, %r10
	shlq	$40, %r8
	addq	%rbx, %r8
	shlq	$56, %r9
	addq	%rcx, %r9
	movabsq	$281474976710657, %rax          ## imm = 0x1000000000001
	imulq	%rax, %rcx
	movabsq	$71776119061217280, %rbx        ## imm = 0xFF000000000000
	andq	%rbx, %rcx
	movabsq	$-72057594037927936, %r12       ## imm = 0xFF00000000000000
	andq	%r12, %r9
	orq	%rcx, %r9
	addl	%r14d, %edi
	movabsq	$1095216660480, %r14            ## imm = 0xFF00000000
	andq	%r14, %r10
	orq	%r11, %r10
	andq	%r13, %r8
	orq	%r10, %r8
	orq	%r8, %r9
	shlq	$56, %rdx
	shlq	$48, %rdi
	andq	%rbx, %rdi
	orq	%rdx, %rdi
	shlq	$32, %rsi
	movq	%rsi, %rax
	andq	%r13, %rax
	addq	%r8, %rax
	movq	-128(%rbp), %r8                 ## 8-byte Reload
	addq	%rsi, %r10
	andq	%r14, %r10
	andq	%r13, %rax
	orq	%r10, %rax
	movq	%rsi, %rcx
	andq	%rbx, %rcx
	addq	%r9, %rcx
	andq	%rbx, %rcx
	orq	%rax, %rcx
	andq	%r12, %rsi
	addq	%r9, %rsi
	andq	%r12, %rsi
	orq	%r11, %rsi
	orq	%rcx, %rsi
	movq	-144(%rbp), %rcx                ## 8-byte Reload
	addq	-176(%rbp), %rdi                ## 8-byte Folded Reload
	addq	-160(%rbp), %rdi                ## 8-byte Folded Reload
	orq	%r15, %rdi
	movq	-72(%rbp), %rax                 ## 8-byte Reload
	movq	%rdi, 3(%rax,%r8)
	movq	%rsi, 11(%rax,%r8)
	addq	$16, %r8
	addq	$-16, %rcx
	cmpq	$15, %rcx
	ja	LBB2_145
## %bb.146:                             ##   in Loop: Header=BB2_103 Depth=2
	addq	$3, %r8
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
LBB2_147:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r15, %r8
	jae	LBB2_238
## %bb.148:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	%r8d, %edx
	notl	%edx
	movq	-48(%rbp), %rsi                 ## 8-byte Reload
	addl	%esi, %edx
	movq	%r8, %rcx
	andl	$3, %edx
	movq	-72(%rbp), %rdi                 ## 8-byte Reload
	je	LBB2_151
## %bb.149:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rcx
LBB2_150:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-3(%rdi,%rcx), %eax
	addb	%al, (%rdi,%rcx)
	incq	%rcx
	decq	%rdx
	jne	LBB2_150
LBB2_151:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %rax
	subq	%r8, %rax
	addq	$-2, %rax
	cmpq	$3, %rax
	jb	LBB2_238
LBB2_152:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-2(%rbx,%rcx), %eax
	movzbl	-1(%rbx,%rcx), %edx
	addb	1(%rbx,%rcx), %al
	movb	%al, 1(%rbx,%rcx)
	addb	%dl, 2(%rbx,%rcx)
	movzbl	(%rbx,%rcx), %edx
	addb	%dl, 3(%rbx,%rcx)
	addb	%al, 4(%rbx,%rcx)
	addq	$4, %rcx
	cmpq	%rcx, %r15
	jne	LBB2_152
	jmp	LBB2_238
LBB2_153:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$9, %r15
	jb	LBB2_238
## %bb.154:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$8, %ecx
	cmpq	$24, %r15
	movq	%rbx, %rdx
	movabsq	$71776119061217280, %r11        ## imm = 0xFF000000000000
	movabsq	$280375465082880, %r15          ## imm = 0xFF0000000000
	movabsq	$1095216660480, %r14            ## imm = 0xFF00000000
	jb	LBB2_157
## %bb.155:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-9(%rax), %rbx
LBB2_156:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	1(%rdx,%rcx), %rdi
	movq	9(%rdx,%rcx), %rsi
	movl	%edi, %r8d
	shrl	$8, %r8d
	movl	%edi, %r9d
	shrl	$16, %r9d
	movzbl	-7(%rdx,%rcx), %r10d
	addb	-6(%rdx,%rcx), %r8b
	addb	%dil, %r10b
	movq	%r15, %r12
	movq	%r11, %r15
	movzbl	%r8b, %r11d
	shll	$8, %r11d
	movzbl	%r10b, %eax
	leaq	(%rax,%r11), %r8
	addl	%esi, %eax
	movzbl	%al, %eax
	addl	%esi, %r11d
	andl	$65280, %r11d                   ## imm = 0xFF00
	orl	%eax, %r11d
	movl	%edi, %eax
	shrl	$24, %eax
	addb	-5(%rdx,%rcx), %r9b
	movzbl	%r9b, %r10d
	shll	$16, %r10d
	orq	%r10, %r8
	addl	%esi, %r10d
	andl	$16711680, %r10d                ## imm = 0xFF0000
	orl	%r11d, %r10d
	movq	%rdi, %r11
	shrq	$32, %r11
	addb	-4(%rdx,%rcx), %al
	movzbl	%al, %r9d
	shll	$24, %r9d
	orq	%r9, %r8
	addl	%esi, %r9d
	andl	$-16777216, %r9d                ## imm = 0xFF000000
	orl	%r10d, %r9d
	movq	%rdi, %rax
	shrq	$40, %rax
	addb	-3(%rdx,%rcx), %r11b
	addb	-2(%rdx,%rcx), %al
	movzbl	%al, %eax
	shlq	$40, %rax
	movzbl	%r11b, %r10d
	movq	%r15, %r11
	movq	%r12, %r15
	shlq	$32, %r10
	orq	%r10, %r8
	orq	%rax, %r8
	addq	%rsi, %r10
	andq	%r14, %r10
	addq	%rsi, %rax
	andq	%r12, %rax
	orq	%r10, %rax
	movq	%rdi, %r10
	shrq	$48, %r10
	addb	-1(%rdx,%rcx), %r10b
	movzbl	%r10b, %r10d
	shlq	$48, %r10
	orq	%r10, %r8
	addq	%rsi, %r10
	andq	%r11, %r10
	orq	%rax, %r10
	shrq	$56, %rdi
	addb	(%rdx,%rcx), %dil
	movzbl	%dil, %eax
	shlq	$56, %rax
	orq	%rax, %r8
	addq	%rsi, %rax
	movabsq	$-72057594037927936, %rsi       ## imm = 0xFF00000000000000
	andq	%rsi, %rax
	orq	%r10, %rax
	orq	%r9, %rax
	movq	%r8, 1(%rdx,%rcx)
	movq	%rax, 9(%rdx,%rcx)
	addq	$16, %rcx
	addq	$-16, %rbx
	cmpq	$15, %rbx
	ja	LBB2_156
LBB2_157:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-88(%rbp), %r15                 ## 8-byte Reload
	cmpq	%r15, %rcx
	movq	%rdx, %rbx
	jae	LBB2_238
## %bb.158:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %rsi
	notq	%rsi
	addq	-48(%rbp), %rsi                 ## 8-byte Folded Reload
	cmpq	$8, %rsi
	jae	LBB2_267
## %bb.159:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %rdx
	jmp	LBB2_270
LBB2_160:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$3, %r15
	jb	LBB2_238
## %bb.161:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$2, %r8d
	cmpq	$18, %r15
	jb	LBB2_165
## %bb.162:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-3(%rax), %rcx
	movl	$11, %r8d
LBB2_163:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	%rcx, -192(%rbp)                ## 8-byte Spill
	movq	%r8, -128(%rbp)                 ## 8-byte Spill
	movq	-8(%rbx,%r8), %rcx
	movq	(%rbx,%r8), %r9
	movl	%ecx, %eax
	shrl	$8, %eax
	movzbl	-10(%rbx,%r8), %edx
	addb	-9(%rbx,%r8), %al
	addb	%cl, %dl
	movzbl	%al, %r11d
	movzbl	%dl, %edx
	movq	%rdx, -144(%rbp)                ## 8-byte Spill
	movq	%rcx, %r14
	shrq	$16, %r14
	movq	%rcx, %r10
	shrq	$24, %r10
	movq	%rcx, %rax
	shrq	$32, %rax
	movq	%rcx, %r8
	shrq	$40, %r8
	movq	%rcx, %rdi
	shrq	$48, %rdi
	movq	%rdi, %rbx
	shrq	$56, %rcx
	movl	%edx, %edi
	addl	%r14d, %edi
	movl	%edi, %r13d
	movl	%edi, -252(%rbp)                ## 4-byte Spill
	leaq	(%r10,%r11), %rdx
	movq	%rdx, -96(%rbp)                 ## 8-byte Spill
	movq	%r11, -176(%rbp)                ## 8-byte Spill
	addl	%eax, %r14d
	addl	%r8d, %r10d
	movq	%rbx, %rdx
	addl	%eax, %ebx
	addq	%rcx, %r8
	addq	%r9, %rdx
	movq	%rdx, %rax
	movq	%rdx, -160(%rbp)                ## 8-byte Spill
	movq	%r9, %rdx
	shrq	$8, %rdx
	addl	%edx, %ecx
	movq	%r9, %r15
	shrq	$24, %r15
	addl	%r15d, %edx
	shll	$24, %edx
	movzbl	%al, %eax
	orq	%rax, %rdx
	movq	%r9, %rax
	shrq	$16, %rax
	leal	(%rax,%r9), %esi
	movzbl	%sil, %esi
	shll	$16, %esi
	orq	%rsi, %rdx
	shll	$8, %ecx
	movzwl	%cx, %ecx
	orq	%rcx, %rdx
	movq	%r9, %rcx
	shrq	$32, %rcx
	leal	(%rcx,%rax), %edi
	shlq	$32, %rdi
	movabsq	$1095216660480, %rsi            ## imm = 0xFF00000000
	andq	%rsi, %rdi
	orq	%rdx, %rdi
	shlq	$48, %rcx
	addq	%r9, %rcx
	movabsq	$71776119061217280, %rdx        ## imm = 0xFF000000000000
	andq	%rdx, %rcx
	movq	%r9, %r12
	shlq	$16, %r12
	movabsq	$-72057594037927936, %rax       ## imm = 0xFF00000000000000
	andq	%rax, %r12
	addq	%r9, %r12
	andq	%rax, %r12
	orq	%rcx, %r12
                                        ## kill: def $r11d killed $r11d killed $r11 def $r11
	shll	$8, %r11d
	movq	%r11, -288(%rbp)                ## 8-byte Spill
	shlq	$48, %rbx
	andq	%rdx, %rbx
	movq	%rbx, -200(%rbp)                ## 8-byte Spill
	shlq	$40, %r10
	movabsq	$280375465082880, %rax          ## imm = 0xFF0000000000
	andq	%rax, %r10
	orq	%rbx, %r10
	shlq	$32, %r14
	andq	%rsi, %r14
	movq	%rsi, %rbx
	orq	%r10, %r14
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	addq	%rax, %r8
                                        ## kill: def $eax killed $eax killed $rax def $rax
	shll	$24, %eax
	orq	%r14, %rax
	movzbl	%r13b, %edx
	shll	$16, %edx
	addq	-144(%rbp), %rdx                ## 8-byte Folded Reload
	orq	%r11, %rdx
	orq	%rax, %rdx
	orq	%rdi, %r12
	movq	%r12, %r11
	shrq	$16, %r11
	movq	%r12, %rcx
	shrq	$24, %rcx
	movq	%r11, %rax
	shlq	$48, %rax
	addq	%r12, %rax
	movq	%rax, -280(%rbp)                ## 8-byte Spill
	movq	%r8, -96(%rbp)                  ## 8-byte Spill
	addl	%ecx, %r8d
	shlq	$56, %rcx
	addq	%r12, %rcx
	movq	%r12, %r13
	shrq	$8, %r13
	addl	%r13d, %r15d
	shlq	$40, %r15
	addq	%r9, %r15
	movq	%r15, -272(%rbp)                ## 8-byte Spill
	shrq	$32, %r14
	addq	%rdx, %r14
	movq	-160(%rbp), %rsi                ## 8-byte Reload
	addl	%r14d, %esi
	shlq	$32, %r14
	movq	%r14, %rax
	andq	%rbx, %rax
	movl	%edx, %r9d
	movl	%edx, %r12d
	andl	$-65536, %edx                   ## imm = 0xFFFF0000
	orq	%rax, %rdx
	shrq	$40, %r10
	shrl	$8, %r9d
	addl	%r10d, %r9d
	shlq	$40, %r9
	movq	%r9, %rax
	movabsq	$280375465082880, %rbx          ## imm = 0xFF0000000000
	andq	%rbx, %rax
	orq	%rax, %rdx
	movq	-200(%rbp), %rax                ## 8-byte Reload
	shrq	$48, %rax
	shrl	$16, %r12d
	addl	%eax, %r12d
	movq	%rax, %rbx
	shlq	$48, %r12
	movq	%r12, %rax
	movabsq	$71776119061217280, %r15        ## imm = 0xFF000000000000
	andq	%r15, %rax
	orq	%rax, %rdx
	addl	-176(%rbp), %r10d               ## 4-byte Folded Reload
	addl	%r13d, %r10d
	movzbl	%sil, %eax
	shll	$8, %r10d
	movzwl	%r10w, %r10d
	orl	%eax, %r10d
	movl	-252(%rbp), %eax                ## 4-byte Reload
	addl	%ebx, %eax
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	addl	%eax, %r11d
	movzbl	%r11b, %eax
	shll	$16, %eax
	orl	%r10d, %eax
	shll	$24, %r8d
	orl	%eax, %r8d
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	imulq	%rax, %rdi
	addq	%rdi, %r14
	addq	-272(%rbp), %r9                 ## 8-byte Folded Reload
	addq	-280(%rbp), %r12                ## 8-byte Folded Reload
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	shlq	$56, %rax
	movabsq	$1095216660480, %rsi            ## imm = 0xFF00000000
	andq	%rsi, %r14
	addq	%rax, %rcx
	movq	%rax, %rsi
	movabsq	$-72057594037927936, %rax       ## imm = 0xFF00000000000000
	andq	%rax, %rcx
	orq	%r14, %rcx
	movabsq	$71776119061217280, %rax        ## imm = 0xFF000000000000
	andq	%rax, %r12
	orq	%r12, %rcx
	orq	%rcx, %r8
	movq	-192(%rbp), %rcx                ## 8-byte Reload
	movabsq	$280375465082880, %rax          ## imm = 0xFF0000000000
	andq	%rax, %r9
	orq	%r9, %r8
	addq	-144(%rbp), %rdx                ## 8-byte Folded Reload
	addq	-288(%rbp), %rdx                ## 8-byte Folded Reload
	orq	%rsi, %rdx
	movq	-128(%rbp), %rax                ## 8-byte Reload
	movq	%rdx, -8(%rbx,%rax)
	movq	-128(%rbp), %rax                ## 8-byte Reload
	movq	%r8, (%rbx,%rax)
	movq	-128(%rbp), %r8                 ## 8-byte Reload
	addq	$16, %r8
	addq	$-16, %rcx
	cmpq	$15, %rcx
	ja	LBB2_163
## %bb.164:                             ##   in Loop: Header=BB2_103 Depth=2
	addq	$-9, %r8
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
LBB2_165:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r15, %r8
	jae	LBB2_238
## %bb.166:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rdx
	notq	%rdx
	addq	-48(%rbp), %rdx                 ## 8-byte Folded Reload
	cmpq	$2, %rdx
	jae	LBB2_272
## %bb.167:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rcx
	jmp	LBB2_275
LBB2_168:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$5, %r15
	jb	LBB2_238
## %bb.169:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$4, %eax
	cmpq	$20, %r15
	jb	LBB2_173
## %bb.170:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-5(%rax), %rcx
	xorl	%eax, %eax
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
LBB2_171:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	%rax, -128(%rbp)                ## 8-byte Spill
	movq	%rcx, -144(%rbp)                ## 8-byte Spill
	movq	4(%rsi,%rax), %r9
	movq	12(%rsi,%rax), %r8
	movl	%r9d, %r11d
	shrl	$8, %r11d
	movl	%r9d, %ecx
	shrl	$16, %ecx
	movl	%r9d, %eax
	shrl	$24, %eax
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	movzbl	(%rsi,%rdx), %r10d
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	addb	1(%rsi,%rdx), %r11b
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	addb	2(%rsi,%rdx), %cl
	addb	%r9b, %r10b
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	addb	3(%rsi,%rdx), %al
	movzbl	%al, %edi
	movq	%r9, %rdx
	movq	%r9, %rbx
	movq	%r9, %rsi
	shrq	$56, %r9
	addq	%rdi, %r9
                                        ## kill: def $edi killed $edi killed $rdi def $rdi
	shll	$24, %edi
	movq	%rdi, -176(%rbp)                ## 8-byte Spill
	movzbl	%cl, %ecx
	movl	%ecx, -192(%rbp)                ## 4-byte Spill
	movl	%ecx, %r12d
	shll	$16, %r12d
	movq	%r12, -96(%rbp)                 ## 8-byte Spill
                                        ## kill: def $r12d killed $r12d killed $r12 def $r12
	orl	%edi, %r12d
	movzbl	%r11b, %ecx
	movl	%ecx, -160(%rbp)                ## 4-byte Spill
	movl	%ecx, %r11d
	shll	$8, %r11d
	movq	%r8, %r15
	shrq	$24, %r15
	movq	%r15, %r14
	shlq	$56, %r14
	addq	%r8, %r14
	addl	%r9d, %r15d
	shlq	$56, %r9
	addq	%r9, %r14
	orq	%r11, %r9
                                        ## kill: def $r11d killed $r11d killed $r11 def $r11
	orl	%r12d, %r11d
	shrq	$32, %rdx
	shrq	$48, %rsi
	movzbl	%r10b, %eax
	addl	%eax, %edx
	shrl	$16, %r12d
	addl	%esi, %r12d
	movq	%rsi, %r10
	shlq	$48, %r12
	movq	%r12, %rdi
	movabsq	$71776119061217280, %rcx        ## imm = 0xFF000000000000
	andq	%rcx, %rdi
	leal	(%r11,%rdx), %ecx
	shlq	$32, %rcx
	movq	%rcx, %r13
	movabsq	$1095216660480, %rsi            ## imm = 0xFF00000000
	andq	%rsi, %r13
	orq	%rdi, %r13
	shrq	$40, %rbx
	shrl	$8, %r11d
	addl	%ebx, %r11d
	shlq	$40, %r11
	movq	%r11, %rdi
	movabsq	$280375465082880, %rsi          ## imm = 0xFF0000000000
	andq	%rsi, %rdi
	orq	%rdi, %r13
	addq	-96(%rbp), %r13                 ## 8-byte Folded Reload
	addq	-176(%rbp), %r13                ## 8-byte Folded Reload
	orq	%rax, %r13
	movq	%r8, %rax
	shrq	$8, %rax
	addl	%eax, %ebx
	movq	%r8, %rdi
	shrq	$16, %rdi
	addl	%edi, %r10d
	shlq	$40, %rax
	addq	%r8, %rax
	shlq	$48, %rdi
	addq	%r8, %rdi
	movq	%r8, %rsi
	addl	%edx, %r8d
	addl	-160(%rbp), %ebx                ## 4-byte Folded Reload
	movzbl	%r8b, %r8d
	shll	$8, %ebx
	movzwl	%bx, %ebx
	orl	%r8d, %ebx
	addl	-192(%rbp), %r10d               ## 4-byte Folded Reload
	movzbl	%r10b, %r8d
	shll	$16, %r8d
	orl	%ebx, %r8d
	shll	$24, %r15d
	orl	%r8d, %r15d
	movabsq	$4294967297, %r8                ## imm = 0x100000001
	imulq	%r8, %rsi
	addq	%rsi, %rcx
	addq	%rax, %r11
	addq	%rdi, %r12
	movabsq	$-72057594037927936, %rax       ## imm = 0xFF00000000000000
	andq	%rax, %r14
	movq	-128(%rbp), %rax                ## 8-byte Reload
	orq	%r14, %r15
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
	movabsq	$71776119061217280, %rdx        ## imm = 0xFF000000000000
	andq	%rdx, %r12
	orq	%r12, %r15
	movabsq	$1095216660480, %rdx            ## imm = 0xFF00000000
	andq	%rdx, %rcx
	orq	%rcx, %r15
	movq	-144(%rbp), %rcx                ## 8-byte Reload
	movabsq	$280375465082880, %rdx          ## imm = 0xFF0000000000
	andq	%rdx, %r11
	orq	%r11, %r15
	orq	%r13, %r9
	movq	%r9, 4(%rsi,%rax)
	movq	%r15, 12(%rsi,%rax)
	addq	$16, %rax
	addq	$-16, %rcx
	cmpq	$15, %rcx
	ja	LBB2_171
## %bb.172:                             ##   in Loop: Header=BB2_103 Depth=2
	addq	$4, %rax
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
LBB2_173:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r15, %rax
	jae	LBB2_238
## %bb.174:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rax, %rsi
	notq	%rsi
	addq	-48(%rbp), %rsi                 ## 8-byte Folded Reload
	cmpq	$4, %rsi
	jae	LBB2_277
## %bb.175:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rax, %rcx
	jmp	LBB2_280
LBB2_176:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	$7, %r15
	jb	LBB2_238
## %bb.177:                             ##   in Loop: Header=BB2_103 Depth=2
	movl	$6, %edx
	cmpq	$22, %r15
	jb	LBB2_181
## %bb.178:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	leaq	-7(%rax), %rcx
	xorl	%edx, %edx
	movq	-72(%rbp), %r14                 ## 8-byte Reload
LBB2_179:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	%rcx, -128(%rbp)                ## 8-byte Spill
	movq	6(%r14,%rdx), %rsi
	movq	14(%r14,%rdx), %rcx
	movl	%esi, %r10d
	shrl	$8, %r10d
	movl	%esi, %r9d
	shrl	$16, %r9d
	movl	%esi, %ebx
	shrl	$24, %ebx
	movq	%rsi, %rax
	shrq	$32, %rax
	movq	%rsi, %r8
	shrq	$40, %r8
	movzbl	(%r14,%rdx), %r11d
	addb	%sil, %r11b
	addb	1(%r14,%rdx), %r10b
	addb	2(%r14,%rdx), %r9b
	addb	3(%r14,%rdx), %bl
	addb	4(%r14,%rdx), %al
	addb	5(%r14,%rdx), %r8b
	movzbl	%r8b, %edi
	shlq	$40, %rdi
	movq	%rdi, %r8
	movq	%rdi, -160(%rbp)                ## 8-byte Spill
	movzbl	%al, %edi
	shlq	$32, %rdi
	movq	%rdi, -144(%rbp)                ## 8-byte Spill
	movzbl	%bl, %ebx
	movl	%ebx, %eax
	shll	$24, %eax
	movzbl	%r9b, %r9d
	shll	$16, %r9d
	orl	%eax, %r9d
	leaq	(%r9,%rdi), %r14
	orq	%r8, %r14
	movzbl	%r10b, %eax
	shll	$8, %eax
	movq	%rax, -192(%rbp)                ## 8-byte Spill
	movzbl	%r11b, %edi
	movq	%rdi, -176(%rbp)                ## 8-byte Spill
	leaq	(%rax,%rdi), %r11
	orq	%r14, %r11
	movq	%rsi, %r12
	shrq	$48, %r12
	shrq	$56, %rsi
	shrq	$16, %r14
	leal	(%r14,%rcx), %eax
	movzbl	%al, %eax
	movq	%rcx, %r15
	shrq	$8, %r15
	addl	%r15d, %ebx
	shll	$8, %ebx
	movzwl	%bx, %ebx
	orl	%eax, %ebx
	movl	%r14d, %eax
	andl	$16711680, %eax                 ## imm = 0xFF0000
	addl	%ecx, %eax
	andl	$16711680, %eax                 ## imm = 0xFF0000
	orl	%ebx, %eax
	leal	(%r12,%r11), %ebx
	andl	$-16777216, %r14d               ## imm = 0xFF000000
	addl	%ecx, %r14d
	andl	$-16777216, %r14d               ## imm = 0xFF000000
	orl	%eax, %r14d
	movl	%r11d, %eax
	shrl	$8, %eax
	leal	(%rsi,%rax), %r13d
	shlq	$32, %r12
	addq	%rcx, %r12
	shlq	$40, %rsi
	addq	%rcx, %rsi
	shlq	$56, %r15
	addq	%rcx, %r15
	movabsq	$281474976710657, %rax          ## imm = 0x1000000000001
	imulq	%rax, %rcx
	movabsq	$71776119061217280, %r8         ## imm = 0xFF000000000000
	andq	%r8, %rcx
	movabsq	$-72057594037927936, %r10       ## imm = 0xFF00000000000000
	andq	%r10, %r15
	orq	%rcx, %r15
	movabsq	$1095216660480, %rdi            ## imm = 0xFF00000000
	andq	%rdi, %r12
	orq	%r14, %r12
	movabsq	$280375465082880, %rcx          ## imm = 0xFF0000000000
	andq	%rcx, %rsi
	orq	%r12, %rsi
	orq	%rsi, %r15
	shlq	$56, %r13
	shlq	$48, %rbx
	andq	%r8, %rbx
	orq	%r13, %rbx
	shlq	$32, %r11
	movq	%r11, %rax
	andq	%rcx, %rax
	addq	%rsi, %rax
	addq	%r11, %r12
	andq	%rdi, %r12
	andq	%rcx, %rax
	orq	%r12, %rax
	movq	%r11, %rcx
	andq	%r8, %rcx
	addq	%r15, %rcx
	andq	%r8, %rcx
	orq	%rax, %rcx
	andq	%r10, %r11
	addq	%r15, %r11
	andq	%r10, %r11
	orq	%r14, %r11
	movq	-72(%rbp), %r14                 ## 8-byte Reload
	orq	%rcx, %r11
	movq	-128(%rbp), %rcx                ## 8-byte Reload
	addq	-160(%rbp), %rbx                ## 8-byte Folded Reload
	addq	-144(%rbp), %rbx                ## 8-byte Folded Reload
	orq	%r9, %rbx
	addq	-176(%rbp), %rbx                ## 8-byte Folded Reload
	orq	-192(%rbp), %rbx                ## 8-byte Folded Reload
	movq	%rbx, 6(%r14,%rdx)
	movq	%r11, 14(%r14,%rdx)
	addq	$16, %rdx
	addq	$-16, %rcx
	cmpq	$15, %rcx
	ja	LBB2_179
## %bb.180:                             ##   in Loop: Header=BB2_103 Depth=2
	addq	$6, %rdx
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
LBB2_181:                               ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r15, %rdx
	jae	LBB2_238
## %bb.182:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdx, %rcx
	notq	%rcx
	addq	-48(%rbp), %rcx                 ## 8-byte Folded Reload
	cmpq	$2, %rcx
	jb	LBB2_186
## %bb.183:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %rsi
	andq	$-2, %rsi
	movq	-72(%rbp), %rax                 ## 8-byte Reload
	leaq	(%rax,%rdx), %rdi
	addq	%rsi, %rdx
	xorl	%r8d, %r8d
LBB2_184:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzwl	(%rdi,%r8), %eax
	movd	%eax, %xmm0
	movzwl	-6(%rdi,%r8), %eax
	movd	%eax, %xmm1
	paddb	%xmm0, %xmm1
	movd	%xmm1, %eax
	movw	%ax, (%rdi,%r8)
	addq	$2, %r8
	cmpq	%r8, %rsi
	jne	LBB2_184
## %bb.185:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %rcx
	je	LBB2_238
LBB2_186:                               ##   in Loop: Header=BB2_103 Depth=2
	incq	%rdx
LBB2_187:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-6(%rbx,%rdx), %eax
	addb	%al, (%rbx,%rdx)
	incq	%rdx
	cmpq	%rdx, -48(%rbp)                 ## 8-byte Folded Reload
	jne	LBB2_187
	jmp	LBB2_238
LBB2_259:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	jmp	LBB2_233
LBB2_188:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %ecx
	andl	$-32, %ecx
	xorl	%r8d, %r8d
LBB2_189:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%r13,%r8), %xmm0
	movdqu	17(%r13,%r8), %xmm1
	movdqu	1(%rbx,%r8), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%rbx,%r8), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%r8)
	movdqu	%xmm0, 17(%rbx,%r8)
	addq	$32, %r8
	cmpq	%r8, %rcx
	jne	LBB2_189
## %bb.190:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %rdi
	je	LBB2_200
## %bb.191:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %dil
	je	LBB2_195
LBB2_192:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r8
	movl	%edi, %ecx
	andl	$-4, %ecx
LBB2_193:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rdx,%r8), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	movd	(%r10,%r8), %xmm1               ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%r10,%r8)
	addq	$4, %r8
	cmpq	%r8, %rcx
	jne	LBB2_193
## %bb.194:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %rdi
	je	LBB2_200
	jmp	LBB2_195
LBB2_208:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r15, %rcx
	andq	$-32, %rcx
	xorl	%esi, %esi
LBB2_209:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%rbx,%rsi), %xmm0
	movdqu	17(%rbx,%rsi), %xmm1
	movdqu	1(%r13,%rsi), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%r13,%rsi), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%rsi)
	movdqu	%xmm0, 17(%rbx,%rsi)
	addq	$32, %rsi
	cmpq	%rsi, %rcx
	jne	LBB2_209
## %bb.210:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %r15
	je	LBB2_238
## %bb.211:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %r15b
	je	LBB2_215
LBB2_212:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %rsi
	movq	%r15, %rcx
	andq	$-4, %rcx
	leaq	1(%rbx), %rax
LBB2_213:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rax,%rsi), %xmm0              ## xmm0 = mem[0],zero,zero,zero
	movd	(%rdx,%rsi), %xmm1              ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%rax,%rsi)
	addq	$4, %rsi
	cmpq	%rsi, %rcx
	jne	LBB2_213
## %bb.214:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %r15
	je	LBB2_238
	jmp	LBB2_215
LBB2_220:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %ecx
	andl	$-32, %ecx
	xorl	%r8d, %r8d
LBB2_221:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	1(%r13,%r8), %xmm0
	movdqu	17(%r13,%r8), %xmm1
	psrlw	$1, %xmm0
	pand	%xmm5, %xmm0
	psrlw	$1, %xmm1
	pand	%xmm5, %xmm1
	movdqu	1(%rbx,%r8), %xmm2
	paddb	%xmm0, %xmm2
	movdqu	17(%rbx,%r8), %xmm0
	paddb	%xmm1, %xmm0
	movdqu	%xmm2, 1(%rbx,%r8)
	movdqu	%xmm0, 17(%rbx,%r8)
	addq	$32, %r8
	cmpq	%r8, %rcx
	jne	LBB2_221
## %bb.222:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %rdi
	je	LBB2_231
## %bb.223:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$28, %dil
	je	LBB2_227
LBB2_224:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r8
	movl	%edi, %ecx
	andl	$-4, %ecx
LBB2_225:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rdx,%r8), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	psrlw	$1, %xmm0
	pand	%xmm5, %xmm0
	movd	(%r9,%r8), %xmm1                ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm0, %xmm1
	movd	%xmm1, (%r9,%r8)
	addq	$4, %r8
	cmpq	%r8, %rcx
	jne	LBB2_225
## %bb.226:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rcx, %rdi
	je	LBB2_231
	jmp	LBB2_227
LBB2_244:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rcx
	jmp	LBB2_253
LBB2_245:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rax, %r14
	andq	$-16, %r14
	movq	%rsi, %r15
	negq	%r15
	movq	%r14, %r9
LBB2_246:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	(%r8,%r15), %xmm8
	movdqa	%xmm8, -192(%rbp)               ## 16-byte Spill
	movdqa	%xmm8, %xmm10
	pxor	%xmm11, %xmm11
	punpckhbw	%xmm11, %xmm10          ## xmm10 = xmm10[8],xmm11[8],xmm10[9],xmm11[9],xmm10[10],xmm11[10],xmm10[11],xmm11[11],xmm10[12],xmm11[12],xmm10[13],xmm11[13],xmm10[14],xmm11[14],xmm10[15],xmm11[15]
	movdqa	%xmm10, %xmm6
	punpcklwd	%xmm11, %xmm10          ## xmm10 = xmm10[0],xmm11[0],xmm10[1],xmm11[1],xmm10[2],xmm11[2],xmm10[3],xmm11[3]
	punpcklbw	%xmm11, %xmm8           ## xmm8 = xmm8[0],xmm11[0],xmm8[1],xmm11[1],xmm8[2],xmm11[2],xmm8[3],xmm11[3],xmm8[4],xmm11[4],xmm8[5],xmm11[5],xmm8[6],xmm11[6],xmm8[7],xmm11[7]
	movdqa	%xmm8, %xmm7
	punpckhwd	%xmm11, %xmm7           ## xmm7 = xmm7[4],xmm11[4],xmm7[5],xmm11[5],xmm7[6],xmm11[6],xmm7[7],xmm11[7]
	punpcklwd	%xmm11, %xmm8           ## xmm8 = xmm8[0],xmm11[0],xmm8[1],xmm11[1],xmm8[2],xmm11[2],xmm8[3],xmm11[3]
	movdqu	(%rbx), %xmm2
	movdqa	%xmm2, -160(%rbp)               ## 16-byte Spill
	movdqa	%xmm2, %xmm14
	punpckhbw	%xmm11, %xmm14          ## xmm14 = xmm14[8],xmm11[8],xmm14[9],xmm11[9],xmm14[10],xmm11[10],xmm14[11],xmm11[11],xmm14[12],xmm11[12],xmm14[13],xmm11[13],xmm14[14],xmm11[14],xmm14[15],xmm11[15]
	movdqa	%xmm14, -144(%rbp)              ## 16-byte Spill
	punpcklwd	%xmm11, %xmm14          ## xmm14 = xmm14[0],xmm11[0],xmm14[1],xmm11[1],xmm14[2],xmm11[2],xmm14[3],xmm11[3]
	punpcklbw	%xmm11, %xmm2           ## xmm2 = xmm2[0],xmm11[0],xmm2[1],xmm11[1],xmm2[2],xmm11[2],xmm2[3],xmm11[3],xmm2[4],xmm11[4],xmm2[5],xmm11[5],xmm2[6],xmm11[6],xmm2[7],xmm11[7]
	movdqa	%xmm2, %xmm12
	punpckhwd	%xmm11, %xmm12          ## xmm12 = xmm12[4],xmm11[4],xmm12[5],xmm11[5],xmm12[6],xmm11[6],xmm12[7],xmm11[7]
	punpcklwd	%xmm11, %xmm2           ## xmm2 = xmm2[0],xmm11[0],xmm2[1],xmm11[1],xmm2[2],xmm11[2],xmm2[3],xmm11[3]
	movdqu	(%rbx,%r15), %xmm0
	movdqa	%xmm0, %xmm13
	movdqa	%xmm0, -176(%rbp)               ## 16-byte Spill
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
	movdqa	-144(%rbp), %xmm10              ## 16-byte Reload
	punpckhwd	%xmm11, %xmm10          ## xmm10 = xmm10[4],xmm11[4],xmm10[5],xmm11[5],xmm10[6],xmm11[6],xmm10[7],xmm11[7]
	movdqa	%xmm10, -144(%rbp)              ## 16-byte Spill
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
	movdqa	%xmm2, -128(%rbp)               ## 16-byte Spill
	movdqa	%xmm9, %xmm12
	psubd	%xmm14, %xmm12
	movdqa	%xmm11, %xmm2
	psubd	-144(%rbp), %xmm2               ## 16-byte Folded Reload
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
	movdqa	-128(%rbp), %xmm1               ## 16-byte Reload
	movdqa	%xmm1, %xmm0
	psrad	$31, %xmm0
	pxor	%xmm0, %xmm1
	psubd	%xmm0, %xmm1
	movdqa	%xmm1, %xmm13
	movdqa	%xmm1, -128(%rbp)               ## 16-byte Spill
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
	movdqa	%xmm14, -144(%rbp)              ## 16-byte Spill
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
	por	-144(%rbp), %xmm3               ## 16-byte Folded Reload
	movdqa	-128(%rbp), %xmm5               ## 16-byte Reload
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
	movdqa	-176(%rbp), %xmm0               ## 16-byte Reload
	pand	%xmm6, %xmm0
	pandn	-160(%rbp), %xmm6               ## 16-byte Folded Reload
	por	%xmm0, %xmm6
	pand	%xmm3, %xmm6
	pandn	-192(%rbp), %xmm3               ## 16-byte Folded Reload
	por	%xmm6, %xmm3
	movdqu	(%r8), %xmm0
	paddb	%xmm3, %xmm0
	movdqu	%xmm0, (%r8)
	addq	$16, %r8
	addq	$16, %rbx
	addq	$-16, %r9
	jne	LBB2_246
## %bb.247:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r14, %rax
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
	je	LBB2_238
## %bb.248:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$12, %al
	je	LBB2_282
LBB2_249:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rax, %r8
	andq	$-4, %r8
	leaq	(%rdi,%r8), %rcx
	leaq	(%rbx,%rdi), %r9
	incq	%r9
	addq	%r13, %rdi
	incq	%rdi
	movq	%r8, %rbx
	pxor	%xmm8, %xmm8
	movdqa	LCPI2_0(%rip), %xmm9            ## xmm9 = [2147483648,2147483648,2147483648,2147483648]
LBB2_250:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%r10,%r14), %xmm0              ## xmm0 = mem[0],zero,zero,zero
	movdqa	%xmm0, %xmm4
	punpcklbw	%xmm8, %xmm4            ## xmm4 = xmm4[0],xmm8[0],xmm4[1],xmm8[1],xmm4[2],xmm8[2],xmm4[3],xmm8[3],xmm4[4],xmm8[4],xmm4[5],xmm8[5],xmm4[6],xmm8[6],xmm4[7],xmm8[7]
	punpcklwd	%xmm8, %xmm4            ## xmm4 = xmm4[0],xmm8[0],xmm4[1],xmm8[1],xmm4[2],xmm8[2],xmm4[3],xmm8[3]
	movd	(%rdi,%r14), %xmm1              ## xmm1 = mem[0],zero,zero,zero
	movdqa	%xmm1, %xmm6
	punpcklbw	%xmm8, %xmm6            ## xmm6 = xmm6[0],xmm8[0],xmm6[1],xmm8[1],xmm6[2],xmm8[2],xmm6[3],xmm8[3],xmm6[4],xmm8[4],xmm6[5],xmm8[5],xmm6[6],xmm8[6],xmm6[7],xmm8[7]
	punpcklwd	%xmm8, %xmm6            ## xmm6 = xmm6[0],xmm8[0],xmm6[1],xmm8[1],xmm6[2],xmm8[2],xmm6[3],xmm8[3]
	movd	(%r11,%r14), %xmm3              ## xmm3 = mem[0],zero,zero,zero
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
	movd	(%r9,%r14), %xmm0               ## xmm0 = mem[0],zero,zero,zero
	paddb	%xmm2, %xmm0
	movd	%xmm0, (%r9,%r14)
	addq	$-4, %rbx
	addq	$4, %r9
	addq	$4, %rdi
	addq	$4, %r11
	addq	$4, %r10
	cmpq	%rbx, %r14
	jne	LBB2_250
## %bb.251:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rax
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	je	LBB2_238
	jmp	LBB2_254
LBB2_252:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rcx
	movq	-104(%rbp), %r13                ## 8-byte Reload
LBB2_253:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
LBB2_254:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%r13, %rdi
	subq	%rsi, %rdi
	incq	%rdi
	movq	%rbx, %r8
	subq	%rsi, %r8
	incq	%r8
	movq	-72(%rbp), %rsi                 ## 8-byte Reload
	movq	%r15, %r9
	jmp	LBB2_256
LBB2_255:                               ##   in Loop: Header=BB2_256 Depth=3
	cmpl	%r12d, %r14d
	cmoval	%r11d, %r10d
	cmpl	%r15d, %r14d
	cmoval	%r11d, %r10d
	addb	%r10b, (%rsi,%rcx)
	decq	%r9
	incq	%rsi
	incq	%rdx
	incq	%rdi
	incq	%r8
	cmpq	%r9, %rcx
	movq	-104(%rbp), %r13                ## 8-byte Reload
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	movq	-88(%rbp), %r15                 ## 8-byte Reload
	je	LBB2_238
LBB2_256:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r8,%rcx), %r10d
	movzbl	(%rdx,%rcx), %ebx
	movzbl	(%rdi,%rcx), %r11d
	leal	(%rbx,%r10), %eax
	subl	%r11d, %eax
	movl	%eax, %r15d
	subl	%r10d, %r15d
	movl	%eax, %r13d
	subl	%r11d, %r13d
	movl	%r15d, %r14d
	negl	%r14d
	cmovsl	%r15d, %r14d
	movl	%eax, %r12d
	subl	%ebx, %r12d
	movl	%ebx, %r15d
	subl	%eax, %r15d
	cmovll	%r12d, %r15d
	movl	%r13d, %r12d
	negl	%r12d
	cmovsl	%r13d, %r12d
	cmpl	%r12d, %r15d
	ja	LBB2_255
## %bb.257:                             ##   in Loop: Header=BB2_256 Depth=3
	movl	%ebx, %r11d
	jmp	LBB2_255
LBB2_260:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r8
	andq	$-32, %r8
	leaq	(%rbx,%rdi), %r9
	addq	$17, %r9
	leaq	(%rdi,%r13), %r10
	addq	$17, %r10
	movq	%rsi, %r11
	negq	%r11
	movq	%r8, %rbx
LBB2_261:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movdqu	-16(%r9,%r11), %xmm0
	movdqu	(%r9,%r11), %xmm1
	movdqu	-16(%r10), %xmm2
	movdqu	(%r10), %xmm3
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
	movdqu	-16(%r9), %xmm1
	paddb	%xmm4, %xmm1
	paddb	%xmm2, %xmm1
	movdqu	(%r9), %xmm2
	paddb	%xmm0, %xmm2
	paddb	%xmm3, %xmm2
	movdqu	%xmm1, -16(%r9)
	movdqu	%xmm2, (%r9)
	addq	$32, %r9
	addq	$32, %r10
	addq	$-32, %rbx
	jne	LBB2_261
## %bb.262:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r8, %rcx
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	je	LBB2_238
## %bb.263:                             ##   in Loop: Header=BB2_103 Depth=2
	testb	$24, %cl
	je	LBB2_283
LBB2_264:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rcx, %r9
	andq	$-8, %r9
	movq	%r8, %r10
	subq	%r9, %r10
	addq	%rdi, %r8
	addq	%r9, %rdi
	leaq	(%rbx,%r8), %r11
	incq	%r11
	addq	%r13, %r8
	incq	%r8
	movq	%rsi, %rbx
	negq	%rbx
LBB2_265:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	(%r11,%rbx), %xmm0              ## xmm0 = mem[0],zero
	movq	(%r8), %xmm1                    ## xmm1 = mem[0],zero
	movdqa	%xmm1, %xmm2
	pand	%xmm0, %xmm2
	pxor	%xmm0, %xmm1
	psrlw	$1, %xmm1
	pand	%xmm5, %xmm1
	movq	(%r11), %xmm0                   ## xmm0 = mem[0],zero
	paddb	%xmm2, %xmm0
	paddb	%xmm1, %xmm0
	movq	%xmm0, (%r11)
	addq	$8, %r11
	addq	$8, %r8
	addq	$8, %r10
	jne	LBB2_265
## %bb.266:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%r9, %rcx
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
	je	LBB2_238
LBB2_233:                               ##   in Loop: Header=BB2_103 Depth=2
	movl	%edi, %eax
	movq	-48(%rbp), %r9                  ## 8-byte Reload
	subl	%r9d, %eax
	movq	%rdi, %r8
	testb	$1, %al
	jne	LBB2_235
## %bb.234:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdi, %rax
	subq	%rsi, %rax
	movq	-72(%rbp), %r8                  ## 8-byte Reload
	movzbl	(%r8,%rax), %eax
	movzbl	(%rdx,%rdi), %ecx
	addl	%eax, %ecx
	shrl	%ecx
	addb	%cl, (%r8,%rdi)
	leaq	1(%rdi), %r8
LBB2_235:                               ##   in Loop: Header=BB2_103 Depth=2
	leaq	-2(%r9), %rax
	cmpq	%rdi, %rax
	je	LBB2_238
## %bb.236:                             ##   in Loop: Header=BB2_103 Depth=2
	movq	%r8, %rcx
	notq	%rcx
	addq	-48(%rbp), %rcx                 ## 8-byte Folded Reload
	leaq	(%rbx,%r8), %rdx
	addq	$2, %rdx
	leaq	(%r8,%r13), %rdi
	addq	$2, %rdi
	negq	%rsi
LBB2_237:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-1(%rdx,%rsi), %eax
	movzbl	-1(%rdi), %r8d
	addl	%eax, %r8d
	shrl	%r8d
	addb	%r8b, -1(%rdx)
	movzbl	(%rdx,%rsi), %eax
	movzbl	(%rdi), %r8d
	addl	%eax, %r8d
	shrl	%r8d
	addb	%r8b, (%rdx)
	addq	$2, %rdx
	addq	$2, %rdi
	addq	$-2, %rcx
	jne	LBB2_237
LBB2_238:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	$0, (%rax)
	subq	$16, %rsp
	leaq	64(%rbp), %rax
	movdqu	(%rax), %xmm0
	movdqu	%xmm0, (%rsp)
	leaq	-2736(%rbp), %r12
	movq	%r12, %rdi
	movq	-224(%rbp), %rsi                ## 8-byte Reload
	movl	-256(%rbp), %r14d               ## 4-byte Reload
	movl	%r14d, %edx
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	movq	%r15, %r8
	callq	*-64(%rbp)                      ## 8-byte Folded Reload
	addq	$16, %rsp
	movl	%eax, %ecx
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	leaq	16(%rbp), %r15
	jne	LBB2_70
## %bb.239:                             ##   in Loop: Header=BB2_103 Depth=2
	incl	%r14d
	movq	%rbx, %rax
	movq	-296(%rbp), %rbx                ## 8-byte Reload
	movq	%r13, %rdx
	movq	-48(%rbp), %r9                  ## 8-byte Reload
	movq	%rax, %r13
	movq	-224(%rbp), %rax                ## 8-byte Reload
	cmpl	20(%rax), %r14d
	movq	-216(%rbp), %rcx                ## 8-byte Reload
	movq	-240(%rbp), %rsi                ## 8-byte Reload
	movq	-232(%rbp), %rdi                ## 8-byte Reload
	jb	LBB2_103
	jmp	LBB2_74
LBB2_267:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %rdi
	andq	$-8, %rdi
	leaq	(%rcx,%rdi), %rdx
	movq	-7(%rbx,%rcx), %xmm0            ## xmm0 = mem[0],zero
	addq	-72(%rbp), %rcx                 ## 8-byte Folded Reload
	xorl	%r8d, %r8d
LBB2_268:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	(%rcx,%r8), %xmm1               ## xmm1 = mem[0],zero
	paddb	%xmm1, %xmm0
	movq	%xmm0, (%rcx,%r8)
	addq	$8, %r8
	cmpq	%r8, %rdi
	jne	LBB2_268
## %bb.269:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdi, %rsi
	je	LBB2_238
LBB2_270:                               ##   in Loop: Header=BB2_103 Depth=2
	incq	%rdx
LBB2_271:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-8(%rbx,%rdx), %eax
	addb	%al, (%rbx,%rdx)
	incq	%rdx
	cmpq	%rdx, -48(%rbp)                 ## 8-byte Folded Reload
	jne	LBB2_271
	jmp	LBB2_238
LBB2_272:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rdx, %rsi
	andq	$-2, %rsi
	leaq	(%r8,%rsi), %rcx
	movzwl	-1(%rbx,%r8), %eax
	movd	%eax, %xmm0
	addq	-72(%rbp), %r8                  ## 8-byte Folded Reload
	xorl	%edi, %edi
LBB2_273:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzwl	(%r8,%rdi), %eax
	movd	%eax, %xmm1
	paddb	%xmm1, %xmm0
	movd	%xmm0, %eax
	movw	%ax, (%r8,%rdi)
	addq	$2, %rdi
	cmpq	%rdi, %rsi
	jne	LBB2_273
## %bb.274:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rsi, %rdx
	je	LBB2_238
LBB2_275:                               ##   in Loop: Header=BB2_103 Depth=2
	incq	%rcx
LBB2_276:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-2(%rbx,%rcx), %eax
	addb	%al, (%rbx,%rcx)
	incq	%rcx
	cmpq	%rcx, -48(%rbp)                 ## 8-byte Folded Reload
	jne	LBB2_276
	jmp	LBB2_238
LBB2_277:                               ##   in Loop: Header=BB2_103 Depth=2
	movq	%rsi, %rdi
	andq	$-4, %rdi
	leaq	(%rax,%rdi), %rcx
	movd	-3(%rbx,%rax), %xmm0            ## xmm0 = mem[0],zero,zero,zero
	addq	-72(%rbp), %rax                 ## 8-byte Folded Reload
	xorl	%r8d, %r8d
LBB2_278:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	(%rax,%r8), %xmm1               ## xmm1 = mem[0],zero,zero,zero
	paddb	%xmm1, %xmm0
	movd	%xmm0, (%rax,%r8)
	addq	$4, %r8
	cmpq	%r8, %rdi
	jne	LBB2_278
## %bb.279:                             ##   in Loop: Header=BB2_103 Depth=2
	cmpq	%rdi, %rsi
	je	LBB2_238
LBB2_280:                               ##   in Loop: Header=BB2_103 Depth=2
	incq	%rcx
LBB2_281:                               ##   Parent Loop BB2_76 Depth=1
                                        ##     Parent Loop BB2_103 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	-4(%rbx,%rcx), %eax
	addb	%al, (%rbx,%rcx)
	incq	%rcx
	cmpq	%rcx, -48(%rbp)                 ## 8-byte Folded Reload
	jne	LBB2_281
	jmp	LBB2_238
LBB2_282:                               ##   in Loop: Header=BB2_103 Depth=2
	addq	%r14, %rdi
	jmp	LBB2_202
LBB2_283:                               ##   in Loop: Header=BB2_103 Depth=2
	addq	%r8, %rdi
	jmp	LBB2_233
LBB2_289:
	subq	$8, %rsp
	leaq	-2736(%rbp), %rdx
	leaq	-320(%rbp), %r8
	xorl	%r9d, %r9d
	pushq	-80(%rbp)                       ## 8-byte Folded Reload
	pushq	$1
	pushq	$0
	callq	__ZN9gui_forms5image12_GLOBAL__N_116read_image_bytesENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS1_4PlanERNS_11compression13InflateStreamERNS1_10IdatReaderENS3_IS4_Lm18446744073709551615EEEbRNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	%ecx, (%rax)
	testl	%ecx, %ecx
	jne	LBB2_70
## %bb.290:
	leaq	96(%rbp), %rax
	leaq	80(%rbp), %r10
	movq	(%r10), %r9
	movq	24(%r15), %rdx
	movq	32(%r15), %rcx
	subq	$8, %rsp
	movq	-232(%rbp), %rdi                ## 8-byte Reload
	movq	-240(%rbp), %rsi                ## 8-byte Reload
	movq	-216(%rbp), %r8                 ## 8-byte Reload
	pushq	-80(%rbp)                       ## 8-byte Folded Reload
	pushq	(%rax)
	pushq	8(%r10)
	callq	__ZN9gui_forms5image12_GLOBAL__N_115decode_metadataENSt3__14spanIKSt4byteLm18446744073709551615EEERKNS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS3_IS4_Lm18446744073709551615EEENS3_INS0_17PngMetadataRecordELm18446744073709551615EEERNS0_15PngDecodeResultE
	addq	$32, %rsp
	movl	%eax, %ecx
	jmp	LBB2_292
LBB2_291:
	movl	$18, %ecx
LBB2_292:
	movq	-80(%rbp), %rax                 ## 8-byte Reload
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
L2_1_set_238 = LBB2_238-LJTI2_1
L2_1_set_106 = LBB2_106-LJTI2_1
L2_1_set_121 = LBB2_121-LJTI2_1
L2_1_set_124 = LBB2_124-LJTI2_1
L2_1_set_118 = LBB2_118-LJTI2_1
LJTI2_1:
	.long	L2_1_set_238
	.long	L2_1_set_106
	.long	L2_1_set_121
	.long	L2_1_set_124
	.long	L2_1_set_118
L2_2_set_108 = LBB2_108-LJTI2_2
L2_2_set_160 = LBB2_160-LJTI2_2
L2_2_set_142 = LBB2_142-LJTI2_2
L2_2_set_168 = LBB2_168-LJTI2_2
L2_2_set_238 = LBB2_238-LJTI2_2
L2_2_set_176 = LBB2_176-LJTI2_2
L2_2_set_153 = LBB2_153-LJTI2_2
LJTI2_2:
	.long	L2_2_set_108
	.long	L2_2_set_160
	.long	L2_2_set_142
	.long	L2_2_set_168
	.long	L2_2_set_238
	.long	L2_2_set_176
	.long	L2_2_set_238
	.long	L2_2_set_153
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

	.p2align	3, 0x0                          ## @switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.4
l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.4:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj0ELj16ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

	.p2align	3, 0x0                          ## @switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5
l_switch.table._ZN9gui_forms5image18decode_png_samplesENSt3__14spanIKSt4byteLm18446744073709551615EEENS0_15PngDecodeLimitsERNS_11compression13InflateStreamENS2_IS3_Lm18446744073709551615EEENS2_ItLm18446744073709551615EEESA_NS2_INS0_17PngMetadataRecordELm18446744073709551615EEE.5:
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj1ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj2ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj4ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE
	.quad	__ZN9gui_forms5image12_GLOBAL__N_17scatterILj3ELj8ELb0EEENS0_14PngDecodeErrorERKNS1_4PlanERKNS1_4PassEjNSt3__14spanIKSt4byteLm18446744073709551615EEENSB_ItLm18446744073709551615EEE

.subsections_via_symbols
