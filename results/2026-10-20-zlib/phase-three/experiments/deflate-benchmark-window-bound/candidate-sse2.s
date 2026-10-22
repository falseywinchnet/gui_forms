	.build_version macos, 14, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE ## -- Begin function _ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE
	.p2align	4
__ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE: ## @_ZN8stx_zlib13DeflateStream4failENS_12DeflateErrorE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%esi, 52(%rdi)
	movl	$5, 48(%rdi)
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream6chargeEy ## -- Begin function _ZN8stx_zlib13DeflateStream6chargeEy
	.p2align	4
__ZN8stx_zlib13DeflateStream6chargeEy:  ## @_ZN8stx_zlib13DeflateStream6chargeEy
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	40(%rdi), %rax
	movq	72(%rdi), %rcx
	subq	%rcx, %rax
	cmpq	%rax, %rsi
	ja	LBB1_1
## %bb.2:
	addq	%rsi, %rcx
	movq	%rcx, 72(%rdi)
	cmpq	%rax, %rsi
	setbe	%al
	popq	%rbp
	retq
LBB1_1:
	movabsq	$21474836485, %rcx              ## imm = 0x500000005
	movq	%rcx, 48(%rdi)
	cmpq	%rax, %rsi
	setbe	%al
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE ## -- Begin function _ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE
	.p2align	4
__ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE: ## @_ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	(%rsi), %eax
	movl	4(%rsi), %ecx
	testl	%eax, %eax
	je	LBB2_2
## %bb.1:
	cmpl	$5, %ecx
	setb	%cl
	cmpl	$1, %eax
	sete	%dl
	movl	$1, %eax
	testb	%dl, %cl
	jne	LBB2_3
LBB2_14:
	popq	%rbp
	retq
LBB2_2:
	movl	$1, %eax
	cmpl	$4, %ecx
	ja	LBB2_14
LBB2_3:
	movl	8(%rsi), %ecx
	addl	$-10, %ecx
	cmpl	$-11, %ecx
	jb	LBB2_14
## %bb.4:
	movl	12(%rsi), %ecx
	andl	$-8, %ecx
	cmpl	$8, %ecx
	jne	LBB2_14
## %bb.5:
	movl	16(%rsi), %ecx
	addl	$-10, %ecx
	cmpl	$-9, %ecx
	jb	LBB2_14
## %bb.6:
	cmpq	$-1, 24(%rsi)
	je	LBB2_14
## %bb.7:
	cmpq	$-1, 32(%rsi)
	je	LBB2_14
## %bb.8:
	cmpq	$-1, 40(%rsi)
	je	LBB2_14
## %bb.9:
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	32(%rsi), %xmm2
	movups	%xmm2, 32(%rdi)
	movups	%xmm1, 16(%rdi)
	movups	%xmm0, (%rdi)
	movl	8(%rsi), %eax
	cmpl	$-1, %eax
	movl	$6, %edx
	cmovnel	%eax, %edx
	movl	%edx, 80(%rdi)
	leaq	l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains(%rip), %rax
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 92(%rdi)
	leaq	l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice(%rip), %rax
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 96(%rdi)
	leaq	l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good(%rip), %rax
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 100(%rdi)
	leaq	l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits(%rip), %rax
	movl	(%rax,%rdx,4), %eax
	movl	%eax, 104(%rdi)
	movzbl	16(%rsi), %ecx
	addb	$7, %cl
	movl	$1, %eax
	shll	%cl, %eax
	movl	%eax, 84(%rdi)
	movl	(%rsi), %r8d
	movl	12(%rsi), %eax
	movl	%eax, %ecx
	xorl	$8, %ecx
	orl	%r8d, %ecx
	movl	$9, %ecx
	cmovnel	%eax, %ecx
	movl	$1, %esi
	shlq	%cl, %rsi
	xorl	%eax, %eax
	movq	%rsi, 112(%rdi)
	movl	$1, 48(%rdi)
	xorps	%xmm0, %xmm0
	movups	%xmm0, 52(%rdi)
	movups	%xmm0, 64(%rdi)
	movups	%xmm0, 65656(%rdi)
	movb	$0, 65672(%rdi)
	movups	%xmm0, 804200(%rdi)
	movq	$0, 804216(%rdi)
	movq	$1, 804224(%rdi)
	testl	%r8d, %r8d
	jne	LBB2_14
## %bb.10:
	shll	$12, %ecx
	xorl	%eax, %eax
	movl	$0, %esi
	cmpl	$2, %edx
	jb	LBB2_13
## %bb.11:
	movl	$64, %esi
	cmpl	$6, %edx
	jb	LBB2_13
## %bb.12:
	setne	%dl
	movzbl	%dl, %esi
	orl	$2, %esi
	shll	$6, %esi
LBB2_13:
	addl	$-30720, %ecx                   ## imm = 0x8800
	movl	%esi, %edx
	orl	%ecx, %edx
	imulq	$138547333, %rdx, %r8           ## imm = 0x8421085
	shrq	$32, %r8
	movl	%edx, %r9d
	subl	%r8d, %r9d
	shrl	%r9d
	addl	%r8d, %r9d
	shrl	$4, %r9d
	movl	%r9d, %r8d
	shll	$5, %r8d
	subl	%r8d, %r9d
	addl	%edx, %r9d
	movl	%r9d, %edx
	xorl	$31, %edx
	testl	%r9d, %r9d
	cmovel	%r9d, %edx
	orl	%esi, %edx
	movb	%ch, 738664(%rdi)
	movb	%dl, 738665(%rdi)
	movq	$2, 804200(%rdi)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
LCPI3_0:
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	3                               ## 0x3
LCPI3_1:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI3_2:
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
	.p2align	4
__ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE: ## @_ZN8stx_zlib13DeflateStream5adlerENSt3__14spanIKSt4byteLm18446744073709551615EEE
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
	movq	%rsi, -48(%rbp)                 ## 8-byte Spill
	testq	%rdx, %rdx
	je	LBB3_11
## %bb.1:
	movl	804224(%rdi), %ebx
	movl	804228(%rdi), %r10d
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	addq	$4, %rax
	movq	%rax, -56(%rbp)                 ## 8-byte Spill
	xorl	%ecx, %ecx
	movl	$2147975281, %r9d               ## imm = 0x80078071
	movdqa	LCPI3_0(%rip), %xmm0            ## xmm0 = [0,1,2,3]
	pxor	%xmm1, %xmm1
	movdqa	LCPI3_1(%rip), %xmm2            ## xmm2 = [4,4,4,4]
	movdqa	LCPI3_2(%rip), %xmm3            ## xmm3 = [8,8,8,8]
	jmp	LBB3_2
	.p2align	4
LBB3_10:                                ##   in Loop: Header=BB3_2 Depth=1
	addl	%r12d, %r10d
	addl	%ebx, %r15d
	movl	%ebx, %eax
	imull	%r11d, %eax
	addl	%r10d, %eax
	movq	%r15, %r8
	imulq	%r9, %r8
	shrq	$47, %r8
	imull	$65521, %r8d, %r8d              ## imm = 0xFFF1
	subl	%r8d, %r15d
	movl	%r15d, 804224(%rdi)
	movq	%rax, %r8
	imulq	%r9, %r8
	shrq	$47, %r8
	imull	$65521, %r8d, %r8d              ## imm = 0xFFF1
	subl	%r8d, %eax
	movl	%eax, 804228(%rdi)
	addq	%r11, %rcx
	movl	%eax, %r10d
	movl	%r15d, %ebx
	cmpq	%rdx, %rcx
	jae	LBB3_11
LBB3_2:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB3_6 Depth 2
                                        ##     Child Loop BB3_9 Depth 2
	movq	%rdx, %r11
	subq	%rcx, %r11
	cmpq	$5552, %r11                     ## imm = 0x15B0
	movl	$5552, %eax                     ## imm = 0x15B0
	cmovaeq	%rax, %r11
	movq	%rdx, %rax
	movl	$0, %r12d
	movl	$0, %r15d
	subq	%rcx, %rax
	je	LBB3_10
## %bb.3:                               ##   in Loop: Header=BB3_2 Depth=1
	cmpq	$8, %rax
	jae	LBB3_5
## %bb.4:                               ##   in Loop: Header=BB3_2 Depth=1
	xorl	%r15d, %r15d
	xorl	%r14d, %r14d
	xorl	%r12d, %r12d
	jmp	LBB3_8
	.p2align	4
LBB3_5:                                 ##   in Loop: Header=BB3_2 Depth=1
	movl	%r11d, %r14d
	andl	$8184, %r14d                    ## imm = 0x1FF8
	movd	%r11d, %xmm4
	pshufd	$0, %xmm4, %xmm7                ## xmm7 = xmm4[0,0,0,0]
	movq	-56(%rbp), %rax                 ## 8-byte Reload
	leaq	(%rax,%rcx), %r15
	pxor	%xmm6, %xmm6
	xorl	%r12d, %r12d
	pxor	%xmm8, %xmm8
	pxor	%xmm5, %xmm5
	pxor	%xmm4, %xmm4
	movdqa	%xmm0, %xmm9
	.p2align	4
LBB3_6:                                 ##   Parent Loop BB3_2 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movd	-4(%r15,%r12), %xmm11           ## xmm11 = mem[0],zero,zero,zero
	movd	(%r15,%r12), %xmm10             ## xmm10 = mem[0],zero,zero,zero
	punpcklbw	%xmm1, %xmm11           ## xmm11 = xmm11[0],xmm1[0],xmm11[1],xmm1[1],xmm11[2],xmm1[2],xmm11[3],xmm1[3],xmm11[4],xmm1[4],xmm11[5],xmm1[5],xmm11[6],xmm1[6],xmm11[7],xmm1[7]
	punpcklwd	%xmm1, %xmm11           ## xmm11 = xmm11[0],xmm1[0],xmm11[1],xmm1[1],xmm11[2],xmm1[2],xmm11[3],xmm1[3]
	punpcklbw	%xmm1, %xmm10           ## xmm10 = xmm10[0],xmm1[0],xmm10[1],xmm1[1],xmm10[2],xmm1[2],xmm10[3],xmm1[3],xmm10[4],xmm1[4],xmm10[5],xmm1[5],xmm10[6],xmm1[6],xmm10[7],xmm1[7]
	punpcklwd	%xmm1, %xmm10           ## xmm10 = xmm10[0],xmm1[0],xmm10[1],xmm1[1],xmm10[2],xmm1[2],xmm10[3],xmm1[3]
	paddd	%xmm11, %xmm8
	paddd	%xmm10, %xmm6
	movdqa	%xmm7, %xmm12
	psubd	%xmm9, %xmm12
	movdqa	%xmm12, %xmm13
	psubd	%xmm2, %xmm13
	pshufd	$245, %xmm12, %xmm14            ## xmm14 = xmm12[1,1,3,3]
	pmuludq	%xmm11, %xmm12
	pshufd	$232, %xmm12, %xmm12            ## xmm12 = xmm12[0,2,2,3]
	pshufd	$245, %xmm11, %xmm11            ## xmm11 = xmm11[1,1,3,3]
	pmuludq	%xmm14, %xmm11
	pshufd	$232, %xmm11, %xmm11            ## xmm11 = xmm11[0,2,2,3]
	punpckldq	%xmm11, %xmm12          ## xmm12 = xmm12[0],xmm11[0],xmm12[1],xmm11[1]
	paddd	%xmm12, %xmm5
	pshufd	$245, %xmm13, %xmm11            ## xmm11 = xmm13[1,1,3,3]
	pmuludq	%xmm10, %xmm13
	pshufd	$232, %xmm13, %xmm12            ## xmm12 = xmm13[0,2,2,3]
	pshufd	$245, %xmm10, %xmm10            ## xmm10 = xmm10[1,1,3,3]
	pmuludq	%xmm11, %xmm10
	pshufd	$232, %xmm10, %xmm10            ## xmm10 = xmm10[0,2,2,3]
	punpckldq	%xmm10, %xmm12          ## xmm12 = xmm12[0],xmm10[0],xmm12[1],xmm10[1]
	paddd	%xmm12, %xmm4
	addq	$8, %r12
	paddd	%xmm3, %xmm9
	cmpq	%r12, %r14
	jne	LBB3_6
## %bb.7:                               ##   in Loop: Header=BB3_2 Depth=1
	paddd	%xmm8, %xmm6
	pshufd	$238, %xmm6, %xmm7              ## xmm7 = xmm6[2,3,2,3]
	paddd	%xmm6, %xmm7
	pshufd	$85, %xmm7, %xmm6               ## xmm6 = xmm7[1,1,1,1]
	paddd	%xmm7, %xmm6
	movd	%xmm6, %r15d
	paddd	%xmm5, %xmm4
	pshufd	$238, %xmm4, %xmm5              ## xmm5 = xmm4[2,3,2,3]
	paddd	%xmm4, %xmm5
	pshufd	$85, %xmm5, %xmm4               ## xmm4 = xmm5[1,1,1,1]
	paddd	%xmm5, %xmm4
	movd	%xmm4, %r12d
	cmpq	%r14, %r11
	je	LBB3_10
LBB3_8:                                 ##   in Loop: Header=BB3_2 Depth=1
	movq	%r11, %r13
	subq	%r14, %r13
	movl	%r11d, %eax
	subl	%r14d, %eax
	movq	-48(%rbp), %rsi                 ## 8-byte Reload
	leaq	(%rsi,%rcx), %r8
	addq	%r14, %r8
	xorl	%r14d, %r14d
	.p2align	4
LBB3_9:                                 ##   Parent Loop BB3_2 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movzbl	(%r8,%r14), %esi
	addl	%esi, %r15d
	imull	%eax, %esi
	addl	%esi, %r12d
	incq	%r14
	decl	%eax
	cmpq	%r14, %r13
	jne	LBB3_9
	jmp	LBB3_10
LBB3_11:
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.private_extern	___clang_call_terminate ## -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	4
___clang_call_terminate:                ## @__clang_call_terminate
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	callq	___cxa_begin_catch
	callq	__ZSt9terminatev
	.cfi_endproc
                                        ## -- End function
	.globl	__ZNK8stx_zlib13DeflateStream9prefix_atEm ## -- Begin function _ZNK8stx_zlib13DeflateStream9prefix_atEm
	.p2align	4
__ZNK8stx_zlib13DeflateStream9prefix_atEm: ## @_ZNK8stx_zlib13DeflateStream9prefix_atEm
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	120(%rdi,%rsi), %edx
	movzbl	88(%rdi), %ecx
	movl	%edx, %r8d
	shll	%cl, %r8d
	movzbl	121(%rdi,%rsi), %r9d
	movzbl	122(%rdi,%rsi), %eax
	xorl	%r9d, %r8d
	shll	%cl, %r8d
	xorl	%eax, %r8d
	movl	84(%rdi), %eax
	decl	%eax
	andl	%r8d, %eax
	shll	$8, %r9d
	orl	%edx, %r9d
	shlq	$32, %r9
	orq	%r9, %rax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream6insertEm ## -- Begin function _ZN8stx_zlib13DeflateStream6insertEm
	.p2align	4
__ZN8stx_zlib13DeflateStream6insertEm:  ## @_ZN8stx_zlib13DeflateStream6insertEm
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movzbl	120(%rdi,%rsi), %edx
	movzbl	121(%rdi,%rsi), %eax
	movzbl	88(%rdi), %ecx
	movl	%edx, %r8d
	shll	%cl, %r8d
	xorl	%eax, %r8d
	shll	%cl, %r8d
	movzbl	122(%rdi,%rsi), %ecx
	xorl	%ecx, %r8d
	movl	84(%rdi), %ecx
	decl	%ecx
	andl	%r8d, %ecx
	shll	$24, %eax
	shll	$16, %edx
	movl	65676(%rdi,%rcx,4), %r8d
	movl	%r8d, 327820(%rdi,%rsi,4)
	orl	%edx, %esi
	orl	%eax, %esi
	movl	%esi, 65676(%rdi,%rcx,4)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj
LCPI7_0:
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
LCPI7_1:
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
LCPI7_2:
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj
	.p2align	4
__ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj: ## @_ZN8stx_zlib13DeflateStream12rebase_linksENSt3__14spanIKjLm18446744073709551615EEENS2_IjLm18446744073709551615EEEjj
	.cfi_startproc
## %bb.0:
	testq	%rdx, %rdx
	je	LBB7_12
## %bb.1:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	16(%rbp), %eax
	cmpq	$8, %rdx
	setb	%dil
	movq	%rcx, %r8
	subq	%rsi, %r8
	cmpq	$32, %r8
	setb	%r8b
	orb	%dil, %r8b
	je	LBB7_3
## %bb.2:
	xorl	%edi, %edi
	jmp	LBB7_6
LBB7_3:
	movq	%rdx, %rdi
	andq	$-8, %rdi
	movd	%r9d, %xmm0
	pshufd	$0, %xmm0, %xmm0                ## xmm0 = xmm0[0,0,0,0]
	movd	%eax, %xmm1
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	xorl	%r8d, %r8d
	movdqa	LCPI7_0(%rip), %xmm2            ## xmm2 = [65535,0,65535,0,65535,0,65535,0]
	movdqa	LCPI7_1(%rip), %xmm3            ## xmm3 = [2147483648,2147483648,2147483648,2147483648]
	pxor	%xmm3, %xmm1
	movdqa	LCPI7_2(%rip), %xmm4            ## xmm4 = [0,65535,0,65535,0,65535,0,65535]
	pcmpeqd	%xmm5, %xmm5
	.p2align	4
LBB7_4:                                 ## =>This Inner Loop Header: Depth=1
	movdqu	(%rsi,%r8,4), %xmm9
	movdqu	16(%rsi,%r8,4), %xmm7
	movdqa	%xmm9, %xmm6
	pand	%xmm2, %xmm6
	movdqa	%xmm7, %xmm8
	pand	%xmm2, %xmm8
	movdqa	%xmm6, %xmm10
	psubd	%xmm0, %xmm10
	movdqa	%xmm8, %xmm11
	psubd	%xmm0, %xmm11
	pcmpeqd	%xmm2, %xmm6
	pcmpeqd	%xmm2, %xmm8
	pand	%xmm4, %xmm9
	por	%xmm10, %xmm9
	pxor	%xmm3, %xmm10
	movdqa	%xmm1, %xmm12
	pcmpgtd	%xmm10, %xmm12
	pandn	%xmm12, %xmm6
	pand	%xmm4, %xmm7
	por	%xmm11, %xmm7
	pxor	%xmm3, %xmm11
	movdqa	%xmm1, %xmm10
	pcmpgtd	%xmm11, %xmm10
	pandn	%xmm10, %xmm8
	pand	%xmm6, %xmm9
	pxor	%xmm5, %xmm6
	por	%xmm9, %xmm6
	pand	%xmm8, %xmm7
	pxor	%xmm5, %xmm8
	por	%xmm7, %xmm8
	movdqu	%xmm6, (%rcx,%r8,4)
	movdqu	%xmm8, 16(%rcx,%r8,4)
	addq	$8, %r8
	cmpq	%r8, %rdi
	jne	LBB7_4
## %bb.5:
	cmpq	%rdi, %rdx
	je	LBB7_11
LBB7_6:
	movq	%rdi, %r8
	orq	$1, %r8
	testb	$1, %dl
	je	LBB7_8
## %bb.7:
	movl	(%rsi,%rdi,4), %r10d
	movzwl	%r10w, %r11d
	movl	%r11d, %ebx
	subl	%r9d, %ebx
	andl	$-65536, %r10d                  ## imm = 0xFFFF0000
	orl	%ebx, %r10d
	cmpl	%eax, %ebx
	movl	$-1, %ebx
	cmovael	%ebx, %r10d
	cmpl	$65535, %r11d                   ## imm = 0xFFFF
	cmovel	%ebx, %r10d
	movl	%r10d, (%rcx,%rdi,4)
	movq	%r8, %rdi
LBB7_8:
	cmpq	%r8, %rdx
	je	LBB7_11
## %bb.9:
	movl	$-1, %r8d
	.p2align	4
LBB7_10:                                ## =>This Inner Loop Header: Depth=1
	movl	(%rsi,%rdi,4), %r10d
	movzwl	%r10w, %r11d
	movl	%r11d, %ebx
	subl	%r9d, %ebx
	andl	$-65536, %r10d                  ## imm = 0xFFFF0000
	orl	%ebx, %r10d
	cmpl	%eax, %ebx
	cmovael	%r8d, %r10d
	cmpl	$65535, %r11d                   ## imm = 0xFFFF
	cmovel	%r8d, %r10d
	movl	%r10d, (%rcx,%rdi,4)
	movl	4(%rsi,%rdi,4), %r10d
	movzwl	%r10w, %r11d
	movl	%r11d, %ebx
	subl	%r9d, %ebx
	andl	$-65536, %r10d                  ## imm = 0xFFFF0000
	orl	%ebx, %r10d
	cmpl	%eax, %ebx
	cmovael	%r8d, %r10d
	cmpl	$65535, %r11d                   ## imm = 0xFFFF
	cmovel	%r8d, %r10d
	movl	%r10d, 4(%rcx,%rdi,4)
	addq	$2, %rdi
	cmpq	%rdi, %rdx
	jne	LBB7_10
LBB7_11:
	popq	%rbx
	popq	%rbp
LBB7_12:
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy ## -- Begin function _ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	.p2align	4
__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy: ## @_ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
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
	movl	%r9d, -72(%rbp)                 ## 4-byte Spill
	movl	16(%rbp), %r13d
	xorl	%ebx, %ebx
	movq	%rsi, %rax
	subq	112(%rdi), %rax
	cmovbq	%rbx, %rax
	movq	%rax, -152(%rbp)                ## 8-byte Spill
	movl	%ecx, -68(%rbp)                 ## 4-byte Spill
	testl	%ecx, %ecx
	sete	%al
	movzwl	%r8w, %r11d
	cmpl	$65535, %r11d                   ## imm = 0xFFFF
	sete	%cl
	orb	%al, %cl
	jne	LBB8_1
## %bb.2:
	leaq	120(%rdi), %rax
	movq	%rax, -80(%rbp)                 ## 8-byte Spill
	leaq	-11(%rdx), %rax
	movq	%rax, -136(%rbp)                ## 8-byte Spill
	movl	96(%rdi), %eax
	movq	%rax, -104(%rbp)                ## 8-byte Spill
	leaq	120(%rdi,%rsi), %rax
	movq	%rax, -144(%rbp)                ## 8-byte Spill
	leaq	123(%rdi), %rax
	movq	%rax, -128(%rbp)                ## 8-byte Spill
	leaq	123(%rsi,%rdi), %rax
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	leaq	-3(%rdx), %rax
	movq	%rax, -120(%rbp)                ## 8-byte Spill
	movq	%rdx, -64(%rbp)                 ## 8-byte Spill
	negq	%rdx
	movq	%rdx, -96(%rbp)                 ## 8-byte Spill
	movq	%rsi, -112(%rbp)                ## 8-byte Spill
	leaq	(%rsi,%rdi), %rax
	addq	$120, %rax
	movq	%rax, -88(%rbp)                 ## 8-byte Spill
	xorl	%r10d, %r10d
	xorl	%r15d, %r15d
	xorl	%r12d, %r12d
	xorl	%ebx, %ebx
	.p2align	4
LBB8_3:                                 ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB8_14 Depth 2
                                        ##     Child Loop BB8_22 Depth 2
	movl	%r15d, %esi
	cmpq	%rsi, 24(%rbp)
	je	LBB8_37
## %bb.4:                               ##   in Loop: Header=BB8_3 Depth=1
	leal	1(%r15), %r14d
	movl	%r11d, %eax
	cmpq	%rax, -152(%rbp)                ## 8-byte Folded Reload
	ja	LBB8_5
## %bb.6:                               ##   in Loop: Header=BB8_3 Depth=1
	shrl	$16, %r8d
	cmpl	-72(%rbp), %r8d                 ## 4-byte Folded Reload
	jne	LBB8_7
## %bb.8:                               ##   in Loop: Header=BB8_3 Depth=1
	movl	%r13d, %edx
	testl	%r13d, %r13d
	movq	-80(%rbp), %r8                  ## 8-byte Reload
	je	LBB8_12
## %bb.9:                               ##   in Loop: Header=BB8_3 Depth=1
	leal	(%r11,%r13), %ecx
	movq	%rdx, %r10
	movzbl	(%r8,%rcx), %edx
	movq	-144(%rbp), %r9                 ## 8-byte Reload
	cmpb	(%r9,%r10), %dl
	jne	LBB8_10
## %bb.11:                              ##   in Loop: Header=BB8_3 Depth=1
	decl	%ecx
	movzbl	(%r8,%rcx), %ecx
	cmpb	-1(%r9,%r10), %cl
	movq	%r10, %rdx
	movl	$0, %r10d
	jne	LBB8_7
LBB8_12:                                ##   in Loop: Header=BB8_3 Depth=1
	movl	$3, %r9d
	cmpq	$-9, -136(%rbp)                 ## 8-byte Folded Reload
	ja	LBB8_19
## %bb.13:                              ##   in Loop: Header=BB8_3 Depth=1
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	movq	-128(%rbp), %rcx                ## 8-byte Reload
	leaq	(%rcx,%rax), %r9
	movq	-120(%rbp), %rcx                ## 8-byte Reload
	xorl	%edx, %edx
	.p2align	4
LBB8_14:                                ##   Parent Loop BB8_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	leal	(%rsi,%rdx), %r8d
	incl	%r8d
	movq	24(%rbp), %r10
	subq	%r8, %r10
	cmpq	$8, %r10
	jb	LBB8_36
## %bb.15:                              ##   in Loop: Header=BB8_14 Depth=2
	movq	(%r9,%rdx), %r8
	movq	-160(%rbp), %r10                ## 8-byte Reload
	movq	(%r10,%rdx), %r10
	cmpq	%r10, %r8
	jne	LBB8_16
## %bb.17:                              ##   in Loop: Header=BB8_14 Depth=2
	addq	$8, %rdx
	addq	$-8, %rcx
	cmpq	$7, %rcx
	ja	LBB8_14
## %bb.18:                              ##   in Loop: Header=BB8_3 Depth=1
	movl	%edx, %ecx
	addq	$3, %rdx
	leal	(%r15,%rcx), %r14d
	incl	%r14d
	movq	%rdx, %r9
	xorl	%r10d, %r10d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
LBB8_19:                                ##   in Loop: Header=BB8_3 Depth=1
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	cmpq	%rcx, %r9
	jae	LBB8_20
## %bb.21:                              ##   in Loop: Header=BB8_3 Depth=1
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	movq	%rbx, -56(%rbp)                 ## 8-byte Spill
	leal	(%r14,%rcx), %r15d
	subl	%r9d, %r15d
	leaq	(%r9,%rax), %rdx
	addq	-80(%rbp), %rdx                 ## 8-byte Folded Reload
	movq	-96(%rbp), %rcx                 ## 8-byte Reload
	addq	%r9, %rcx
	movq	-88(%rbp), %rsi                 ## 8-byte Reload
	leaq	(%rsi,%r9), %r8
	movl	%r14d, %r10d
	xorl	%esi, %esi
	.p2align	4
LBB8_22:                                ##   Parent Loop BB8_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	leal	(%r10,%rsi), %ebx
	cmpq	%rbx, 24(%rbp)
	je	LBB8_35
## %bb.23:                              ##   in Loop: Header=BB8_22 Depth=2
	movzbl	(%rdx,%rsi), %ebx
	cmpb	(%r8,%rsi), %bl
	jne	LBB8_26
## %bb.24:                              ##   in Loop: Header=BB8_22 Depth=2
	incq	%rsi
	movq	%rcx, %rbx
	addq	%rsi, %rbx
	jne	LBB8_22
## %bb.25:                              ##   in Loop: Header=BB8_3 Depth=1
	movq	-64(%rbp), %r9                  ## 8-byte Reload
	jmp	LBB8_27
	.p2align	4
LBB8_7:                                 ##   in Loop: Header=BB8_3 Depth=1
	movl	%r13d, %r9d
	movl	%r14d, %r15d
	jmp	LBB8_33
LBB8_10:                                ##   in Loop: Header=BB8_3 Depth=1
	movl	%r13d, %r9d
	movl	%r14d, %r15d
	xorl	%r10d, %r10d
	jmp	LBB8_33
LBB8_20:                                ##   in Loop: Header=BB8_3 Depth=1
	movl	%r14d, %r15d
	jmp	LBB8_29
LBB8_16:                                ##   in Loop: Header=BB8_3 Depth=1
	xorq	%r8, %r10
	rep		bsfq	%r10, %rcx
	shrl	$3, %ecx
	leaq	(%rcx,%rdx), %r9
	addq	$3, %r9
	addl	%edx, %r15d
	addl	$9, %r15d
	jmp	LBB8_28
LBB8_26:                                ##   in Loop: Header=BB8_3 Depth=1
	addq	%rsi, %r9
	leal	(%r14,%rsi), %r15d
	incl	%r15d
LBB8_27:                                ##   in Loop: Header=BB8_3 Depth=1
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
LBB8_28:                                ##   in Loop: Header=BB8_3 Depth=1
	xorl	%r10d, %r10d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
LBB8_29:                                ##   in Loop: Header=BB8_3 Depth=1
	cmpq	$3, %r9
	setae	%cl
	cmpq	%rdx, %r9
	seta	%dl
	andb	%cl, %dl
	cmpb	$1, %dl
	jne	LBB8_30
## %bb.31:                              ##   in Loop: Header=BB8_3 Depth=1
	movq	-112(%rbp), %rbx                ## 8-byte Reload
                                        ## kill: def $ebx killed $ebx killed $rbx def $rbx
	subl	%r11d, %ebx
	cmpq	-64(%rbp), %r9                  ## 8-byte Folded Reload
	je	LBB8_38
## %bb.32:                              ##   in Loop: Header=BB8_3 Depth=1
	cmpq	-104(%rbp), %r9                 ## 8-byte Folded Reload
	jb	LBB8_33
	jmp	LBB8_38
LBB8_30:                                ##   in Loop: Header=BB8_3 Depth=1
	movl	%r13d, %r9d
	.p2align	4
LBB8_33:                                ##   in Loop: Header=BB8_3 Depth=1
	incl	%r12d
	cmpl	-68(%rbp), %r12d                ## 4-byte Folded Reload
	jae	LBB8_38
## %bb.34:                              ##   in Loop: Header=BB8_3 Depth=1
	movl	327820(%rdi,%rax,4), %r8d
	movzwl	%r8w, %r11d
	movl	%r9d, %r13d
	cmpl	$65535, %r11d                   ## imm = 0xFFFF
	jne	LBB8_3
	jmp	LBB8_38
LBB8_1:
	movl	%r13d, %r9d
	xorl	%r15d, %r15d
	xorl	%r10d, %r10d
	jmp	LBB8_38
LBB8_36:
	addl	%edx, %r15d
	incl	%r15d
LBB8_37:
	movl	%r13d, %r9d
	movabsq	$4294967296, %r10               ## imm = 0x100000000
	jmp	LBB8_38
LBB8_5:
	movl	%r13d, %r9d
	movl	%r14d, %r15d
	jmp	LBB8_38
LBB8_35:
	addl	%esi, %r14d
	movl	%r13d, %r9d
	movl	%r14d, %r15d
	movabsq	$4294967296, %r10               ## imm = 0x100000000
	movq	-56(%rbp), %rbx                 ## 8-byte Reload
LBB8_38:
	shlq	$32, %rbx
	movl	%r9d, %eax
	orq	%rbx, %rax
	movl	%r15d, %edx
	orq	%r10, %rdx
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream12append_tokenEjjRm ## -- Begin function _ZN8stx_zlib13DeflateStream12append_tokenEjjRm
	.p2align	4
__ZN8stx_zlib13DeflateStream12append_tokenEjjRm: ## @_ZN8stx_zlib13DeflateStream12append_tokenEjjRm
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
                                        ## kill: def $edx killed $edx def $rdx
	movq	(%rcx), %rax
	movw	%si, 589964(%rdi,%rax,4)
	movw	%dx, 589966(%rdi,%rax,4)
	incq	%rax
	movq	%rax, (%rcx)
	movl	%esi, %eax
	testl	%edx, %edx
	je	LBB9_1
## %bb.2:
	leaq	__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE(%rip), %rcx
	movl	(%rcx,%rax,4), %eax
	leal	-1(%rdx), %ecx
	cmpl	$5, %edx
	jb	LBB9_4
## %bb.3:
	bsrl	%ecx, %edx
	xorl	$31, %edx
	movl	$30, %esi
	subl	%edx, %esi
	addl	%edx, %edx
	xorl	%r8d, %r8d
	btl	%esi, %ecx
	adcl	$62, %r8d
	subl	%edx, %r8d
	movl	%r8d, %ecx
LBB9_4:
	leal	257(%rax), %edx
	incl	721048(%rdi,%rdx,4)
	movl	%ecx, %edx
	incl	722192(%rdi,%rdx,4)
	leaq	__ZN8stx_zlib12_GLOBAL__N_112length_extraE(%rip), %rdx
	movl	(%rdx,%rax,4), %eax
	movl	%ecx, %edx
	shrl	%edx
	decl	%edx
	xorl	%esi, %esi
	cmpl	$4, %ecx
	cmovael	%edx, %esi
	addq	%rax, %rsi
	addq	%rsi, 727128(%rdi)
	popq	%rbp
	retq
LBB9_1:
	incl	721048(%rdi,%rax,4)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream8tokenizeEv ## -- Begin function _ZN8stx_zlib13DeflateStream8tokenizeEv
	.p2align	4
__ZN8stx_zlib13DeflateStream8tokenizeEv: ## @_ZN8stx_zlib13DeflateStream8tokenizeEv
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
	subq	$200, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdi, %rbx
	leaq	721040(%rdi), %r14
	movq	$0, 727128(%rdi)
	movl	$1272, %esi                     ## imm = 0x4F8
	movq	%r14, %rdi
	callq	___bzero
	movl	$1, 722072(%rbx)
	movq	65664(%rbx), %rax
	movq	40(%rbx), %r15
	movq	72(%rbx), %r13
	movq	%r15, %rcx
	subq	%r13, %rcx
	cmpq	%rcx, %rax
	jbe	LBB10_2
LBB10_1:
	movabsq	$21474836485, %rax              ## imm = 0x500000005
	movq	%rax, 48(%rbx)
	xorl	%eax, %eax
	jmp	LBB10_156
LBB10_2:
	movq	65656(%rbx), %r12
	leaq	(%rax,%r12), %r11
	addq	%rax, %r13
	movq	%r13, 72(%rbx)
	movl	4(%rbx), %edi
	cmpl	$2, %edi
	je	LBB10_18
## %bb.3:
	cmpl	$3, %edi
	jne	LBB10_21
## %bb.4:
	xorl	%eax, %eax
	cmpq	%r11, %r12
	jae	LBB10_146
## %bb.5:
	movl	$258, %ecx                      ## imm = 0x102
	xorl	%r8d, %r8d
	.p2align	4
LBB10_6:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB10_9 Depth 2
	movq	%r11, %rsi
	subq	%r12, %rsi
	cmpq	$258, %rsi                      ## imm = 0x102
	cmovaeq	%rcx, %rsi
	testq	%r12, %r12
	je	LBB10_16
## %bb.7:                               ##   in Loop: Header=BB10_6 Depth=1
	cmpq	%r12, %r11
	je	LBB10_16
## %bb.8:                               ##   in Loop: Header=BB10_6 Depth=1
	movq	%r11, %rdx
	leaq	120(%rbx), %rcx
	leaq	(%rcx,%r12), %r11
	movq	%r13, %r9
	movq	%r15, %r13
	leaq	1(%r9), %r15
	movq	%r13, %rdi
	movq	%r9, %rcx
	subq	%r9, %r13
	xorl	%r10d, %r10d
	.p2align	4
LBB10_9:                                ##   Parent Loop BB10_6 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cmpq	%r10, %r13
	je	LBB10_1
## %bb.10:                              ##   in Loop: Header=BB10_9 Depth=2
	movq	%r10, %r9
	addq	%r15, %r10
	movq	%r10, 72(%rbx)
	movzbl	(%r11,%r9), %r10d
	cmpb	-1(%r11), %r10b
	jne	LBB10_12
## %bb.11:                              ##   in Loop: Header=BB10_9 Depth=2
	leaq	1(%r9), %r10
	cmpq	%r10, %rsi
	jne	LBB10_9
	jmp	LBB10_13
LBB10_12:                               ##   in Loop: Header=BB10_6 Depth=1
	movq	%r9, %rsi
