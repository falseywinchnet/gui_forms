	.build_version macos, 14, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI0_0:
	.long	209                             ## 0xd1
	.long	52                              ## 0x34
	.space	4
	.space	4
LCPI0_1:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI0_2:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
LCPI0_3:
	.long	209                             ## 0xd1
	.long	52                              ## 0x34
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
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
	subq	$1064, %rsp                     ## imm = 0x428
	.cfi_offset %rbx, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edx, %eax
	movq	%rsi, %r14
	movq	%rdi, %rbx
	cmpq	$3, %rsi
	ja	LBB0_5
## %bb.1:
	notl	%eax
	testq	%r14, %r14
	je	LBB0_14
## %bb.2:
	movzbl	(%rbx), %ecx
	xorl	%ecx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm0
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm0, %xmm1                ## xmm1 = xmm0[0,0,0,0]
	movdqa	LCPI0_1(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI0_2(%rip), %xmm1
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	xorl	%ecx, %edx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %eax
	xorl	%edx, %eax
	cmpq	$1, %r14
	je	LBB0_14
## %bb.3:
	movzbl	1(%rbx), %ecx
	xorl	%ecx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm1
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI0_2(%rip), %xmm1
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	xorl	%ecx, %edx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %eax
	xorl	%edx, %eax
	cmpq	$2, %r14
	je	LBB0_14
## %bb.4:
	movzbl	2(%rbx), %ecx
	xorl	%ecx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm1
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI0_2(%rip), %xmm1
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	xorl	%ecx, %eax
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	pxor	%xmm0, %xmm1
	movd	%xmm1, %ecx
	xorl	%ecx, %eax
	jmp	LBB0_14
LBB0_5:
	leaq	-1072(%rbp), %rdi
	movl	$1040, %esi                     ## imm = 0x410
	movl	%eax, %r15d
	callq	___bzero
	xorl	(%rbx), %r15d
	movl	$0, -1076(%rbp)
	notl	%r15d
	movl	%r15d, -240(%rbp)
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	movq	%rax, -32(%rbp)
	cmpq	$8, %r14
	jae	LBB0_7
## %bb.6:
	movl	$4, %r15d
	movl	$1, %eax
	movl	$1, %ecx
	jmp	LBB0_9
LBB0_7:
	leaq	-4(%r14), %rdx
	movl	$4, %r15d
	movl	$1, %eax
	pcmpeqd	%xmm0, %xmm0
	movq	LCPI0_3(%rip), %xmm1            ## xmm1 = [209,52,0,0]
	movl	$1, %ecx
	.p2align	4
LBB0_8:                                 ## =>This Inner Loop Header: Depth=1
	movl	%ecx, %esi
	subl	$12, %esi
	leal	209(%rsi), %edi
	cmovael	%esi, %edi
	movl	%ecx, %esi
	subl	$150, %ecx
	leal	209(%rcx), %r8d
	cmovael	%ecx, %r8d
	movl	%eax, %eax
	movl	-1076(%rbp,%rdi,4), %ecx
	xorl	-240(%rbp,%rax,4), %ecx
	movl	-1076(%rbp,%rsi,4), %eax
	movl	-1076(%rbp,%r8,4), %edi
	xorl	%ecx, %edi
	xorl	(%rbx,%r15), %eax
	movl	%edi, -1076(%rbp,%rsi,4)
	xorl	%ecx, %eax
	movl	-28(%rbp), %ecx
	movl	%eax, -240(%rbp,%rcx,4)
	movq	-32(%rbp), %xmm2                ## xmm2 = mem[0],zero
	psubd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pcmpeqd	%xmm1, %xmm3
	pandn	%xmm2, %xmm3
	movq	%xmm3, -32(%rbp)
	addq	$4, %r15
	movd	%xmm3, %ecx
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	movd	%xmm2, %eax
	addq	$-4, %rdx
	cmpq	$3, %rdx
	ja	LBB0_8
LBB0_9:
	movl	$52, %edx
	pcmpeqd	%xmm0, %xmm0
	movq	LCPI0_3(%rip), %xmm1            ## xmm1 = [209,52,0,0]
	.p2align	4
LBB0_10:                                ## =>This Inner Loop Header: Depth=1
	movl	%ecx, %esi
	subl	$12, %esi
	leal	209(%rsi), %edi
	cmovael	%esi, %edi
	movl	%ecx, %esi
	subl	$150, %ecx
	leal	209(%rcx), %r8d
	cmovael	%ecx, %r8d
	movl	%eax, %eax
	movl	-1076(%rbp,%rdi,4), %ecx
	xorl	-240(%rbp,%rax,4), %ecx
	movl	-1076(%rbp,%r8,4), %eax
	xorl	%ecx, %eax
	xorl	-1076(%rbp,%rsi,4), %ecx
	movl	%eax, -1076(%rbp,%rsi,4)
	movl	-28(%rbp), %eax
	movl	%ecx, -240(%rbp,%rax,4)
	movq	-32(%rbp), %xmm2                ## xmm2 = mem[0],zero
	psubd	%xmm0, %xmm2
	movdqa	%xmm2, %xmm3
	pcmpeqd	%xmm1, %xmm3
	pandn	%xmm2, %xmm3
	movq	%xmm3, -32(%rbp)
	movd	%xmm3, %ecx
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	movd	%xmm2, %eax
	decl	%edx
	jne	LBB0_10
## %bb.11:
	leaq	-1076(%rbp), %rdi
	callq	__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	cmpq	%r14, %r15
	jae	LBB0_14
## %bb.12:
	movdqa	LCPI0_1(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI0_2(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	.p2align	4
LBB0_13:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rbx,%r15), %ecx
	xorl	%eax, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movl	%ecx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	%edx
	movd	%ecx, %xmm2
	shll	$30, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %eax
	xorl	%ecx, %eax
	incq	%r15
	cmpq	%r15, %r14
	jne	LBB0_13
LBB0_14:
	notl	%eax
	addq	$1064, %rsp                     ## imm = 0x428
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI1_0:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI1_1:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
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
	subq	$2152, %rsp                     ## imm = 0x868
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edx, %ebx
	movq	%rsi, %r12
	movq	%rdi, %r14
	cmpq	$7, %rsi
	ja	LBB1_9
## %bb.1:
	notl	%ebx
	testq	%r12, %r12
	je	LBB1_35
## %bb.2:
	movzbl	(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm0
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm0, %xmm1                ## xmm1 = xmm0[0,0,0,0]
	movdqa	LCPI1_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$1, %r12
	je	LBB1_35
## %bb.3:
	movzbl	1(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$2, %r12
	je	LBB1_35
## %bb.4:
	movzbl	2(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$3, %r12
	je	LBB1_35
## %bb.5:
	movzbl	3(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$4, %r12
	je	LBB1_35
## %bb.6:
	movzbl	4(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$5, %r12
	je	LBB1_35
## %bb.7:
	movzbl	5(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$6, %r12
	je	LBB1_35
## %bb.8:
	movzbl	6(%r14), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI1_1(%rip), %xmm1
	shrl	$6, %ecx
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	xorl	%eax, %ebx
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	pxor	%xmm0, %xmm1
	movd	%xmm1, %eax
	xorl	%ebx, %eax
	movl	%eax, %ebx
	jmp	LBB1_35
LBB1_9:
	leaq	-2184(%rbp), %rdi
	movl	$2080, %esi                     ## imm = 0x820
	callq	___bzero
	xorl	(%r14), %ebx
	movl	4(%r14), %eax
	notl	%ebx
	movq	$0, -2192(%rbp)
	movl	%ebx, -520(%rbp)
	movl	%eax, -516(%rbp)
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	movq	%rax, -104(%rbp)
	cmpq	$16, %r12
	jae	LBB1_11
## %bb.10:
	movl	$8, %r15d
	movl	$1, %eax
	movl	$1, %ecx
	jmp	LBB1_13
LBB1_11:
	leaq	-8(%r12), %rdx
	movl	$8, %r15d
	movl	$1, %eax
	xorl	%esi, %esi
	movl	$1, %ecx
	.p2align	4
LBB1_12:                                ## =>This Inner Loop Header: Depth=1
	movl	(%r14,%r15), %r8d
	movl	%ecx, %edi
	subl	$12, %edi
	leal	209(%rdi), %r9d
	cmovael	%edi, %r9d
	movl	4(%r14,%r15), %edi
	movl	%ecx, %r10d
	subl	$150, %ecx
	leal	209(%rcx), %r11d
	cmovael	%ecx, %r11d
	movl	%eax, %eax
	movl	-2192(%rbp,%r9,8), %ecx
	xorl	-520(%rbp,%rax,8), %ecx
	movl	-2192(%rbp,%r11,8), %ebx
	xorl	%ecx, %ebx
	xorl	-2192(%rbp,%r10,8), %r8d
	movl	%ebx, -2192(%rbp,%r10,8)
	xorl	%ecx, %r8d
	movl	%r8d, -520(%rbp,%rax,8)
	movl	-2188(%rbp,%r9,8), %ecx
	xorl	-516(%rbp,%rax,8), %ecx
	movl	-104(%rbp), %eax
	movl	-2188(%rbp,%r11,8), %r8d
	xorl	%ecx, %r8d
	xorl	-2188(%rbp,%rax,8), %edi
	movl	%r8d, -2188(%rbp,%rax,8)
	xorl	%ecx, %edi
	movl	-100(%rbp), %ecx
	movl	%edi, -516(%rbp,%rcx,8)
	leal	1(%rax), %ecx
	cmpl	$209, %ecx
	cmovel	%esi, %ecx
	movl	%ecx, -104(%rbp)
	movl	-100(%rbp), %eax
	incl	%eax
	cmpl	$52, %eax
	cmovel	%esi, %eax
	movl	%eax, -100(%rbp)
	addq	$8, %r15
	addq	$-8, %rdx
	cmpq	$7, %rdx
	ja	LBB1_12
LBB1_13:
	movl	$52, %edx
	xorl	%esi, %esi
	.p2align	4
LBB1_14:                                ## =>This Inner Loop Header: Depth=1
	movl	%ecx, %edi
	subl	$12, %edi
	leal	209(%rdi), %r8d
	cmovael	%edi, %r8d
	movl	%ecx, %edi
	subl	$150, %ecx
	leal	209(%rcx), %r9d
	cmovael	%ecx, %r9d
	movl	%eax, %eax
	movl	-2192(%rbp,%r8,8), %ecx
	xorl	-520(%rbp,%rax,8), %ecx
	movl	-2192(%rbp,%r9,8), %r10d
	xorl	%ecx, %r10d
	xorl	-2192(%rbp,%rdi,8), %ecx
	movl	%r10d, -2192(%rbp,%rdi,8)
	movl	%ecx, -520(%rbp,%rax,8)
	movl	-2188(%rbp,%r8,8), %ecx
	xorl	-516(%rbp,%rax,8), %ecx
	movl	-104(%rbp), %eax
	movl	-2188(%rbp,%r9,8), %edi
	xorl	%ecx, %edi
	xorl	-2188(%rbp,%rax,8), %ecx
	movl	%edi, -2188(%rbp,%rax,8)
	movl	-100(%rbp), %edi
	movl	%ecx, -516(%rbp,%rdi,8)
	leal	1(%rax), %ecx
	cmpl	$209, %ecx
	cmovel	%esi, %ecx
	movl	%ecx, -104(%rbp)
	movl	-100(%rbp), %eax
	incl	%eax
	cmpl	$52, %eax
	cmovel	%esi, %eax
	movl	%eax, -100(%rbp)
	decl	%edx
	jne	LBB1_14
## %bb.15:
	movq	%r12, -56(%rbp)                 ## 8-byte Spill
	movq	%r14, -48(%rbp)                 ## 8-byte Spill
	movl	$1, %eax
	movl	$-1, %ecx
	.p2align	4
LBB1_16:                                ## =>This Inner Loop Header: Depth=1
	movl	-100(%rbp), %edx
	leal	52(%rdx), %esi
	cmpl	%eax, %edx
	cmovael	%edx, %esi
	movl	-104(%rbp), %edx
	addl	%ecx, %esi
	leal	209(%rdx), %edi
	cmpl	%eax, %edx
	movl	%edx, %r8d
	cmovbl	%edi, %r8d
	addl	%ecx, %r8d
	leal	150(%rax), %r9d
	cmpl	%r9d, %edx
	cmovbl	%edi, %edx
	addl	%ecx, %edx
	addl	$-150, %edx
	movq	-2192(%rbp,%rdx,8), %rdx
	xorq	-2192(%rbp,%r8,8), %rdx
	xorq	%rdx, -520(%rbp,%rsi,8)
	decl	%ecx
	incl	%eax
	cmpl	$53, %eax
	jne	LBB1_16
## %bb.17:
	movl	-104(%rbp), %r11d
	movl	-100(%rbp), %eax
	leal	-53(%r11), %ecx
	movq	%rcx, -88(%rbp)                 ## 8-byte Spill
	leal	-151(%r11), %ecx
	movq	%rcx, -96(%rbp)                 ## 8-byte Spill
	leal	-203(%r11), %r9d
	movq	%rax, -72(%rbp)                 ## 8-byte Spill
	addl	$-53, %eax
	movq	%rax, -64(%rbp)                 ## 8-byte Spill
	leal	-1(%r11), %eax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	xorl	%ebx, %ebx
	movl	$261, %r10d                     ## imm = 0x105
	movl	$-248, %r13d
	movl	$1, %ecx
	leaq	__ZN15crc_suitability12_GLOBAL__N_19reductionE(%rip), %r14
	xorl	%eax, %eax
	jmp	LBB1_18
	.p2align	4
LBB1_30:                                ##   in Loop: Header=BB1_18 Depth=1
	movq	-80(%rbp), %rdx                 ## 8-byte Reload
	leal	(%rdx,%rax), %esi
	cmpq	%r11, %rcx
	leal	209(%rdx,%rax), %edx
	cmovbel	%esi, %edx
	leaq	-2192(%rbp), %r12
LBB1_31:                                ##   in Loop: Header=BB1_18 Depth=1
	movl	%edx, %edi
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	movd	%xmm1, %edx
	xorl	4(%r12,%rdi,8), %edx
	movd	%xmm0, %esi
	xorl	(%r12,%rdi,8), %esi
	movzbl	%bl, %edi
	xorl	(%r14,%rdi,4), %edx
	movzbl	%bh, %edi
	xorl	1024(%r14,%rdi,4), %edx
	movl	%ebx, %edi
	shrl	$14, %edi
	andl	$1020, %edi                     ## imm = 0x3FC
	xorl	2048(%rdi,%r14), %edx
	shrl	$24, %ebx
	xorl	3072(%r14,%rbx,4), %edx
	movzbl	%dl, %edi
	xorl	(%r14,%rdi,4), %esi
	movzbl	%dh, %edi
	xorl	1024(%r14,%rdi,4), %esi
	movl	%edx, %edi
	shrl	$14, %edi
	andl	$1020, %edi                     ## imm = 0x3FC
	xorl	2048(%rdi,%r14), %esi
	shrl	$24, %edx
	movl	%esi, %ebx
	xorl	3072(%r14,%rdx,4), %ebx
	decq	%rax
	incl	%r13d
	incq	%rcx
	decq	%r10
	cmpq	$-261, %rax                     ## imm = 0xFEFB
	je	LBB1_32
LBB1_18:                                ## =>This Inner Loop Header: Depth=1
	leaq	261(%rax), %rsi
	cmpq	$12, %rsi
	ja	LBB1_20
## %bb.19:                              ##   in Loop: Header=BB1_18 Depth=1
	leal	(%r11,%rax), %edx
	cmpl	%r13d, %r11d
	leal	209(%r11,%rax), %r12d
	cmovael	%edx, %r12d
	addl	$248, %r12d
	movq	-2192(%rbp,%r12,8), %xmm1       ## xmm1 = mem[0],zero
	movl	%r10d, %r12d
	jmp	LBB1_22
	.p2align	4
LBB1_20:                                ##   in Loop: Header=BB1_18 Depth=1
	cmpq	$52, %rsi
	ja	LBB1_23
## %bb.21:                              ##   in Loop: Header=BB1_18 Depth=1
	leal	261(%rax), %r12d
	pxor	%xmm1, %xmm1
LBB1_22:                                ##   in Loop: Header=BB1_18 Depth=1
	movl	$53, %edx
	subl	%r12d, %edx
	movq	-88(%rbp), %r8                  ## 8-byte Reload
	leal	(%r12,%r8), %edi
	cmpl	%edx, %r11d
	leal	209(%r12,%r8), %edx
	cmovael	%edi, %edx
	movq	-2192(%rbp,%rdx,8), %xmm0       ## xmm0 = mem[0],zero
	pxor	%xmm1, %xmm0
LBB1_25:                                ##   in Loop: Header=BB1_18 Depth=1
	movl	$151, %edx
	subl	%r12d, %edx
	movq	-96(%rbp), %r8                  ## 8-byte Reload
	leal	(%r12,%r8), %edi
	cmpl	%edx, %r11d
	leal	209(%r12,%r8), %edx
	cmovael	%edi, %edx
	movq	-2192(%rbp,%rdx,8), %xmm1       ## xmm1 = mem[0],zero
	pxor	%xmm0, %xmm1
LBB1_28:                                ##   in Loop: Header=BB1_18 Depth=1
	movl	$203, %edx
	subl	%r12d, %edx
	leal	(%r12,%r9), %edi
	cmpl	%edx, %r11d
	leal	209(%r12,%r9), %edx
	cmovael	%edi, %edx
	movq	-2192(%rbp,%rdx,8), %xmm0       ## xmm0 = mem[0],zero
	pxor	%xmm1, %xmm0
	cmpq	$52, %rsi
	ja	LBB1_30
## %bb.29:                              ##   in Loop: Header=BB1_18 Depth=1
	leal	40(%r13), %edx
	movq	-64(%rbp), %rdi                 ## 8-byte Reload
	leal	(%rdi,%r10), %esi
	cmpl	%edx, -72(%rbp)                 ## 4-byte Folded Reload
	leal	52(%rdi,%r10), %edx
	cmovael	%esi, %edx
	leaq	-520(%rbp), %r12
	jmp	LBB1_31
	.p2align	4
LBB1_23:                                ##   in Loop: Header=BB1_18 Depth=1
	cmpq	$150, %rsi
	ja	LBB1_26
## %bb.24:                              ##   in Loop: Header=BB1_18 Depth=1
	leal	261(%rax), %r12d
	pxor	%xmm0, %xmm0
	jmp	LBB1_25
LBB1_26:                                ##   in Loop: Header=BB1_18 Depth=1
	pxor	%xmm0, %xmm0
	cmpq	$202, %rsi
	ja	LBB1_30
## %bb.27:                              ##   in Loop: Header=BB1_18 Depth=1
	leal	261(%rax), %r12d
	pxor	%xmm1, %xmm1
	jmp	LBB1_28
LBB1_32:
	movq	-56(%rbp), %rdi                 ## 8-byte Reload
	cmpq	%rdi, %r15
	jae	LBB1_35
## %bb.33:
	movdqa	LCPI1_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI1_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	movq	-48(%rbp), %rsi                 ## 8-byte Reload
	.p2align	4
LBB1_34:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rsi,%r15), %eax
	xorl	%ebx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm2
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	xorl	%ecx, %eax
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%eax, %ebx
	incq	%r15
	cmpq	%r15, %rdi
	jne	LBB1_34
LBB1_35:
	notl	%ebx
	movl	%ebx, %eax
	addq	$2152, %rsp                     ## imm = 0x868
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI2_0:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI2_1:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
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
	subq	$4248, %rsp                     ## imm = 0x1098
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edx, %ebx
	movq	%rsi, %r14
	movq	%rdi, %r15
	cmpq	$15, %rsi
	ja	LBB2_4
## %bb.1:
	notl	%ebx
	testq	%r14, %r14
	je	LBB2_30
## %bb.2:
	xorl	%eax, %eax
	movdqa	LCPI2_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI2_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	.p2align	4
LBB2_3:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%r15,%rax), %ecx
	xorl	%ebx, %ecx
	movl	%ecx, %esi
	shrl	%esi
	movl	%ecx, %edx
	andl	$1, %edx
	negl	%edx
	andl	$-306674912, %edx               ## imm = 0xEDB88320
	xorl	%esi, %edx
	movl	%edx, %esi
	shrl	%esi
	movd	%ecx, %xmm2
	shll	$30, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	movl	%ecx, %esi
	shrl	$6, %esi
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %edx
	sarl	$31, %edx
	andl	$1994146192, %edx               ## imm = 0x76DC4190
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	xorl	%edx, %ecx
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%ecx, %ebx
	incq	%rax
	cmpq	%rax, %r14
	jne	LBB2_3
	jmp	LBB2_30
LBB2_4:
	leaq	-944(%rbp), %r12
	leaq	-4272(%rbp), %rdi
	movl	$4160, %esi                     ## imm = 0x1040
	callq	___bzero
	movl	12(%r15), %eax
	xorl	(%r15), %ebx
	notl	%ebx
	movl	%ebx, -944(%rbp)
	movq	4(%r15), %rcx
	movq	%rcx, -940(%rbp)
	pxor	%xmm0, %xmm0
	movdqa	%xmm0, -4288(%rbp)
	movl	%eax, -932(%rbp)
	movabsq	$4294967297, %rax               ## imm = 0x100000001
	movq	%rax, -112(%rbp)
	cmpq	$32, %r14
	movq	%r14, -56(%rbp)                 ## 8-byte Spill
	jae	LBB2_6
## %bb.5:
	movl	$16, %r9d
	movl	$1, %esi
	movl	$1, %eax
	jmp	LBB2_8
LBB2_6:
	leaq	-16(%r14), %rcx
	movl	$16, %r9d
	movl	$1, %esi
	xorl	%edx, %edx
	movl	$1, %eax
	.p2align	4
LBB2_7:                                 ## =>This Inner Loop Header: Depth=1
	movl	(%r15,%r9), %r11d
	movl	4(%r15,%r9), %edi
	movl	%eax, %ebx
	movl	%eax, %r14d
                                        ## kill: def $eax killed $eax def $rax
	subl	$12, %eax
	leal	209(%rax), %r8d
	cmovael	%eax, %r8d
	subl	$150, %ebx
	leal	209(%rbx), %r10d
	cmovael	%ebx, %r10d
	shlq	$4, %r8
	movl	%esi, %eax
	shlq	$4, %rax
	movl	-4288(%rbp,%r8), %esi
	xorl	(%r12,%rax), %esi
	shlq	$4, %r10
	shlq	$4, %r14
	movl	-4288(%rbp,%r10), %ebx
	xorl	%esi, %ebx
	xorl	-4288(%rbp,%r14), %r11d
	movl	%ebx, -4288(%rbp,%r14)
	xorl	%esi, %r11d
	movl	%r11d, (%r12,%rax)
	movl	-4284(%rbp,%r8), %esi
	xorl	4(%r12,%rax), %esi
	movl	-112(%rbp), %r11d
	leal	1(%r11), %eax
	shlq	$4, %r11
	movl	-4284(%rbp,%r10), %ebx
	xorl	-4284(%rbp,%r11), %edi
	xorl	%esi, %ebx
	movl	%ebx, -4284(%rbp,%r11)
	xorl	%esi, %edi
	movl	-108(%rbp), %esi
	shlq	$4, %rsi
	movl	%edi, 4(%r12,%rsi)
	movl	-108(%rbp), %edi
	leal	1(%rdi), %esi
	shlq	$4, %rdi
	movq	8(%r15,%r9), %rbx
	movq	-4280(%rbp,%r8), %r8
	xorq	8(%r12,%rdi), %r8
	movq	-4280(%rbp,%r10), %r10
	xorq	%r8, %r10
	xorq	-4280(%rbp,%r11), %rbx
	movq	%r10, -4280(%rbp,%r11)
	xorq	%r8, %rbx
	movq	%rbx, 8(%r12,%rdi)
	cmpl	$209, %eax
	cmovel	%edx, %eax
	movl	%eax, -112(%rbp)
	cmpl	$52, %esi
	cmovel	%edx, %esi
	movl	%esi, -108(%rbp)
	addq	$16, %r9
	addq	$-16, %rcx
	cmpq	$15, %rcx
	ja	LBB2_7
LBB2_8:
	movl	$52, %ecx
	xorl	%edx, %edx
	.p2align	4
LBB2_9:                                 ## =>This Inner Loop Header: Depth=1
	movl	%eax, %r10d
	movl	%eax, %r11d
                                        ## kill: def $eax killed $eax def $rax
	subl	$12, %eax
	leal	209(%rax), %edi
	cmovael	%eax, %edi
	subl	$150, %r10d
	leal	209(%r10), %r8d
	cmovael	%r10d, %r8d
	shlq	$4, %rdi
	shlq	$4, %r8
	movl	%esi, %eax
	shlq	$4, %rax
	movl	-4288(%rbp,%rdi), %esi
	xorl	(%r12,%rax), %esi
	shlq	$4, %r11
	movl	-4288(%rbp,%r8), %r10d
	xorl	%esi, %r10d
	xorl	-4288(%rbp,%r11), %esi
	movl	%r10d, -4288(%rbp,%r11)
	movl	%esi, (%r12,%rax)
	movl	-4284(%rbp,%rdi), %esi
	xorl	4(%r12,%rax), %esi
	movl	-112(%rbp), %r10d
	leal	1(%r10), %eax
	shlq	$4, %r10
	movl	-4284(%rbp,%r8), %r11d
	xorl	%esi, %r11d
	xorl	-4284(%rbp,%r10), %esi
	movl	%r11d, -4284(%rbp,%r10)
	movl	-108(%rbp), %r11d
	shlq	$4, %r11
	movl	%esi, 4(%r12,%r11)
	movl	-108(%rbp), %esi
	movq	%rsi, %r11
	shlq	$4, %r11
	movq	-4280(%rbp,%rdi), %rdi
	xorq	8(%r12,%r11), %rdi
	movq	-4280(%rbp,%r8), %r8
	xorq	%rdi, %r8
	xorq	-4280(%rbp,%r10), %rdi
	incl	%esi
	movq	%r8, -4280(%rbp,%r10)
	movq	%rdi, 8(%r12,%r11)
	cmpl	$209, %eax
	cmovel	%edx, %eax
	movl	%eax, -112(%rbp)
	cmpl	$52, %esi
	cmovel	%edx, %esi
	movl	%esi, -108(%rbp)
	decl	%ecx
	jne	LBB2_9
## %bb.10:
	movq	%r15, -48(%rbp)                 ## 8-byte Spill
	movl	$1, %eax
	movl	$-1, %ecx
	.p2align	4
LBB2_11:                                ## =>This Inner Loop Header: Depth=1
	movl	-112(%rbp), %esi
	movl	-108(%rbp), %edi
	leal	52(%rdi), %edx
	cmpl	%eax, %edi
	cmovael	%edi, %edx
	addl	%ecx, %edx
	leal	209(%rsi), %edi
	cmpl	%eax, %esi
	movl	%esi, %r8d
	cmovbl	%edi, %r8d
	addl	%ecx, %r8d
	leal	150(%rax), %r10d
	cmpl	%r10d, %esi
	cmovbl	%edi, %esi
	addl	%ecx, %esi
	addl	$-150, %esi
	shlq	$4, %r8
	shlq	$4, %rsi
	shlq	$4, %rdx
	movdqa	-4288(%rbp,%rsi), %xmm0
	pxor	-4288(%rbp,%r8), %xmm0
	pxor	(%r12,%rdx), %xmm0
	movdqa	%xmm0, (%r12,%rdx)
	decl	%ecx
	incl	%eax
	cmpl	$53, %eax
	jne	LBB2_11
## %bb.12:
	movl	-112(%rbp), %r8d
	movl	-108(%rbp), %eax
	leal	-53(%r8), %ecx
	movq	%rcx, -88(%rbp)                 ## 8-byte Spill
	leal	-151(%r8), %ecx
	movq	%rcx, -96(%rbp)                 ## 8-byte Spill
	leal	-203(%r8), %ecx
	movq	%rcx, -104(%rbp)                ## 8-byte Spill
	movq	%rax, -72(%rbp)                 ## 8-byte Spill
	addl	$-53, %eax
	movq	%rax, -64(%rbp)                 ## 8-byte Spill
	leal	-1(%r8), %eax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	xorl	%ebx, %ebx
	movl	$261, %r11d                     ## imm = 0x105
	movl	$-248, %r13d
	movl	$1, %edx
	leaq	__ZN15crc_suitability12_GLOBAL__N_19reductionE(%rip), %r15
	xorl	%eax, %eax
	jmp	LBB2_13
	.p2align	4
LBB2_25:                                ##   in Loop: Header=BB2_13 Depth=1
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	leal	(%rcx,%rax), %edi
	cmpq	%r8, %rdx
	leal	209(%rcx,%rax), %ecx
	cmovbel	%edi, %ecx
	leaq	-4288(%rbp), %r14
LBB2_26:                                ##   in Loop: Header=BB2_13 Depth=1
	movl	%ecx, %r10d
	shlq	$4, %r10
	pshufd	$255, %xmm0, %xmm1              ## xmm1 = xmm0[3,3,3,3]
	movd	%xmm1, %ecx
	xorl	12(%r14,%r10), %ecx
	movzbl	%bl, %edi
	xorl	(%r15,%rdi,4), %ecx
	movzbl	%bh, %edi
	xorl	1024(%r15,%rdi,4), %ecx
	movl	%ebx, %edi
	shrl	$14, %edi
	andl	$1020, %edi                     ## imm = 0x3FC
	xorl	2048(%rdi,%r15), %ecx
	shrl	$24, %ebx
	xorl	3072(%r15,%rbx,4), %ecx
	pshufd	$238, %xmm0, %xmm1              ## xmm1 = xmm0[2,3,2,3]
	movd	%xmm1, %ebx
	xorl	8(%r14,%r10), %ebx
	movzbl	%cl, %edi
	xorl	(%r15,%rdi,4), %ebx
	movzbl	%ch, %edi
	xorl	1024(%r15,%rdi,4), %ebx
	movl	%ecx, %edi
	shrl	$14, %edi
	andl	$1020, %edi                     ## imm = 0x3FC
	xorl	2048(%rdi,%r15), %ebx
	shrl	$24, %ecx
	xorl	3072(%r15,%rcx,4), %ebx
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	movd	%xmm1, %ecx
	xorl	4(%r14,%r10), %ecx
	movd	%xmm0, %edi
	xorl	(%r14,%r10), %edi
	movzbl	%bl, %r10d
	xorl	(%r15,%r10,4), %ecx
	movzbl	%bh, %esi
	xorl	1024(%r15,%rsi,4), %ecx
	movl	%ebx, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%r15), %ecx
	shrl	$24, %ebx
	xorl	3072(%r15,%rbx,4), %ecx
	movzbl	%cl, %esi
	xorl	(%r15,%rsi,4), %edi
	movzbl	%ch, %esi
	xorl	1024(%r15,%rsi,4), %edi
	movl	%ecx, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%r15), %edi
	shrl	$24, %ecx
	movl	%edi, %ebx
	xorl	3072(%r15,%rcx,4), %ebx
	decq	%rax
	incl	%r13d
	incq	%rdx
	decq	%r11
	cmpq	$-261, %rax                     ## imm = 0xFEFB
	je	LBB2_27
LBB2_13:                                ## =>This Inner Loop Header: Depth=1
	leaq	261(%rax), %r14
	cmpq	$12, %r14
	ja	LBB2_15
## %bb.14:                              ##   in Loop: Header=BB2_13 Depth=1
	leal	(%r8,%rax), %ecx
	cmpl	%r13d, %r8d
	leal	209(%r8,%rax), %edi
	cmovael	%ecx, %edi
	addl	$248, %edi
	shlq	$4, %rdi
	movdqa	-4288(%rbp,%rdi), %xmm0
	movl	%r11d, %r10d
	jmp	LBB2_17
	.p2align	4
LBB2_15:                                ##   in Loop: Header=BB2_13 Depth=1
	cmpq	$52, %r14
	ja	LBB2_18
## %bb.16:                              ##   in Loop: Header=BB2_13 Depth=1
	leal	261(%rax), %r10d
	pxor	%xmm0, %xmm0
LBB2_17:                                ##   in Loop: Header=BB2_13 Depth=1
	movl	$53, %ecx
	subl	%r10d, %ecx
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	leal	(%r10,%rsi), %edi
	cmpl	%ecx, %r8d
	leal	209(%r10,%rsi), %ecx
	cmovael	%edi, %ecx
	shlq	$4, %rcx
	pxor	-4288(%rbp,%rcx), %xmm0
LBB2_20:                                ##   in Loop: Header=BB2_13 Depth=1
	movl	$151, %ecx
	subl	%r10d, %ecx
	movq	-96(%rbp), %rsi                 ## 8-byte Reload
	leal	(%r10,%rsi), %edi
	cmpl	%ecx, %r8d
	leal	209(%r10,%rsi), %ecx
	cmovael	%edi, %ecx
	shlq	$4, %rcx
	pxor	-4288(%rbp,%rcx), %xmm0
LBB2_23:                                ##   in Loop: Header=BB2_13 Depth=1
	movl	$203, %ecx
	subl	%r10d, %ecx
	movq	-104(%rbp), %rsi                ## 8-byte Reload
	leal	(%r10,%rsi), %edi
	cmpl	%ecx, %r8d
	leal	209(%r10,%rsi), %ecx
	cmovael	%edi, %ecx
	shlq	$4, %rcx
	pxor	-4288(%rbp,%rcx), %xmm0
	cmpq	$52, %r14
	ja	LBB2_25
## %bb.24:                              ##   in Loop: Header=BB2_13 Depth=1
	leal	40(%r13), %ecx
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
	leal	(%rsi,%r11), %edi
	cmpl	%ecx, -72(%rbp)                 ## 4-byte Folded Reload
	leal	52(%rsi,%r11), %ecx
	cmovael	%edi, %ecx
	movq	%r12, %r14
	jmp	LBB2_26
	.p2align	4
LBB2_18:                                ##   in Loop: Header=BB2_13 Depth=1
	cmpq	$150, %r14
	ja	LBB2_21
## %bb.19:                              ##   in Loop: Header=BB2_13 Depth=1
	leal	261(%rax), %r10d
	pxor	%xmm0, %xmm0
	jmp	LBB2_20
LBB2_21:                                ##   in Loop: Header=BB2_13 Depth=1
	pxor	%xmm0, %xmm0
	cmpq	$202, %r14
	ja	LBB2_25
## %bb.22:                              ##   in Loop: Header=BB2_13 Depth=1
	leal	261(%rax), %r10d
	jmp	LBB2_23
LBB2_27:
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
	cmpq	%rsi, %r9
	jae	LBB2_30
## %bb.28:
	movdqa	LCPI2_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI2_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	movq	-48(%rbp), %rdi                 ## 8-byte Reload
	.p2align	4
LBB2_29:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%rdi,%r9), %eax
	xorl	%ebx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm2
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	xorl	%ecx, %eax
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%eax, %ebx
	incq	%r9
	cmpq	%r9, %rsi
	jne	LBB2_29
LBB2_30:
	notl	%ebx
	movl	%ebx, %eax
	addq	$4248, %rsp                     ## imm = 0x1098
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI3_0:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI3_1:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
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
	subq	$2168, %rsp                     ## imm = 0x878
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdi, %r13
	cmpq	$207, %rsi
	ja	LBB3_1
## %bb.195:
	movq	%r13, %rdi
	addq	$2168, %rsp                     ## imm = 0x878
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	jmp	__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj ## TAILCALL
LBB3_1:
	leaq	-1088(%rbp), %rdi
	movq	%rsi, %r14
	movl	$788, %esi                      ## imm = 0x314
	movl	%edx, %ebx
	callq	___bzero
	xorps	%xmm0, %xmm0
	movups	%xmm0, -104(%rbp)
	movups	%xmm0, -120(%rbp)
	movups	%xmm0, -136(%rbp)
	movups	%xmm0, -152(%rbp)
	movups	%xmm0, -168(%rbp)
	movups	%xmm0, -184(%rbp)
	movups	%xmm0, -200(%rbp)
	movups	%xmm0, -216(%rbp)
	movups	%xmm0, -232(%rbp)
	movups	%xmm0, -248(%rbp)
	xorl	(%r13), %ebx
	notl	%ebx
	movl	%ebx, -296(%rbp)
	movaps	%xmm0, -1136(%rbp)
	movups	4(%r13), %xmm1
	movups	%xmm1, -292(%rbp)
	movaps	%xmm0, -1120(%rbp)
	movups	20(%r13), %xmm1
	movups	%xmm1, -276(%rbp)
	movaps	%xmm0, -1104(%rbp)
	movl	52(%r13), %edi
	movq	-1136(%rbp), %rcx
	movq	-1128(%rbp), %rax
	movq	-1080(%rbp), %rdx
	movdqu	36(%r13), %xmm0
	movl	-248(%rbp), %esi
	xorl	%ecx, %esi
	movd	%esi, %xmm1
	pslldq	$12, %xmm1                      ## xmm1 = zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,zero,xmm1[0,1,2,3]
	pxor	%xmm0, %xmm1
	movl	56(%r13), %r8d
	xorl	%edx, %r8d
	shrq	$32, %rdx
	xorl	60(%r13), %edx
	movdqu	%xmm1, -260(%rbp)
	shrq	$32, %rcx
	xorl	-244(%rbp), %ecx
	xorl	%ecx, %edi
	movl	%edi, -244(%rbp)
	movl	-240(%rbp), %edi
	xorl	%eax, %edi
	xorl	%edi, %r8d
	movl	%r8d, -240(%rbp)
	shrq	$32, %rax
	xorl	-236(%rbp), %eax
	xorl	%eax, %edx
	movl	%edx, -236(%rbp)
	movl	%esi, -1088(%rbp)
	movl	%ecx, -1084(%rbp)
	movl	%edi, -1080(%rbp)
	movl	%eax, -1076(%rbp)
	movq	-1120(%rbp), %r8
	movq	-1112(%rbp), %rdx
	movq	-836(%rbp), %rcx
	movq	-828(%rbp), %rax
	movq	-1072(%rbp), %r9
	movq	-1064(%rbp), %rsi
	movl	-232(%rbp), %edi
	xorl	%r8d, %edi
	movl	64(%r13), %r10d
	xorl	%r9d, %r10d
	shrq	$32, %r9
	xorl	68(%r13), %r9d
	xorl	%edi, %r10d
	movl	72(%r13), %r11d
	xorl	%esi, %r11d
	shrq	$32, %rsi
	xorl	76(%r13), %esi
	movl	%r10d, -232(%rbp)
	movl	%ecx, %r10d
	shrq	$32, %r8
	xorl	-228(%rbp), %r8d
	shrq	$32, %rcx
	xorl	%r8d, %ecx
	xorl	%r9d, %r8d
	movl	%r8d, -228(%rbp)
	movl	-224(%rbp), %r8d
	xorl	%edx, %r8d
	movl	%eax, %r9d
	xorl	%r8d, %r9d
	xorl	%r11d, %r8d
	movl	%r8d, -224(%rbp)
	shrq	$32, %rdx
	xorl	-220(%rbp), %edx
	xorl	%edi, %r10d
	shrq	$32, %rax
	xorl	%edx, %eax
	xorl	%esi, %edx
	movl	%edx, -220(%rbp)
	movl	%r10d, -1072(%rbp)
	movl	%ecx, -1068(%rbp)
	movl	%r9d, -1064(%rbp)
	movl	%eax, -1060(%rbp)
	movl	-216(%rbp), %eax
	xorl	-1104(%rbp), %eax
	movl	80(%r13), %ecx
	movdqa	-1040(%rbp), %xmm2
	movdqa	-1104(%rbp), %xmm7
	movdqa	-1088(%rbp), %xmm1
	movdqa	-1072(%rbp), %xmm0
	movdqa	-1056(%rbp), %xmm3
	xorl	-1056(%rbp), %ecx
	xorl	%eax, %ecx
	movl	%ecx, -216(%rbp)
	movdqa	%xmm1, %xmm5
	psrlq	$32, %xmm5
	movdqa	%xmm7, %xmm6
	psrlq	$32, %xmm6
	movups	-212(%rbp), %xmm10
	movups	-196(%rbp), %xmm11
	movdqa	%xmm2, %xmm9
	psrlq	$32, %xmm9
	movdqa	%xmm3, %xmm8
	psrlq	$32, %xmm8
	movups	84(%r13), %xmm4
	shufps	$78, %xmm1, %xmm6               ## xmm6 = xmm6[2,3],xmm1[0,1]
	movsd	%xmm5, %xmm1                    ## xmm1 = xmm5[0],xmm1[1]
	pshufd	$238, %xmm5, %xmm5              ## xmm5 = xmm5[2,3,2,3]
	punpcklqdq	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0]
	shufps	$136, %xmm5, %xmm1              ## xmm1 = xmm1[0,2],xmm5[0,2]
	movups	100(%r13), %xmm12
	xorps	%xmm11, %xmm1
	shufps	$137, %xmm6, %xmm7              ## xmm7 = xmm7[1,2],xmm6[0,2]
	xorps	%xmm10, %xmm7
	movdqa	-1024(%rbp), %xmm6
	movdqa	-1008(%rbp), %xmm5
	movups	-820(%rbp), %xmm10
	xorl	-820(%rbp), %eax
	shufps	$78, %xmm2, %xmm8               ## xmm8 = xmm8[2,3],xmm2[0,1]
	movsd	%xmm9, %xmm2                    ## xmm2 = xmm9[0],xmm2[1]
	pshufd	$238, %xmm9, %xmm9              ## xmm9 = xmm9[2,3,2,3]
	punpcklqdq	%xmm6, %xmm9            ## xmm9 = xmm9[0],xmm6[0]
	shufps	$136, %xmm9, %xmm2              ## xmm2 = xmm2[0,2],xmm9[0,2]
	movq	-1064(%rbp), %xmm9              ## xmm9 = mem[0],zero
	movl	%eax, -1056(%rbp)
	xorps	%xmm12, %xmm2
	shufps	$137, %xmm8, %xmm3              ## xmm3 = xmm3[1,2],xmm8[0,2]
	xorps	%xmm4, %xmm3
	xorps	%xmm1, %xmm2
	xorps	%xmm7, %xmm3
	movups	%xmm3, -212(%rbp)
	movups	%xmm2, -196(%rbp)
	punpcklqdq	%xmm9, %xmm0            ## xmm0 = xmm0[0],xmm9[0]
	movups	116(%r13), %xmm8
	movups	132(%r13), %xmm9
	movq	-716(%rbp), %rax
	movq	-960(%rbp), %rdi
	movq	-952(%rbp), %rcx
	movdqa	-992(%rbp), %xmm3
	movdqa	-976(%rbp), %xmm2
	movups	-804(%rbp), %xmm11
	movaps	%xmm10, %xmm4
	shufps	$78, %xmm11, %xmm4              ## xmm4 = xmm4[2,3],xmm11[0,1]
	shufps	$153, %xmm4, %xmm10             ## xmm10 = xmm10[1,2],xmm4[1,2]
	movups	-788(%rbp), %xmm4
	xorps	%xmm7, %xmm10
	movq	%rdi, %rdx
	shrq	$32, %rdx
	xorl	180(%r13), %edx
	movl	184(%r13), %esi
	xorl	%ecx, %esi
	shrq	$32, %rcx
	xorl	188(%r13), %ecx
	movups	%xmm10, -1052(%rbp)
	movups	-796(%rbp), %xmm7
	shufps	$153, %xmm7, %xmm11             ## xmm11 = xmm11[1,2],xmm7[1,2]
	movdqa	-1056(%rbp), %xmm7
	movdqa	%xmm0, %xmm10
	psrlq	$32, %xmm10
	xorps	%xmm1, %xmm11
	movdqa	%xmm7, %xmm12
	psrlq	$32, %xmm12
	movups	%xmm11, -1036(%rbp)
	movdqa	-1040(%rbp), %xmm1
	shufps	$78, %xmm7, %xmm10              ## xmm10 = xmm10[2,3],xmm7[0,1]
	movsd	%xmm12, %xmm7                   ## xmm7 = xmm12[0],xmm7[1]
	pshufd	$238, %xmm12, %xmm11            ## xmm11 = xmm12[2,3,2,3]
	punpcklqdq	%xmm1, %xmm11           ## xmm11 = xmm11[0],xmm1[0]
	shufps	$136, %xmm11, %xmm7             ## xmm7 = xmm7[0,2],xmm11[0,2]
	movups	-164(%rbp), %xmm11
	xorps	%xmm11, %xmm7
	movups	-180(%rbp), %xmm11
	shufps	$137, %xmm10, %xmm0             ## xmm0 = xmm0[1,2],xmm10[0,2]
	movdqa	%xmm5, %xmm10
	psrlq	$32, %xmm10
	xorps	%xmm11, %xmm0
	movdqa	%xmm6, %xmm11
	psrlq	$32, %xmm11
	shufps	$78, %xmm5, %xmm11              ## xmm11 = xmm11[2,3],xmm5[0,1]
	movsd	%xmm10, %xmm5                   ## xmm5 = xmm10[0],xmm5[1]
	pshufd	$238, %xmm10, %xmm10            ## xmm10 = xmm10[2,3,2,3]
	punpcklqdq	%xmm3, %xmm10           ## xmm10 = xmm10[0],xmm3[0]
	shufps	$136, %xmm10, %xmm5             ## xmm5 = xmm5[0,2],xmm10[0,2]
	xorps	%xmm9, %xmm5
	shufps	$137, %xmm11, %xmm6             ## xmm6 = xmm6[1,2],xmm11[0,2]
	xorps	%xmm8, %xmm6
	xorps	%xmm7, %xmm5
	xorps	%xmm0, %xmm6
	movups	%xmm6, -180(%rbp)
	movq	-1032(%rbp), %xmm6              ## xmm6 = mem[0],zero
	movups	%xmm5, -164(%rbp)
	punpcklqdq	%xmm6, %xmm1            ## xmm1 = xmm1[0],xmm6[0]
	movups	-772(%rbp), %xmm6
	movaps	%xmm4, %xmm5
	shufps	$78, %xmm6, %xmm5               ## xmm5 = xmm5[2,3],xmm6[0,1]
	shufps	$153, %xmm5, %xmm4              ## xmm4 = xmm4[1,2],xmm5[1,2]
	movq	%rdi, %xmm8
	xorps	%xmm0, %xmm4
	movups	-756(%rbp), %xmm5
	movups	%xmm4, -1020(%rbp)
	movups	-764(%rbp), %xmm0
	shufps	$153, %xmm0, %xmm6              ## xmm6 = xmm6[1,2],xmm0[1,2]
	movdqa	-1024(%rbp), %xmm0
	xorps	%xmm7, %xmm6
	movdqa	%xmm0, %xmm4
	psrlq	$32, %xmm4
	movups	%xmm6, -1004(%rbp)
	movq	-1008(%rbp), %rdi
	movq	%rdi, %xmm6
	shufps	$78, %xmm6, %xmm4               ## xmm4 = xmm4[2,3],xmm6[0,1]
	movdqa	%xmm1, %xmm6
	psrlq	$32, %xmm6
	shufps	$78, %xmm0, %xmm6               ## xmm6 = xmm6[2,3],xmm0[0,1]
	shufps	$137, %xmm6, %xmm1              ## xmm1 = xmm1[1,2],xmm6[0,2]
	movups	-148(%rbp), %xmm6
	xorps	%xmm6, %xmm1
	movups	-132(%rbp), %xmm6
	shufps	$137, %xmm4, %xmm0              ## xmm0 = xmm0[1,2],xmm4[0,2]
	movdqa	%xmm2, %xmm4
	psrlq	$32, %xmm4
	xorps	%xmm6, %xmm0
	movdqa	%xmm3, %xmm6
	psrlq	$32, %xmm6
	shufps	$78, %xmm2, %xmm6               ## xmm6 = xmm6[2,3],xmm2[0,1]
	movsd	%xmm4, %xmm2                    ## xmm2 = xmm4[0],xmm2[1]
	pshufd	$238, %xmm4, %xmm4              ## xmm4 = xmm4[2,3,2,3]
	punpcklqdq	%xmm8, %xmm4            ## xmm4 = xmm4[0],xmm8[0]
	shufps	$136, %xmm4, %xmm2              ## xmm2 = xmm2[0,2],xmm4[0,2]
	movups	164(%r13), %xmm4
	xorps	%xmm4, %xmm2
	shufps	$137, %xmm6, %xmm3              ## xmm3 = xmm3[1,2],xmm6[0,2]
	movups	148(%r13), %xmm4
	xorps	%xmm4, %xmm3
	xorps	%xmm0, %xmm2
	xorps	%xmm1, %xmm3
	movups	%xmm3, -148(%rbp)
	movups	%xmm2, -132(%rbp)
	movq	-740(%rbp), %r10
	movq	%r10, %xmm2
	movaps	%xmm5, %xmm3
	shufps	$79, %xmm2, %xmm3               ## xmm3 = xmm3[3,3],xmm2[0,1]
	movq	-1000(%rbp), %r9
	movq	-724(%rbp), %r8
	movq	-732(%rbp), %r11
	shufps	$137, %xmm3, %xmm5              ## xmm5 = xmm5[1,2],xmm3[0,2]
	movd	%r11d, %xmm2
	shrq	$32, %r11
	shrq	$32, %r10
	xorps	%xmm1, %xmm5
	movups	%xmm5, -988(%rbp)
	movd	%r8d, %xmm1
	movd	%r11d, %xmm3
	punpckldq	%xmm1, %xmm3            ## xmm3 = xmm3[0],xmm1[0],xmm3[1],xmm1[1]
	movd	%r10d, %xmm1
	punpckldq	%xmm2, %xmm1            ## xmm1 = xmm1[0],xmm2[0],xmm1[1],xmm2[1]
	punpcklqdq	%xmm3, %xmm1            ## xmm1 = xmm1[0],xmm3[0]
	pxor	%xmm0, %xmm1
	shrq	$32, %rdi
	xorl	-116(%rbp), %edi
	movdqu	%xmm1, -972(%rbp)
	xorl	%edi, %edx
	movl	%edx, -116(%rbp)
	movl	-112(%rbp), %edx
	xorl	%r9d, %edx
	movl	%eax, %r10d
	xorl	%edx, %r10d
	xorl	%esi, %edx
	movl	%edx, -112(%rbp)
	shrq	$32, %r9
	xorl	-108(%rbp), %r9d
	shrq	$32, %rax
	xorl	%r9d, %eax
	xorl	%ecx, %r9d
	movl	%r9d, -108(%rbp)
	shrq	$32, %r8
	xorl	%edi, %r8d
	movl	%r8d, -956(%rbp)
	movl	%r10d, -952(%rbp)
	movl	%eax, -948(%rbp)
	movq	-992(%rbp), %r8
	movq	-984(%rbp), %rdx
	movq	-708(%rbp), %rcx
	movq	-700(%rbp), %rax
	movq	-944(%rbp), %r9
	movq	-936(%rbp), %rsi
	movl	-104(%rbp), %r10d
	xorl	%r8d, %r10d
	movl	%ecx, %edi
	xorl	%r10d, %edi
	movl	192(%r13), %r11d
	xorl	%r9d, %r11d
	shrq	$32, %r9
	xorl	196(%r13), %r9d
	xorl	%r10d, %r11d
	movl	200(%r13), %r10d
	xorl	%esi, %r10d
	shrq	$32, %rsi
	xorl	204(%r13), %esi
	movl	%r11d, -104(%rbp)
	shrq	$32, %r8
	xorl	-100(%rbp), %r8d
	shrq	$32, %rcx
	xorl	%r8d, %ecx
	xorl	%r9d, %r8d
	movl	%r8d, -100(%rbp)
	movl	-96(%rbp), %r8d
	xorl	%edx, %r8d
	movl	%eax, %r9d
	xorl	%r8d, %r9d
	xorl	%r10d, %r8d
	movl	%r8d, -96(%rbp)
	shrq	$32, %rdx
	xorl	-92(%rbp), %edx
	shrq	$32, %rax
	xorl	%edx, %eax
	xorl	%esi, %edx
	movl	%edx, -92(%rbp)
	movl	%edi, -944(%rbp)
	movl	%ecx, -940(%rbp)
	movl	%r9d, -936(%rbp)
	movl	%eax, -932(%rbp)
	cmpq	$416, %r14                      ## imm = 0x1A0
	movq	%r13, -64(%rbp)                 ## 8-byte Spill
	movq	%r14, -1144(%rbp)               ## 8-byte Spill
	jb	LBB3_2
## %bb.6:
	leaq	-208(%r14), %rcx
	movl	$52, %r12d
	movl	$208, %eax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	xorl	%r10d, %r10d
	jmp	LBB3_7
	.p2align	4
LBB3_162:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	movl	%r14d, -1132(%rbp,%rcx,4)
	movl	%eax, -1128(%rbp,%rcx,4)
	movl	%edi, -1124(%rbp,%rcx,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %r10d
	cmovel	%r10d, %r12d
LBB3_163:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %r13                 ## 8-byte Reload
	movq	-88(%rbp), %rax                 ## 8-byte Reload
	addq	$208, -80(%rbp)                 ## 8-byte Folded Spill
	addq	$-208, %rax
	cmpq	$207, %rax
	movq	%rax, %rcx
	jbe	LBB3_3
LBB3_7:                                 ## =>This Inner Loop Header: Depth=1
	movq	%rcx, -88(%rbp)                 ## 8-byte Spill
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_8
## %bb.9:                               ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rdx
	addq	%rbp, %rdx
	leaq	-1128(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1124(,%rax,4), %rsi
	addq	%rbp, %rsi
	jmp	LBB3_10
	.p2align	4
LBB3_8:                                 ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %esi
	cmpl	$209, %esi
	cmovel	%r10d, %esi
	leaq	-1136(,%rsi,4), %rdx
	addq	%rbp, %rdx
	incl	%esi
	cmpl	$209, %esi
	cmovel	%r10d, %esi
	leaq	-1136(,%rsi,4), %rcx
	addq	%rbp, %rcx
	incl	%esi
	cmpl	$209, %esi
	cmovel	%r10d, %esi
	leaq	-1136(,%rsi,4), %rsi
	addq	%rbp, %rsi
LBB3_10:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%r12d, %r8d
	subl	$150, %r8d
	leal	209(%r8), %edi
	cmovael	%r8d, %edi
	cmpl	$206, %edi
	jae	LBB3_11
## %bb.12:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rdi,4), %r8
	addq	%rbp, %r8
	leaq	-1128(,%rdi,4), %r11
	addq	%rbp, %r11
	leaq	-1124(,%rdi,4), %r14
	addq	%rbp, %r14
	jmp	LBB3_13
	.p2align	4
LBB3_11:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rdi), %r9d
	cmpl	$209, %r9d
	cmovel	%r10d, %r9d
	leaq	-1136(,%r9,4), %r8
	addq	%rbp, %r8
	incl	%r9d
	cmpl	$209, %r9d
	cmovel	%r10d, %r9d
	leaq	-1136(,%r9,4), %r11
	addq	%rbp, %r11
	incl	%r9d
	cmpl	$209, %r9d
	cmovel	%r10d, %r9d
	leaq	-1136(,%r9,4), %r14
	addq	%rbp, %r14
LBB3_13:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rcx), %r9d
	movl	(%rsi), %ecx
	movl	-1136(%rbp,%rax,4), %ebx
	movl	(%rdx), %r10d
	movl	(%r11), %esi
	movl	(%r14), %eax
	movl	%eax, -44(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rdi,4), %edi
	movl	(%r8), %eax
	movl	%r12d, %r8d
	cmpl	$206, %r12d
	jae	LBB3_14
## %bb.15:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r8,4), %r14
	movq	-1128(%rbp,%r8,4), %r15
	movq	%r14, %r11
	shrq	$32, %r11
	movq	%r13, %rdx
	jmp	LBB3_16
	.p2align	4
LBB3_14:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %r15d
	cmpl	$209, %r15d
	movl	$0, %r14d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%r15,4), %r11d
	incl	%r15d
	cmpl	$209, %r15d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%r15,4), %r13d
	incl	%r15d
	cmpl	$209, %r15d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%r8,4), %r14d
	movl	-1136(%rbp,%r15,4), %r15d
	shlq	$32, %r15
	orq	%r13, %r15
	movq	-64(%rbp), %rdx                 ## 8-byte Reload
