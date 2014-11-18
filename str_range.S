#ifdef __AVX__
#define _movdqu vmovdqu
#define _movdqa vmovdqa
#define _pcmpestri vpcmpestri
#else
#define _movdqu movdqu
#define _movdqa movdqa
#define _pcmpestri pcmpestri
#endif

.section .rodata
.align 16
.shiftPtn:
.byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
.byte 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80

.text
#ifdef ADD_UNDERSCORE
.global _find_str_range
_find_str_range:
#else
.global find_str_range
find_str_range:
#endif

    _movdqu (%rdx), %xmm0
    mov    %rcx, %rax
    mov    %rsi, %rdx
    sub    %rdi, %rdx
.lp:
    mov    %rdi, %rsi
    and    $0xfff, %rsi
    cmp    $0xff0, %rsi
    jbe    .in
    lea    (%rsi, %rdx, 1), %r8
    cmp    $0xfff, %r8
    jbe    .short
.in:
    _pcmpestri $0x14,(%rdi), %xmm0
    lea     16(%rdi), %rdi
    lea     -16(%rdx), %rdx
    ja     .lp
.exit:
    sbb    %rax, %rax
    and    %rcx, %rax
    lea    -16(%rdi, %rcx, 1), %rax
    ret

.short:
    mov   %rdi, %rsi
    and   $-16, %rsi
    _movdqa (%rsi), %xmm1
    mov   %rdi, %r8
    and   $15, %r8
    _movdqu .shiftPtn(%r8, 1), %xmm2
    lea     16(%rdi), %rdi
#ifdef __AVX__
    vpshufb %xmm2, %xmm1, %xmm1
#else
    pshufb %xmm2, %xmm1
#endif
   _pcmpestri $0x14, %xmm1, %xmm0
    jmp   .exit