LBB10_13:                               ##   in Loop: Header=BB10_6 Depth=1
	leaq	(%rcx,%r9), %r13
	incq	%r13
	cmpq	$3, %rsi
	movq	%rdi, %r15
	movq	%rdx, %r11
	jb	LBB10_15
## %bb.14:                              ##   in Loop: Header=BB10_6 Depth=1
	movw	%si, 589964(%rbx,%rax,4)
	movw	$1, 589966(%rbx,%rax,4)
	movl	%esi, %r9d
	leaq	__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE(%rip), %rcx
	movl	(%rcx,%r9,4), %r9d
	leal	257(%r9), %r10d
	incl	721048(%rbx,%r10,4)
	incl	722192(%rbx)
	leaq	__ZN8stx_zlib12_GLOBAL__N_112length_extraE(%rip), %rcx
	movl	(%rcx,%r9,4), %r9d
	addq	%r9, %r8
	movq	%r8, 727128(%rbx)
	movl	$258, %ecx                      ## imm = 0x102
	jmp	LBB10_17
LBB10_15:                               ##   in Loop: Header=BB10_6 Depth=1
	movl	$258, %ecx                      ## imm = 0x102
LBB10_16:                               ##   in Loop: Header=BB10_6 Depth=1
	movzbl	120(%rbx,%r12), %esi
	movw	%si, 589964(%rbx,%rax,4)
	movw	$0, 589966(%rbx,%rax,4)
	incl	721048(%rbx,%rsi,4)
	movl	$1, %esi
LBB10_17:                               ##   in Loop: Header=BB10_6 Depth=1
	incq	%rax
	addq	%rsi, %r12
	cmpq	%r11, %r12
	jb	LBB10_6
	jmp	LBB10_146
LBB10_18:
	cmpq	%r11, %r12
	jae	LBB10_141
## %bb.19:
	cmpq	$1, %rax
	jne	LBB10_142
## %bb.20:
	xorl	%ecx, %ecx
	jmp	LBB10_145
LBB10_21:
	xorl	%r14d, %r14d
	movq	%r11, %rcx
	subq	$2, %rcx
	cmovaeq	%rcx, %r14
	cmpq	$2, %r12
	movl	$2, %ecx
	cmovbq	%r12, %rcx
	addq	%rcx, %rax
	movq	%r15, %rdx
	subq	%r13, %rdx
	cmpq	%rdx, %rax
	ja	LBB10_1
## %bb.22:
	addq	%rax, %r13
	movq	%r13, 72(%rbx)
	movq	%r12, %rax
	subq	%rcx, %rax
	movl	16(%rbx), %edx
	addl	$7, %edx
	leal	(%r11,%r11), %ecx
	cmpl	$257, %ecx                      ## imm = 0x101
	movl	$256, %esi                      ## imm = 0x100
	cmovael	%ecx, %esi
	decl	%esi
	bsrl	%esi, %ecx
	incl	%ecx
	cmpl	%edx, %ecx
	cmovael	%edx, %ecx
	movl	$1, %edx
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edx
	cmpq	$3, %r11
	movl	%edi, -44(%rbp)                 ## 4-byte Spill
	movq	%r14, -96(%rbp)                 ## 8-byte Spill
	jb	LBB10_27
## %bb.23:
	cmpb	$1, 65672(%rbx)
	jne	LBB10_25
## %bb.24:
	cmpl	84(%rbx), %edx
	jbe	LBB10_27
LBB10_25:
	movl	%edx, 84(%rbx)
	leal	-1(%rdx), %eax
	bsrl	%eax, %ecx
	xorl	$31, %ecx
	movb	$34, %sil
	subb	%cl, %sil
	movzbl	%sil, %ecx
	imull	$86, %ecx, %ecx
	shrl	$8, %ecx
	testl	%eax, %eax
	cmovel	%eax, %ecx
	movl	%ecx, 88(%rbx)
	movl	%edx, %ecx
	addq	%r12, %rcx
	movq	%r15, %rdx
	subq	%r13, %rdx
	cmpq	%rdx, %rcx
	ja	LBB10_1
## %bb.26:
	addq	%rcx, %r13
	movq	%r13, 72(%rbx)
	leaq	65676(%rbx), %rdi
	movl	%eax, %eax
	leaq	4(,%rax,4), %rdx
	movl	$255, %esi
	movq	%r11, %r14
	callq	_memset
	movq	%r14, %r11
	movq	-96(%rbp), %r14                 ## 8-byte Reload
	movb	$1, 65672(%rbx)
	xorl	%eax, %eax
LBB10_27:
	cmpq	%r12, %r14
	movq	%r12, %rdx
	cmovbq	%r14, %rdx
	cmpq	%rdx, %rax
	jae	LBB10_30
## %bb.28:
	movl	84(%rbx), %esi
	movl	88(%rbx), %ecx
	decl	%esi
	.p2align	4
LBB10_29:                               ## =>This Inner Loop Header: Depth=1
	movzbl	120(%rbx,%rax), %edi
	movzbl	121(%rbx,%rax), %r8d
	movzbl	122(%rbx,%rax), %r9d
	movl	%edi, %r10d
	shll	%cl, %r10d
	xorl	%r8d, %r10d
	shll	%cl, %r10d
	xorl	%r9d, %r10d
	andl	%esi, %r10d
	shll	$24, %r8d
	shll	$16, %edi
	movl	65676(%rbx,%r10,4), %r9d
	movl	%r9d, 327820(%rbx,%rax,4)
	movl	%eax, %r9d
	orl	%edi, %r9d
	orl	%r8d, %r9d
	movl	%r9d, 65676(%rbx,%r10,4)
	incq	%rax
	cmpq	%rax, %rdx
	jne	LBB10_29
LBB10_30:
	subq	%r13, %r15
	cmpq	%r14, %r12
	movq	%r15, -128(%rbp)                ## 8-byte Spill
	movq	%r13, -152(%rbp)                ## 8-byte Spill
	jae	LBB10_147
## %bb.31:
	movl	80(%rbx), %eax
	movl	%eax, -120(%rbp)                ## 4-byte Spill
	movq	727128(%rbx), %rax
	movq	%rax, -208(%rbp)                ## 8-byte Spill
	leaq	120(%rbx), %rsi
	movl	84(%rbx), %eax
	movl	88(%rbx), %ecx
	movl	%ecx, -76(%rbp)                 ## 4-byte Spill
	decl	%eax
	movl	%eax, -72(%rbp)                 ## 4-byte Spill
	movl	100(%rbx), %eax
	movl	92(%rbx), %ecx
	movl	%ecx, %edx
	shrl	$2, %edx
	movl	%eax, -176(%rbp)                ## 4-byte Spill
	testl	%eax, %eax
	movl	%ecx, -172(%rbp)                ## 4-byte Spill
	movl	%ecx, %eax
	movl	%edx, -168(%rbp)                ## 4-byte Spill
	cmovel	%edx, %eax
	movq	%rax, -224(%rbp)                ## 8-byte Spill
	movq	112(%rbx), %rax
	movq	%rax, -192(%rbp)                ## 8-byte Spill
	movl	96(%rbx), %eax
	movq	%rax, -200(%rbp)                ## 8-byte Spill
	movq	%r12, %r10
	movl	104(%rbx), %eax
	movl	%eax, -116(%rbp)                ## 4-byte Spill
	movq	%r15, -64(%rbp)                 ## 8-byte Spill
	xorl	%r15d, %r15d
	xorl	%r13d, %r13d
	movl	-44(%rbp), %edi                 ## 4-byte Reload
	movq	%r11, -184(%rbp)                ## 8-byte Spill
	jmp	LBB10_35
	.p2align	4
LBB10_32:                               ##   in Loop: Header=BB10_35 Depth=1
	xorl	%r15d, %r15d
LBB10_33:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	-144(%rbp), %rax                ## 8-byte Reload
	movzbl	(%rax), %eax
	movw	%ax, 589964(%rbx,%r13,4)
	movw	$0, 589966(%rbx,%r13,4)
	incl	721048(%rbx,%rax,4)
	movl	$1, %r12d
LBB10_34:                               ##   in Loop: Header=BB10_35 Depth=1
	addq	%r10, %r12
	incq	%r13
	movq	%r12, %r10
	cmpq	%r14, %r12
	jae	LBB10_148
LBB10_35:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB10_45 Depth 2
                                        ##       Child Loop BB10_52 Depth 3
                                        ##       Child Loop BB10_57 Depth 3
                                        ##     Child Loop BB10_93 Depth 2
                                        ##       Child Loop BB10_100 Depth 3
                                        ##       Child Loop BB10_105 Depth 3
                                        ##     Child Loop BB10_138 Depth 2
	leaq	(%rsi,%r10), %rax
	movq	%rax, -144(%rbp)                ## 8-byte Spill
	movzbl	120(%rbx,%r10), %eax
	movzbl	121(%rbx,%r10), %r9d
	movl	%eax, %edx
	movl	-76(%rbp), %ecx                 ## 4-byte Reload
	shll	%cl, %edx
	xorl	%r9d, %edx
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edx
	movzbl	122(%rbx,%r10), %ecx
	xorl	%ecx, %edx
	andl	-72(%rbp), %edx                 ## 4-byte Folded Reload
	shll	$8, %r9d
	orl	%eax, %r9d
	movl	65676(%rbx,%rdx,4), %r8d
	movl	%r8d, 327820(%rbx,%r10,4)
	movl	%r9d, %eax
	shll	$16, %eax
	orl	%r10d, %eax
	movl	%eax, 65676(%rbx,%rdx,4)
	testl	%r15d, %r15d
	movq	%r10, -56(%rbp)                 ## 8-byte Spill
	je	LBB10_37
## %bb.36:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%r15, %rcx
	cmpl	$1, %edi
	je	LBB10_77
	jmp	LBB10_78
	.p2align	4
LBB10_37:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	%r11, %rax
	subq	%r10, %rax
	cmpq	$258, %rax                      ## imm = 0x102
	movl	$258, %r10d                     ## imm = 0x102
	cmovbq	%rax, %r10
	xorl	%r15d, %r15d
	cmpq	$3, %rax
	jb	LBB10_42
## %bb.38:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%r13, -136(%rbp)                ## 8-byte Spill
	movzwl	%r8w, %r13d
	cmpl	$65535, %r13d                   ## imm = 0xFFFF
	je	LBB10_41
## %bb.39:                              ##   in Loop: Header=BB10_35 Depth=1
	leaq	9(%r10), %rax
	movq	-224(%rbp), %rcx                ## 8-byte Reload
	imulq	%rcx, %rax
	movq	-64(%rbp), %r15                 ## 8-byte Reload
	cmpq	%rax, %r15
	jae	LBB10_43
## %bb.40:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%rbx, %rdi
	movq	-56(%rbp), %rsi                 ## 8-byte Reload
	movq	%r10, %rdx
                                        ## kill: def $ecx killed $ecx killed $rcx
	pushq	%r15
	pushq	$0
	movq	%r11, %r14
	callq	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	movl	-44(%rbp), %edi                 ## 4-byte Reload
	movq	%r14, %r11
	movq	-96(%rbp), %r14                 ## 8-byte Reload
	addq	$16, %rsp
	movq	%r15, %rsi
	movq	%rax, %r15
	movq	%rdx, %rax
	jmp	LBB10_74
LBB10_41:                               ##   in Loop: Header=BB10_35 Depth=1
	xorl	%ecx, %ecx
	movq	-136(%rbp), %r13                ## 8-byte Reload
	jmp	LBB10_76
LBB10_42:                               ##   in Loop: Header=BB10_35 Depth=1
	xorl	%ecx, %ecx
	jmp	LBB10_76
LBB10_43:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	subq	-192(%rbp), %rdx                ## 8-byte Folded Reload
	movl	$0, %eax
	cmovbq	%rax, %rdx
	movq	%rdx, -112(%rbp)                ## 8-byte Spill
	movl	$0, %eax
	movl	$0, %r15d
	testq	%rcx, %rcx
	je	LBB10_73
## %bb.44:                              ##   in Loop: Header=BB10_35 Depth=1
	leaq	123(%rbx), %rax
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	addq	%rcx, %rax
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	xorl	%r12d, %r12d
	movq	$0, -104(%rbp)                  ## 8-byte Folded Spill
	xorl	%r11d, %r11d
	xorl	%esi, %esi
	movq	-112(%rbp), %r14                ## 8-byte Reload
	.p2align	4
LBB10_45:                               ##   Parent Loop BB10_35 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB10_52 Depth 3
                                        ##       Child Loop BB10_57 Depth 3
	leal	1(%r12), %r15d
	movl	%r13d, %r13d
	cmpq	%r13, %r14
	ja	LBB10_71
## %bb.46:                              ##   in Loop: Header=BB10_45 Depth=2
	shrl	$16, %r8d
	cmpl	%r9d, %r8d
	jne	LBB10_60
## %bb.47:                              ##   in Loop: Header=BB10_45 Depth=2
	movl	%esi, %r8d
	testl	%esi, %esi
	je	LBB10_50
## %bb.48:                              ##   in Loop: Header=BB10_45 Depth=2
	leal	(%rsi,%r13), %eax
	leaq	120(%rbx), %rdx
	movzbl	(%rdx,%rax), %ecx
	movq	-144(%rbp), %r14                ## 8-byte Reload
	cmpb	(%r14,%r8), %cl
	jne	LBB10_61
## %bb.49:                              ##   in Loop: Header=BB10_45 Depth=2
	decl	%eax
	movzbl	(%rdx,%rax), %eax
	cmpb	-1(%r14,%r8), %al
	movq	-112(%rbp), %r14                ## 8-byte Reload
	jne	LBB10_60
LBB10_50:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	$3, %edx
	leaq	-11(%r10), %rax
	cmpq	$-9, %rax
	ja	LBB10_55
## %bb.51:                              ##   in Loop: Header=BB10_45 Depth=2
	leaq	123(%rbx), %rax
	addq	%r13, %rax
	leaq	-3(%r10), %rdi
	xorl	%edx, %edx
	.p2align	4
LBB10_52:                               ##   Parent Loop BB10_35 Depth=1
                                        ##     Parent Loop BB10_45 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	(%rax,%rdx), %r15
	movq	-160(%rbp), %rcx                ## 8-byte Reload
	movq	(%rcx,%rdx), %rcx
	cmpq	%rcx, %r15
	jne	LBB10_63
## %bb.53:                              ##   in Loop: Header=BB10_52 Depth=3
	addq	$8, %rdx
	addq	$-8, %rdi
	cmpq	$7, %rdi
	ja	LBB10_52
## %bb.54:                              ##   in Loop: Header=BB10_45 Depth=2
	leal	(%r12,%rdx), %r15d
	incl	%r15d
	addq	$3, %rdx
	movl	-44(%rbp), %edi                 ## 4-byte Reload
LBB10_55:                               ##   in Loop: Header=BB10_45 Depth=2
	cmpq	%r10, %rdx
	jae	LBB10_62
## %bb.56:                              ##   in Loop: Header=BB10_45 Depth=2
	leaq	120(%rbx), %rax
	leaq	(%rax,%r13), %rdi
	leal	(%r15,%r10), %eax
	subl	%edx, %eax
	incl	%r15d
	movl	%r15d, %r12d
	movq	-144(%rbp), %r15                ## 8-byte Reload
	.p2align	4
LBB10_57:                               ##   Parent Loop BB10_35 Depth=1
                                        ##     Parent Loop BB10_45 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%rdi,%rdx), %ecx
	cmpb	(%r15,%rdx), %cl
	jne	LBB10_64
## %bb.58:                              ##   in Loop: Header=BB10_57 Depth=3
	incq	%rdx
	incl	%r12d
	cmpq	%rdx, %r10
	jne	LBB10_57
## %bb.59:                              ##   in Loop: Header=BB10_45 Depth=2
	movq	%r10, %rdx
	movl	%eax, %r12d
	jmp	LBB10_64
	.p2align	4
LBB10_60:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	%esi, %edx
	movl	%r15d, %r12d
	jmp	LBB10_69
LBB10_61:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	%esi, %edx
	movl	%r15d, %r12d
	movq	-112(%rbp), %r14                ## 8-byte Reload
	jmp	LBB10_69
LBB10_62:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	%r15d, %r12d
	jmp	LBB10_65
LBB10_63:                               ##   in Loop: Header=BB10_45 Depth=2
	xorq	%r15, %rcx
	rep		bsfq	%rcx, %rax
	shrl	$3, %eax
	addl	%edx, %r12d
	addl	$9, %r12d
	addq	%rax, %rdx
	addq	$3, %rdx
LBB10_64:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	-44(%rbp), %edi                 ## 4-byte Reload
LBB10_65:                               ##   in Loop: Header=BB10_45 Depth=2
	cmpq	$3, %rdx
	setae	%al
	cmpq	%r8, %rdx
	seta	%cl
	andb	%al, %cl
	cmpb	$1, %cl
	jne	LBB10_68
## %bb.66:                              ##   in Loop: Header=BB10_45 Depth=2
	movq	-56(%rbp), %rax                 ## 8-byte Reload
                                        ## kill: def $eax killed $eax killed $rax def $rax
	subl	%r13d, %eax
	movq	%rax, -104(%rbp)                ## 8-byte Spill
	cmpq	%r10, %rdx
	je	LBB10_72
## %bb.67:                              ##   in Loop: Header=BB10_45 Depth=2
	cmpq	-200(%rbp), %rdx                ## 8-byte Folded Reload
	jb	LBB10_69
	jmp	LBB10_72
LBB10_68:                               ##   in Loop: Header=BB10_45 Depth=2
	movl	%esi, %edx
	.p2align	4
LBB10_69:                               ##   in Loop: Header=BB10_45 Depth=2
	incl	%r11d
	cmpl	-224(%rbp), %r11d               ## 4-byte Folded Reload
	jae	LBB10_72
## %bb.70:                              ##   in Loop: Header=BB10_45 Depth=2
	movl	327820(%rbx,%r13,4), %r8d
	movzwl	%r8w, %r13d
	movl	%edx, %esi
	cmpl	$65535, %r13d                   ## imm = 0xFFFF
	jne	LBB10_45
	jmp	LBB10_72
LBB10_71:                               ##   in Loop: Header=BB10_35 Depth=1
	movl	%esi, %edx
	movl	%r15d, %r12d
LBB10_72:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	-104(%rbp), %rax                ## 8-byte Reload
	shlq	$32, %rax
	movl	%edx, %r15d
	orq	%rax, %r15
	movl	%r12d, %eax
	movq	-184(%rbp), %r11                ## 8-byte Reload
	movq	-96(%rbp), %r14                 ## 8-byte Reload
LBB10_73:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
LBB10_74:                               ##   in Loop: Header=BB10_35 Depth=1
	movl	%eax, %ecx
	subq	%rcx, %rsi
	movq	%rsi, -64(%rbp)                 ## 8-byte Spill
	movabsq	$4294967296, %rcx               ## imm = 0x100000000
	testq	%rcx, %rax
	movq	-136(%rbp), %r13                ## 8-byte Reload
	jne	LBB10_157
## %bb.75:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%r15, %rcx
	leaq	120(%rbx), %rsi
LBB10_76:                               ##   in Loop: Header=BB10_35 Depth=1
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	cmpl	$1, %edi
	jne	LBB10_78
LBB10_77:                               ##   in Loop: Header=BB10_35 Depth=1
	cmpl	$6, %r15d
	jb	LBB10_80
LBB10_78:                               ##   in Loop: Header=BB10_35 Depth=1
	shrq	$32, %rcx
	cmpl	$3, %r15d
	jne	LBB10_81
## %bb.79:                              ##   in Loop: Header=BB10_35 Depth=1
	cmpl	$4097, %ecx                     ## imm = 0x1001
	jb	LBB10_81
LBB10_80:                               ##   in Loop: Header=BB10_35 Depth=1
	xorl	%r15d, %r15d
	xorl	%ecx, %ecx
LBB10_81:                               ##   in Loop: Header=BB10_35 Depth=1
	cmpl	$4, -120(%rbp)                  ## 4-byte Folded Reload
	jb	LBB10_128
## %bb.82:                              ##   in Loop: Header=BB10_35 Depth=1
	cmpl	$3, %r15d
	jb	LBB10_128
## %bb.83:                              ##   in Loop: Header=BB10_35 Depth=1
	cmpl	-116(%rbp), %r15d               ## 4-byte Folded Reload
	jae	LBB10_128
## %bb.84:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%r11, %rax
	subq	%r10, %rax
	cmpq	$4, %rax
	jb	LBB10_128
## %bb.85:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%rcx, -240(%rbp)                ## 8-byte Spill
	movzbl	121(%rbx,%r10), %edx
	movzbl	122(%rbx,%r10), %r9d
	movl	%edx, %esi
	movl	-76(%rbp), %ecx                 ## 4-byte Reload
	shll	%cl, %esi
	xorl	%r9d, %esi
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %esi
	leaq	1(%r10), %rax
	movq	%r11, %r12
	movq	%rax, -104(%rbp)                ## 8-byte Spill
	subq	%rax, %r12
	cmpq	$258, %r12                      ## imm = 0x102
	movl	$258, %eax                      ## imm = 0x102
	cmovaeq	%rax, %r12
	movl	%r15d, %eax
	cmpq	%rax, %r12
	jbe	LBB10_89
## %bb.86:                              ##   in Loop: Header=BB10_35 Depth=1
	movzbl	123(%rbx,%r10), %ecx
	xorl	%ecx, %esi
	andl	-72(%rbp), %esi                 ## 4-byte Folded Reload
	movl	65676(%rbx,%rsi,4), %r8d
	movzwl	%r8w, %esi
	cmpl	$65535, %esi                    ## imm = 0xFFFF
	je	LBB10_89
## %bb.87:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%r13, -136(%rbp)                ## 8-byte Spill
	shll	$8, %r9d
	orl	%edx, %r9d
	cmpl	-176(%rbp), %r15d               ## 4-byte Folded Reload
	movl	-168(%rbp), %ecx                ## 4-byte Reload
	movl	%ecx, %r14d
	cmovbl	-172(%rbp), %r14d               ## 4-byte Folded Reload
	leaq	9(%r12), %rcx
	imulq	%r14, %rcx
	movq	-64(%rbp), %r13                 ## 8-byte Reload
	cmpq	%rcx, %r13
	jae	LBB10_91
## %bb.88:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	%rbx, %rdi
	movq	-104(%rbp), %rsi                ## 8-byte Reload
	movq	%r12, %rdx
	movl	%r14d, %ecx
	pushq	%r13
	pushq	%r15
	movq	%r11, %r14
	callq	__ZNK8stx_zlib13DeflateStream14search_checkedEmmjjjjy
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	movq	%r14, %r11
	movq	-96(%rbp), %r14                 ## 8-byte Reload
	addq	$16, %rsp
	movq	%rdx, %rdi
	jmp	LBB10_125
	.p2align	4
LBB10_89:                               ##   in Loop: Header=BB10_35 Depth=1
	xorl	%ecx, %ecx
	movl	%r15d, %eax
	leaq	120(%rbx), %rsi
	cmpl	%eax, %r15d
	jae	LBB10_127
LBB10_90:                               ##   in Loop: Header=BB10_35 Depth=1
	movl	%eax, %eax
	orq	%rax, %rcx
	movq	%rcx, %r15
	jmp	LBB10_33
LBB10_91:                               ##   in Loop: Header=BB10_35 Depth=1
	leaq	1(%r10), %rdx
	subq	-192(%rbp), %rdx                ## 8-byte Folded Reload
	movl	$0, %ecx
	cmovbq	%rcx, %rdx
	movq	%rdx, -112(%rbp)                ## 8-byte Spill
	movl	$0, %edi
	testl	%r14d, %r14d
	je	LBB10_124
## %bb.92:                              ##   in Loop: Header=BB10_35 Depth=1
	leaq	120(%rbx), %rax
	addq	%r10, %rax
	incq	%rax
	movq	%rax, -216(%rbp)                ## 8-byte Spill
	leaq	124(%rbx), %rax
	addq	%r10, %rax
	movq	%rax, -160(%rbp)                ## 8-byte Spill
	xorl	%edi, %edi
	xorl	%eax, %eax
	xorl	%r11d, %r11d
	movl	%r15d, %r13d
	movq	%r14, -88(%rbp)                 ## 8-byte Spill
	movl	%r9d, -68(%rbp)                 ## 4-byte Spill
	.p2align	4
LBB10_93:                               ##   Parent Loop BB10_35 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB10_100 Depth 3
                                        ##       Child Loop BB10_105 Depth 3
	leal	1(%rdi), %r10d
	movl	%esi, %esi
	cmpq	%rsi, -112(%rbp)                ## 8-byte Folded Reload
	ja	LBB10_122
## %bb.94:                              ##   in Loop: Header=BB10_93 Depth=2
	shrl	$16, %r8d
	cmpl	%r9d, %r8d
	jne	LBB10_109
## %bb.95:                              ##   in Loop: Header=BB10_93 Depth=2
	movq	%r15, %r9
	movl	%r13d, %r15d
	testl	%r13d, %r13d
	je	LBB10_98
## %bb.96:                              ##   in Loop: Header=BB10_93 Depth=2
	leal	(%rsi,%r13), %edx
	leaq	120(%rbx), %r8
	movzbl	(%r8,%rdx), %ecx
	movq	-216(%rbp), %r14                ## 8-byte Reload
	cmpb	(%r14,%r15), %cl
	jne	LBB10_110
## %bb.97:                              ##   in Loop: Header=BB10_93 Depth=2
	decl	%edx
	movzbl	(%r8,%rdx), %ecx
	movq	-144(%rbp), %rdx                ## 8-byte Reload
	cmpb	(%rdx,%r15), %cl
	movq	-88(%rbp), %r14                 ## 8-byte Reload
	jne	LBB10_115
LBB10_98:                               ##   in Loop: Header=BB10_93 Depth=2
	movq	%r15, -232(%rbp)                ## 8-byte Spill
	movl	$3, %edx
	leaq	-11(%r12), %rcx
	cmpq	$-9, %rcx
	ja	LBB10_103
## %bb.99:                              ##   in Loop: Header=BB10_93 Depth=2
	leaq	123(%rbx), %rcx
	leaq	(%rcx,%rsi), %r8
	leaq	-3(%r12), %r10
	xorl	%edx, %edx
	movq	%r9, %r15
	.p2align	4
LBB10_100:                              ##   Parent Loop BB10_35 Depth=1
                                        ##     Parent Loop BB10_93 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movq	(%r8,%rdx), %rcx
	movq	-160(%rbp), %r14                ## 8-byte Reload
	movq	(%r14,%rdx), %r14
	cmpq	%r14, %rcx
	jne	LBB10_117
## %bb.101:                             ##   in Loop: Header=BB10_100 Depth=3
	addq	$8, %rdx
	addq	$-8, %r10
	cmpq	$7, %r10
	ja	LBB10_100
## %bb.102:                             ##   in Loop: Header=BB10_93 Depth=2
	leal	(%rdi,%rdx), %r10d
	incl	%r10d
	addq	$3, %rdx
	movq	-88(%rbp), %r14                 ## 8-byte Reload
LBB10_103:                              ##   in Loop: Header=BB10_93 Depth=2
	cmpq	%r12, %rdx
	jae	LBB10_111
## %bb.104:                             ##   in Loop: Header=BB10_93 Depth=2
	leaq	120(%rbx), %rcx
	leaq	(%rcx,%rsi), %r8
	leal	(%r10,%r12), %ecx
	subl	%edx, %ecx
	movl	%ecx, -164(%rbp)                ## 4-byte Spill
	incl	%r10d
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	movq	-216(%rbp), %r14                ## 8-byte Reload
	movq	%r9, %r15
	movl	-68(%rbp), %r9d                 ## 4-byte Reload
	.p2align	4
LBB10_105:                              ##   Parent Loop BB10_35 Depth=1
                                        ##     Parent Loop BB10_93 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r8,%rdx), %ecx
	cmpb	(%r14,%rdx), %cl
	jne	LBB10_108
## %bb.106:                             ##   in Loop: Header=BB10_105 Depth=3
	incq	%rdx
	incl	%edi
	cmpq	%rdx, %r12
	jne	LBB10_105
## %bb.107:                             ##   in Loop: Header=BB10_93 Depth=2
	movq	%r12, %rdx
	movl	-164(%rbp), %ecx                ## 4-byte Reload
	movl	%ecx, %edi
LBB10_108:                              ##   in Loop: Header=BB10_93 Depth=2
	movq	-88(%rbp), %r14                 ## 8-byte Reload
	cmpq	$3, %rdx
	jae	LBB10_112
	jmp	LBB10_119
	.p2align	4
LBB10_109:                              ##   in Loop: Header=BB10_93 Depth=2
	movl	%r13d, %edx
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	jmp	LBB10_120
LBB10_110:                              ##   in Loop: Header=BB10_93 Depth=2
	movl	%r13d, %edx
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	movq	-88(%rbp), %r14                 ## 8-byte Reload
	jmp	LBB10_116
LBB10_111:                              ##   in Loop: Header=BB10_93 Depth=2
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	movq	%r9, %r15
	movl	-68(%rbp), %r9d                 ## 4-byte Reload
	cmpq	$3, %rdx
	jae	LBB10_112
	jmp	LBB10_119
LBB10_115:                              ##   in Loop: Header=BB10_93 Depth=2
	movl	%r13d, %edx
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
LBB10_116:                              ##   in Loop: Header=BB10_93 Depth=2
	movq	%r9, %r15
	movl	-68(%rbp), %r9d                 ## 4-byte Reload
	jmp	LBB10_120
LBB10_117:                              ##   in Loop: Header=BB10_93 Depth=2
	xorq	%rcx, %r14
	rep		bsfq	%r14, %rcx
	shrl	$3, %ecx
	addl	%edx, %edi
	addl	$9, %edi
	leaq	3(%rcx,%rdx), %rdx
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	movq	-88(%rbp), %r14                 ## 8-byte Reload
	movl	-68(%rbp), %r9d                 ## 4-byte Reload
	cmpq	$3, %rdx
	jb	LBB10_119
LBB10_112:                              ##   in Loop: Header=BB10_93 Depth=2
	cmpq	-232(%rbp), %rdx                ## 8-byte Folded Reload
	jbe	LBB10_119
## %bb.113:                             ##   in Loop: Header=BB10_93 Depth=2
	movq	-104(%rbp), %rax                ## 8-byte Reload
                                        ## kill: def $eax killed $eax killed $rax def $rax
	subl	%esi, %eax
	cmpq	%r12, %rdx
	je	LBB10_123
## %bb.114:                             ##   in Loop: Header=BB10_93 Depth=2
	cmpq	-200(%rbp), %rdx                ## 8-byte Folded Reload
	jb	LBB10_120
	jmp	LBB10_123
LBB10_119:                              ##   in Loop: Header=BB10_93 Depth=2
	movl	%r13d, %edx
	.p2align	4
LBB10_120:                              ##   in Loop: Header=BB10_93 Depth=2
	incl	%r11d
	cmpl	%r14d, %r11d
	jae	LBB10_123
## %bb.121:                             ##   in Loop: Header=BB10_93 Depth=2
	movl	327820(%rbx,%rsi,4), %r8d
	movzwl	%r8w, %esi
	movl	%edx, %r13d
	cmpl	$65535, %esi                    ## imm = 0xFFFF
	jne	LBB10_93
	jmp	LBB10_123
LBB10_122:                              ##   in Loop: Header=BB10_35 Depth=1
	movl	%r13d, %edx
	movl	%r10d, %edi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
LBB10_123:                              ##   in Loop: Header=BB10_35 Depth=1
	movl	%edx, %ecx
	shlq	$32, %rax
	orq	%rcx, %rax
	movl	%edi, %edi
	movq	-184(%rbp), %r11                ## 8-byte Reload
LBB10_124:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	-64(%rbp), %r13                 ## 8-byte Reload
	movq	-96(%rbp), %r14                 ## 8-byte Reload
LBB10_125:                              ##   in Loop: Header=BB10_35 Depth=1
	movl	%edi, %ecx
	subq	%rcx, %r13
	movq	%r13, -64(%rbp)                 ## 8-byte Spill
	movabsq	$4294967296, %rcx               ## imm = 0x100000000
	testq	%rcx, %rdi
	leaq	120(%rbx), %rsi
	jne	LBB10_157
## %bb.126:                             ##   in Loop: Header=BB10_35 Depth=1
	movq	%rax, %rcx
	movabsq	$-4294967296, %rdx              ## imm = 0xFFFFFFFF00000000
	andq	%rdx, %rcx
	movl	-44(%rbp), %edi                 ## 4-byte Reload
	movq	-136(%rbp), %r13                ## 8-byte Reload
	cmpl	%eax, %r15d
	jb	LBB10_90
LBB10_127:                              ##   in Loop: Header=BB10_35 Depth=1
	movq	-240(%rbp), %rcx                ## 8-byte Reload
LBB10_128:                              ##   in Loop: Header=BB10_35 Depth=1
	cmpl	$2, %r15d
	jbe	LBB10_32
## %bb.129:                             ##   in Loop: Header=BB10_35 Depth=1
	movw	%r15w, 589964(%rbx,%r13,4)
	movw	%cx, 589966(%rbx,%r13,4)
	movl	%r15d, %r12d
	testl	%ecx, %ecx
	je	LBB10_133
## %bb.130:                             ##   in Loop: Header=BB10_35 Depth=1
	leal	-1(%rcx), %eax
	cmpl	$5, %ecx
	jb	LBB10_132
## %bb.131:                             ##   in Loop: Header=BB10_35 Depth=1
	bsrl	%eax, %ecx
	xorl	$31, %ecx
	movl	$30, %edx
	subl	%ecx, %edx
	addl	%ecx, %ecx
	btl	%edx, %eax
	movl	$0, %eax
	adcl	$62, %eax
	subl	%ecx, %eax
LBB10_132:                              ##   in Loop: Header=BB10_35 Depth=1
	leaq	__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE(%rip), %rcx
	movl	(%rcx,%r12,4), %ecx
	leal	257(%rcx), %edx
	incl	721048(%rbx,%rdx,4)
	movl	%eax, %edx
	incl	722192(%rbx,%rdx,4)
	leaq	__ZN8stx_zlib12_GLOBAL__N_112length_extraE(%rip), %rdx
	movl	(%rdx,%rcx,4), %ecx
	movl	%eax, %edx
	shrl	%edx
	decl	%edx
	cmpl	$4, %eax
	movl	$0, %eax
	cmovbl	%eax, %edx
	addq	-208(%rbp), %rdx                ## 8-byte Folded Reload
	addq	%rcx, %rdx
	movq	%rdx, 727128(%rbx)
	movq	%rdx, -208(%rbp)                ## 8-byte Spill
	cmpl	$3, -120(%rbp)                  ## 4-byte Folded Reload
	jbe	LBB10_134
	jmp	LBB10_136
LBB10_133:                              ##   in Loop: Header=BB10_35 Depth=1
	incl	721048(%rbx,%r12,4)
	cmpl	$3, -120(%rbp)                  ## 4-byte Folded Reload
	ja	LBB10_136
LBB10_134:                              ##   in Loop: Header=BB10_35 Depth=1
	cmpl	-116(%rbp), %r15d               ## 4-byte Folded Reload
	ja	LBB10_135
LBB10_136:                              ##   in Loop: Header=BB10_35 Depth=1
	leaq	(%r10,%r12), %rax
	cmpq	%rax, %r14
	cmovbq	%r14, %rax
	leaq	1(%r10), %rcx
	cmpq	%rax, %rcx
	jae	LBB10_135
## %bb.137:                             ##   in Loop: Header=BB10_35 Depth=1
	decq	%rax
	movq	%r10, %rdx
	movl	-72(%rbp), %r10d                ## 4-byte Reload
	movl	-76(%rbp), %ecx                 ## 4-byte Reload
	.p2align	4
LBB10_138:                              ##   Parent Loop BB10_35 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movzbl	121(%rbx,%rdx), %edi
	movzbl	122(%rbx,%rdx), %esi
	movzbl	123(%rbx,%rdx), %r8d
	movl	%edi, %r9d
	shll	%cl, %r9d
	xorl	%esi, %r9d
	shll	%cl, %r9d
	xorl	%r8d, %r9d
	andl	%r10d, %r9d
	shll	$24, %esi
	shll	$16, %edi
	movl	65676(%rbx,%r9,4), %r8d
	movl	%r8d, 327824(%rbx,%rdx,4)
	incq	%rdx
	movl	%edx, %r8d
	orl	%edi, %r8d
	orl	%esi, %r8d
	movl	%r8d, 65676(%rbx,%r9,4)
	cmpq	%rdx, %rax
	jne	LBB10_138
