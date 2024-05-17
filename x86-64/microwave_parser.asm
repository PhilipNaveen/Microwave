section .data
    input_filename db 'input.mw',0
    buffer db 1
    buffer_size equ $ - buffer
    MEMORY_SIZE equ 30000
    memory rb MEMORY_SIZE
    pointer dd 0
    reg dd 0
    has_register dd 0

section .text
    global main

main:
    ; Open input file
    mov eax, 5         ; sys_open syscall number
    mov ebx, input_filename
    mov ecx, 0         ; O_RDONLY mode
    int 0x80
    mov esi, eax       ; Store file descriptor in esi

read_loop:
    ; Read character from input file
    mov eax, 3         ; sys_read syscall number
    mov ebx, esi       ; File descriptor
    mov ecx, buffer
    mov edx, buffer_size
    int 0x80
    cmp eax, 0         ; Check if end of file
    je end_program

    ; Process character (parse command)
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
    ; Execute corresponding logic for 'm' command
    jmp read_loop

; Implement process_M, process_b, process_B similarly

end_program:
    ; Close input file
    mov eax, 6         ; sys_close syscall number
    mov ebx, esi       ; File descriptor
    int 0x80

    ; Exit program
    mov eax, 1         ; sys_exit syscall number
    xor ebx, ebx       ; Exit code 0
    int 0x80
