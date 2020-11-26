asm(
  ".text\n"
  ".globl ut_switch\n"
  "ut_switch:\n"

  "movq 0(%rsp), %rdx\n"
  "leaq 8(%rsp), %rcx\n"

  "movq %r12,  0(%rdi)\n"
  "movq %r13,  8(%rdi)\n"
  "movq %r14, 16(%rdi)\n"
  "movq %r15, 24(%rdi)\n"

  "movq  0(%rsi), %r12\n"
  "movq  8(%rsi), %r13\n"
  "movq 16(%rsi), %r14\n"
  "movq 24(%rsi), %r15\n"

  "movq %rbx, 32(%rdi)\n"
  "movq %rbp, 40(%rdi)\n"
  "movq %rdx, 48(%rdi)\n"
  "movq %rcx, 56(%rdi)\n"

  "movq 32(%rsi), %rbx\n"
  "movq 40(%rsi), %rbp\n"
  "movq 48(%rsi), %rdx\n"
  "movq 56(%rsi), %rcx\n"

  "movq %rcx, %rsp\n"
  "jmp *%rdx\n"
);