## %bb.139:                             ##   in Loop: Header=BB10_35 Depth=1
	xorl	%r15d, %r15d
	movl	-44(%rbp), %edi                 ## 4-byte Reload
	leaq	120(%rbx), %rsi
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	jmp	LBB10_34
	.p2align	4
LBB10_135:                              ##   in Loop: Header=BB10_35 Depth=1
	xorl	%r15d, %r15d
	jmp	LBB10_34
LBB10_141:
	xorl	%eax, %eax
	movq	%rax, (%r14)
	jmp	LBB10_155
LBB10_142:
	movq	%rax, %rdx
	andq	$-2, %rdx
	xorl	%ecx, %ecx
	.p2align	4
LBB10_143:                              ## =>This Inner Loop Header: Depth=1
	movzbl	120(%rbx,%r12), %esi
	movw	%si, 589964(%rbx,%rcx,4)
	movw	$0, 589966(%rbx,%rcx,4)
	incl	721048(%rbx,%rsi,4)
	movzbl	121(%rbx,%r12), %esi
	movw	%si, 589968(%rbx,%rcx,4)
	movw	$0, 589970(%rbx,%rcx,4)
	incl	721048(%rbx,%rsi,4)
	addq	$2, %rcx
	addq	$2, %r12
	cmpq	%rcx, %rdx
	jne	LBB10_143
## %bb.144:
	testb	$1, %al
	je	LBB10_146
LBB10_145:
	movzbl	120(%rbx,%r12), %edx
	movw	%dx, 589964(%rbx,%rcx,4)
	movw	$0, 589966(%rbx,%rcx,4)
	incl	721048(%rbx,%rdx,4)
LBB10_146:
	movq	%rax, (%r14)
	jmp	LBB10_155
LBB10_147:
	xorl	%r13d, %r13d
	movq	%r15, -64(%rbp)                 ## 8-byte Spill
LBB10_148:
	cmpq	%r11, %r12
	jae	LBB10_153
## %bb.149:
	movl	%r11d, %ecx
	subl	%r12d, %ecx
	leaq	1(%r12), %rax
	testb	$1, %cl
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
	je	LBB10_151
## %bb.150:
	movzbl	120(%rbx,%r12), %ecx
	movw	%cx, 589964(%rbx,%r13,4)
	movw	$0, 589966(%rbx,%r13,4)
	incq	%r13
	incl	721048(%rbx,%rcx,4)
	movq	%rax, %r12
LBB10_151:
	cmpq	%rax, %r11
	je	LBB10_154
	.p2align	4
LBB10_152:                              ## =>This Inner Loop Header: Depth=1
	movzbl	120(%rbx,%r12), %eax
	movw	%ax, 589964(%rbx,%r13,4)
	movw	$0, 589966(%rbx,%r13,4)
	incl	721048(%rbx,%rax,4)
	movzbl	121(%rbx,%r12), %eax
	movw	%ax, 589968(%rbx,%r13,4)
	movw	$0, 589970(%rbx,%r13,4)
	incl	721048(%rbx,%rax,4)
	addq	$2, %r13
	addq	$2, %r12
	cmpq	%r11, %r12
	jb	LBB10_152
	jmp	LBB10_154
LBB10_153:
	movq	-128(%rbp), %rdx                ## 8-byte Reload
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
LBB10_154:
	movq	%r13, 721040(%rbx)
	subq	%rsi, %rdx
	addq	-152(%rbp), %rdx                ## 8-byte Folded Reload
	movq	%rdx, 72(%rbx)
LBB10_155:
	movb	$1, %al
LBB10_156:
                                        ## kill: def $al killed $al killed $eax
	addq	$200, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB10_157:
	movq	-128(%rbp), %rax                ## 8-byte Reload
	subq	-64(%rbp), %rax                 ## 8-byte Folded Reload
	addq	-152(%rbp), %rax                ## 8-byte Folded Reload
	movq	%rax, 72(%rbx)
	jmp	LBB10_1
	.cfi_endproc
                                        ## -- End function
	.globl	__ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_ ## -- Begin function _ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_
	.p2align	4
__ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_: ## @_ZNK8stx_zlib13DeflateStream11WeightOrderclERKNS0_14WeightedSymbolES4_
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	(%rsi), %ecx
	movl	4(%rsi), %eax
	xorl	%esi, %esi
	cmpl	4(%rdx), %eax
	setb	%sil
	xorl	%eax, %eax
	cmpl	(%rdx), %ecx
	setb	%al
	cmovel	%esi, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj ## -- Begin function _ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	.p2align	4
__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj: ## @_ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
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
	subq	$168, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rcx, %rbx
	movq	%rsi, %r14
	movl	%edx, %eax
	shll	$6, %eax
	addl	$512, %eax                      ## imm = 0x200
	movq	40(%rdi), %rsi
	movq	72(%rdi), %rcx
	subq	%rcx, %rsi
	cmpq	%rsi, %rax
	jbe	LBB12_4
## %bb.1:
	movabsq	$21474836485, %rax              ## imm = 0x500000005
	movq	%rax, 48(%rdi)
LBB12_2:
	xorl	%eax, %eax
	jmp	LBB12_3
LBB12_4:
	movl	%r9d, %r12d
	leaq	16(%rbp), %r15
	addq	%rax, %rcx
	movq	%rcx, 72(%rdi)
	testq	%r8, %r8
	movq	%rdx, -64(%rbp)                 ## 8-byte Spill
	movq	%rdi, -56(%rbp)                 ## 8-byte Spill
	jle	LBB12_6
## %bb.5:
	movq	%rbx, %rdi
	movq	%r8, %rsi
	callq	___bzero
	movq	-56(%rbp), %rdi                 ## 8-byte Reload
	movq	-64(%rbp), %rdx                 ## 8-byte Reload
LBB12_6:
	movq	(%r15), %r13
	movq	8(%r15), %rsi
	testq	%rsi, %rsi
	jle	LBB12_8
## %bb.7:
	addq	%rsi, %rsi
	movq	%r13, %rdi
	callq	___bzero
	movq	-56(%rbp), %rdi                 ## 8-byte Reload
	movq	-64(%rbp), %rdx                 ## 8-byte Reload
LBB12_8:
	leaq	736360(%rdi), %r15
	testl	%edx, %edx
	movq	%r13, -80(%rbp)                 ## 8-byte Spill
	je	LBB12_96
## %bb.9:
	cmpl	$1, %edx
	jne	LBB12_24
## %bb.10:
	xorl	%eax, %eax
	xorl	%r13d, %r13d
	jmp	LBB12_12
LBB12_24:
	movl	%edx, %ecx
	andl	$-2, %ecx
	xorl	%eax, %eax
	xorl	%r13d, %r13d
	jmp	LBB12_25
	.p2align	4
LBB12_29:                               ##   in Loop: Header=BB12_25 Depth=1
	addq	$2, %rax
	cmpq	%rax, %rcx
	je	LBB12_11
LBB12_25:                               ## =>This Inner Loop Header: Depth=1
	movl	(%r14,%rax,4), %r8d
	testl	%r8d, %r8d
	je	LBB12_27
## %bb.26:                              ##   in Loop: Header=BB12_25 Depth=1
	movl	%r13d, %esi
	movl	%r8d, (%r15,%rsi,8)
	movl	%eax, 4(%r15,%rsi,8)
	incl	%r13d
LBB12_27:                               ##   in Loop: Header=BB12_25 Depth=1
	movl	4(%r14,%rax,4), %r8d
	testl	%r8d, %r8d
	je	LBB12_29
## %bb.28:                              ##   in Loop: Header=BB12_25 Depth=1
	movl	%r13d, %esi
	movl	%r8d, (%r15,%rsi,8)
	leal	1(%rax), %r8d
	movl	%r8d, 4(%r15,%rsi,8)
	incl	%r13d
	jmp	LBB12_29
LBB12_11:
	testb	$1, %dl
	je	LBB12_14
LBB12_12:
	movl	(%r14,%rax,4), %ecx
	testl	%ecx, %ecx
	je	LBB12_14
## %bb.13:
	movl	%r13d, %esi
	movl	%ecx, (%r15,%rsi,8)
	movl	%eax, 4(%r15,%rsi,8)
	incl	%r13d
LBB12_14:
	cmpl	$1, %r13d
	ja	LBB12_20
## %bb.15:
	movl	%edx, %eax
	xorl	%ecx, %ecx
	.p2align	4
LBB12_16:                               ## =>This Inner Loop Header: Depth=1
	cmpl	$0, (%r14,%rcx,4)
	jne	LBB12_18
## %bb.17:                              ##   in Loop: Header=BB12_16 Depth=1
	movl	%r13d, %edx
	movl	$1, 736360(%rdi,%rdx,8)
	movl	%ecx, 736364(%rdi,%rdx,8)
	incl	%r13d
LBB12_18:                               ##   in Loop: Header=BB12_16 Depth=1
	cmpl	$1, %r13d
	ja	LBB12_20
## %bb.19:                              ##   in Loop: Header=BB12_16 Depth=1
	incq	%rcx
	cmpq	%rax, %rcx
	jb	LBB12_16
LBB12_20:
	leaq	736360(%rdi), %r15
	testl	%r13d, %r13d
	je	LBB12_96
## %bb.21:
	movl	%r13d, %r14d
	leaq	(%r15,%r14,8), %rsi
	bsrq	%r14, %rcx
	xorl	$63, %ecx
	addl	%ecx, %ecx
	xorq	$126, %rcx
Ltmp0:                                  ## EH_LABEL
	leaq	-144(%rbp), %rdx
	movq	%r15, %rdi
	movl	$1, %r8d
	callq	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
Ltmp1:                                  ## EH_LABEL
## %bb.22:
	movabsq	$-4294967296, %rax              ## imm = 0xFFFFFFFF00000000
	cmpl	$1, %r13d
	movq	-56(%rbp), %r8                  ## 8-byte Reload
	jne	LBB12_39
## %bb.23:
	xorl	%ecx, %ecx
	jmp	LBB12_31
LBB12_96:
	movb	$1, %al
	movl	%eax, -44(%rbp)                 ## 4-byte Spill
	xorl	%r13d, %r13d
	jmp	LBB12_33
LBB12_39:
	movl	%r14d, %edx
	andl	$-2, %edx
	leaq	727172(%r8), %rsi
	xorl	%ecx, %ecx
	.p2align	4
LBB12_40:                               ## =>This Inner Loop Header: Depth=1
	movl	(%r15,%rcx,8), %edi
	movq	%rdi, -28(%rsi)
	movq	%rax, -20(%rsi)
	movl	8(%r15,%rcx,8), %edi
	movq	%rdi, -12(%rsi)
	movq	%rax, -4(%rsi)
	addq	$2, %rcx
	addq	$32, %rsi
	cmpq	%rcx, %rdx
	jne	LBB12_40
## %bb.30:
	testb	$1, %r14b
	je	LBB12_32
LBB12_31:
	leaq	727144(%r8), %rdx
	movl	(%r15,%rcx,8), %esi
	shlq	$4, %rcx
	movq	%rsi, (%rdx,%rcx)
	movq	%rax, 8(%rdx,%rcx)
LBB12_32:
	movl	$0, -44(%rbp)                   ## 4-byte Folded Spill
LBB12_33:
	leal	(,%r13,2), %eax
	movl	%eax, -68(%rbp)                 ## 4-byte Spill
	leal	-1(,%r13,2), %eax
	cmpl	%eax, %r13d
	jae	LBB12_48
## %bb.34:
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	leaq	727144(%rcx), %rdx
	movl	%r13d, %esi
	movl	%eax, %edi
	movq	%rsi, %rax
	shlq	$4, %rax
	leaq	(%rax,%rcx), %r8
	addq	$727156, %r8                    ## imm = 0xB1874
	xorl	%r10d, %r10d
	movabsq	$-4294967296, %r9               ## imm = 0xFFFFFFFF00000000
	movl	%r13d, %r11d
	jmp	LBB12_35
	.p2align	4
LBB12_45:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r11d, %eax
	leal	1(%r11), %ecx
	movl	%ecx, %r11d
LBB12_47:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r14d, %ecx
	shlq	$4, %rcx
	movl	%eax, %eax
	shlq	$4, %rax
	movq	(%rdx,%rax), %r14
	addq	(%rdx,%rcx), %r14
	movq	%r14, -12(%r8)
	movq	%r9, -4(%r8)
	movl	%esi, 12(%rdx,%rcx)
	movl	%esi, 12(%rdx,%rax)
	incq	%rsi
	addq	$16, %r8
	cmpq	%rsi, %rdi
	je	LBB12_48
LBB12_35:                               ## =>This Inner Loop Header: Depth=1
	cmpl	%r13d, %r10d
	jae	LBB12_41
## %bb.36:                              ##   in Loop: Header=BB12_35 Depth=1
	movl	%r11d, %eax
	cmpq	%rax, %rsi
	je	LBB12_38
## %bb.37:                              ##   in Loop: Header=BB12_35 Depth=1
	movl	%r10d, %r14d
	shlq	$4, %r14
	movq	(%rdx,%r14), %r14
	shlq	$4, %rax
	cmpq	(%rdx,%rax), %r14
	jbe	LBB12_38
	.p2align	4
LBB12_41:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r11d, %r14d
	leal	1(%r11), %eax
	movl	%eax, %r11d
	cmpl	%r13d, %r10d
	jae	LBB12_45
	jmp	LBB12_43
	.p2align	4
LBB12_38:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r10d, %r14d
	leal	1(%r10), %eax
	movl	%eax, %r10d
	cmpl	%r13d, %r10d
	jae	LBB12_45
LBB12_43:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r11d, %eax
	cmpq	%rax, %rsi
	je	LBB12_46
## %bb.44:                              ##   in Loop: Header=BB12_35 Depth=1
	movl	%r10d, %ecx
	shlq	$4, %rcx
	movq	(%rdx,%rcx), %rcx
	shlq	$4, %rax
	cmpq	(%rdx,%rax), %rcx
	ja	LBB12_45
LBB12_46:                               ##   in Loop: Header=BB12_35 Depth=1
	movl	%r10d, %eax
	leal	1(%r10), %ecx
	movl	%ecx, %r10d
	jmp	LBB12_47
LBB12_48:
	movl	-68(%rbp), %eax                 ## 4-byte Reload
	addl	$-2, %eax
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	je	LBB12_51
## %bb.49:
	leaq	727144(%r10), %rdx
	movl	%eax, %ecx
	movq	%rcx, %rax
	shlq	$4, %rax
	leaq	(%rax,%r10), %rsi
	addq	$727140, %rsi                   ## imm = 0xB1864
	.p2align	4
LBB12_50:                               ## =>This Inner Loop Header: Depth=1
	movl	-16(%rsi), %eax
	movl	(%rsi), %edi
	shlq	$4, %rdi
	movl	8(%rdx,%rdi), %edi
	incl	%edi
	movl	%edi, -4(%rsi)
	shlq	$4, %rax
	movl	8(%rdx,%rax), %eax
	incl	%eax
	movl	%eax, -20(%rsi)
	addq	$-32, %rsi
	addq	$-2, %rcx
	jne	LBB12_50
LBB12_51:
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -96(%rbp)
	movaps	%xmm0, -112(%rbp)
	movaps	%xmm0, -128(%rbp)
	movaps	%xmm0, -144(%rbp)
	cmpb	$0, -44(%rbp)                   ## 1-byte Folded Reload
	movq	-64(%rbp), %r11                 ## 8-byte Reload
	jne	LBB12_66
## %bb.52:
	cmpl	$1, %r13d
	jne	LBB12_63
## %bb.53:
	xorl	%edx, %edx
	xorl	%eax, %eax
	jmp	LBB12_55
LBB12_63:
	movl	%r13d, %esi
	movl	%esi, %edi
	andl	$-2, %edi
	leaq	727168(%r10), %r8
	xorl	%edx, %edx
	xorl	%eax, %eax
	.p2align	4
LBB12_64:                               ## =>This Inner Loop Header: Depth=1
	movl	-16(%r8), %r9d
	cmpl	%r9d, %r12d
	cmovbl	%r12d, %r9d
	incl	-144(%rbp,%r9,4)
	movl	%r12d, %ecx
	subl	%r9d, %ecx
	movl	$1, %r9d
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r9d
	addl	%eax, %r9d
	movl	(%r8), %eax
	cmpl	%eax, %r12d
	cmovbl	%r12d, %eax
	incl	-144(%rbp,%rax,4)
	movl	%r12d, %ecx
	subl	%eax, %ecx
	movl	$1, %eax
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %eax
	addl	%r9d, %eax
	addq	$2, %rdx
	addq	$32, %r8
	cmpq	%rdx, %rdi
	jne	LBB12_64
## %bb.54:
	testb	$1, %sil
	je	LBB12_56
LBB12_55:
	shlq	$4, %rdx
	movl	727152(%r10,%rdx), %edx
	cmpl	%edx, %r12d
	cmovbl	%r12d, %edx
	movl	%eax, %esi
	incl	-144(%rbp,%rdx,4)
	movl	%r12d, %ecx
	subl	%edx, %ecx
	movl	$1, %eax
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %eax
	addl	%esi, %eax
LBB12_56:
	movl	$1, %edx
	movl	%r12d, %ecx
	shll	%cl, %edx
	cmpl	%edx, %eax
	jbe	LBB12_66
## %bb.57:
	movl	%r12d, %ecx
	leal	1(%r12), %esi
	leal	-1(%r12), %edi
	leaq	-140(,%rdi,4), %rdi
	addq	%rbp, %rdi
LBB12_58:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB12_59 Depth 2
	movq	%rdi, %r9
	movl	%esi, %r8d
	.p2align	4
LBB12_59:                               ##   Parent Loop BB12_58 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cmpl	$2, %r8d
	je	LBB12_62
## %bb.60:                              ##   in Loop: Header=BB12_59 Depth=2
	movl	-4(%r9), %r10d
	addq	$-4, %r9
	decl	%r8d
	testl	%r10d, %r10d
	je	LBB12_59
## %bb.61:                              ##   in Loop: Header=BB12_58 Depth=1
	cmpl	$0, -144(%rbp,%rcx,4)
	je	LBB12_62
## %bb.65:                              ##   in Loop: Header=BB12_58 Depth=1
	decl	%r10d
	movl	%r10d, (%r9)
	movl	%r8d, %r8d
	addl	$2, -144(%rbp,%r8,4)
	decl	-144(%rbp,%rcx,4)
	decl	%eax
	cmpl	%edx, %eax
	ja	LBB12_58
LBB12_66:
	testl	%r12d, %r12d
	je	LBB12_67
## %bb.68:
	movl	%r12d, %eax
	xorl	%ecx, %ecx
	movq	-80(%rbp), %r14                 ## 8-byte Reload
	jmp	LBB12_69
	.p2align	4
LBB12_75:                               ##   in Loop: Header=BB12_69 Depth=1
	decq	%rax
	testl	%eax, %eax
	je	LBB12_76
LBB12_69:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB12_72 Depth 2
                                        ##     Child Loop BB12_74 Depth 2
	movl	-144(%rbp,%rax,4), %esi
	testl	%esi, %esi
	je	LBB12_75
## %bb.70:                              ##   in Loop: Header=BB12_69 Depth=1
	movl	%esi, %edi
	movl	%ecx, %edx
	andl	$3, %edi
	je	LBB12_73
## %bb.71:                              ##   in Loop: Header=BB12_69 Depth=1
	movl	%ecx, %edx
	.p2align	4
LBB12_72:                               ##   Parent Loop BB12_69 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	%edx, %r8d
	movl	4(%r15,%r8,8), %r8d
	movb	%al, (%rbx,%r8)
	incl	%edx
	decl	%edi
	jne	LBB12_72
LBB12_73:                               ##   in Loop: Header=BB12_69 Depth=1
	addl	%esi, %ecx
	cmpl	$4, %esi
	jb	LBB12_75
	.p2align	4
LBB12_74:                               ##   Parent Loop BB12_69 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	%edx, %esi
	movl	4(%r15,%rsi,8), %esi
	movb	%al, (%rbx,%rsi)
	leal	1(%rdx), %esi
	movl	4(%r15,%rsi,8), %esi
	movb	%al, (%rbx,%rsi)
	leal	2(%rdx), %esi
	movl	4(%r15,%rsi,8), %esi
	movb	%al, (%rbx,%rsi)
	leal	3(%rdx), %esi
	movl	4(%r15,%rsi,8), %esi
	movb	%al, (%rbx,%rsi)
	addl	$4, %edx
	cmpl	%edx, %ecx
	jne	LBB12_74
	jmp	LBB12_75
LBB12_76:
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -160(%rbp)
	movaps	%xmm0, -176(%rbp)
	movaps	%xmm0, -192(%rbp)
	movaps	%xmm0, -208(%rbp)
	incl	%r12d
	cmpl	$3, %r12d
	movl	$2, %ecx
	cmovael	%r12d, %ecx
	leaq	-1(%rcx), %rsi
	movl	%esi, %eax
	andl	$3, %eax
	addl	$-2, %ecx
	cmpl	$3, %ecx
	jae	LBB12_93
## %bb.77:
	xorl	%ecx, %ecx
	movl	$1, %edx
	jmp	LBB12_80
LBB12_67:
	xorps	%xmm0, %xmm0
	movaps	%xmm0, -160(%rbp)
	movaps	%xmm0, -176(%rbp)
	movaps	%xmm0, -192(%rbp)
	movaps	%xmm0, -208(%rbp)
	movq	-80(%rbp), %r14                 ## 8-byte Reload
	jmp	LBB12_82
LBB12_62:
	movabsq	$25769803781, %rax              ## imm = 0x600000005
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movq	%rax, 48(%rcx)
	jmp	LBB12_2
LBB12_93:
	andq	$-4, %rsi
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	.p2align	4
LBB12_94:                               ## =>This Inner Loop Header: Depth=1
	addl	-144(%rbp,%rdx,4), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -204(%rbp,%rdx,4)
	addl	-140(%rbp,%rdx,4), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -200(%rbp,%rdx,4)
	addl	-136(%rbp,%rdx,4), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -196(%rbp,%rdx,4)
	addl	-132(%rbp,%rdx,4), %ecx
	addl	%ecx, %ecx
	movl	%ecx, -192(%rbp,%rdx,4)
	addq	$4, %rdx
	cmpq	%rdx, %rsi
	jne	LBB12_94
## %bb.78:
	testq	%rax, %rax
	je	LBB12_82
## %bb.79:
	incq	%rdx
LBB12_80:
	leaq	-208(,%rdx,4), %rsi
	addq	%rbp, %rsi
	leaq	-148(,%rdx,4), %rdx
	addq	%rbp, %rdx
	xorl	%edi, %edi
	.p2align	4
LBB12_81:                               ## =>This Inner Loop Header: Depth=1
	addl	(%rdx,%rdi,4), %ecx
	addl	%ecx, %ecx
	movl	%ecx, (%rsi,%rdi,4)
	incq	%rdi
	cmpq	%rdi, %rax
	jne	LBB12_81
LBB12_82:
	testl	%r11d, %r11d
	je	LBB12_92
## %bb.83:
	movl	%r11d, %eax
	xorl	%ecx, %ecx
	jmp	LBB12_84
	.p2align	4
LBB12_90:                               ##   in Loop: Header=BB12_84 Depth=1
	movw	%r9w, (%r14,%rcx,2)
	incl	%esi
	movl	%esi, -208(%rbp,%rdx,4)
LBB12_91:                               ##   in Loop: Header=BB12_84 Depth=1
	incq	%rcx
	cmpq	%rax, %rcx
	je	LBB12_92
LBB12_84:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB12_87 Depth 2
                                        ##     Child Loop BB12_89 Depth 2
	movzbl	(%rbx,%rcx), %edx
	testl	%edx, %edx
	je	LBB12_91
## %bb.85:                              ##   in Loop: Header=BB12_84 Depth=1
	movl	-208(%rbp,%rdx,4), %esi
	movl	%edx, %edi
	andl	$3, %edi
	xorl	%r9d, %r9d
	movl	%esi, %r8d
	cmpb	$4, %dl
	jb	LBB12_89
## %bb.86:                              ##   in Loop: Header=BB12_84 Depth=1
	movl	%edx, %r10d
	andl	$-4, %r10d
	xorl	%r9d, %r9d
	movl	%esi, %r8d
	.p2align	4
LBB12_87:                               ##   Parent Loop BB12_84 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	%r8d, %r11d
	andl	$1, %r11d
	shll	$2, %r11d
	leal	(%r11,%r9,8), %r9d
	movl	%r8d, %r11d
	andl	$2, %r11d
	orl	%r9d, %r11d
	movl	%r8d, %r9d
	shrl	$2, %r9d
	andl	$1, %r9d
	orl	%r11d, %r9d
	movl	%r8d, %r11d
	shrl	$3, %r11d
	andl	$1, %r11d
	leal	(%r11,%r9,2), %r9d
	shrl	$4, %r8d
	addl	$-4, %r10d
	jne	LBB12_87
## %bb.88:                              ##   in Loop: Header=BB12_84 Depth=1
	testl	%edi, %edi
	je	LBB12_90
	.p2align	4
LBB12_89:                               ##   Parent Loop BB12_84 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	%r8d, %r10d
	andl	$1, %r10d
	leal	(%r10,%r9,2), %r9d
	shrl	%r8d
	decl	%edi
	jne	LBB12_89
	jmp	LBB12_90
LBB12_92:
	movb	$1, %al
LBB12_3:
                                        ## kill: def $al killed $al killed $eax
	addq	$168, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB12_95:
Ltmp2:                                  ## EH_LABEL
	movq	%rax, %rdi
	callq	___clang_call_terminate
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table12:
Lexception0:
	.byte	255                             ## @LPStart Encoding = omit
	.byte	155                             ## @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ## Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ## >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ##   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ##     has no landing pad
	.byte	0                               ##   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ## >> Call Site 2 <<
	.uleb128 Ltmp1-Ltmp0                    ##   Call between Ltmp0 and Ltmp1
	.uleb128 Ltmp2-Lfunc_begin0             ##     jumps to Ltmp2
	.byte	1                               ##   On action: 1
Lcst_end0:
	.byte	1                               ## >> Action Record 1 <<
                                        ##   Catch TypeInfo 1
	.byte	0                               ##   No further actions
	.p2align	2, 0x0
                                        ## >> Catch TypeInfos <<
	.long	0                               ## TypeInfo 1
Lttbase0:
	.p2align	2, 0x0
                                        ## -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream15prepare_dynamicEv ## -- Begin function _ZN8stx_zlib13DeflateStream15prepare_dynamicEv
	.p2align	4
__ZN8stx_zlib13DeflateStream15prepare_dynamicEv: ## @_ZN8stx_zlib13DeflateStream15prepare_dynamicEv
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
	subq	$472, %rsp                      ## imm = 0x1D8
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdi, %rbx
	movq	___stack_chk_guard@GOTPCREL(%rip), %rax
	movq	(%rax), %rax
	movq	%rax, -48(%rbp)
	leaq	721048(%rdi), %rsi
	leaq	722312(%rdi), %rcx
	leaq	722628(%rdi), %rax
	movq	%rax, -416(%rbp)
	movq	$286, -408(%rbp)                ## imm = 0x11E
	movups	-416(%rbp), %xmm0
	movups	%xmm0, (%rsp)
	movl	$286, %edx                      ## imm = 0x11E
	movl	$286, %r8d                      ## imm = 0x11E
	movl	$15, %r9d
	callq	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	testb	%al, %al
	je	LBB13_83
## %bb.1:
	leaq	722192(%rbx), %rsi
	leaq	722598(%rbx), %rcx
	leaq	723200(%rbx), %rax
	movq	%rax, -400(%rbp)
	movq	$30, -392(%rbp)
	movups	-400(%rbp), %xmm0
	movups	%xmm0, (%rsp)
	movl	$30, %edx
	movl	$30, %r8d
	movq	%rbx, %rdi
	movl	$15, %r9d
	callq	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
	testb	%al, %al
	je	LBB13_83
## %bb.2:
	movl	$286, 727116(%rbx)              ## imm = 0x11E
	movl	$286, %r14d                     ## imm = 0x11E
	cmpb	$0, 722597(%rbx)
	jne	LBB13_31
## %bb.3:
	movl	$285, 727116(%rbx)              ## imm = 0x11D
	movl	$285, %r14d                     ## imm = 0x11D
	cmpb	$0, 722596(%rbx)
	jne	LBB13_31
## %bb.4:
	movl	$284, 727116(%rbx)              ## imm = 0x11C
	movl	$284, %r14d                     ## imm = 0x11C
	cmpb	$0, 722595(%rbx)
	jne	LBB13_31
## %bb.5:
	movl	$283, 727116(%rbx)              ## imm = 0x11B
	movl	$283, %r14d                     ## imm = 0x11B
	cmpb	$0, 722594(%rbx)
	jne	LBB13_31
## %bb.6:
	movl	$282, 727116(%rbx)              ## imm = 0x11A
	movl	$282, %r14d                     ## imm = 0x11A
	cmpb	$0, 722593(%rbx)
	jne	LBB13_31
## %bb.7:
	movl	$281, 727116(%rbx)              ## imm = 0x119
	movl	$281, %r14d                     ## imm = 0x119
	cmpb	$0, 722592(%rbx)
	jne	LBB13_31
## %bb.8:
	movl	$280, 727116(%rbx)              ## imm = 0x118
	movl	$280, %r14d                     ## imm = 0x118
	cmpb	$0, 722591(%rbx)
	jne	LBB13_31
## %bb.9:
	movl	$279, 727116(%rbx)              ## imm = 0x117
	movl	$279, %r14d                     ## imm = 0x117
	cmpb	$0, 722590(%rbx)
	jne	LBB13_31
## %bb.10:
	movl	$278, 727116(%rbx)              ## imm = 0x116
	movl	$278, %r14d                     ## imm = 0x116
	cmpb	$0, 722589(%rbx)
	jne	LBB13_31
## %bb.11:
	movl	$277, 727116(%rbx)              ## imm = 0x115
	movl	$277, %r14d                     ## imm = 0x115
	cmpb	$0, 722588(%rbx)
	jne	LBB13_31
## %bb.12:
	movl	$276, 727116(%rbx)              ## imm = 0x114
	movl	$276, %r14d                     ## imm = 0x114
	cmpb	$0, 722587(%rbx)
	jne	LBB13_31
## %bb.13:
	movl	$275, 727116(%rbx)              ## imm = 0x113
	movl	$275, %r14d                     ## imm = 0x113
	cmpb	$0, 722586(%rbx)
	jne	LBB13_31
## %bb.14:
	movl	$274, 727116(%rbx)              ## imm = 0x112
	movl	$274, %r14d                     ## imm = 0x112
	cmpb	$0, 722585(%rbx)
	jne	LBB13_31
## %bb.15:
	movl	$273, 727116(%rbx)              ## imm = 0x111
	movl	$273, %r14d                     ## imm = 0x111
	cmpb	$0, 722584(%rbx)
	jne	LBB13_31
## %bb.16:
	movl	$272, 727116(%rbx)              ## imm = 0x110
	movl	$272, %r14d                     ## imm = 0x110
	cmpb	$0, 722583(%rbx)
	jne	LBB13_31
## %bb.17:
	movl	$271, 727116(%rbx)              ## imm = 0x10F
	movl	$271, %r14d                     ## imm = 0x10F
	cmpb	$0, 722582(%rbx)
	jne	LBB13_31
## %bb.18:
	movl	$270, 727116(%rbx)              ## imm = 0x10E
	movl	$270, %r14d                     ## imm = 0x10E
	cmpb	$0, 722581(%rbx)
	jne	LBB13_31
## %bb.19:
	movl	$269, 727116(%rbx)              ## imm = 0x10D
	movl	$269, %r14d                     ## imm = 0x10D
	cmpb	$0, 722580(%rbx)
	jne	LBB13_31
## %bb.20:
	movl	$268, 727116(%rbx)              ## imm = 0x10C
	movl	$268, %r14d                     ## imm = 0x10C
	cmpb	$0, 722579(%rbx)
	jne	LBB13_31
## %bb.21:
	movl	$267, 727116(%rbx)              ## imm = 0x10B
	movl	$267, %r14d                     ## imm = 0x10B
	cmpb	$0, 722578(%rbx)
	jne	LBB13_31
## %bb.22:
	movl	$266, 727116(%rbx)              ## imm = 0x10A
	movl	$266, %r14d                     ## imm = 0x10A
	cmpb	$0, 722577(%rbx)
	jne	LBB13_31
## %bb.23:
	movl	$265, 727116(%rbx)              ## imm = 0x109
	movl	$265, %r14d                     ## imm = 0x109
	cmpb	$0, 722576(%rbx)
	jne	LBB13_31
## %bb.24:
	movl	$264, 727116(%rbx)              ## imm = 0x108
	movl	$264, %r14d                     ## imm = 0x108
	cmpb	$0, 722575(%rbx)
	jne	LBB13_31
## %bb.25:
	movl	$263, 727116(%rbx)              ## imm = 0x107
	movl	$263, %r14d                     ## imm = 0x107
	cmpb	$0, 722574(%rbx)
	jne	LBB13_31
## %bb.26:
	movl	$262, 727116(%rbx)              ## imm = 0x106
	movl	$262, %r14d                     ## imm = 0x106
	cmpb	$0, 722573(%rbx)
	jne	LBB13_31
## %bb.27:
	movl	$261, 727116(%rbx)              ## imm = 0x105
	movl	$261, %r14d                     ## imm = 0x105
	cmpb	$0, 722572(%rbx)
	jne	LBB13_31
## %bb.28:
	movl	$260, 727116(%rbx)              ## imm = 0x104
	movl	$260, %r14d                     ## imm = 0x104
	cmpb	$0, 722571(%rbx)
	jne	LBB13_31
## %bb.29:
	movl	$259, 727116(%rbx)              ## imm = 0x103
	movl	$259, %r14d                     ## imm = 0x103
	cmpb	$0, 722570(%rbx)
	jne	LBB13_31
## %bb.30:
	cmpb	$1, 722569(%rbx)
	movl	$258, %r14d                     ## imm = 0x102
	sbbl	$0, %r14d
	movl	%r14d, 727116(%rbx)
LBB13_31:
	movl	$30, 727120(%rbx)
	movl	$30, %r15d
	cmpb	$0, 722627(%rbx)
	jne	LBB13_60
## %bb.32:
	movl	$29, 727120(%rbx)
	movl	$29, %r15d
	cmpb	$0, 722626(%rbx)
	jne	LBB13_60
## %bb.33:
	movl	$28, 727120(%rbx)
	movl	$28, %r15d
	cmpb	$0, 722625(%rbx)
	jne	LBB13_60
## %bb.34:
	movl	$27, 727120(%rbx)
	movl	$27, %r15d
	cmpb	$0, 722624(%rbx)
	jne	LBB13_60
## %bb.35:
	movl	$26, 727120(%rbx)
	movl	$26, %r15d
	cmpb	$0, 722623(%rbx)
	jne	LBB13_60
## %bb.36:
	movl	$25, 727120(%rbx)
	movl	$25, %r15d
	cmpb	$0, 722622(%rbx)
	jne	LBB13_60
## %bb.37:
	movl	$24, 727120(%rbx)
	movl	$24, %r15d
	cmpb	$0, 722621(%rbx)
	jne	LBB13_60
## %bb.38:
	movl	$23, 727120(%rbx)
	movl	$23, %r15d
	cmpb	$0, 722620(%rbx)
	jne	LBB13_60
## %bb.39:
	movl	$22, 727120(%rbx)
	movl	$22, %r15d
	cmpb	$0, 722619(%rbx)
	jne	LBB13_60
## %bb.40:
	movl	$21, 727120(%rbx)
	movl	$21, %r15d
	cmpb	$0, 722618(%rbx)
	jne	LBB13_60
## %bb.41:
	movl	$20, 727120(%rbx)
	movl	$20, %r15d
	cmpb	$0, 722617(%rbx)
	jne	LBB13_60
## %bb.42:
	movl	$19, 727120(%rbx)
	movl	$19, %r15d
	cmpb	$0, 722616(%rbx)
	jne	LBB13_60
## %bb.43:
	movl	$18, 727120(%rbx)
	movl	$18, %r15d
	cmpb	$0, 722615(%rbx)
	jne	LBB13_60
## %bb.44:
	movl	$17, 727120(%rbx)
	movl	$17, %r15d
	cmpb	$0, 722614(%rbx)
	jne	LBB13_60
## %bb.45:
	movl	$16, 727120(%rbx)
	movl	$16, %r15d
	cmpb	$0, 722613(%rbx)
	jne	LBB13_60
## %bb.46:
	movl	$15, 727120(%rbx)
	movl	$15, %r15d
	cmpb	$0, 722612(%rbx)
	jne	LBB13_60
