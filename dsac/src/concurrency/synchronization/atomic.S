#if(APPLE)
  #define FUNCTION_NAME(name) _##name
#else
  #define FUNCTION_NAME(name) name
#endif

.globl FUNCTION_NAME(atomic_store_seq_cst)

.globl FUNCTION_NAME(atomic_load_seq_cst)

.globl FUNCTION_NAME(atomic_exchange_seq_cst)

# Load

FUNCTION_NAME(atomic_load_seq_cst):
    movq (%rdi), %rax
    retq

# Store

FUNCTION_NAME(atomic_store_seq_cst):
    xchg %rsi, (%rdi)
    retq

# Exchange

FUNCTION_NAME(atomic_exchange_seq_cst):
    mov  %rsi, %rax
    xchg %rax, (%rdi)
    retq
