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

	mov rbp, rsp ; Used to save the value of rsp because of alignment issues
	and rsp, -16

	call printf

	mov rsp, rbp ; Fix rsp value

	mov rdi, [rel stdout]
	call fflush

	ret
print_i64_string:
	db `%lld\n`,0


	global i64_to_bool
i64_to_bool:
	mov rax, [rsp+8]
	cmp rax, 0
	je i64_to_bool_true
	jmp i64_to_bool_false

i64_to_bool_false:
	mov al, 0
	mov [rsp+16], al
	ret
i64_to_bool_true:
	mov al, 1
	mov [rsp+16], al
	ret

	global is_zero
is_zero:
	mov rax, [rsp+8]
	cmp rax, 0
	je is_zero_false
	jmp is_zero_true

is_zero_false:
	mov al, 0
	mov [rsp+16], al
	ret
is_zero_true:
	mov al, 1
	mov [rsp+16], al
	ret