## %bb.47:
	movl	$14, 727120(%rbx)
	movl	$14, %r15d
	cmpb	$0, 722611(%rbx)
	jne	LBB13_60
## %bb.48:
	movl	$13, 727120(%rbx)
	movl	$13, %r15d
	cmpb	$0, 722610(%rbx)
	jne	LBB13_60
## %bb.49:
	movl	$12, 727120(%rbx)
	movl	$12, %r15d
	cmpb	$0, 722609(%rbx)
	jne	LBB13_60
## %bb.50:
	movl	$11, 727120(%rbx)
	movl	$11, %r15d
	cmpb	$0, 722608(%rbx)
	jne	LBB13_60
## %bb.51:
	movl	$10, 727120(%rbx)
	movl	$10, %r15d
	cmpb	$0, 722607(%rbx)
	jne	LBB13_60
## %bb.52:
	movl	$9, 727120(%rbx)
	movl	$9, %r15d
	cmpb	$0, 722606(%rbx)
	jne	LBB13_60
## %bb.53:
	movl	$8, 727120(%rbx)
	movl	$8, %r15d
	cmpb	$0, 722605(%rbx)
	jne	LBB13_60
## %bb.54:
	movl	$7, 727120(%rbx)
	movl	$7, %r15d
	cmpb	$0, 722604(%rbx)
	jne	LBB13_60
## %bb.55:
	movl	$6, 727120(%rbx)
	movl	$6, %r15d
	cmpb	$0, 722603(%rbx)
	jne	LBB13_60
## %bb.56:
	movl	$5, 727120(%rbx)
	movl	$5, %r15d
	cmpb	$0, 722602(%rbx)
	jne	LBB13_60
## %bb.57:
	movl	$4, 727120(%rbx)
	movl	$4, %r15d
	cmpb	$0, 722601(%rbx)
	jne	LBB13_60
## %bb.58:
	movl	$3, 727120(%rbx)
	movl	$3, %r15d
	cmpb	$0, 722600(%rbx)
	jne	LBB13_60
## %bb.59:
	cmpb	$1, 722599(%rbx)
	movl	$2, %r15d
	sbbl	$0, %r15d
	movl	%r15d, 727120(%rbx)
LBB13_60:
	movl	%r14d, %r12d
	movl	$316, %esi                      ## imm = 0x13C
	subq	%r12, %rsi
	leaq	(%r12,%rbp), %r13
	addq	$-364, %r13                     ## imm = 0xFE94
	movq	%r13, %rdi
	callq	___bzero
	leaq	-364(%rbp), %rdi
	leaq	722312(%rbx), %rsi
	movq	%r12, %rdx
	callq	_memcpy
	movl	%r15d, %edx
	movq	%r13, %rdi
	leaq	722598(%rbx), %rsi
	callq	_memcpy
	addl	%r14d, %r15d
	xorps	%xmm0, %xmm0
	movups	%xmm0, -436(%rbp)
	movaps	%xmm0, -448(%rbp)
	movaps	%xmm0, -464(%rbp)
	movaps	%xmm0, -480(%rbp)
	movaps	%xmm0, -496(%rbp)
	movl	$0, 727112(%rbx)
	leaq	723320(%rbx), %r14
	xorl	%esi, %esi
	xorl	%eax, %eax
	jmp	LBB13_62
	.p2align	4
LBB13_61:                               ##   in Loop: Header=BB13_62 Depth=1
	cmpl	%r15d, %esi
	jae	LBB13_84
LBB13_62:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB13_64 Depth 2
                                        ##     Child Loop BB13_72 Depth 2
	movl	%esi, %edi
	movzbl	-364(%rbp,%rdi), %ecx
	incl	%esi
	movl	$1, %edx
	cmpl	%r15d, %esi
	jae	LBB13_67
## %bb.63:                              ##   in Loop: Header=BB13_62 Depth=1
	movl	%r15d, %edx
	subl	%edi, %edx
	movl	$1, %esi
	.p2align	4
LBB13_64:                               ##   Parent Loop BB13_62 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	leal	(%rdi,%rsi), %r8d
	cmpb	%cl, -364(%rbp,%r8)
	jne	LBB13_81
## %bb.65:                              ##   in Loop: Header=BB13_64 Depth=2
	incq	%rsi
	cmpq	%rsi, %rdx
	jne	LBB13_64
## %bb.66:                              ##   in Loop: Header=BB13_62 Depth=1
	movl	%r15d, %esi
LBB13_67:                               ##   in Loop: Header=BB13_62 Depth=1
	testl	%ecx, %ecx
	je	LBB13_69
LBB13_68:                               ##   in Loop: Header=BB13_62 Depth=1
	movl	%eax, %eax
	leaq	(%rax,%rax,2), %rax
	movl	%ecx, (%r14,%rax,4)
	movq	$0, 4(%r14,%rax,4)
	movl	727112(%rbx), %eax
	incl	%eax
	movl	%eax, 727112(%rbx)
	movl	%ecx, %edi
	incl	-496(%rbp,%rdi,4)
	decl	%edx
LBB13_69:                               ##   in Loop: Header=BB13_62 Depth=1
	testl	%edx, %edx
	jne	LBB13_72
	jmp	LBB13_61
	.p2align	4
LBB13_70:                               ##   in Loop: Header=BB13_72 Depth=2
	cmpl	$138, %edx
	movl	$138, %edi
	cmovbl	%edx, %edi
	leal	-11(%rdi), %r8d
	movl	$7, %r10d
	movl	$18, %r9d
LBB13_71:                               ##   in Loop: Header=BB13_72 Depth=2
	movl	%eax, %eax
	leaq	(%rax,%rax,2), %rax
	movl	%r9d, (%r14,%rax,4)
	movl	%r8d, 4(%r14,%rax,4)
	movl	%r10d, 8(%r14,%rax,4)
	movl	727112(%rbx), %eax
	incl	%eax
	movl	%eax, 727112(%rbx)
	movl	%r9d, %r8d
	incl	-496(%rbp,%r8,4)
	subl	%edi, %edx
	je	LBB13_61
LBB13_72:                               ##   Parent Loop BB13_62 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	testl	%ecx, %ecx
	jne	LBB13_74
## %bb.73:                              ##   in Loop: Header=BB13_72 Depth=2
	cmpl	$11, %edx
	jae	LBB13_70
LBB13_74:                               ##   in Loop: Header=BB13_72 Depth=2
	testl	%ecx, %ecx
	jne	LBB13_77
## %bb.75:                              ##   in Loop: Header=BB13_72 Depth=2
	cmpl	$3, %edx
	jb	LBB13_77
## %bb.76:                              ##   in Loop: Header=BB13_72 Depth=2
	cmpl	$10, %edx
	movl	$10, %edi
	cmovbl	%edx, %edi
	leal	-3(%rdi), %r8d
	movl	$3, %r10d
	movl	$17, %r9d
	jmp	LBB13_71
	.p2align	4
LBB13_77:                               ##   in Loop: Header=BB13_72 Depth=2
	xorl	%r8d, %r8d
	movl	$1, %edi
	movl	%ecx, %r9d
	testl	%ecx, %ecx
	je	LBB13_80
## %bb.78:                              ##   in Loop: Header=BB13_72 Depth=2
	movl	$0, %r10d
	cmpl	$3, %edx
	jb	LBB13_71
## %bb.79:                              ##   in Loop: Header=BB13_72 Depth=2
	cmpl	$6, %edx
	movl	$6, %edi
	cmovbl	%edx, %edi
	leal	-3(%rdi), %r8d
	movl	$2, %r10d
	movl	$16, %r9d
	jmp	LBB13_71
LBB13_80:                               ##   in Loop: Header=BB13_72 Depth=2
	xorl	%r10d, %r10d
	jmp	LBB13_71
	.p2align	4
LBB13_81:                               ##   in Loop: Header=BB13_62 Depth=1
	addl	%esi, %edi
	movl	%esi, %edx
	movl	%edi, %esi
	testl	%ecx, %ecx
	jne	LBB13_68
	jmp	LBB13_69
LBB13_83:
	xorl	%eax, %eax
LBB13_109:
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	cmpq	-48(%rbp), %rcx
	jne	LBB13_111
## %bb.110:
                                        ## kill: def $al killed $al killed $eax
	addq	$472, %rsp                      ## imm = 0x1D8
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB13_84:
	leaq	723260(%rbx), %rcx
	leaq	723280(%rbx), %rax
	movq	%rax, -384(%rbp)
	movq	$19, -376(%rbp)
	movups	-384(%rbp), %xmm0
	movups	%xmm0, (%rsp)
	leaq	-496(%rbp), %rsi
	movl	$19, %edx
	movl	$19, %r8d
	movq	%rbx, %rdi
	movl	$7, %r9d
	callq	__ZN8stx_zlib13DeflateStream10build_treeENSt3__14spanIKjLm18446744073709551615EEENS2_IhLm18446744073709551615EEENS2_ItLm18446744073709551615EEEj
                                        ## kill: def $al killed $al def $eax
	testb	%al, %al
	je	LBB13_109
## %bb.85:
	movl	$19, 727124(%rbx)
	movl	$74, %ecx
	cmpb	$0, 723275(%rbx)
	jne	LBB13_100
## %bb.86:
	movl	$18, 727124(%rbx)
	movl	$71, %ecx
	cmpb	$0, 723261(%rbx)
	jne	LBB13_100
## %bb.87:
	movl	$17, 727124(%rbx)
	movl	$68, %ecx
	cmpb	$0, 723274(%rbx)
	jne	LBB13_100
## %bb.88:
	movl	$16, 727124(%rbx)
	movl	$65, %ecx
	cmpb	$0, 723262(%rbx)
	jne	LBB13_100
## %bb.89:
	movl	$15, 727124(%rbx)
	movl	$62, %ecx
	cmpb	$0, 723273(%rbx)
	jne	LBB13_100
## %bb.90:
	movl	$14, 727124(%rbx)
	movl	$59, %ecx
	cmpb	$0, 723263(%rbx)
	jne	LBB13_100
## %bb.91:
	movl	$13, 727124(%rbx)
	movl	$56, %ecx
	cmpb	$0, 723272(%rbx)
	jne	LBB13_100
## %bb.92:
	movl	$12, 727124(%rbx)
	movl	$53, %ecx
	cmpb	$0, 723264(%rbx)
	jne	LBB13_100
## %bb.93:
	movl	$11, 727124(%rbx)
	movl	$50, %ecx
	cmpb	$0, 723271(%rbx)
	jne	LBB13_100
## %bb.94:
	movl	$10, 727124(%rbx)
	movl	$47, %ecx
	cmpb	$0, 723265(%rbx)
	jne	LBB13_100
## %bb.95:
	movl	$9, 727124(%rbx)
	movl	$44, %ecx
	cmpb	$0, 723270(%rbx)
	jne	LBB13_100
## %bb.96:
	movl	$8, 727124(%rbx)
	movl	$41, %ecx
	cmpb	$0, 723266(%rbx)
	jne	LBB13_100
## %bb.97:
	movl	$7, 727124(%rbx)
	movl	$38, %ecx
	cmpb	$0, 723269(%rbx)
	jne	LBB13_100
## %bb.98:
	movl	$6, 727124(%rbx)
	movl	$35, %ecx
	cmpb	$0, 723267(%rbx)
	jne	LBB13_100
## %bb.99:
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	cmpb	$0, 723268(%rbx)
	setne	%cl
	sete	%dl
	xorl	$5, %edx
	movl	%edx, 727124(%rbx)
	leaq	(%rcx,%rcx,2), %rcx
	addq	$29, %rcx
LBB13_100:
	addq	727128(%rbx), %rcx
	movq	%rcx, 727136(%rbx)
	xorl	%edx, %edx
	.p2align	4
LBB13_101:                              ## =>This Inner Loop Header: Depth=1
	movzbl	722312(%rbx,%rdx), %esi
	imull	721048(%rbx,%rdx,4), %esi
	addq	%rcx, %rsi
	movq	%rsi, 727136(%rbx)
	movzbl	722313(%rbx,%rdx), %ecx
	imull	721052(%rbx,%rdx,4), %ecx
	addq	%rsi, %rcx
	movq	%rcx, 727136(%rbx)
	addq	$2, %rdx
	cmpq	$286, %rdx                      ## imm = 0x11E
	jne	LBB13_101
## %bb.102:
	movzbl	722598(%rbx), %edx
	imull	722192(%rbx), %edx
	movzbl	722599(%rbx), %esi
	imull	722196(%rbx), %esi
	addq	%rdx, %rsi
	movzbl	722600(%rbx), %edx
	imull	722200(%rbx), %edx
	addq	%rsi, %rdx
	addq	%rdx, %rcx
	movzbl	722601(%rbx), %edx
	imull	722204(%rbx), %edx
	movzbl	722602(%rbx), %esi
	imull	722208(%rbx), %esi
	addq	%rdx, %rsi
	movzbl	722603(%rbx), %edx
	imull	722212(%rbx), %edx
	addq	%rsi, %rdx
	movzbl	722604(%rbx), %esi
	imull	722216(%rbx), %esi
	addq	%rdx, %rsi
	addq	%rcx, %rsi
	movzbl	722605(%rbx), %ecx
	imull	722220(%rbx), %ecx
	movzbl	722606(%rbx), %edx
	imull	722224(%rbx), %edx
	addq	%rcx, %rdx
	movzbl	722607(%rbx), %ecx
	imull	722228(%rbx), %ecx
	addq	%rdx, %rcx
	movzbl	722608(%rbx), %edi
	imull	722232(%rbx), %edi
	addq	%rcx, %rdi
	movzbl	722609(%rbx), %edx
	imull	722236(%rbx), %edx
	addq	%rdi, %rdx
	addq	%rsi, %rdx
	movzbl	722610(%rbx), %ecx
	imull	722240(%rbx), %ecx
	movzbl	722611(%rbx), %esi
	imull	722244(%rbx), %esi
	addq	%rcx, %rsi
	movzbl	722612(%rbx), %ecx
	imull	722248(%rbx), %ecx
	addq	%rsi, %rcx
	movzbl	722613(%rbx), %esi
	imull	722252(%rbx), %esi
	addq	%rcx, %rsi
	movzbl	722614(%rbx), %edi
	imull	722256(%rbx), %edi
	addq	%rsi, %rdi
	movzbl	722615(%rbx), %ecx
	imull	722260(%rbx), %ecx
	addq	%rdi, %rcx
	addq	%rdx, %rcx
	movzbl	722616(%rbx), %edx
	imull	722264(%rbx), %edx
	movzbl	722617(%rbx), %esi
	imull	722268(%rbx), %esi
	addq	%rdx, %rsi
	movzbl	722618(%rbx), %edx
	imull	722272(%rbx), %edx
	addq	%rsi, %rdx
	movzbl	722619(%rbx), %esi
	imull	722276(%rbx), %esi
	addq	%rdx, %rsi
	movzbl	722620(%rbx), %edx
	imull	722280(%rbx), %edx
	addq	%rsi, %rdx
	movzbl	722621(%rbx), %esi
	imull	722284(%rbx), %esi
	addq	%rdx, %rsi
	movzbl	722622(%rbx), %edx
	imull	722288(%rbx), %edx
	addq	%rsi, %rdx
	addq	%rcx, %rdx
	movzbl	722623(%rbx), %ecx
	imull	722292(%rbx), %ecx
	movzbl	722624(%rbx), %esi
	imull	722296(%rbx), %esi
	addq	%rcx, %rsi
	movzbl	722625(%rbx), %ecx
	imull	722300(%rbx), %ecx
	addq	%rsi, %rcx
	movzbl	722626(%rbx), %esi
	imull	722304(%rbx), %esi
	addq	%rcx, %rsi
	movzbl	722627(%rbx), %ecx
	imull	722308(%rbx), %ecx
	addq	%rsi, %rcx
	addq	%rdx, %rcx
	movq	%rcx, 727136(%rbx)
	movl	727112(%rbx), %edx
	testq	%rdx, %rdx
	je	LBB13_109
## %bb.103:
	cmpl	$1, %edx
	jne	LBB13_105
## %bb.104:
	xorl	%esi, %esi
	jmp	LBB13_108
LBB13_105:
	movl	%edx, %edi
	andl	$-2, %edi
	leaq	723340(%rbx), %r8
	xorl	%esi, %esi
	.p2align	4
LBB13_106:                              ## =>This Inner Loop Header: Depth=1
	movl	-20(%r8), %r9d
	movzbl	723260(%rbx,%r9), %r9d
	addl	-12(%r8), %r9d
	addq	%rcx, %r9
	movq	%r9, 727136(%rbx)
	movl	-8(%r8), %ecx
	movzbl	723260(%rbx,%rcx), %ecx
	addl	(%r8), %ecx
	addq	%r9, %rcx
	movq	%rcx, 727136(%rbx)
	addq	$2, %rsi
	addq	$24, %r8
	cmpq	%rsi, %rdi
	jne	LBB13_106
## %bb.107:
	testb	$1, %dl
	je	LBB13_109
LBB13_108:
	leaq	(%rsi,%rsi,2), %rdx
	movl	(%r14,%rdx,4), %esi
	movzbl	723260(%rbx,%rsi), %esi
	addl	8(%r14,%rdx,4), %esi
	addq	%rcx, %rsi
	movq	%rsi, 727136(%rbx)
	jmp	LBB13_109
LBB13_111:
	callq	___stack_chk_fail
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream10write_bitsEjj ## -- Begin function _ZN8stx_zlib13DeflateStream10write_bitsEjj
	.p2align	4
__ZN8stx_zlib13DeflateStream10write_bitsEjj: ## @_ZN8stx_zlib13DeflateStream10write_bitsEjj
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	72(%rdi), %rax
	movl	$5, %ecx
	cmpq	%rax, 40(%rdi)
	je	LBB14_5
## %bb.1:
	incq	%rax
	movl	$-1, %r8d
	movl	%edx, %ecx
	shll	%cl, %r8d
	movq	%rax, 72(%rdi)
	notl	%r8d
	andl	%r8d, %esi
	movl	804220(%rdi), %ecx
	shll	%cl, %esi
	orl	804216(%rdi), %esi
	movl	%esi, 804216(%rdi)
	addl	%edx, %ecx
	movl	%ecx, 804220(%rdi)
	movb	$1, %al
	cmpl	$8, %ecx
	jb	LBB14_7
## %bb.2:
	movq	804200(%rdi), %rcx
	.p2align	4
LBB14_3:                                ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB14_4
## %bb.6:                               ##   in Loop: Header=BB14_3 Depth=1
	movb	%sil, 738664(%rdi,%rcx)
	movq	804200(%rdi), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rdi)
	movl	804216(%rdi), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rdi)
	movl	804220(%rdi), %edx
	addl	$-8, %edx
	movl	%edx, 804220(%rdi)
	cmpl	$7, %edx
	ja	LBB14_3
LBB14_7:
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
LBB14_4:
	movl	$6, %ecx
LBB14_5:
	movl	%ecx, 52(%rdi)
	movl	$5, 48(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream10align_bitsEv ## -- Begin function _ZN8stx_zlib13DeflateStream10align_bitsEv
	.p2align	4
__ZN8stx_zlib13DeflateStream10align_bitsEv: ## @_ZN8stx_zlib13DeflateStream10align_bitsEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movb	$1, %al
	cmpl	$0, 804220(%rdi)
	je	LBB15_7
## %bb.1:
	movq	72(%rdi), %rdx
	movl	$5, %ecx
	cmpq	%rdx, 40(%rdi)
	je	LBB15_5
## %bb.2:
	incq	%rdx
	movq	%rdx, 72(%rdi)
	movl	804216(%rdi), %edx
	movl	$8, 804220(%rdi)
	movq	804200(%rdi), %rcx
	.p2align	4
LBB15_3:                                ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB15_4
## %bb.6:                               ##   in Loop: Header=BB15_3 Depth=1
	movb	%dl, 738664(%rdi,%rcx)
	movq	804200(%rdi), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rdi)
	movl	804216(%rdi), %edx
	shrl	$8, %edx
	movl	%edx, 804216(%rdi)
	movl	804220(%rdi), %esi
	addl	$-8, %esi
	movl	%esi, 804220(%rdi)
	cmpl	$7, %esi
	ja	LBB15_3
LBB15_7:
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
LBB15_4:
	movl	$6, %ecx
LBB15_5:
	movl	%ecx, 52(%rdi)
	movl	$5, 48(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream11emit_tokensEb ## -- Begin function _ZN8stx_zlib13DeflateStream11emit_tokensEb
	.p2align	4
__ZN8stx_zlib13DeflateStream11emit_tokensEb: ## @_ZN8stx_zlib13DeflateStream11emit_tokensEb
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
	leaq	722312(%rdi), %rax
	leaq	__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE(%rip), %r8
	testl	%esi, %esi
	cmovneq	%rax, %r8
	movq	727128(%rdi), %rax
	xorl	%ecx, %ecx
	.p2align	4
LBB16_1:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%r8,%rcx), %edx
	imull	721048(%rdi,%rcx,4), %edx
	addq	%rax, %rdx
	movzbl	1(%r8,%rcx), %eax
	imull	721052(%rdi,%rcx,4), %eax
	addq	%rdx, %rax
	addq	$2, %rcx
	cmpq	$286, %rcx                      ## imm = 0x11E
	jne	LBB16_1
## %bb.2:
	leaq	723200(%rdi), %rcx
	testb	%sil, %sil
	leaq	__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE+888(%rip), %rdx
	cmovneq	%rcx, %rdx
	movq	%rdx, -88(%rbp)                 ## 8-byte Spill
	leaq	722628(%rdi), %rcx
	testb	%sil, %sil
	leaq	__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE+286(%rip), %r12
	cmovneq	%rcx, %r12
	leaq	722598(%rdi), %rcx
	testb	%sil, %sil
	leaq	__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE+858(%rip), %rdx
	cmovneq	%rcx, %rdx
	movdqu	722192(%rdi), %xmm4
	movdqu	722208(%rdi), %xmm1
	movdqu	722224(%rdi), %xmm2
	movdqu	722240(%rdi), %xmm3
	movdqu	(%rdx), %xmm5
	pxor	%xmm0, %xmm0
	movdqa	%xmm5, %xmm7
	punpckhbw	%xmm0, %xmm7            ## xmm7 = xmm7[8],xmm0[8],xmm7[9],xmm0[9],xmm7[10],xmm0[10],xmm7[11],xmm0[11],xmm7[12],xmm0[12],xmm7[13],xmm0[13],xmm7[14],xmm0[14],xmm7[15],xmm0[15]
	movdqa	%xmm7, %xmm6
	punpcklwd	%xmm0, %xmm6            ## xmm6 = xmm6[0],xmm0[0],xmm6[1],xmm0[1],xmm6[2],xmm0[2],xmm6[3],xmm0[3]
	punpckhwd	%xmm0, %xmm7            ## xmm7 = xmm7[4],xmm0[4],xmm7[5],xmm0[5],xmm7[6],xmm0[6],xmm7[7],xmm0[7]
	punpcklbw	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0],xmm5[1],xmm0[1],xmm5[2],xmm0[2],xmm5[3],xmm0[3],xmm5[4],xmm0[4],xmm5[5],xmm0[5],xmm5[6],xmm0[6],xmm5[7],xmm0[7]
	movdqa	%xmm5, %xmm8
	punpcklwd	%xmm0, %xmm8            ## xmm8 = xmm8[0],xmm0[0],xmm8[1],xmm0[1],xmm8[2],xmm0[2],xmm8[3],xmm0[3]
	punpckhwd	%xmm0, %xmm5            ## xmm5 = xmm5[4],xmm0[4],xmm5[5],xmm0[5],xmm5[6],xmm0[6],xmm5[7],xmm0[7]
	pshufd	$245, %xmm1, %xmm9              ## xmm9 = xmm1[1,1,3,3]
	pmuludq	%xmm5, %xmm1
	pshufd	$232, %xmm1, %xmm1              ## xmm1 = xmm1[0,2,2,3]
	pshufd	$245, %xmm5, %xmm5              ## xmm5 = xmm5[1,1,3,3]
	pmuludq	%xmm9, %xmm5
	pshufd	$232, %xmm5, %xmm5              ## xmm5 = xmm5[0,2,2,3]
	punpckldq	%xmm5, %xmm1            ## xmm1 = xmm1[0],xmm5[0],xmm1[1],xmm5[1]
	pshufd	$245, %xmm4, %xmm5              ## xmm5 = xmm4[1,1,3,3]
	pmuludq	%xmm8, %xmm4
	pshufd	$232, %xmm4, %xmm4              ## xmm4 = xmm4[0,2,2,3]
	pshufd	$245, %xmm8, %xmm8              ## xmm8 = xmm8[1,1,3,3]
	pmuludq	%xmm5, %xmm8
	pshufd	$232, %xmm8, %xmm5              ## xmm5 = xmm8[0,2,2,3]
	punpckldq	%xmm5, %xmm4            ## xmm4 = xmm4[0],xmm5[0],xmm4[1],xmm5[1]
	pshufd	$245, %xmm3, %xmm8              ## xmm8 = xmm3[1,1,3,3]
	pmuludq	%xmm7, %xmm3
	pshufd	$232, %xmm3, %xmm5              ## xmm5 = xmm3[0,2,2,3]
	pshufd	$245, %xmm7, %xmm3              ## xmm3 = xmm7[1,1,3,3]
	pmuludq	%xmm8, %xmm3
	pshufd	$232, %xmm3, %xmm3              ## xmm3 = xmm3[0,2,2,3]
	punpckldq	%xmm3, %xmm5            ## xmm5 = xmm5[0],xmm3[0],xmm5[1],xmm3[1]
	pshufd	$245, %xmm2, %xmm7              ## xmm7 = xmm2[1,1,3,3]
	pmuludq	%xmm6, %xmm2
	pshufd	$232, %xmm2, %xmm3              ## xmm3 = xmm2[0,2,2,3]
	pshufd	$245, %xmm6, %xmm2              ## xmm2 = xmm6[1,1,3,3]
	pmuludq	%xmm7, %xmm2
	pshufd	$232, %xmm2, %xmm2              ## xmm2 = xmm2[0,2,2,3]
	punpckldq	%xmm2, %xmm3            ## xmm3 = xmm3[0],xmm2[0],xmm3[1],xmm2[1]
	movdqa	%xmm3, %xmm2
	punpckhdq	%xmm0, %xmm2            ## xmm2 = xmm2[2],xmm0[2],xmm2[3],xmm0[3]
	movdqa	%xmm5, %xmm7
	punpckhdq	%xmm0, %xmm7            ## xmm7 = xmm7[2],xmm0[2],xmm7[3],xmm0[3]
	punpckldq	%xmm0, %xmm3            ## xmm3 = xmm3[0],xmm0[0],xmm3[1],xmm0[1]
	punpckldq	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0],xmm5[1],xmm0[1]
	movdqa	%xmm4, %xmm11
	punpckhdq	%xmm0, %xmm11           ## xmm11 = xmm11[2],xmm0[2],xmm11[3],xmm0[3]
	punpckldq	%xmm0, %xmm4            ## xmm4 = xmm4[0],xmm0[0],xmm4[1],xmm0[1]
	movdqa	%xmm1, %xmm10
	punpckldq	%xmm0, %xmm10           ## xmm10 = xmm10[0],xmm0[0],xmm10[1],xmm0[1]
	punpckhdq	%xmm0, %xmm1            ## xmm1 = xmm1[2],xmm0[2],xmm1[3],xmm0[3]
	movdqu	722256(%rdi), %xmm8
	movdqu	722272(%rdi), %xmm6
	movq	16(%rdx), %xmm9                 ## xmm9 = mem[0],zero
	punpcklbw	%xmm0, %xmm9            ## xmm9 = xmm9[0],xmm0[0],xmm9[1],xmm0[1],xmm9[2],xmm0[2],xmm9[3],xmm0[3],xmm9[4],xmm0[4],xmm9[5],xmm0[5],xmm9[6],xmm0[6],xmm9[7],xmm0[7]
	movdqa	%xmm9, %xmm12
	punpcklwd	%xmm0, %xmm12           ## xmm12 = xmm12[0],xmm0[0],xmm12[1],xmm0[1],xmm12[2],xmm0[2],xmm12[3],xmm0[3]
	punpckhwd	%xmm0, %xmm9            ## xmm9 = xmm9[4],xmm0[4],xmm9[5],xmm0[5],xmm9[6],xmm0[6],xmm9[7],xmm0[7]
	pshufd	$245, %xmm6, %xmm13             ## xmm13 = xmm6[1,1,3,3]
	pmuludq	%xmm9, %xmm6
	pshufd	$232, %xmm6, %xmm6              ## xmm6 = xmm6[0,2,2,3]
	pshufd	$245, %xmm9, %xmm9              ## xmm9 = xmm9[1,1,3,3]
	pmuludq	%xmm13, %xmm9
	pshufd	$232, %xmm9, %xmm9              ## xmm9 = xmm9[0,2,2,3]
	punpckldq	%xmm9, %xmm6            ## xmm6 = xmm6[0],xmm9[0],xmm6[1],xmm9[1]
	pshufd	$245, %xmm8, %xmm13             ## xmm13 = xmm8[1,1,3,3]
	pmuludq	%xmm12, %xmm8
	pshufd	$232, %xmm8, %xmm9              ## xmm9 = xmm8[0,2,2,3]
	pshufd	$245, %xmm12, %xmm8             ## xmm8 = xmm12[1,1,3,3]
	pmuludq	%xmm13, %xmm8
	pshufd	$232, %xmm8, %xmm8              ## xmm8 = xmm8[0,2,2,3]
	punpckldq	%xmm8, %xmm9            ## xmm9 = xmm9[0],xmm8[0],xmm9[1],xmm8[1]
	movdqa	%xmm9, %xmm8
	punpckhdq	%xmm0, %xmm8            ## xmm8 = xmm8[2],xmm0[2],xmm8[3],xmm0[3]
	paddq	%xmm11, %xmm8
	punpckldq	%xmm0, %xmm9            ## xmm9 = xmm9[0],xmm0[0],xmm9[1],xmm0[1]
	paddq	%xmm4, %xmm9
	movdqa	%xmm6, %xmm4
	punpckldq	%xmm0, %xmm4            ## xmm4 = xmm4[0],xmm0[0],xmm4[1],xmm0[1]
	paddq	%xmm10, %xmm4
	paddq	%xmm5, %xmm4
	punpckhdq	%xmm0, %xmm6            ## xmm6 = xmm6[2],xmm0[2],xmm6[3],xmm0[3]
	paddq	%xmm1, %xmm6
	paddq	%xmm7, %xmm6
	movdqu	722288(%rdi), %xmm1
	movd	24(%rdx), %xmm5                 ## xmm5 = mem[0],zero,zero,zero
	punpcklbw	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0],xmm5[1],xmm0[1],xmm5[2],xmm0[2],xmm5[3],xmm0[3],xmm5[4],xmm0[4],xmm5[5],xmm0[5],xmm5[6],xmm0[6],xmm5[7],xmm0[7]
	punpcklwd	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0],xmm5[1],xmm0[1],xmm5[2],xmm0[2],xmm5[3],xmm0[3]
	pshufd	$245, %xmm1, %xmm7              ## xmm7 = xmm1[1,1,3,3]
	pmuludq	%xmm5, %xmm1
	pshufd	$232, %xmm1, %xmm1              ## xmm1 = xmm1[0,2,2,3]
	pshufd	$245, %xmm5, %xmm5              ## xmm5 = xmm5[1,1,3,3]
	pmuludq	%xmm7, %xmm5
	pshufd	$232, %xmm5, %xmm5              ## xmm5 = xmm5[0,2,2,3]
	punpckldq	%xmm5, %xmm1            ## xmm1 = xmm1[0],xmm5[0],xmm1[1],xmm5[1]
	movdqa	%xmm1, %xmm5
	punpckldq	%xmm0, %xmm5            ## xmm5 = xmm5[0],xmm0[0],xmm5[1],xmm0[1]
	paddq	%xmm3, %xmm5
	paddq	%xmm9, %xmm5
	paddq	%xmm4, %xmm5
	punpckhdq	%xmm0, %xmm1            ## xmm1 = xmm1[2],xmm0[2],xmm1[3],xmm0[3]
	paddq	%xmm2, %xmm1
	paddq	%xmm8, %xmm1
	paddq	%xmm6, %xmm1
	paddq	%xmm5, %xmm1
	movzbl	28(%rdx), %ecx
	imull	722304(%rdi), %ecx
	movq	%rdx, -80(%rbp)                 ## 8-byte Spill
	movzbl	29(%rdx), %r9d
	imull	722308(%rdi), %r9d
	movl	804220(%rdi), %edx
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	paddq	%xmm1, %xmm0
	movq	%xmm0, %r10
	addq	%rdx, %r9
	addq	%rcx, %r9
	addq	%rax, %r9
	addq	%r10, %r9
	shrq	$3, %r9
	movq	804200(%rdi), %r11
	movl	$65536, %eax                    ## imm = 0x10000
	subq	%r11, %rax
	cmpq	%rax, %r9
	jbe	LBB16_6
LBB16_3:
	movl	$6, %esi
	jmp	LBB16_4
LBB16_6:
	movq	721040(%rdi), %rbx
	leaq	(,%rbx,4), %rax
	movq	40(%rdi), %r10
	movq	72(%rdi), %r9
	movq	%r10, %rcx
	subq	%r9, %rcx
	movl	$5, %esi
	cmpq	%rcx, %rax
	ja	LBB16_4
## %bb.7:
	addq	%rax, %r9
	movq	%r9, 72(%rdi)
	movl	804216(%rdi), %r13d
	testq	%rbx, %rbx
	je	LBB16_12
## %bb.8:
	movq	%r10, -48(%rbp)                 ## 8-byte Spill
	movq	%r9, -56(%rbp)                  ## 8-byte Spill
	xorl	%r14d, %r14d
	movq	%r12, -64(%rbp)                 ## 8-byte Spill
	movq	%rbx, -72(%rbp)                 ## 8-byte Spill
	jmp	LBB16_9
	.p2align	4
LBB16_20:                               ##   in Loop: Header=BB16_9 Depth=1
	movl	%eax, %edx
	movq	%r15, %r13
LBB16_27:                               ##   in Loop: Header=BB16_9 Depth=1
	incq	%r14
	cmpq	%rbx, %r14
	je	LBB16_11
LBB16_9:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB16_23 Depth 2
                                        ##     Child Loop BB16_26 Depth 2
	movzwl	589964(%rdi,%r14,4), %eax
	movzwl	589966(%rdi,%r14,4), %r15d
	testl	%r15d, %r15d
	je	LBB16_10
## %bb.16:                              ##   in Loop: Header=BB16_9 Depth=1
	leaq	__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE(%rip), %rcx
	movl	(%rcx,%rax,4), %r10d
	leal	257(%r10), %r9d
	movzbl	(%r8,%r9), %ecx
	leaq	__ZN8stx_zlib12_GLOBAL__N_111length_baseE(%rip), %rsi
	movl	(%rsi,%r10,4), %ebx
	subl	%ebx, %eax
	shlq	%cl, %rax
	leal	-1(%r15), %ebx
	cmpw	$5, %r15w
	jb	LBB16_18
## %bb.17:                              ##   in Loop: Header=BB16_9 Depth=1
	bsrl	%ebx, %r12d
	xorl	$31, %r12d
	movl	$30, %esi
	subl	%r12d, %esi
	addl	%r12d, %r12d
	btl	%esi, %ebx
	movl	$0, %ebx
	adcl	$62, %ebx
	subl	%r12d, %ebx
	movq	-64(%rbp), %r12                 ## 8-byte Reload
LBB16_18:                               ##   in Loop: Header=BB16_9 Depth=1
	movzwl	(%r12,%r9,2), %esi
	leaq	__ZN8stx_zlib12_GLOBAL__N_112length_extraE(%rip), %r9
	addl	(%r9,%r10,4), %ecx
	orq	%rsi, %rax
	movl	%ebx, %esi
	movq	-88(%rbp), %r9                  ## 8-byte Reload
	movzwl	(%r9,%rsi,2), %r9d
	shlq	%cl, %r9
	orq	%rax, %r9
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	movzbl	(%rax,%rsi), %eax
	addl	%ecx, %eax
	leaq	__ZN8stx_zlib12_GLOBAL__N_113distance_baseE(%rip), %rcx
	subl	(%rcx,%rsi,4), %r15d
	movl	%eax, %ecx
	shlq	%cl, %r15
	orq	%r9, %r15
	movl	%ebx, %r9d
	shrl	%r9d
	decl	%r9d
	cmpl	$4, %ebx
	movl	$0, %ecx
	cmovbl	%ecx, %r9d
	addl	%eax, %r9d
	movq	-72(%rbp), %rbx                 ## 8-byte Reload
	jmp	LBB16_19
	.p2align	4
