	.build_version macos, 14, 0	sdk_version 26, 5
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE ## -- Begin function _ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	.p2align	4
__ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE: ## @_ZN9gui_forms11compression13InflateStream5resetERKNS0_14InflateOptionsE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$1, %eax
	cmpl	$1, (%rsi)
	ja	LBB0_6
## %bb.1:
	movl	4(%rsi), %ecx
	andl	$-8, %ecx
	cmpl	$8, %ecx
	jne	LBB0_6
## %bb.2:
	cmpq	$-1, 8(%rsi)
	je	LBB0_6
## %bb.3:
	cmpq	$-1, 16(%rsi)
	je	LBB0_6
## %bb.4:
	cmpq	$-1, 24(%rsi)
	je	LBB0_6
## %bb.5:
	movups	(%rsi), %xmm0
	movups	16(%rsi), %xmm1
	movups	%xmm1, 16(%rdi)
	movups	%xmm0, (%rdi)
	xorl	%eax, %eax
	cmpl	$1, (%rsi)
	sete	%al
	leal	1(,%rax,2), %eax
	movl	%eax, 32(%rdi)
	xorps	%xmm0, %xmm0
	movups	%xmm0, 48(%rdi)
	movups	%xmm0, 36(%rdi)
	movups	%xmm0, 32832(%rdi)
	movzbl	4(%rsi), %ecx
	movl	$1, %eax
	shlq	%cl, %rax
	movq	%rax, 32848(%rdi)
	movl	$1, 38104(%rdi)
	movups	%xmm0, 38108(%rdi)
	movups	%xmm0, 38124(%rdi)
	movups	%xmm0, 38140(%rdi)
	movups	%xmm0, 38153(%rdi)
	movl	$1, 38172(%rdi)
	movups	%xmm0, 38176(%rdi)
	xorl	%eax, %eax
LBB0_6:
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE ## -- Begin function _ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE
	.p2align	4
__ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE: ## @_ZN9gui_forms11compression13InflateStream4failENS0_12InflateErrorE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	%esi, 36(%rdi)
	movl	$19, 32(%rdi)
	xorl	%eax, %eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream6chargeEy ## -- Begin function _ZN9gui_forms11compression13InflateStream6chargeEy
	.p2align	4
__ZN9gui_forms11compression13InflateStream6chargeEy: ## @_ZN9gui_forms11compression13InflateStream6chargeEy
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	24(%rdi), %rax
	movq	56(%rdi), %rcx
	subq	%rcx, %rax
	cmpq	%rax, %rsi
	ja	LBB2_1
## %bb.2:
	addq	%rsi, %rcx
	movq	%rcx, 56(%rdi)
	cmpq	%rax, %rsi
	setbe	%al
	popq	%rbp
	retq
LBB2_1:
	movabsq	$21474836499, %rcx              ## imm = 0x500000013
	movq	%rcx, 32(%rdi)
	cmpq	%rax, %rsi
	setbe	%al
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj ## -- Begin function _ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
	.p2align	4
__ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj: ## @_ZN9gui_forms11compression13InflateStream4bitsERNS1_4CallEjRj
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
	movq	%rcx, %r9
	movl	38124(%rdi), %ecx
	cmpl	%edx, %ecx
	jae	LBB3_8
## %bb.1:
	movq	%r9, -56(%rbp)                  ## 8-byte Spill
	movq	24(%rdi), %rax
	movq	40(%rdi), %r10
	movq	32(%rsi), %r9
	movq	56(%rdi), %r8
	movl	38120(%rdi), %ebx
	movq	%r8, %r14
	movq	%rax, -48(%rbp)                 ## 8-byte Spill
	subq	%rax, %r14
	movq	%r10, %r15
	subq	8(%rdi), %r15
	movq	(%rsi), %r12
	addq	%r9, %r12
	movq	%r9, %rax
	subq	8(%rsi), %rax
	xorl	%r13d, %r13d
	.p2align	4
LBB3_2:                                 ## =>This Inner Loop Header: Depth=1
	movq	%rax, %r11
	addq	%r13, %r11
	je	LBB3_10
## %bb.3:                               ##   in Loop: Header=BB3_2 Depth=1
	movq	%r15, %r11
	addq	%r13, %r11
	je	LBB3_13
## %bb.4:                               ##   in Loop: Header=BB3_2 Depth=1
	movq	%r14, %r11
	addq	%r13, %r11
	je	LBB3_9
## %bb.5:                               ##   in Loop: Header=BB3_2 Depth=1
	leaq	(%r8,%r13), %r11
	incq	%r11
	movq	%r11, 56(%rdi)
	movzbl	(%r12,%r13), %r11d
	shll	%cl, %r11d
	orl	%r11d, %ebx
	movl	%ebx, 38120(%rdi)
	addl	$8, %ecx
	movl	%ecx, 38124(%rdi)
	leaq	(%r9,%r13), %r11
	incq	%r11
	movq	%r11, 32(%rsi)
	leaq	(%r10,%r13), %r11
	incq	%r11
	movq	%r11, 40(%rdi)
	incq	%r13
	cmpl	%edx, %ecx
	jb	LBB3_2
## %bb.6:
	addq	%r13, %r8
	movq	-56(%rbp), %r9                  ## 8-byte Reload
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	cmpq	%r8, %rax
	je	LBB3_9
LBB3_7:
	incq	%r8
	movl	$-1, %eax
	movl	%edx, %ecx
	shll	%cl, %eax
	movq	%r8, 56(%rdi)
	notl	%eax
	andl	38120(%rdi), %eax
	movl	%eax, (%r9)
	shrl	%cl, 38120(%rdi)
	subl	%edx, 38124(%rdi)
	movb	$1, %al
	jmp	LBB3_16
LBB3_8:
	movq	24(%rdi), %rax
	movq	56(%rdi), %r8
	cmpq	%r8, %rax
	jne	LBB3_7
LBB3_9:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	jmp	LBB3_14
LBB3_10:
	cmpb	$1, 48(%rsi)
	jne	LBB3_12
## %bb.11:
	movabsq	$25769803795, %rax              ## imm = 0x600000013
	movq	%rax, 32(%rdi)
LBB3_12:
	movl	$0, 52(%rsi)
	jmp	LBB3_15
LBB3_13:
	movabsq	$12884901907, %rax              ## imm = 0x300000013
LBB3_14:
	movq	%rax, 32(%rdi)
LBB3_15:
	xorl	%eax, %eax
LBB3_16:
                                        ## kill: def $al killed $al killed $eax
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
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
LCPI4_0:
	.long	1                               ## 0x1
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.section	__TEXT,__literal4,4byte_literals
	.p2align	2, 0x0
LCPI4_1:
	.long	1                               ## 0x1
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	.p2align	4
__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj: ## @_ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
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
	movl	38104(%rdi), %r9d
	cmpl	$1, %r9d
	jne	LBB4_28
## %bb.1:
	movl	38124(%rdi), %eax
	movl	$16, %r8d
	subl	%eax, %r8d
	shrl	$3, %r8d
	movq	8(%rsi), %r11
	movq	32(%rsi), %rbx
	movq	%r11, %r10
	subq	%rbx, %r10
	cmpq	%r8, %r10
	jb	LBB4_28
## %bb.2:
	movq	8(%rdi), %r12
	movq	40(%rdi), %r14
	movq	%r12, %r10
	subq	%r14, %r10
	cmpq	%r8, %r10
	jae	LBB4_3
LBB4_28:
	movl	1632(%rdx), %eax
	cmpl	%eax, %r9d
	jbe	LBB4_29
LBB4_41:
	movabsq	$77309411347, %rax              ## imm = 0x1200000013
	jmp	LBB4_42
LBB4_29:
	movq	24(%rdi), %r8
	movq	8(%rsi), %r10
	movl	38124(%rdi), %r11d
	jmp	LBB4_30
	.p2align	4
LBB4_40:                                ##   in Loop: Header=BB4_30 Depth=1
	addl	%r15d, 38116(%rdi)
	addl	%r14d, %r14d
	movl	%r14d, 38112(%rdi)
	addl	%ebx, %ebx
	movl	%ebx, 38108(%rdi)
	incl	%r9d
	movl	%r9d, 38104(%rdi)
	cmpl	%eax, %r9d
	ja	LBB4_41
LBB4_30:                                ## =>This Inner Loop Header: Depth=1
	testl	%r11d, %r11d
	je	LBB4_32
## %bb.31:                              ##   in Loop: Header=BB4_30 Depth=1
	movq	56(%rdi), %rbx
	decl	%r11d
	movq	%r8, %r14
	cmpq	%rbx, %r14
	jne	LBB4_37
	jmp	LBB4_4
	.p2align	4
LBB4_32:                                ##   in Loop: Header=BB4_30 Depth=1
	movq	32(%rsi), %r11
	cmpq	%r10, %r11
	je	LBB4_10
## %bb.33:                              ##   in Loop: Header=BB4_30 Depth=1
	movq	40(%rdi), %r15
	cmpq	8(%rdi), %r15
	je	LBB4_14
## %bb.34:                              ##   in Loop: Header=BB4_30 Depth=1
	movq	24(%rdi), %r14
	movq	56(%rdi), %rbx
	cmpq	%rbx, %r14
	je	LBB4_4
## %bb.35:                              ##   in Loop: Header=BB4_30 Depth=1
	movq	(%rsi), %r12
	movl	38120(%rdi), %r13d
	incq	%rbx
	movq	%rbx, 56(%rdi)
	movzbl	(%r12,%r11), %r12d
	orl	%r13d, %r12d
	movl	%r12d, 38120(%rdi)
	movl	$8, 38124(%rdi)
	incq	%r11
	movq	%r11, 32(%rsi)
	incq	%r15
	movq	%r15, 40(%rdi)
	movl	$7, %r11d
	cmpq	%rbx, %r14
	je	LBB4_4
LBB4_37:                                ##   in Loop: Header=BB4_30 Depth=1
	incq	%rbx
	movq	%rbx, 56(%rdi)
	movl	38112(%rdi), %r13d
	movl	38120(%rdi), %r14d
	movl	%r14d, %ebx
	andl	$1, %ebx
	shrl	%r14d
	movl	%r14d, 38120(%rdi)
	movl	%r11d, 38124(%rdi)
	orl	38108(%rdi), %ebx
	movl	%r9d, %r14d
	movzwl	(%rdx,%r14,2), %r15d
	leal	(%r15,%r13), %r14d
	movl	%ebx, %r12d
	subl	%r13d, %r12d
	jb	LBB4_40
## %bb.38:                              ##   in Loop: Header=BB4_30 Depth=1
	cmpl	%r14d, %ebx
	jae	LBB4_40
## %bb.39:
	addl	38116(%rdi), %r12d
	movzwl	32(%rdx,%r12,2), %eax
	movl	%eax, (%rcx)
	movss	LCPI4_1(%rip), %xmm0            ## xmm0 = [1,0,0,0]
	movups	%xmm0, 38104(%rdi)
	movb	$1, %al
	jmp	LBB4_44
LBB4_3:
	movq	24(%rdi), %r10
	movq	56(%rdi), %r15
	cmpq	%r15, %r10
	jne	LBB4_5
LBB4_4:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
LBB4_42:
	movq	%rax, 32(%rdi)
LBB4_43:
	xorl	%eax, %eax
LBB4_44:
                                        ## kill: def $al killed $al killed $eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB4_5:
	movq	%r10, -72(%rbp)                 ## 8-byte Spill
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	leaq	1(%r15), %rcx
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	movq	%rcx, 56(%rdi)
	movq	(%rsi), %r10
	movzbl	(%r10,%rbx), %r13d
	movl	%eax, %ecx
	shll	%cl, %r13d
	movl	38120(%rdi), %ecx
	movl	%ecx, -44(%rbp)                 ## 4-byte Spill
	orl	%ecx, %r13d
	cmpl	$2, %r8d
	movq	%r10, -80(%rbp)                 ## 8-byte Spill
	jne	LBB4_7
## %bb.6:
	movzbl	1(%r10,%rbx), %r8d
	leal	8(%rax), %ecx
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r8d
	orl	%r13d, %r8d
	movl	%r8d, %r13d
LBB4_7:
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	andl	$511, %r13d                     ## imm = 0x1FF
	movzwl	608(%rdx,%r13,2), %r13d
	movl	%r13d, %r8d
	andl	$15, %r8d
	je	LBB4_28
## %bb.8:
	cmpl	%r8d, %eax
	jae	LBB4_26
## %bb.9:
	cmpq	%rbx, %r11
	jne	LBB4_13
LBB4_10:
	cmpb	$1, 48(%rsi)
	jne	LBB4_12
## %bb.11:
	movabsq	$25769803795, %rax              ## imm = 0x600000013
	movq	%rax, 32(%rdi)
LBB4_12:
	movl	$0, 52(%rsi)
	jmp	LBB4_43
LBB4_13:
	cmpq	%r14, %r12
	movq	-80(%rbp), %rdx                 ## 8-byte Reload
	jne	LBB4_15
LBB4_14:
	movabsq	$12884901907, %rax              ## imm = 0x300000013
	jmp	LBB4_42
LBB4_15:
	leaq	1(%r15), %rcx
	cmpq	%rcx, -72(%rbp)                 ## 8-byte Folded Reload
	je	LBB4_4
