#ifndef INLINE_SYSCALL_H_
#define INLINE_SYSCALL_H_

#include <unistd.h>      //for intptr_t
#include <sys/syscall.h> //for SYS_[name] definitions

//This pre-processor block calls a different function
//depending on the number of arguments to our syscall().
#define PP_HAS_ARGS_IMPL2(_1, _2, _3, _4, _5, _6, _7 ,N, ...) N
#define PP_HAS_ARGS_SOURCE() 6, 5, 4, 3, 2, 1, 0, _ERROR
#define PP_HAS_ARGS_IMPL(...) PP_HAS_ARGS_IMPL2(__VA_ARGS__)
#define PP_HAS_ARGS(...) PP_HAS_ARGS_IMPL(__VA_ARGS__, PP_HAS_ARGS_SOURCE())
#define _syscall_ERROR(x) _Static_assert(0); //failure
#define SYSCALL_DISAMBIGUATE2(has_args, ...) _syscall ## has_args(__VA_ARGS__)
#define SYSCALL_DISAMBIGUATE(has_args, ...) SYSCALL_DISAMBIGUATE2(has_args, __VA_ARGS__)
#define syscall(...) SYSCALL_DISAMBIGUATE(PP_HAS_ARGS(__VA_ARGS__), __VA_ARGS__)

static inline intptr_t _syscall0(int syscall){
	intptr_t ret;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall) // rax
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall1(int syscall, uintptr_t arg0){
	intptr_t ret;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0)    // rdi
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall2(int syscall, uintptr_t arg0, uintptr_t arg1){
	intptr_t ret;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0),   // rdi
	               "S"(arg1)    // rsi
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall3(int syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2){
	intptr_t ret;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0),   // rdi
	               "S"(arg1),   // rsi
	               "d"(arg2)    // rdx
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall4(int syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3){
	intptr_t ret;
	register unsigned long r10 __asm("r10") = arg3;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0),   // rdi
	               "S"(arg1),   // rsi
	               "d"(arg2),   // rdx
	               "r" (r10)
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall5(int syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4){
	intptr_t ret;
	register unsigned long r10 __asm("r10") = arg3;
	register unsigned long r8  __asm("r8")  = arg4;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0),   // rdi
	               "S"(arg1),   // rsi
	               "d"(arg2),   // rdx
	               "r" (r10),
	               "r" (r8)
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

static inline intptr_t _syscall6(int syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4, uintptr_t arg5){
	intptr_t ret;
	register unsigned long r10 __asm("r10") = arg3;
	register unsigned long r8  __asm("r8")  = arg4;
	register unsigned long r9  __asm("r9")  = arg5;
	__asm volatile("syscall"
	              :"=a" (ret)   // rax
	              :"a"(syscall),// rax
	               "D"(arg0),   // rdi
	               "S"(arg1),   // rsi
	               "d"(arg2),   // rdx
	               "r" (r10),
	               "r" (r8),
	               "r" (r9)
	              :"rcx",
	               "r11",
	               "memory"
	);
	return ret;
}

#endif //INLINE_SYSCALL_H_