LBB16_10:                               ##   in Loop: Header=BB16_9 Depth=1
	movzwl	(%r12,%rax,2), %r15d
	movzbl	(%r8,%rax), %r9d
LBB16_19:                               ##   in Loop: Header=BB16_9 Depth=1
	movl	%edx, %ecx
	shlq	%cl, %r15
	orq	%r13, %r15
	leal	(%r9,%rdx), %eax
	cmpl	$8, %eax
	jb	LBB16_20
## %bb.21:                              ##   in Loop: Header=BB16_9 Depth=1
	leal	(%r9,%rdx), %ecx
	addl	$-8, %ecx
	movl	%ecx, %edx
	notl	%edx
	testb	$24, %dl
	je	LBB16_24
## %bb.22:                              ##   in Loop: Header=BB16_9 Depth=1
	movl	%ecx, %edx
	shrl	$3, %edx
	incl	%edx
	andl	$3, %edx
	.p2align	4
LBB16_23:                               ##   Parent Loop BB16_9 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movb	%r15b, 738664(%rdi,%r11)
	incq	%r11
	shrq	$8, %r15
	addl	$-8, %eax
	decl	%edx
	jne	LBB16_23
LBB16_24:                               ##   in Loop: Header=BB16_9 Depth=1
	movl	%eax, %edx
	movq	%r15, %r13
	cmpl	$24, %ecx
	jb	LBB16_27
## %bb.25:                              ##   in Loop: Header=BB16_9 Depth=1
	movq	%r15, %r13
	movl	%eax, %edx
	.p2align	4
LBB16_26:                               ##   Parent Loop BB16_9 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movl	%r13d, 738664(%rdi,%r11)
	addq	$4, %r11
	shrq	$32, %r13
	addl	$-32, %edx
	cmpl	$7, %edx
	ja	LBB16_26
	jmp	LBB16_27
LBB16_11:
	movq	-56(%rbp), %r9                  ## 8-byte Reload
	movl	$5, %esi
	movq	-48(%rbp), %r10                 ## 8-byte Reload
LBB16_12:
	movl	%r13d, 804216(%rdi)
	movl	%edx, 804220(%rdi)
	movq	%r11, 804200(%rdi)
	cmpq	%r9, %r10
	jne	LBB16_13
LBB16_4:
	movl	%esi, 52(%rdi)
	movl	$5, 48(%rdi)
	xorl	%eax, %eax
LBB16_5:
                                        ## kill: def $al killed $al killed $eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB16_13:
	movzbl	256(%r8), %eax
	movl	$-1, %esi
	movl	%eax, %ecx
	shll	%cl, %esi
	movzwl	512(%r12), %ecx
	notl	%esi
	andl	%ecx, %esi
	movl	%edx, %ecx
	shll	%cl, %esi
	incq	%r9
	movq	%r9, 72(%rdi)
	orl	%r13d, %esi
	movl	%esi, 804216(%rdi)
	addl	%eax, %edx
	movl	%edx, 804220(%rdi)
	movb	$1, %al
	cmpl	$8, %edx
	jb	LBB16_5
	.p2align	4
LBB16_14:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r11                    ## imm = 0x10000
	je	LBB16_3
## %bb.15:                              ##   in Loop: Header=BB16_14 Depth=1
	movb	%sil, 738664(%rdi,%r11)
	movq	804200(%rdi), %r11
	incq	%r11
	movq	%r11, 804200(%rdi)
	movl	804216(%rdi), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rdi)
	movl	804220(%rdi), %ecx
	addl	$-8, %ecx
	movl	%ecx, 804220(%rdi)
	cmpl	$7, %ecx
	ja	LBB16_14
	jmp	LBB16_5
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN8stx_zlib13DeflateStream12encode_blockEb
LCPI17_0:
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
LCPI17_1:
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
	.long	2147483648                      ## 0x80000000
LCPI17_2:
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.short	0                               ## 0x0
	.short	65535                           ## 0xffff
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream12encode_blockEb
	.p2align	4
__ZN8stx_zlib13DeflateStream12encode_blockEb: ## @_ZN8stx_zlib13DeflateStream12encode_blockEb
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
	pushq	%rax
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%esi, %r14d
	movq	%rdi, %rbx
	movl	$5, %r15d
	cmpl	$0, 80(%rdi)
	je	LBB17_10
## %bb.1:
	movl	$5, %r12d
	subl	804220(%rbx), %r12d
	movq	65664(%rbx), %r13
	movq	%rbx, %rdi
	callq	__ZN8stx_zlib13DeflateStream8tokenizeEv
	testb	%al, %al
	je	LBB17_37
## %bb.2:
	andl	$7, %r12d
	leaq	(%r12,%r13,8), %r12
	addq	$35, %r12
	movq	727128(%rbx), %rcx
	leaq	3(%rcx), %rax
	addq	$29, %rcx
	xorl	%edx, %edx
	jmp	LBB17_4
	.p2align	4
LBB17_3:                                ##   in Loop: Header=BB17_4 Depth=1
	movl	721048(%rbx,%rdx,4), %r8d
	imull	%r8d, %esi
	movq	%rax, %r9
	addq	%rsi, %r9
	addq	%r8, %rcx
	movl	721052(%rbx,%rdx,4), %eax
	addq	%rax, %rcx
                                        ## kill: def $eax killed $eax killed $rax def $rax
	imull	%edi, %eax
	addq	%r9, %rax
	addq	$2, %rdx
	cmpq	$286, %rdx                      ## imm = 0x11E
	je	LBB17_12
LBB17_4:                                ## =>This Inner Loop Header: Depth=1
	movl	$8, %esi
	cmpq	$144, %rdx
	jb	LBB17_7
## %bb.5:                               ##   in Loop: Header=BB17_4 Depth=1
	movl	$9, %esi
	cmpq	$256, %rdx                      ## imm = 0x100
	jb	LBB17_7
## %bb.6:                               ##   in Loop: Header=BB17_4 Depth=1
	cmpq	$280, %rdx                      ## imm = 0x118
	movl	$8, %esi
	sbbl	$0, %esi
LBB17_7:                                ##   in Loop: Header=BB17_4 Depth=1
	movl	$8, %edi
	cmpq	$144, %rdx
	jb	LBB17_3
## %bb.8:                               ##   in Loop: Header=BB17_4 Depth=1
	movl	$9, %edi
	cmpq	$256, %rdx                      ## imm = 0x100
	jb	LBB17_3
## %bb.9:                               ##   in Loop: Header=BB17_4 Depth=1
	cmpq	$280, %rdx                      ## imm = 0x118
	movl	$8, %edi
	sbbl	$0, %edi
	jmp	LBB17_3
LBB17_10:
	xorl	%edx, %edx
	movq	40(%rbx), %rdi
	movq	72(%rbx), %r9
	cmpq	%r9, %rdi
	jne	LBB17_21
	jmp	LBB17_35