LBB3_16:                                ##   in Loop: Header=BB3_7 Depth=1
	xorl	-296(%rbp), %ebx
	xorl	%ebx, %edi
	movq	-80(%rbp), %r13                 ## 8-byte Reload
	xorl	(%rdx,%r13), %r14d
	xorl	%ebx, %r14d
	xorl	-292(%rbp), %r10d
	xorl	4(%rdx,%r13), %r11d
	xorl	%r10d, %eax
	xorl	%r10d, %r11d
	xorl	-288(%rbp), %r9d
	xorl	%r9d, %esi
	movl	8(%rdx,%r13), %r10d
	xorl	%r15d, %r10d
	xorl	%r9d, %r10d
	xorl	-284(%rbp), %ecx
	shrq	$32, %r15
	movl	-44(%rbp), %r9d                 ## 4-byte Reload
	xorl	%ecx, %r9d
	xorl	12(%rdx,%r13), %r15d
	xorl	%ecx, %r15d
	movl	%r14d, -296(%rbp)
	movl	%r11d, -292(%rbp)
	movl	%r10d, -288(%rbp)
	movl	%r15d, -284(%rbp)
	cmpl	$206, %r12d
	movq	%rdx, %r13
	jae	LBB3_17
## %bb.18:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%edi, -1136(%rbp,%r8,4)
	movl	%eax, -1132(%rbp,%r8,4)
	movl	%esi, -1128(%rbp,%r8,4)
	movl	%r9d, -1124(%rbp,%r8,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %r11d
	cmovel	%r11d, %r12d
	jmp	LBB3_19
	.p2align	4
LBB3_17:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%edi, -1136(%rbp,%r8,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %r11d
	cmovel	%r11d, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r11d, %r12d
	movl	%esi, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r11d, %r12d
	movl	%r9d, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_19:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_20
## %bb.21:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdx
	addq	%rbp, %rdx
	leaq	-1124(,%rax,4), %rdi
	addq	%rbp, %rdi
	jmp	LBB3_22
	.p2align	4
LBB3_20:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %esi
	cmpl	$209, %esi
	cmovel	%r11d, %esi
	leaq	-1136(,%rsi,4), %rcx
	addq	%rbp, %rcx
	incl	%esi
	cmpl	$209, %esi
	cmovel	%r11d, %esi
	leaq	-1136(,%rsi,4), %rdx
	addq	%rbp, %rdx
	incl	%esi
	cmpl	$209, %esi
	cmovel	%r11d, %esi
	leaq	-1136(,%rsi,4), %rdi
	addq	%rbp, %rdi
LBB3_22:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdx), %esi
	movl	(%rdi), %edx
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %eax
	movl	%r12d, %edi
	subl	$150, %edi
	leal	209(%rdi), %ecx
	cmovael	%edi, %ecx
	cmpl	$206, %ecx
	jae	LBB3_23
## %bb.24:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %r10
	addq	%rbp, %r10
	leaq	-1128(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rcx,4), %r11
	addq	%rbp, %r11
	jmp	LBB3_25
	.p2align	4
LBB3_23:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%r11d, %r8d
	leaq	-1136(,%r8,4), %r10
	addq	%rbp, %r10
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%r11d, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%r11d, %r8d
	leaq	-1136(,%r8,4), %r11
	addq	%rbp, %r11
LBB3_25:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r8d
	movl	(%r11), %edi
	movl	%edi, -44(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %r11d
	movl	(%r10), %r10d
	movl	%r12d, %ebx
	cmpl	$206, %r12d
	jae	LBB3_26
## %bb.27:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%rbx,4), %r14
	movq	-1128(%rbp,%rbx,4), %r15
	movq	%r14, %rcx
	shrq	$32, %rcx
	movq	%r13, %rdi
	jmp	LBB3_28
	.p2align	4
LBB3_26:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %r15d
	cmpl	$209, %r15d
	movl	$0, %r14d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%r15,4), %ecx
	incl	%r15d
	cmpl	$209, %r15d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%r15,4), %r13d
	incl	%r15d
	cmpl	$209, %r15d
	cmovel	%r14d, %r15d
	movl	-1136(%rbp,%rbx,4), %r14d
	movl	-1136(%rbp,%r15,4), %r15d
	shlq	$32, %r15
	orq	%r13, %r15
	movq	-64(%rbp), %rdi                 ## 8-byte Reload
