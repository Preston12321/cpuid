	.globl CPUIDInstructionSupported
	.globl getCPUIDData

	.text

# TODO: Make this function work
# bool CPUIDInstructionSupported()
CPUIDInstructionSupported:
	pushfq						# Attempt to set ID Flag
	orq $0x200000, (%rsp)		# (bit 21 in eflags reg.)
	popfq
	pushfq						# Get new eflags contents
	movq (%rsp), %rcx
	popfq
	testq $0x200000, %rcx		# Check if write worked
	jz unsupported				# If so, return true
	movb $1, %al
	ret
unsupported:
	movb $0, %al				# Else, return false
	ret

# void getCPUIDData(uint32_t, CPUID_DATA *)
getCPUIDData:
	push %rbx
	movl %edi, %eax				# Pass arg to cpuid
	cpuid
	movl %eax, (%rsi)			# Copy results into struct
	movl %ebx, 4(%rsi)
	movl %ecx, 8(%rsi)
	movl %edx, 12(%rsi)
	pop %rbx
	ret