LBB17_12:
	movl	722192(%rbx), %edx
	movl	722196(%rbx), %esi
	leal	(%rdx,%rdx,4), %edi
	leal	(%rsi,%rsi,4), %r8d
	addq	%rdi, %r8
	addq	%rsi, %rdx
	movl	722200(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%r8, %rdi
	addq	%rsi, %rdx
	addq	%rdx, %rcx
	movl	722204(%rbx), %edx
	leal	(%rdx,%rdx,4), %esi
	addq	%rdi, %rsi
	movl	722208(%rbx), %edi
	leal	(%rdi,%rdi,4), %r8d
	addq	%rsi, %r8
	addq	%r8, %rax
	addq	%rdi, %rdx
	movl	722212(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%rsi, %rdx
	movl	722216(%rbx), %esi
	leal	(%rsi,%rsi,4), %r8d
	addq	%rdi, %r8
	addq	%rsi, %rdx
	addq	%rcx, %rdx
	movl	722220(%rbx), %ecx
	leal	(%rcx,%rcx,4), %esi
	addq	%r8, %rsi
	movl	722224(%rbx), %edi
	leal	(%rdi,%rdi,4), %r8d
	addq	%rsi, %r8
	addq	%rdi, %rcx
	movl	722228(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%r8, %rdi
	addq	%rsi, %rcx
	movl	722232(%rbx), %r8d
	leal	(%r8,%r8,4), %esi
	addq	%rdi, %rsi
	addq	%rax, %rsi
	addq	%r8, %rcx
	movl	722236(%rbx), %eax
	leal	(%rax,%rax,4), %edi
	addq	%rax, %rcx
	addq	%rdx, %rcx
	movl	722240(%rbx), %eax
	leal	(%rax,%rax,4), %edx
	addq	%rdi, %rdx
	movl	722244(%rbx), %edi
	leal	(%rdi,%rdi,4), %r8d
	addq	%rdx, %r8
	addq	%rdi, %rax
	movl	722248(%rbx), %edx
	leal	(%rdx,%rdx,4), %edi
	addq	%r8, %rdi
	addq	%rdx, %rax
	movl	722252(%rbx), %edx
	leal	(%rdx,%rdx,4), %r8d
	addq	%rdi, %r8
	addq	%rdx, %rax
	movl	722256(%rbx), %edx
	leal	(%rdx,%rdx,4), %edi
	addq	%r8, %rdi
	addq	%rdx, %rax
	movl	722260(%rbx), %r8d
	leal	(%r8,%r8,4), %edx
	addq	%rdi, %rdx
	addq	%rsi, %rdx
	addq	%r8, %rax
	addq	%rcx, %rax
	movl	722264(%rbx), %ecx
	leal	(%rcx,%rcx,4), %esi
	movl	722268(%rbx), %edi
	leal	(%rdi,%rdi,4), %r8d
	addq	%rsi, %r8
	addq	%rdi, %rcx
	movl	722272(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%r8, %rdi
	addq	%rsi, %rcx
	movl	722276(%rbx), %esi
	leal	(%rsi,%rsi,4), %r8d
	addq	%rdi, %r8
	addq	%rsi, %rcx
	movl	722280(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%r8, %rdi
	addq	%rsi, %rcx
	movl	722284(%rbx), %esi
	leal	(%rsi,%rsi,4), %r8d
	addq	%rdi, %r8
	addq	%rsi, %rcx
	movl	722288(%rbx), %esi
	leal	(%rsi,%rsi,4), %edi
	addq	%r8, %rdi
	addq	%rsi, %rcx
	addq	%rax, %rcx
	movl	722292(%rbx), %eax
	leal	(%rax,%rax,4), %esi
	addq	%rdi, %rsi
	addq	%rdx, %rsi
	movl	722296(%rbx), %edx
	leal	(%rdx,%rdx,4), %edi
	addq	%rdx, %rax
	movl	722300(%rbx), %edx
	leal	(%rdx,%rdx,4), %r8d
	addq	%rdi, %r8
	addq	%rdx, %rax
	movl	722304(%rbx), %edx
	leal	(%rdx,%rdx,4), %edi
	addq	%r8, %rdi
	addq	%rdx, %rax
	movl	722308(%rbx), %edx
	leal	(%rdx,%rdx,4), %r13d
	addq	%rdi, %r13
	addq	%rsi, %r13
	addq	%rdx, %rax
	addq	%rcx, %rax
	testb	%r14b, %r14b
	je	LBB17_14
## %bb.13:
	movl	804220(%rbx), %ecx
	leal	(%rcx,%r13), %edx
	negl	%edx
	andl	$7, %edx
	addq	%rdx, %r13
	addl	%eax, %ecx
	negl	%ecx
	andl	$7, %ecx
	addq	%rcx, %rax
LBB17_14:
	cmpq	%r13, %r12
	movq	%r13, %rdx
	cmovbq	%r12, %rdx
	cmpl	$4, 4(%rbx)
	movl	$1, %ecx
	je	LBB17_20
## %bb.15:
	cmpq	%rdx, %rax
	jae	LBB17_20
## %bb.16:
	movq	%rbx, %rdi
	callq	__ZN8stx_zlib13DeflateStream15prepare_dynamicEv
	testb	%al, %al
	je	LBB17_37
## %bb.17:
	movq	727136(%rbx), %rax
	testb	%r14b, %r14b
	je	LBB17_19
## %bb.18:
	movl	804220(%rbx), %ecx
	addl	%eax, %ecx
	negl	%ecx
	andl	$7, %ecx
	addq	%rcx, %rax
LBB17_19:
	xorl	%ecx, %ecx
	cmpq	%r13, %rax
	cmovbq	%rax, %r13
	adcl	$1, %ecx
LBB17_20:
	xorl	%edx, %edx
	cmpq	%r13, %r12
	cmoval	%ecx, %edx
	movq	40(%rbx), %rdi
	movq	72(%rbx), %r9
	cmpq	%r9, %rdi
	je	LBB17_35
LBB17_21:
	movzbl	%r14b, %eax
	leal	(%rax,%rdx,2), %r8d
	leaq	1(%r9), %rax
	movq	%rax, 72(%rbx)
	movl	804220(%rbx), %ecx
	shll	%cl, %r8d
	orl	804216(%rbx), %r8d
	movl	%r8d, 804216(%rbx)
	addl	$3, %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$8, %ecx
	jb	LBB17_25
## %bb.22:
	movq	804200(%rbx), %rsi
	.p2align	4
LBB17_23:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rsi                    ## imm = 0x10000
	je	LBB17_34
## %bb.24:                              ##   in Loop: Header=BB17_23 Depth=1
	movb	%r8b, 738664(%rbx,%rsi)
	movq	804200(%rbx), %rsi
	incq	%rsi
	movq	%rsi, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %ecx
	addl	$-8, %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB17_23
LBB17_25:
	cmpl	$1, %edx
	je	LBB17_39
## %bb.26:
	testl	%edx, %edx
	jne	LBB17_45
## %bb.27:
	testl	%ecx, %ecx
	je	LBB17_62
## %bb.28:
	movl	$5, %ecx
	cmpq	%rax, %rdi
	je	LBB17_76
## %bb.29:
	addq	$2, %r9
	movq	%r9, 72(%rbx)
	movl	$8, 804220(%rbx)
	movq	804200(%rbx), %rax
	.p2align	4
LBB17_30:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rax                    ## imm = 0x10000
	je	LBB17_75
## %bb.31:                              ##   in Loop: Header=BB17_30 Depth=1
	movb	%r8b, 738664(%rbx,%rax)
	movq	804200(%rbx), %rax
	incq	%rax
	movq	%rax, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %ecx
	addl	$-8, %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB17_30
## %bb.32:
	movq	%r9, %rax
	jmp	LBB17_63
LBB17_34:
	movl	$6, %r15d
LBB17_35:
	movl	%r15d, 52(%rbx)
LBB17_36:
	movl	$5, 48(%rbx)
LBB17_37:
	xorl	%eax, %eax
LBB17_38:
                                        ## kill: def $al killed $al killed $eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB17_39:
	movq	%rbx, %rdi
	xorl	%esi, %esi
	callq	__ZN8stx_zlib13DeflateStream11emit_tokensEb
	movl	%eax, %ecx
	xorl	%eax, %eax
	testb	%cl, %cl
	je	LBB17_38
LBB17_40:
	testb	%r14b, %r14b
	jne	LBB17_132
## %bb.41:
	movq	65664(%rbx), %rsi
	addq	65656(%rbx), %rsi
	movq	112(%rbx), %r14
	cmpq	%r14, %rsi
	cmovbq	%rsi, %r14
	jbe	LBB17_131
## %bb.42:
	movq	40(%rbx), %rdi
	movq	72(%rbx), %rdx
	movq	%rdi, %rax
	subq	%rdx, %rax
	cmpq	%rax, %r14
	jbe	LBB17_83
LBB17_43:
	movabsq	$21474836485, %rax              ## imm = 0x500000005
LBB17_44:
	movq	%rax, 48(%rbx)
	jmp	LBB17_37
LBB17_45:
	movl	$5, %edx
	cmpq	%rax, %rdi
	je	LBB17_74
## %bb.46:
	movl	727116(%rbx), %edx
	decl	%edx
	leaq	2(%r9), %rax
	movq	%rax, 72(%rbx)
	andl	$31, %edx
	shll	%cl, %edx
	orl	%edx, %r8d
	movl	%r8d, 804216(%rbx)
	leal	5(%rcx), %edx
	movl	%edx, 804220(%rbx)
	cmpl	$3, %ecx
	jb	LBB17_50
## %bb.47:
	movq	804200(%rbx), %rcx
	.p2align	4
LBB17_48:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_73
## %bb.49:                              ##   in Loop: Header=BB17_48 Depth=1
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %edx
	addl	$-8, %edx
	movl	%edx, 804220(%rbx)
	cmpl	$7, %edx
	ja	LBB17_48
LBB17_50:
	movl	$5, %ecx
	cmpq	%rax, %rdi
	je	LBB17_76
## %bb.51:
	movl	727120(%rbx), %eax
	decl	%eax
	leaq	3(%r9), %rsi
	movq	%rsi, 72(%rbx)
	andl	$31, %eax
	movl	%edx, %ecx
	shll	%cl, %eax
	orl	%eax, %r8d
	movl	%r8d, 804216(%rbx)
	leal	5(%rdx), %eax
	movl	%eax, 804220(%rbx)
	cmpl	$3, %edx
	jb	LBB17_55
## %bb.52:
	movq	804200(%rbx), %rcx
LBB17_53:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_75
## %bb.54:                              ##   in Loop: Header=BB17_53 Depth=1
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %eax
	addl	$-8, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$7, %eax
	ja	LBB17_53
LBB17_55:
	movl	$5, %ecx
	cmpq	%rsi, %rdi
	je	LBB17_76
## %bb.56:
	movl	727124(%rbx), %edx
	leal	12(%rdx), %esi
	addq	$4, %r9
	movq	%r9, 72(%rbx)
	andl	$15, %esi
	movl	%eax, %ecx
	shll	%cl, %esi
	orl	%esi, %r8d
	movl	%r8d, 804216(%rbx)
	leal	4(%rax), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$4, %eax
	jb	LBB17_60
## %bb.57:
	movq	804200(%rbx), %rax
LBB17_58:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rax                    ## imm = 0x10000
	je	LBB17_75
## %bb.59:                              ##   in Loop: Header=BB17_58 Depth=1
	movb	%r8b, 738664(%rbx,%rax)
	movq	804200(%rbx), %rax
	incq	%rax
	movq	%rax, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %ecx
	addl	$-8, %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB17_58
LBB17_60:
	testq	%rdx, %rdx
	je	LBB17_91
## %bb.61:
	xorl	%esi, %esi
	leaq	__ZN8stx_zlib12_GLOBAL__N_110code_orderE(%rip), %r10
	jmp	LBB17_78
LBB17_62:
	xorl	%ecx, %ecx
LBB17_63:
	movl	$5, %edx
	cmpq	%rax, %rdi
	je	LBB17_74
## %bb.64:
	movq	65664(%rbx), %r15
	leaq	1(%rax), %rsi
	movq	%rsi, 72(%rbx)
	movzwl	%r15w, %edx
	movl	%edx, %r9d
	shll	%cl, %r9d
	orl	%r9d, %r8d
	movl	%r8d, 804216(%rbx)
	orl	$16, %ecx
	movl	%ecx, 804220(%rbx)
	movq	804200(%rbx), %r10
	.p2align	4
LBB17_65:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r10                    ## imm = 0x10000
	je	LBB17_73
## %bb.66:                              ##   in Loop: Header=BB17_65 Depth=1
	movb	%r8b, 738664(%rbx,%r10)
	movq	804200(%rbx), %r10
	incq	%r10
	movq	%r10, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %r9d
	leal	-8(%r9), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB17_65
## %bb.67:
	movl	$5, %r10d
	cmpq	%rsi, %rdi
	je	LBB17_89
## %bb.68:
	addq	$2, %rax
	movq	%rax, 72(%rbx)
	xorl	$65535, %edx                    ## imm = 0xFFFF
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edx
	orl	%edx, %r8d
	movl	%r8d, 804216(%rbx)
	addl	$8, %r9d
	movl	%r9d, 804220(%rbx)
	movq	804200(%rbx), %rcx
LBB17_69:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_88
## %bb.70:                              ##   in Loop: Header=BB17_69 Depth=1
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rdx
	leaq	1(%rdx), %rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %esi
	addl	$-8, %esi
	movl	%esi, 804220(%rbx)
	cmpl	$7, %esi
	ja	LBB17_69
## %bb.71:
	movl	$65535, %esi                    ## imm = 0xFFFF
	subq	%rdx, %rsi
	cmpq	%rsi, %r15
	jbe	LBB17_107
## %bb.72:
	movabsq	$25769803781, %rax              ## imm = 0x600000005
	jmp	LBB17_44
LBB17_73:
	movl	$6, %edx
LBB17_74:
	movl	%edx, 52(%rbx)
	jmp	LBB17_36
LBB17_75:
	movl	$6, %ecx
LBB17_76:
	movl	%ecx, 52(%rbx)
	jmp	LBB17_36
LBB17_77:                               ##   in Loop: Header=BB17_78 Depth=1
	incq	%rsi
	movl	%eax, %ecx
	cmpq	%rdx, %rsi
	je	LBB17_92
LBB17_78:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB17_81 Depth 2
	cmpq	%r9, %rdi
	je	LBB17_115
## %bb.79:                              ##   in Loop: Header=BB17_78 Depth=1
	movl	(%r10,%rsi,4), %eax
	movzbl	723260(%rbx,%rax), %eax
	incq	%r9
	movq	%r9, 72(%rbx)
	andl	$7, %eax
	shll	%cl, %eax
	orl	%eax, %r8d
	movl	%r8d, 804216(%rbx)
	leal	3(%rcx), %eax
	movl	%eax, 804220(%rbx)
	cmpl	$5, %ecx
	jb	LBB17_77
## %bb.80:                              ##   in Loop: Header=BB17_78 Depth=1
	movq	804200(%rbx), %rcx
LBB17_81:                               ##   Parent Loop BB17_78 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_109
## %bb.82:                              ##   in Loop: Header=BB17_81 Depth=2
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %eax
	addl	$-8, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$7, %eax
	ja	LBB17_81
	jmp	LBB17_77
LBB17_83:
	movq	%rsi, %rax
	subq	%r14, %rax
	addq	%r14, %rdx
	movq	%rdx, 72(%rbx)
	cmpb	$1, 65672(%rbx)
	jne	LBB17_130
## %bb.84:
	movl	84(%rbx), %ecx
	leaq	(%r14,%rcx), %r8
	subq	%rdx, %rdi
	cmpq	%rdi, %r8
	ja	LBB17_43
## %bb.85:
	addq	%rdx, %r8
	movq	%r8, 72(%rbx)
	movl	$65536, %edx                    ## imm = 0x10000
	subq	%rax, %rdx
	cmpq	$-1, %r14
	cmovneq	%r14, %rdx
	testq	%rdx, %rdx
	je	LBB17_122
## %bb.86:
	cmpq	$8, %rdx
	jb	LBB17_116
## %bb.110:
	movq	%r14, %rdi
	subq	%rsi, %rdi
	movabsq	$4611686018427387896, %rsi      ## imm = 0x3FFFFFFFFFFFFFF8
	testq	%rsi, %rdi
	je	LBB17_116
## %bb.111:
	movq	%rdx, %rsi
	andq	$-8, %rsi
	movd	%eax, %xmm0
	pshufd	$0, %xmm0, %xmm0                ## xmm0 = xmm0[0,0,0,0]
	movd	%r14d, %xmm1
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	leaq	327836(%rbx), %rdi
	movdqa	LCPI17_0(%rip), %xmm2           ## xmm2 = [65535,0,65535,0,65535,0,65535,0]
	movdqa	LCPI17_1(%rip), %xmm3           ## xmm3 = [2147483648,2147483648,2147483648,2147483648]
	pxor	%xmm3, %xmm1
	movdqa	LCPI17_2(%rip), %xmm4           ## xmm4 = [0,65535,0,65535,0,65535,0,65535]
	pcmpeqd	%xmm5, %xmm5
	movq	%rsi, %r8
LBB17_112:                              ## =>This Inner Loop Header: Depth=1
	movdqu	-16(%rdi,%rax,4), %xmm9
	movdqu	(%rdi,%rax,4), %xmm7
	movdqa	%xmm9, %xmm6
	pand	%xmm2, %xmm6
	movdqa	%xmm7, %xmm8
	pand	%xmm2, %xmm8
	movdqa	%xmm6, %xmm10
	psubd	%xmm0, %xmm10
	movdqa	%xmm8, %xmm11
	psubd	%xmm0, %xmm11
	pcmpeqd	%xmm2, %xmm6
	pcmpeqd	%xmm2, %xmm8
	pand	%xmm4, %xmm9
	por	%xmm10, %xmm9
	pxor	%xmm3, %xmm10
	movdqa	%xmm1, %xmm12
	pcmpgtd	%xmm10, %xmm12
	pandn	%xmm12, %xmm6
	pand	%xmm4, %xmm7
	por	%xmm11, %xmm7
	pxor	%xmm3, %xmm11
	movdqa	%xmm1, %xmm10
	pcmpgtd	%xmm11, %xmm10
	pandn	%xmm10, %xmm8
	pand	%xmm6, %xmm9
	pxor	%xmm5, %xmm6
	por	%xmm9, %xmm6
	pand	%xmm8, %xmm7
	pxor	%xmm5, %xmm8
	por	%xmm7, %xmm8
	movdqu	%xmm6, -16(%rdi)
	movdqu	%xmm8, (%rdi)
	addq	$32, %rdi
	addq	$-8, %r8
	jne	LBB17_112
## %bb.113:
	cmpq	%rsi, %rdx
	jne	LBB17_117
	jmp	LBB17_122
LBB17_88:
	movl	$6, %r10d
LBB17_89:
	movl	%r10d, 52(%rbx)
	jmp	LBB17_36
LBB17_91:
	movl	%ecx, %eax
LBB17_92:
	movl	727112(%rbx), %ecx
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	testq	%rcx, %rcx
	je	LBB17_105
## %bb.93:
	xorl	%r15d, %r15d
LBB17_94:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB17_97 Depth 2
                                        ##     Child Loop BB17_102 Depth 2
	cmpq	%r9, %rdi
	je	LBB17_115
## %bb.95:                              ##   in Loop: Header=BB17_94 Depth=1
	leaq	(%r15,%r15,2), %rcx
	leaq	723320(%rbx), %rdx
	leaq	(%rdx,%rcx,4), %rsi
	movl	4(%rdx,%rcx,4), %r12d
	movl	8(%rdx,%rcx,4), %edx
	movl	(%rsi), %ecx
	movzbl	723260(%rbx,%rcx), %esi
	movzwl	723280(%rbx,%rcx,2), %r11d
	leaq	1(%r9), %r13
	movl	$-1, %r10d
	movl	%esi, %ecx
	shll	%cl, %r10d
	movq	%r13, 72(%rbx)
	notl	%r10d
	andl	%r11d, %r10d
	movl	%eax, %ecx
	shll	%cl, %r10d
	orl	%r10d, %r8d
	movl	%r8d, 804216(%rbx)
	addl	%esi, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$8, %eax
	jb	LBB17_99
## %bb.96:                              ##   in Loop: Header=BB17_94 Depth=1
	movq	804200(%rbx), %rcx
LBB17_97:                               ##   Parent Loop BB17_94 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_109
## %bb.98:                              ##   in Loop: Header=BB17_97 Depth=2
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %eax
	addl	$-8, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$7, %eax
	ja	LBB17_97
LBB17_99:                               ##   in Loop: Header=BB17_94 Depth=1
	cmpq	%r13, %rdi
	je	LBB17_115
## %bb.100:                             ##   in Loop: Header=BB17_94 Depth=1
	addq	$2, %r9
	movl	$-1, %esi
	movl	%edx, %ecx
	shll	%cl, %esi
	movq	%r9, 72(%rbx)
	notl	%esi
	andl	%esi, %r12d
	movl	%eax, %ecx
	shll	%cl, %r12d
	orl	%r12d, %r8d
	movl	%r8d, 804216(%rbx)
	addl	%edx, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$8, %eax
	jb	LBB17_104
## %bb.101:                             ##   in Loop: Header=BB17_94 Depth=1
	movq	804200(%rbx), %rcx
LBB17_102:                              ##   Parent Loop BB17_94 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB17_109
## %bb.103:                             ##   in Loop: Header=BB17_102 Depth=2
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %eax
	addl	$-8, %eax
	movl	%eax, 804220(%rbx)
	cmpl	$7, %eax
	ja	LBB17_102
LBB17_104:                              ##   in Loop: Header=BB17_94 Depth=1
	incq	%r15
	cmpq	-48(%rbp), %r15                 ## 8-byte Folded Reload
	jne	LBB17_94
LBB17_105:
	movq	%rbx, %rdi
	movl	$1, %esi
	callq	__ZN8stx_zlib13DeflateStream11emit_tokensEb
	testb	%al, %al
	je	LBB17_37
	jmp	LBB17_40
LBB17_109:
	movl	$6, %eax
	movl	%eax, 52(%rbx)
	jmp	LBB17_36
LBB17_107:
	subq	%rax, %rdi
	cmpq	%rdi, %r15
	ja	LBB17_43
## %bb.108:
	leaq	738664(%rbx), %rdi
	addq	%r15, %rax
	movq	%rax, 72(%rbx)
	addq	%rcx, %rdi
	movq	65656(%rbx), %rax
	leaq	(%rbx,%rax), %rsi
	addq	$120, %rsi
	movq	%r15, %rdx
	callq	_memcpy
	addq	%r15, 804200(%rbx)
	jmp	LBB17_40
LBB17_116:
	xorl	%esi, %esi
LBB17_117:
	movq	%rsi, %rdi
	orq	$1, %rdi
	testb	$1, %dl
	je	LBB17_119
## %bb.118:
	leaq	327820(%rbx), %r8
	leaq	(%rbx,%rax,4), %r9
	addq	$327820, %r9                    ## imm = 0x5008C
	movl	(%r9,%rsi,4), %r9d
	movzwl	%r9w, %r10d
	movl	%r10d, %r11d
	subl	%eax, %r11d
	andl	$-65536, %r9d                   ## imm = 0xFFFF0000
	orl	%r11d, %r9d
	cmpl	%r14d, %r11d
	movl	$-1, %r11d
	cmovael	%r11d, %r9d
	cmpl	$65535, %r10d                   ## imm = 0xFFFF
	cmovel	%r11d, %r9d
	movl	%r9d, (%r8,%rsi,4)
	movq	%rdi, %rsi
LBB17_119:
	cmpq	%rdi, %rdx
	je	LBB17_122
## %bb.120:
	subq	%rsi, %rdx
	leaq	(%rbx,%rsi,4), %rsi
	addq	$327824, %rsi                   ## imm = 0x50090
	movl	$-1, %edi
LBB17_121:                              ## =>This Inner Loop Header: Depth=1
	movl	-4(%rsi,%rax,4), %r8d
	movzwl	%r8w, %r9d
	movl	%r9d, %r10d
	subl	%eax, %r10d
	andl	$-65536, %r8d                   ## imm = 0xFFFF0000
	orl	%r10d, %r8d
	cmpl	%r14d, %r10d
	cmovael	%edi, %r8d
	cmpl	$65535, %r9d                    ## imm = 0xFFFF
	cmovel	%edi, %r8d
	movl	%r8d, -4(%rsi)
	movl	(%rsi,%rax,4), %r8d
	movzwl	%r8w, %r9d
	movl	%r9d, %r10d
	subl	%eax, %r10d
	andl	$-65536, %r8d                   ## imm = 0xFFFF0000
	orl	%r10d, %r8d
	cmpl	%r14d, %r10d
	cmovael	%edi, %r8d
	cmpl	$65535, %r9d                    ## imm = 0xFFFF
	cmovel	%edi, %r8d
	movl	%r8d, (%rsi)
	addq	$8, %rsi
	addq	$-2, %rdx
	jne	LBB17_121
LBB17_122:
	testq	%rcx, %rcx
	je	LBB17_130
## %bb.123:
	cmpl	$8, %ecx
	jae	LBB17_125
## %bb.124:
	xorl	%edx, %edx
	jmp	LBB17_128
LBB17_125:
	movl	%ecx, %edx
	andl	$-8, %edx
	movd	%eax, %xmm0
	pshufd	$0, %xmm0, %xmm0                ## xmm0 = xmm0[0,0,0,0]
	movd	%r14d, %xmm1
	pshufd	$0, %xmm1, %xmm1                ## xmm1 = xmm1[0,0,0,0]
	leaq	(,%rcx,4), %rsi
	andq	$-32, %rsi
	xorl	%edi, %edi
	movdqa	LCPI17_0(%rip), %xmm2           ## xmm2 = [65535,0,65535,0,65535,0,65535,0]
	movdqa	LCPI17_1(%rip), %xmm3           ## xmm3 = [2147483648,2147483648,2147483648,2147483648]
	pxor	%xmm3, %xmm1
	movdqa	LCPI17_2(%rip), %xmm4           ## xmm4 = [0,65535,0,65535,0,65535,0,65535]
	pcmpeqd	%xmm5, %xmm5
LBB17_126:                              ## =>This Inner Loop Header: Depth=1
	movdqu	65676(%rbx,%rdi), %xmm9
	movdqu	65692(%rbx,%rdi), %xmm7
	movdqa	%xmm9, %xmm6
	pand	%xmm2, %xmm6
	movdqa	%xmm7, %xmm8
	pand	%xmm2, %xmm8
	movdqa	%xmm6, %xmm10
	psubd	%xmm0, %xmm10
	movdqa	%xmm8, %xmm11
	psubd	%xmm0, %xmm11
	pcmpeqd	%xmm2, %xmm6
	pcmpeqd	%xmm2, %xmm8
	pand	%xmm4, %xmm9
	por	%xmm10, %xmm9
	pxor	%xmm3, %xmm10
	movdqa	%xmm1, %xmm12
	pcmpgtd	%xmm10, %xmm12
	pandn	%xmm12, %xmm6
	pand	%xmm4, %xmm7
	por	%xmm11, %xmm7
	pxor	%xmm3, %xmm11
	movdqa	%xmm1, %xmm10
	pcmpgtd	%xmm11, %xmm10
	pandn	%xmm10, %xmm8
	pand	%xmm6, %xmm9
	pxor	%xmm5, %xmm6
	por	%xmm9, %xmm6
	pand	%xmm8, %xmm7
	pxor	%xmm5, %xmm8
	por	%xmm7, %xmm8
	movdqu	%xmm6, 65676(%rbx,%rdi)
	movdqu	%xmm8, 65692(%rbx,%rdi)
	addq	$32, %rdi
	cmpq	%rdi, %rsi
	jne	LBB17_126
## %bb.127:
	cmpl	%ecx, %edx
	je	LBB17_130
LBB17_128:
	movl	$-1, %esi
LBB17_129:                              ## =>This Inner Loop Header: Depth=1
	movl	65676(%rbx,%rdx,4), %edi
	movzwl	%di, %r8d
	movl	%r8d, %r9d
	subl	%eax, %r9d
	andl	$-65536, %edi                   ## imm = 0xFFFF0000
	orl	%r9d, %edi
	cmpl	%r14d, %r9d
	cmovael	%esi, %edi
	cmpl	$65535, %r8d                    ## imm = 0xFFFF
	cmovel	%esi, %edi
	movl	%edi, 65676(%rbx,%rdx,4)
	incq	%rdx
	cmpq	%rdx, %rcx
	jne	LBB17_129
LBB17_130:
	leaq	120(%rbx), %rdi
	leaq	(%rbx,%rax), %rsi
	addq	$120, %rsi
	movq	%r14, %rdx
	callq	_memmove
LBB17_131:
	movq	%r14, 65656(%rbx)
LBB17_132:
	movq	$0, 65664(%rbx)
	movb	$1, %al
	jmp	LBB17_38
LBB17_115:
	movl	$5, %eax
	movl	%eax, 52(%rbx)
	jmp	LBB17_36
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE ## -- Begin function _ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
	.p2align	4
__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE: ## @_ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
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
	pushq	%rax
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rdi, %rbx
	cmpl	$2, %esi
	jne	LBB18_25
## %bb.1:
	movq	%rbx, %rdi
	movl	$1, %esi
	callq	__ZN8stx_zlib13DeflateStream12encode_blockEb
	movl	%eax, %edx
	xorl	%ecx, %ecx
	movl	$0, %eax
	testb	%dl, %dl
	je	LBB18_60
## %bb.2:
	cmpl	$0, 804220(%rbx)
	je	LBB18_7
## %bb.3:
	movq	72(%rbx), %rax
	movl	$5, %r12d
	movl	$5, %ecx
	cmpq	%rax, 40(%rbx)
	je	LBB18_48
## %bb.4:
	incq	%rax
	movq	%rax, 72(%rbx)
	movl	804216(%rbx), %esi
	movl	$8, 804220(%rbx)
	movq	804200(%rbx), %rax
	.p2align	4
LBB18_5:                                ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rax                    ## imm = 0x10000
	je	LBB18_47
## %bb.6:                               ##   in Loop: Header=BB18_5 Depth=1
	movb	%sil, 738664(%rbx,%rax)
	movq	804200(%rbx), %rax
	incq	%rax
	movq	%rax, 804200(%rbx)
	movl	804216(%rbx), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rbx)
	movl	804220(%rbx), %ecx
	addl	$-8, %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB18_5
LBB18_7:
	movb	$1, %al
	movl	$3, %r12d
	cmpl	$0, (%rbx)
	jne	LBB18_59
## %bb.8:
	movq	40(%rbx), %rsi
	movq	72(%rbx), %r9
	movl	$5, %r12d
	movl	$5, %r10d
	cmpq	%r9, %rsi
	je	LBB18_56
## %bb.9:
	movl	804228(%rbx), %r11d
	shll	$16, %r11d
	movl	804224(%rbx), %edx
	movl	%edx, %r13d
	orl	%edx, %r11d
	movl	%r11d, %r8d
	shrl	$24, %r8d
	shll	%cl, %r8d
	leaq	4(%r9), %rdx
	leaq	1(%r9), %r14
	orl	804216(%rbx), %r8d
	movl	%r8d, 804216(%rbx)
	orl	$8, %ecx
	movl	%ecx, 804220(%rbx)
	movq	804200(%rbx), %r10
	.p2align	4
LBB18_10:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r10                    ## imm = 0x10000
	je	LBB18_53
## %bb.11:                              ##   in Loop: Header=BB18_10 Depth=1
	movb	%r8b, 738664(%rbx,%r10)
	movq	804200(%rbx), %r10
	incq	%r10
	movq	%r10, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %r15d
	leal	-8(%r15), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB18_10
## %bb.12:
	movl	$5, %r10d
	cmpq	%r14, %rsi
	je	LBB18_55
## %bb.13:
	shrl	$16, %r11d
	movzbl	%r11b, %r14d
	leaq	2(%r9), %r11
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r14d
	orl	%r14d, %r8d
	movl	%r8d, 804216(%rbx)
	movl	%r15d, 804220(%rbx)
	movq	804200(%rbx), %r14
LBB18_14:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r14                    ## imm = 0x10000
	je	LBB18_61
## %bb.15:                              ##   in Loop: Header=BB18_14 Depth=1
	movb	%r8b, 738664(%rbx,%r14)
	movq	804200(%rbx), %r14
	incq	%r14
	movq	%r14, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %r15d
	leal	-8(%r15), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB18_14
## %bb.16:
	cmpq	%r11, %rsi
	je	LBB18_55
## %bb.17:
	movq	%rdx, %r11
	movl	%r13d, %edx
	movzbl	%dh, %edi
	movq	%r11, %rdx
	addq	$3, %r9
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edi
	orl	%edi, %r8d
	movl	%r8d, 804216(%rbx)
	movl	%r15d, 804220(%rbx)
	movq	804200(%rbx), %r11
LBB18_18:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r11                    ## imm = 0x10000
	je	LBB18_62
## %bb.19:                              ##   in Loop: Header=BB18_18 Depth=1
	movb	%r8b, 738664(%rbx,%r11)
	movq	804200(%rbx), %r11
	incq	%r11
	movq	%r11, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %r14d
	leal	-8(%r14), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB18_18
## %bb.20:
	cmpq	%r9, %rsi
	je	LBB18_55
## %bb.21:
	movzbl	%r13b, %esi
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %esi
	orl	%esi, %r8d
	movl	%r8d, 804216(%rbx)
	movl	%r14d, 804220(%rbx)
	movq	804200(%rbx), %rcx
LBB18_22:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB18_63
## %bb.23:                              ##   in Loop: Header=BB18_22 Depth=1
	movb	%r8b, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %r8d
	shrl	$8, %r8d
	movl	%r8d, 804216(%rbx)
	movl	804220(%rbx), %esi
	addl	$-8, %esi
	movl	%esi, 804220(%rbx)
	cmpl	$7, %esi
	ja	LBB18_22
## %bb.24:
	movq	%rdx, 72(%rbx)
	movl	$3, %r12d
	jmp	LBB18_59
LBB18_25:
	cmpq	$0, 65664(%rbx)
	je	LBB18_27
## %bb.26:
	movq	%rbx, %rdi
	xorl	%esi, %esi
	callq	__ZN8stx_zlib13DeflateStream12encode_blockEb
	movl	%eax, %ecx
	xorl	%eax, %eax
	testb	%cl, %cl
	je	LBB18_60
LBB18_27:
	movq	40(%rbx), %rax
	movq	72(%rbx), %rcx
	movl	$5, %r12d
	movl	$5, %esi
	cmpq	%rcx, %rax
	je	LBB18_50
## %bb.28:
	leaq	1(%rcx), %rdi
	movq	%rdi, 72(%rbx)
	movl	804216(%rbx), %esi
	movl	804220(%rbx), %r9d
	addl	$3, %r9d
	movl	%r9d, 804220(%rbx)
	cmpl	$8, %r9d
	jb	LBB18_32
## %bb.29:
	movq	804200(%rbx), %r8
	.p2align	4
LBB18_30:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r8                     ## imm = 0x10000
	je	LBB18_49
## %bb.31:                              ##   in Loop: Header=BB18_30 Depth=1
	movb	%sil, 738664(%rbx,%r8)
	movq	804200(%rbx), %r8
	incq	%r8
	movq	%r8, 804200(%rbx)
	movl	804216(%rbx), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rbx)
	movl	804220(%rbx), %r9d
	addl	$-8, %r9d
	movl	%r9d, 804220(%rbx)
	cmpl	$7, %r9d
	ja	LBB18_30
LBB18_32:
	testl	%r9d, %r9d
	je	LBB18_38
## %bb.33:
	movl	$5, %r12d
	movl	$5, %r8d
	cmpq	%rdi, %rax
	je	LBB18_52
## %bb.34:
	addq	$2, %rcx
	movq	%rcx, 72(%rbx)
	movl	$8, 804220(%rbx)
	movq	804200(%rbx), %rdi
	.p2align	4
LBB18_35:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rdi                    ## imm = 0x10000
	je	LBB18_51
## %bb.36:                              ##   in Loop: Header=BB18_35 Depth=1
	movb	%sil, 738664(%rbx,%rdi)
	movq	804200(%rbx), %rdi
	incq	%rdi
	movq	%rdi, 804200(%rbx)
	movl	804216(%rbx), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rbx)
	movl	804220(%rbx), %r9d
	leal	-8(%r9), %r8d
	movl	%r8d, 804220(%rbx)
	cmpl	$7, %r8d
	ja	LBB18_35
## %bb.37:
	addl	$8, %r9d
	movq	%rcx, %rdi
	jmp	LBB18_39
LBB18_38:
	movl	$16, %r9d
LBB18_39:
	movl	$5, %r12d
	movl	$5, %ecx
	cmpq	%rdi, %rax
	je	LBB18_48
## %bb.40:
	leaq	1(%rdi), %r8
	movq	%r8, 72(%rbx)
	movl	%r9d, 804220(%rbx)
	movq	804200(%rbx), %r10
	.p2align	4
LBB18_41:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %r10                    ## imm = 0x10000
	je	LBB18_47
## %bb.42:                              ##   in Loop: Header=BB18_41 Depth=1
	movb	%sil, 738664(%rbx,%r10)
	movq	804200(%rbx), %r10
	incq	%r10
	movq	%r10, 804200(%rbx)
	movl	804216(%rbx), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rbx)
	movl	804220(%rbx), %r9d
	leal	-8(%r9), %ecx
	movl	%ecx, 804220(%rbx)
	cmpl	$7, %ecx
	ja	LBB18_41
## %bb.43:
	movl	$5, %edx
	cmpq	%r8, %rax
	je	LBB18_58
## %bb.44:
	addq	$2, %rdi
	movq	%rdi, 72(%rbx)
	movl	$65535, %eax                    ## imm = 0xFFFF
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %eax
	orl	%eax, %esi
	movl	%esi, 804216(%rbx)
	addl	$8, %r9d
	movl	%r9d, 804220(%rbx)
	movq	804200(%rbx), %rcx
	movl	$2, %r12d
LBB18_45:                               ## =>This Inner Loop Header: Depth=1
	cmpq	$65536, %rcx                    ## imm = 0x10000
	je	LBB18_57
## %bb.46:                              ##   in Loop: Header=BB18_45 Depth=1
	movb	%sil, 738664(%rbx,%rcx)
	movq	804200(%rbx), %rcx
	incq	%rcx
	movq	%rcx, 804200(%rbx)
	movl	804216(%rbx), %esi
	shrl	$8, %esi
	movl	%esi, 804216(%rbx)
	movl	804220(%rbx), %edi
	addl	$-8, %edi
	movl	%edi, 804220(%rbx)
	movb	$1, %al
	cmpl	$7, %edi
	ja	LBB18_45
	jmp	LBB18_59
LBB18_47:
	movl	$6, %ecx
LBB18_48:
	movl	%ecx, 52(%rbx)
	xorl	%eax, %eax
	jmp	LBB18_59
LBB18_49:
	movl	$6, %esi
LBB18_50:
	movl	%esi, 52(%rbx)
	xorl	%eax, %eax
	jmp	LBB18_59
LBB18_51:
	movl	$6, %r8d
LBB18_52:
	movl	%r8d, 52(%rbx)
	xorl	%eax, %eax
	jmp	LBB18_59
LBB18_53:
	movq	%r14, %rsi
LBB18_54:
	movl	$6, %r10d
LBB18_55:
	movq	%rsi, 72(%rbx)
LBB18_56:
	movl	%r10d, 52(%rbx)
	xorl	%eax, %eax
	jmp	LBB18_59
LBB18_57:
	movl	$6, %edx
LBB18_58:
	movl	%edx, 52(%rbx)
	xorl	%eax, %eax
	movl	$5, %r12d
LBB18_59:
	movl	%r12d, 48(%rbx)
LBB18_60:
                                        ## kill: def $al killed $al killed $eax
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB18_61:
	movq	%r11, %rsi
	jmp	LBB18_54
LBB18_62:
	movq	%r9, %rsi
	jmp	LBB18_54
LBB18_63:
	movq	%rdx, %rsi
	jmp	LBB18_54
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
LCPI19_0:
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	3                               ## 0x3
LCPI19_1:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI19_2:
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
	.p2align	4
__ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE: ## @_ZN8stx_zlib13DeflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS2_IS3_Lm18446744073709551615EEENS_12DeflateFlushE
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
	subq	$104, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	movq	%rsi, %r14
	movq	%rdi, %rbx
	movl	48(%rsi), %r15d
	leal	-4(%r15), %eax
	cmpl	$2, %eax
	jae	LBB19_1
LBB19_13:
	movl	%r15d, %eax
	andl	$-2, %eax
	cmpl	$4, %eax
	jne	LBB19_15
## %bb.14:
	xorl	%edx, %edx
	xorl	%r13d, %r13d
	xorl	%r12d, %r12d
LBB19_56:
	cmpl	$5, %r15d
	movl	$4, %eax
	cmovnel	%r13d, %eax
	cmpl	$4, %r15d
	movl	$3, %r15d
	cmovnel	%eax, %r15d
	jmp	LBB19_60
LBB19_1:
	testl	%r15d, %r15d
	jne	LBB19_4
## %bb.2:
	movl	$2, 52(%r14)
	jmp	LBB19_3
LBB19_15:
	movq	%r8, -136(%rbp)                 ## 8-byte Spill
	movq	%r9, -88(%rbp)                  ## 8-byte Spill
	movq	%rdx, -128(%rbp)                ## 8-byte Spill
	leaq	4(%rdx), %rax
	movq	%rax, -120(%rbp)                ## 8-byte Spill
	xorl	%edx, %edx
	pxor	%xmm11, %xmm11
	xorl	%r12d, %r12d
LBB19_16:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB19_17 Depth 2
                                        ##     Child Loop BB19_46 Depth 2
                                        ##       Child Loop BB19_50 Depth 3
                                        ##       Child Loop BB19_53 Depth 3
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	jmp	LBB19_17
	.p2align	4
LBB19_27:                               ##   in Loop: Header=BB19_17 Depth=2
	pxor	%xmm11, %xmm11
LBB19_28:                               ##   in Loop: Header=BB19_17 Depth=2
	movl	48(%r14), %r15d
	movl	%r15d, %eax
	andl	$-2, %eax
	cmpl	$4, %eax
	je	LBB19_29
LBB19_17:                               ##   Parent Loop BB19_16 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	804200(%r14), %r13
	movq	804208(%r14), %rsi
	subq	%rsi, %r13
	je	LBB19_31
## %bb.18:                              ##   in Loop: Header=BB19_17 Depth=2
	movq	32(%r14), %rax
	subq	64(%r14), %rax
	je	LBB19_19
## %bb.21:                              ##   in Loop: Header=BB19_17 Depth=2
	movq	-88(%rbp), %rcx                 ## 8-byte Reload
	subq	%r12, %rcx
	je	LBB19_22
## %bb.24:                              ##   in Loop: Header=BB19_17 Depth=2
	cmpq	%r13, %rcx
	cmovbq	%rcx, %r13
	cmpq	%rax, %r13
	cmovaeq	%rax, %r13
	movq	40(%r14), %rcx
	movq	72(%r14), %rax
	subq	%rax, %rcx
	cmpq	%rcx, %r13
	ja	LBB19_25
## %bb.26:                              ##   in Loop: Header=BB19_17 Depth=2
	addq	%r13, %rax
	movq	%rax, 72(%r14)
	movq	-136(%rbp), %rax                ## 8-byte Reload
	leaq	(%rax,%r12), %rdi
	leaq	738664(%r14), %rax
	addq	%rax, %rsi
	movq	%r13, %rdx
	callq	_memcpy
	movq	804208(%r14), %rax
	addq	%r13, %rax
	movq	%rax, 804208(%r14)
	addq	%r13, 64(%r14)
	addq	%r13, %r12
	cmpq	804200(%r14), %rax
	jne	LBB19_27
## %bb.30:                              ##   in Loop: Header=BB19_17 Depth=2
	movl	48(%r14), %r15d
	pxor	%xmm11, %xmm11
LBB19_31:                               ##   in Loop: Header=BB19_17 Depth=2
	leaq	804200(%r14), %rax
	movdqu	%xmm11, (%rax)
	cmpl	$2, %r15d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	je	LBB19_57
## %bb.32:                              ##   in Loop: Header=BB19_17 Depth=2
	cmpl	$3, %r15d
	je	LBB19_58
## %bb.33:                              ##   in Loop: Header=BB19_17 Depth=2
	movq	65664(%r14), %rax
	cmpq	$32768, %rax                    ## imm = 0x8000
	jne	LBB19_40
## %bb.34:                              ##   in Loop: Header=BB19_17 Depth=2
	cmpq	%rdx, -56(%rbp)                 ## 8-byte Folded Reload
	je	LBB19_37
## %bb.35:                              ##   in Loop: Header=BB19_17 Depth=2
	xorl	%r13d, %r13d
	movq	%r14, %rdi
	xorl	%esi, %esi
	callq	__ZN8stx_zlib13DeflateStream12encode_blockEb
	pxor	%xmm11, %xmm11
	testb	%al, %al
	jne	LBB19_28
	jmp	LBB19_36
	.p2align	4
LBB19_40:                               ##   in Loop: Header=BB19_17 Depth=2
	movq	-56(%rbp), %r13                 ## 8-byte Reload
	subq	%rdx, %r13
	jne	LBB19_41
LBB19_37:                               ##   in Loop: Header=BB19_17 Depth=2
	xorl	%r13d, %r13d
	movl	16(%rbp), %esi
	testl	%esi, %esi
	je	LBB19_39
## %bb.38:                              ##   in Loop: Header=BB19_17 Depth=2
	movq	%r14, %rdi
	callq	__ZN8stx_zlib13DeflateStream15finish_boundaryENS_12DeflateFlushE
	pxor	%xmm11, %xmm11
	testb	%al, %al
	jne	LBB19_28
	jmp	LBB19_39
LBB19_41:                               ##   in Loop: Header=BB19_16 Depth=1
	movq	24(%r14), %rcx
	movq	56(%r14), %rdi
	subq	%rdi, %rcx
	je	LBB19_42
## %bb.43:                              ##   in Loop: Header=BB19_16 Depth=1
	movl	$32768, %edx                    ## imm = 0x8000
	subq	%rax, %rdx
	cmpq	%rdx, %r13
	cmovbq	%r13, %rdx
	cmpq	%rcx, %rdx
	cmovaeq	%rcx, %rdx
	leaq	(%rdx,%rdx), %rcx
	movq	40(%r14), %rsi
	movq	72(%r14), %r8
	subq	%r8, %rsi
	cmpq	%rsi, %rcx
	ja	LBB19_25
## %bb.44:                              ##   in Loop: Header=BB19_16 Depth=1
	movq	%rdi, -96(%rbp)                 ## 8-byte Spill
	addq	%rcx, %r8
	movq	%r8, 72(%r14)
	movq	65656(%r14), %rdi
	leaq	120(%r14), %rcx
	addq	%rcx, %rdi
	addq	%rax, %rdi
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	movq	-128(%rbp), %rcx                ## 8-byte Reload
	leaq	(%rcx,%rax), %rsi
	movq	%rsi, -112(%rbp)                ## 8-byte Spill
	movq	%rdx, -80(%rbp)                 ## 8-byte Spill
	callq	_memcpy
	movdqa	LCPI19_2(%rip), %xmm13          ## xmm13 = [8,8,8,8]
	movdqa	LCPI19_1(%rip), %xmm12          ## xmm12 = [4,4,4,4]
	pxor	%xmm11, %xmm11
	cmpl	$0, (%r14)
	je	LBB19_45
LBB19_55:                               ##   in Loop: Header=BB19_16 Depth=1
	movq	-80(%rbp), %rcx                 ## 8-byte Reload
	addq	%rcx, 65664(%r14)
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	addq	%rcx, %rdx
	movq	-96(%rbp), %rax                 ## 8-byte Reload
	addq	%rcx, %rax
	movq	%rax, 56(%r14)
	movl	%r15d, %eax
	andl	$-2, %eax
	xorl	%r13d, %r13d
	cmpl	$4, %eax
	jne	LBB19_16
	jmp	LBB19_56
LBB19_45:                               ##   in Loop: Header=BB19_16 Depth=1
	movq	-80(%rbp), %rax                 ## 8-byte Reload
	cmpq	$-1, %rax
	cmovneq	%rax, %r13
	movl	804224(%r14), %edx
	movl	804228(%r14), %edi
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	movq	-120(%rbp), %rcx                ## 8-byte Reload
	addq	%rcx, %rax
	movq	%rax, -104(%rbp)                ## 8-byte Spill
	xorl	%ecx, %ecx
	jmp	LBB19_46
	.p2align	4
LBB19_54:                               ##   in Loop: Header=BB19_46 Depth=2
	addl	%r10d, %edi
	movl	-68(%rbp), %ecx                 ## 4-byte Reload
	addl	%ecx, %r9d
	movl	%edx, %eax
	imull	%ecx, %eax
	addl	%edi, %eax
	movq	%r9, %rsi
	movl	$2147975281, %ecx               ## imm = 0x80078071
	imulq	%rcx, %rsi
	shrq	$47, %rsi
	imull	$65521, %esi, %esi              ## imm = 0xFFF1
	subl	%esi, %r9d
	movl	%r9d, 804224(%r14)
	movq	%rax, %rsi
	imulq	%rcx, %rsi
	shrq	$47, %rsi
	imull	$65521, %esi, %esi              ## imm = 0xFFF1
	subl	%esi, %eax
	movl	%eax, 804228(%r14)
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	addq	%rdx, %rcx
	movl	%r9d, %edx
	movl	%eax, %edi
	cmpq	%r13, %rcx
	jae	LBB19_55
LBB19_46:                               ##   Parent Loop BB19_16 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB19_50 Depth 3
                                        ##       Child Loop BB19_53 Depth 3
	movl	%edx, -68(%rbp)                 ## 4-byte Spill
	movq	%r13, %rdx
	subq	%rcx, %rdx
	cmpq	$5552, %rdx                     ## imm = 0x15B0
	movl	$5552, %eax                     ## imm = 0x15B0
	cmovaeq	%rax, %rdx
	xorl	%r10d, %r10d
	movq	%r13, %rax
	movl	$0, %r9d
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	subq	%rcx, %rax
	je	LBB19_54
## %bb.47:                              ##   in Loop: Header=BB19_46 Depth=2
	cmpq	$8, %rax
	jae	LBB19_49
## %bb.48:                              ##   in Loop: Header=BB19_46 Depth=2
	xorl	%r9d, %r9d
	xorl	%r8d, %r8d
	xorl	%r10d, %r10d
	jmp	LBB19_52
LBB19_49:                               ##   in Loop: Header=BB19_46 Depth=2
	movl	%edx, %r8d
	andl	$8184, %r8d                     ## imm = 0x1FF8
	movd	%edx, %xmm0
	pshufd	$0, %xmm0, %xmm3                ## xmm3 = xmm0[0,0,0,0]
	movq	-104(%rbp), %rax                ## 8-byte Reload
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	leaq	(%rax,%rcx), %r9
	pxor	%xmm2, %xmm2
	xorl	%r10d, %r10d
	pxor	%xmm4, %xmm4
	pxor	%xmm1, %xmm1
	pxor	%xmm0, %xmm0
	movdqa	LCPI19_0(%rip), %xmm5           ## xmm5 = [0,1,2,3]
	.p2align	4
LBB19_50:                               ##   Parent Loop BB19_16 Depth=1
                                        ##     Parent Loop BB19_46 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movd	-4(%r9,%r10), %xmm7             ## xmm7 = mem[0],zero,zero,zero
	movd	(%r9,%r10), %xmm6               ## xmm6 = mem[0],zero,zero,zero
	punpcklbw	%xmm11, %xmm7           ## xmm7 = xmm7[0],xmm11[0],xmm7[1],xmm11[1],xmm7[2],xmm11[2],xmm7[3],xmm11[3],xmm7[4],xmm11[4],xmm7[5],xmm11[5],xmm7[6],xmm11[6],xmm7[7],xmm11[7]
	punpcklwd	%xmm11, %xmm7           ## xmm7 = xmm7[0],xmm11[0],xmm7[1],xmm11[1],xmm7[2],xmm11[2],xmm7[3],xmm11[3]
	punpcklbw	%xmm11, %xmm6           ## xmm6 = xmm6[0],xmm11[0],xmm6[1],xmm11[1],xmm6[2],xmm11[2],xmm6[3],xmm11[3],xmm6[4],xmm11[4],xmm6[5],xmm11[5],xmm6[6],xmm11[6],xmm6[7],xmm11[7]
	punpcklwd	%xmm11, %xmm6           ## xmm6 = xmm6[0],xmm11[0],xmm6[1],xmm11[1],xmm6[2],xmm11[2],xmm6[3],xmm11[3]
	paddd	%xmm7, %xmm4
	paddd	%xmm6, %xmm2
	movdqa	%xmm3, %xmm8
	psubd	%xmm5, %xmm8
	movdqa	%xmm8, %xmm9
	psubd	%xmm12, %xmm9
	pshufd	$245, %xmm8, %xmm10             ## xmm10 = xmm8[1,1,3,3]
	pmuludq	%xmm7, %xmm8
	pshufd	$232, %xmm8, %xmm8              ## xmm8 = xmm8[0,2,2,3]
	pshufd	$245, %xmm7, %xmm7              ## xmm7 = xmm7[1,1,3,3]
	pmuludq	%xmm10, %xmm7
	pshufd	$232, %xmm7, %xmm7              ## xmm7 = xmm7[0,2,2,3]
	punpckldq	%xmm7, %xmm8            ## xmm8 = xmm8[0],xmm7[0],xmm8[1],xmm7[1]
	paddd	%xmm8, %xmm1
	pshufd	$245, %xmm9, %xmm7              ## xmm7 = xmm9[1,1,3,3]
	pmuludq	%xmm6, %xmm9
	pshufd	$232, %xmm9, %xmm8              ## xmm8 = xmm9[0,2,2,3]
	pshufd	$245, %xmm6, %xmm6              ## xmm6 = xmm6[1,1,3,3]
	pmuludq	%xmm7, %xmm6
	pshufd	$232, %xmm6, %xmm6              ## xmm6 = xmm6[0,2,2,3]
	punpckldq	%xmm6, %xmm8            ## xmm8 = xmm8[0],xmm6[0],xmm8[1],xmm6[1]
	paddd	%xmm8, %xmm0
	addq	$8, %r10
	paddd	%xmm13, %xmm5
	cmpq	%r10, %r8
	jne	LBB19_50
## %bb.51:                              ##   in Loop: Header=BB19_46 Depth=2
	paddd	%xmm4, %xmm2
	pshufd	$238, %xmm2, %xmm3              ## xmm3 = xmm2[2,3,2,3]
	paddd	%xmm2, %xmm3
	pshufd	$85, %xmm3, %xmm2               ## xmm2 = xmm3[1,1,1,1]
	paddd	%xmm3, %xmm2
	movd	%xmm2, %r9d
	paddd	%xmm1, %xmm0
	pshufd	$238, %xmm0, %xmm1              ## xmm1 = xmm0[2,3,2,3]
	paddd	%xmm0, %xmm1
	pshufd	$85, %xmm1, %xmm0               ## xmm0 = xmm1[1,1,1,1]
	paddd	%xmm1, %xmm0
	movd	%xmm0, %r10d
	cmpq	%r8, %rdx
	je	LBB19_54
LBB19_52:                               ##   in Loop: Header=BB19_46 Depth=2
	movq	%rdx, %r11
	subq	%r8, %r11
	movl	%edx, %eax
	subl	%r8d, %eax
	addq	-64(%rbp), %r8                  ## 8-byte Folded Reload
	addq	-112(%rbp), %r8                 ## 8-byte Folded Reload
	xorl	%esi, %esi
	.p2align	4
LBB19_53:                               ##   Parent Loop BB19_16 Depth=1
                                        ##     Parent Loop BB19_46 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movzbl	(%r8,%rsi), %ecx
	addl	%ecx, %r9d
	imull	%eax, %ecx
	addl	%ecx, %r10d
	incq	%rsi
	decl	%eax
	cmpq	%rsi, %r11
	jne	LBB19_53
	jmp	LBB19_54
LBB19_4:
	cmpl	$2, 16(%rbp)
	ja	LBB19_12
## %bb.5:
	cmpq	$0, -56(%rbp)                   ## 8-byte Folded Reload
	je	LBB19_8
## %bb.6:
	testq	%r9, %r9
	je	LBB19_8
## %bb.7:
	movq	%rdx, %rax
	subq	%r8, %rax
	movq	%rax, %rcx
	negq	%rcx
	xorl	%esi, %esi
	cmpq	-56(%rbp), %rcx                 ## 8-byte Folded Reload
	setb	%sil
	xorl	%ecx, %ecx
	cmpq	%r9, %rax
	setb	%cl
	cmpq	%r8, %rdx
	cmoval	%ecx, %esi
	testb	%sil, %sil
	je	LBB19_9
	jmp	LBB19_12
LBB19_8:
	cmpq	$0, -56(%rbp)                   ## 8-byte Folded Reload
	je	LBB19_10
LBB19_9:
	movq	%rdx, %rax
	subq	%r14, %rax
	xorl	%ecx, %ecx
	cmpq	$804232, %rax                   ## imm = 0xC4588
	setb	%cl
	movq	%r14, %rax
	subq	%rdx, %rax
	xorl	%esi, %esi
	cmpq	-56(%rbp), %rax                 ## 8-byte Folded Reload
	setb	%sil
	cmpq	%rdx, %r14
	cmoval	%esi, %ecx
	testb	%cl, %cl
	jne	LBB19_12
LBB19_10:
	testq	%r9, %r9
	je	LBB19_13
## %bb.11:
	movq	%r14, %rax
	subq	%r8, %rax
	movq	%rax, %rcx
	negq	%rcx
	xorl	%esi, %esi
	cmpq	$804232, %rcx                   ## imm = 0xC4588
	setb	%sil
	xorl	%ecx, %ecx
	cmpq	%r9, %rax
	setb	%cl
	cmpq	%r8, %r14
	cmoval	%ecx, %esi
	cmpb	$1, %sil
	jne	LBB19_13
LBB19_12:
	movl	$1, 52(%r14)
LBB19_3:
	movl	$4, %r15d
	movl	$5, %eax
	xorl	%edx, %edx
	xorl	%r12d, %r12d
LBB19_59:
	movl	%eax, 48(%r14)
LBB19_60:
	movl	%r15d, (%rbx)
	movl	52(%r14), %eax
	movl	%eax, 4(%rbx)
	movq	%rdx, 8(%rbx)
	movq	%r12, 16(%rbx)
	movups	56(%r14), %xmm0
	movups	%xmm0, 24(%rbx)
	movq	72(%r14), %rax
	movq	%rax, 40(%rbx)
	movq	%rbx, %rax
	addq	$104, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB19_39:
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	movl	48(%r14), %r15d
	jmp	LBB19_56
LBB19_29:
	xorl	%r13d, %r13d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	movl	48(%r14), %r15d
	jmp	LBB19_56
LBB19_58:
	movl	$4, %eax
	jmp	LBB19_59
LBB19_57:
	movl	$1, %eax
	jmp	LBB19_59
LBB19_19:
	movl	$4, 52(%r14)
	jmp	LBB19_20
LBB19_22:
	movl	$1, %r13d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	movq	-88(%rbp), %r12                 ## 8-byte Reload
	movl	48(%r14), %r15d
	jmp	LBB19_56
LBB19_25:
	movl	$5, 52(%r14)
LBB19_20:
	movl	$4, %r15d
	movl	$5, %eax
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	jmp	LBB19_59
LBB19_36:
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	movl	48(%r14), %r15d
	jmp	LBB19_56
LBB19_42:
	movl	$3, 52(%r14)
	movl	$4, %r15d
	movl	$5, %eax
	jmp	LBB19_59
	.cfi_endproc
                                        ## -- End function
	.globl	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb ## -- Begin function _ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	.weak_def_can_be_hidden	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	.p2align	4
__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb: ## @_ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
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
	subq	$24, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movl	%r8d, -44(%rbp)                 ## 4-byte Spill
	movq	%rcx, %r15
	movq	%rdx, -56(%rbp)                 ## 8-byte Spill
	movq	%rsi, %r13
	movq	%rdi, %rbx
LBB20_1:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB20_2 Depth 2
                                        ##       Child Loop BB20_56 Depth 3
                                        ##       Child Loop BB20_58 Depth 3
                                        ##       Child Loop BB20_60 Depth 3
                                        ##       Child Loop BB20_64 Depth 3
                                        ##         Child Loop BB20_65 Depth 4
                                        ##         Child Loop BB20_66 Depth 4
	movq	%rbx, %rax
LBB20_2:                                ##   Parent Loop BB20_1 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB20_56 Depth 3
                                        ##       Child Loop BB20_58 Depth 3
                                        ##       Child Loop BB20_60 Depth 3
                                        ##       Child Loop BB20_64 Depth 3
                                        ##         Child Loop BB20_65 Depth 4
                                        ##         Child Loop BB20_66 Depth 4
	movq	%rax, %rbx
	movq	%r13, %rsi
	subq	%rax, %rsi
	sarq	$3, %rsi
	cmpq	$5, %rsi
	jbe	LBB20_78
## %bb.3:                               ##   in Loop: Header=BB20_2 Depth=2
	cmpq	$23, %rsi
	jle	LBB20_90
## %bb.4:                               ##   in Loop: Header=BB20_2 Depth=2
	testq	%r15, %r15
	je	LBB20_101
## %bb.5:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rsi, %rax
	shrq	%rax
	movl	-8(%r13), %ecx
	movl	-4(%r13), %edx
	cmpq	$129, %rsi
	jb	LBB20_9
## %bb.6:                               ##   in Loop: Header=BB20_2 Depth=2
	movl	(%rbx,%rax,8), %esi
	movl	4(%rbx,%rax,8), %edi
	xorl	%r8d, %r8d
	cmpl	4(%rbx), %edi
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	(%rbx), %esi
	setb	%r9b
	cmovel	%r8d, %r9d
	xorl	%r8d, %r8d
	cmpl	%edi, %edx
	setb	%r8b
	xorl	%edx, %edx
	cmpl	%esi, %ecx
	setb	%dl
	cmovel	%r8d, %edx
	testb	%r9b, %r9b
	je	LBB20_12
## %bb.7:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rcx
	testb	%dl, %dl
	je	LBB20_18
## %bb.8:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%r13), %rdx
	movq	%rdx, (%rbx)
	jmp	LBB20_20
	.p2align	4
LBB20_9:                                ##   in Loop: Header=BB20_2 Depth=2
	movl	(%rbx), %esi
	movl	4(%rbx), %edi
	xorl	%r8d, %r8d
	cmpl	4(%rbx,%rax,8), %edi
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	(%rbx,%rax,8), %esi
	setb	%r9b
	cmovel	%r8d, %r9d
	xorl	%r8d, %r8d
	cmpl	%edi, %edx
	setb	%r8b
	xorl	%edx, %edx
	cmpl	%esi, %ecx
	setb	%dl
	cmovel	%r8d, %edx
	testb	%r9b, %r9b
	je	LBB20_15
## %bb.10:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rcx
	testb	%dl, %dl
	je	LBB20_27
## %bb.11:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%r13), %rdx
	movq	%rdx, (%rbx,%rax,8)
	jmp	LBB20_29
LBB20_12:                               ##   in Loop: Header=BB20_2 Depth=2
	testb	%dl, %dl
	je	LBB20_21
## %bb.13:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rcx
	movq	-8(%r13), %rdx
	movq	%rdx, (%rbx,%rax,8)
	movq	%rcx, -8(%r13)
	movl	(%rbx,%rax,8), %ecx
	movl	4(%rbx,%rax,8), %edx
	xorl	%esi, %esi
	cmpl	4(%rbx), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	(%rbx), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_21
## %bb.14:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rcx
	movq	(%rbx,%rax,8), %rdx
	movq	%rdx, (%rbx)
	movq	%rcx, (%rbx,%rax,8)
	jmp	LBB20_21
LBB20_15:                               ##   in Loop: Header=BB20_2 Depth=2
	testb	%dl, %dl
	je	LBB20_30
## %bb.16:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rcx
	movq	-8(%r13), %rdx
	movq	%rdx, (%rbx)
	movq	%rcx, -8(%r13)
	movl	(%rbx), %ecx
	movl	4(%rbx), %edx
	xorl	%esi, %esi
	cmpl	4(%rbx,%rax,8), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	(%rbx,%rax,8), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_30
## %bb.17:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rcx
	movq	(%rbx), %rdx
	movq	%rdx, (%rbx,%rax,8)
	movq	%rcx, (%rbx)
	decq	%r15
	testb	$1, -44(%rbp)                   ## 1-byte Folded Reload
	je	LBB20_54
	jmp	LBB20_55
LBB20_18:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rdx
	movq	%rdx, (%rbx)
	movq	%rcx, (%rbx,%rax,8)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%esi, %esi
	cmpl	%edx, -4(%r13)
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, -8(%r13)
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_21
## %bb.19:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%r13), %rdx
	movq	%rdx, (%rbx,%rax,8)
LBB20_20:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rcx, -8(%r13)
LBB20_21:                               ##   in Loop: Header=BB20_2 Depth=2
	movl	-8(%rbx,%rax,8), %ecx
	movl	-4(%rbx,%rax,8), %edx
	xorl	%esi, %esi
	cmpl	12(%rbx), %edx
	setb	%sil
	xorl	%edi, %edi
	cmpl	8(%rbx), %ecx
	setb	%dil
	cmovel	%esi, %edi
	xorl	%esi, %esi
	cmpl	%edx, -12(%r13)
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, -16(%r13)
	setb	%dl
	cmovel	%esi, %edx
	testb	%dil, %dil
	je	LBB20_24
## %bb.22:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx), %rcx
	testb	%dl, %dl
	je	LBB20_31
## %bb.23:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-16(%r13), %rdx
	movq	%rdx, 8(%rbx)
	jmp	LBB20_33
LBB20_24:                               ##   in Loop: Header=BB20_2 Depth=2
	testb	%dl, %dl
	je	LBB20_34
## %bb.25:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%rbx,%rax,8), %rcx
	movq	-16(%r13), %rdx
	movq	%rdx, -8(%rbx,%rax,8)
	movq	%rcx, -16(%r13)
	movl	-8(%rbx,%rax,8), %ecx
	movl	-4(%rbx,%rax,8), %edx
	xorl	%esi, %esi
	cmpl	12(%rbx), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	8(%rbx), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_34
## %bb.26:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx), %rcx
	movq	-8(%rbx,%rax,8), %rdx
	movq	%rdx, 8(%rbx)
	movq	%rcx, -8(%rbx,%rax,8)
	jmp	LBB20_34
LBB20_27:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rdx
	movq	%rdx, (%rbx,%rax,8)
	movq	%rcx, (%rbx)
	movq	%rcx, %rax
	shrq	$32, %rax
	xorl	%edx, %edx
	cmpl	%eax, -4(%r13)
	setb	%dl
	xorl	%eax, %eax
	cmpl	%ecx, -8(%r13)
	setb	%al
	cmovel	%edx, %eax
	cmpb	$1, %al
	jne	LBB20_30
## %bb.28:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%r13), %rax
	movq	%rax, (%rbx)
LBB20_29:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rcx, -8(%r13)
	.p2align	4
LBB20_30:                               ##   in Loop: Header=BB20_2 Depth=2
	decq	%r15
	testb	$1, -44(%rbp)                   ## 1-byte Folded Reload
	je	LBB20_54
	jmp	LBB20_55
LBB20_31:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%rbx,%rax,8), %rdx
	movq	%rdx, 8(%rbx)
	movq	%rcx, -8(%rbx,%rax,8)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%esi, %esi
	cmpl	%edx, -12(%r13)
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, -16(%r13)
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_34
## %bb.32:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-16(%r13), %rdx
	movq	%rdx, -8(%rbx,%rax,8)