LBB3_28:                                ##   in Loop: Header=BB3_7 Depth=1
	xorl	-280(%rbp), %r9d
	xorl	%r9d, %r11d
	movq	-80(%rbp), %r13                 ## 8-byte Reload
	xorl	16(%rdi,%r13), %r14d
	xorl	%r9d, %r14d
	xorl	-276(%rbp), %eax
	xorl	20(%rdi,%r13), %ecx
	xorl	%eax, %r10d
	xorl	%eax, %ecx
	xorl	-272(%rbp), %esi
	xorl	%esi, %r8d
	movl	24(%rdi,%r13), %eax
	xorl	%r15d, %eax
	xorl	%esi, %eax
	xorl	-268(%rbp), %edx
	shrq	$32, %r15
	movl	-44(%rbp), %esi                 ## 4-byte Reload
	xorl	%edx, %esi
	xorl	28(%rdi,%r13), %r15d
	xorl	%edx, %r15d
	movl	%r14d, -280(%rbp)
	movl	%ecx, -276(%rbp)
	movl	%eax, -272(%rbp)
	movl	%r15d, -268(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_29
## %bb.30:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%r11d, -1136(%rbp,%rbx,4)
	movl	%r10d, -1132(%rbp,%rbx,4)
	movl	%r8d, -1128(%rbp,%rbx,4)
	movl	%esi, -1124(%rbp,%rbx,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_31
	.p2align	4
LBB3_29:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%r11d, -1136(%rbp,%rbx,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r10d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%r8d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%esi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_31:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movl	32(%rdi,%rax), %ecx
	movl	%ecx, -52(%rbp)                 ## 4-byte Spill
	movl	36(%rdi,%rax), %ecx
	movl	%ecx, -44(%rbp)                 ## 4-byte Spill
	movl	40(%rdi,%rax), %ecx
	movl	%ecx, -72(%rbp)                 ## 4-byte Spill
	movl	44(%rdi,%rax), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_32
## %bb.33:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_34
	.p2align	4
LBB3_32:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_34:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_35
## %bb.36:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_37
	.p2align	4
LBB3_35:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_37:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_38
## %bb.39:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_40
	.p2align	4
LBB3_38:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_40:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-264(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-260(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-256(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-252(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -264(%rbp)
	movl	%r10d, -260(%rbp)
	movl	%edx, -256(%rbp)
	movl	%esi, -252(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_41
## %bb.42:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_43
	.p2align	4
LBB3_41:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_43:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	48(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	52(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	56(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	60(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_44
## %bb.45:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_46
	.p2align	4
LBB3_44:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_46:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_47
## %bb.48:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_49
	.p2align	4
LBB3_47:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_49:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_50
## %bb.51:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_52
	.p2align	4
LBB3_50:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_52:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-248(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-244(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-240(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-236(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -248(%rbp)
	movl	%r10d, -244(%rbp)
	movl	%edx, -240(%rbp)
	movl	%esi, -236(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_53
## %bb.54:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_55
	.p2align	4
LBB3_53:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_55:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	64(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	68(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	72(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	76(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_56
## %bb.57:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_58
	.p2align	4
LBB3_56:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_58:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_59
## %bb.60:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_61
	.p2align	4
LBB3_59:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_61:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_62
## %bb.63:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_64
	.p2align	4
LBB3_62:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_64:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-232(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-228(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-224(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-220(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -232(%rbp)
	movl	%r10d, -228(%rbp)
	movl	%edx, -224(%rbp)
	movl	%esi, -220(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_65
## %bb.66:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_67
	.p2align	4
LBB3_65:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_67:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	80(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	84(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	88(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	92(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_68
## %bb.69:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_70
	.p2align	4
LBB3_68:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_70:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_71
## %bb.72:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_73
	.p2align	4
LBB3_71:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_73:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_74
## %bb.75:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_76
	.p2align	4
LBB3_74:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_76:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-216(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-212(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-208(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-204(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -216(%rbp)
	movl	%r10d, -212(%rbp)
	movl	%edx, -208(%rbp)
	movl	%esi, -204(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_77
## %bb.78:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_79
	.p2align	4
LBB3_77:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_79:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	96(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	100(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	104(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	108(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_80
## %bb.81:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_82
	.p2align	4
LBB3_80:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_82:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_83
## %bb.84:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_85
	.p2align	4
LBB3_83:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_85:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_86
## %bb.87:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_88
	.p2align	4
LBB3_86:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_88:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-200(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-196(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-192(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-188(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -200(%rbp)
	movl	%r10d, -196(%rbp)
	movl	%edx, -192(%rbp)
	movl	%esi, -188(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_89
## %bb.90:                              ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_91
	.p2align	4
LBB3_89:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_91:                                ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	112(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	116(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	120(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	124(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_92
## %bb.93:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_94
	.p2align	4
LBB3_92:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_94:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_95
## %bb.96:                              ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_97
	.p2align	4
LBB3_95:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_97:                                ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_98
## %bb.99:                              ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_100
	.p2align	4
LBB3_98:                                ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_100:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-184(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-180(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-176(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-172(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -184(%rbp)
	movl	%r10d, -180(%rbp)
	movl	%edx, -176(%rbp)
	movl	%esi, -172(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_101
## %bb.102:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_103
	.p2align	4
LBB3_101:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_103:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	128(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	132(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	136(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	140(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_104
## %bb.105:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_106
	.p2align	4
LBB3_104:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_106:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	-1136(%rbp,%rax,4), %r9d
	movl	(%rcx), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_107
## %bb.108:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_109
	.p2align	4
LBB3_107:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_109:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%rcx,4), %ecx
	movl	(%rdi), %r14d
	movl	%r12d, %r15d
	cmpl	$206, %r12d
	jae	LBB3_110
## %bb.111:                             ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%r15,4), %rbx
	movq	-1128(%rbp,%r15,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_112
	.p2align	4
LBB3_110:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%r15,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_112:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-168(%rbp), %r9d
	xorl	%r9d, %ecx
	xorl	%r8d, %r9d
	xorl	-164(%rbp), %r10d
	xorl	%r10d, %r14d
	xorl	%edx, %r10d
	xorl	-160(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-156(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r9d, -168(%rbp)
	movl	%r10d, -164(%rbp)
	movl	%edx, -160(%rbp)
	movl	%esi, -156(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_113
## %bb.114:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	movl	%r14d, -1132(%rbp,%r15,4)
	movl	%eax, -1128(%rbp,%r15,4)
	movl	%edi, -1124(%rbp,%r15,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_115
	.p2align	4
LBB3_113:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%ecx, -1136(%rbp,%r15,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_115:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	144(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	148(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	152(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	156(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_116
## %bb.117:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_118
	.p2align	4
LBB3_116:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_118:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	(%rcx), %r9d
	movl	-1136(%rbp,%rax,4), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_119
## %bb.120:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_121
	.p2align	4
LBB3_119:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_121:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	(%rdi), %r14d
	movl	-1136(%rbp,%rcx,4), %r15d
	movl	%r12d, %ecx
	cmpl	$206, %r12d
	jae	LBB3_122
## %bb.123:                             ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%rcx,4), %rbx
	movq	-1128(%rbp,%rcx,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_124
	.p2align	4
LBB3_122:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rcx,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_124:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-152(%rbp), %r10d
	xorl	%r10d, %r15d
	xorl	%r8d, %r10d
	xorl	-148(%rbp), %r9d
	xorl	%r9d, %r14d
	xorl	%edx, %r9d
	xorl	-144(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-140(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r10d, -152(%rbp)
	movl	%r9d, -148(%rbp)
	movl	%edx, -144(%rbp)
	movl	%esi, -140(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_125
## %bb.126:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	movl	%r14d, -1132(%rbp,%rcx,4)
	movl	%eax, -1128(%rbp,%rcx,4)
	movl	%edi, -1124(%rbp,%rcx,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_127
	.p2align	4
LBB3_125:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_127:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	160(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	164(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	168(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	172(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_128
## %bb.129:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_130
	.p2align	4
LBB3_128:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_130:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	(%rcx), %r9d
	movl	-1136(%rbp,%rax,4), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_131
## %bb.132:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_133
	.p2align	4
LBB3_131:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_133:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	(%rdi), %r14d
	movl	-1136(%rbp,%rcx,4), %r15d
	movl	%r12d, %ecx
	cmpl	$206, %r12d
	jae	LBB3_134
## %bb.135:                             ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%rcx,4), %rbx
	movq	-1128(%rbp,%rcx,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_136
	.p2align	4
LBB3_134:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rcx,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_136:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-136(%rbp), %r10d
	xorl	%r10d, %r15d
	xorl	%r8d, %r10d
	xorl	-132(%rbp), %r9d
	xorl	%r9d, %r14d
	xorl	%edx, %r9d
	xorl	-128(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-124(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r10d, -136(%rbp)
	movl	%r9d, -132(%rbp)
	movl	%edx, -128(%rbp)
	movl	%esi, -124(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_137
## %bb.138:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	movl	%r14d, -1132(%rbp,%rcx,4)
	movl	%eax, -1128(%rbp,%rcx,4)
	movl	%edi, -1124(%rbp,%rcx,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_139
	.p2align	4
LBB3_137:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_139:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	176(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	180(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	184(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	188(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_140
## %bb.141:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_142
	.p2align	4
LBB3_140:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_142:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	(%rcx), %r9d
	movl	-1136(%rbp,%rax,4), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_143
## %bb.144:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_145
	.p2align	4
LBB3_143:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_145:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	(%rdi), %r14d
	movl	-1136(%rbp,%rcx,4), %r15d
	movl	%r12d, %ecx
	cmpl	$206, %r12d
	jae	LBB3_146
## %bb.147:                             ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%rcx,4), %rbx
	movq	-1128(%rbp,%rcx,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_148
	.p2align	4
LBB3_146:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rcx,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_148:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-120(%rbp), %r10d
	xorl	%r10d, %r15d
	xorl	%r8d, %r10d
	xorl	-116(%rbp), %r9d
	xorl	%r9d, %r14d
	xorl	%edx, %r9d
	xorl	-112(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-108(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r10d, -120(%rbp)
	movl	%r9d, -116(%rbp)
	movl	%edx, -112(%rbp)
	movl	%esi, -108(%rbp)
	cmpl	$206, %r12d
	jae	LBB3_149
## %bb.150:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	movl	%r14d, -1132(%rbp,%rcx,4)
	movl	%eax, -1128(%rbp,%rcx,4)
	movl	%edi, -1124(%rbp,%rcx,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	jmp	LBB3_151
	.p2align	4
LBB3_149:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %ebx
	cmovel	%ebx, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%ebx, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
LBB3_151:                               ##   in Loop: Header=BB3_7 Depth=1
	movq	-64(%rbp), %rax                 ## 8-byte Reload
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	movl	192(%rax,%rcx), %edx
	movl	%edx, -52(%rbp)                 ## 4-byte Spill
	movl	196(%rax,%rcx), %edx
	movl	%edx, -44(%rbp)                 ## 4-byte Spill
	movl	200(%rax,%rcx), %edx
	movl	%edx, -72(%rbp)                 ## 4-byte Spill
	movl	204(%rax,%rcx), %esi
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_152
## %bb.153:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rax,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%rax,4), %rdi
	addq	%rbp, %rdi
	leaq	-1124(,%rax,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_154
	.p2align	4
LBB3_152:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rax), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rcx
	addq	%rbp, %rcx
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_154:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rdi), %r13d
	movl	(%r8), %r11d
	movl	(%rcx), %r9d
	movl	-1136(%rbp,%rax,4), %r10d
	movl	%r12d, %eax
	subl	$150, %eax
	leal	209(%rax), %ecx
	cmovael	%eax, %ecx
	cmpl	$206, %ecx
	jae	LBB3_155
## %bb.156:                             ##   in Loop: Header=BB3_7 Depth=1
	leaq	-1132(,%rcx,4), %rdi
	addq	%rbp, %rdi
	leaq	-1128(,%rcx,4), %rax
	addq	%rbp, %rax
	leaq	-1124(,%rcx,4), %r8
	addq	%rbp, %r8
	jmp	LBB3_157
	.p2align	4
LBB3_155:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%rcx), %r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rdi
	addq	%rbp, %rdi
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %rax
	addq	%rbp, %rax
	incl	%r8d
	cmpl	$209, %r8d
	cmovel	%ebx, %r8d
	leaq	-1136(,%r8,4), %r8
	addq	%rbp, %r8
LBB3_157:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	(%rax), %eax
	movl	(%r8), %edx
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	(%rdi), %r14d
	movl	-1136(%rbp,%rcx,4), %r15d
	movl	%r12d, %ecx
	cmpl	$206, %r12d
	jae	LBB3_158
## %bb.159:                             ##   in Loop: Header=BB3_7 Depth=1
	movq	-1136(%rbp,%rcx,4), %rbx
	movq	-1128(%rbp,%rcx,4), %rdi
	movq	%rbx, %r8
	shrq	$32, %r8
	jmp	LBB3_160
	.p2align	4
LBB3_158:                               ##   in Loop: Header=BB3_7 Depth=1
	leal	1(%r12), %edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %r8d
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rdi,4), %edx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%ebx, %edi
	movl	-1136(%rbp,%rcx,4), %ebx
	movl	-1136(%rbp,%rdi,4), %edi
	shlq	$32, %rdi
	orq	%rdx, %rdi
LBB3_160:                               ##   in Loop: Header=BB3_7 Depth=1
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%r8d, %edx
	movl	-52(%rbp), %r8d                 ## 4-byte Reload
	xorl	%ebx, %r8d
	xorl	-104(%rbp), %r10d
	xorl	%r10d, %r15d
	xorl	%r8d, %r10d
	xorl	-100(%rbp), %r9d
	xorl	%r9d, %r14d
	xorl	%edx, %r9d
	xorl	-96(%rbp), %r13d
	xorl	%r13d, %eax
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%edi, %edx
	xorl	%r13d, %edx
	xorl	-92(%rbp), %r11d
	shrq	$32, %rdi
	xorl	%edi, %esi
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%r11d, %edi
	xorl	%r11d, %esi
	movl	%r10d, -104(%rbp)
	movl	%r9d, -100(%rbp)
	movl	%edx, -96(%rbp)
	movl	%esi, -92(%rbp)
	cmpl	$206, %r12d
	jb	LBB3_162
## %bb.161:                             ##   in Loop: Header=BB3_7 Depth=1
	movl	%r15d, -1136(%rbp,%rcx,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %r10d
	cmovel	%r10d, %r12d
	movl	%r14d, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r10d, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r10d, %r12d
	movl	%edi, -1136(%rbp,%r12,4)
	incl	%r12d
	jmp	LBB3_163
LBB3_2:
	movl	$208, %eax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	movl	$52, %r12d
LBB3_3:
	movq	-1144(%rbp), %rax               ## 8-byte Reload
	subq	-80(%rbp), %rax                 ## 8-byte Folded Reload
	xorl	%r9d, %r9d
	xorl	%r13d, %r13d
	cmpq	$16, %rax
	jae	LBB3_4
LBB3_175:
	movl	$13, %eax
	jmp	LBB3_176
	.p2align	4
LBB3_187:                               ##   in Loop: Header=BB3_176 Depth=1
	movl	%edi, -1136(%rbp,%r8,4)
	movl	%eax, -1132(%rbp,%r8,4)
	movl	%esi, -1128(%rbp,%r8,4)
	movl	%edx, -1124(%rbp,%r8,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %r9d
	cmovel	%r9d, %r12d
LBB3_188:                               ##   in Loop: Header=BB3_176 Depth=1
	movl	-88(%rbp), %eax                 ## 4-byte Reload
	incl	%r13d
	cmpl	$13, %r13d
	cmovel	%r9d, %r13d
	decl	%eax
	je	LBB3_189
LBB3_176:                               ## =>This Inner Loop Header: Depth=1
	movl	%eax, -88(%rbp)                 ## 4-byte Spill
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_177
## %bb.178:                             ##   in Loop: Header=BB3_176 Depth=1
	leaq	-1132(,%rax,4), %r11
	addq	%rbp, %r11
	leaq	-1128(,%rax,4), %rsi
	addq	%rbp, %rsi
	leaq	-1124(,%rax,4), %r10
	addq	%rbp, %r10
	jmp	LBB3_179
	.p2align	4
LBB3_177:                               ##   in Loop: Header=BB3_176 Depth=1
	leal	1(%rax), %ecx
	cmpl	$209, %ecx
	cmovel	%r9d, %ecx
	leaq	-1136(,%rcx,4), %r11
	addq	%rbp, %r11
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%r9d, %ecx
	leaq	-1136(,%rcx,4), %rsi
	addq	%rbp, %rsi
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%r9d, %ecx
	leaq	-1136(,%rcx,4), %r10
	addq	%rbp, %r10
LBB3_179:                               ##   in Loop: Header=BB3_176 Depth=1
	movl	%r13d, %r8d
	shlq	$4, %r8
	movl	%r12d, %ecx
	subl	$150, %ecx
	leal	209(%rcx), %r15d
	cmovael	%ecx, %r15d
	cmpl	$206, %r15d
	jae	LBB3_180
## %bb.181:                             ##   in Loop: Header=BB3_176 Depth=1
	leaq	-1132(,%r15,4), %rcx
	addq	%rbp, %rcx
	leaq	-1128(,%r15,4), %rdx
	addq	%rbp, %rdx
	leaq	-1124(,%r15,4), %rdi
	addq	%rbp, %rdi
	jmp	LBB3_182
	.p2align	4
LBB3_180:                               ##   in Loop: Header=BB3_176 Depth=1
	leal	1(%r15), %edi
	cmpl	$209, %edi
	cmovel	%r9d, %edi
	leaq	-1136(,%rdi,4), %rcx
	addq	%rbp, %rcx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%r9d, %edi
	leaq	-1136(,%rdi,4), %rdx
	addq	%rbp, %rdx
	incl	%edi
	cmpl	$209, %edi
	cmovel	%r9d, %edi
	leaq	-1136(,%rdi,4), %rdi
	addq	%rbp, %rdi
LBB3_182:                               ##   in Loop: Header=BB3_176 Depth=1
	leaq	(%r8,%rbp), %r9
	addq	$-296, %r9                      ## imm = 0xFED8
	movl	(%rsi), %r14d
	movl	(%r10), %ebx
	movl	-1136(%rbp,%rax,4), %r10d
	movl	(%r11), %r11d
	movl	(%rdx), %esi
	movl	(%rdi), %eax
	movl	%eax, -44(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%r15,4), %edi
	movl	(%rcx), %eax
	movl	%r12d, %r8d
	cmpl	$206, %r12d
	jae	LBB3_183
## %bb.184:                             ##   in Loop: Header=BB3_176 Depth=1
	movq	-1136(%rbp,%r8,4), %rcx
	movq	-1128(%rbp,%r8,4), %r15
	movq	%rcx, %rdx
	shrq	$32, %rdx
	jmp	LBB3_185
	.p2align	4
LBB3_183:                               ##   in Loop: Header=BB3_176 Depth=1
	leal	1(%r12), %r15d
	cmpl	$209, %r15d
	movl	$0, %ecx
	cmovel	%ecx, %r15d
	movl	-1136(%rbp,%r15,4), %edx
	incl	%r15d
	cmpl	$209, %r15d
	cmovel	%ecx, %r15d
	movl	-1136(%rbp,%r15,4), %ecx
	movq	%rcx, -72(%rbp)                 ## 8-byte Spill
	incl	%r15d
	cmpl	$209, %r15d
	movl	$0, %ecx
	cmovel	%ecx, %r15d
	movl	-1136(%rbp,%r8,4), %ecx
	movl	-1136(%rbp,%r15,4), %r15d
	shlq	$32, %r15
	addq	-72(%rbp), %r15                 ## 8-byte Folded Reload
LBB3_185:                               ##   in Loop: Header=BB3_176 Depth=1
	xorl	(%r9), %r10d
	xorl	%r10d, %edi
	xorl	%ecx, %r10d
	xorl	4(%r9), %r11d
	xorl	%r11d, %eax
	xorl	%edx, %r11d
	xorl	8(%r9), %r14d
	xorl	%r14d, %esi
	movl	%r15d, %ecx
	xorl	%r14d, %ecx
	xorl	12(%r9), %ebx
	shrq	$32, %r15
	movl	-44(%rbp), %edx                 ## 4-byte Reload
	xorl	%ebx, %edx
	xorl	%ebx, %r15d
	movl	%r10d, (%r9)
	movl	%r11d, 4(%r9)
	movl	%ecx, 8(%r9)
	movl	%r15d, 12(%r9)
	cmpl	$206, %r12d
	jb	LBB3_187
## %bb.186:                             ##   in Loop: Header=BB3_176 Depth=1
	movl	%edi, -1136(%rbp,%r8,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %r9d
	cmovel	%r9d, %r12d
	movl	%eax, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r9d, %r12d
	movl	%esi, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%r9d, %r12d
	movl	%edx, -1136(%rbp,%r12,4)
	incl	%r12d
	jmp	LBB3_188
	.p2align	4
LBB3_173:                               ##   in Loop: Header=BB3_4 Depth=1
	movl	%edi, -1136(%rbp,%r10,4)
	movl	%esi, -1132(%rbp,%r10,4)
	movl	%edx, -1128(%rbp,%r10,4)
	movl	%ecx, -1124(%rbp,%r10,4)
	addl	$4, %r12d
	cmpl	$209, %r12d
	movl	$0, %eax
	cmovel	%eax, %r12d
LBB3_174:                               ##   in Loop: Header=BB3_4 Depth=1
	xorl	%r9d, %r9d
	movq	-88(%rbp), %rax                 ## 8-byte Reload
	incl	%r13d
	addq	$16, -80(%rbp)                  ## 8-byte Folded Spill
	addq	$-16, %rax
	cmpq	$15, %rax
	jbe	LBB3_175
LBB3_4:                                 ## =>This Inner Loop Header: Depth=1
	movq	%rax, -88(%rbp)                 ## 8-byte Spill
	movl	%r12d, %ecx
	subl	$12, %ecx
	leal	209(%rcx), %eax
	cmovael	%ecx, %eax
	cmpl	$206, %eax
	jae	LBB3_5
## %bb.164:                             ##   in Loop: Header=BB3_4 Depth=1
	leaq	-1132(,%rax,4), %rsi
	addq	%rbp, %rsi
	leaq	-1128(,%rax,4), %rdx
	addq	%rbp, %rdx
	leaq	-1124(,%rax,4), %rcx
	addq	%rbp, %rcx
	jmp	LBB3_165
	.p2align	4
LBB3_5:                                 ##   in Loop: Header=BB3_4 Depth=1
	leal	1(%rax), %ecx
	cmpl	$209, %ecx
	movl	$0, %edi
	cmovel	%edi, %ecx
	leaq	-1136(,%rcx,4), %rsi
	addq	%rbp, %rsi
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%edi, %ecx
	leaq	-1136(,%rcx,4), %rdx
	addq	%rbp, %rdx
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%edi, %ecx
	leaq	-1136(,%rcx,4), %rcx
	addq	%rbp, %rcx
LBB3_165:                               ##   in Loop: Header=BB3_4 Depth=1
	movl	%r13d, %edi
	shlq	$4, %rdi
	movl	%r12d, %r9d
	subl	$150, %r9d
	leal	209(%r9), %r8d
	cmovael	%r9d, %r8d
	cmpl	$206, %r8d
	jae	LBB3_166
## %bb.167:                             ##   in Loop: Header=BB3_4 Depth=1
	leaq	-1132(,%r8,4), %r10
	addq	%rbp, %r10
	leaq	-1128(,%r8,4), %r9
	addq	%rbp, %r9
	leaq	-1124(,%r8,4), %r15
	addq	%rbp, %r15
	jmp	LBB3_168
	.p2align	4
LBB3_166:                               ##   in Loop: Header=BB3_4 Depth=1
	leal	1(%r8), %r11d
	cmpl	$209, %r11d
	movl	$0, %ebx
	cmovel	%ebx, %r11d
	leaq	-1136(,%r11,4), %r10
	addq	%rbp, %r10
	incl	%r11d
	cmpl	$209, %r11d
	cmovel	%ebx, %r11d
	leaq	-1136(,%r11,4), %r9
	addq	%rbp, %r9
	incl	%r11d
	cmpl	$209, %r11d
	cmovel	%ebx, %r11d
	leaq	-1136(,%r11,4), %r15
	addq	%rbp, %r15
LBB3_168:                               ##   in Loop: Header=BB3_4 Depth=1
	leaq	(%rdi,%rbp), %r14
	addq	$-296, %r14                     ## imm = 0xFED8
	movl	(%rdx), %r11d
	movl	(%rcx), %ebx
	movl	-1136(%rbp,%rax,4), %edx
	movl	(%rsi), %eax
	movl	(%r9), %ecx
	movl	%ecx, -72(%rbp)                 ## 4-byte Spill
	movl	(%r15), %ecx
	movl	%ecx, -44(%rbp)                 ## 4-byte Spill
	movl	-1136(%rbp,%r8,4), %ecx
	movl	%ecx, -48(%rbp)                 ## 4-byte Spill
	movl	(%r10), %ecx
	movl	%ecx, -52(%rbp)                 ## 4-byte Spill
	movl	%r12d, %r10d
	cmpl	$206, %r12d
	jae	LBB3_169
## %bb.170:                             ##   in Loop: Header=BB3_4 Depth=1
	movq	-1136(%rbp,%r10,4), %r15
	movq	-1128(%rbp,%r10,4), %rcx
	movq	%r15, %rsi
	shrq	$32, %rsi
	jmp	LBB3_171
	.p2align	4
LBB3_169:                               ##   in Loop: Header=BB3_4 Depth=1
	leal	1(%r12), %ecx
	cmpl	$209, %ecx
	movl	$0, %r8d
	cmovel	%r8d, %ecx
	movl	-1136(%rbp,%rcx,4), %esi
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%r8d, %ecx
	movl	-1136(%rbp,%rcx,4), %edi
	incl	%ecx
	cmpl	$209, %ecx
	cmovel	%r8d, %ecx
	movl	-1136(%rbp,%r10,4), %r15d
	movl	-1136(%rbp,%rcx,4), %ecx
	shlq	$32, %rcx
	orq	%rdi, %rcx
LBB3_171:                               ##   in Loop: Header=BB3_4 Depth=1
	xorl	(%r14), %edx
	movq	-64(%rbp), %r8                  ## 8-byte Reload
	movq	-80(%rbp), %r9                  ## 8-byte Reload
	xorl	(%r8,%r9), %r15d
	xorl	4(%r14), %eax
	xorl	4(%r8,%r9), %esi
	xorl	8(%r14), %r11d
	xorl	%edx, %r15d
	movl	8(%r8,%r9), %edi
	xorl	12(%r14), %ebx
	xorl	%ecx, %edi
	shrq	$32, %rcx
	xorl	12(%r8,%r9), %ecx
	movl	%r15d, (%r14)
	xorl	%eax, %esi
	movl	%esi, 4(%r14)
	xorl	%r11d, %edi
	movl	%edi, 8(%r14)
	xorl	%ebx, %ecx
	movl	%ecx, 12(%r14)
	movl	-48(%rbp), %edi                 ## 4-byte Reload
	xorl	%edx, %edi
	movl	-52(%rbp), %esi                 ## 4-byte Reload
	xorl	%eax, %esi
	movl	-72(%rbp), %edx                 ## 4-byte Reload
	xorl	%r11d, %edx
	movl	-44(%rbp), %ecx                 ## 4-byte Reload
	xorl	%ebx, %ecx
	cmpl	$206, %r12d
	jb	LBB3_173
## %bb.172:                             ##   in Loop: Header=BB3_4 Depth=1
	movl	%edi, -1136(%rbp,%r10,4)
	incl	%r12d
	cmpl	$209, %r12d
	movl	$0, %eax
	cmovel	%eax, %r12d
	movl	%esi, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%eax, %r12d
	movl	%edx, -1136(%rbp,%r12,4)
	incl	%r12d
	cmpl	$209, %r12d
	cmovel	%eax, %r12d
	movl	%ecx, -1136(%rbp,%r12,4)
	incl	%r12d
	jmp	LBB3_174
LBB3_189:
	pxor	%xmm0, %xmm0
	movdqu	%xmm0, -1168(%rbp)
	movdqu	%xmm0, -1184(%rbp)
	movdqu	%xmm0, -1200(%rbp)
	movdqu	%xmm0, -1216(%rbp)
	movdqu	%xmm0, -1232(%rbp)
	movdqu	%xmm0, -1248(%rbp)
	movdqu	%xmm0, -1264(%rbp)
	movdqu	%xmm0, -1280(%rbp)
	movdqu	%xmm0, -1296(%rbp)
	movdqu	%xmm0, -1312(%rbp)
	movdqu	%xmm0, -1328(%rbp)
	movdqu	%xmm0, -1344(%rbp)
	movdqu	%xmm0, -1360(%rbp)
	leaq	-2196(%rbp), %rdi
	leaq	-1136(%rbp), %rsi
	movl	$836, %edx                      ## imm = 0x344
	callq	_memcpy
	xorl	%eax, %eax
	.p2align	4
LBB3_190:                               ## =>This Inner Loop Header: Depth=1
	movl	%eax, %ecx
	shrl	$2, %ecx
	shlq	$4, %rcx
	addq	%rbp, %rcx
	addq	$-296, %rcx                     ## imm = 0xFED8
	movl	%eax, %edx
	andl	$2, %edx
	movl	(%rcx,%rdx,4), %edx
	movl	%edx, -1360(%rbp,%rax,4)
	leal	1(%rax), %edx
	andl	$3, %edx
	movl	(%rcx,%rdx,4), %ecx
	movl	%ecx, -1356(%rbp,%rax,4)
	addq	$2, %rax
	cmpq	$52, %rax
	jne	LBB3_190
## %bb.191:
	movl	%r12d, -1152(%rbp)
	shll	$2, %r13d
	movl	%r13d, -1148(%rbp)
	leaq	-2196(%rbp), %rdi
	callq	__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	movq	-1144(%rbp), %rsi               ## 8-byte Reload
	movq	-80(%rbp), %r8                  ## 8-byte Reload
	cmpq	%rsi, %r8
	jae	LBB3_194
## %bb.192:
	movdqa	LCPI3_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI3_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	movq	-64(%rbp), %rdi                 ## 8-byte Reload
	.p2align	4
LBB3_193:                               ## =>This Inner Loop Header: Depth=1
	movzbl	(%rdi,%r8), %ecx
	xorl	%eax, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movl	%ecx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	%edx
	movd	%ecx, %xmm2
	shll	$30, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %eax
	xorl	%ecx, %eax
	incq	%r8
	cmpq	%r8, %rsi
	jne	LBB3_193
LBB3_194:
	notl	%eax
	addq	$2168, %rsp                     ## imm = 0x878
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI4_0:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI4_1:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$1200, %rsp                     ## imm = 0x4B0
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edx, %ebx
	movq	%rsi, %r14
	movq	%rdi, %r15
	cmpq	$3, %rsi
	ja	LBB4_6
## %bb.1:
	notl	%ebx
	testq	%r14, %r14
	je	LBB4_5
## %bb.2:
	movzbl	(%r15), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm0
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm0, %xmm1                ## xmm1 = xmm0[0,0,0,0]
	movdqa	LCPI4_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI4_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$1, %r14
	je	LBB4_5
## %bb.3:
	movzbl	1(%r15), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	shrl	$6, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI4_1(%rip), %xmm1
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	xorl	%eax, %ecx
	pshufd	$238, %xmm1, %xmm2              ## xmm2 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm2
	pshufd	$85, %xmm2, %xmm1               ## xmm1 = xmm2[1,1,1,1]
	pxor	%xmm2, %xmm1
	movd	%xmm1, %ebx
	xorl	%ecx, %ebx
	cmpq	$2, %r14
	je	LBB4_5
## %bb.4:
	movzbl	2(%r15), %eax
	xorl	%eax, %ebx
	movl	%ebx, %ecx
	shrl	%ecx
	movl	%ebx, %eax
	andl	$1, %eax
	negl	%eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%ecx
	movd	%ebx, %xmm1
	shll	$30, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	movl	%ebx, %ecx
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	pand	%xmm0, %xmm1
	pcmpeqd	%xmm0, %xmm1
	pand	LCPI4_1(%rip), %xmm1
	shrl	$6, %ecx
	shll	$26, %eax
	sarl	$31, %eax
	andl	$1994146192, %eax               ## imm = 0x76DC4190
	shll	$26, %ebx
	sarl	$31, %ebx
	andl	$-306674912, %ebx               ## imm = 0xEDB88320
	xorl	%ecx, %ebx
	xorl	%eax, %ebx
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	pxor	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	pxor	%xmm0, %xmm1
	movd	%xmm1, %eax
	xorl	%ebx, %eax
	movl	%eax, %ebx
	jmp	LBB4_5
LBB4_6:
	leaq	-1228(%rbp), %rdi
	movl	$1196, %esi                     ## imm = 0x4AC
	callq	___bzero
	xorl	(%r15), %ebx
	notl	%ebx
	movl	%ebx, -1232(%rbp)
	movq	%r14, %rsi
	andq	$-4, %rsi
	movl	$4, %eax
	movl	$1, %ecx
	movl	$1, %edx
	cmpq	$4, %rsi
	je	LBB4_9
## %bb.7:
	leaq	-4(%r14), %rsi
	movl	$1, %edx
	xorl	%edi, %edi
	.p2align	4
LBB4_8:                                 ## =>This Inner Loop Header: Depth=1
	movl	%edx, %r8d
	subl	$145, %r8d
	leal	300(%r8), %r9d
	cmovael	%r8d, %r9d
	movl	%edx, %r8d
	subl	$183, %r8d
	leal	300(%r8), %r10d
	cmovael	%r8d, %r10d
	movl	%edx, %r8d
	subl	$211, %r8d
	leal	300(%r8), %r11d
	cmovael	%r8d, %r11d
	movl	(%r15,%rax), %r8d
	movl	%edx, %ebx
	xorl	-1232(%rbp,%rbx,4), %r8d
	xorl	-1232(%rbp,%r9,4), %r8d
	xorl	-1232(%rbp,%r10,4), %r8d
	xorl	-1232(%rbp,%r11,4), %r8d
	incl	%edx
	cmpl	$300, %edx                      ## imm = 0x12C
	cmovel	%edi, %edx
	movl	%r8d, -1232(%rbp,%rbx,4)
	addq	$4, %rax
	addq	$-4, %rsi
	cmpq	$3, %rsi
	ja	LBB4_8
LBB4_9:
	movl	$300, %r8d                      ## imm = 0x12C
	xorl	%ebx, %ebx
	leal	300(%rdx), %esi
	leaq	__ZN15crc_suitability12_GLOBAL__N_19reductionE(%rip), %r11
	jmp	LBB4_10
	.p2align	4
LBB4_11:                                ##   in Loop: Header=BB4_10 Depth=1
	leal	-155(%rcx), %r9d
	cmpl	%r9d, %edx
	movl	%edx, %r9d
	cmovbl	%esi, %r9d
	addl	%r8d, %r9d
	addl	$-146, %r9d
	movl	-1232(%rbp,%r9,4), %r9d
LBB4_16:                                ##   in Loop: Header=BB4_10 Depth=1
	leal	-117(%rcx), %r10d
	cmpl	%r10d, %edx
	movl	%edx, %r10d
	cmovbl	%esi, %r10d
	addl	%r8d, %r10d
	addl	$-184, %r10d
	xorl	-1232(%rbp,%r10,4), %r9d
LBB4_18:                                ##   in Loop: Header=BB4_10 Depth=1
	leal	-89(%rcx), %r10d
	cmpl	%r10d, %edx
	movl	%edx, %r10d
	cmovbl	%esi, %r10d
	addl	%r8d, %r10d
	addl	$-212, %r10d
	xorl	-1232(%rbp,%r10,4), %r9d
LBB4_19:                                ##   in Loop: Header=BB4_10 Depth=1
	cmpl	%ecx, %edx
	movl	%edx, %r10d
	cmovbl	%esi, %r10d
	addl	%r8d, %r10d
	addl	$-301, %r10d                    ## imm = 0xFED3
	decl	%r8d
	xorl	-1232(%rbp,%r10,4), %r9d
	movzbl	%bl, %r10d
	movl	(%r11,%r10,4), %r10d
	movzbl	%bh, %edi
	movl	%ebx, %r12d
	shrl	$14, %r12d
	andl	$1020, %r12d                    ## imm = 0x3FC
	xorl	1024(%r11,%rdi,4), %r10d
	xorl	2048(%r12,%r11), %r10d
	shrl	$24, %ebx
	xorl	3072(%r11,%rbx,4), %r10d
	movl	%r10d, %ebx
	xorl	%r9d, %ebx
	incl	%ecx
	testl	%r8d, %r8d
                                        ## kill: def $r8d killed $r8d def $r8
	je	LBB4_12
LBB4_10:                                ## =>This Inner Loop Header: Depth=1
	cmpl	$145, %r8d
	jle	LBB4_11
## %bb.15:                              ##   in Loop: Header=BB4_10 Depth=1
	xorl	%r9d, %r9d
	cmpl	$183, %r8d
	jbe	LBB4_16
## %bb.17:                              ##   in Loop: Header=BB4_10 Depth=1
	xorl	%r9d, %r9d
	cmpl	$211, %r8d
	jbe	LBB4_18
	jmp	LBB4_19
LBB4_12:
	cmpq	%r14, %rax
	jae	LBB4_5
## %bb.13:
	movdqa	LCPI4_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI4_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	.p2align	4
LBB4_14:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%r15,%rax), %ecx
	xorl	%ebx, %ecx
	movl	%ecx, %esi
	shrl	%esi
	movl	%ecx, %edx
	andl	$1, %edx
	negl	%edx
	andl	$-306674912, %edx               ## imm = 0xEDB88320
	xorl	%esi, %edx
	movl	%edx, %esi
	shrl	%esi
	movd	%ecx, %xmm2
	shll	$30, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	movl	%ecx, %esi
	shrl	$6, %esi
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %edx
	sarl	$31, %edx
	andl	$1994146192, %edx               ## imm = 0x76DC4190
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	xorl	%edx, %ecx
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%ecx, %ebx
	incq	%rax
	cmpq	%rax, %r14
	jne	LBB4_14
LBB4_5:
	notl	%ebx
	movl	%ebx, %eax
	addq	$1200, %rsp                     ## imm = 0x4B0
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
LCPI5_0:
	.long	8                               ## 0x8
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	32                              ## 0x20
LCPI5_1:
	.long	249268274                       ## 0xedb8832
	.long	124634137                       ## 0x76dc419
	.long	498536548                       ## 0x1db71064
	.long	997073096                       ## 0x3b6e20c8
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	4
__ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj: ## @_ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r15
	pushq	%r14
	pushq	%r12
	pushq	%rbx
	subq	$4800, %rsp                     ## imm = 0x12C0
	.cfi_offset %rbx, -48
	.cfi_offset %r12, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%edx, %ebx
	movq	%rsi, %r15
	movq	%rdi, %r14
	cmpq	$15, %rsi
	ja	LBB5_5
## %bb.1:
	notl	%ebx
	testq	%r15, %r15
	je	LBB5_4
## %bb.2:
	xorl	%eax, %eax
	movdqa	LCPI5_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI5_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	.p2align	4
LBB5_3:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%r14,%rax), %ecx
	xorl	%ebx, %ecx
	movl	%ecx, %esi
	shrl	%esi
	movl	%ecx, %edx
	andl	$1, %edx
	negl	%edx
	andl	$-306674912, %edx               ## imm = 0xEDB88320
	xorl	%esi, %edx
	movl	%edx, %esi
	shrl	%esi
	movd	%ecx, %xmm2
	shll	$30, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	movl	%ecx, %esi
	shrl	$6, %esi
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %edx
	sarl	$31, %edx
	andl	$1994146192, %edx               ## imm = 0x76DC4190
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%esi, %ecx
	xorl	%edx, %ecx
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%ecx, %ebx
	incq	%rax
	cmpq	%rax, %r15
	jne	LBB5_3
	jmp	LBB5_4
LBB5_5:
	leaq	-4816(%rbp), %rdi
	movl	$4784, %esi                     ## imm = 0x12B0
	callq	___bzero
	xorl	(%r14), %ebx
	movl	12(%r14), %eax
	notl	%ebx
	movl	%ebx, -4832(%rbp)
	movq	4(%r14), %rcx
	movq	%rcx, -4828(%rbp)
	movl	%eax, -4820(%rbp)
	movq	%r15, %rax
	andq	$-16, %rax
	movl	$16, %r10d
	movl	$1, %ecx
	movl	$1, %edx
	cmpq	$16, %rax
	je	LBB5_8
## %bb.6:
	leaq	-16(%r15), %rsi
	movl	$1, %edx
	xorl	%edi, %edi
	.p2align	4
LBB5_7:                                 ## =>This Inner Loop Header: Depth=1
	movl	%edx, %eax
	subl	$145, %eax
	leal	300(%rax), %r8d
	cmovael	%eax, %r8d
	movl	%edx, %eax
	subl	$183, %eax
	leal	300(%rax), %r9d
	cmovael	%eax, %r9d
	movl	%edx, %eax
	subl	$211, %eax
	leal	300(%rax), %r11d
	cmovael	%eax, %r11d
	movl	%edx, %eax
	shlq	$4, %rax
	shlq	$4, %r8
	shlq	$4, %r9
	shlq	$4, %r11
	movdqu	(%r14,%r10), %xmm0
	movdqu	-4832(%rbp,%rax), %xmm1
	pxor	%xmm0, %xmm1
	movdqu	-4832(%rbp,%r8), %xmm0
	pxor	%xmm1, %xmm0
	movdqu	-4832(%rbp,%r9), %xmm1
	movdqu	-4832(%rbp,%r11), %xmm2
	pxor	%xmm1, %xmm2
	pxor	%xmm0, %xmm2
	incl	%edx
	cmpl	$300, %edx                      ## imm = 0x12C
	cmovel	%edi, %edx
	movdqu	%xmm2, -4832(%rbp,%rax)
	addq	$16, %r10
	addq	$-16, %rsi
	cmpq	$15, %rsi
	ja	LBB5_7
LBB5_8:
	movl	$300, %r8d                      ## imm = 0x12C
	xorl	%ebx, %ebx
	leal	300(%rdx), %r12d
	leaq	__ZN15crc_suitability12_GLOBAL__N_19reductionE(%rip), %rdi
	jmp	LBB5_9
	.p2align	4
LBB5_10:                                ##   in Loop: Header=BB5_9 Depth=1
	leal	-155(%rcx), %eax
	cmpl	%eax, %edx
	movl	%edx, %eax
	cmovbl	%r12d, %eax
	addl	%r8d, %eax
	addl	$-146, %eax
	shlq	$4, %rax
	movdqu	-4832(%rbp,%rax), %xmm1
LBB5_15:                                ##   in Loop: Header=BB5_9 Depth=1
	leal	-117(%rcx), %eax
	cmpl	%eax, %edx
	movl	%edx, %eax
	cmovbl	%r12d, %eax
	addl	%r8d, %eax
	addl	$-184, %eax
	shlq	$4, %rax
	movdqu	-4832(%rbp,%rax), %xmm0
	pxor	%xmm1, %xmm0
LBB5_17:                                ##   in Loop: Header=BB5_9 Depth=1
	leal	-89(%rcx), %eax
	cmpl	%eax, %edx
	movl	%edx, %eax
	cmovbl	%r12d, %eax
	addl	%r8d, %eax
	addl	$-212, %eax
	shlq	$4, %rax
	movdqu	-4832(%rbp,%rax), %xmm1
	pxor	%xmm1, %xmm0
LBB5_18:                                ##   in Loop: Header=BB5_9 Depth=1
	cmpl	%ecx, %edx
	movl	%edx, %eax
	cmovbl	%r12d, %eax
	leal	(%rax,%r8), %r9d
	addl	$-301, %r9d                     ## imm = 0xFED3
	shlq	$4, %r9
	pshufd	$255, %xmm0, %xmm1              ## xmm1 = xmm0[3,3,3,3]
	movd	%xmm1, %eax
	xorl	-4820(%rbp,%r9), %eax
	movzbl	%bl, %r11d
	xorl	(%rdi,%r11,4), %eax
	movzbl	%bh, %esi
	xorl	1024(%rdi,%rsi,4), %eax
	movl	%ebx, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%rdi), %eax
	shrl	$24, %ebx
	xorl	3072(%rdi,%rbx,4), %eax
	pshufd	$238, %xmm0, %xmm1              ## xmm1 = xmm0[2,3,2,3]
	movd	%xmm1, %ebx
	xorl	-4824(%rbp,%r9), %ebx
	movzbl	%al, %esi
	xorl	(%rdi,%rsi,4), %ebx
	movzbl	%ah, %esi
	xorl	1024(%rdi,%rsi,4), %ebx
	movl	%eax, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%rdi), %ebx
	shrl	$24, %eax
	xorl	3072(%rdi,%rax,4), %ebx
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	movd	%xmm1, %eax
	xorl	-4828(%rbp,%r9), %eax
	movd	%xmm0, %r11d
	xorl	-4832(%rbp,%r9), %r11d
	movzbl	%bl, %esi
	xorl	(%rdi,%rsi,4), %eax
	movzbl	%bh, %esi
	xorl	1024(%rdi,%rsi,4), %eax
	movl	%ebx, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%rdi), %eax
	shrl	$24, %ebx
	xorl	3072(%rdi,%rbx,4), %eax
	movzbl	%al, %esi
	xorl	(%rdi,%rsi,4), %r11d
	movzbl	%ah, %esi
	xorl	1024(%rdi,%rsi,4), %r11d
	movl	%eax, %esi
	shrl	$14, %esi
	andl	$1020, %esi                     ## imm = 0x3FC
	xorl	2048(%rsi,%rdi), %r11d
	shrl	$24, %eax
	movl	%r11d, %ebx
	xorl	3072(%rdi,%rax,4), %ebx
	leal	-1(%r8), %eax
	incl	%ecx
	testl	%eax, %eax
	movl	%eax, %r8d
	je	LBB5_11
LBB5_9:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$145, %r8d
	jle	LBB5_10
## %bb.14:                              ##   in Loop: Header=BB5_9 Depth=1
	pxor	%xmm1, %xmm1
	cmpl	$183, %r8d
	jbe	LBB5_15
## %bb.16:                              ##   in Loop: Header=BB5_9 Depth=1
	pxor	%xmm0, %xmm0
	cmpl	$211, %r8d
	jbe	LBB5_17
	jmp	LBB5_18
LBB5_11:
	cmpq	%r15, %r10
	jae	LBB5_4
## %bb.12:
	movdqa	LCPI5_0(%rip), %xmm0            ## xmm0 = [8,4,16,32]
	movdqa	LCPI5_1(%rip), %xmm1            ## xmm1 = [249268274,124634137,498536548,997073096]
	.p2align	4
LBB5_13:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%r14,%r10), %eax
	xorl	%ebx, %eax
	movl	%eax, %edx
	shrl	%edx
	movl	%eax, %ecx
	andl	$1, %ecx
	negl	%ecx
	andl	$-306674912, %ecx               ## imm = 0xEDB88320
	xorl	%edx, %ecx
	movl	%ecx, %edx
	shrl	%edx
	movd	%eax, %xmm2
	shll	$30, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	movl	%eax, %edx
	shrl	$6, %edx
	pshufd	$0, %xmm2, %xmm2                ## xmm2 = xmm2[0,0,0,0]
	pand	%xmm0, %xmm2
	pcmpeqd	%xmm0, %xmm2
	pand	%xmm1, %xmm2
	shll	$26, %ecx
	sarl	$31, %ecx
	andl	$1994146192, %ecx               ## imm = 0x76DC4190
	shll	$26, %eax
	sarl	$31, %eax
	andl	$-306674912, %eax               ## imm = 0xEDB88320
	xorl	%edx, %eax
	xorl	%ecx, %eax
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	pxor	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	pxor	%xmm3, %xmm2
	movd	%xmm2, %ebx
	xorl	%eax, %ebx
	incq	%r10
	cmpq	%r10, %r15
	jne	LBB5_13
LBB5_4:
	notl	%ebx
	movl	%ebx, %eax
	addq	$4800, %rsp                     ## imm = 0x12C0
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4                               ## -- Begin function _ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
__ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE: ## @_ZN15crc_suitability12_GLOBAL__N_16finishILj1EEEjRNS0_7CompactIXT_EEE
	.cfi_startproc
## %bb.0:
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
	leaq	836(%rdi), %r12
	movl	$1, %eax
	movl	$-1, %edx
	.p2align	4
LBB6_1:                                 ## =>This Inner Loop Header: Depth=1
	movl	1048(%rdi), %esi
	leal	52(%rsi), %r8d
	cmpl	%eax, %esi
	cmovael	%esi, %r8d
	movl	1044(%rdi), %esi
	addl	%edx, %r8d
	leal	209(%rsi), %r9d
	cmpl	%eax, %esi
	movl	%esi, %r10d
	cmovbl	%r9d, %r10d
	addl	%edx, %r10d
	leal	150(%rax), %r11d
	cmpl	%r11d, %esi
	cmovbl	%r9d, %esi
	addl	%edx, %esi
	addl	$-150, %esi
	movl	(%rdi,%rsi,4), %esi
	xorl	(%rdi,%r10,4), %esi
	xorl	%esi, 836(%rdi,%r8,4)
	decl	%edx
	incl	%eax
	cmpl	$53, %eax
	jne	LBB6_1
## %bb.2:
	movl	1044(%rdi), %edx
	movl	1048(%rdi), %esi
	xorl	%eax, %eax
	movl	$261, %r8d                      ## imm = 0x105
	movl	$-248, %r9d
	movl	$1, %r10d
	leaq	__ZN15crc_suitability12_GLOBAL__N_19reductionE(%rip), %r11
	jmp	LBB6_3
	.p2align	4
LBB6_13:                                ##   in Loop: Header=BB6_3 Depth=1
	movl	$262, %r14d                     ## imm = 0x106
	subq	%r8, %r14
	movl	%edx, %r15d
	subl	%r14d, %r15d
	leal	209(%r15), %r14d
	cmpq	%rdx, %r10
	cmovbel	%r15d, %r14d
	movq	%rdi, %r15
LBB6_14:                                ##   in Loop: Header=BB6_3 Depth=1
	decq	%r8
	movl	%r14d, %r14d
	xorl	(%r15,%r14,4), %ebx
	movzbl	%al, %r14d
	movl	(%r11,%r14,4), %r14d
	movzbl	%ah, %ecx
	movl	%eax, %r15d
	shrl	$14, %r15d
	andl	$1020, %r15d                    ## imm = 0x3FC
	xorl	1024(%r11,%rcx,4), %r14d
	xorl	2048(%r15,%r11), %r14d
	shrl	$24, %eax
	xorl	3072(%r11,%rax,4), %r14d
	movl	%r14d, %eax
	xorl	%ebx, %eax
	incl	%r9d
	incq	%r10
	testq	%r8, %r8
	je	LBB6_15
LBB6_3:                                 ## =>This Inner Loop Header: Depth=1
	cmpq	$12, %r8
	jg	LBB6_5
## %bb.4:                               ##   in Loop: Header=BB6_3 Depth=1
	leal	(%rdx,%r8), %ebx
	cmpl	%r9d, %edx
	leal	209(%rdx,%r8), %r14d
	cmovael	%ebx, %r14d
	addl	$-13, %r14d
	movl	(%rdi,%r14,4), %ebx
	jmp	LBB6_6
	.p2align	4
LBB6_5:                                 ##   in Loop: Header=BB6_3 Depth=1
	xorl	%ebx, %ebx
	cmpq	$52, %r8
	ja	LBB6_7
LBB6_6:                                 ##   in Loop: Header=BB6_3 Depth=1
	leal	40(%r9), %r14d
	leal	(%r8,%rdx), %r15d
	cmpl	%r14d, %edx
	leal	209(%rdx,%r8), %r14d
	cmovael	%r15d, %r14d
	addl	$-53, %r14d
	xorl	(%rdi,%r14,4), %ebx
LBB6_8:                                 ##   in Loop: Header=BB6_3 Depth=1
	leal	138(%r9), %r14d
	leal	(%rdx,%r8), %r15d
	cmpl	%r14d, %edx
	leal	209(%rdx,%r8), %r14d
	cmovael	%r15d, %r14d
	addl	$-151, %r14d
	xorl	(%rdi,%r14,4), %ebx
LBB6_10:                                ##   in Loop: Header=BB6_3 Depth=1
	leal	190(%r9), %r14d
	leal	(%r8,%rdx), %r15d
	cmpl	%r14d, %edx
	leal	209(%rdx,%r8), %r14d
	cmovael	%r15d, %r14d
	addl	$-203, %r14d
	xorl	(%rdi,%r14,4), %ebx
LBB6_11:                                ##   in Loop: Header=BB6_3 Depth=1
	cmpq	$210, %r10
	jb	LBB6_13
## %bb.12:                              ##   in Loop: Header=BB6_3 Depth=1
	leal	40(%r9), %r14d
	leal	(%rsi,%r8), %r15d
	cmpl	%r14d, %esi
	leal	52(%rsi,%r8), %r14d
	cmovael	%r15d, %r14d
	addl	$-53, %r14d
	movq	%r12, %r15
	jmp	LBB6_14
	.p2align	4
LBB6_7:                                 ##   in Loop: Header=BB6_3 Depth=1
	xorl	%ebx, %ebx
	cmpq	$150, %r8
	jbe	LBB6_8
## %bb.9:                               ##   in Loop: Header=BB6_3 Depth=1
	xorl	%ebx, %ebx
	cmpq	$202, %r8
	jbe	LBB6_10
	jmp	LBB6_11
LBB6_15:
                                        ## kill: def $eax killed $eax killed $rax
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"compact261-32"

L_.str.1:                               ## @.str.1
	.asciz	"compact261-64"

L_.str.2:                               ## @.str.2
	.asciz	"compact261-128"

L_.str.3:                               ## @.str.3
	.asciz	"quartet261"

L_.str.4:                               ## @.str.4
	.asciz	"direct300-32"

L_.str.5:                               ## @.str.5
	.asciz	"direct300-128"

	.section	__DATA,__const
	.globl	__ZN15crc_suitability8circuitsE ## @_ZN15crc_suitability8circuitsE
	.p2align	3, 0x0
__ZN15crc_suitability8circuitsE:
	.quad	L_.str
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	1044                            ## 0x414
	.byte	1                               ## 0x1
	.space	7
	.quad	L_.str.1
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj2EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	2088                            ## 0x828
	.byte	1                               ## 0x1
	.space	7
	.quad	L_.str.2
	.quad	__ZN15crc_suitability12_GLOBAL__N_17compactILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	4176                            ## 0x1050
	.byte	1                               ## 0x1
	.space	7
	.quad	L_.str.3
	.quad	__ZN15crc_suitability12_GLOBAL__N_19one_writeENSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	2088                            ## 0x828
	.byte	1                               ## 0x1
	.space	7
	.quad	L_.str.4
	.quad	__ZN15crc_suitability12_GLOBAL__N_19direct300ILj1EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	1200                            ## 0x4b0
	.byte	1                               ## 0x1
	.space	7
	.quad	L_.str.5
	.quad	__ZN15crc_suitability12_GLOBAL__N_19direct300ILj4EEEjNSt3__14spanIKSt4byteLm18446744073709551615EEEj
	.quad	4096                            ## 0x1000
	.quad	4800                            ## 0x12c0
	.byte	1                               ## 0x1
	.space	7

	.section	__TEXT,__const
	.p2align	2, 0x0                          ## @_ZN15crc_suitability12_GLOBAL__N_19reductionE
__ZN15crc_suitability12_GLOBAL__N_19reductionE:
	.long	0                               ## 0x0
	.long	3681617473                      ## 0xdb710641
	.long	1838353091                      ## 0x6d930ac3
	.long	3068267650                      ## 0xb6e20c82
	.long	3676706182                      ## 0xdb261586
	.long	5706695                         ## 0x5713c7
	.long	3065323333                      ## 0xb6b51f45
	.long	1841568004                      ## 0x6dc41904
	.long	1832725837                      ## 0x6d3d2d4d
	.long	3058445068                      ## 0xb64c2b0c
	.long	11413390                        ## 0xae278e
	.long	3688833487                      ## 0xdbdf21cf
	.long	3055237323                      ## 0xb61b38cb
	.long	1835679370                      ## 0x6d6a3e8a
	.long	3683136008                      ## 0xdb883208
	.long	16331849                        ## 0xf93449
	.long	3665451674                      ## 0xda7a5a9a
	.long	17521883                        ## 0x10b5cdb
	.long	3085520985                      ## 0xb7e95059
	.long	1821922840                      ## 0x6c985618
	.long	22826780                        ## 0x15c4f1c
	.long	3660400989                      ## 0xda2d495d
	.long	1825523167                      ## 0x6ccf45df
	.long	3082699678                      ## 0xb7be439e
	.long	3074914263                      ## 0xb74777d7
	.long	1815507350                      ## 0x6c367196
	.long	3671358740                      ## 0xdad47d14
	.long	27622229                        ## 0x1a57b55
	.long	1818321489                      ## 0x6c616251
	.long	3071304720                      ## 0xb7106410
	.long	32663698                        ## 0x1f26892
	.long	3666046675                      ## 0xda836ed3
	.long	1871033205                      ## 0x6f85b375
	.long	3035936052                      ## 0xb4f4b534
	.long	35043766                        ## 0x216b9b6
	.long	3647455223                      ## 0xd967bff7
	.long	3030623987                      ## 0xb4a3a6f3
	.long	1876074674                      ## 0x6fd2a0b2
	.long	3643845680                      ## 0xd930ac30
	.long	37857905                        ## 0x241aa71
	.long	45653560                        ## 0x2b89e38
	.long	3653867641                      ## 0xd9c99879
	.long	1865127163                      ## 0x6f2b94fb
	.long	3025834682                      ## 0xb45a92ba
	.long	3651046334                      ## 0xd99e8bbe
	.long	49253887                        ## 0x2ef8dff
	.long	3020783997                      ## 0xb40d817d
	.long	1870432060                      ## 0x6f7c873c
	.long	3053447663                      ## 0xb5ffe9ef
	.long	1854861230                      ## 0x6e8eefae
	.long	3631014700                      ## 0xd86ce32c
	.long	52290925                        ## 0x31de56d
	.long	1859779689                      ## 0x6ed9fc69
	.long	3047750184                      ## 0xb5a8fa28
	.long	55244458                        ## 0x34af6aa
	.long	3627806955                      ## 0xd83bf0eb
	.long	3636642978                      ## 0xd8c2c4a2
	.long	62112483                        ## 0x3b3c2e3
	.long	3042037345                      ## 0xb551ce61
	.long	1847642144                      ## 0x6e20c820
	.long	65327396                        ## 0x3e4d124
	.long	3633698661                      ## 0xd895d765
	.long	1853348839                      ## 0x6e77dbe7
	.long	3037126054                      ## 0xb506dda6
	.long	3742066410                      ## 0xdf0b66ea
	.long	75129003                        ## 0x47a60ab
	.long	2996333609                      ## 0xb2986c29
	.long	1776904808                      ## 0x69e96a68
	.long	70087532                        ## 0x42d736c
	.long	3747378477                      ## 0xdf5c752d
	.long	1774090671                      ## 0x69be79af
	.long	2999943150                      ## 0xb2cf7fee
	.long	2989902759                      ## 0xb2364ba7
	.long	1766280678                      ## 0x69474de6
	.long	3752149348                      ## 0xdfa54164
	.long	81020709                        ## 0x4d44725
	.long	1762680353                      ## 0x69105e21
	.long	2992724064                      ## 0xb2615860
	.long	75715810                        ## 0x48354e2
	.long	3757200035                      ## 0xdff252a3
	.long	91307120                        ## 0x5713c70
	.long	3724556849                      ## 0xde003a31
	.long	1759655603                      ## 0x68e236b3
	.long	3012767986                      ## 0xb39330f2
	.long	3730254326                      ## 0xde5729f6
	.long	86388663                        ## 0x5262fb7
	.long	3015975733                      ## 0xb3c42335
	.long	1756702068                      ## 0x68b52574
	.long	1749815613                      ## 0x684c113d
	.long	3007125372                      ## 0xb33d177c
	.long	98507774                        ## 0x5df1bfe
	.long	3735952831                      ## 0xdeae1dbf
	.long	3010069691                      ## 0xb36a04bb
	.long	1746600698                      ## 0x681b02fa
	.long	3740864120                      ## 0xdef90e78
	.long	92801081                        ## 0x5880839
	.long	2962150815                      ## 0xb08ed59f
	.long	1811928030                      ## 0x6bffd3de
	.long	3709722460                      ## 0xdd1ddf5c
	.long	107796765                       ## 0x66cd91d
	.long	1806221337                      ## 0x6ba8c019
	.long	2967062104                      ## 0xb0d9c658
	.long	104581850                       ## 0x63bcada
	.long	3712666779                      ## 0xdd4acc9b
	.long	3719559378                      ## 0xddb3f8d2
	.long	113442451                       ## 0x6c2fe93
	.long	2954949137                      ## 0xb020f211
	.long	1800533072                      ## 0x6b51f450
	.long	110488916                       ## 0x695ed54
	.long	3722767125                      ## 0xdde4eb15
	.long	1795614615                      ## 0x6b06e797
	.long	2960646614                      ## 0xb077e1d6
	.long	1794412293                      ## 0x6af48f05
	.long	2978318660                      ## 0xb1858944
	.long	124224966                       ## 0x76785c6
	.long	3692462983                      ## 0xdc168387
	.long	2983369347                      ## 0xb1d29a83
	.long	1789107394                      ## 0x6aa39cc2
	.long	3695284288                      ## 0xdc419040
	.long	120624641                       ## 0x7309601
	.long	130654792                       ## 0x7c9a248
	.long	3703088137                      ## 0xdcb8a409
	.long	1784326283                      ## 0x6a5aa88b
	.long	2972430026                      ## 0xb12baeca
	.long	3706697678                      ## 0xdcefb7ce
	.long	127840655                       ## 0x79eb18f
	.long	2977742093                      ## 0xb17cbd0d
	.long	1779284812                      ## 0x6a0dbb4c
	.long	1701301141                      ## 0x6567cb95
	.long	3189165524                      ## 0xbe16cdd4
	.long	150258006                       ## 0x8f4c156
	.long	3548759831                      ## 0xd385c717
	.long	3191987731                      ## 0xbe41de13
	.long	1697699922                      ## 0x6530d852
	.long	3553809616                      ## 0xd3d2d4d0
	.long	144954001                       ## 0x8a3d291
	.long	140175064                       ## 0x85ae6d8
	.long	3542868121                      ## 0xd32be099
	.long	1707731995                      ## 0x65c9ec1b
	.long	3199789658                      ## 0xbeb8ea5a
	.long	3548181342                      ## 0xd37cf35e
	.long	135132447                       ## 0x80df51f
	.long	3203398045                      ## 0xbeeff99d
	.long	1704919004                      ## 0x659effdc
	.long	3206385935                      ## 0xbf1d910f
	.long	1684838222                      ## 0x646c974e
	.long	3532561356                      ## 0xd28e9bcc
	.long	167746957                       ## 0x9ff9d8d
	.long	1681622153                      ## 0x643b8489
	.long	3209331400                      ## 0xbf4a82c8
	.long	162041418                       ## 0x9a88e4a
	.long	3537471499                      ## 0xd2d9880b
	.long	3525360706                      ## 0xd220bc42
	.long	156350979                       ## 0x951ba03
	.long	3216225921                      ## 0xbfb3b681
	.long	1690480832                      ## 0x64c2b0c0
	.long	151431620                       ## 0x906a9c4
	.long	3531059077                      ## 0xd277af85
	.long	1687528199                      ## 0x6495a307
	.long	3219432774                      ## 0xbfe4a546
	.long	182614240                       ## 0xae278e0
	.long	3516104353                      ## 0xd1937ea1
	.long	1735488035                      ## 0x67717223
	.long	3154146402                      ## 0xbc007462
	.long	3519311206                      ## 0xd1c46d66
	.long	179661607                       ## 0xab56b27
	.long	3159844773                      ## 0xbc5767a5
	.long	1730568676                      ## 0x672661e4
	.long	1742689709                      ## 0x67df55ad
	.long	3165541356                      ## 0xbcae53ec
	.long	172777326                       ## 0xa4c5f6e
	.long	3510458671                      ## 0xd13d592f
	.long	3170451499                      ## 0xbcf9402b
	.long	1736984170                      ## 0x6788466a
	.long	3513404136                      ## 0xd16a4ae8
	.long	169561257                       ## 0xa1b4ca9
	.long	3499631226                      ## 0xd098227a
	.long	199828539                       ## 0xbe9243b
	.long	3171625145                      ## 0xbd0b28b9
	.long	1719283448                      ## 0x667a2ef8
	.long	197015548                       ## 0xbbe37fc
	.long	3503239613                      ## 0xd0cf31bd
	.long	1714240831                      ## 0x662d3d3f
	.long	3176938366                      ## 0xbd5c3b7e
	.long	3181711159                      ## 0xbda50f37
	.long	1725172086                      ## 0x66d40976
	.long	3493201396                      ## 0xd03605f4
	.long	189203381                       ## 0xb4703b5
	.long	1719868081                      ## 0x66831ab1
	.long	3186760944                      ## 0xbdf21cf0
	.long	185602162                       ## 0xb101072
	.long	3496023603                      ## 0xd0611633
	.long	3127684479                      ## 0xba6cad7f
	.long	1629334334                      ## 0x611dab3e
	.long	3623856060                      ## 0xd7ffa7bc
	.long	210674173                       ## 0xc8ea1fd
	.long	1632286969                      ## 0x614ab8f9
	.long	3124477624                      ## 0xba3bbeb8
	.long	215593530                       ## 0xcd9b23a
	.long	3618157691                      ## 0xd7a8b47b
	.long	3612442674                      ## 0xd7518032
	.long	203458163                       ## 0xc208673
	.long	3133311729                      ## 0xbac28af1
	.long	1639156912                      ## 0x61b38cb0
	.long	209163700                       ## 0xc7795b4
	.long	3607532533                      ## 0xd70693f5
	.long	1642372983                      ## 0x61e49f77
	.long	3130366262                      ## 0xba959936
	.long	1612117989                      ## 0x6016f7e5
	.long	3144151460                      ## 0xbb67f1a4
	.long	226884902                       ## 0xd85fd26
	.long	3606379367                      ## 0xd6f4fb67
	.long	3140543075                      ## 0xbb30e263
	.long	1614930978                      ## 0x6041e422
	.long	3601066144                      ## 0xd6a3e8a0
	.long	231927521                       ## 0xdd2eee1
	.long	220977832                       ## 0xd2bdaa8
	.long	3596279017                      ## 0xd65adce9
	.long	1622724715                      ## 0x60b8d06b
	.long	3150566954                      ## 0xbbc9d62a
	.long	3591229230                      ## 0xd60dcf2e
	.long	226281839                       ## 0xd7cc96f
	.long	3147744749                      ## 0xbb9ec5ed
	.long	1626325932                      ## 0x60efc3ac
	.long	3588824586                      ## 0xd5e91e0a
	.long	244848715                       ## 0xe98184b
	.long	3095008457                      ## 0xb87a14c9
	.long	1661670024                      ## 0x630b1288
	.long	248449932                       ## 0xecf0b8c
	.long	3586002381                      ## 0xd5be0dcd
	.long	1666974031                      ## 0x635c014f
	.long	3089958670                      ## 0xb82d070e
	.long	3100914503                      ## 0xb8d43347
	.long	1671771398                      ## 0x63a53506
	.long	3578214788                      ## 0xd5473984
	.long	238436293                       ## 0xe363fc5
	.long	1676814017                      ## 0x63f226c1
	.long	3095601280                      ## 0xb8832080
	.long	241249282                       ## 0xe612c02
	.long	3574606403                      ## 0xd5102a43
	.long	261309584                       ## 0xf934490
	.long	3571598033                      ## 0xd4e242d1
	.long	1644187219                      ## 0x62004e53
	.long	3111208978                      ## 0xb9714812
	.long	3568652566                      ## 0xd4b55116
	.long	264525655                       ## 0xfc45757
	.long	3106298837                      ## 0xb9265bd5
	.long	1649892756                      ## 0x62575d94
	.long	1655597533                      ## 0x62ae69dd
	.long	3118428060                      ## 0xb9df6f9c
	.long	255681310                       ## 0xf3d631e
	.long	3561776479                      ## 0xd44c655f
	.long	3112729691                      ## 0xb9887c5b
	.long	1660516890                      ## 0x62f97a1a
	.long	3558569624                      ## 0xd41b7698
	.long	258633945                       ## 0xf6a70d9
	.long	0                               ## 0x0
	.long	3402602282                      ## 0xcacf972a
	.long	1324230677                      ## 0x4eee2815
	.long	2216804159                      ## 0x8421bf3f
	.long	2648461354                      ## 0x9ddc502a
	.long	1460913920                      ## 0x5713c700
	.long	3543300159                      ## 0xd332783f
	.long	436072213                       ## 0x19fdef15
	.long	3771311637                      ## 0xe0c9a615
	.long	705048895                       ## 0x2a06313f
	.long	2921827840                      ## 0xae278e00
	.long	1692932394                      ## 0x64e8192a
	.long	2098591295                      ## 0x7d15f63f
	.long	3084542229                      ## 0xb7da6115
	.long	872144426                       ## 0x33fbde2a
	.long	4180953344                      ## 0xf9344900
	.long	451037803                       ## 0x1ae24a6b
	.long	3492666689                      ## 0xd02ddd41
	.long	1410097790                      ## 0x540c627e
	.long	2663642452                      ## 0x9ec3f554
	.long	2268994113                      ## 0x873e1a41
	.long	1307676011                      ## 0x4df18d6b
	.long	3385864788                      ## 0xc9d03254
	.long	52405630                        ## 0x31fa57e
	.long	4197182590                      ## 0xfa2bec7e
	.long	820280148                       ## 0x30e47b54
	.long	3032859755                      ## 0xb4c5c46b
	.long	2114605889                      ## 0x7e0a5341
	.long	1744288852                      ## 0x67f7bc54
	.long	2906139518                      ## 0xad382b7e
	.long	689542209                       ## 0x29199441
	.long	3822453611                      ## 0xe3d6036b
	.long	902075606                       ## 0x35c494d6
	.long	4278911996                      ## 0xff0b03fc
	.long	2066398403                      ## 0x7b2abcc3
	.long	2984586217                      ## 0xb1e52be9
	.long	2820195580                      ## 0xa818c4fc
	.long	1658278870                      ## 0x62d753d6
	.long	3874942185                      ## 0xe6f6ece9
	.long	741964739                       ## 0x2c397bc3
	.long	3574411971                      ## 0xd50d32c3
	.long	532850153                       ## 0x1fc2a5e9
	.long	2615352022                      ## 0x9be31ad6
	.long	1361874428                      ## 0x512c8dfc
	.long	1221681897                      ## 0x48d162e9
	.long	2183067075                      ## 0x821ef5c3
	.long	104811260                       ## 0x63f4afc
	.long	3438337494                      ## 0xccf0ddd6
	.long	791076541                       ## 0x2f26debd
	.long	3857271191                      ## 0xe5e94997
	.long	1640560296                      ## 0x61c8f6a8
	.long	2869387650                      ## 0xab076182
	.long	3002764951                      ## 0xb2fa8e97
	.long	2016745917                      ## 0x783519bd
	.long	4229211778                      ## 0xfc14a682
	.long	920334760                       ## 0x36db31a8
	.long	3488577704                      ## 0xcfef78a8
	.long	86044546                        ## 0x520ef82
	.long	2164347069                      ## 0x810150bd
	.long	1271842711                      ## 0x4bcec797
	.long	1379084418                      ## 0x52332882
	.long	2566700968                      ## 0x98fcbfa8
	.long	484245655                       ## 0x1cdd0097
	.long	3591542717                      ## 0xd61297bd
	.long	1804151212                      ## 0x6b8929ac
	.long	2705768070                      ## 0xa146be86
	.long	627507641                       ## 0x256701b9
	.long	4020803219                      ## 0xefa89693
	.long	4132796806                      ## 0xf6557986
	.long	1016786604                      ## 0x3c9aeeac
	.long	3099283859                      ## 0xb8bb5193
	.long	1920255673                      ## 0x7274c6b9
	.long	2336264121                      ## 0x8b408fb9
	.long	1099896979                      ## 0x418f1893
	.long	3316557740                      ## 0xc5aea7ac
	.long	258027654                       ## 0xf613086
	.long	379379603                       ## 0x169cdf93
	.long	3696445625                      ## 0xdc5348b9
	.long	1483929478                      ## 0x5872f786
	.long	2461884588                      ## 0x92bd60ac
	.long	1902863303                      ## 0x716b63c7
	.long	3148149997                      ## 0xbba4f4ed
	.long	1065700306                      ## 0x3f854bd2
	.long	4115324152                      ## 0xf54adcf8
	.long	3971429357                      ## 0xecb733ed
	.long	645440711                       ## 0x2678a4c7
	.long	2723748856                      ## 0xa2591bf8
	.long	1754696914                      ## 0x68968cd2
	.long	2443363794                      ## 0x91a2c5d2
	.long	1533891320                      ## 0x5b6d52f8
	.long	3746360775                      ## 0xdf4cedc7
	.long	360938221                       ## 0x15837aed
	.long	209622520                       ## 0xc7e95f8
	.long	3333489362                      ## 0xc6b102d2
	.long	1116782061                      ## 0x4290bded
	.long	2287938247                      ## 0x885f2ac7
	.long	1582153082                      ## 0x5e4dbd7a
	.long	2491558480                      ## 0x94822a50
	.long	279156079                       ## 0x10a3956f
	.long	3664511557                      ## 0xda6c0245
	.long	3281120592                      ## 0xc391ed50
	.long	157186682                       ## 0x95e7a7a
	.long	2373961029                      ## 0x8d7fc545
	.long	1202737775                      ## 0x47b0526f
	.long	3196328815                      ## 0xbe841b6f
	.long	1951108165                      ## 0x744b8c45
	.long	4033491834                      ## 0xf06a337a
	.long	983934032                       ## 0x3aa5a450
	.long	592988997                       ## 0x23584b45
	.long	3919043695                      ## 0xe997dc6f
	.long	1840669520                      ## 0x6db66350
	.long	2809787514                      ## 0xa779f47a
	.long	1152382737                      ## 0x44aff711
	.long	2388680763                      ## 0x8e60603b
	.long	172089092                       ## 0xa41df04
	.long	3230550062                      ## 0xc08e482e
	.long	3648235323                      ## 0xd973a73b
	.long	331100177                       ## 0x13bc3011
	.long	2543685422                      ## 0x979d8f2e
	.long	1565661188                      ## 0x5d521804
	.long	2758168836                      ## 0xa4665104
	.long	1856620078                      ## 0x6ea9c62e
	.long	3934812433                      ## 0xea887911
	.long	541584955                       ## 0x2047ee3b
	.long	968491310                       ## 0x39ba012e
	.long	4084569604                      ## 0xf3759604
	.long	2002004283                      ## 0x7754293b
	.long	3181100561                      ## 0xbd9bbe11
	.long	3608302424                      ## 0xd7125358
	.long	501073010                       ## 0x1dddc472
	.long	2583460685                      ## 0x99fc7b4d
	.long	1395911783                      ## 0x5333ec67
	.long	1255015282                      ## 0x4ace0372
	.long	2147587160                      ## 0x80019458
	.long	69217127                        ## 0x4202b67
	.long	3471817805                      ## 0xceefbc4d
	.long	937162061                       ## 0x37dbf54d
	.long	4245971559                      ## 0xfd146267
	.long	2033573208                      ## 0x7935dd58
	.long	3019524722                      ## 0xb3fa4a72
	.long	2852627815                      ## 0xaa07a567
	.long	1623732813                      ## 0x60c8324d
	.long	3840511346                      ## 0xe4e98d72
	.long	774249048                       ## 0x2e261a58
	.long	3455064371                      ## 0xcdf01933
	.long	121605657                       ## 0x73f8e19
	.long	2199793958                      ## 0x831e3126
	.long	1238476300                      ## 0x49d1a60c
	.long	1345079577                      ## 0x502c4919
	.long	2598624819                      ## 0x9ae3de33
	.long	516055308                       ## 0x1ec2610c
	.long	3557684774                      ## 0xd40df626
	.long	758759206                       ## 0x2d39bf26
	.long	3891669004                      ## 0xe7f6280c
	.long	1675073331                      ## 0x63d79733
	.long	2836922393                      ## 0xa9180019
	.long	2967858956                      ## 0xb0e5ef0c
	.long	2049603622                      ## 0x7a2a7826
	.long	4262184729                      ## 0xfe0bc719
	.long	885280819                       ## 0x34c45033
	.long	3805726606                      ## 0xe2d6c78e
	.long	672747684                       ## 0x281950a4
	.long	2889412507                      ## 0xac38ef9b
	.long	1727494321                      ## 0x66f778b1
	.long	2131400612                      ## 0x7f0a97a4
	.long	3049586830                      ## 0xb5c5008e
	.long	837074865                       ## 0x31e4bfb1
	.long	4213909659                      ## 0xfb2b289b
	.long	35611035                        ## 0x21f619b
	.long	3369137841                      ## 0xc8d0f6b1
	.long	1290881422                      ## 0x4cf1498e
	.long	2252267172                      ## 0x863edea4
	.long	2680369585                      ## 0x9fc331b1
	.long	1426892443                      ## 0x550ca69b
	.long	3509393828                      ## 0xd12d19a4
	.long	467832462                       ## 0x1be28e8e
	.long	4164193765                      ## 0xf8348de5
	.long	855317199                       ## 0x32fb1acf
	.long	3067782640                      ## 0xb6daa5f0
	.long	2081764058                      ## 0x7c1532da
	.long	1709759951                      ## 0x65e8ddcf
	.long	2938587877                      ## 0xaf274ae5
	.long	721876442                       ## 0x2b06f5da
	.long	3788071664                      ## 0xe1c962f0
	.long	419245040                       ## 0x18fd2bf0
	.long	3526540506                      ## 0xd232bcda
	.long	1444086757                      ## 0x561303e5
	.long	2631701711                      ## 0x9cdc94cf
	.long	2233564122                      ## 0x85217bda
	.long	1341058288                      ## 0x4feeecf0
	.long	3419362255                      ## 0xcbcf53cf
	.long	16827621                        ## 0x100c4e5
	.long	3164306164                      ## 0xbc9b7af4
	.long	1985277406                      ## 0x7654edde
	.long	4067775201                      ## 0xf27552e1
	.long	951764427                       ## 0x38bac5cb
	.long	558312158                       ## 0x21472ade
	.long	3951607284                      ## 0xeb88bdf4
	.long	1873347275                      ## 0x6fa902cb
	.long	2774963681                      ## 0xa56695e1
	.long	1548934369                      ## 0x5c52dce1
	.long	2526890955                      ## 0x969d4bcb
	.long	314373364                       ## 0x12bcf4f4
	.long	3631440862                      ## 0xd87363de
	.long	3247344843                      ## 0xc18e8ccb
	.long	188816353                       ## 0xb411be1
	.long	2405475550                      ## 0x8f60a4de
	.long	1169110004                      ## 0x45af33f4
	.long	2792960159                      ## 0xa679309f
	.long	1823909813                      ## 0x6cb6a7b5
	.long	3902216330                      ## 0xe897188a
	.long	576229280                       ## 0x22588fa0
	.long	1000693941                      ## 0x3ba560b5
	.long	4050319263                      ## 0xf16af79f
	.long	1967868064                      ## 0x754b48a0
	.long	3213156234                      ## 0xbf84df8a
	.long	1185977994                      ## 0x46b0968a
	.long	2357133728                      ## 0x8c7f01a0
	.long	140426911                       ## 0x85ebe9f
	.long	3264293301                      ## 0xc29129b5
	.long	3681339040                      ## 0xdb6cc6a0
	.long	295915914                       ## 0x11a3518a
	.long	2508385973                      ## 0x9582eeb5
	.long	1598912927                      ## 0x5f4d799f
	.long	2304765474                      ## 0x895fee22
	.long	1133541640                      ## 0x43907908
	.long	3350316599                      ## 0xc7b1c637
	.long	226382109                       ## 0xd7e511d
	.long	344178184                       ## 0x1483be08
	.long	3729533218                      ## 0xde4c2922
	.long	1517131293                      ## 0x5a6d961d
	.long	2426536247                      ## 0x90a20137
	.long	1771456567                      ## 0x69964837
	.long	2740576029                      ## 0xa359df1d
	.long	662200354                       ## 0x27786022
	.long	3988256520                      ## 0xedb7f708
	.long	4098496541                      ## 0xf44a181d
	.long	1048940343                      ## 0x3e858f37
	.long	3131322376                      ## 0xbaa43008
	.long	1886103330                      ## 0x706ba722
	.long	2478679113                      ## 0x93bda449
	.long	1500656483                      ## 0x59723363
	.long	3713240156                      ## 0xdd538c5c
	.long	396106614                       ## 0x179c1b76
	.long	241300579                       ## 0xe61f463
	.long	3299763017                      ## 0xc4ae6349
	.long	1083169910                      ## 0x408fdc76
	.long	2319469404                      ## 0x8a404b5c
	.long	1936982620                      ## 0x7374025c
	.long	3116078454                      ## 0xb9bb9576
	.long	1033513545                      ## 0x3d9a2a49
	.long	4149591395                      ## 0xf755bd63
	.long	4004008566                      ## 0xeea85276
	.long	610780508                       ## 0x2467c55c
	.long	2688973411                      ## 0xa0467a63
	.long	1787424073                      ## 0x6a89ed49
	.long	0                               ## 0x0
	.long	1968546033                      ## 0x7555a0f1
	.long	3937092066                      ## 0xeaab41e2
	.long	2684281107                      ## 0x9ffee113
	.long	237471109                       ## 0xe278585
	.long	2071078260                      ## 0x7b722574
	.long	3834430567                      ## 0xe48cc467
	.long	2446943382                      ## 0x91d96496
	.long	474942218                       ## 0x1c4f0b0a
	.long	1763355643                      ## 0x691aabfb
	.long	4142156520                      ## 0xf6e44ae8
	.long	2209475097                      ## 0x83b1ea19
	.long	308842127                       ## 0x12688e8f
	.long	1732062846                      ## 0x673d2e7e
	.long	4173582189                      ## 0xf8c3cf6d
	.long	2375446428                      ## 0x8d966f9c
	.long	949884436                       ## 0x389e1614
	.long	1305196261                      ## 0x4dcbb6e5
	.long	3526711286                      ## 0xd23557f6
	.long	2808149767                      ## 0xa760f707
	.long	918131601                       ## 0x36b99391
	.long	1139553120                      ## 0x43ec3360
	.long	3692221043                      ## 0xdc12d273
	.long	2840031874                      ## 0xa9477282
	.long	617684254                       ## 0x24d11d1e
	.long	1367653871                      ## 0x5184bdef
	.long	3464125692                      ## 0xce7a5cfc
	.long	3140484109                      ## 0xbb2ffc0d
	.long	720803995                       ## 0x2af6989b
	.long	1604532330                      ## 0x5fa3386a
	.long	3227375993                      ## 0xc05dd979
	.long	3037231496                      ## 0xb5087988
	.long	1899768872                      ## 0x713c2c28
	.long	74026201                        ## 0x4698cd9
	.long	2610392522                      ## 0x9b976dca
	.long	4005743931                      ## 0xeec2cd3b
	.long	2132519341                      ## 0x7f1ba9ad
	.long	172886364                       ## 0xa4e095c
	.long	2511398991                      ## 0x95b0e84f
	.long	3773122750                      ## 0xe0e548be
	.long	1836263202                      ## 0x6d732722
	.long	405178323                       ## 0x182687d3
	.long	2279106240                      ## 0x87d866c0
	.long	4069377585                      ## 0xf28dc631
	.long	1666491047                      ## 0x6354a2a7
	.long	369164886                       ## 0x16010256
	.long	2315248453                      ## 0x89ffe345
	.long	4239016884                      ## 0xfcaa43b4
	.long	1235368508                      ## 0x49a23a3c
	.long	1022859981                      ## 0x3cf79acd
	.long	2735307742                      ## 0xa3097bde
	.long	3596409647                      ## 0xd65cdb2f
	.long	1199947705                      ## 0x4785bfb9
	.long	852500296                       ## 0x32d01f48
	.long	2905538139                      ## 0xad2efe5b
	.long	3631963818                      ## 0xd87b5eaa
	.long	1441607990                      ## 0x55ed3136
	.long	548966855                       ## 0x20b891c7
	.long	3209064660                      ## 0xbf4670d4
	.long	3390296101                      ## 0xca13d025
	.long	1540011187                      ## 0x5bcab4b3
	.long	782177346                       ## 0x2e9f1442
	.long	2975987025                      ## 0xb161f551
	.long	3291764128                      ## 0xc43455a0
	.long	3799537744                      ## 0xe2785850
	.long	2536372385                      ## 0x972df8a1
	.long	148052402                       ## 0x8d319b2
	.long	2105981251                      ## 0x7d86b943
	.long	3965705685                      ## 0xec5fddd5
	.long	2567601444                      ## 0x990a7d24
	.long	116694071                       ## 0x6f49c37
	.long	1939946694                      ## 0x73a13cc6
	.long	4265038682                      ## 0xfe37535a
	.long	2338517931                      ## 0x8b62f3ab
	.long	345772728                       ## 0x149c12b8
	.long	1640608329                      ## 0x61c9b249
	.long	4027635423                      ## 0xf010d6df
	.long	2235921966                      ## 0x8545762e
	.long	448501565                       ## 0x1abb973d
	.long	1877882828                      ## 0x6fee37cc
	.long	3672526404                      ## 0xdae64e44
	.long	2947804853                      ## 0xafb3eeb5
	.long	810356646                       ## 0x304d0fa6
	.long	1159245655                      ## 0x4518af57
	.long	3569470401                      ## 0xd4c1cbc1
	.long	2710858544                      ## 0xa1946b30
	.long	1047169571                      ## 0x3e6a8a23
	.long	1262430930                      ## 0x4b3f2ad2
	.long	3332982094                      ## 0xc6a9454e
	.long	3019695551                      ## 0xb3fce5bf
	.long	738329772                       ## 0x2c0204ac
	.long	1498915933                      ## 0x5957a45d
	.long	3364798667                      ## 0xc88ec0cb
	.long	3185270842                      ## 0xbddb603a
	.long	572883241                       ## 0x22258129
	.long	1466966488                      ## 0x577021d8
	.long	2470737016                      ## 0x93447478
	.long	3859928201                      ## 0xe611d489
	.long	2045719962                      ## 0x79ef359a
	.long	213554539                       ## 0xcba956b
	.long	2640572925                      ## 0x9d63f1fd
	.long	3895873804                      ## 0xe836510c
	.long	2009640991                      ## 0x77c8b01f
	.long	43847918                        ## 0x29d10ee
	.long	2399895410                      ## 0x8f0b7f72
	.long	4200521603                      ## 0xfa5edf83
	.long	1705000592                      ## 0x65a03e90
	.long	284532321                       ## 0x10f59e61
	.long	2167208695                      ## 0x812cfaf7
	.long	4101593606                      ## 0xf4795a06
	.long	1804057365                      ## 0x6b87bb15
	.long	517086180                       ## 0x1ed21be4
	.long	2883215980                      ## 0xabda626c
	.long	3733963421                      ## 0xde8fc29d
	.long	1097933710                      ## 0x4171238e
	.long	874808191                       ## 0x3424837f
	.long	2784880617                      ## 0xa5fde7e9
	.long	3500689176                      ## 0xd0a84718
	.long	1331078667                      ## 0x4f56a60b
	.long	973276922                       ## 0x3a0306fa
	.long	3080022374                      ## 0xb7956966
	.long	3267414423                      ## 0xc2c0c997
	.long	1564354692                      ## 0x5d3e2884
	.long	678135925                       ## 0x286b8875
	.long	3115511011                      ## 0xb9b2ece3
	.long	3437710354                      ## 0xcce74c12
	.long	1394191617                      ## 0x5319ad01
	.long	642518512                       ## 0x264c0df0
	.long	528594657                       ## 0x1f81b6e1
	.long	1792284176                      ## 0x6ad41610
	.long	4113233667                      ## 0xf52af703
	.long	2155829234                      ## 0x807f57f2
	.long	296104804                       ## 0x11a63364
	.long	1693684629                      ## 0x64f39395
	.long	4211962502                      ## 0xfb0d7286
	.long	2388185719                      ## 0x8e58d277
	.long	63880683                        ## 0x3cebdeb
	.long	1989877018                      ## 0x769b1d1a
	.long	3915774985                      ## 0xe965fc09
	.long	2620415224                      ## 0x9c305cf8
	.long	233388142                       ## 0xde9386e
	.long	2025625759                      ## 0x78bc989f
	.long	3879893388                      ## 0xe742798c
	.long	2451036541                      ## 0x9217d97d
	.long	656384245                       ## 0x271fa0f5
	.long	1380581380                      ## 0x524a0004
	.long	3451183383                      ## 0xcdb4e117
	.long	3101770214                      ## 0xb8e141e6
	.long	691545456                       ## 0x29382570
	.long	1550681473                      ## 0x5c6d8581
	.long	3281216658                      ## 0xc3936492
	.long	3066479715                      ## 0xb6c6c463
	.long	995142655                       ## 0x3b50abff
	.long	1308953358                      ## 0x4e050b0e
	.long	3522947613                      ## 0xd1fbea1d
	.long	2762885868                      ## 0xa4ae4aec
	.long	897003130                       ## 0x35772e7a
	.long	1076006539                      ## 0x40228e8b
	.long	3755765656                      ## 0xdfdc6f98
	.long	2861158249                      ## 0xaa89cf69
	.long	1857919689                      ## 0x6ebd9ac9
	.long	468204088                       ## 0x1be83a38
	.long	2216090411                      ## 0x8416db2b
	.long	4047731674                      ## 0xf1437bda
	.long	1620713292                      ## 0x609a1f4c
	.long	365936573                       ## 0x15cfbfbd
	.long	2318491310                      ## 0x8a315eae
	.long	4284808799                      ## 0xff64fe5f
	.long	1928499651                      ## 0x72f291c3
	.long	128397618                       ## 0x7a73132
	.long	2556022817                      ## 0x9859d021
	.long	3977015504                      ## 0xed0c70d0
	.long	2094339142                      ## 0x7cd51446
	.long	159429815                       ## 0x980b4b7
	.long	2524861860                      ## 0x967e55a4
	.long	3811308885                      ## 0xe32bf555
	.long	1445170397                      ## 0x56238cdd
	.long	594947116                       ## 0x23762c2c
	.long	3163082047                      ## 0xbc88cd3f
	.long	3386731982                      ## 0xc9dd6dce
	.long	1476659544                      ## 0x58040958
	.long	760326569                       ## 0x2d51a9a9
	.long	2997831866                      ## 0xb2af48ba
	.long	3355109451                      ## 0xc7fae84b
	.long	1248626647                      ## 0x4a6c87d7
	.long	1060710182                      ## 0x3f392726
	.long	2697446965                      ## 0xa0c7c635
	.long	3583141572                      ## 0xd59266c4
	.long	1145766482                      ## 0x444b0252
	.long	824091299                       ## 0x311ea2a3
	.long	2933932976                      ## 0xaee043b0
	.long	3686130497                      ## 0xdbb5e341
	.long	4261015217                      ## 0xfdf9eeb1
	.long	2292993600                      ## 0x88ac4e40
	.long	391294803                       ## 0x1752af53
	.long	1644629922                      ## 0x62070fa2
	.long	4091439924                      ## 0xf3de6b34
	.long	2257308613                      ## 0x868bcbc5
	.long	427109078                       ## 0x19752ad6
	.long	1814071847                      ## 0x6c208a27
	.long	3786859963                      ## 0xe1b6e5bb
	.long	2497922378                      ## 0x94e3454a
	.long	186491993                       ## 0xb1da459
	.long	2118649000                      ## 0x7e4804a8
	.long	4019281982                      ## 0xef91603e
	.long	2596585679                      ## 0x9ac4c0cf
	.long	87695836                        ## 0x53a21dc
	.long	1886355757                      ## 0x706f812d
	.long	3311925413                      ## 0xc567f8a5
	.long	2956089428                      ## 0xb0325854
	.long	801945927                       ## 0x2fccb947
	.long	1519983030                      ## 0x5a9919b6
	.long	3410001184                      ## 0xcb407d20
	.long	3189104081                      ## 0xbe15ddd1
	.long	569064642                       ## 0x21eb3cc2
	.long	1421777971                      ## 0x54be9c33
	.long	3643339695                      ## 0xd928f3af
	.long	2893894494                      ## 0xac7d535e
	.long	864268877                       ## 0x3383b24d
	.long	1188434620                      ## 0x46d612bc
	.long	3608114730                      ## 0xd70f762a
	.long	2723862235                      ## 0xa25ad6db
	.long	1034172360                      ## 0x3da437c8
	.long	1223792441                      ## 0x48f19739
	.long	2361770649                      ## 0x8cc5c299
	.long	4186989160                      ## 0xf9906268
	.long	1718518651                      ## 0x666e837b
	.long	322642826                       ## 0x133b238a
	.long	2195867420                      ## 0x82e2471c
	.long	4156024813                      ## 0xf7b7e7ed
	.long	1749616382                      ## 0x684906fe
	.long	488416783                       ## 0x1d1ca60f
	.long	2425014675                      ## 0x908ac993
	.long	3856623970                      ## 0xe5df6962
	.long	2049017969                      ## 0x7a218871
	.long	259270784                       ## 0xf742880
	.long	2662157334                      ## 0x9ead4c16
	.long	3958959335                      ## 0xebf8ece7
	.long	1946553844                      ## 0x74060df4
	.long	22260997                        ## 0x153ad05
	.long	3025917069                      ## 0xb45bd48d
	.long	3238950012                      ## 0xc10e747c
	.long	1592825199                      ## 0x5ef0956f
	.long	732247454                       ## 0x2ba5359e
	.long	3128709384                      ## 0xba7c5108
	.long	3475632633                      ## 0xcf29f1f9
	.long	1356271850                      ## 0x50d710ea
	.long	629321755                       ## 0x2582b01b
	.long	2819940231                      ## 0xa814df87
	.long	3712057206                      ## 0xdd417f76
	.long	1119854181                      ## 0x42bf9e65
	.long	938098324                       ## 0x37ea3e94
	.long	2788383234                      ## 0xa6335a02
	.long	3546741491                      ## 0xd366faf3
	.long	1285037024                      ## 0x4c981be0
	.long	969784081                       ## 0x39cdbb11
	.long	0                               ## 0x0
	.long	1057189314                      ## 0x3f036dc2
	.long	2114378628                      ## 0x7e06db84
	.long	1090893382                      ## 0x4105b646
	.long	4228757256                      ## 0xfc0db708
	.long	3272530634                      ## 0xc30edaca
	.long	2181786764                      ## 0x820b6c8c
	.long	3171418446                      ## 0xbd08014e
	.long	594176081                       ## 0x236a6851
	.long	476644755                       ## 0x1c690593
	.long	1567405013                      ## 0x5d6cb3d5
	.long	1651498519                      ## 0x626fde17
	.long	3748126553                      ## 0xdf67df59
	.long	3764695707                      ## 0xe064b29b
	.long	2707490013                      ## 0xa16104dd
	.long	2657249567                      ## 0x9e62691f
	.long	1188352162                      ## 0x46d4d0a2
	.long	2044181856                      ## 0x79d7bd60
	.long	953289510                       ## 0x38d20b26
	.long	131163876                       ## 0x7d166e4
	.long	3134810026                      ## 0xbad967aa
	.long	2245659240                      ## 0x85da0a68
	.long	3302997038                      ## 0xc4dfbc2e
	.long	4225552876                      ## 0xfbdcd1ec
	.long	1706998003                      ## 0x65beb8f3
	.long	1522390321                      ## 0x5abdd531
	.long	465068919                       ## 0x1bb86377
	.long	616238773                       ## 0x24bb0eb5
	.long	2578649083                      ## 0x99b30ffb
	.long	2796577337                      ## 0xa6b06239
	.long	3887453311                      ## 0xe7b5d47f
	.long	3635853757                      ## 0xd8b6b9bd
	.long	2376704324                      ## 0x8da9a144
	.long	2997537926                      ## 0xb2aacc86
	.long	4088363712                      ## 0xf3af7ac0
	.long	3433830146                      ## 0xccac1702
	.long	1906579020                      ## 0x71a4164c
	.long	1319598990                      ## 0x4ea77b8e
	.long	262327752                       ## 0xfa2cdc8
	.long	815898634                       ## 0x30a1a00a
	.long	2932066581                      ## 0xaec3c915
	.long	2445321431                      ## 0x91c0a4d7
	.long	3502576273                      ## 0xd0c51291
	.long	4022763347                      ## 0xefc67f53
	.long	1389264413                      ## 0x52ce7e1d
	.long	1842156511                      ## 0x6dcd13df
	.long	751347097                       ## 0x2cc8a599
	.long	332122203                       ## 0x13cbc85b
	.long	3413996006                      ## 0xcb7d71e6
	.long	4101905444                      ## 0xf47e1c24
	.long	3044780642                      ## 0xb57baa62
	.long	2323171232                      ## 0x8a78c7a0
	.long	930137838                       ## 0x3770c6ee
	.long	141798188                       ## 0x873ab2c
	.long	1232477546                      ## 0x49761d6a
	.long	1987408040                      ## 0x767570a8
	.long	3893828023                      ## 0xe81719b7
	.long	3608441973                      ## 0xd7147475
	.long	2517746227                      ## 0x9611c233
	.long	2836574193                      ## 0xa912aff1
	.long	337292991                       ## 0x141aaebf
	.long	723108733                       ## 0x2b19c37d
	.long	1780249915                      ## 0x6a1c753b
	.long	1428101369                      ## 0x551f18f9
	.long	3223471305                      ## 0xc02244c9
	.long	4280363275                      ## 0xff21290b
	.long	3190071117                      ## 0xbe249f4d
	.long	2166878863                      ## 0x8127f28f
	.long	1009775553                      ## 0x3c2ff3c1
	.long	53255683                        ## 0x32c9e03
	.long	1109993541                      ## 0x42292845
	.long	2099922311                      ## 0x7d2a4587
	.long	3813158040                      ## 0xe3482c98
	.long	3695919450                      ## 0xdc4b415a
	.long	2639197980                      ## 0x9d4ef71c
	.long	2722994910                      ## 0xa24d9ade
	.long	524655504                       ## 0x1f459b90
	.long	541521490                       ## 0x2046f652
	.long	1631797268                      ## 0x61434014
	.long	1581264342                      ## 0x5e402dd6
	.long	2264306795                      ## 0x86f6946b
	.long	3119905193                      ## 0xb9f5f9a9
	.long	4176498671                      ## 0xf8f04fef
	.long	3354599981                      ## 0xc7f3222d
	.long	2063278947                      ## 0x7afb2363
	.long	1173900961                      ## 0x45f84ea1
	.long	83753191                        ## 0x4fdf8e7
	.long	1006540069                      ## 0x3bfe9525
	.long	2778528826                      ## 0xa59cfc3a
	.long	2594148856                      ## 0x9a9f91f8
	.long	3684313022                      ## 0xdb9a27be
	.long	3835251324                      ## 0xe4994a7c
	.long	1502694194                      ## 0x59914b32
	.long	1720854256                      ## 0x669226f0
	.long	664244406                       ## 0x279790b6
	.long	412417396                       ## 0x1894fd74
	.long	1301013901                      ## 0x4d8be58d
	.long	1921550415                      ## 0x7288884f
	.long	864894473                       ## 0x338d3e09
	.long	210654155                       ## 0xc8e53cb
	.long	2978370181                      ## 0xb1865285
	.long	2391097159                      ## 0x8e853f47
	.long	3481307393                      ## 0xcf808901
	.long	4035175619                      ## 0xf083e4c3
	.long	1860275676                      ## 0x6ee18ddc
	.long	1373823006                      ## 0x51e2e01e
	.long	283596376                       ## 0x10e75658
	.long	803486618                       ## 0x2fe43b9a
	.long	2464955092                      ## 0x92ec3ad4
	.long	2918143766                      ## 0xadef5716
	.long	3974816080                      ## 0xeceae150
	.long	3555298450                      ## 0xd3e98c92
	.long	190788911                       ## 0xb5f352f
	.long	878467309                       ## 0x345c58ed
	.long	1968828075                      ## 0x7559eeab
	.long	1247445865                      ## 0x4a5a8369
	.long	4149379623                      ## 0xf7528227
	.long	3360813029                      ## 0xc851efe5
	.long	2304006563                      ## 0x895459a3
	.long	3059168353                      ## 0xb6573461
	.long	674585982                       ## 0x28355d7e
	.long	389427388                       ## 0x173630bc
	.long	1446217466                      ## 0x563386fa
	.long	1764813624                      ## 0x6930eb38
	.long	3560499830                      ## 0xd438ea76
	.long	3946547124                      ## 0xeb3b87b4
	.long	2856202738                      ## 0xaa3e31f2
	.long	2503826480                      ## 0x953d5c30
	.long	1530236883                      ## 0x5b358fd3
	.long	1681318417                      ## 0x6436e211
	.long	624120919                       ## 0x25335457
	.long	439368085                       ## 0x1a303995
	.long	2805479643                      ## 0xa73838db
	.long	2554025241                      ## 0x983b5519
	.long	3644777311                      ## 0xd93ee35f
	.long	3862793885                      ## 0xe63d8e9d
	.long	2019551106                      ## 0x785fe782
	.long	1197247040                      ## 0x475c8a40
	.long	106511366                       ## 0x6593c06
	.long	962220484                       ## 0x395a51c4
	.long	2219987082                      ## 0x8452508a
	.long	3142663496                      ## 0xbb513d48
	.long	4199844622                      ## 0xfa548b0e
	.long	3310872268                      ## 0xc557e6cc
	.long	501309297                       ## 0x1de15f71
	.long	585249459                       ## 0x22e232b3
	.long	1676117237                      ## 0x63e784f5
	.long	1558505783                      ## 0x5ce4e937
	.long	3790399609                      ## 0xe1ece879
	.long	3740239291                      ## 0xdeef85bb
	.long	2682926077                      ## 0x9fea33fd
	.long	2699648575                      ## 0xa0e95e3f
	.long	1049311008                      ## 0x3e8b3720
	.long	25713378                        ## 0x1885ae2
	.long	1083042980                      ## 0x408deca4
	.long	2140045670                      ## 0x7f8e8166
	.long	3263594536                      ## 0xc2868028
	.long	4253412842                      ## 0xfd85edea
	.long	3162528684                      ## 0xbc805bac
	.long	2206414446                      ## 0x8383366e
	.long	3600559767                      ## 0xd69c2e97
	.long	3919528789                      ## 0xe99f4355
	.long	2828727571                      ## 0xa89af513
	.long	2543425745                      ## 0x979998d1
	.long	714185119                       ## 0x2a91999f
	.long	361952349                       ## 0x1592f45d
	.long	1419199003                      ## 0x5497421b
	.long	1804873689                      ## 0x6b942fd9
	.long	4126557894                      ## 0xf5f646c6
	.long	3405064964                      ## 0xcaf52b04
	.long	2347801922                      ## 0x8bf09d42
	.long	3035885696                      ## 0xb4f3f080
	.long	167506382                       ## 0x9fbf1ce
	.long	922262540                       ## 0x36f89c0c
	.long	2013080138                      ## 0x77fd2a4a
	.long	1224624008                      ## 0x48fe4788
	.long	2420702773                      ## 0x9048fe35
	.long	2940965879                      ## 0xaf4b93f7
	.long	3998098865                      ## 0xee4e25b1
	.long	3511502963                      ## 0xd14d4873
	.long	1816480061                      ## 0x6c45493d
	.long	1397105919                      ## 0x534624ff
	.long	306418361                       ## 0x124392b9
	.long	759234427                       ## 0x2d40ff7b
	.long	3005388388                      ## 0xb3229664
	.long	2351037350                      ## 0x8c21fba6
	.long	3441708512                      ## 0xcd244de0
	.long	4062650402                      ## 0xf2272022
	.long	1328488812                      ## 0x4f2f216c
	.long	1881951406                      ## 0x702c4cae
	.long	824834792                       ## 0x3129fae8
	.long	237672234                       ## 0xe2a972a
	.long	2602027802                      ## 0x9b17cb1a
	.long	2752816856                      ## 0xa414a6d8
	.long	3843100830                      ## 0xe511109e
	.long	3658644828                      ## 0xda127d5c
	.long	1729788946                      ## 0x671a7c12
	.long	1478037968                      ## 0x581911d0
	.long	421308310                       ## 0x191ca796
	.long	639617620                       ## 0x261fca54
	.long	3095241547                      ## 0xb87da34b
	.long	2273234569                      ## 0x877ece89
	.long	3329980623                      ## 0xc67b78cf
	.long	4185396493                      ## 0xf978150d
	.long	1148195907                      ## 0x44701443
	.long	2071165313                      ## 0x7b737981
	.long	980864967                       ## 0x3a76cfc7
	.long	91595269                        ## 0x575a205
	.long	3720551352                      ## 0xddc31bb8
	.long	3804264058                      ## 0xe2c0767a
	.long	2747646012                      ## 0xa3c5c03c
	.long	2630266366                      ## 0x9cc6adfe
	.long	567192752                       ## 0x21ceacb0
	.long	516800882                       ## 0x1ecdc172
	.long	1606973236                      ## 0x5fc87734
	.long	1623923446                      ## 0x60cb1af6
	.long	4272518121                      ## 0xfea973e9
	.long	3249151531                      ## 0xc1aa1e2b
	.long	2158995565                      ## 0x80afa86d
	.long	3215771055                      ## 0xbfacc5af
	.long	44352737                        ## 0x2a4c4e1
	.long	1034397987                      ## 0x3da7a923
	.long	2090999653                      ## 0x7ca21f65
	.long	1134654119                      ## 0x43a172a7
	.long	381577822                       ## 0x16be6a5e
	.long	700254108                       ## 0x29bd079c
	.long	1756934618                      ## 0x68b8b1da
	.long	1471929368                      ## 0x57bbdc18
	.long	3937656150                      ## 0xeab3dd56
	.long	3585126548                      ## 0xd5b0b094
	.long	2494891730                      ## 0x94b506d2
	.long	2880858896                      ## 0xabb66b10
	.long	903086607                       ## 0x35d4020f
	.long	181891021                       ## 0xad76fcd
	.long	1272109451                      ## 0x4bd2d98b
	.long	1959900233                      ## 0x74d1b449
	.long	3386488071                      ## 0xc9d9b507
	.long	4141537477                      ## 0xf6dad8c5
	.long	3084873347                      ## 0xb7df6e83
	.long	2296120129                      ## 0x88dc0341
	.long	1349171964                      ## 0x506abafc
	.long	1869207358                      ## 0x6f69d73e
	.long	778854776                       ## 0x2e6c6178
	.long	292490426                       ## 0x116f0cba
	.long	2892434932                      ## 0xac670df4
	.long	2472828982                      ## 0x93646036
	.long	3529627248                      ## 0xd261d670
	.long	3982670770                      ## 0xed62bbb2
	.long	1929433773                      ## 0x7300d2ad
	.long	1275314031                      ## 0x4c03bf6f
	.long	218499369                       ## 0xd060929
	.long	839214315                       ## 0x320564eb
	.long	2400019877                      ## 0x8f0d65a5
	.long	2953709671                      ## 0xb00e0867
	.long	4044078625                      ## 0xf10bbe21
	.long	3456685027                      ## 0xce08d3e3

.subsections_via_symbols
