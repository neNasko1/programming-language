	section .text
	extern printf
	extern exit
	extern stdout
	extern fflush

	global print
print:
	mov rbp, rsp ; Used to save the value of rsp because of alignment issues
	mov rdi, print_i64_string
	mov rsi, [rsp+8]
	mov al, 0
	and rsp, -16
	call printf
	mov rsp, rbp ; Fix rsp value

	mov rdi, [rel stdout]
	call fflush

	ret

print_i64_string:
	db `%lld\n`,0