LBB20_33:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rcx, -16(%r13)
LBB20_34:                               ##   in Loop: Header=BB20_2 Depth=2
	movl	8(%rbx,%rax,8), %ecx
	movl	12(%rbx,%rax,8), %edx
	xorl	%esi, %esi
	cmpl	20(%rbx), %edx
	setb	%sil
	xorl	%edi, %edi
	cmpl	16(%rbx), %ecx
	setb	%dil
	cmovel	%esi, %edi
	xorl	%esi, %esi
	cmpl	%edx, -20(%r13)
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, -24(%r13)
	setb	%dl
	cmovel	%esi, %edx
	testb	%dil, %dil
	je	LBB20_37
## %bb.35:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	16(%rbx), %rcx
	testb	%dl, %dl
	je	LBB20_40
## %bb.36:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-24(%r13), %rdx
	movq	%rdx, 16(%rbx)
	jmp	LBB20_42
LBB20_37:                               ##   in Loop: Header=BB20_2 Depth=2
	testb	%dl, %dl
	je	LBB20_43
## %bb.38:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx,%rax,8), %rcx
	movq	-24(%r13), %rdx
	movq	%rdx, 8(%rbx,%rax,8)
	movq	%rcx, -24(%r13)
	movl	8(%rbx,%rax,8), %ecx
	movl	12(%rbx,%rax,8), %edx
	xorl	%esi, %esi
	cmpl	20(%rbx), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	16(%rbx), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_43
## %bb.39:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	16(%rbx), %rcx
	movq	8(%rbx,%rax,8), %rdx
	movq	%rdx, 16(%rbx)
	movq	%rcx, 8(%rbx,%rax,8)
	jmp	LBB20_43
LBB20_40:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx,%rax,8), %rdx
	movq	%rdx, 16(%rbx)
	movq	%rcx, 8(%rbx,%rax,8)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%esi, %esi
	cmpl	%edx, -20(%r13)
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, -24(%r13)
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_43
## %bb.41:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-24(%r13), %rdx
	movq	%rdx, 8(%rbx,%rax,8)
LBB20_42:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rcx, -24(%r13)
LBB20_43:                               ##   in Loop: Header=BB20_2 Depth=2
	movl	(%rbx,%rax,8), %ecx
	movl	-8(%rbx,%rax,8), %edx
	movl	-4(%rbx,%rax,8), %esi
	movl	4(%rbx,%rax,8), %r9d
	xorl	%edi, %edi
	cmpl	%esi, %r9d
	setb	%dil
	xorl	%r10d, %r10d
	cmpl	%edx, %ecx
	setb	%r10b
	cmovel	%edi, %r10d
	movl	8(%rbx,%rax,8), %edi
	movl	12(%rbx,%rax,8), %r8d
	xorl	%r11d, %r11d
	cmpl	%r9d, %r8d
	setb	%r11b
	xorl	%r9d, %r9d
	cmpl	%ecx, %edi
	setb	%r9b
	cmovel	%r11d, %r9d
	testb	%r10b, %r10b
	je	LBB20_46
## %bb.44:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%rbx,%rax,8), %rcx
	testb	%r9b, %r9b
	je	LBB20_49
## %bb.45:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rdx
	movq	8(%rbx,%rax,8), %rsi
	movq	%rsi, -8(%rbx,%rax,8)
	jmp	LBB20_51
LBB20_46:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rcx
	testb	%r9b, %r9b
	je	LBB20_53
## %bb.47:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx,%rax,8), %rdi
	movq	%rdi, %r8
	shrq	$32, %r8
	xorl	%r9d, %r9d
	cmpl	%r8d, %esi
	movq	%rdi, (%rbx,%rax,8)
	seta	%r9b
	xorl	%esi, %esi
	cmpl	%edi, %edx
	seta	%sil
	movq	%rcx, 8(%rbx,%rax,8)
	cmovnel	%esi, %r9d
	cmpb	$1, %r9b
	jne	LBB20_52
## %bb.48:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	-8(%rbx,%rax,8), %rcx
	movq	%rdi, -8(%rbx,%rax,8)
	movq	%rcx, (%rbx,%rax,8)
	jmp	LBB20_53
LBB20_49:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx,%rax,8), %rdx
	movq	%rdx, -8(%rbx,%rax,8)
	movq	%rcx, (%rbx,%rax,8)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%esi, %esi
	cmpl	%edx, %r8d
	setb	%sil
	xorl	%edx, %edx
	cmpl	%ecx, %edi
	setb	%dl
	cmovnel	%edx, %esi
	cmpb	$1, %sil
	jne	LBB20_53
## %bb.50:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	8(%rbx,%rax,8), %rdx
	movq	%rdx, (%rbx,%rax,8)
LBB20_51:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rcx, 8(%rbx,%rax,8)
	movq	%rdx, %rcx
	jmp	LBB20_53
LBB20_52:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rdi, %rcx
	.p2align	4
LBB20_53:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rdx
	movq	%rcx, (%rbx)
	movq	%rdx, (%rbx,%rax,8)
	decq	%r15
	testb	$1, -44(%rbp)                   ## 1-byte Folded Reload
	jne	LBB20_55
LBB20_54:                               ##   in Loop: Header=BB20_2 Depth=2
	movl	-8(%rbx), %eax
	movl	-4(%rbx), %ecx
	xorl	%edx, %edx
	cmpl	4(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	testb	%cl, %cl
	je	LBB20_76
LBB20_55:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%rbx), %rax
	movq	%rax, %rcx
	shrq	$32, %rcx
	movl	$12, %edx
	xorl	%edi, %edi
	.p2align	4
LBB20_56:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	xorl	%esi, %esi
	cmpl	%ecx, (%rbx,%rdx)
	setb	%sil
	xorl	%r8d, %r8d
	cmpl	%eax, -4(%rbx,%rdx)
	setb	%r8b
	cmovel	%esi, %r8d
	addq	$-8, %rdi
	addq	$8, %rdx
	testb	%r8b, %r8b
	jne	LBB20_56
## %bb.57:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	%rbx, %rdx
	subq	%rdi, %rdx
	movq	%r13, %rsi
	cmpq	$-8, %rdi
	je	LBB20_59
	.p2align	4
LBB20_58:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	xorl	%edi, %edi
	cmpl	%ecx, -4(%rsi)
	setb	%dil
	xorl	%r8d, %r8d
	cmpl	%eax, -8(%rsi)
	leaq	-8(%rsi), %rsi
	setb	%r8b
	cmovel	%edi, %r8d
	cmpb	$1, %r8b
	jne	LBB20_58
	jmp	LBB20_62
	.p2align	4
LBB20_59:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%r13, %rdi
	.p2align	4
LBB20_60:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	cmpq	%rdi, %rdx
	jae	LBB20_69
## %bb.61:                              ##   in Loop: Header=BB20_60 Depth=3
	leaq	-8(%rdi), %rsi
	xorl	%r8d, %r8d
	cmpl	%ecx, -4(%rdi)
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	%eax, -8(%rdi)
	setb	%r9b
	cmovel	%r8d, %r9d
	movq	%rsi, %rdi
	testb	%r9b, %r9b
	je	LBB20_60
	.p2align	4
LBB20_62:                               ##   in Loop: Header=BB20_2 Depth=2
	cmpq	%rsi, %rdx
	jae	LBB20_70
LBB20_63:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rdx, %r14
	movq	%rsi, %rdi
	.p2align	4
LBB20_64:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ## =>    This Loop Header: Depth=3
                                        ##         Child Loop BB20_65 Depth 4
                                        ##         Child Loop BB20_66 Depth 4
	movq	(%r14), %r8
	movq	(%rdi), %r9
	movq	%r9, (%r14)
	movq	%r8, (%rdi)
	.p2align	4
LBB20_65:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ##       Parent Loop BB20_64 Depth=3
                                        ## =>      This Inner Loop Header: Depth=4
	xorl	%r8d, %r8d
	cmpl	%ecx, 12(%r14)
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	%eax, 8(%r14)
	leaq	8(%r14), %r14
	setb	%r9b
	cmovel	%r8d, %r9d
	testb	%r9b, %r9b
	jne	LBB20_65
	.p2align	4
LBB20_66:                               ##   Parent Loop BB20_1 Depth=1
                                        ##     Parent Loop BB20_2 Depth=2
                                        ##       Parent Loop BB20_64 Depth=3
                                        ## =>      This Inner Loop Header: Depth=4
	xorl	%r8d, %r8d
	cmpl	%ecx, -4(%rdi)
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	%eax, -8(%rdi)
	leaq	-8(%rdi), %rdi
	setb	%r9b
	cmovel	%r8d, %r9d
	cmpb	$1, %r9b
	jne	LBB20_66
## %bb.67:                              ##   in Loop: Header=BB20_64 Depth=3
	cmpq	%rdi, %r14
	jb	LBB20_64
## %bb.68:                              ##   in Loop: Header=BB20_2 Depth=2
	addq	$-8, %r14
	cmpq	%r14, %rbx
	jne	LBB20_71
	jmp	LBB20_72
LBB20_69:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rdi, %rsi
	cmpq	%rsi, %rdx
	jb	LBB20_63
	.p2align	4
LBB20_70:                               ##   in Loop: Header=BB20_2 Depth=2
	leaq	-8(%rdx), %r14
	cmpq	%r14, %rbx
	je	LBB20_72
LBB20_71:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	(%r14), %rcx
	movq	%rcx, (%rbx)
LBB20_72:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rax, (%r14)
	cmpq	%rsi, %rdx
	jb	LBB20_75
## %bb.73:                              ##   in Loop: Header=BB20_2 Depth=2
	movq	%r15, -64(%rbp)                 ## 8-byte Spill
	movq	%rbx, %rdi
	movq	%r14, %rsi
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	callq	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	movl	%eax, %r12d
	leaq	8(%r14), %rdi
	movq	%r13, %rsi
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	callq	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	testb	%al, %al
	jne	LBB20_77
## %bb.74:                              ##   in Loop: Header=BB20_2 Depth=2
	testb	%r12b, %r12b
	movq	-64(%rbp), %r15                 ## 8-byte Reload
	leaq	8(%r14), %rax
	jne	LBB20_2
LBB20_75:                               ##   in Loop: Header=BB20_2 Depth=2
	movzbl	-44(%rbp), %r8d                 ## 1-byte Folded Reload
	movq	%rbx, %rdi
	movq	%r14, %rsi
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	movq	%r15, %rcx
	callq	__ZNSt3__111__introsortINS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolELb0EEEvT1_S8_T0_NS_15iterator_traitsIS8_E15difference_typeEb
	addq	$8, %r14
	movl	$0, -44(%rbp)                   ## 4-byte Folded Spill
	movq	%r14, %rax
	jmp	LBB20_2
LBB20_76:                               ##   in Loop: Header=BB20_2 Depth=2
	movq	%rbx, %rdi
	movq	%r13, %rsi
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	callq	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	movl	$0, -44(%rbp)                   ## 4-byte Folded Spill
	jmp	LBB20_2
LBB20_77:                               ##   in Loop: Header=BB20_1 Depth=1
	movq	%r14, %r13
	testb	%r12b, %r12b
	movq	-64(%rbp), %r15                 ## 8-byte Reload
	je	LBB20_1
	jmp	LBB20_115
LBB20_78:
	leaq	LJTI20_0(%rip), %rax
	movslq	(%rax,%rsi,4), %rcx
	addq	%rax, %rcx
	jmpq	*%rcx