## %bb.16:
	leaq	2(%r15), %rcx
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	movq	%rcx, 56(%rdi)
	movzbl	(%rdx,%rbx), %edx
	movl	%eax, %ecx
	shll	%cl, %edx
	movl	-44(%rbp), %ecx                 ## 4-byte Reload
	orl	%edx, %ecx
	movl	%ecx, -44(%rbp)                 ## 4-byte Spill
	movl	%ecx, 38120(%rdi)
	leal	8(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	1(%rbx), %r9
	movq	%r9, 32(%rsi)
	leaq	1(%r14), %rdx
	movq	%rdx, 40(%rdi)
	cmpl	%r8d, %ecx
	jae	LBB4_17
## %bb.18:
	cmpq	%r11, %r9
	je	LBB4_10
## %bb.19:
	cmpq	%r12, %rdx
	je	LBB4_14
## %bb.20:
	leaq	2(%r15), %rdx
	cmpq	%rdx, -72(%rbp)                 ## 8-byte Folded Reload
	movq	-80(%rbp), %rdx                 ## 8-byte Reload
	je	LBB4_4
## %bb.21:
	movq	%rdx, %r9
	leaq	3(%r15), %rdx
	movq	%rdx, -64(%rbp)                 ## 8-byte Spill
	movq	%rdx, 56(%rdi)
	movzbl	1(%r9,%rbx), %edx
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edx
	movl	-44(%rbp), %ecx                 ## 4-byte Reload
	orl	%edx, %ecx
	movl	%ecx, -44(%rbp)                 ## 4-byte Spill
	movl	%ecx, 38120(%rdi)
	leal	16(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	2(%rbx), %r9
	movq	%r9, 32(%rsi)
	leaq	2(%r14), %rdx
	movq	%rdx, 40(%rdi)
	cmpl	%r8d, %ecx
	jae	LBB4_17
## %bb.22:
	cmpq	%r11, %r9
	je	LBB4_10
## %bb.23:
	cmpq	%r12, %rdx
	je	LBB4_14
## %bb.24:
	leaq	3(%r15), %rdx
	cmpq	%rdx, -72(%rbp)                 ## 8-byte Folded Reload
	movq	-80(%rbp), %rdx                 ## 8-byte Reload
	je	LBB4_4
## %bb.25:
	addq	$4, %r15
	movq	%r15, 56(%rdi)
	movzbl	2(%rdx,%rbx), %edx
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %edx
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
	movl	-44(%rbp), %r9d                 ## 4-byte Reload
	orl	%edx, %r9d
	movl	%r9d, -44(%rbp)                 ## 4-byte Spill
	movl	%r9d, 38120(%rdi)
	addl	$24, %eax
	movl	%eax, 38124(%rdi)
	addq	$3, %rbx
	movq	%rbx, 32(%rsi)
	addq	$3, %r14
	movq	%r14, 40(%rdi)
	movq	%r15, -64(%rbp)                 ## 8-byte Spill
	jmp	LBB4_26
LBB4_17:
	movl	%ecx, %eax
	movq	-56(%rbp), %rcx                 ## 8-byte Reload
LBB4_26:
	movq	-64(%rbp), %r9                  ## 8-byte Reload
	cmpq	%r9, -72(%rbp)                  ## 8-byte Folded Reload
	je	LBB4_4
## %bb.27:
	movq	%rcx, %rdx
	movl	%r8d, %ecx
	movl	-44(%rbp), %esi                 ## 4-byte Reload
	shrl	%cl, %esi
	incq	%r9
	movq	%r9, 56(%rdi)
	movl	%esi, 38120(%rdi)
	subl	%r8d, %eax
	movl	%eax, 38124(%rdi)
	shrl	$4, %r13d
	movl	%r13d, (%rdx)
	movb	$1, %al
	jmp	LBB4_44
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb ## -- Begin function _ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	.p2align	4
__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb: ## @_ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
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
	subq	$136, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rcx, %r12
	movq	%rdx, %r15
	movq	%rdi, %r13
	leaq	(%rcx,%rcx,4), %rax
	leaq	(%rcx,%rax,2), %rax
	addq	$1088, %rax                     ## imm = 0x440
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rcx
	subq	%rcx, %rdx
	cmpq	%rdx, %rax
	jbe	LBB5_3
## %bb.1:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	jmp	LBB5_2
LBB5_3:
	movl	%r8d, %r14d
	movq	%rsi, %rbx
	addq	%rax, %rcx
	movq	%rcx, 56(%r13)
	leaq	608(%rsi), %rdi
	xorps	%xmm0, %xmm0
	movups	%xmm0, 16(%rsi)
	movups	%xmm0, (%rsi)
	movl	$1028, %esi                     ## imm = 0x404
	callq	___bzero
	testq	%r12, %r12
	je	LBB5_35
## %bb.4:
	movq	%r12, %rcx
	decq	%rcx
	je	LBB5_5
## %bb.29:
	movq	%r12, %rsi
	andq	$-2, %rsi
	xorl	%edx, %edx
	xorl	%eax, %eax
	jmp	LBB5_30
	.p2align	4
LBB5_34:                                ##   in Loop: Header=BB5_30 Depth=1
	addq	$2, %rdx
	cmpq	%rdx, %rsi
	je	LBB5_6
LBB5_30:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%r15,%rdx), %edi
	incw	(%rbx,%rdi,2)
	cmpl	%edi, %eax
	jb	LBB5_31
## %bb.32:                              ##   in Loop: Header=BB5_30 Depth=1
	movzbl	1(%r15,%rdx), %edi
	incw	(%rbx,%rdi,2)
	cmpl	%edi, %eax
	jae	LBB5_34
	jmp	LBB5_33
	.p2align	4
LBB5_31:                                ##   in Loop: Header=BB5_30 Depth=1
	movl	%edi, 1632(%rbx)
	movl	%edi, %eax
	movzbl	1(%r15,%rdx), %edi
	incw	(%rbx,%rdi,2)
	cmpl	%edi, %eax
	jae	LBB5_34
LBB5_33:                                ##   in Loop: Header=BB5_30 Depth=1
	movl	%edi, 1632(%rbx)
	movl	%edi, %eax
	jmp	LBB5_34
LBB5_6:
	testb	$1, %r12b
	je	LBB5_9
## %bb.7:
	movzbl	(%r15,%rdx), %edx
	incw	(%rbx,%rdx,2)
	cmpl	%edx, %eax
	jae	LBB5_9
LBB5_8:
	movl	%edx, 1632(%rbx)
	movl	%edx, %eax
LBB5_9:
	testl	%eax, %eax
	je	LBB5_35
## %bb.10:
	movzwl	2(%rbx), %esi
	cmpl	$2, %esi
	ja	LBB5_37
## %bb.11:
	movzwl	4(%rbx), %edx
	leal	(%rdx,%rsi,2), %r8d
	cmpl	$4, %r8d
	ja	LBB5_37
## %bb.12:
	movzwl	6(%rbx), %edi
	leal	(%rdi,%r8,2), %r9d
	cmpl	$8, %r9d
	ja	LBB5_37
## %bb.13:
	movzwl	8(%rbx), %r8d
	leal	(%r8,%r9,2), %r10d
	cmpl	$16, %r10d
	ja	LBB5_37
## %bb.14:
	movzwl	10(%rbx), %r9d
	leal	(%r9,%r10,2), %r11d
	cmpl	$32, %r11d
	ja	LBB5_37
## %bb.15:
	movzwl	12(%rbx), %r10d
	movq	%r10, -56(%rbp)                 ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$64, %r11d
	ja	LBB5_37
## %bb.16:
	movzwl	14(%rbx), %r10d
	movq	%r10, -48(%rbp)                 ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$128, %r11d
	ja	LBB5_37
## %bb.17:
	movzwl	16(%rbx), %r10d
	movq	%r10, -176(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$256, %r11d                     ## imm = 0x100
	ja	LBB5_37
## %bb.18:
	movzwl	18(%rbx), %r10d
	movq	%r10, -168(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$512, %r11d                     ## imm = 0x200
	ja	LBB5_37
## %bb.19:
	movzwl	20(%rbx), %r10d
	movq	%r10, -160(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$1024, %r11d                    ## imm = 0x400
	ja	LBB5_37
## %bb.20:
	movzwl	22(%rbx), %r10d
	movq	%r10, -152(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$2048, %r11d                    ## imm = 0x800
	ja	LBB5_37
## %bb.21:
	movzwl	24(%rbx), %r10d
	movq	%r10, -144(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$4096, %r11d                    ## imm = 0x1000
	ja	LBB5_37
## %bb.22:
	movzwl	26(%rbx), %r10d
	movq	%r10, -136(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$8192, %r11d                    ## imm = 0x2000
	ja	LBB5_37
## %bb.23:
	movzwl	28(%rbx), %r10d
	movq	%r10, -128(%rbp)                ## 8-byte Spill
	leal	(%r10,%r11,2), %r11d
	cmpl	$16384, %r11d                   ## imm = 0x4000
	ja	LBB5_37
## %bb.24:
	movq	%r11, %r10
	movzwl	30(%rbx), %r11d
	leal	(%r11,%r10,2), %r11d
	cmpl	$32768, %r11d                   ## imm = 0x8000
	jbe	LBB5_25
LBB5_37:
	movabsq	$60129542163, %rax              ## imm = 0xE00000013
	jmp	LBB5_2
LBB5_35:
	movb	$1, %al
	testb	%r14b, %r14b
	je	LBB5_61
LBB5_36:
	movabsq	$64424509459, %rax              ## imm = 0xF00000013
LBB5_2:
	movq	%rax, 32(%r13)
	xorl	%eax, %eax
LBB5_61:
                                        ## kill: def $al killed $al killed $eax
	addq	$136, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB5_5:
	xorl	%edx, %edx
	xorl	%eax, %eax
	movzbl	(%r15,%rdx), %edx
	incw	(%rbx,%rdx,2)
	cmpl	%edx, %eax
	jb	LBB5_8
	jmp	LBB5_9
LBB5_25:
	cmpl	$1, %eax
	setne	%r10b
	cmpl	$32768, %r11d                   ## imm = 0x8000
	je	LBB5_27
## %bb.26:
	orb	%r10b, %r14b
	jne	LBB5_36
LBB5_27:
	movq	$0, -120(%rbp)
	movl	%esi, -112(%rbp)
	addl	%esi, %edx
	movl	%edx, -108(%rbp)
	addl	%edi, %edx
	movl	%edx, -104(%rbp)
	addl	%r8d, %edx
	movl	%edx, -100(%rbp)
	addl	%r9d, %edx
	movl	%edx, -96(%rbp)
	addl	-56(%rbp), %edx                 ## 4-byte Folded Reload
	movl	%edx, -92(%rbp)
	addl	-48(%rbp), %edx                 ## 4-byte Folded Reload
	movl	%edx, -88(%rbp)
	addl	-176(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -84(%rbp)
	addl	-168(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -80(%rbp)
	addl	-160(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -76(%rbp)
	addl	-152(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -72(%rbp)
	addl	-144(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -68(%rbp)
	addl	-136(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -64(%rbp)
	addl	-128(%rbp), %edx                ## 4-byte Folded Reload
	movl	%edx, -60(%rbp)
	testq	%rcx, %rcx
	je	LBB5_28
## %bb.38:
	movq	%r12, %rdx
	andq	$-2, %rdx
	xorl	%ecx, %ecx
	jmp	LBB5_39
LBB5_43:                                ##   in Loop: Header=BB5_39 Depth=1
	addq	$2, %rcx
	cmpq	%rcx, %rdx
	je	LBB5_44
LBB5_39:                                ## =>This Inner Loop Header: Depth=1
	movzbl	(%r15,%rcx), %esi
	testq	%rsi, %rsi
	je	LBB5_41
## %bb.40:                              ##   in Loop: Header=BB5_39 Depth=1
	movl	-120(%rbp,%rsi,4), %edi
	movw	%cx, 32(%rbx,%rdi,2)
	incl	%edi
	movl	%edi, -120(%rbp,%rsi,4)
LBB5_41:                                ##   in Loop: Header=BB5_39 Depth=1
	movzbl	1(%r15,%rcx), %esi
	testq	%rsi, %rsi
	je	LBB5_43
## %bb.42:                              ##   in Loop: Header=BB5_39 Depth=1
	movl	-120(%rbp,%rsi,4), %edi
	leal	1(%rcx), %r8d
	movw	%r8w, 32(%rbx,%rdi,2)
	incl	%edi
	movl	%edi, -120(%rbp,%rsi,4)
	jmp	LBB5_43
LBB5_44:
	testb	$1, %r12b
	jne	LBB5_45
	jmp	LBB5_47
LBB5_28:
	xorl	%ecx, %ecx
LBB5_45:
	movzbl	(%r15,%rcx), %edx
	testq	%rdx, %rdx
	je	LBB5_47
## %bb.46:
	movl	-120(%rbp,%rdx,4), %esi
	movw	%cx, 32(%rbx,%rsi,2)
	leal	1(%rsi), %ecx
	movl	%ecx, -120(%rbp,%rdx,4)
LBB5_47:
	cmpl	$9, %eax
	movl	$9, %ecx
	cmovbl	%eax, %ecx
	incl	%ecx
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	xorl	%r11d, %r11d
	movl	$1, %ecx
	xorl	%eax, %eax
	xorl	%esi, %esi
	jmp	LBB5_48
LBB5_58:                                ##   in Loop: Header=BB5_48 Depth=1
	movl	-48(%rbp), %edx                 ## 4-byte Reload
	movl	%edx, %r11d
LBB5_59:                                ##   in Loop: Header=BB5_48 Depth=1
	addl	%esi, %esi
	incq	%rcx
	incq	%rax
	cmpq	-56(%rbp), %rcx                 ## 8-byte Folded Reload
	je	LBB5_60
LBB5_48:                                ## =>This Loop Header: Depth=1
                                        ##     Child Loop BB5_50 Depth 2
                                        ##       Child Loop BB5_52 Depth 3
                                        ##       Child Loop BB5_55 Depth 3
                                        ##       Child Loop BB5_63 Depth 3
	movzwl	(%rbx,%rcx,2), %edx
	addl	%r11d, %edx
	cmpl	%edx, %r11d
	jae	LBB5_59
## %bb.49:                              ##   in Loop: Header=BB5_48 Depth=1
	movl	%ecx, %r8d
	andl	$3, %r8d
	movl	$1, %r9d
	shll	%cl, %r9d
	movq	%rcx, %rdi
	andq	$-4, %rdi
	movl	%r11d, %r11d
	movl	%edx, -48(%rbp)                 ## 4-byte Spill
	movl	%edx, %r14d
	jmp	LBB5_50
LBB5_57:                                ##   in Loop: Header=BB5_50 Depth=2
	incl	%esi
	incq	%r11
	cmpq	%r14, %r11
	je	LBB5_58
LBB5_50:                                ##   Parent Loop BB5_48 Depth=1
                                        ## =>  This Loop Header: Depth=2
                                        ##       Child Loop BB5_52 Depth 3
                                        ##       Child Loop BB5_55 Depth 3
                                        ##       Child Loop BB5_63 Depth 3
	xorl	%edx, %edx
	movl	%esi, %r12d
	cmpq	$3, %rax
	jb	LBB5_54
## %bb.51:                              ##   in Loop: Header=BB5_50 Depth=2
	xorl	%edx, %edx
	movq	%rdi, %r10
	movl	%esi, %r12d
LBB5_52:                                ##   Parent Loop BB5_48 Depth=1
                                        ##     Parent Loop BB5_50 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movl	%r12d, %r15d
	andl	$1, %r15d
	shll	$2, %r15d
	leal	(%r15,%rdx,8), %edx
	movl	%r12d, %r13d
	andl	$2, %r13d
	orl	%edx, %r13d
	movl	%r12d, %r15d
	shrl	$2, %r15d
	andl	$1, %r15d
	orl	%r13d, %r15d
	movl	%r12d, %r13d
	shrl	$3, %r13d
	movl	%r13d, %edx
	andl	$1, %edx
	leal	(%rdx,%r15,2), %edx
	shrl	$4, %r12d
	addq	$-4, %r10
	jne	LBB5_52
## %bb.53:                              ##   in Loop: Header=BB5_50 Depth=2
	testb	$3, %cl
	je	LBB5_56
LBB5_54:                                ##   in Loop: Header=BB5_50 Depth=2
	movq	%r8, %r10
LBB5_55:                                ##   Parent Loop BB5_48 Depth=1
                                        ##     Parent Loop BB5_50 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movl	%r12d, %r13d
	movl	%edx, %r15d
	movl	%r12d, %edx
	andl	$1, %edx
	leal	(%rdx,%r15,2), %edx
	shrl	%r12d
	decq	%r10
	jne	LBB5_55
LBB5_56:                                ##   in Loop: Header=BB5_50 Depth=2
	addl	%r15d, %r15d
	cmpl	$511, %r15d                     ## imm = 0x1FF
	leaq	608(%rbx), %r10
	ja	LBB5_57
## %bb.62:                              ##   in Loop: Header=BB5_50 Depth=2
	movzwl	32(%rbx,%r11,2), %edx
	shll	$4, %edx
	orl	%ecx, %edx
	andl	$1, %r13d
	orl	%r13d, %r15d
LBB5_63:                                ##   Parent Loop BB5_48 Depth=1
                                        ##     Parent Loop BB5_50 Depth=2
                                        ## =>    This Inner Loop Header: Depth=3
	movw	%dx, (%r10,%r15,2)
	addq	%r9, %r15
	cmpq	$512, %r15                      ## imm = 0x200
	jb	LBB5_63
	jmp	LBB5_57
LBB5_60:
	movb	$1, %al
	jmp	LBB5_61
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream11fixed_treesEv ## -- Begin function _ZN9gui_forms11compression13InflateStream11fixed_treesEv
	.p2align	4
__ZN9gui_forms11compression13InflateStream11fixed_treesEv: ## @_ZN9gui_forms11compression13InflateStream11fixed_treesEv
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
	movq	24(%rdi), %r14
	movq	56(%rdi), %rax
	subq	%rax, %r14
	cmpq	$3271, %r14                     ## imm = 0xCC7
	jbe	LBB6_1
## %bb.2:
	addq	$3272, %rax                     ## imm = 0xCC8
	movq	%rax, 56(%rdi)
	leaq	32856(%rdi), %rax
	leaq	__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE12literal_tree(%rip), %rsi
	movl	$1636, %edx                     ## imm = 0x664
	movq	%rdi, %rbx
	movq	%rax, %rdi
	callq	_memcpy
	addq	$34492, %rbx                    ## imm = 0x86BC
	leaq	__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE13distance_tree(%rip), %rsi
	movl	$1636, %edx                     ## imm = 0x664
	movq	%rbx, %rdi
	callq	_memcpy
	jmp	LBB6_3
LBB6_1:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	movq	%rax, 32(%rdi)
LBB6_3:
	cmpq	$3272, %r14                     ## imm = 0xCC8
	setae	%al
	popq	%rbx
	popq	%r14
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE ## -- Begin function _ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.p2align	4
__ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE: ## @_ZN9gui_forms11compression13InflateStream12output_readyERNS1_4CallE
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	48(%rdi), %rax
	cmpq	16(%rdi), %rax
	jne	LBB7_3
## %bb.1:
	movabsq	$17179869203, %rax              ## imm = 0x400000013
	movq	%rax, 32(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
LBB7_3:
	movq	40(%rsi), %rax
	cmpq	24(%rsi), %rax
	jne	LBB7_5
## %bb.4:
	movl	$1, 52(%rsi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
LBB7_5:
	movq	56(%rdi), %rax
	cmpq	%rax, 24(%rdi)
	jne	LBB7_7
## %bb.6:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	movq	%rax, 32(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
LBB7_7:
	incq	%rax
	movq	%rax, 56(%rdi)
	movb	$1, %al
                                        ## kill: def $al killed $al killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte ## -- Begin function _ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	.p2align	4
__ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte: ## @_ZN9gui_forms11compression13InflateStream4emitERNS1_4CallESt4byte
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	16(%rsi), %rax
	movq	40(%rsi), %rcx
	movb	%dl, (%rax,%rcx)
	incq	40(%rsi)
	incq	48(%rdi)
	movq	32832(%rdi), %rax
	movb	%dl, 64(%rdi,%rax)
	movl	32832(%rdi), %eax
	incl	%eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rax, 32832(%rdi)
	movq	32840(%rdi), %rax
	cmpq	$32767, %rax                    ## imm = 0x7FFF
	ja	LBB8_2
## %bb.1:
	incq	%rax
	movq	%rax, 32840(%rdi)
LBB8_2:
	cmpl	$1, (%rdi)
	je	LBB8_5
## %bb.3:
	movzbl	%dl, %ecx
	addl	38172(%rdi), %ecx
	movl	%ecx, %eax
	subl	$65521, %eax                    ## imm = 0xFFF1
	cmovbl	%ecx, %eax
	movl	%eax, 38172(%rdi)
	addl	38176(%rdi), %eax
	movl	%eax, 38176(%rdi)
	cmpl	$65521, %eax                    ## imm = 0xFFF1
	jb	LBB8_5
## %bb.4:
	addl	$-65521, %eax                   ## imm = 0xFFFF000F
	movl	%eax, 38176(%rdi)
LBB8_5:
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
LCPI9_0:
	.quad	2                               ## 0x2
	.quad	3                               ## 0x3
LCPI9_1:
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	1                               ## 0x1
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
LCPI9_2:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI9_3:
	.quad	8                               ## 0x8
	.quad	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
	.p2align	4
__ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm: ## @_ZN9gui_forms11compression13InflateStream7publishERNS1_4CallEm
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
	movq	%rdx, %r14
	movq	%rsi, %r12
	movq	%rdi, %rbx
	movq	16(%rsi), %rcx
	movq	40(%rsi), %rdx
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	leaq	(%rcx,%rdx), %r15
	movq	32832(%rdi), %rax
	movl	$32768, %r13d                   ## imm = 0x8000
	subq	%rax, %r13
	cmpq	%r14, %r13
	cmovaeq	%r14, %r13
	addq	%rax, %rdi
	addq	$64, %rdi
	movq	%r15, %rsi
	movq	%r13, %rdx
	callq	_memcpy
	leaq	(%r15,%r13), %rsi
	movq	%r14, %rdx
	subq	%r13, %rdx
	leaq	64(%rbx), %rdi
	callq	_memcpy
	movl	32832(%rbx), %eax
	addl	%r14d, %eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rax, 32832(%rbx)
	movq	32840(%rbx), %rax
	addq	%r14, %rax
	cmpq	$32768, %rax                    ## imm = 0x8000
	movl	$32768, %ecx                    ## imm = 0x8000
	cmovaeq	%rcx, %rax
	movq	%rax, 32840(%rbx)
	addq	%r14, 40(%r12)
	addq	%r14, 48(%rbx)
	cmpl	$1, (%rbx)
	je	LBB9_11
## %bb.1:
	testl	%r14d, %r14d
	je	LBB9_2
## %bb.3:
	movl	%r14d, %eax
	cmpq	$8, %rax
	jae	LBB9_5
## %bb.4:
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	xorl	%esi, %esi
	jmp	LBB9_8
LBB9_2:
	xorl	%esi, %esi
	xorl	%edx, %edx
	jmp	LBB9_10
LBB9_5:
	movl	%r14d, %ecx
	andl	$-8, %ecx
	movq	%r14, %xmm0
	pshufd	$68, %xmm0, %xmm0               ## xmm0 = xmm0[0,1,0,1]
	movq	-56(%rbp), %rdx                 ## 8-byte Reload
	movq	-48(%rbp), %rsi                 ## 8-byte Reload
	leaq	4(%rsi,%rdx), %rdx
	movdqa	LCPI9_0(%rip), %xmm2            ## xmm2 = [2,3]
	movdqa	LCPI9_1(%rip), %xmm4            ## xmm4 = [0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0]
	pxor	%xmm5, %xmm5
	xorl	%esi, %esi
	movdqa	LCPI9_2(%rip), %xmm6            ## xmm6 = [4,4,4,4]
	movdqa	LCPI9_3(%rip), %xmm8            ## xmm8 = [8,8]
	pxor	%xmm9, %xmm9
	pxor	%xmm7, %xmm7
	pxor	%xmm3, %xmm3
	pxor	%xmm1, %xmm1
	.p2align	4
LBB9_6:                                 ## =>This Inner Loop Header: Depth=1
	movd	-4(%rdx,%rsi), %xmm11           ## xmm11 = mem[0],zero,zero,zero
	movd	(%rdx,%rsi), %xmm10             ## xmm10 = mem[0],zero,zero,zero
	punpcklbw	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3],xmm11[4],xmm5[4],xmm11[5],xmm5[5],xmm11[6],xmm5[6],xmm11[7],xmm5[7]
	punpcklwd	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3]
	punpcklbw	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3],xmm10[4],xmm5[4],xmm10[5],xmm5[5],xmm10[6],xmm5[6],xmm10[7],xmm5[7]
	punpcklwd	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3]
	paddd	%xmm11, %xmm9
	paddd	%xmm10, %xmm7
	movdqa	%xmm0, %xmm12
	psubq	%xmm4, %xmm12
	movdqa	%xmm0, %xmm13
	psubq	%xmm2, %xmm13
	movdqa	%xmm12, %xmm14
	shufps	$136, %xmm13, %xmm14            ## xmm14 = xmm14[0,2],xmm13[0,2]
	movaps	%xmm14, %xmm15
	psubd	%xmm6, %xmm15
	pmuludq	%xmm11, %xmm14
	pshufd	$232, %xmm14, %xmm14            ## xmm14 = xmm14[0,2,2,3]
	punpckhqdq	%xmm13, %xmm12          ## xmm12 = xmm12[1],xmm13[1]
	pshufd	$245, %xmm11, %xmm11            ## xmm11 = xmm11[1,1,3,3]
	pmuludq	%xmm12, %xmm11
	pshufd	$232, %xmm11, %xmm11            ## xmm11 = xmm11[0,2,2,3]
	punpckldq	%xmm11, %xmm14          ## xmm14 = xmm14[0],xmm11[0],xmm14[1],xmm11[1]
	paddd	%xmm14, %xmm3
	pshufd	$245, %xmm15, %xmm11            ## xmm11 = xmm15[1,1,3,3]
	pmuludq	%xmm10, %xmm15
	pshufd	$232, %xmm15, %xmm12            ## xmm12 = xmm15[0,2,2,3]
	pshufd	$245, %xmm10, %xmm10            ## xmm10 = xmm10[1,1,3,3]
	pmuludq	%xmm11, %xmm10
	pshufd	$232, %xmm10, %xmm10            ## xmm10 = xmm10[0,2,2,3]
	punpckldq	%xmm10, %xmm12          ## xmm12 = xmm12[0],xmm10[0],xmm12[1],xmm10[1]
	paddd	%xmm12, %xmm1
	addq	$8, %rsi
	paddq	%xmm8, %xmm4
	paddq	%xmm8, %xmm2
	cmpq	%rsi, %rcx
	jne	LBB9_6
## %bb.7:
	paddd	%xmm9, %xmm7
	pshufd	$238, %xmm7, %xmm0              ## xmm0 = xmm7[2,3,2,3]
	paddd	%xmm7, %xmm0
	pshufd	$85, %xmm0, %xmm2               ## xmm2 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm2
	movd	%xmm2, %edx
	paddd	%xmm3, %xmm1
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	paddd	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm1
	movd	%xmm1, %esi
	cmpl	%ecx, %eax
	je	LBB9_10
LBB9_8:
	movl	%r14d, %edi
	subl	%ecx, %edi
	.p2align	4
LBB9_9:                                 ## =>This Inner Loop Header: Depth=1
	movzbl	(%r15,%rcx), %r8d
	addl	%r8d, %edx
	imull	%edi, %r8d
	addl	%r8d, %esi
	incq	%rcx
	decl	%edi
	cmpq	%rcx, %rax
	jne	LBB9_9
LBB9_10:
	movl	38172(%rbx), %eax
	addl	%eax, %edx
	imull	%r14d, %eax
	addl	38176(%rbx), %esi
	addl	%eax, %esi
	movl	$2147975281, %eax               ## imm = 0x80078071
	movq	%rsi, %rcx
	imulq	%rax, %rcx
	shrq	$47, %rcx
	imull	$65521, %ecx, %ecx              ## imm = 0xFFF1
	subl	%ecx, %esi
	movl	%esi, 38176(%rbx)
	imulq	%rdx, %rax
	shrq	$47, %rax
	imull	$65521, %eax, %eax              ## imm = 0xFFF1
	subl	%eax, %edx
	movl	%edx, 38172(%rbx)
LBB9_11:
	addq	$24, %rsp
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
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
LCPI10_0:
	.quad	2                               ## 0x2
	.quad	3                               ## 0x3
LCPI10_1:
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	1                               ## 0x1
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
LCPI10_2:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI10_3:
	.quad	8                               ## 0x8
	.quad	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	.p2align	4
__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE: ## @_ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	.cfi_startproc
## %bb.0:
	cmpl	$1, 38104(%rdi)
	jne	LBB10_1
## %bb.3:
	movq	8(%rsi), %rcx
	movq	24(%rsi), %rax
	subq	40(%rsi), %rax
	movq	32(%rsi), %r8
	subq	%r8, %rcx
	shrq	%rcx
	cmpq	%rcx, %rax
	cmovbq	%rax, %rcx
	movq	8(%rdi), %r10
	subq	40(%rdi), %r10
	movq	16(%rdi), %r9
	shrq	%r10
	subq	48(%rdi), %r9
	movq	24(%rdi), %rax
	subq	56(%rdi), %rax
	movabsq	$-3689348814741910323, %rdx     ## imm = 0xCCCCCCCCCCCCCCCD
	mulq	%rdx
	shrq	$2, %rdx
	cmpq	%r10, %rcx
	cmovbq	%rcx, %r10
	cmpq	%r9, %r10
	cmovaeq	%r9, %r10
	cmpq	%rdx, %r10
	cmovaeq	%rdx, %r10
	cmpq	$4096, %r10                     ## imm = 0x1000
	movl	$4096, %r9d                     ## imm = 0x1000
	cmovbq	%r10, %r9
	testq	%r10, %r10
	je	LBB10_1
## %bb.4:
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
	movl	38120(%rdi), %edx
	movl	38124(%rdi), %eax
	xorl	%r12d, %r12d
	.p2align	4
LBB10_5:                                ## =>This Inner Loop Header: Depth=1
	movq	(%rsi), %rcx
	movzwl	(%rcx,%r8), %r11d
	movl	%eax, %ecx
	shll	%cl, %r11d
	orl	%edx, %r11d
	movl	%r11d, %ecx
	andl	$511, %ecx                      ## imm = 0x1FF
	movzwl	33464(%rdi,%rcx,2), %r10d
	leal	-4096(%r10), %ecx
	movzwl	%cx, %ecx
	cmpl	$61441, %ecx                    ## imm = 0xF001
	jb	LBB10_11
## %bb.6:                               ##   in Loop: Header=BB10_5 Depth=1
	movl	%r10d, %edx
	andl	$15, %edx
	movl	%edx, %ecx
	subl	%eax, %ecx
	addl	$7, %ecx
	movl	%ecx, %ebx
	andl	$-8, %ecx
	subl	%edx, %eax
	addl	%ecx, %eax
	movl	$-1, %r14d
	movl	%eax, %ecx
	shll	%cl, %r14d
	movl	%edx, %ecx
	shrl	%cl, %r11d
	shrl	$3, %ebx
	notl	%r14d
	movl	%r11d, %edx
	andl	%r14d, %edx
	addq	%rbx, %r8
	movq	16(%rsi), %rcx
	addq	40(%rsi), %rcx
	shrl	$4, %r10d
	movb	%r10b, (%r12,%rcx)
	incq	%r12
	cmpq	%r12, %r9
	jne	LBB10_5
## %bb.7:
	movq	%r9, %r12
	jmp	LBB10_8
LBB10_1:
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	retq
LBB10_11:
	testq	%r12, %r12
	je	LBB10_12
LBB10_8:
	movq	%r8, %rcx
	subq	32(%rsi), %rcx
	leaq	(%r12,%r12,2), %r9
	addq	%rcx, %r9
	addq	%r9, 56(%rdi)
	addq	%rcx, 40(%rdi)
	movq	%r8, 32(%rsi)
	movl	%edx, 38120(%rdi)
	movl	%eax, 38124(%rdi)
	movq	16(%rsi), %rcx
	movq	40(%rsi), %rdx
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	movq	%rdx, -48(%rbp)                 ## 8-byte Spill
	leaq	(%rcx,%rdx), %rbx
	leaq	64(%rdi), %rax
	movq	%rax, -64(%rbp)                 ## 8-byte Spill
	movq	32832(%rdi), %rax
	movl	$32768, %r15d                   ## imm = 0x8000
	subq	%rax, %r15
	cmpq	%r12, %r15
	cmovaeq	%r12, %r15
	leaq	64(%rdi,%rax), %rax
	movq	%rdi, %r13
	movq	%rax, %rdi
	movq	%rsi, %r14
	movq	%rbx, %rsi
	movq	%r15, %rdx
	callq	_memcpy
	leaq	(%rbx,%r15), %rsi
	movq	%r12, %rdx
	subq	%r15, %rdx
	movq	-64(%rbp), %rdi                 ## 8-byte Reload
	callq	_memcpy
	movl	32832(%r13), %eax
	addl	%r12d, %eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rax, 32832(%r13)
	movq	32840(%r13), %rax
	addq	%r12, %rax
	cmpq	$32768, %rax                    ## imm = 0x8000
	movl	$32768, %edx                    ## imm = 0x8000
	cmovaeq	%rdx, %rax
	movq	%rax, 32840(%r13)
	addq	%r12, 40(%r14)
	addq	%r12, 48(%r13)
	movb	$1, %al
	cmpl	$1, (%r13)
	je	LBB10_21
## %bb.9:
	testl	%r12d, %r12d
	je	LBB10_10
## %bb.13:
	movl	%r12d, %edx
	cmpq	$8, %rdx
	jae	LBB10_15
## %bb.14:
	xorl	%esi, %esi
	xorl	%edi, %edi
	xorl	%r8d, %r8d
	jmp	LBB10_18
LBB10_12:
	xorl	%eax, %eax
	jmp	LBB10_21
LBB10_10:
	xorl	%r8d, %r8d
	xorl	%edi, %edi
	jmp	LBB10_20
LBB10_15:
	movl	%r12d, %esi
	andl	$-8, %esi
	movq	%r12, %xmm0
	pshufd	$68, %xmm0, %xmm0               ## xmm0 = xmm0[0,1,0,1]
	movq	-56(%rbp), %rdi                 ## 8-byte Reload
	movq	-48(%rbp), %r8                  ## 8-byte Reload
	leaq	4(%r8,%rdi), %rdi
	movdqa	LCPI10_0(%rip), %xmm2           ## xmm2 = [2,3]
	movdqa	LCPI10_1(%rip), %xmm4           ## xmm4 = [0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0]
	pxor	%xmm5, %xmm5
	xorl	%r8d, %r8d
	movdqa	LCPI10_2(%rip), %xmm6           ## xmm6 = [4,4,4,4]
	movdqa	LCPI10_3(%rip), %xmm8           ## xmm8 = [8,8]
	pxor	%xmm9, %xmm9
	pxor	%xmm7, %xmm7
	pxor	%xmm3, %xmm3
	pxor	%xmm1, %xmm1
	.p2align	4
LBB10_16:                               ## =>This Inner Loop Header: Depth=1
	movd	-4(%rdi,%r8), %xmm11            ## xmm11 = mem[0],zero,zero,zero
	movd	(%rdi,%r8), %xmm10              ## xmm10 = mem[0],zero,zero,zero
	punpcklbw	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3],xmm11[4],xmm5[4],xmm11[5],xmm5[5],xmm11[6],xmm5[6],xmm11[7],xmm5[7]
	punpcklwd	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3]
	punpcklbw	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3],xmm10[4],xmm5[4],xmm10[5],xmm5[5],xmm10[6],xmm5[6],xmm10[7],xmm5[7]
	punpcklwd	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3]
	paddd	%xmm11, %xmm9
	paddd	%xmm10, %xmm7
	movdqa	%xmm0, %xmm12
	psubq	%xmm4, %xmm12
	movdqa	%xmm0, %xmm13
	psubq	%xmm2, %xmm13
	movdqa	%xmm12, %xmm14
	shufps	$136, %xmm13, %xmm14            ## xmm14 = xmm14[0,2],xmm13[0,2]
	pshufd	$245, %xmm11, %xmm15            ## xmm15 = xmm11[1,1,3,3]
	pmuludq	%xmm14, %xmm11
	psubd	%xmm6, %xmm14
	pshufd	$232, %xmm11, %xmm11            ## xmm11 = xmm11[0,2,2,3]
	punpckhqdq	%xmm13, %xmm12          ## xmm12 = xmm12[1],xmm13[1]
	pmuludq	%xmm15, %xmm12
	pshufd	$232, %xmm12, %xmm12            ## xmm12 = xmm12[0,2,2,3]
	punpckldq	%xmm12, %xmm11          ## xmm11 = xmm11[0],xmm12[0],xmm11[1],xmm12[1]
	paddd	%xmm11, %xmm3
	pshufd	$245, %xmm10, %xmm11            ## xmm11 = xmm10[1,1,3,3]
	pmuludq	%xmm14, %xmm10
	pshufd	$232, %xmm10, %xmm10            ## xmm10 = xmm10[0,2,2,3]
	pshufd	$245, %xmm14, %xmm12            ## xmm12 = xmm14[1,1,3,3]
	pmuludq	%xmm11, %xmm12
	pshufd	$232, %xmm12, %xmm11            ## xmm11 = xmm12[0,2,2,3]
	punpckldq	%xmm11, %xmm10          ## xmm10 = xmm10[0],xmm11[0],xmm10[1],xmm11[1]
	paddd	%xmm10, %xmm1
	addq	$8, %r8
	paddq	%xmm8, %xmm4
	paddq	%xmm8, %xmm2
	cmpq	%r8, %rsi
	jne	LBB10_16
## %bb.17:
	paddd	%xmm9, %xmm7
	pshufd	$238, %xmm7, %xmm0              ## xmm0 = xmm7[2,3,2,3]
	paddd	%xmm7, %xmm0
	pshufd	$85, %xmm0, %xmm2               ## xmm2 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm2
	movd	%xmm2, %edi
	paddd	%xmm3, %xmm1
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	paddd	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm1
	movd	%xmm1, %r8d
	cmpl	%esi, %edx
	je	LBB10_20
LBB10_18:
	movl	%r12d, %r9d
	subl	%esi, %r9d
	.p2align	4
LBB10_19:                               ## =>This Inner Loop Header: Depth=1
	movzbl	(%rbx,%rsi), %r10d
	addl	%r10d, %edi
	imull	%r9d, %r10d
	addl	%r10d, %r8d
	incq	%rsi
	decl	%r9d
	cmpq	%rsi, %rdx
	jne	LBB10_19
LBB10_20:
	movl	38172(%r13), %edx
	addl	%edx, %edi
	imull	%r12d, %edx
	addl	38176(%r13), %r8d
	addl	%edx, %r8d
	movl	$2147975281, %edx               ## imm = 0x80078071
	movq	%r8, %rsi
	imulq	%rdx, %rsi
	shrq	$47, %rsi
	imull	$65521, %esi, %esi              ## imm = 0xFFF1
	subl	%esi, %r8d
	movl	%r8d, 38176(%r13)
	imulq	%rdi, %rdx
	shrq	$47, %rdx
	imull	$65521, %edx, %edx              ## imm = 0xFFF1
	subl	%edx, %edi
	movl	%edi, 38172(%r13)
LBB10_21:
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
                                        ## kill: def $al killed $al killed $eax
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
LCPI11_0:
	.quad	2                               ## 0x2
	.quad	3                               ## 0x3
LCPI11_1:
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	1                               ## 0x1
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
LCPI11_2:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI11_3:
	.quad	8                               ## 0x8
	.quad	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
	.p2align	4
__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE: ## @_ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE
	.cfi_startproc
## %bb.0:
	movq	16(%rdi), %rdx
	subq	48(%rdi), %rdx
	jne	LBB11_2
## %bb.1:
	movabsq	$17179869203, %rax              ## imm = 0x400000013
	movq	%rax, 32(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	retq
LBB11_2:
	movq	24(%rsi), %r10
	movq	40(%rsi), %rax
	subq	%rax, %r10
	jne	LBB11_4
## %bb.3:
	movl	$1, 52(%rsi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	retq
LBB11_4:
	movq	24(%rdi), %r9
	movq	56(%rdi), %r8
	cmpq	%r8, %r9
	jne	LBB11_6
## %bb.5:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	movq	%rax, 32(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	retq
LBB11_6:
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
	leaq	1(%r8), %r11
	movq	%r11, 56(%rdi)
	movq	8(%rsi), %r14
	movq	32(%rsi), %rcx
	subq	%rcx, %r14
	jne	LBB11_10
## %bb.7:
	cmpb	$1, 48(%rsi)
	jne	LBB11_9
## %bb.8:
	movabsq	$25769803795, %rax              ## imm = 0x600000013
	movq	%rax, 32(%rdi)
LBB11_9:
	movl	$0, 52(%rsi)
	jmp	LBB11_15
LBB11_10:
	movq	8(%rdi), %r15
	subq	40(%rdi), %r15
	jne	LBB11_12
## %bb.11:
	movabsq	$12884901907, %rax              ## imm = 0x300000013
	jmp	LBB11_14
LBB11_12:
	movl	38164(%rdi), %r12d
	cmpl	$4096, %r12d                    ## imm = 0x1000
	movl	$4096, %ebx                     ## imm = 0x1000
	cmovbl	%r12d, %ebx
	cmpq	%rbx, %r10
	cmovbq	%r10, %rbx
	cmpq	%rbx, %r14
	cmovbq	%r14, %rbx
	cmpq	%rbx, %r15
	cmovbq	%r15, %rbx
	cmpq	%rbx, %rdx
	cmovbq	%rdx, %rbx
	leaq	-1(,%rbx,2), %rdx
	subq	%r11, %r9
	cmpq	%r9, %rdx
	jbe	LBB11_17
## %bb.13:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
LBB11_14:
	movq	%rax, 32(%rdi)
LBB11_15:
	xorl	%eax, %eax
LBB11_16:
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
                                        ## kill: def $al killed $al killed $eax
	retq
LBB11_17:
	movl	%r12d, -44(%rbp)                ## 4-byte Spill
	addq	%rbx, %r8
	addq	%rbx, %r8
	movq	%r8, 56(%rdi)
	addq	16(%rsi), %rax
	addq	(%rsi), %rcx
	movq	%rdi, %r15
	movq	%rax, %rdi
	movq	%rsi, %r13
	movq	%rcx, %rsi
	movq	%rbx, %rdx
	callq	_memcpy
	addq	%rbx, 32(%r13)
	addq	%rbx, 40(%r15)
	subl	%ebx, 38164(%r15)
	movq	16(%r13), %rax
	movq	40(%r13), %rcx
	movq	%rax, -64(%rbp)                 ## 8-byte Spill
	movq	%rcx, -56(%rbp)                 ## 8-byte Spill
	leaq	(%rax,%rcx), %r14
	movq	32832(%r15), %rax
	movl	$32768, %r12d                   ## imm = 0x8000
	subq	%rax, %r12
	cmpq	%rbx, %r12
	cmovaeq	%rbx, %r12
	leaq	(%r15,%rax), %rdi
	addq	$64, %rdi
	movq	%r14, %rsi
	movq	%r12, %rdx
	callq	_memcpy
	addq	%r12, %r14
	movq	%rbx, %rdx
	subq	%r12, %rdx
	leaq	64(%r15), %rdi
	movq	%r14, %rsi
	callq	_memcpy
	movl	32832(%r15), %eax
	addl	%ebx, %eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rax, 32832(%r15)
	movq	32840(%r15), %rax
	addq	%rbx, %rax
	cmpq	$32768, %rax                    ## imm = 0x8000
	movl	$32768, %edx                    ## imm = 0x8000
	cmovaeq	%rdx, %rax
	movq	%rax, 32840(%r15)
	addq	%rbx, 40(%r13)
	addq	%rbx, 48(%r15)
	movb	$1, %al
	cmpl	$1, (%r15)
	je	LBB11_16
## %bb.18:
	cmpl	$0, -44(%rbp)                   ## 4-byte Folded Reload
	je	LBB11_21
## %bb.19:
	cmpq	$8, %rbx
	jae	LBB11_22
## %bb.20:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	-64(%rbp), %r11                 ## 8-byte Reload
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	jmp	LBB11_25
LBB11_21:
	xorl	%edi, %edi
	xorl	%esi, %esi
	jmp	LBB11_27
LBB11_22:
	movl	%ebx, %edx
	andl	$2147483640, %edx               ## imm = 0x7FFFFFF8
	movq	%rbx, %xmm0
	pshufd	$68, %xmm0, %xmm0               ## xmm0 = xmm0[0,1,0,1]
	movq	-64(%rbp), %r11                 ## 8-byte Reload
	movq	-56(%rbp), %r10                 ## 8-byte Reload
	leaq	4(%r10,%r11), %rsi
	movdqa	LCPI11_0(%rip), %xmm2           ## xmm2 = [2,3]
	movdqa	LCPI11_1(%rip), %xmm4           ## xmm4 = [0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0]
	pxor	%xmm5, %xmm5
	xorl	%edi, %edi
	movdqa	LCPI11_2(%rip), %xmm6           ## xmm6 = [4,4,4,4]
	movdqa	LCPI11_3(%rip), %xmm8           ## xmm8 = [8,8]
	pxor	%xmm9, %xmm9
	pxor	%xmm7, %xmm7
	pxor	%xmm3, %xmm3
	pxor	%xmm1, %xmm1
LBB11_23:                               ## =>This Inner Loop Header: Depth=1
	movd	-4(%rsi,%rdi), %xmm11           ## xmm11 = mem[0],zero,zero,zero
	movd	(%rsi,%rdi), %xmm10             ## xmm10 = mem[0],zero,zero,zero
	punpcklbw	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3],xmm11[4],xmm5[4],xmm11[5],xmm5[5],xmm11[6],xmm5[6],xmm11[7],xmm5[7]
	punpcklwd	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3]
	punpcklbw	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3],xmm10[4],xmm5[4],xmm10[5],xmm5[5],xmm10[6],xmm5[6],xmm10[7],xmm5[7]
	punpcklwd	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3]
	paddd	%xmm11, %xmm9
	paddd	%xmm10, %xmm7
	movdqa	%xmm0, %xmm12
	psubq	%xmm4, %xmm12
	movdqa	%xmm0, %xmm13
	psubq	%xmm2, %xmm13
	movdqa	%xmm12, %xmm14
	shufps	$136, %xmm13, %xmm14            ## xmm14 = xmm14[0,2],xmm13[0,2]
	pshufd	$245, %xmm11, %xmm15            ## xmm15 = xmm11[1,1,3,3]
	pmuludq	%xmm14, %xmm11
	psubd	%xmm6, %xmm14
	pshufd	$232, %xmm11, %xmm11            ## xmm11 = xmm11[0,2,2,3]
	punpckhqdq	%xmm13, %xmm12          ## xmm12 = xmm12[1],xmm13[1]
	pmuludq	%xmm15, %xmm12
	pshufd	$232, %xmm12, %xmm12            ## xmm12 = xmm12[0,2,2,3]
	punpckldq	%xmm12, %xmm11          ## xmm11 = xmm11[0],xmm12[0],xmm11[1],xmm12[1]
	paddd	%xmm11, %xmm3
	pshufd	$245, %xmm10, %xmm11            ## xmm11 = xmm10[1,1,3,3]
	pmuludq	%xmm14, %xmm10
	pshufd	$232, %xmm10, %xmm10            ## xmm10 = xmm10[0,2,2,3]
	pshufd	$245, %xmm14, %xmm12            ## xmm12 = xmm14[1,1,3,3]
	pmuludq	%xmm11, %xmm12
	pshufd	$232, %xmm12, %xmm11            ## xmm11 = xmm12[0,2,2,3]
	punpckldq	%xmm11, %xmm10          ## xmm10 = xmm10[0],xmm11[0],xmm10[1],xmm11[1]
	paddd	%xmm10, %xmm1
	addq	$8, %rdi
	paddq	%xmm8, %xmm4
	paddq	%xmm8, %xmm2
	cmpq	%rdi, %rdx
	jne	LBB11_23
## %bb.24:
	paddd	%xmm9, %xmm7
	pshufd	$238, %xmm7, %xmm0              ## xmm0 = xmm7[2,3,2,3]
	paddd	%xmm7, %xmm0
	pshufd	$85, %xmm0, %xmm2               ## xmm2 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm2
	movd	%xmm2, %esi
	paddd	%xmm3, %xmm1
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	paddd	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm1
	movd	%xmm1, %edi
	cmpq	%rdx, %rbx
	je	LBB11_27
LBB11_25:
	movq	%rbx, %r8
	subq	%rdx, %r8
	movl	%ebx, %r9d
	subl	%edx, %r9d
	addq	%rdx, %r10
	addq	%r10, %r11
	xorl	%edx, %edx
LBB11_26:                               ## =>This Inner Loop Header: Depth=1
	movzbl	(%r11,%rdx), %r10d
	addl	%r10d, %esi
	imull	%r9d, %r10d
	addl	%r10d, %edi
	incq	%rdx
	decl	%r9d
	cmpq	%rdx, %r8
	jne	LBB11_26
LBB11_27:
	movl	38172(%r15), %edx
	addl	%edx, %esi
	imull	%ebx, %edx
	addl	38176(%r15), %edi
	addl	%edx, %edi
	movl	$2147975281, %edx               ## imm = 0x80078071
	movq	%rdi, %r8
	imulq	%rdx, %r8
	shrq	$47, %r8
	imull	$65521, %r8d, %r8d              ## imm = 0xFFF1
	subl	%r8d, %edi
	movl	%edi, 38176(%r15)
	imulq	%rsi, %rdx
	shrq	$47, %rdx
	imull	$65521, %edx, %edx              ## imm = 0xFFF1
	subl	%edx, %esi
	movl	%esi, 38172(%r15)
	jmp	LBB11_16
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ## -- Begin function _ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
LCPI12_0:
	.quad	2                               ## 0x2
	.quad	3                               ## 0x3
LCPI12_1:
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	1                               ## 0x1
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
	.byte	0                               ## 0x0
LCPI12_2:
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	4                               ## 0x4
LCPI12_3:
	.quad	8                               ## 0x8
	.quad	8                               ## 0x8
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
	.p2align	4
__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE: ## @_ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE
	.cfi_startproc
## %bb.0:
	movq	16(%rdi), %rax
	subq	48(%rdi), %rax
	jne	LBB12_2
## %bb.1:
	movabsq	$17179869203, %rax              ## imm = 0x400000013
	movq	%rax, 32(%rdi)
	xorl	%eax, %eax
                                        ## kill: def $al killed $al killed $eax
	retq
LBB12_2:
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
	subq	$56, %rsp
	.cfi_offset %rbx, -56
	.cfi_offset %r12, -48
	.cfi_offset %r13, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	24(%rsi), %rbx
	movq	40(%rsi), %r14
	subq	%r14, %rbx
	jne	LBB12_4
## %bb.3:
	movl	$1, 52(%rsi)
	jmp	LBB12_7
LBB12_4:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rcx
	cmpq	%rcx, %rdx
	je	LBB12_6
## %bb.5:
	leaq	1(%rcx), %r8
	movq	%r8, 56(%rdi)
	movl	38148(%rdi), %r9d
	cmpq	%r9, %rbx
	cmovaeq	%r9, %rbx
	cmpq	%rbx, %rax
	cmovbq	%rax, %rbx
	leaq	-1(%rbx), %rax
	subq	%r8, %rdx
	cmpq	%rdx, %rax
	jbe	LBB12_9
LBB12_6:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	movq	%rax, 32(%rdi)
LBB12_7:
	xorl	%eax, %eax
LBB12_8:
	addq	$56, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
                                        ## kill: def $al killed $al killed $eax
	retq
LBB12_9:
	movq	%r9, -80(%rbp)                  ## 8-byte Spill
	addq	%rbx, %rcx
	movq	%rcx, 56(%rdi)
	movl	32832(%rdi), %eax
	leaq	64(%rdi), %rcx
	movq	%rdi, -88(%rbp)                 ## 8-byte Spill
	movl	38152(%rdi), %r15d
	subl	%r15d, %eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rsi, -72(%rbp)                 ## 8-byte Spill
	addq	16(%rsi), %r14
	cmpq	$1, %r15
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	jne	LBB12_11
## %bb.10:
	movzbl	(%rcx,%rax), %esi
	movq	%r14, %rdi
	movq	%rbx, %rdx
	callq	_memset
	jmp	LBB12_13
LBB12_11:
	cmpq	%r15, %rbx
	movq	%r15, %r12
	cmovbq	%rbx, %r12
	movl	$32768, %r13d                   ## imm = 0x8000
	subq	%rax, %r13
	cmpq	%r12, %r13
	cmovaeq	%r12, %r13
	addq	%rcx, %rax
	movq	%r14, %rdi
	movq	%rax, %rsi
	movq	%r13, %rdx
	callq	_memcpy
	leaq	(%r14,%r13), %rdi
	movq	%r12, %rdx
	subq	%r13, %rdx
	movq	-64(%rbp), %rsi                 ## 8-byte Reload
	callq	_memcpy
	cmpq	%r15, %rbx
	jbe	LBB12_13
	.p2align	4
LBB12_12:                               ## =>This Inner Loop Header: Depth=1
	movq	%r12, %r15
	movq	%rbx, %r12
	subq	%r15, %r12
	cmpq	%r15, %r12
	cmovaeq	%r15, %r12
	leaq	(%r14,%r15), %rdi
	movq	%r14, %rsi
	movq	%r12, %rdx
	callq	_memcpy
	addq	%r15, %r12
	cmpq	%rbx, %r12
	jb	LBB12_12
LBB12_13:
	movq	-88(%rbp), %r13                 ## 8-byte Reload
	subl	%ebx, 38148(%r13)
	movq	-72(%rbp), %rcx                 ## 8-byte Reload
	movq	16(%rcx), %rax
	movq	40(%rcx), %rcx
	movq	%rax, -56(%rbp)                 ## 8-byte Spill
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	leaq	(%rax,%rcx), %r14
	movq	32832(%r13), %rdi
	movl	$32768, %r12d                   ## imm = 0x8000
	subq	%rdi, %r12
	cmpq	%rbx, %r12
	cmovaeq	%rbx, %r12
	movq	-64(%rbp), %r15                 ## 8-byte Reload
	addq	%r15, %rdi
	movq	%r14, %rsi
	movq	%r12, %rdx
	callq	_memcpy
	addq	%r12, %r14
	movq	%rbx, %rdx
	subq	%r12, %rdx
	movq	%r15, %rdi
	movq	%r14, %rsi
	callq	_memcpy
	movl	32832(%r13), %eax
	addl	%ebx, %eax
	andl	$32767, %eax                    ## imm = 0x7FFF
	movq	%rax, 32832(%r13)
	movq	32840(%r13), %rax
	addq	%rbx, %rax
	cmpq	$32768, %rax                    ## imm = 0x8000
	movl	$32768, %edx                    ## imm = 0x8000
	cmovaeq	%rdx, %rax
	movq	%rax, 32840(%r13)
	movq	-72(%rbp), %rax                 ## 8-byte Reload
	addq	%rbx, 40(%rax)
	addq	%rbx, 48(%r13)
	movb	$1, %al
	cmpl	$1, (%r13)
	je	LBB12_8
## %bb.14:
	cmpl	$0, -80(%rbp)                   ## 4-byte Folded Reload
	je	LBB12_17
## %bb.15:
	cmpq	$8, %rbx
	jae	LBB12_18
## %bb.16:
	xorl	%edx, %edx
	xorl	%esi, %esi
	xorl	%edi, %edi
	movq	-56(%rbp), %r11                 ## 8-byte Reload
	movq	-48(%rbp), %r10                 ## 8-byte Reload
	jmp	LBB12_21
LBB12_17:
	xorl	%edi, %edi
	xorl	%esi, %esi
	jmp	LBB12_23
LBB12_18:
	movl	%ebx, %edx
	andl	$-8, %edx
	movq	%rbx, %xmm0
	pshufd	$68, %xmm0, %xmm0               ## xmm0 = xmm0[0,1,0,1]
	movq	-56(%rbp), %r11                 ## 8-byte Reload
	movq	-48(%rbp), %r10                 ## 8-byte Reload
	leaq	4(%r10,%r11), %rsi
	movdqa	LCPI12_0(%rip), %xmm2           ## xmm2 = [2,3]
	movdqa	LCPI12_1(%rip), %xmm4           ## xmm4 = [0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0]
	pxor	%xmm5, %xmm5
	xorl	%edi, %edi
	movdqa	LCPI12_2(%rip), %xmm6           ## xmm6 = [4,4,4,4]
	movdqa	LCPI12_3(%rip), %xmm8           ## xmm8 = [8,8]
	pxor	%xmm9, %xmm9
	pxor	%xmm7, %xmm7
	pxor	%xmm3, %xmm3
	pxor	%xmm1, %xmm1
	.p2align	4
LBB12_19:                               ## =>This Inner Loop Header: Depth=1
	movd	-4(%rsi,%rdi), %xmm11           ## xmm11 = mem[0],zero,zero,zero
	movd	(%rsi,%rdi), %xmm10             ## xmm10 = mem[0],zero,zero,zero
	punpcklbw	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3],xmm11[4],xmm5[4],xmm11[5],xmm5[5],xmm11[6],xmm5[6],xmm11[7],xmm5[7]
	punpcklwd	%xmm5, %xmm11           ## xmm11 = xmm11[0],xmm5[0],xmm11[1],xmm5[1],xmm11[2],xmm5[2],xmm11[3],xmm5[3]
	punpcklbw	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3],xmm10[4],xmm5[4],xmm10[5],xmm5[5],xmm10[6],xmm5[6],xmm10[7],xmm5[7]
	punpcklwd	%xmm5, %xmm10           ## xmm10 = xmm10[0],xmm5[0],xmm10[1],xmm5[1],xmm10[2],xmm5[2],xmm10[3],xmm5[3]
	paddd	%xmm11, %xmm9
	paddd	%xmm10, %xmm7
	movdqa	%xmm0, %xmm12
	psubq	%xmm4, %xmm12
	movdqa	%xmm0, %xmm13
	psubq	%xmm2, %xmm13
	movdqa	%xmm12, %xmm14
	shufps	$136, %xmm13, %xmm14            ## xmm14 = xmm14[0,2],xmm13[0,2]
	pshufd	$245, %xmm11, %xmm15            ## xmm15 = xmm11[1,1,3,3]
	pmuludq	%xmm14, %xmm11
	psubd	%xmm6, %xmm14
	pshufd	$232, %xmm11, %xmm11            ## xmm11 = xmm11[0,2,2,3]
	punpckhqdq	%xmm13, %xmm12          ## xmm12 = xmm12[1],xmm13[1]
	pmuludq	%xmm15, %xmm12
	pshufd	$232, %xmm12, %xmm12            ## xmm12 = xmm12[0,2,2,3]
	punpckldq	%xmm12, %xmm11          ## xmm11 = xmm11[0],xmm12[0],xmm11[1],xmm12[1]
	paddd	%xmm11, %xmm3
	pshufd	$245, %xmm10, %xmm11            ## xmm11 = xmm10[1,1,3,3]
	pmuludq	%xmm14, %xmm10
	pshufd	$232, %xmm10, %xmm10            ## xmm10 = xmm10[0,2,2,3]
	pshufd	$245, %xmm14, %xmm12            ## xmm12 = xmm14[1,1,3,3]
	pmuludq	%xmm11, %xmm12
	pshufd	$232, %xmm12, %xmm11            ## xmm11 = xmm12[0,2,2,3]
	punpckldq	%xmm11, %xmm10          ## xmm10 = xmm10[0],xmm11[0],xmm10[1],xmm11[1]
	paddd	%xmm10, %xmm1
	addq	$8, %rdi
	paddq	%xmm8, %xmm4
	paddq	%xmm8, %xmm2
	cmpq	%rdi, %rdx
	jne	LBB12_19
## %bb.20:
	paddd	%xmm9, %xmm7
	pshufd	$238, %xmm7, %xmm0              ## xmm0 = xmm7[2,3,2,3]
	paddd	%xmm7, %xmm0
	pshufd	$85, %xmm0, %xmm2               ## xmm2 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm2
	movd	%xmm2, %esi
	paddd	%xmm3, %xmm1
	pshufd	$238, %xmm1, %xmm0              ## xmm0 = xmm1[2,3,2,3]
	paddd	%xmm1, %xmm0
	pshufd	$85, %xmm0, %xmm1               ## xmm1 = xmm0[1,1,1,1]
	paddd	%xmm0, %xmm1
	movd	%xmm1, %edi
	cmpq	%rdx, %rbx
	je	LBB12_23
LBB12_21:
	movq	%rbx, %r8
	subq	%rdx, %r8
	movl	%ebx, %r9d
	subl	%edx, %r9d
	addq	%rdx, %r10
	addq	%r10, %r11
	xorl	%edx, %edx
	.p2align	4
LBB12_22:                               ## =>This Inner Loop Header: Depth=1
	movzbl	(%r11,%rdx), %r10d
	addl	%r10d, %esi
	imull	%r9d, %r10d
	addl	%r10d, %edi
	incq	%rdx
	decl	%r9d
	cmpq	%rdx, %r8
	jne	LBB12_22
LBB12_23:
	movl	38172(%r13), %edx
	addl	%edx, %esi
	imull	%ebx, %edx
	addl	38176(%r13), %edi
	addl	%edx, %edi
	movl	$2147975281, %edx               ## imm = 0x80078071
	movq	%rdi, %r8
	imulq	%rdx, %r8
	shrq	$47, %r8
	imull	$65521, %r8d, %r8d              ## imm = 0xFFF1
	subl	%r8d, %edi
	movl	%edi, 38176(%r13)
	imulq	%rsi, %rdx
	shrq	$47, %rdx
	imull	$65521, %edx, %edx              ## imm = 0xFFF1
	subl	%edx, %esi
	movl	%esi, 38172(%r13)
	jmp	LBB12_8
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream9end_blockEv ## -- Begin function _ZN9gui_forms11compression13InflateStream9end_blockEv
	.p2align	4
__ZN9gui_forms11compression13InflateStream9end_blockEv: ## @_ZN9gui_forms11compression13InflateStream9end_blockEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movl	$3, 32(%rdi)
	cmpb	$1, 38168(%rdi)
	jne	LBB13_2
## %bb.1:
	movq	$0, 38120(%rdi)
	xorl	%eax, %eax
	cmpl	$1, (%rdi)
	sete	%al
	addl	$17, %eax
	movl	%eax, 32(%rdi)
LBB13_2:
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE ## -- Begin function _ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	.p2align	4
__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE: ## @_ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
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
	movl	$0, -52(%rbp)
	movl	32(%rdi), %eax
	cmpq	$19, %rax
	ja	LBB14_2
## %bb.1:
	leaq	LJTI14_0(%rip), %rcx
	movslq	(%rcx,%rax,4), %rax
	addq	%rcx, %rax
	jmpq	*%rax
LBB14_2:
	movabsq	$8589934611, %rax               ## imm = 0x200000013
	jmp	LBB14_190
LBB14_3:
	movl	38128(%rdi), %ecx
	movl	38132(%rdi), %eax
	addl	%ecx, %eax
	cmpl	%eax, 38140(%rdi)
	jne	LBB14_93
## %bb.4:
	cmpb	$0, 38020(%rdi)
	je	LBB14_147
## %bb.5:
	leaq	37764(%rdi), %rbx
	leaq	32856(%rdi), %rsi
	movq	%rdi, %r14
	movq	%rbx, %rdx
	xorl	%r8d, %r8d
	callq	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	movl	%eax, %ecx
	xorl	%eax, %eax
	testb	%cl, %cl
	je	LBB14_192
## %bb.6:
	movq	%r14, %rdi
	movl	38128(%r14), %eax
	movl	38132(%r14), %ecx
	addq	%rax, %rbx
	leaq	34492(%r14), %rsi
	movq	%rbx, %rdx
	xorl	%r8d, %r8d
	callq	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	movl	%eax, %ecx
	xorl	%eax, %eax
	testb	%cl, %cl
	je	LBB14_192
## %bb.7:
	movl	$11, 32(%r14)
	movb	$1, %al
	jmp	LBB14_192
LBB14_8:
	movl	38144(%rdi), %eax
	cmpl	$16, %eax
	jne	LBB14_96
## %bb.9:
	movl	38140(%rdi), %ecx
	decl	%ecx
	movzbl	37764(%rdi,%rcx), %edx
	jmp	LBB14_97
LBB14_10:
	movl	38124(%rdi), %eax
	cmpl	$14, %eax
	jae	LBB14_105
## %bb.11:
	movq	8(%rsi), %rbx
	movq	32(%rsi), %r9
	cmpq	%rbx, %r9
	je	LBB14_99
## %bb.12:
	movq	8(%rdi), %r15
	movq	40(%rdi), %r10
	cmpq	%r15, %r10
	je	LBB14_142
## %bb.13:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r11
	cmpq	%r11, %rdx
	je	LBB14_189
## %bb.14:
	movq	(%rsi), %rcx
	movl	38120(%rdi), %r13d
	leaq	1(%r11), %r8
	movq	%r8, 56(%rdi)
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	movzbl	(%rcx,%r9), %r14d
	movl	%eax, %ecx
	shll	%cl, %r14d
	orl	%r13d, %r14d
	movl	%r14d, 38120(%rdi)
	leal	8(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	1(%r9), %r13
	movq	%r13, 32(%rsi)
	leaq	1(%r10), %r12
	movq	%r12, 40(%rdi)
	cmpl	$5, %eax
	ja	LBB14_179
## %bb.15:
	cmpq	%rbx, %r13
	je	LBB14_99
## %bb.16:
	leaq	1(%r10), %rbx
	cmpq	%r15, %rbx
	je	LBB14_142
## %bb.17:
	cmpq	%r8, %rdx
	je	LBB14_189
## %bb.18:
	addq	$2, %r11
	movq	%r11, 56(%rdi)
	movq	-48(%rbp), %r8                  ## 8-byte Reload
	movzbl	1(%r8,%r9), %r8d
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r8d
	orl	%r14d, %r8d
	movl	%r8d, 38120(%rdi)
	orl	$16, %eax
	movl	%eax, 38124(%rdi)
	addq	$2, %r9
	movq	%r9, 32(%rsi)
	addq	$2, %r10
	movq	%r10, 40(%rdi)
	movq	%r11, %r8
	cmpq	%r8, %rdx
	jne	LBB14_106
	jmp	LBB14_189
LBB14_19:
	movl	38124(%rdi), %eax
	cmpl	$16, %eax
	jae	LBB14_108
## %bb.20:
	movq	8(%rsi), %rbx
	movq	32(%rsi), %r8
	cmpq	%rbx, %r8
	je	LBB14_99
## %bb.21:
	movq	8(%rdi), %r15
	movq	40(%rdi), %r9
	cmpq	%r15, %r9
	je	LBB14_142
## %bb.22:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r10
	cmpq	%r10, %rdx
	je	LBB14_189
## %bb.23:
	movq	(%rsi), %rcx
	movl	38120(%rdi), %r13d
	leaq	1(%r10), %r11
	movq	%r11, 56(%rdi)
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	movzbl	(%rcx,%r8), %r14d
	movl	%eax, %ecx
	shll	%cl, %r14d
	orl	%r13d, %r14d
	movl	%r14d, 38120(%rdi)
	leal	8(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	1(%r8), %r13
	movq	%r13, 32(%rsi)
	leaq	1(%r9), %r12
	movq	%r12, 40(%rdi)
	cmpl	$7, %eax
	ja	LBB14_180
## %bb.24:
	cmpq	%rbx, %r13
	je	LBB14_99
## %bb.25:
	leaq	1(%r9), %rbx
	cmpq	%r15, %rbx
	je	LBB14_142
## %bb.26:
	cmpq	%r11, %rdx
	je	LBB14_189
## %bb.27:
	addq	$2, %r10
	movq	%r10, 56(%rdi)
	movq	-48(%rbp), %r11                 ## 8-byte Reload
	movzbl	1(%r11,%r8), %r11d
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r11d
	orl	%r14d, %r11d
	movl	%r11d, 38120(%rdi)
	orl	$16, %eax
	movl	%eax, 38124(%rdi)
	addq	$2, %r8
	movq	%r8, 32(%rsi)
	addq	$2, %r9
	movq	%r9, 40(%rdi)
	movq	%r10, %r11
	cmpq	%r11, %rdx
	jne	LBB14_109
	jmp	LBB14_189
LBB14_28:
	movl	38124(%rdi), %eax
	movl	38156(%rdi), %ecx
	movl	%ecx, -48(%rbp)                 ## 4-byte Spill
	cmpl	%ecx, %eax
	jae	LBB14_111
## %bb.29:
	movq	24(%rdi), %rcx
	movq	40(%rdi), %r10
	movq	32(%rsi), %r9
	movq	56(%rdi), %r8
	movl	38120(%rdi), %ebx
	movq	%r8, %r14
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	subq	%rcx, %r14
	movq	%r10, %r15
	subq	8(%rdi), %r15
	movq	(%rsi), %r12
	addq	%r9, %r12
	movq	%r9, %r11
	subq	8(%rsi), %r11
	xorl	%r13d, %r13d
LBB14_30:                               ## =>This Inner Loop Header: Depth=1
	movq	%r11, %rcx
	addq	%r13, %rcx
	je	LBB14_99
## %bb.31:                              ##   in Loop: Header=BB14_30 Depth=1
	movq	%r15, %rcx
	addq	%r13, %rcx
	je	LBB14_142
## %bb.32:                              ##   in Loop: Header=BB14_30 Depth=1
	movq	%r14, %rcx
	addq	%r13, %rcx
	je	LBB14_189
## %bb.33:                              ##   in Loop: Header=BB14_30 Depth=1
	leaq	(%r8,%r13), %rcx
	incq	%rcx
	movq	%rcx, 56(%rdi)
	movzbl	(%r12,%r13), %edx
	movl	%eax, %ecx
	shll	%cl, %edx
	orl	%edx, %ebx
	movl	%ebx, 38120(%rdi)
	addl	$8, %eax
	movl	%eax, 38124(%rdi)
	leaq	(%r9,%r13), %rcx
	incq	%rcx
	movq	%rcx, 32(%rsi)
	leaq	(%r10,%r13), %rcx
	incq	%rcx
	movq	%rcx, 40(%rdi)
	incq	%r13
	cmpl	-48(%rbp), %eax                 ## 4-byte Folded Reload
	jb	LBB14_30
## %bb.34:
	addq	%r13, %r8
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	jmp	LBB14_112
LBB14_35:
	movl	38124(%rdi), %ecx
	cmpl	$3, %ecx
	jae	LBB14_114
## %bb.36:
	movq	32(%rsi), %r8
	cmpq	8(%rsi), %r8
	je	LBB14_99
## %bb.37:
	movq	40(%rdi), %r9
	cmpq	8(%rdi), %r9
	je	LBB14_142
## %bb.38:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.39:
	movq	(%rsi), %r10
	movl	38120(%rdi), %r11d
	incq	%rax
	movq	%rax, 56(%rdi)
	movzbl	(%r10,%r8), %r10d
	shll	%cl, %r10d
	orl	%r11d, %r10d
	movl	%r10d, 38120(%rdi)
	orl	$8, %ecx
	movl	%ecx, 38124(%rdi)
	incq	%r8
	movq	%r8, 32(%rsi)
	incq	%r9
	movq	%r9, 40(%rdi)
	jmp	LBB14_115
LBB14_40:
	movl	38124(%rdi), %eax
	cmpl	$16, %eax
	jae	LBB14_118
## %bb.41:
	movq	8(%rsi), %rbx
	movq	32(%rsi), %r8
	cmpq	%rbx, %r8
	je	LBB14_99
## %bb.42:
	movq	8(%rdi), %r15
	movq	40(%rdi), %r9
	cmpq	%r15, %r9
	je	LBB14_142
## %bb.43:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r10
	cmpq	%r10, %rdx
	je	LBB14_189
## %bb.44:
	movq	(%rsi), %rcx
	movl	38120(%rdi), %r13d
	leaq	1(%r10), %r11
	movq	%r11, 56(%rdi)
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	movzbl	(%rcx,%r8), %r14d
	movl	%eax, %ecx
	shll	%cl, %r14d
	orl	%r13d, %r14d
	movl	%r14d, 38120(%rdi)
	leal	8(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	1(%r8), %r13
	movq	%r13, 32(%rsi)
	leaq	1(%r9), %r12
	movq	%r12, 40(%rdi)
	cmpl	$7, %eax
	ja	LBB14_181
## %bb.45:
	cmpq	%rbx, %r13
	je	LBB14_99
## %bb.46:
	leaq	1(%r9), %rbx
	cmpq	%r15, %rbx
	je	LBB14_142
## %bb.47:
	cmpq	%r11, %rdx
	je	LBB14_189
## %bb.48:
	addq	$2, %r10
	movq	%r10, 56(%rdi)
	movq	-48(%rbp), %r11                 ## 8-byte Reload
	movzbl	1(%r11,%r8), %r11d
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r11d
	orl	%r14d, %r11d
	movl	%r11d, 38120(%rdi)
	orl	$16, %eax
	movl	%eax, 38124(%rdi)
	addq	$2, %r8
	movq	%r8, 32(%rsi)
	addq	$2, %r9
	movq	%r9, 40(%rdi)
	movq	%r10, %r11
	cmpq	%r11, %rdx
	jne	LBB14_119
	jmp	LBB14_189
LBB14_49:
	movl	38140(%rdi), %eax
	cmpl	38136(%rdi), %eax
	jae	LBB14_120
## %bb.50:
	movl	38124(%rdi), %ecx
	cmpl	$3, %ecx
	jae	LBB14_143
## %bb.51:
	movq	32(%rsi), %r9
	cmpq	8(%rsi), %r9
	je	LBB14_99
## %bb.52:
	movq	40(%rdi), %r10
	cmpq	8(%rdi), %r10
	je	LBB14_142
## %bb.53:
	movq	24(%rdi), %r8
	movq	56(%rdi), %rdx
	cmpq	%rdx, %r8
	je	LBB14_189
## %bb.54:
	movq	(%rsi), %r11
	movl	38120(%rdi), %ebx
	incq	%rdx
	movq	%rdx, 56(%rdi)
	movzbl	(%r11,%r9), %r11d
	shll	%cl, %r11d
	orl	%ebx, %r11d
	movl	%r11d, 38120(%rdi)
	orl	$8, %ecx
	movl	%ecx, 38124(%rdi)
	incq	%r9
	movq	%r9, 32(%rsi)
	incq	%r10
	movq	%r10, 40(%rdi)
	jmp	LBB14_144
LBB14_55:
	cmpl	$0, 38164(%rdi)
	je	LBB14_139
## %bb.56:
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	jmp	__ZN9gui_forms11compression13InflateStream12stored_bytesERNS1_4CallE ## TAILCALL
LBB14_57:
	movl	38124(%rdi), %ecx
	cmpl	$8, %ecx
	jae	LBB14_122
## %bb.58:
	movq	32(%rsi), %r8
	cmpq	8(%rsi), %r8
	je	LBB14_99
## %bb.59:
	movq	40(%rdi), %r9
	cmpq	8(%rdi), %r9
	je	LBB14_142
## %bb.60:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.61:
	movq	(%rsi), %r10
	movl	38120(%rdi), %r11d
	incq	%rax
	movq	%rax, 56(%rdi)
	movzbl	(%r10,%r8), %r10d
	shll	%cl, %r10d
	orl	%r11d, %r10d
	movl	%r10d, 38120(%rdi)
	orl	$8, %ecx
	movl	%ecx, 38124(%rdi)
	incq	%r8
	movq	%r8, 32(%rsi)
	incq	%r9
	movq	%r9, 40(%rdi)
	jmp	LBB14_123
LBB14_62:
	movl	38124(%rdi), %eax
	cmpl	$16, %eax
	jae	LBB14_126
## %bb.63:
	movq	8(%rsi), %rbx
	movq	32(%rsi), %r8
	cmpq	%rbx, %r8
	je	LBB14_99
## %bb.64:
	movq	8(%rdi), %r15
	movq	40(%rdi), %r9
	cmpq	%r15, %r9
	je	LBB14_142
## %bb.65:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r10
	cmpq	%r10, %rdx
	je	LBB14_189
## %bb.66:
	movq	(%rsi), %rcx
	movl	38120(%rdi), %r13d
	leaq	1(%r10), %r11
	movq	%r11, 56(%rdi)
	movq	%rcx, -48(%rbp)                 ## 8-byte Spill
	movzbl	(%rcx,%r8), %r14d
	movl	%eax, %ecx
	shll	%cl, %r14d
	orl	%r13d, %r14d
	movl	%r14d, 38120(%rdi)
	leal	8(%rax), %ecx
	movl	%ecx, 38124(%rdi)
	leaq	1(%r8), %r13
	movq	%r13, 32(%rsi)
	leaq	1(%r9), %r12
	movq	%r12, 40(%rdi)
	cmpl	$7, %eax
	ja	LBB14_182
## %bb.67:
	cmpq	%rbx, %r13
	je	LBB14_99
## %bb.68:
	leaq	1(%r9), %rbx
	cmpq	%r15, %rbx
	je	LBB14_142
## %bb.69:
	cmpq	%r11, %rdx
	je	LBB14_189
## %bb.70:
	addq	$2, %r10
	movq	%r10, 56(%rdi)
	movq	-48(%rbp), %r11                 ## 8-byte Reload
	movzbl	1(%r11,%r8), %r11d
                                        ## kill: def $cl killed $cl killed $ecx
	shll	%cl, %r11d
	orl	%r14d, %r11d
	movl	%r11d, 38120(%rdi)
	orl	$16, %eax
	movl	%eax, 38124(%rdi)
	addq	$2, %r8
	movq	%r8, 32(%rsi)
	addq	$2, %r9
	movq	%r9, 40(%rdi)
	movq	%r10, %r11
	cmpq	%r11, %rdx
	jne	LBB14_127
	jmp	LBB14_189
LBB14_71:
	movl	38124(%rdi), %eax
	movl	38156(%rdi), %ecx
	movl	%ecx, -48(%rbp)                 ## 4-byte Spill
	cmpl	%ecx, %eax
	jae	LBB14_130
## %bb.72:
	movq	24(%rdi), %rcx
	movq	40(%rdi), %r10
	movq	32(%rsi), %r9
	movq	56(%rdi), %r8
	movl	38120(%rdi), %ebx
	movq	%r8, %r14
	movq	%rcx, -64(%rbp)                 ## 8-byte Spill
	subq	%rcx, %r14
	movq	%r10, %r15
	subq	8(%rdi), %r15
	movq	(%rsi), %r12
	addq	%r9, %r12
	movq	%r9, %r11
	subq	8(%rsi), %r11
	xorl	%r13d, %r13d
LBB14_73:                               ## =>This Inner Loop Header: Depth=1
	movq	%r11, %rcx
	addq	%r13, %rcx
	je	LBB14_99
## %bb.74:                              ##   in Loop: Header=BB14_73 Depth=1
	movq	%r15, %rcx
	addq	%r13, %rcx
	je	LBB14_142
## %bb.75:                              ##   in Loop: Header=BB14_73 Depth=1
	movq	%r14, %rcx
	addq	%r13, %rcx
	je	LBB14_189
## %bb.76:                              ##   in Loop: Header=BB14_73 Depth=1
	leaq	(%r8,%r13), %rcx
	incq	%rcx
	movq	%rcx, 56(%rdi)
	movzbl	(%r12,%r13), %edx
	movl	%eax, %ecx
	shll	%cl, %edx
	orl	%edx, %ebx
	movl	%ebx, 38120(%rdi)
	addl	$8, %eax
	movl	%eax, 38124(%rdi)
	leaq	(%r9,%r13), %rcx
	incq	%rcx
	movq	%rcx, 32(%rsi)
	leaq	(%r10,%r13), %rcx
	incq	%rcx
	movq	%rcx, 40(%rdi)
	incq	%r13
	cmpl	-48(%rbp), %eax                 ## 4-byte Folded Reload
	jb	LBB14_73
## %bb.77:
	addq	%r13, %r8
	movq	-64(%rbp), %rcx                 ## 8-byte Reload
	jmp	LBB14_131
LBB14_78:
	cmpl	$0, 38148(%rdi)
	je	LBB14_198
## %bb.79:
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	jmp	__ZN9gui_forms11compression13InflateStream10copy_matchERNS1_4CallE ## TAILCALL
LBB14_80:
	movq	%rdi, %r14
	movq	%rsi, %rbx
	callq	__ZN9gui_forms11compression13InflateStream11literal_runERNS1_4CallE
	movl	%eax, %ecx
	movb	$1, %al
	testb	%cl, %cl
	jne	LBB14_192
## %bb.81:
	movq	%r14, %rdi
	leaq	32856(%r14), %rdx
	leaq	-52(%rbp), %rcx
	movq	%rbx, %rsi
	callq	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	testb	%al, %al
	je	LBB14_191
## %bb.82:
	movl	-52(%rbp), %ecx
	cmpl	$256, %ecx                      ## imm = 0x100
	jne	LBB14_166
## %bb.83:
	movl	$3, 32(%r14)
	cmpb	$1, 38168(%r14)
	movb	$1, %al
	jne	LBB14_192
## %bb.84:
	movq	$0, 38120(%r14)
	xorl	%ecx, %ecx
	cmpl	$1, (%r14)
	sete	%cl
	addl	$17, %ecx
	movl	%ecx, 32(%r14)
	jmp	LBB14_192
LBB14_85:
	movl	38124(%rdi), %ecx
	cmpl	$8, %ecx
	jae	LBB14_134
## %bb.86:
	movq	32(%rsi), %r8
	cmpq	8(%rsi), %r8
	je	LBB14_99
## %bb.87:
	movq	40(%rdi), %r9
	cmpq	8(%rdi), %r9
	je	LBB14_142
## %bb.88:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.89:
	movq	(%rsi), %r10
	movl	38120(%rdi), %r11d
	incq	%rax
	movq	%rax, 56(%rdi)
	movzbl	(%r10,%r8), %r10d
	shll	%cl, %r10d
	orl	%r11d, %r10d
	movl	%r10d, 38120(%rdi)
	orl	$8, %ecx
	movl	%ecx, 38124(%rdi)
	incq	%r8
	movq	%r8, 32(%rsi)
	incq	%r9
	movq	%r9, 40(%rdi)
	jmp	LBB14_135
LBB14_90:
	leaq	34492(%rdi), %rdx
	leaq	-52(%rbp), %rcx
	movq	%rdi, %rbx
	callq	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	testb	%al, %al
	je	LBB14_191
## %bb.91:
	movl	-52(%rbp), %eax
	cmpq	$30, %rax
	jb	LBB14_146
## %bb.92:
	movabsq	$85899345939, %rax              ## imm = 0x1400000013
	movq	%rax, 32(%rbx)
	jmp	LBB14_191
LBB14_93:
	leaq	36128(%rdi), %rdx
	leaq	-52(%rbp), %rcx
	movq	%rdi, %rbx
	callq	__ZN9gui_forms11compression13InflateStream6symbolERNS1_4CallERKNS1_4TreeERj
	testb	%al, %al
	je	LBB14_191
## %bb.94:
	movl	-52(%rbp), %eax
	cmpl	$15, %eax
	ja	LBB14_161
## %bb.95:
	movl	38140(%rbx), %ecx
	movb	%al, 37764(%rbx,%rcx)
	incl	38140(%rbx)
	movb	$1, %al
	jmp	LBB14_192
LBB14_96:
	xorl	%edx, %edx
LBB14_97:
	xorl	%r8d, %r8d
	cmpl	$17, %eax
	sete	%r8b
	orl	$2, %r8d
	xorl	%r10d, %r10d
	cmpl	$18, %eax
	sete	%r11b
	movl	$7, %ecx
	cmovnel	%r8d, %ecx
	movl	38124(%rdi), %eax
	cmpl	%ecx, %eax
	jae	LBB14_102
## %bb.98:
	movq	32(%rsi), %rbx
	cmpq	8(%rsi), %rbx
	jne	LBB14_141
LBB14_99:
	cmpb	$1, 48(%rsi)
	jne	LBB14_101
## %bb.100:
	movabsq	$25769803795, %rax              ## imm = 0x600000013
	movq	%rax, 32(%rdi)
LBB14_101:
	movl	$0, 52(%rsi)
	jmp	LBB14_191
LBB14_102:
	movq	24(%rdi), %r9
	movq	56(%rdi), %r8
	cmpq	%r8, %r9
	je	LBB14_189
LBB14_103:
	movb	%r11b, %r10b
	leal	3(,%r10,8), %r10d
	incq	%r8
	movq	%r8, 56(%rdi)
	movl	$-1, %esi
	shll	%cl, %esi
	notl	%esi
	movl	38120(%rdi), %r11d
	movl	38132(%rdi), %ebx
	andl	%r11d, %esi
	shrl	%cl, %r11d
	movl	%r11d, 38120(%rdi)
	subl	%ecx, %eax
	movl	%eax, 38124(%rdi)
	addl	38128(%rdi), %ebx
	addl	%r10d, %esi
	movl	38140(%rdi), %ecx
	subl	%ecx, %ebx
	cmpl	%ebx, %esi
	jbe	LBB14_150
## %bb.104:
	movabsq	$73014444051, %rax              ## imm = 0x1100000013
	jmp	LBB14_190
LBB14_105:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r8
	cmpq	%r8, %rdx
	je	LBB14_189
LBB14_106:
	leaq	1(%r8), %rcx
	movq	%rcx, 56(%rdi)
	movl	38120(%rdi), %esi
	movl	%esi, %r9d
	shrl	$14, %r9d
	movl	%r9d, 38120(%rdi)
	addl	$-14, %eax
	movl	%eax, 38124(%rdi)
	movl	%esi, %eax
	andl	$31, %eax
	leal	257(%rax), %r9d
	movl	%r9d, 38128(%rdi)
	movl	%esi, %r9d
	shrl	$5, %r9d
	andl	$31, %r9d
	leal	1(%r9), %r10d
	movl	%r10d, 38132(%rdi)
	shrl	$10, %esi
	andl	$15, %esi
	addl	$4, %esi
	movl	%esi, 38136(%rdi)
	cmpl	$30, %eax
	setae	%al
	cmpl	$30, %r9d
	setae	%sil
	orb	%al, %sil
	cmpb	$1, %sil
	jne	LBB14_157
## %bb.107:
	movabsq	$55834574867, %rax              ## imm = 0xD00000013
	jmp	LBB14_190
LBB14_108:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r11
	cmpq	%r11, %rdx
	je	LBB14_189
LBB14_109:
	incq	%r11
	movq	%r11, 56(%rdi)
	movl	38120(%rdi), %ecx
	movzwl	%cx, %edx
	shrl	$16, %ecx
	movl	%ecx, 38120(%rdi)
	addl	$-16, %eax
	movl	%eax, 38124(%rdi)
	xorl	38164(%rdi), %edx
	cmpl	$65535, %edx                    ## imm = 0xFFFF
	jne	LBB14_159
## %bb.110:
	movl	$6, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_111:
	movq	24(%rdi), %rcx
	movq	56(%rdi), %r8
LBB14_112:
	cmpq	%r8, %rcx
	je	LBB14_189
## %bb.113:
	incq	%r8
	movl	$-1, %edx
	movl	-48(%rbp), %ecx                 ## 4-byte Reload
	shll	%cl, %edx
	movq	%r8, 56(%rdi)
	notl	%edx
	movl	38120(%rdi), %esi
	andl	%esi, %edx
	shrl	%cl, %esi
	movl	%esi, 38120(%rdi)
	subl	%ecx, %eax
	movl	%eax, 38124(%rdi)
	addl	%edx, 38148(%rdi)
	movl	$14, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_114:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
LBB14_115:
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.116:
	leaq	1(%rax), %rsi
	movq	%rsi, 56(%rdi)
	movl	38120(%rdi), %r8d
	movl	%r8d, %r9d
	shrl	$3, %r9d
	movl	%r9d, 38120(%rdi)
	addl	$-3, %ecx
	movl	%ecx, 38124(%rdi)
	movl	%r8d, %ecx
	andb	$1, %cl
	movb	%cl, 38168(%rdi)
	shrl	%r8d
	andl	$3, %r8d
	leaq	LJTI14_1(%rip), %rcx
	movslq	(%rcx,%r8,4), %r8
	addq	%rcx, %r8
	jmpq	*%r8
LBB14_117:
	movq	$0, 38120(%rdi)
	movl	$4, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_118:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r11
	cmpq	%r11, %rdx
	je	LBB14_189
LBB14_119:
	incq	%r11
	movq	%r11, 56(%rdi)
	movl	38120(%rdi), %ecx
	movzwl	%cx, %edx
	movl	%edx, 38164(%rdi)
	shrl	$16, %ecx
	movl	%ecx, 38120(%rdi)
	addl	$-16, %eax
	movl	%eax, 38124(%rdi)
	movl	$5, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_120:
	leaq	36128(%rdi), %rsi
	leaq	38084(%rdi), %rdx
	movl	$19, %ecx
	movq	%rdi, %rbx
	movl	$1, %r8d
	callq	__ZN9gui_forms11compression13InflateStream10build_treeERNS1_4TreeENSt3__14spanIKhLm18446744073709551615EEEb
	testb	%al, %al
	je	LBB14_191
## %bb.121:
	movl	$0, 38140(%rbx)
	movl	$9, 32(%rbx)
	movb	$1, %al
	jmp	LBB14_192
LBB14_122:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
LBB14_123:
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.124:
	incq	%rax
	movq	%rax, 56(%rdi)
	movl	38120(%rdi), %eax
	movl	38184(%rdi), %edx
	movzbl	%al, %esi
	shrl	$8, %eax
	movl	%eax, 38120(%rdi)
	addl	$-8, %ecx
	movl	%ecx, 38124(%rdi)
	movl	38188(%rdi), %eax
	shll	$8, %eax
	orl	%esi, %eax
	movl	%eax, 38188(%rdi)
	incl	%edx
	movl	%edx, 38184(%rdi)
	movb	$1, %al
	cmpl	$4, %edx
	jne	LBB14_192
## %bb.125:
	movabsq	$42949672979, %rax              ## imm = 0xA00000013
	jmp	LBB14_190
LBB14_126:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %r11
	cmpq	%r11, %rdx
	je	LBB14_189
LBB14_127:
	incq	%r11
	movq	%r11, 56(%rdi)
	movl	38120(%rdi), %edx
	movl	%edx, %ecx
	shrl	$16, %ecx
	movl	%ecx, 38120(%rdi)
	addl	$-16, %eax
	movl	%eax, 38124(%rdi)
	movl	%edx, %eax
	andl	$15, %eax
	cmpl	$8, %eax
	jne	LBB14_160
## %bb.128:
	movl	%edx, %eax
	andl	$248, %eax
	movl	%eax, %ecx
	shrl	$4, %ecx
	addq	$8, %rcx
	cmpl	4(%rdi), %ecx
	seta	%sil
	cmpl	$127, %eax
	seta	%al
	orb	%sil, %al
	cmpb	$1, %al
	jne	LBB14_174
## %bb.129:
	movabsq	$34359738387, %rax              ## imm = 0x800000013
	jmp	LBB14_190
LBB14_130:
	movq	24(%rdi), %rcx
	movq	56(%rdi), %r8
LBB14_131:
	cmpq	%r8, %rcx
	je	LBB14_189
## %bb.132:
	incq	%r8
	movq	%r8, 56(%rdi)
	movl	$-1, %esi
	movl	-48(%rbp), %ecx                 ## 4-byte Reload
	shll	%cl, %esi
	notl	%esi
	movl	38120(%rdi), %edx
	andl	%edx, %esi
	shrl	%cl, %edx
	movl	%edx, 38120(%rdi)
	subl	%ecx, %eax
	movl	%eax, 38124(%rdi)
	addl	38152(%rdi), %esi
	movl	%esi, 38152(%rdi)
	cmpq	%rsi, 32848(%rdi)
	jae	LBB14_148
## %bb.133:
	movabsq	$94489280531, %rax              ## imm = 0x1600000013
	jmp	LBB14_190
LBB14_134:
	movq	24(%rdi), %rdx
	movq	56(%rdi), %rax
LBB14_135:
	cmpq	%rax, %rdx
	je	LBB14_189
## %bb.136:
	incq	%rax
	movq	%rax, 56(%rdi)
	movl	38120(%rdi), %eax
	movl	38180(%rdi), %edx
	movzbl	%al, %esi
	shrl	$8, %eax
	movl	%eax, 38120(%rdi)
	addl	$-8, %ecx
	movl	%ecx, 38124(%rdi)
	shll	$8, %edx
	orl	%esi, %edx
	movl	%edx, 38180(%rdi)
	movl	38184(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38184(%rdi)
	movb	$1, %al
	cmpl	$4, %ecx
	jne	LBB14_192
## %bb.137:
	movl	38176(%rdi), %ecx
	shll	$16, %ecx
	orl	38172(%rdi), %ecx
	cmpl	%ecx, %edx
	jne	LBB14_173
## %bb.138:
	movl	$18, 32(%rdi)
	jmp	LBB14_192
LBB14_139:
	movl	$3, 32(%rdi)
	movb	$1, %al
	cmpb	$1, 38168(%rdi)
	jne	LBB14_192
## %bb.140:
	movq	$0, 38120(%rdi)
	xorl	%ecx, %ecx
	cmpl	$1, (%rdi)
	sete	%cl
	addl	$17, %ecx
	movl	%ecx, 32(%rdi)
	jmp	LBB14_192
LBB14_141:
	movq	40(%rdi), %r14
	cmpq	8(%rdi), %r14
	jne	LBB14_164
LBB14_142:
	movabsq	$12884901907, %rax              ## imm = 0x300000013
	jmp	LBB14_190
LBB14_143:
	movq	24(%rdi), %r8
	movq	56(%rdi), %rdx
LBB14_144:
	cmpq	%rdx, %r8
	je	LBB14_189
## %bb.145:
	incq	%rdx
	movq	%rdx, 56(%rdi)
	movl	38120(%rdi), %edx
	movl	%edx, %esi
	shrl	$3, %esi
	movl	%esi, 38120(%rdi)
	addl	$-3, %ecx
	movl	%ecx, 38124(%rdi)
	leaq	__ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE(%rip), %rcx
	movl	(%rcx,%rax,4), %eax
	andb	$7, %dl
	movb	%dl, 38084(%rdi,%rax)
	incl	38140(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_146:
	leaq	__ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE(%rip), %rcx
	movl	(%rcx,%rax,4), %ecx
	movl	%ecx, 38152(%rbx)
	leaq	__ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE(%rip), %rcx
	movl	(%rcx,%rax,4), %eax
	movl	%eax, 38156(%rbx)
	movl	$15, 32(%rbx)
	movb	$1, %al
	jmp	LBB14_192
LBB14_147:
	movabsq	$68719476755, %rax              ## imm = 0x1000000013
	jmp	LBB14_190
LBB14_148:
	cmpq	%rsi, 32840(%rdi)
	jae	LBB14_168
## %bb.149:
	movabsq	$90194313235, %rax              ## imm = 0x1500000013
	jmp	LBB14_190
LBB14_150:
	movl	%esi, %eax
	subq	%r8, %r9
	cmpq	%rax, %r9
	jb	LBB14_189
## %bb.151:
	addq	%rax, %r8
	movq	%r8, 56(%rdi)
	movl	%esi, %eax
	andl	$3, %eax
	cmpl	$4, %esi
	jb	LBB14_155
## %bb.152:
	andl	$-4, %esi
LBB14_153:                              ## =>This Inner Loop Header: Depth=1
	movl	%ecx, %ecx
	movb	%dl, 37764(%rdi,%rcx)
	movl	38140(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38140(%rdi)
	movb	%dl, 37764(%rdi,%rcx)
	movl	38140(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38140(%rdi)
	movb	%dl, 37764(%rdi,%rcx)
	movl	38140(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38140(%rdi)
	movb	%dl, 37764(%rdi,%rcx)
	movl	38140(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38140(%rdi)
	addl	$-4, %esi
	jne	LBB14_153
## %bb.154:
	testl	%eax, %eax
	je	LBB14_156
LBB14_155:                              ## =>This Inner Loop Header: Depth=1
	movl	%ecx, %ecx
	movb	%dl, 37764(%rdi,%rcx)
	movl	38140(%rdi), %ecx
	incl	%ecx
	movl	%ecx, 38140(%rdi)
	decl	%eax
	jne	LBB14_155
LBB14_156:
	movl	$9, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_157:
	subq	%rcx, %rdx
	cmpq	$18, %rdx
	jbe	LBB14_189
## %bb.158:
	addq	$20, %r8
	movq	%r8, 56(%rdi)
	xorps	%xmm0, %xmm0
	movups	%xmm0, 38084(%rdi)
	movl	$0, 38099(%rdi)
	movl	$0, 38140(%rdi)
	movl	$8, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_159:
	movabsq	$51539607571, %rax              ## imm = 0xC00000013
	jmp	LBB14_190
LBB14_160:
	movabsq	$30064771091, %rax              ## imm = 0x700000013
	jmp	LBB14_190
LBB14_161:
	cmpl	$16, %eax
	jne	LBB14_176
## %bb.162:
	cmpl	$0, 38140(%rbx)
	jne	LBB14_176
## %bb.163:
	movabsq	$73014444051, %rax              ## imm = 0x1100000013
	movq	%rax, 32(%rbx)
	jmp	LBB14_191
LBB14_164:
	movq	24(%rdi), %r9
	movq	56(%rdi), %r8
	cmpq	%r8, %r9
	je	LBB14_189
## %bb.165:
	movq	(%rsi), %r15
	movl	38120(%rdi), %r12d
	incq	%r8
	movq	%r8, 56(%rdi)
	movzbl	(%r15,%rbx), %r13d
	movl	%ecx, %r15d
	movl	%eax, %ecx
	shll	%cl, %r13d
	movl	%r15d, %ecx
	orl	%r12d, %r13d
	movl	%r13d, 38120(%rdi)
	addl	$8, %eax
	movl	%eax, 38124(%rdi)
	incq	%rbx
	movq	%rbx, 32(%rsi)
	incq	%r14
	movq	%r14, 40(%rdi)
	cmpq	%r8, %r9
	jne	LBB14_103
	jmp	LBB14_189
LBB14_166:
	cmpl	$286, %ecx                      ## imm = 0x11E
	movq	%r14, %rdi
	movb	$1, %al
	jb	LBB14_177
## %bb.167:
	movabsq	$81604378643, %rax              ## imm = 0x1300000013
	jmp	LBB14_190
LBB14_168:
	movl	$16, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_169:
	movl	$7, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_170:
	movabsq	$47244640275, %rax              ## imm = 0xB00000013
	jmp	LBB14_190
LBB14_171:
	subq	%rsi, %rdx
	cmpq	$3271, %rdx                     ## imm = 0xCC7
	jbe	LBB14_189
## %bb.172:
	addq	$3273, %rax                     ## imm = 0xCC9
	movq	%rax, 56(%rdi)
	leaq	32856(%rdi), %rax
	leaq	__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE12literal_tree(%rip), %rsi
	movl	$1636, %edx                     ## imm = 0x664
	movq	%rdi, %rbx
	movq	%rax, %rdi
	callq	_memcpy
	leaq	34492(%rbx), %rdi
	leaq	__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE13distance_tree(%rip), %rsi
	movl	$1636, %edx                     ## imm = 0x664
	callq	_memcpy
	movl	$11, 32(%rbx)
	movb	$1, %al
	jmp	LBB14_192
LBB14_173:
	movabsq	$98784247827, %rax              ## imm = 0x1700000013
	jmp	LBB14_190
LBB14_174:
	movl	%edx, %eax
	rolw	$8, %ax
	imull	$31711, %eax, %eax              ## imm = 0x7BDF
	movzwl	%ax, %eax
	cmpl	$2115, %eax                     ## imm = 0x843
	jb	LBB14_199
## %bb.175:
	movabsq	$38654705683, %rax              ## imm = 0x900000013
	jmp	LBB14_190
LBB14_176:
	movl	%eax, 38144(%rbx)
	movl	$10, 32(%rbx)
	movb	$1, %al
	jmp	LBB14_192
LBB14_177:
	cmpl	$257, %ecx                      ## imm = 0x101
	jb	LBB14_183
## %bb.178:
	addl	$-257, %ecx                     ## imm = 0xFEFF
	leaq	__ZN9gui_forms11compression12_GLOBAL__N_111length_baseE(%rip), %rdx
	movl	(%rdx,%rcx,4), %edx
	movl	%edx, 38148(%rdi)
	leaq	__ZN9gui_forms11compression12_GLOBAL__N_112length_extraE(%rip), %rdx
	movl	(%rdx,%rcx,4), %ecx
	movl	%ecx, 38156(%rdi)
	movl	$13, 32(%rdi)
	jmp	LBB14_192
LBB14_179:
	movl	%ecx, %eax
	cmpq	%r8, %rdx
	jne	LBB14_106
	jmp	LBB14_189
LBB14_180:
	movl	%ecx, %eax
	cmpq	%r11, %rdx
	jne	LBB14_109
	jmp	LBB14_189
LBB14_181:
	movl	%ecx, %eax
	cmpq	%r11, %rdx
	jne	LBB14_119
	jmp	LBB14_189
LBB14_182:
	movl	%ecx, %eax
	cmpq	%r11, %rdx
	jne	LBB14_127
	jmp	LBB14_189
LBB14_183:
	movl	%ecx, 38160(%rdi)
	movl	$12, 32(%rdi)
	movq	%rbx, %rsi
LBB14_184:
	movq	48(%rdi), %rax
	cmpq	16(%rdi), %rax
	jne	LBB14_186
## %bb.185:
	movabsq	$17179869203, %rax              ## imm = 0x400000013
	jmp	LBB14_190
LBB14_186:
	movq	40(%rsi), %rcx
	cmpq	24(%rsi), %rcx
	jne	LBB14_188
## %bb.187:
	movl	$1, 52(%rsi)
	jmp	LBB14_191
LBB14_188:
	movq	56(%rdi), %rax
	cmpq	%rax, 24(%rdi)
	jne	LBB14_193
LBB14_189:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
LBB14_190:
	movq	%rax, 32(%rdi)
LBB14_191:
	xorl	%eax, %eax
LBB14_192:
                                        ## kill: def $al killed $al killed $eax
	addq	$24, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
LBB14_193:
	incq	%rax
	movq	%rax, 56(%rdi)
	movl	38160(%rdi), %eax
	movq	16(%rsi), %rdx
	movb	%al, (%rdx,%rcx)
	incq	40(%rsi)
	incq	48(%rdi)
	movq	32832(%rdi), %rcx
	movb	%al, 64(%rdi,%rcx)
	movl	32832(%rdi), %ecx
	incl	%ecx
	andl	$32767, %ecx                    ## imm = 0x7FFF
	movq	%rcx, 32832(%rdi)
	movq	32840(%rdi), %rcx
	cmpq	$32767, %rcx                    ## imm = 0x7FFF
	ja	LBB14_195
## %bb.194:
	incq	%rcx
	movq	%rcx, 32840(%rdi)
LBB14_195:
	cmpl	$1, (%rdi)
	je	LBB14_198
## %bb.196:
	movzbl	%al, %ecx
	addl	38172(%rdi), %ecx
	movl	%ecx, %eax
	subl	$65521, %eax                    ## imm = 0xFFF1
	cmovbl	%ecx, %eax
	movl	%eax, 38172(%rdi)
	addl	38176(%rdi), %eax
	movl	%eax, 38176(%rdi)
	cmpl	$65521, %eax                    ## imm = 0xFFF1
	jb	LBB14_198
## %bb.197:
	addl	$-65521, %eax                   ## imm = 0xFFFF000F
	movl	%eax, 38176(%rdi)
LBB14_198:
	movl	$11, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
LBB14_199:
	movl	$1, %eax
                                        ## kill: def $cl killed $cl killed $rcx
	shlq	%cl, %rax
	movq	%rax, 32848(%rdi)
	andl	$8192, %edx                     ## imm = 0x2000
	xorl	%eax, %eax
	cmpl	$1, %edx
	adcl	$2, %eax
	movl	%eax, 32(%rdi)
	movb	$1, %al
	jmp	LBB14_192
	.cfi_endproc
	.p2align	2
	.data_region jt32
L14_0_set_2 = LBB14_2-LJTI14_0
L14_0_set_62 = LBB14_62-LJTI14_0
L14_0_set_57 = LBB14_57-LJTI14_0
L14_0_set_35 = LBB14_35-LJTI14_0
L14_0_set_40 = LBB14_40-LJTI14_0
L14_0_set_19 = LBB14_19-LJTI14_0
L14_0_set_55 = LBB14_55-LJTI14_0
L14_0_set_10 = LBB14_10-LJTI14_0
L14_0_set_49 = LBB14_49-LJTI14_0
L14_0_set_3 = LBB14_3-LJTI14_0
L14_0_set_8 = LBB14_8-LJTI14_0
L14_0_set_80 = LBB14_80-LJTI14_0
L14_0_set_184 = LBB14_184-LJTI14_0
L14_0_set_28 = LBB14_28-LJTI14_0
L14_0_set_90 = LBB14_90-LJTI14_0
L14_0_set_71 = LBB14_71-LJTI14_0
L14_0_set_78 = LBB14_78-LJTI14_0
L14_0_set_85 = LBB14_85-LJTI14_0
L14_0_set_191 = LBB14_191-LJTI14_0
LJTI14_0:
	.long	L14_0_set_2
	.long	L14_0_set_62
	.long	L14_0_set_57
	.long	L14_0_set_35
	.long	L14_0_set_40
	.long	L14_0_set_19
	.long	L14_0_set_55
	.long	L14_0_set_10
	.long	L14_0_set_49
	.long	L14_0_set_3
	.long	L14_0_set_8
	.long	L14_0_set_80
	.long	L14_0_set_184
	.long	L14_0_set_28
	.long	L14_0_set_90
	.long	L14_0_set_71
	.long	L14_0_set_78
	.long	L14_0_set_85
	.long	L14_0_set_191
	.long	L14_0_set_191
L14_1_set_117 = LBB14_117-LJTI14_1
L14_1_set_171 = LBB14_171-LJTI14_1
L14_1_set_169 = LBB14_169-LJTI14_1
L14_1_set_170 = LBB14_170-LJTI14_1
LJTI14_1:
	.long	L14_1_set_117
	.long	L14_1_set_171
	.long	L14_1_set_169
	.long	L14_1_set_170
	.end_data_region
                                        ## -- End function
	.globl	__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb ## -- Begin function _ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
	.p2align	4
__ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb: ## @_ZN9gui_forms11compression13InflateStream7processENSt3__14spanIKSt4byteLm18446744073709551615EEENS3_IS4_Lm18446744073709551615EEEb
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
	subq	$56, %rsp
	.cfi_offset %rbx, -40
	.cfi_offset %r14, -32
	.cfi_offset %r15, -24
	movq	%rsi, %r14
	movq	%rdi, %rbx
	movzbl	16(%rbp), %eax
	movq	%rdx, -80(%rbp)
	movq	%rcx, -72(%rbp)
	movq	%r8, -64(%rbp)
	movq	%r9, -56(%rbp)
	xorps	%xmm0, %xmm0
	movups	%xmm0, -48(%rbp)
	movb	%al, -32(%rbp)
	movl	$0, -28(%rbp)
	testq	%rcx, %rcx
	je	LBB15_3
## %bb.1:
	testq	%r9, %r9
	je	LBB15_3
## %bb.2:
	movq	%rdx, %rax
	subq	%r8, %rax
	movq	%rax, %rdi
	negq	%rdi
	xorl	%esi, %esi
	cmpq	%rcx, %rdi
	setb	%sil
	xorl	%edi, %edi
	cmpq	%r9, %rax
	setb	%dil
	cmpq	%r8, %rdx
	cmoval	%edi, %esi
	jmp	LBB15_6
LBB15_3:
	testq	%rcx, %rcx
	je	LBB15_4
## %bb.5:
	xorl	%esi, %esi
LBB15_6:
	movq	%r14, %rdi
	subq	%rdx, %rdi
	xorl	%eax, %eax
	cmpq	%rcx, %rdi
	setb	%al
	movq	%rdx, %rcx
	subq	%r14, %rcx
	xorl	%edi, %edi
	cmpq	$38192, %rcx                    ## imm = 0x9530
	setb	%dil
	cmpq	%r14, %rdx
	cmoval	%edi, %eax
	orb	%sil, %al
	testq	%r9, %r9
	je	LBB15_8
LBB15_9:
	movq	%r8, %rdx
	subq	%r14, %rdx
	movq	%rdx, %rsi
	negq	%rsi
	xorl	%ecx, %ecx
	cmpq	%r9, %rsi
	setb	%cl
	xorl	%esi, %esi
	cmpq	$38192, %rdx                    ## imm = 0x9530
	setb	%sil
	cmpq	%r14, %r8
	cmoval	%esi, %ecx
	jmp	LBB15_10
LBB15_4:
	xorl	%eax, %eax
	testq	%r9, %r9
	jne	LBB15_9
LBB15_8:
	xorl	%ecx, %ecx
LBB15_10:
	movl	32(%r14), %edx
	movl	%edx, %esi
	andl	$-2, %esi
	cmpl	$18, %esi
	setne	%sil
	orb	%cl, %al
	movl	$1, %ecx
	testb	%sil, %al
	jne	LBB15_13
## %bb.11:
	testl	%edx, %edx
	jne	LBB15_14
## %bb.12:
	movl	$2, %ecx
LBB15_13:
	movl	%ecx, 36(%r14)
	movl	$19, 32(%r14)
LBB15_14:
	leaq	-80(%rbp), %r15
	.p2align	4
LBB15_15:                               ## =>This Inner Loop Header: Depth=1
	movl	32(%r14), %eax
	movl	%eax, %ecx
	andl	$-2, %ecx
	cmpl	$18, %ecx
	je	LBB15_21
## %bb.16:                              ##   in Loop: Header=BB15_15 Depth=1
	movq	56(%r14), %rax
	cmpq	%rax, 24(%r14)
	je	LBB15_17
## %bb.19:                              ##   in Loop: Header=BB15_15 Depth=1
	incq	%rax
	movq	%rax, 56(%r14)
	movq	%r14, %rdi
	movq	%r15, %rsi
	callq	__ZN9gui_forms11compression13InflateStream4stepERNS1_4CallE
	testb	%al, %al
	jne	LBB15_15
## %bb.20:
	movl	32(%r14), %eax
LBB15_21:
	cmpl	$18, %eax
	je	LBB15_22
## %bb.23:
	cmpl	$19, %eax
	je	LBB15_18
## %bb.24:
	movl	-28(%rbp), %eax
	jmp	LBB15_25
LBB15_17:
	movabsq	$21474836499, %rax              ## imm = 0x500000013
	movq	%rax, 32(%r14)
LBB15_18:
	movl	$3, %eax
	jmp	LBB15_25
LBB15_22:
	movl	$2, %eax
LBB15_25:
	movl	%eax, (%rbx)
	movl	36(%r14), %eax
	movl	%eax, 4(%rbx)
	movups	-48(%rbp), %xmm0
	movups	%xmm0, 8(%rbx)
	movups	40(%r14), %xmm0
	movups	%xmm0, 24(%rbx)
	movq	56(%r14), %rax
	movq	%rax, 40(%rbx)
	movl	38188(%r14), %eax
	movl	%eax, 48(%rbx)
	movq	%rbx, %rax
	addq	$56, %rsp
	popq	%rbx
	popq	%r14
	popq	%r15
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__const
	.p2align	2, 0x0                          ## @_ZZN9gui_forms11compression13InflateStream11fixed_treesEvE12literal_tree
__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE12literal_tree:
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	24                              ## 0x18
	.short	152                             ## 0x98
	.short	112                             ## 0x70
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	256                             ## 0x100
	.short	257                             ## 0x101
	.short	258                             ## 0x102
	.short	259                             ## 0x103
	.short	260                             ## 0x104
	.short	261                             ## 0x105
	.short	262                             ## 0x106
	.short	263                             ## 0x107
	.short	264                             ## 0x108
	.short	265                             ## 0x109
	.short	266                             ## 0x10a
	.short	267                             ## 0x10b
	.short	268                             ## 0x10c
	.short	269                             ## 0x10d
	.short	270                             ## 0x10e
	.short	271                             ## 0x10f
	.short	272                             ## 0x110
	.short	273                             ## 0x111
	.short	274                             ## 0x112
	.short	275                             ## 0x113
	.short	276                             ## 0x114
	.short	277                             ## 0x115
	.short	278                             ## 0x116
	.short	279                             ## 0x117
	.short	0                               ## 0x0
	.short	1                               ## 0x1
	.short	2                               ## 0x2
	.short	3                               ## 0x3
	.short	4                               ## 0x4
	.short	5                               ## 0x5
	.short	6                               ## 0x6
	.short	7                               ## 0x7
	.short	8                               ## 0x8
	.short	9                               ## 0x9
	.short	10                              ## 0xa
	.short	11                              ## 0xb
	.short	12                              ## 0xc
	.short	13                              ## 0xd
	.short	14                              ## 0xe
	.short	15                              ## 0xf
	.short	16                              ## 0x10
	.short	17                              ## 0x11
	.short	18                              ## 0x12
	.short	19                              ## 0x13
	.short	20                              ## 0x14
	.short	21                              ## 0x15
	.short	22                              ## 0x16
	.short	23                              ## 0x17
	.short	24                              ## 0x18
	.short	25                              ## 0x19
	.short	26                              ## 0x1a
	.short	27                              ## 0x1b
	.short	28                              ## 0x1c
	.short	29                              ## 0x1d
	.short	30                              ## 0x1e
	.short	31                              ## 0x1f
	.short	32                              ## 0x20
	.short	33                              ## 0x21
	.short	34                              ## 0x22
	.short	35                              ## 0x23
	.short	36                              ## 0x24
	.short	37                              ## 0x25
	.short	38                              ## 0x26
	.short	39                              ## 0x27
	.short	40                              ## 0x28
	.short	41                              ## 0x29
	.short	42                              ## 0x2a
	.short	43                              ## 0x2b
	.short	44                              ## 0x2c
	.short	45                              ## 0x2d
	.short	46                              ## 0x2e
	.short	47                              ## 0x2f
	.short	48                              ## 0x30
	.short	49                              ## 0x31
	.short	50                              ## 0x32
	.short	51                              ## 0x33
	.short	52                              ## 0x34
	.short	53                              ## 0x35
	.short	54                              ## 0x36
	.short	55                              ## 0x37
	.short	56                              ## 0x38
	.short	57                              ## 0x39
	.short	58                              ## 0x3a
	.short	59                              ## 0x3b
	.short	60                              ## 0x3c
	.short	61                              ## 0x3d
	.short	62                              ## 0x3e
	.short	63                              ## 0x3f
	.short	64                              ## 0x40
	.short	65                              ## 0x41
	.short	66                              ## 0x42
	.short	67                              ## 0x43
	.short	68                              ## 0x44
	.short	69                              ## 0x45
	.short	70                              ## 0x46
	.short	71                              ## 0x47
	.short	72                              ## 0x48
	.short	73                              ## 0x49
	.short	74                              ## 0x4a
	.short	75                              ## 0x4b
	.short	76                              ## 0x4c
	.short	77                              ## 0x4d
	.short	78                              ## 0x4e
	.short	79                              ## 0x4f
	.short	80                              ## 0x50
	.short	81                              ## 0x51
	.short	82                              ## 0x52
	.short	83                              ## 0x53
	.short	84                              ## 0x54
	.short	85                              ## 0x55
	.short	86                              ## 0x56
	.short	87                              ## 0x57
	.short	88                              ## 0x58
	.short	89                              ## 0x59
	.short	90                              ## 0x5a
	.short	91                              ## 0x5b
	.short	92                              ## 0x5c
	.short	93                              ## 0x5d
	.short	94                              ## 0x5e
	.short	95                              ## 0x5f
	.short	96                              ## 0x60
	.short	97                              ## 0x61
	.short	98                              ## 0x62
	.short	99                              ## 0x63
	.short	100                             ## 0x64
	.short	101                             ## 0x65
	.short	102                             ## 0x66
	.short	103                             ## 0x67
	.short	104                             ## 0x68
	.short	105                             ## 0x69
	.short	106                             ## 0x6a
	.short	107                             ## 0x6b
	.short	108                             ## 0x6c
	.short	109                             ## 0x6d
	.short	110                             ## 0x6e
	.short	111                             ## 0x6f
	.short	112                             ## 0x70
	.short	113                             ## 0x71
	.short	114                             ## 0x72
	.short	115                             ## 0x73
	.short	116                             ## 0x74
	.short	117                             ## 0x75
	.short	118                             ## 0x76
	.short	119                             ## 0x77
	.short	120                             ## 0x78
	.short	121                             ## 0x79
	.short	122                             ## 0x7a
	.short	123                             ## 0x7b
	.short	124                             ## 0x7c
	.short	125                             ## 0x7d
	.short	126                             ## 0x7e
	.short	127                             ## 0x7f
	.short	128                             ## 0x80
	.short	129                             ## 0x81
	.short	130                             ## 0x82
	.short	131                             ## 0x83
	.short	132                             ## 0x84
	.short	133                             ## 0x85
	.short	134                             ## 0x86
	.short	135                             ## 0x87
	.short	136                             ## 0x88
	.short	137                             ## 0x89
	.short	138                             ## 0x8a
	.short	139                             ## 0x8b
	.short	140                             ## 0x8c
	.short	141                             ## 0x8d
	.short	142                             ## 0x8e
	.short	143                             ## 0x8f
	.short	280                             ## 0x118
	.short	281                             ## 0x119
	.short	282                             ## 0x11a
	.short	283                             ## 0x11b
	.short	284                             ## 0x11c
	.short	285                             ## 0x11d
	.short	286                             ## 0x11e
	.short	287                             ## 0x11f
	.short	144                             ## 0x90
	.short	145                             ## 0x91
	.short	146                             ## 0x92
	.short	147                             ## 0x93
	.short	148                             ## 0x94
	.short	149                             ## 0x95
	.short	150                             ## 0x96
	.short	151                             ## 0x97
	.short	152                             ## 0x98
	.short	153                             ## 0x99
	.short	154                             ## 0x9a
	.short	155                             ## 0x9b
	.short	156                             ## 0x9c
	.short	157                             ## 0x9d
	.short	158                             ## 0x9e
	.short	159                             ## 0x9f
	.short	160                             ## 0xa0
	.short	161                             ## 0xa1
	.short	162                             ## 0xa2
	.short	163                             ## 0xa3
	.short	164                             ## 0xa4
	.short	165                             ## 0xa5
	.short	166                             ## 0xa6
	.short	167                             ## 0xa7
	.short	168                             ## 0xa8
	.short	169                             ## 0xa9
	.short	170                             ## 0xaa
	.short	171                             ## 0xab
	.short	172                             ## 0xac
	.short	173                             ## 0xad
	.short	174                             ## 0xae
	.short	175                             ## 0xaf
	.short	176                             ## 0xb0
	.short	177                             ## 0xb1
	.short	178                             ## 0xb2
	.short	179                             ## 0xb3
	.short	180                             ## 0xb4
	.short	181                             ## 0xb5
	.short	182                             ## 0xb6
	.short	183                             ## 0xb7
	.short	184                             ## 0xb8
	.short	185                             ## 0xb9
	.short	186                             ## 0xba
	.short	187                             ## 0xbb
	.short	188                             ## 0xbc
	.short	189                             ## 0xbd
	.short	190                             ## 0xbe
	.short	191                             ## 0xbf
	.short	192                             ## 0xc0
	.short	193                             ## 0xc1
	.short	194                             ## 0xc2
	.short	195                             ## 0xc3
	.short	196                             ## 0xc4
	.short	197                             ## 0xc5
	.short	198                             ## 0xc6
	.short	199                             ## 0xc7
	.short	200                             ## 0xc8
	.short	201                             ## 0xc9
	.short	202                             ## 0xca
	.short	203                             ## 0xcb
	.short	204                             ## 0xcc
	.short	205                             ## 0xcd
	.short	206                             ## 0xce
	.short	207                             ## 0xcf
	.short	208                             ## 0xd0
	.short	209                             ## 0xd1
	.short	210                             ## 0xd2
	.short	211                             ## 0xd3
	.short	212                             ## 0xd4
	.short	213                             ## 0xd5
	.short	214                             ## 0xd6
	.short	215                             ## 0xd7
	.short	216                             ## 0xd8
	.short	217                             ## 0xd9
	.short	218                             ## 0xda
	.short	219                             ## 0xdb
	.short	220                             ## 0xdc
	.short	221                             ## 0xdd
	.short	222                             ## 0xde
	.short	223                             ## 0xdf
	.short	224                             ## 0xe0
	.short	225                             ## 0xe1
	.short	226                             ## 0xe2
	.short	227                             ## 0xe3
	.short	228                             ## 0xe4
	.short	229                             ## 0xe5
	.short	230                             ## 0xe6
	.short	231                             ## 0xe7
	.short	232                             ## 0xe8
	.short	233                             ## 0xe9
	.short	234                             ## 0xea
	.short	235                             ## 0xeb
	.short	236                             ## 0xec
	.short	237                             ## 0xed
	.short	238                             ## 0xee
	.short	239                             ## 0xef
	.short	240                             ## 0xf0
	.short	241                             ## 0xf1
	.short	242                             ## 0xf2
	.short	243                             ## 0xf3
	.short	244                             ## 0xf4
	.short	245                             ## 0xf5
	.short	246                             ## 0xf6
	.short	247                             ## 0xf7
	.short	248                             ## 0xf8
	.short	249                             ## 0xf9
	.short	250                             ## 0xfa
	.short	251                             ## 0xfb
	.short	252                             ## 0xfc
	.short	253                             ## 0xfd
	.short	254                             ## 0xfe
	.short	255                             ## 0xff
	.short	4103                            ## 0x1007
	.short	1288                            ## 0x508
	.short	264                             ## 0x108
	.short	4488                            ## 0x1188
	.short	4359                            ## 0x1107
	.short	1800                            ## 0x708
	.short	776                             ## 0x308
	.short	3081                            ## 0xc09
	.short	4231                            ## 0x1087
	.short	1544                            ## 0x608
	.short	520                             ## 0x208
	.short	2569                            ## 0xa09
	.short	8                               ## 0x8
	.short	2056                            ## 0x808
	.short	1032                            ## 0x408
	.short	3593                            ## 0xe09
	.short	4167                            ## 0x1047
	.short	1416                            ## 0x588
	.short	392                             ## 0x188
	.short	2313                            ## 0x909
	.short	4423                            ## 0x1147
	.short	1928                            ## 0x788
	.short	904                             ## 0x388
	.short	3337                            ## 0xd09
	.short	4295                            ## 0x10c7
	.short	1672                            ## 0x688
	.short	648                             ## 0x288
	.short	2825                            ## 0xb09
	.short	136                             ## 0x88
	.short	2184                            ## 0x888
	.short	1160                            ## 0x488
	.short	3849                            ## 0xf09
	.short	4135                            ## 0x1027
	.short	1352                            ## 0x548
	.short	328                             ## 0x148
	.short	4552                            ## 0x11c8
	.short	4391                            ## 0x1127
	.short	1864                            ## 0x748
	.short	840                             ## 0x348
	.short	3209                            ## 0xc89
	.short	4263                            ## 0x10a7
	.short	1608                            ## 0x648
	.short	584                             ## 0x248
	.short	2697                            ## 0xa89
	.short	72                              ## 0x48
	.short	2120                            ## 0x848
	.short	1096                            ## 0x448
	.short	3721                            ## 0xe89
	.short	4199                            ## 0x1067
	.short	1480                            ## 0x5c8
	.short	456                             ## 0x1c8
	.short	2441                            ## 0x989
	.short	4455                            ## 0x1167
	.short	1992                            ## 0x7c8
	.short	968                             ## 0x3c8
	.short	3465                            ## 0xd89
	.short	4327                            ## 0x10e7
	.short	1736                            ## 0x6c8
	.short	712                             ## 0x2c8
	.short	2953                            ## 0xb89
	.short	200                             ## 0xc8
	.short	2248                            ## 0x8c8
	.short	1224                            ## 0x4c8
	.short	3977                            ## 0xf89
	.short	4119                            ## 0x1017
	.short	1320                            ## 0x528
	.short	296                             ## 0x128
	.short	4520                            ## 0x11a8
	.short	4375                            ## 0x1117
	.short	1832                            ## 0x728
	.short	808                             ## 0x328
	.short	3145                            ## 0xc49
	.short	4247                            ## 0x1097
	.short	1576                            ## 0x628
	.short	552                             ## 0x228
	.short	2633                            ## 0xa49
	.short	40                              ## 0x28
	.short	2088                            ## 0x828
	.short	1064                            ## 0x428
	.short	3657                            ## 0xe49
	.short	4183                            ## 0x1057
	.short	1448                            ## 0x5a8
	.short	424                             ## 0x1a8
	.short	2377                            ## 0x949
	.short	4439                            ## 0x1157
	.short	1960                            ## 0x7a8
	.short	936                             ## 0x3a8
	.short	3401                            ## 0xd49
	.short	4311                            ## 0x10d7
	.short	1704                            ## 0x6a8
	.short	680                             ## 0x2a8
	.short	2889                            ## 0xb49
	.short	168                             ## 0xa8
	.short	2216                            ## 0x8a8
	.short	1192                            ## 0x4a8
	.short	3913                            ## 0xf49
	.short	4151                            ## 0x1037
	.short	1384                            ## 0x568
	.short	360                             ## 0x168
	.short	4584                            ## 0x11e8
	.short	4407                            ## 0x1137
	.short	1896                            ## 0x768
	.short	872                             ## 0x368
	.short	3273                            ## 0xcc9
	.short	4279                            ## 0x10b7
	.short	1640                            ## 0x668
	.short	616                             ## 0x268
	.short	2761                            ## 0xac9
	.short	104                             ## 0x68
	.short	2152                            ## 0x868
	.short	1128                            ## 0x468
	.short	3785                            ## 0xec9
	.short	4215                            ## 0x1077
	.short	1512                            ## 0x5e8
	.short	488                             ## 0x1e8
	.short	2505                            ## 0x9c9
	.short	4471                            ## 0x1177
	.short	2024                            ## 0x7e8
	.short	1000                            ## 0x3e8
	.short	3529                            ## 0xdc9
	.short	4343                            ## 0x10f7
	.short	1768                            ## 0x6e8
	.short	744                             ## 0x2e8
	.short	3017                            ## 0xbc9
	.short	232                             ## 0xe8
	.short	2280                            ## 0x8e8
	.short	1256                            ## 0x4e8
	.short	4041                            ## 0xfc9
	.short	4103                            ## 0x1007
	.short	1304                            ## 0x518
	.short	280                             ## 0x118
	.short	4504                            ## 0x1198
	.short	4359                            ## 0x1107
	.short	1816                            ## 0x718
	.short	792                             ## 0x318
	.short	3113                            ## 0xc29
	.short	4231                            ## 0x1087
	.short	1560                            ## 0x618
	.short	536                             ## 0x218
	.short	2601                            ## 0xa29
	.short	24                              ## 0x18
	.short	2072                            ## 0x818
	.short	1048                            ## 0x418
	.short	3625                            ## 0xe29
	.short	4167                            ## 0x1047
	.short	1432                            ## 0x598
	.short	408                             ## 0x198
	.short	2345                            ## 0x929
	.short	4423                            ## 0x1147
	.short	1944                            ## 0x798
	.short	920                             ## 0x398
	.short	3369                            ## 0xd29
	.short	4295                            ## 0x10c7
	.short	1688                            ## 0x698
	.short	664                             ## 0x298
	.short	2857                            ## 0xb29
	.short	152                             ## 0x98
	.short	2200                            ## 0x898
	.short	1176                            ## 0x498
	.short	3881                            ## 0xf29
	.short	4135                            ## 0x1027
	.short	1368                            ## 0x558
	.short	344                             ## 0x158
	.short	4568                            ## 0x11d8
	.short	4391                            ## 0x1127
	.short	1880                            ## 0x758
	.short	856                             ## 0x358
	.short	3241                            ## 0xca9
	.short	4263                            ## 0x10a7
	.short	1624                            ## 0x658
	.short	600                             ## 0x258
	.short	2729                            ## 0xaa9
	.short	88                              ## 0x58
	.short	2136                            ## 0x858
	.short	1112                            ## 0x458
	.short	3753                            ## 0xea9
	.short	4199                            ## 0x1067
	.short	1496                            ## 0x5d8
	.short	472                             ## 0x1d8
	.short	2473                            ## 0x9a9
	.short	4455                            ## 0x1167
	.short	2008                            ## 0x7d8
	.short	984                             ## 0x3d8
	.short	3497                            ## 0xda9
	.short	4327                            ## 0x10e7
	.short	1752                            ## 0x6d8
	.short	728                             ## 0x2d8
	.short	2985                            ## 0xba9
	.short	216                             ## 0xd8
	.short	2264                            ## 0x8d8
	.short	1240                            ## 0x4d8
	.short	4009                            ## 0xfa9
	.short	4119                            ## 0x1017
	.short	1336                            ## 0x538
	.short	312                             ## 0x138
	.short	4536                            ## 0x11b8
	.short	4375                            ## 0x1117
	.short	1848                            ## 0x738
	.short	824                             ## 0x338
	.short	3177                            ## 0xc69
	.short	4247                            ## 0x1097
	.short	1592                            ## 0x638
	.short	568                             ## 0x238
	.short	2665                            ## 0xa69
	.short	56                              ## 0x38
	.short	2104                            ## 0x838
	.short	1080                            ## 0x438
	.short	3689                            ## 0xe69
	.short	4183                            ## 0x1057
	.short	1464                            ## 0x5b8
	.short	440                             ## 0x1b8
	.short	2409                            ## 0x969
	.short	4439                            ## 0x1157
	.short	1976                            ## 0x7b8
	.short	952                             ## 0x3b8
	.short	3433                            ## 0xd69
	.short	4311                            ## 0x10d7
	.short	1720                            ## 0x6b8
	.short	696                             ## 0x2b8
	.short	2921                            ## 0xb69
	.short	184                             ## 0xb8
	.short	2232                            ## 0x8b8
	.short	1208                            ## 0x4b8
	.short	3945                            ## 0xf69
	.short	4151                            ## 0x1037
	.short	1400                            ## 0x578
	.short	376                             ## 0x178
	.short	4600                            ## 0x11f8
	.short	4407                            ## 0x1137
	.short	1912                            ## 0x778
	.short	888                             ## 0x378
	.short	3305                            ## 0xce9
	.short	4279                            ## 0x10b7
	.short	1656                            ## 0x678
	.short	632                             ## 0x278
	.short	2793                            ## 0xae9
	.short	120                             ## 0x78
	.short	2168                            ## 0x878
	.short	1144                            ## 0x478
	.short	3817                            ## 0xee9
	.short	4215                            ## 0x1077
	.short	1528                            ## 0x5f8
	.short	504                             ## 0x1f8
	.short	2537                            ## 0x9e9
	.short	4471                            ## 0x1177
	.short	2040                            ## 0x7f8
	.short	1016                            ## 0x3f8
	.short	3561                            ## 0xde9
	.short	4343                            ## 0x10f7
	.short	1784                            ## 0x6f8
	.short	760                             ## 0x2f8
	.short	3049                            ## 0xbe9
	.short	248                             ## 0xf8
	.short	2296                            ## 0x8f8
	.short	1272                            ## 0x4f8
	.short	4073                            ## 0xfe9
	.short	4103                            ## 0x1007
	.short	1288                            ## 0x508
	.short	264                             ## 0x108
	.short	4488                            ## 0x1188
	.short	4359                            ## 0x1107
	.short	1800                            ## 0x708
	.short	776                             ## 0x308
	.short	3097                            ## 0xc19
	.short	4231                            ## 0x1087
	.short	1544                            ## 0x608
	.short	520                             ## 0x208
	.short	2585                            ## 0xa19
	.short	8                               ## 0x8
	.short	2056                            ## 0x808
	.short	1032                            ## 0x408
	.short	3609                            ## 0xe19
	.short	4167                            ## 0x1047
	.short	1416                            ## 0x588
	.short	392                             ## 0x188
	.short	2329                            ## 0x919
	.short	4423                            ## 0x1147
	.short	1928                            ## 0x788
	.short	904                             ## 0x388
	.short	3353                            ## 0xd19
	.short	4295                            ## 0x10c7
	.short	1672                            ## 0x688
	.short	648                             ## 0x288
	.short	2841                            ## 0xb19
	.short	136                             ## 0x88
	.short	2184                            ## 0x888
	.short	1160                            ## 0x488
	.short	3865                            ## 0xf19
	.short	4135                            ## 0x1027
	.short	1352                            ## 0x548
	.short	328                             ## 0x148
	.short	4552                            ## 0x11c8
	.short	4391                            ## 0x1127
	.short	1864                            ## 0x748
	.short	840                             ## 0x348
	.short	3225                            ## 0xc99
	.short	4263                            ## 0x10a7
	.short	1608                            ## 0x648
	.short	584                             ## 0x248
	.short	2713                            ## 0xa99
	.short	72                              ## 0x48
	.short	2120                            ## 0x848
	.short	1096                            ## 0x448
	.short	3737                            ## 0xe99
	.short	4199                            ## 0x1067
	.short	1480                            ## 0x5c8
	.short	456                             ## 0x1c8
	.short	2457                            ## 0x999
	.short	4455                            ## 0x1167
	.short	1992                            ## 0x7c8
	.short	968                             ## 0x3c8
	.short	3481                            ## 0xd99
	.short	4327                            ## 0x10e7
	.short	1736                            ## 0x6c8
	.short	712                             ## 0x2c8
	.short	2969                            ## 0xb99
	.short	200                             ## 0xc8
	.short	2248                            ## 0x8c8
	.short	1224                            ## 0x4c8
	.short	3993                            ## 0xf99
	.short	4119                            ## 0x1017
	.short	1320                            ## 0x528
	.short	296                             ## 0x128
	.short	4520                            ## 0x11a8
	.short	4375                            ## 0x1117
	.short	1832                            ## 0x728
	.short	808                             ## 0x328
	.short	3161                            ## 0xc59
	.short	4247                            ## 0x1097
	.short	1576                            ## 0x628
	.short	552                             ## 0x228
	.short	2649                            ## 0xa59
	.short	40                              ## 0x28
	.short	2088                            ## 0x828
	.short	1064                            ## 0x428
	.short	3673                            ## 0xe59
	.short	4183                            ## 0x1057
	.short	1448                            ## 0x5a8
	.short	424                             ## 0x1a8
	.short	2393                            ## 0x959
	.short	4439                            ## 0x1157
	.short	1960                            ## 0x7a8
	.short	936                             ## 0x3a8
	.short	3417                            ## 0xd59
	.short	4311                            ## 0x10d7
	.short	1704                            ## 0x6a8
	.short	680                             ## 0x2a8
	.short	2905                            ## 0xb59
	.short	168                             ## 0xa8
	.short	2216                            ## 0x8a8
	.short	1192                            ## 0x4a8
	.short	3929                            ## 0xf59
	.short	4151                            ## 0x1037
	.short	1384                            ## 0x568
	.short	360                             ## 0x168
	.short	4584                            ## 0x11e8
	.short	4407                            ## 0x1137
	.short	1896                            ## 0x768
	.short	872                             ## 0x368
	.short	3289                            ## 0xcd9
	.short	4279                            ## 0x10b7
	.short	1640                            ## 0x668
	.short	616                             ## 0x268
	.short	2777                            ## 0xad9
	.short	104                             ## 0x68
	.short	2152                            ## 0x868
	.short	1128                            ## 0x468
	.short	3801                            ## 0xed9
	.short	4215                            ## 0x1077
	.short	1512                            ## 0x5e8
	.short	488                             ## 0x1e8
	.short	2521                            ## 0x9d9
	.short	4471                            ## 0x1177
	.short	2024                            ## 0x7e8
	.short	1000                            ## 0x3e8
	.short	3545                            ## 0xdd9
	.short	4343                            ## 0x10f7
	.short	1768                            ## 0x6e8
	.short	744                             ## 0x2e8
	.short	3033                            ## 0xbd9
	.short	232                             ## 0xe8
	.short	2280                            ## 0x8e8
	.short	1256                            ## 0x4e8
	.short	4057                            ## 0xfd9
	.short	4103                            ## 0x1007
	.short	1304                            ## 0x518
	.short	280                             ## 0x118
	.short	4504                            ## 0x1198
	.short	4359                            ## 0x1107
	.short	1816                            ## 0x718
	.short	792                             ## 0x318
	.short	3129                            ## 0xc39
	.short	4231                            ## 0x1087
	.short	1560                            ## 0x618
	.short	536                             ## 0x218
	.short	2617                            ## 0xa39
	.short	24                              ## 0x18
	.short	2072                            ## 0x818
	.short	1048                            ## 0x418
	.short	3641                            ## 0xe39
	.short	4167                            ## 0x1047
	.short	1432                            ## 0x598
	.short	408                             ## 0x198
	.short	2361                            ## 0x939
	.short	4423                            ## 0x1147
	.short	1944                            ## 0x798
	.short	920                             ## 0x398
	.short	3385                            ## 0xd39
	.short	4295                            ## 0x10c7
	.short	1688                            ## 0x698
	.short	664                             ## 0x298
	.short	2873                            ## 0xb39
	.short	152                             ## 0x98
	.short	2200                            ## 0x898
	.short	1176                            ## 0x498
	.short	3897                            ## 0xf39
	.short	4135                            ## 0x1027
	.short	1368                            ## 0x558
	.short	344                             ## 0x158
	.short	4568                            ## 0x11d8
	.short	4391                            ## 0x1127
	.short	1880                            ## 0x758
	.short	856                             ## 0x358
	.short	3257                            ## 0xcb9
	.short	4263                            ## 0x10a7
	.short	1624                            ## 0x658
	.short	600                             ## 0x258
	.short	2745                            ## 0xab9
	.short	88                              ## 0x58
	.short	2136                            ## 0x858
	.short	1112                            ## 0x458
	.short	3769                            ## 0xeb9
	.short	4199                            ## 0x1067
	.short	1496                            ## 0x5d8
	.short	472                             ## 0x1d8
	.short	2489                            ## 0x9b9
	.short	4455                            ## 0x1167
	.short	2008                            ## 0x7d8
	.short	984                             ## 0x3d8
	.short	3513                            ## 0xdb9
	.short	4327                            ## 0x10e7
	.short	1752                            ## 0x6d8
	.short	728                             ## 0x2d8
	.short	3001                            ## 0xbb9
	.short	216                             ## 0xd8
	.short	2264                            ## 0x8d8
	.short	1240                            ## 0x4d8
	.short	4025                            ## 0xfb9
	.short	4119                            ## 0x1017
	.short	1336                            ## 0x538
	.short	312                             ## 0x138
	.short	4536                            ## 0x11b8
	.short	4375                            ## 0x1117
	.short	1848                            ## 0x738
	.short	824                             ## 0x338
	.short	3193                            ## 0xc79
	.short	4247                            ## 0x1097
	.short	1592                            ## 0x638
	.short	568                             ## 0x238
	.short	2681                            ## 0xa79
	.short	56                              ## 0x38
	.short	2104                            ## 0x838
	.short	1080                            ## 0x438
	.short	3705                            ## 0xe79
	.short	4183                            ## 0x1057
	.short	1464                            ## 0x5b8
	.short	440                             ## 0x1b8
	.short	2425                            ## 0x979
	.short	4439                            ## 0x1157
	.short	1976                            ## 0x7b8
	.short	952                             ## 0x3b8
	.short	3449                            ## 0xd79
	.short	4311                            ## 0x10d7
	.short	1720                            ## 0x6b8
	.short	696                             ## 0x2b8
	.short	2937                            ## 0xb79
	.short	184                             ## 0xb8
	.short	2232                            ## 0x8b8
	.short	1208                            ## 0x4b8
	.short	3961                            ## 0xf79
	.short	4151                            ## 0x1037
	.short	1400                            ## 0x578
	.short	376                             ## 0x178
	.short	4600                            ## 0x11f8
	.short	4407                            ## 0x1137
	.short	1912                            ## 0x778
	.short	888                             ## 0x378
	.short	3321                            ## 0xcf9
	.short	4279                            ## 0x10b7
	.short	1656                            ## 0x678
	.short	632                             ## 0x278
	.short	2809                            ## 0xaf9
	.short	120                             ## 0x78
	.short	2168                            ## 0x878
	.short	1144                            ## 0x478
	.short	3833                            ## 0xef9
	.short	4215                            ## 0x1077
	.short	1528                            ## 0x5f8
	.short	504                             ## 0x1f8
	.short	2553                            ## 0x9f9
	.short	4471                            ## 0x1177
	.short	2040                            ## 0x7f8
	.short	1016                            ## 0x3f8
	.short	3577                            ## 0xdf9
	.short	4343                            ## 0x10f7
	.short	1784                            ## 0x6f8
	.short	760                             ## 0x2f8
	.short	3065                            ## 0xbf9
	.short	248                             ## 0xf8
	.short	2296                            ## 0x8f8
	.short	1272                            ## 0x4f8
	.short	4089                            ## 0xff9
	.long	9                               ## 0x9

	.p2align	2, 0x0                          ## @_ZZN9gui_forms11compression13InflateStream11fixed_treesEvE13distance_tree
__ZZN9gui_forms11compression13InflateStream11fixed_treesEvE13distance_tree:
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	0                               ## 0x0
	.short	32                              ## 0x20
	.space	20
	.short	0                               ## 0x0
	.short	1                               ## 0x1
	.short	2                               ## 0x2
	.short	3                               ## 0x3
	.short	4                               ## 0x4
	.short	5                               ## 0x5
	.short	6                               ## 0x6
	.short	7                               ## 0x7
	.short	8                               ## 0x8
	.short	9                               ## 0x9
	.short	10                              ## 0xa
	.short	11                              ## 0xb
	.short	12                              ## 0xc
	.short	13                              ## 0xd
	.short	14                              ## 0xe
	.short	15                              ## 0xf
	.short	16                              ## 0x10
	.short	17                              ## 0x11
	.short	18                              ## 0x12
	.short	19                              ## 0x13
	.short	20                              ## 0x14
	.short	21                              ## 0x15
	.short	22                              ## 0x16
	.short	23                              ## 0x17
	.short	24                              ## 0x18
	.short	25                              ## 0x19
	.short	26                              ## 0x1a
	.short	27                              ## 0x1b
	.short	28                              ## 0x1c
	.short	29                              ## 0x1d
	.short	30                              ## 0x1e
	.short	31                              ## 0x1f
	.space	512
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.short	5                               ## 0x5
	.short	261                             ## 0x105
	.short	133                             ## 0x85
	.short	389                             ## 0x185
	.short	69                              ## 0x45
	.short	325                             ## 0x145
	.short	197                             ## 0xc5
	.short	453                             ## 0x1c5
	.short	37                              ## 0x25
	.short	293                             ## 0x125
	.short	165                             ## 0xa5
	.short	421                             ## 0x1a5
	.short	101                             ## 0x65
	.short	357                             ## 0x165
	.short	229                             ## 0xe5
	.short	485                             ## 0x1e5
	.short	21                              ## 0x15
	.short	277                             ## 0x115
	.short	149                             ## 0x95
	.short	405                             ## 0x195
	.short	85                              ## 0x55
	.short	341                             ## 0x155
	.short	213                             ## 0xd5
	.short	469                             ## 0x1d5
	.short	53                              ## 0x35
	.short	309                             ## 0x135
	.short	181                             ## 0xb5
	.short	437                             ## 0x1b5
	.short	117                             ## 0x75
	.short	373                             ## 0x175
	.short	245                             ## 0xf5
	.short	501                             ## 0x1f5
	.long	5                               ## 0x5

	.p2align	2, 0x0                          ## @_ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE
__ZN9gui_forms11compression12_GLOBAL__N_114alphabet_orderE:
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

	.p2align	2, 0x0                          ## @_ZN9gui_forms11compression12_GLOBAL__N_111length_baseE
__ZN9gui_forms11compression12_GLOBAL__N_111length_baseE:
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

	.p2align	2, 0x0                          ## @_ZN9gui_forms11compression12_GLOBAL__N_112length_extraE
__ZN9gui_forms11compression12_GLOBAL__N_112length_extraE:
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

	.p2align	2, 0x0                          ## @_ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE
__ZN9gui_forms11compression12_GLOBAL__N_113distance_baseE:
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

	.p2align	2, 0x0                          ## @_ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE
__ZN9gui_forms11compression12_GLOBAL__N_114distance_extraE:
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	0                               ## 0x0
	.long	1                               ## 0x1
	.long	1                               ## 0x1
	.long	2                               ## 0x2
	.long	2                               ## 0x2
	.long	3                               ## 0x3
	.long	3                               ## 0x3
	.long	4                               ## 0x4
	.long	4                               ## 0x4
	.long	5                               ## 0x5
	.long	5                               ## 0x5
	.long	6                               ## 0x6
	.long	6                               ## 0x6
	.long	7                               ## 0x7
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
	.long	13                              ## 0xd
	.long	13                              ## 0xd

.subsections_via_symbols
