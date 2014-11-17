.text
.global find_str_range

find_str_range:
.ifdef __AVX2__
    vmovdqu (%rdx), %xmm0
.else
    movdqu (%rdx), %xmm0
.endif
    mov    %rcx, %rax
    mov    %rsi, %rdx
    sub    %rdi, %rdx
    sub    $16, %rdi
.lp:
.ifdef __AVX2__
    vpcmpestri $0x14,16(%rdi), %xmm0
.else
    pcmpestri $0x14,16(%rdi), %xmm0
.endif
    lea     16(%rdi), %rdi
    lea     -16(%rdx), %rdx
    ja     .lp
    sbb    %rax, %rax
    and    %rcx, %rax
    lea    (%rdi, %rcx, 1), %rax
    ret