LBB20_79:
	movl	-8(%r13), %eax
	movl	-4(%r13), %ecx
	xorl	%edx, %edx
	cmpl	4(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.80:
	movq	(%rbx), %rax
	jmp	LBB20_83
LBB20_81:
	movl	8(%rbx), %eax
	movl	12(%rbx), %ecx
	xorl	%edx, %edx
	cmpl	4(%rbx), %ecx
	setb	%dl
	xorl	%esi, %esi
	cmpl	(%rbx), %eax
	setb	%sil
	cmovel	%edx, %esi
	xorl	%edx, %edx
	cmpl	%ecx, -4(%r13)
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	%eax, -8(%r13)
	setb	%cl
	cmovel	%edx, %ecx
	testb	%sil, %sil
	je	LBB20_103
## %bb.82:
	movq	(%rbx), %rax
	testb	%cl, %cl
	je	LBB20_112
LBB20_83:
	movq	-8(%r13), %rcx
	movq	%rcx, (%rbx)
LBB20_114:
	movq	%rax, -8(%r13)
	jmp	LBB20_115
LBB20_84:
	leaq	8(%rbx), %rsi
	leaq	16(%rbx), %rdx
	leaq	24(%rbx), %rcx
	movq	%rbx, %rdi
	movq	-56(%rbp), %r8                  ## 8-byte Reload
	callq	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	movl	-8(%r13), %eax
	movl	-4(%r13), %ecx
	xorl	%edx, %edx
	cmpl	28(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	24(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.85:
	movq	24(%rbx), %rax
	movq	-8(%r13), %rcx
	movq	%rcx, 24(%rbx)
	movq	%rax, -8(%r13)
	movl	24(%rbx), %eax
	movl	28(%rbx), %ecx
	xorl	%edx, %edx
	cmpl	20(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	16(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.86:
	movq	16(%rbx), %rcx
	movq	24(%rbx), %rax
	movq	%rax, 16(%rbx)
	movq	%rcx, 24(%rbx)
	movq	%rax, %rcx
	shrq	$32, %rcx
	xorl	%edx, %edx
	cmpl	%ecx, 12(%rbx)
	seta	%dl
	xorl	%esi, %esi
	cmpl	%eax, 8(%rbx)
	seta	%sil
	cmovnel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB20_115
## %bb.87:
	movq	8(%rbx), %rdx
	movq	%rax, 8(%rbx)
	movq	%rdx, 16(%rbx)
	xorl	%edx, %edx
	cmpl	%ecx, 4(%rbx)
	seta	%dl
	xorl	%ecx, %ecx
	cmpl	%eax, (%rbx)
	seta	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.88:
	movq	(%rbx), %rcx
	movq	%rax, (%rbx)
	movq	%rcx, 8(%rbx)
	jmp	LBB20_115
LBB20_89:
	leaq	8(%rbx), %rsi
	leaq	16(%rbx), %rdx
	addq	$-8, %r13
	movq	%rbx, %rdi
	movq	%r13, %rcx
	movq	-56(%rbp), %r8                  ## 8-byte Reload
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	jmp	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_ ## TAILCALL
LBB20_90:
	cmpq	%r13, %rbx
	sete	%cl
	leaq	8(%rbx), %rdx
	cmpq	%r13, %rdx
	sete	%al
	orb	%cl, %al
	cmpb	$1, -44(%rbp)                   ## 1-byte Folded Reload
	jne	LBB20_106
## %bb.91:
	testb	%al, %al
	jne	LBB20_115
## %bb.92:
	movl	$8, %eax
	movq	%rbx, %rcx
	jmp	LBB20_96
	.p2align	4
LBB20_93:                               ##   in Loop: Header=BB20_96 Depth=1
	movq	%rbx, %rsi
LBB20_94:                               ##   in Loop: Header=BB20_96 Depth=1
	movq	%rdx, (%rsi)
LBB20_95:                               ##   in Loop: Header=BB20_96 Depth=1
	leaq	8(%rcx), %rdx
	addq	$8, %rax
	cmpq	%r13, %rdx
	je	LBB20_115
LBB20_96:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB20_98 Depth 2
	movl	8(%rcx), %esi
	movl	12(%rcx), %edi
	xorl	%r8d, %r8d
	cmpl	4(%rcx), %edi
	setb	%r8b
	xorl	%edi, %edi
	cmpl	(%rcx), %esi
	movq	%rdx, %rcx
	setb	%dil
	cmovel	%r8d, %edi
	cmpb	$1, %dil
	jne	LBB20_95
## %bb.97:                              ##   in Loop: Header=BB20_96 Depth=1
	movq	(%rcx), %rdx
	movq	%rdx, %rdi
	shrq	$32, %rdi
	movq	%rax, %rsi
	.p2align	4
LBB20_98:                               ##   Parent Loop BB20_96 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	-8(%rbx,%rsi), %r8
	movq	%r8, (%rbx,%rsi)
	cmpq	$8, %rsi
	je	LBB20_93
## %bb.99:                              ##   in Loop: Header=BB20_98 Depth=2
	xorl	%r8d, %r8d
	cmpl	%edi, -12(%rbx,%rsi)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%edx, -16(%rbx,%rsi)
	seta	%r9b
	cmovel	%r8d, %r9d
	addq	$-8, %rsi
	testb	%r9b, %r9b
	jne	LBB20_98
## %bb.100:                             ##   in Loop: Header=BB20_96 Depth=1
	addq	%rbx, %rsi
	jmp	LBB20_94
LBB20_101:
	cmpq	%r13, %rbx
	je	LBB20_115
## %bb.102:
	movq	%rbx, %rdi
	movq	%r13, %rsi
	movq	%r13, %rdx
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	jmp	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_ ## TAILCALL
LBB20_103:
	testb	%cl, %cl
	je	LBB20_115
## %bb.104:
	movq	8(%rbx), %rax
	movq	-8(%r13), %rcx
	movq	%rcx, 8(%rbx)
	movq	%rax, -8(%r13)
	movl	8(%rbx), %eax
	movl	12(%rbx), %ecx
	xorl	%edx, %edx
	cmpl	4(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.105:
	movdqu	(%rbx), %xmm0
	pshufd	$78, %xmm0, %xmm0               ## xmm0 = xmm0[2,3,0,1]
	movdqu	%xmm0, (%rbx)
	jmp	LBB20_115
LBB20_106:
	testb	%al, %al
	je	LBB20_108
	jmp	LBB20_115
	.p2align	4
LBB20_107:                              ##   in Loop: Header=BB20_108 Depth=1
	leaq	8(%rbx), %rdx
	cmpq	%r13, %rdx
	je	LBB20_115
LBB20_108:                              ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB20_110 Depth 2
	movl	8(%rbx), %eax
	movl	12(%rbx), %ecx
	xorl	%esi, %esi
	cmpl	4(%rbx), %ecx
	setb	%sil
	xorl	%ecx, %ecx
	cmpl	(%rbx), %eax
	movq	%rdx, %rbx
	setb	%cl
	cmovel	%esi, %ecx
	cmpb	$1, %cl
	jne	LBB20_107
## %bb.109:                             ##   in Loop: Header=BB20_108 Depth=1
	movq	(%rbx), %rax
	movq	%rax, %rcx
	shrq	$32, %rcx
	movq	%rbx, %rdx
	.p2align	4
LBB20_110:                              ##   Parent Loop BB20_108 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	-8(%rdx), %rsi
	movq	%rsi, (%rdx)
	xorl	%esi, %esi
	cmpl	%ecx, -12(%rdx)
	seta	%sil
	xorl	%edi, %edi
	cmpl	%eax, -16(%rdx)
	seta	%dil
	leaq	-8(%rdx), %rdx
	cmovel	%esi, %edi
	testb	%dil, %dil
	jne	LBB20_110
## %bb.111:                             ##   in Loop: Header=BB20_108 Depth=1
	movq	%rax, (%rdx)
	jmp	LBB20_107
LBB20_112:
	movq	8(%rbx), %rcx
	movq	%rcx, (%rbx)
	movq	%rax, 8(%rbx)
	movq	%rax, %rcx
	shrq	$32, %rcx
	xorl	%edx, %edx
	cmpl	%ecx, -4(%r13)
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	%eax, -8(%r13)
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB20_115
## %bb.113:
	movq	-8(%r13), %rcx
	movq	%rcx, 8(%rbx)
	jmp	LBB20_114
LBB20_115:
	addq	$24, %rsp
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
L20_0_set_115 = LBB20_115-LJTI20_0
L20_0_set_79 = LBB20_79-LJTI20_0
L20_0_set_81 = LBB20_81-LJTI20_0
L20_0_set_89 = LBB20_89-LJTI20_0
L20_0_set_84 = LBB20_84-LJTI20_0
LJTI20_0:
	.long	L20_0_set_115
	.long	L20_0_set_115
	.long	L20_0_set_79
	.long	L20_0_set_81
	.long	L20_0_set_89
	.long	L20_0_set_84
	.end_data_region
                                        ## -- End function
	.private_extern	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_ ## -- Begin function _ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.globl	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.weak_def_can_be_hidden	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.p2align	4
__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_: ## @_ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%rbx
	.cfi_offset %rbx, -24
	movl	(%rsi), %r9d
	movl	4(%rsi), %r10d
	xorl	%eax, %eax
	cmpl	4(%rdi), %r10d
	setb	%al
	xorl	%r11d, %r11d
	cmpl	(%rdi), %r9d
	setb	%r11b
	cmovel	%eax, %r11d
	movl	(%rdx), %eax
	movl	4(%rdx), %r8d
	xorl	%ebx, %ebx
	cmpl	%r10d, %r8d
	setb	%bl
	xorl	%r10d, %r10d
	cmpl	%r9d, %eax
	setb	%r10b
	cmovel	%ebx, %r10d
	testb	%r11b, %r11b
	je	LBB21_1
## %bb.5:
	movq	(%rdi), %r9
	testb	%r10b, %r10b
	je	LBB21_7
## %bb.6:
	movq	(%rdx), %rax
	movq	%rax, (%rdi)
	movq	%r9, (%rdx)
	movq	%r9, %r8
	shrq	$32, %r8
	jmp	LBB21_9
LBB21_1:
	testb	%r10b, %r10b
	je	LBB21_10
## %bb.2:
	movq	(%rsi), %rax
	movq	(%rdx), %r8
	movq	%r8, (%rsi)
	movq	%rax, (%rdx)
	movl	(%rsi), %r8d
	movl	4(%rsi), %r9d
	xorl	%r10d, %r10d
	cmpl	4(%rdi), %r9d
	setb	%r10b
	xorl	%r9d, %r9d
	cmpl	(%rdi), %r8d
	setb	%r9b
	cmovel	%r10d, %r9d
	cmpb	$1, %r9b
	jne	LBB21_3
## %bb.4:
	movq	(%rdi), %rax
	movq	(%rsi), %r8
	movq	%r8, (%rdi)
	movq	%rax, (%rsi)
	movl	(%rdx), %eax
	movl	4(%rdx), %r8d
	jmp	LBB21_10
LBB21_7:
	movq	(%rsi), %rax
	movq	%rax, (%rdi)
	movq	%r9, (%rsi)
	movl	(%rdx), %eax
	movl	4(%rdx), %r8d
	movq	%r9, %r10
	shrq	$32, %r10
	xorl	%r11d, %r11d
	cmpl	%r10d, %r8d
	setb	%r11b
	xorl	%ebx, %ebx
	cmpl	%r9d, %eax
	setb	%bl
	cmovel	%r11d, %ebx
	cmpb	$1, %bl
	jne	LBB21_10
## %bb.8:
	movq	(%rdx), %rax
	movq	%rax, (%rsi)
	movq	%r9, (%rdx)
	movl	%r10d, %r8d
LBB21_9:
	movl	%r9d, %eax
	jmp	LBB21_10
LBB21_3:
	movq	%rax, %r8
	shrq	$32, %r8
LBB21_10:
	xorl	%r9d, %r9d
	cmpl	%r8d, 4(%rcx)
	setb	%r9b
	xorl	%r8d, %r8d
	cmpl	%eax, (%rcx)
	setb	%r8b
	cmovel	%r9d, %r8d
	cmpb	$1, %r8b
	jne	LBB21_14
## %bb.11:
	movq	(%rdx), %rax
	movq	(%rcx), %r8
	movq	%r8, (%rdx)
	movq	%rax, (%rcx)
	movl	(%rdx), %eax
	movl	4(%rdx), %ecx
	xorl	%r8d, %r8d
	cmpl	4(%rsi), %ecx
	setb	%r8b
	xorl	%ecx, %ecx
	cmpl	(%rsi), %eax
	setb	%cl
	cmovel	%r8d, %ecx
	cmpb	$1, %cl
	jne	LBB21_14
## %bb.12:
	movq	(%rsi), %rax
	movq	(%rdx), %rcx
	movq	%rcx, (%rsi)
	movq	%rax, (%rdx)
	movl	(%rsi), %eax
	movl	4(%rsi), %ecx
	xorl	%edx, %edx
	cmpl	4(%rdi), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	(%rdi), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	jne	LBB21_14
## %bb.13:
	movq	(%rdi), %rax
	movq	(%rsi), %rcx
	movq	%rcx, (%rdi)
	movq	%rax, (%rsi)
LBB21_14:
	popq	%rbx
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.private_extern	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_ ## -- Begin function _ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.globl	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.weak_def_can_be_hidden	__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.p2align	4
__ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_: ## @_ZNSt3__131__partition_with_equals_on_leftB9nqe220108INS_17_ClassicAlgPolicyEPN8stx_zlib13DeflateStream14WeightedSymbolERNS3_11WeightOrderEEET0_S8_S8_T1_
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	(%rdi), %rcx
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%eax, %eax
	cmpl	%edx, -4(%rsi)
	seta	%al
	xorl	%r8d, %r8d
	cmpl	%ecx, -8(%rsi)
	seta	%r8b
	cmovnel	%r8d, %eax
	testb	%al, %al
	je	LBB22_1
## %bb.4:
	movq	%rdi, %rax
	.p2align	4
LBB22_5:                                ## =>This Inner Loop Header: Depth=1
	xorl	%r8d, %r8d
	cmpl	%edx, 12(%rax)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%ecx, 8(%rax)
	seta	%r9b
	leaq	8(%rax), %rax
	cmovel	%r8d, %r9d
	testb	%r9b, %r9b
	je	LBB22_5
	jmp	LBB22_6
LBB22_1:
	leaq	8(%rdi), %r8
	.p2align	4
LBB22_2:                                ## =>This Inner Loop Header: Depth=1
	movq	%r8, %rax
	cmpq	%rsi, %r8
	jae	LBB22_6
## %bb.3:                               ##   in Loop: Header=BB22_2 Depth=1
	xorl	%r8d, %r8d
	cmpl	%edx, 4(%rax)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%ecx, (%rax)
	seta	%r9b
	cmovel	%r8d, %r9d
	leaq	8(%rax), %r8
	cmpb	$1, %r9b
	jne	LBB22_2
LBB22_6:
	cmpq	%rsi, %rax
	jae	LBB22_8
	.p2align	4
LBB22_7:                                ## =>This Inner Loop Header: Depth=1
	xorl	%r8d, %r8d
	cmpl	%edx, -4(%rsi)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%ecx, -8(%rsi)
	seta	%r9b
	leaq	-8(%rsi), %rsi
	cmovel	%r8d, %r9d
	testb	%r9b, %r9b
	jne	LBB22_7
	jmp	LBB22_8
	.p2align	4
LBB22_9:                                ##   in Loop: Header=BB22_8 Depth=1
	movq	(%rax), %r8
	movq	(%rsi), %r9
	movq	%r9, (%rax)
	movq	%r8, (%rsi)
	.p2align	4
LBB22_10:                               ##   Parent Loop BB22_8 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	xorl	%r8d, %r8d
	cmpl	%edx, 12(%rax)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%ecx, 8(%rax)
	seta	%r9b
	leaq	8(%rax), %rax
	cmovel	%r8d, %r9d
	cmpb	$1, %r9b
	jne	LBB22_10
	.p2align	4
LBB22_11:                               ##   Parent Loop BB22_8 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	xorl	%r8d, %r8d
	cmpl	%edx, -4(%rsi)
	seta	%r8b
	xorl	%r9d, %r9d
	cmpl	%ecx, -8(%rsi)
	seta	%r9b
	leaq	-8(%rsi), %rsi
	cmovel	%r8d, %r9d
	testb	%r9b, %r9b
	jne	LBB22_11
LBB22_8:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB22_10 Depth 2
                                        ##     Child Loop BB22_11 Depth 2
	cmpq	%rsi, %rax
	jb	LBB22_9
## %bb.12:
	leaq	-8(%rax), %rdx
	cmpq	%rdx, %rdi
	je	LBB22_14
## %bb.13:
	movq	(%rdx), %rdx
	movq	%rdx, (%rdi)
LBB22_14:
	movq	%rcx, -8(%rax)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.private_extern	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_ ## -- Begin function _ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.globl	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.weak_def_can_be_hidden	__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.p2align	4
__ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_: ## @_ZNSt3__127__insertion_sort_incompleteB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolEEEbT1_S8_T0_
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	pushq	%r14
	pushq	%rbx
	.cfi_offset %rbx, -32
	.cfi_offset %r14, -24
	movq	%rsi, %rcx
	subq	%rdi, %rcx
	sarq	$3, %rcx
	movb	$1, %al
	cmpq	$5, %rcx
	ja	LBB23_16
## %bb.1:
	movq	%rdx, %r8
	leaq	LJTI23_0(%rip), %rdx
	movslq	(%rdx,%rcx,4), %rcx
	addq	%rdx, %rcx
	jmpq	*%rcx
LBB23_2:
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB23_3:
	movl	8(%rdi), %ecx
	movl	12(%rdi), %edx
	xorl	%r8d, %r8d
	cmpl	4(%rdi), %edx
	setb	%r8b
	xorl	%r9d, %r9d
	cmpl	(%rdi), %ecx
	setb	%r9b
	cmovel	%r8d, %r9d
	xorl	%r8d, %r8d
	cmpl	%edx, -4(%rsi)
	setb	%r8b
	xorl	%edx, %edx
	cmpl	%ecx, -8(%rsi)
	setb	%dl
	cmovel	%r8d, %edx
	testb	%r9b, %r9b
	je	LBB23_20
## %bb.4:
	movq	(%rdi), %rcx
	testb	%dl, %dl
	jne	LBB23_9
## %bb.5:
	movq	8(%rdi), %rdx
	movq	%rdx, (%rdi)
	movq	%rcx, 8(%rdi)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%r8d, %r8d
	cmpl	%edx, -4(%rsi)
	setb	%r8b
	xorl	%edx, %edx
	cmpl	%ecx, -8(%rsi)
	setb	%dl
	cmovel	%r8d, %edx
	cmpb	$1, %dl
	jne	LBB23_2
## %bb.6:
	movq	-8(%rsi), %rdx
	movq	%rdx, 8(%rdi)
	jmp	LBB23_10
LBB23_7:
	movl	-8(%rsi), %ecx
	movl	-4(%rsi), %edx
	xorl	%r8d, %r8d
	cmpl	4(%rdi), %edx
	setb	%r8b
	xorl	%edx, %edx
	cmpl	(%rdi), %ecx
	setb	%dl
	cmovel	%r8d, %edx
	cmpb	$1, %dl
	jne	LBB23_2
## %bb.8:
	movq	(%rdi), %rcx
LBB23_9:
	movq	-8(%rsi), %rdx
	movq	%rdx, (%rdi)
LBB23_10:
	movq	%rcx, -8(%rsi)
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB23_11:
	leaq	8(%rdi), %rax
	leaq	16(%rdi), %rdx
	leaq	24(%rdi), %rcx
	movq	%rdi, %rbx
	movq	%rsi, %r14
	movq	%rax, %rsi
	callq	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
	movl	-8(%r14), %ecx
	movl	-4(%r14), %edx
	xorl	%esi, %esi
	cmpl	28(%rbx), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	24(%rbx), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB23_23
## %bb.12:
	movq	24(%rbx), %rcx
	movq	-8(%r14), %rdx
	movq	%rdx, 24(%rbx)
	movq	%rcx, -8(%r14)
	movl	24(%rbx), %eax
	movl	28(%rbx), %ecx
	xorl	%edx, %edx
	cmpl	20(%rbx), %ecx
	setb	%dl
	xorl	%ecx, %ecx
	cmpl	16(%rbx), %eax
	setb	%cl
	cmovel	%edx, %ecx
	cmpb	$1, %cl
	movb	$1, %al
	jne	LBB23_2
## %bb.13:
	movq	16(%rbx), %rdx
	movq	24(%rbx), %rcx
	movq	%rcx, 16(%rbx)
	movq	%rdx, 24(%rbx)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%esi, %esi
	cmpl	%edx, 12(%rbx)
	seta	%sil
	xorl	%edi, %edi
	cmpl	%ecx, 8(%rbx)
	seta	%dil
	cmovnel	%edi, %esi
	cmpb	$1, %sil
	jne	LBB23_2
## %bb.14:
	movq	8(%rbx), %rsi
	movq	%rcx, 8(%rbx)
	movq	%rsi, 16(%rbx)
	xorl	%esi, %esi
	cmpl	%edx, 4(%rbx)
	seta	%sil
	xorl	%edx, %edx
	cmpl	%ecx, (%rbx)
	seta	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB23_2
## %bb.15:
	movq	(%rbx), %rdx
	movq	%rcx, (%rbx)
	movq	%rdx, 8(%rbx)
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB23_16:
	movl	8(%rdi), %r11d
	movl	(%rdi), %ecx
	movl	4(%rdi), %edx
	movl	12(%rdi), %r10d
	xorl	%r8d, %r8d
	cmpl	%edx, %r10d
	setb	%r8b
	xorl	%ebx, %ebx
	cmpl	%ecx, %r11d
	setb	%bl
	cmovel	%r8d, %ebx
	movl	16(%rdi), %r8d
	movl	20(%rdi), %r9d
	xorl	%r14d, %r14d
	cmpl	%r10d, %r9d
	setb	%r14b
	xorl	%r10d, %r10d
	cmpl	%r11d, %r8d
	setb	%r10b
	cmovel	%r14d, %r10d
	testb	%bl, %bl
	je	LBB23_24
## %bb.17:
	movq	(%rdi), %rcx
	testb	%r10b, %r10b
	je	LBB23_27
## %bb.18:
	movq	16(%rdi), %rdx
	movq	%rdx, (%rdi)
	jmp	LBB23_29
LBB23_19:
	leaq	8(%rdi), %rax
	leaq	16(%rdi), %rdx
	addq	$-8, %rsi
	movq	%rsi, %rcx
	movq	%rax, %rsi
	callq	__ZNSt3__17__sort4B9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolETnNS_9enable_ifIXnt21__use_branchless_sortIT0_T1_EEiE4typeELi0EEEvSA_SA_SA_SA_S9_
LBB23_23:
	movb	$1, %al
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB23_20:
	testb	%dl, %dl
	je	LBB23_2
## %bb.21:
	movq	8(%rdi), %rcx
	movq	-8(%rsi), %rdx
	movq	%rdx, 8(%rdi)
	movq	%rcx, -8(%rsi)
	movl	8(%rdi), %ecx
	movl	12(%rdi), %edx
	xorl	%esi, %esi
	cmpl	4(%rdi), %edx
	setb	%sil
	xorl	%edx, %edx
	cmpl	(%rdi), %ecx
	setb	%dl
	cmovel	%esi, %edx
	cmpb	$1, %dl
	jne	LBB23_2
## %bb.22:
	movdqu	(%rdi), %xmm0
	pshufd	$78, %xmm0, %xmm0               ## xmm0 = xmm0[2,3,0,1]
	movdqu	%xmm0, (%rdi)
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
LBB23_24:
	testb	%r10b, %r10b
	je	LBB23_30
## %bb.25:
	movq	16(%rdi), %r8
	movq	%r8, %r9
	shrq	$32, %r9
	xorl	%r10d, %r10d
	cmpl	%r9d, %edx
	movq	8(%rdi), %rdx
	movq	%r8, 8(%rdi)
	seta	%r10b
	xorl	%r9d, %r9d
	cmpl	%r8d, %ecx
	seta	%r9b
	movq	%rdx, 16(%rdi)
	cmovnel	%r9d, %r10d
	cmpb	$1, %r10b
	jne	LBB23_30
## %bb.26:
	movq	(%rdi), %rcx
	movq	%r8, (%rdi)
	movq	%rcx, 8(%rdi)
	jmp	LBB23_30
LBB23_27:
	movq	8(%rdi), %rdx
	movq	%rdx, (%rdi)
	movq	%rcx, 8(%rdi)
	movq	%rcx, %rdx
	shrq	$32, %rdx
	xorl	%r10d, %r10d
	cmpl	%edx, %r9d
	setb	%r10b
	xorl	%edx, %edx
	cmpl	%ecx, %r8d
	setb	%dl
	cmovnel	%edx, %r10d
	cmpb	$1, %r10b
	jne	LBB23_30
## %bb.28:
	movq	16(%rdi), %rdx
	movq	%rdx, 8(%rdi)
LBB23_29:
	movq	%rcx, 16(%rdi)
LBB23_30:
	leaq	24(%rdi), %rcx
	cmpq	%rsi, %rcx
	je	LBB23_2
## %bb.31:
	leaq	16(%rdi), %r9
	xorl	%edx, %edx
	movl	$24, %r8d
	jmp	LBB23_33
	.p2align	4
LBB23_32:                               ##   in Loop: Header=BB23_33 Depth=1
	movq	%rdi, %r10
	movq	%r9, (%r10)
	incl	%edx
	cmpl	$8, %edx
	je	LBB23_39
LBB23_38:                               ##   in Loop: Header=BB23_33 Depth=1
	movq	%rcx, %r9
	addq	$8, %rcx
	addq	$8, %r8
	cmpq	%rsi, %rcx
	je	LBB23_2
LBB23_33:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB23_35 Depth 2
	movl	(%rcx), %r10d
	movl	4(%rcx), %r11d
	xorl	%ebx, %ebx
	cmpl	4(%r9), %r11d
	setb	%bl
	xorl	%r11d, %r11d
	cmpl	(%r9), %r10d
	setb	%r11b
	cmovel	%ebx, %r11d
	cmpb	$1, %r11b
	jne	LBB23_38
## %bb.34:                              ##   in Loop: Header=BB23_33 Depth=1
	movq	(%rcx), %r9
	movq	%r9, %r11
	shrq	$32, %r11
	movq	%r8, %r10
	.p2align	4
LBB23_35:                               ##   Parent Loop BB23_33 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	-8(%rdi,%r10), %rbx
	movq	%rbx, (%rdi,%r10)
	cmpq	$8, %r10
	je	LBB23_32
## %bb.36:                              ##   in Loop: Header=BB23_35 Depth=2
	xorl	%ebx, %ebx
	cmpl	%r11d, -12(%rdi,%r10)
	seta	%bl
	xorl	%r14d, %r14d
	cmpl	%r9d, -16(%rdi,%r10)
	seta	%r14b
	cmovel	%ebx, %r14d
	addq	$-8, %r10
	testb	%r14b, %r14b
	jne	LBB23_35
## %bb.37:                              ##   in Loop: Header=BB23_33 Depth=1
	addq	%rdi, %r10
	movq	%r9, (%r10)
	incl	%edx
	cmpl	$8, %edx
	jne	LBB23_38
LBB23_39:
	addq	$8, %rcx
	cmpq	%rsi, %rcx
	sete	%al
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
	.cfi_endproc
	.p2align	2
	.data_region jt32
L23_0_set_2 = LBB23_2-LJTI23_0
L23_0_set_7 = LBB23_7-LJTI23_0
L23_0_set_3 = LBB23_3-LJTI23_0
L23_0_set_19 = LBB23_19-LJTI23_0
L23_0_set_11 = LBB23_11-LJTI23_0
LJTI23_0:
	.long	L23_0_set_2
	.long	L23_0_set_2
	.long	L23_0_set_7
	.long	L23_0_set_3
	.long	L23_0_set_19
	.long	L23_0_set_11
	.end_data_region
                                        ## -- End function
	.private_extern	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_ ## -- Begin function _ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.globl	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.weak_def_can_be_hidden	__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.p2align	4
__ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_: ## @_ZNSt3__119__partial_sort_implB9nqe220108INS_17_ClassicAlgPolicyERN8stx_zlib13DeflateStream11WeightOrderEPNS3_14WeightedSymbolES7_EET1_S8_S8_T2_OT0_
	.cfi_startproc
## %bb.0:
	movq	%rdx, %rax
	cmpq	%rsi, %rdi
	je	LBB24_52
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
	movq	%rax, -56(%rbp)                 ## 8-byte Spill
	movq	%rsi, -48(%rbp)                 ## 8-byte Spill
	movq	%rsi, %rcx
	subq	%rdi, %rcx
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	sarq	$3, %rcx
	movq	%rcx, %rsi
	subq	$2, %rsi
	jl	LBB24_16
## %bb.2:
	leaq	-2(%rcx), %rdx
	shrq	%rdx
	movq	%rdx, %r10
	jmp	LBB24_3
	.p2align	4
LBB24_14:                               ##   in Loop: Header=BB24_3 Depth=1
	movq	%rbx, (%rdi,%r15,8)
LBB24_15:                               ##   in Loop: Header=BB24_3 Depth=1
	testq	%r10, %r10
	leaq	-1(%r10), %r10
	jle	LBB24_16
LBB24_3:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB24_8 Depth 2
	cmpq	%r10, %rdx
	jb	LBB24_15
## %bb.4:                               ##   in Loop: Header=BB24_3 Depth=1
	leaq	1(,%r10,2), %r11
	leaq	2(,%r10,2), %rbx
	cmpq	%rcx, %rbx
	jae	LBB24_6
## %bb.5:                               ##   in Loop: Header=BB24_3 Depth=1
	leaq	(%r10,%r10), %r14
	movl	(%rdi,%r11,8), %r15d
	movl	4(%rdi,%r11,8), %r12d
	xorl	%r13d, %r13d
	cmpl	20(%rdi,%r14,8), %r12d
	setb	%r13b
	xorl	%r12d, %r12d
	cmpl	16(%rdi,%r14,8), %r15d
	setb	%r12b
	cmovel	%r13d, %r12d
	testb	%r12b, %r12b
	cmovneq	%rbx, %r11
LBB24_6:                                ##   in Loop: Header=BB24_3 Depth=1
	movl	(%rdi,%r11,8), %ebx
	movl	4(%rdi,%r11,8), %r14d
	xorl	%r15d, %r15d
	cmpl	4(%rdi,%r10,8), %r14d
	setb	%r15b
	xorl	%r14d, %r14d
	cmpl	(%rdi,%r10,8), %ebx
	setb	%r14b
	cmovel	%r15d, %r14d
	testb	%r14b, %r14b
	jne	LBB24_15
## %bb.7:                               ##   in Loop: Header=BB24_3 Depth=1
	movq	(%rdi,%r10,8), %rbx
	movq	%rbx, %r14
	shrq	$32, %r14
	movq	%r10, %r12
	jmp	LBB24_8
	.p2align	4
LBB24_12:                               ##   in Loop: Header=BB24_8 Depth=2
	movq	%r12, %r11
LBB24_13:                               ##   in Loop: Header=BB24_8 Depth=2
	xorl	%eax, %eax
	cmpl	%r14d, 4(%rdi,%r11,8)
	setb	%al
	xorl	%r8d, %r8d
	cmpl	%ebx, (%rdi,%r11,8)
	setb	%r8b
	cmovel	%eax, %r8d
	movq	%r15, %r12
	cmpb	$1, %r8b
	je	LBB24_14
LBB24_8:                                ##   Parent Loop BB24_3 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	%r11, %r15
	movq	(%rdi,%r11,8), %r11
	movq	%r11, (%rdi,%r12,8)
	cmpq	%r15, %rdx
	jl	LBB24_14
## %bb.9:                               ##   in Loop: Header=BB24_8 Depth=2
	leaq	1(,%r15,2), %r11
	leaq	2(,%r15,2), %r12
	cmpq	%rcx, %r12
	jge	LBB24_13
## %bb.10:                              ##   in Loop: Header=BB24_8 Depth=2
	leaq	(%r15,%r15), %r13
	movl	(%rdi,%r11,8), %r8d
	movl	4(%rdi,%r11,8), %r9d
	xorl	%eax, %eax
	cmpl	20(%rdi,%r13,8), %r9d
	setb	%al
	xorl	%r9d, %r9d
	cmpl	16(%rdi,%r13,8), %r8d
	setb	%r9b
	cmovel	%eax, %r9d
	testb	%r9b, %r9b
	jne	LBB24_12
## %bb.11:                              ##   in Loop: Header=BB24_8 Depth=2
	movq	%r11, %r12
	jmp	LBB24_12
LBB24_16:
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	movq	%rax, %rdx
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	cmpq	%r10, %rax
	je	LBB24_36
## %bb.17:
	cmpq	$2, %rcx
	jge	LBB24_18
## %bb.32:
	movl	(%rdi), %r8d
	movl	4(%rdi), %r9d
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	jmp	LBB24_33
	.p2align	4
LBB24_35:                               ##   in Loop: Header=BB24_33 Depth=1
	addq	$8, %rdx
	cmpq	%r10, %rdx
	je	LBB24_36
LBB24_33:                               ## =>This Inner Loop Header: Depth=1
	xorl	%eax, %eax
	cmpl	%r9d, 4(%rdx)
	setb	%al
	xorl	%esi, %esi
	cmpl	%r8d, (%rdx)
	setb	%sil
	cmovel	%eax, %esi
	cmpb	$1, %sil
	jne	LBB24_35
## %bb.34:                              ##   in Loop: Header=BB24_33 Depth=1
	movq	(%rdx), %r8
	movq	(%rdi), %rax
	movq	%rax, (%rdx)
	movq	%r8, (%rdi)
	movq	%r8, %r9
	shrq	$32, %r9
	jmp	LBB24_35
LBB24_18:
	shrq	%rsi
	movq	-48(%rbp), %rdx                 ## 8-byte Reload
	jmp	LBB24_19
	.p2align	4
LBB24_30:                               ##   in Loop: Header=BB24_19 Depth=1
	movq	%r10, (%rdi,%r14,8)
LBB24_31:                               ##   in Loop: Header=BB24_19 Depth=1
	addq	$8, %rdx
	cmpq	-56(%rbp), %rdx                 ## 8-byte Folded Reload
	je	LBB24_36
LBB24_19:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB24_24 Depth 2
	movl	(%rdx), %eax
	movl	4(%rdx), %r8d
	xorl	%r9d, %r9d
	cmpl	4(%rdi), %r8d
	setb	%r9b
	xorl	%r8d, %r8d
	cmpl	(%rdi), %eax
	setb	%r8b
	cmovel	%r9d, %r8d
	cmpb	$1, %r8b
	jne	LBB24_31
## %bb.20:                              ##   in Loop: Header=BB24_19 Depth=1
	movq	(%rdx), %r10
	movq	(%rdi), %rax
	movq	%rax, (%rdx)
	movq	%r10, (%rdi)
	movl	$1, %r11d
	cmpq	$16, -64(%rbp)                  ## 8-byte Folded Reload
	je	LBB24_22
## %bb.21:                              ##   in Loop: Header=BB24_19 Depth=1
	movl	8(%rdi), %eax
	movl	12(%rdi), %r8d
	xorl	%r9d, %r9d
	cmpl	20(%rdi), %r8d
	setb	%r9b
	xorl	%r8d, %r8d
	cmpl	16(%rdi), %eax
	setb	%r8b
	cmovel	%r9d, %r8d
	movzbl	%r8b, %r11d
	incq	%r11
LBB24_22:                               ##   in Loop: Header=BB24_19 Depth=1
	movq	%r10, %rbx
	shrq	$32, %rbx
	xorl	%eax, %eax
	cmpl	%ebx, 4(%rdi,%r11,8)
	setb	%al
	xorl	%r8d, %r8d
	cmpl	%r10d, (%rdi,%r11,8)
	setb	%r8b
	cmovel	%eax, %r8d
	testb	%r8b, %r8b
	jne	LBB24_31
## %bb.23:                              ##   in Loop: Header=BB24_19 Depth=1
	xorl	%r15d, %r15d
	jmp	LBB24_24
	.p2align	4
LBB24_28:                               ##   in Loop: Header=BB24_24 Depth=2
	movq	%r15, %r11
LBB24_29:                               ##   in Loop: Header=BB24_24 Depth=2
	xorl	%eax, %eax
	cmpl	%ebx, 4(%rdi,%r11,8)
	setb	%al
	xorl	%r8d, %r8d
	cmpl	%r10d, (%rdi,%r11,8)
	setb	%r8b
	cmovel	%eax, %r8d
	movq	%r14, %r15
	cmpb	$1, %r8b
	je	LBB24_30
LBB24_24:                               ##   Parent Loop BB24_19 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	%r11, %r14
	movq	(%rdi,%r11,8), %rax
	movq	%rax, (%rdi,%r15,8)
	cmpq	%r11, %rsi
	jl	LBB24_30
## %bb.25:                              ##   in Loop: Header=BB24_24 Depth=2
	leaq	1(,%r14,2), %r11
	leaq	2(,%r14,2), %r15
	cmpq	%rcx, %r15
	jge	LBB24_29
## %bb.26:                              ##   in Loop: Header=BB24_24 Depth=2
	leaq	(%r14,%r14), %rax
	movl	(%rdi,%r11,8), %r8d
	movl	4(%rdi,%r11,8), %r9d
	xorl	%r12d, %r12d
	cmpl	20(%rdi,%rax,8), %r9d
	setb	%r12b
	xorl	%r9d, %r9d
	cmpl	16(%rdi,%rax,8), %r8d
	setb	%r9b
	cmovel	%r12d, %r9d
	testb	%r9b, %r9b
	jne	LBB24_28
## %bb.27:                              ##   in Loop: Header=BB24_24 Depth=2
	movq	%r11, %r15
	jmp	LBB24_28
LBB24_36:
	cmpq	$2, %rcx
	jge	LBB24_37
LBB24_51:
	movq	%rdx, %rax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
LBB24_52:
	retq
	.p2align	4
LBB24_43:                               ##   in Loop: Header=BB24_37 Depth=1
	movq	%r8, (%rax)
LBB24_50:                               ##   in Loop: Header=BB24_37 Depth=1
	cmpq	$2, %rcx
	leaq	-1(%rcx), %rcx
	jle	LBB24_51
LBB24_37:                               ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB24_38 Depth 2
                                        ##     Child Loop BB24_47 Depth 2
	movq	(%rdi), %r8
	leaq	-2(%rcx), %r9
	shrq	%r9
	movq	%rdi, %r10
	xorl	%r14d, %r14d
	jmp	LBB24_38
	.p2align	4
LBB24_41:                               ##   in Loop: Header=BB24_38 Depth=2
	movq	(%rax), %rsi
	movq	%rsi, (%r10)
	movq	%rax, %r10
	movq	%r11, %r14
	cmpq	%r9, %r11
	jg	LBB24_42
LBB24_38:                               ##   Parent Loop BB24_37 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	leaq	(%r10,%r14,8), %rax
	addq	$8, %rax
	leaq	1(,%r14,2), %r11
	leaq	2(,%r14,2), %rbx
	cmpq	%rcx, %rbx
	jge	LBB24_41
## %bb.39:                              ##   in Loop: Header=BB24_38 Depth=2
	leaq	(%r10,%r14,8), %r14
	movl	8(%r14), %esi
	movl	12(%r14), %r15d
	xorl	%r12d, %r12d
	cmpl	20(%r14), %r15d
	setb	%r12b
	xorl	%r15d, %r15d
	cmpl	16(%r14), %esi
	setb	%r15b
	cmovel	%r12d, %r15d
	testb	%r15b, %r15b
	je	LBB24_41
## %bb.40:                              ##   in Loop: Header=BB24_38 Depth=2
	addq	$16, %r14
	movq	%r14, %rax
	movq	%rbx, %r11
	jmp	LBB24_41
	.p2align	4
LBB24_42:                               ##   in Loop: Header=BB24_37 Depth=1
	movq	-48(%rbp), %rsi                 ## 8-byte Reload
	addq	$-8, %rsi
	cmpq	%rsi, %rax
	movq	%rsi, -48(%rbp)                 ## 8-byte Spill
	je	LBB24_43
## %bb.44:                              ##   in Loop: Header=BB24_37 Depth=1
	movq	%rsi, %r9
	movq	(%rsi), %rsi
	movq	%rsi, (%rax)
	movq	%r8, (%r9)
	movq	%rax, %r8
	subq	%rdi, %r8
	addq	$8, %r8
	sarq	$3, %r8
	cmpq	$2, %r8
	jl	LBB24_50
## %bb.45:                              ##   in Loop: Header=BB24_37 Depth=1
	addq	$-2, %r8
	shrq	%r8
	movl	(%rdi,%r8,8), %esi
	movl	4(%rdi,%r8,8), %r9d
	xorl	%r10d, %r10d
	cmpl	4(%rax), %r9d
	setb	%r10b
	xorl	%r9d, %r9d
	cmpl	(%rax), %esi
	setb	%r9b
	cmovel	%r10d, %r9d
	cmpb	$1, %r9b
	jne	LBB24_50
## %bb.46:                              ##   in Loop: Header=BB24_37 Depth=1
	leaq	(%rdi,%r8,8), %r11
	movq	(%rax), %r9
	movq	%r9, %r10
	shrq	$32, %r10
	.p2align	4
LBB24_47:                               ##   Parent Loop BB24_37 Depth=1
                                        ## =>  This Inner Loop Header: Depth=2
	movq	%rax, %rsi
	movq	%r11, %rax
	movq	(%r11), %r11
	movq	%r11, (%rsi)
	testq	%r8, %r8
	je	LBB24_49
## %bb.48:                              ##   in Loop: Header=BB24_47 Depth=2
	decq	%r8
	shrq	%r8
	leaq	(%rdi,%r8,8), %r11
	xorl	%esi, %esi
	cmpl	%r10d, 4(%rdi,%r8,8)
	setb	%sil
	xorl	%ebx, %ebx
	cmpl	%r9d, (%rdi,%r8,8)
	setb	%bl
	cmovel	%esi, %ebx
	testb	%bl, %bl
	jne	LBB24_47
LBB24_49:                               ##   in Loop: Header=BB24_37 Depth=1
	movq	%r9, (%rax)
	jmp	LBB24_50
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ## @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.chains:
	.long	0                               ## 0x0
	.long	4                               ## 0x4
	.long	8                               ## 0x8
	.long	32                              ## 0x20
	.long	16                              ## 0x10
	.long	32                              ## 0x20
	.long	128                             ## 0x80
	.long	256                             ## 0x100
	.long	1024                            ## 0x400
	.long	4096                            ## 0x1000

	.p2align	2, 0x0                          ## @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.nice:
	.long	0                               ## 0x0
	.long	8                               ## 0x8
	.long	16                              ## 0x10
	.long	32                              ## 0x20
	.long	16                              ## 0x10
	.long	32                              ## 0x20
	.long	128                             ## 0x80
	.long	128                             ## 0x80
	.long	258                             ## 0x102
	.long	258                             ## 0x102

	.p2align	2, 0x0                          ## @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.good:
	.long	0                               ## 0x0
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	32                              ## 0x20
	.long	32                              ## 0x20

	.p2align	2, 0x0                          ## @__const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits
l___const._ZN8stx_zlib13DeflateStream5resetERKNS_14DeflateOptionsE.lazy_limits:
	.long	0                               ## 0x0
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	6                               ## 0x6
	.long	4                               ## 0x4
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	32                              ## 0x20
	.long	128                             ## 0x80
	.long	258                             ## 0x102

	.p2align	2, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_114length_symbolsE
__ZN8stx_zlib12_GLOBAL__N_114length_symbolsE:
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	3                               ## 0x3
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	6                               ## 0x6
	.long	7                               ## 0x7
	.long	8                               ## 0x8
	.long	8                               ## 0x8
	.long	9                               ## 0x9
	.long	9                               ## 0x9
	.long	10                              ## 0xa
	.long	10                              ## 0xa
	.long	11                              ## 0xb
	.long	11                              ## 0xb
	.long	12                              ## 0xc
	.long	12                              ## 0xc
	.long	12                              ## 0xc
	.long	12                              ## 0xc
	.long	13                              ## 0xd
	.long	13                              ## 0xd
	.long	13                              ## 0xd
	.long	13                              ## 0xd
	.long	14                              ## 0xe
	.long	14                              ## 0xe
	.long	14                              ## 0xe
	.long	14                              ## 0xe
	.long	15                              ## 0xf
	.long	15                              ## 0xf
	.long	15                              ## 0xf
	.long	15                              ## 0xf
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	16                              ## 0x10
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	17                              ## 0x11
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	18                              ## 0x12
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	19                              ## 0x13
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	20                              ## 0x14
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	21                              ## 0x15
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	22                              ## 0x16
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	23                              ## 0x17
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	24                              ## 0x18
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	25                              ## 0x19
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	26                              ## 0x1a
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	27                              ## 0x1b
	.long	28                              ## 0x1c

	.p2align	2, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_112length_extraE
__ZN8stx_zlib12_GLOBAL__N_112length_extraE:
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	1                               ## 0x1
	.long	1                               ## 0x1
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	2                               ## 0x2
	.long	2                               ## 0x2
	.long	2                               ## 0x2
	.long	3                               ## 0x3
	.long	3                               ## 0x3
	.long	3                               ## 0x3
	.long	3                               ## 0x3
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	5                               ## 0x5
	.long	5                               ## 0x5
	.long	5                               ## 0x5
	.long	0                               ## 0x0

	.p2align	2, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_110code_orderE
__ZN8stx_zlib12_GLOBAL__N_110code_orderE:
	.long	16                              ## 0x10
	.long	17                              ## 0x11
	.long	18                              ## 0x12
	.long	0                               ## 0x0
	.long	8                               ## 0x8
	.long	7                               ## 0x7
	.long	9                               ## 0x9
	.long	6                               ## 0x6
	.long	10                              ## 0xa
	.long	5                               ## 0x5
	.long	11                              ## 0xb
	.long	4                               ## 0x4
	.long	12                              ## 0xc
	.long	3                               ## 0x3
	.long	13                              ## 0xd
	.long	2                               ## 0x2
	.long	14                              ## 0xe
	.long	1                               ## 0x1
	.long	15                              ## 0xf

	.p2align	1, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE
__ZN8stx_zlib12_GLOBAL__N_112fixed_tablesE:
	.ascii	"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\007\b\b\b\b\b\b"
	.short	12                              ## 0xc
	.short	140                             ## 0x8c
	.short	76                              ## 0x4c
	.short	204                             ## 0xcc
	.short	44                              ## 0x2c
	.short	172                             ## 0xac
	.short	108                             ## 0x6c
	.short	236                             ## 0xec
	.short	28                              ## 0x1c
	.short	156                             ## 0x9c
	.short	92                              ## 0x5c
	.short	220                             ## 0xdc
	.short	60                              ## 0x3c
	.short	188                             ## 0xbc
	.short	124                             ## 0x7c
	.short	252                             ## 0xfc
	.short	2                               ## 0x2
	.short	130                             ## 0x82
	.short	66                              ## 0x42
	.short	194                             ## 0xc2
	.short	34                              ## 0x22
	.short	162                             ## 0xa2
	.short	98                              ## 0x62
	.short	226                             ## 0xe2
	.short	18                              ## 0x12
	.short	146                             ## 0x92
	.short	82                              ## 0x52
	.short	210                             ## 0xd2
	.short	50                              ## 0x32
	.short	178                             ## 0xb2
	.short	114                             ## 0x72
	.short	242                             ## 0xf2
	.short	10                              ## 0xa
	.short	138                             ## 0x8a
	.short	74                              ## 0x4a
	.short	202                             ## 0xca
	.short	42                              ## 0x2a
	.short	170                             ## 0xaa
	.short	106                             ## 0x6a
	.short	234                             ## 0xea
	.short	26                              ## 0x1a
	.short	154                             ## 0x9a
	.short	90                              ## 0x5a
	.short	218                             ## 0xda
	.short	58                              ## 0x3a
	.short	186                             ## 0xba
	.short	122                             ## 0x7a
	.short	250                             ## 0xfa
	.short	6                               ## 0x6
	.short	134                             ## 0x86
	.short	70                              ## 0x46
	.short	198                             ## 0xc6
	.short	38                              ## 0x26
	.short	166                             ## 0xa6
	.short	102                             ## 0x66
	.short	230                             ## 0xe6
	.short	22                              ## 0x16
	.short	150                             ## 0x96
	.short	86                              ## 0x56
	.short	214                             ## 0xd6
	.short	54                              ## 0x36
	.short	182                             ## 0xb6
	.short	118                             ## 0x76
	.short	246                             ## 0xf6
	.short	14                              ## 0xe
	.short	142                             ## 0x8e
	.short	78                              ## 0x4e
	.short	206                             ## 0xce
	.short	46                              ## 0x2e
	.short	174                             ## 0xae
	.short	110                             ## 0x6e
	.short	238                             ## 0xee
	.short	30                              ## 0x1e
	.short	158                             ## 0x9e
	.short	94                              ## 0x5e
	.short	222                             ## 0xde
	.short	62                              ## 0x3e
	.short	190                             ## 0xbe
	.short	126                             ## 0x7e
	.short	254                             ## 0xfe
	.short	1                               ## 0x1
	.short	129                             ## 0x81
	.short	65                              ## 0x41
	.short	193                             ## 0xc1
	.short	33                              ## 0x21
	.short	161                             ## 0xa1
	.short	97                              ## 0x61
	.short	225                             ## 0xe1
	.short	17                              ## 0x11
	.short	145                             ## 0x91
	.short	81                              ## 0x51
	.short	209                             ## 0xd1
	.short	49                              ## 0x31
	.short	177                             ## 0xb1
	.short	113                             ## 0x71
	.short	241                             ## 0xf1
	.short	9                               ## 0x9
	.short	137                             ## 0x89
	.short	73                              ## 0x49
	.short	201                             ## 0xc9
	.short	41                              ## 0x29
	.short	169                             ## 0xa9
	.short	105                             ## 0x69
	.short	233                             ## 0xe9
	.short	25                              ## 0x19
	.short	153                             ## 0x99
	.short	89                              ## 0x59
	.short	217                             ## 0xd9
	.short	57                              ## 0x39
	.short	185                             ## 0xb9
	.short	121                             ## 0x79
	.short	249                             ## 0xf9
	.short	5                               ## 0x5
	.short	133                             ## 0x85
	.short	69                              ## 0x45
	.short	197                             ## 0xc5
	.short	37                              ## 0x25
	.short	165                             ## 0xa5
	.short	101                             ## 0x65
	.short	229                             ## 0xe5
	.short	21                              ## 0x15
	.short	149                             ## 0x95
	.short	85                              ## 0x55
	.short	213                             ## 0xd5
	.short	53                              ## 0x35
	.short	181                             ## 0xb5
	.short	117                             ## 0x75
	.short	245                             ## 0xf5
	.short	13                              ## 0xd
	.short	141                             ## 0x8d
	.short	77                              ## 0x4d
	.short	205                             ## 0xcd
	.short	45                              ## 0x2d
	.short	173                             ## 0xad
	.short	109                             ## 0x6d
	.short	237                             ## 0xed
	.short	29                              ## 0x1d
	.short	157                             ## 0x9d
	.short	93                              ## 0x5d
	.short	221                             ## 0xdd
	.short	61                              ## 0x3d
	.short	189                             ## 0xbd
	.short	125                             ## 0x7d
	.short	253                             ## 0xfd
	.short	19                              ## 0x13
	.short	275                             ## 0x113
	.short	147                             ## 0x93
	.short	403                             ## 0x193
	.short	83                              ## 0x53
	.short	339                             ## 0x153
	.short	211                             ## 0xd3
	.short	467                             ## 0x1d3
	.short	51                              ## 0x33
	.short	307                             ## 0x133
	.short	179                             ## 0xb3
	.short	435                             ## 0x1b3
	.short	115                             ## 0x73
	.short	371                             ## 0x173
	.short	243                             ## 0xf3
	.short	499                             ## 0x1f3
	.short	11                              ## 0xb
	.short	267                             ## 0x10b
	.short	139                             ## 0x8b
	.short	395                             ## 0x18b
	.short	75                              ## 0x4b
	.short	331                             ## 0x14b
	.short	203                             ## 0xcb
	.short	459                             ## 0x1cb
	.short	43                              ## 0x2b
	.short	299                             ## 0x12b
	.short	171                             ## 0xab
	.short	427                             ## 0x1ab
	.short	107                             ## 0x6b
	.short	363                             ## 0x16b
	.short	235                             ## 0xeb
	.short	491                             ## 0x1eb
	.short	27                              ## 0x1b
	.short	283                             ## 0x11b
	.short	155                             ## 0x9b
	.short	411                             ## 0x19b
	.short	91                              ## 0x5b
	.short	347                             ## 0x15b
	.short	219                             ## 0xdb
	.short	475                             ## 0x1db
	.short	59                              ## 0x3b
	.short	315                             ## 0x13b
	.short	187                             ## 0xbb
	.short	443                             ## 0x1bb
	.short	123                             ## 0x7b
	.short	379                             ## 0x17b
	.short	251                             ## 0xfb
	.short	507                             ## 0x1fb
	.short	7                               ## 0x7
	.short	263                             ## 0x107
	.short	135                             ## 0x87
	.short	391                             ## 0x187
	.short	71                              ## 0x47
	.short	327                             ## 0x147
	.short	199                             ## 0xc7
	.short	455                             ## 0x1c7
	.short	39                              ## 0x27
	.short	295                             ## 0x127
	.short	167                             ## 0xa7
	.short	423                             ## 0x1a7
	.short	103                             ## 0x67
	.short	359                             ## 0x167
	.short	231                             ## 0xe7
	.short	487                             ## 0x1e7
	.short	23                              ## 0x17
	.short	279                             ## 0x117
	.short	151                             ## 0x97
	.short	407                             ## 0x197
	.short	87                              ## 0x57
	.short	343                             ## 0x157
	.short	215                             ## 0xd7
	.short	471                             ## 0x1d7
	.short	55                              ## 0x37
	.short	311                             ## 0x137
	.short	183                             ## 0xb7
	.short	439                             ## 0x1b7
	.short	119                             ## 0x77
	.short	375                             ## 0x177
	.short	247                             ## 0xf7
	.short	503                             ## 0x1f7
	.short	15                              ## 0xf
	.short	271                             ## 0x10f
	.short	143                             ## 0x8f
	.short	399                             ## 0x18f
	.short	79                              ## 0x4f
	.short	335                             ## 0x14f
	.short	207                             ## 0xcf
	.short	463                             ## 0x1cf
	.short	47                              ## 0x2f
	.short	303                             ## 0x12f
	.short	175                             ## 0xaf
	.short	431                             ## 0x1af
	.short	111                             ## 0x6f
	.short	367                             ## 0x16f
	.short	239                             ## 0xef
	.short	495                             ## 0x1ef
	.short	31                              ## 0x1f
	.short	287                             ## 0x11f
	.short	159                             ## 0x9f
	.short	415                             ## 0x19f
	.short	95                              ## 0x5f
	.short	351                             ## 0x15f
	.short	223                             ## 0xdf
	.short	479                             ## 0x1df
	.short	63                              ## 0x3f
	.short	319                             ## 0x13f
	.short	191                             ## 0xbf
	.short	447                             ## 0x1bf
	.short	127                             ## 0x7f
	.short	383                             ## 0x17f
	.short	255                             ## 0xff
	.short	511                             ## 0x1ff
	.short	0                               ## 0x0
	.short	64                              ## 0x40
	.short	32                              ## 0x20
	.short	96                              ## 0x60
	.short	16                              ## 0x10
	.short	80                              ## 0x50
	.short	48                              ## 0x30
	.short	112                             ## 0x70
	.short	8                               ## 0x8
	.short	72                              ## 0x48
	.short	40                              ## 0x28
	.short	104                             ## 0x68
	.short	24                              ## 0x18
	.short	88                              ## 0x58
	.short	56                              ## 0x38
	.short	120                             ## 0x78
	.short	4                               ## 0x4
	.short	68                              ## 0x44
	.short	36                              ## 0x24
	.short	100                             ## 0x64
	.short	20                              ## 0x14
	.short	84                              ## 0x54
	.short	52                              ## 0x34
	.short	116                             ## 0x74
	.short	3                               ## 0x3
	.short	131                             ## 0x83
	.short	67                              ## 0x43
	.short	195                             ## 0xc3
	.short	35                              ## 0x23
	.short	163                             ## 0xa3
	.space	30,5
	.short	0                               ## 0x0
	.short	16                              ## 0x10
	.short	8                               ## 0x8
	.short	24                              ## 0x18
	.short	4                               ## 0x4
	.short	20                              ## 0x14
	.short	12                              ## 0xc
	.short	28                              ## 0x1c
	.short	2                               ## 0x2
	.short	18                              ## 0x12
	.short	10                              ## 0xa
	.short	26                              ## 0x1a
	.short	6                               ## 0x6
	.short	22                              ## 0x16
	.short	14                              ## 0xe
	.short	30                              ## 0x1e
	.short	1                               ## 0x1
	.short	17                              ## 0x11
	.short	9                               ## 0x9
	.short	25                              ## 0x19
	.short	5                               ## 0x5
	.short	21                              ## 0x15
	.short	13                              ## 0xd
	.short	29                              ## 0x1d
	.short	3                               ## 0x3
	.short	19                              ## 0x13
	.short	11                              ## 0xb
	.short	27                              ## 0x1b
	.short	7                               ## 0x7
	.short	23                              ## 0x17

	.p2align	2, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_111length_baseE
__ZN8stx_zlib12_GLOBAL__N_111length_baseE:
	.long	3                               ## 0x3
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	6                               ## 0x6
	.long	7                               ## 0x7
	.long	8                               ## 0x8
	.long	9                               ## 0x9
	.long	10                              ## 0xa
	.long	11                              ## 0xb
	.long	13                              ## 0xd
	.long	15                              ## 0xf
	.long	17                              ## 0x11
	.long	19                              ## 0x13
	.long	23                              ## 0x17
	.long	27                              ## 0x1b
	.long	31                              ## 0x1f
	.long	35                              ## 0x23
	.long	43                              ## 0x2b
	.long	51                              ## 0x33
	.long	59                              ## 0x3b
	.long	67                              ## 0x43
	.long	83                              ## 0x53
	.long	99                              ## 0x63
	.long	115                             ## 0x73
	.long	131                             ## 0x83
	.long	163                             ## 0xa3
	.long	195                             ## 0xc3
	.long	227                             ## 0xe3
	.long	258                             ## 0x102

	.p2align	2, 0x0                          ## @_ZN8stx_zlib12_GLOBAL__N_113distance_baseE
__ZN8stx_zlib12_GLOBAL__N_113distance_baseE:
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	3                               ## 0x3
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	7                               ## 0x7
	.long	9                               ## 0x9
	.long	13                              ## 0xd
	.long	17                              ## 0x11
	.long	25                              ## 0x19
	.long	33                              ## 0x21
	.long	49                              ## 0x31
	.long	65                              ## 0x41
	.long	97                              ## 0x61
	.long	129                             ## 0x81
	.long	193                             ## 0xc1
	.long	257                             ## 0x101
	.long	385                             ## 0x181
	.long	513                             ## 0x201
	.long	769                             ## 0x301
	.long	1025                            ## 0x401
	.long	1537                            ## 0x601
	.long	2049                            ## 0x801
	.long	3073                            ## 0xc01
	.long	4097                            ## 0x1001
	.long	6145                            ## 0x1801
	.long	8193                            ## 0x2001
	.long	12289                           ## 0x3001
	.long	16385                           ## 0x4001
	.long	24577                           ## 0x6001

.subsections_via_symbols
