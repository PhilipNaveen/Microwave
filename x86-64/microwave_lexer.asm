section .data
    input_filename db 'input.mw',0
    output_filename db 'output.cow',0
    buffer db 1
    buffer_size equ $ - buffer

section .text
    global main

main:
    ; Open input file
    mov eax, 5         ; sys_open syscall number
    mov ebx, input_filename
    mov ecx, 0         ; O_RDONLY mode
    int 0x80
    mov esi, eax       ; Store file descriptor in esi

    ; Open output file
    mov eax, 5         ; sys_open syscall number
    mov ebx, output_filename
    mov ecx, 577       ; O_WRONLY | O_CREAT | O_TRUNC mode
    mov edx, 0644      ; Permissions
    int 0x80
    mov edi, eax       ; Store file descriptor in edi

read_loop:
    ; Read character from input file
    mov eax, 3         ; sys_read syscall number
    mov ebx, esi       ; File descriptor
    mov ecx, buffer
    mov edx, buffer_size
    int 0x80
    cmp eax, 0         ; Check if end of file
    je end_program

    ; Process character (generate corresponding COW code)
    cmp byte [buffer], 'm'
    je process_m
    cmp byte [buffer], 'M'
    je process_M
    cmp byte [buffer], 'b'
    je process_b
    cmp byte [buffer], 'B'
    je process_B
    jmp read_loop

process_m:
    ; Write "moo" to output file
    mov eax, 4         ; sys_write syscall number
    mov ebx, edi       ; File descriptor
    mov ecx, buffer
    mov edx, buffer_size
    int 0x80
    jmp read_loop

; Implement process_M, process_b, process_B similarly

end_program:
    ; Close input file
    mov eax, 6         ; sys_close syscall number
    mov ebx, esi       ; File descriptor
    int 0x80

    ; Close output file
    mov eax, 6         ; sys_close syscall number
    mov ebx, edi       ; File descriptor
    int 0x80

    ; Exit program
    mov eax, 1         ; sys_exit syscall number
    xor ebx, ebx       ; Exit code 0
    int 0x80
