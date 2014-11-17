.text
.global find_str_range

find_str_range:
    vmovdqu (%rdx), %xmm0
    mov    %rcx, %rax
    mov    %rsi, %rdx
    sub    %rdi, %rdx
    jmp    .lp_1
.lp_0:
    add    $16, %rdi
    sub    $16, %rdx
.lp_1:
#ifdef __AVX2__
    vpcmpestri $0x14,(%rdi), %xmm0
#else
    pcmpestri $0x14,(%rdi), %xmm0
#endif
    ja     .lp_0
    sbb    %rax, %rax
    and    %rcx, %rax
    lea    (%rdi, %rcx, 1), %rax
    ret

