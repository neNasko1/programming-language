	section .text
	extern printf
	extern exit
	extern stdout
	extern fflush

	global print
print:
	mov rdi, print_i64_string
	mov rsi, [rsp+8]
	mov al, 0

 	call printf

	mov rdi, [rel stdout]
	call fflush

	ret

print_i64_string:
	db `%lld\n`,0
