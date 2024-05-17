section .data
    output_filename db 'output.cow',0

section .text
    global main

main:
    ; Open output file
    mov eax, 5         ; sys_open syscall number
    mov ebx, output_filename
    mov ecx, 577       ; O_WRONLY | O_CREAT | O_TRUNC mode
    mov edx, 0644      ; Permissions
    int 0x80
    mov edi, eax       ; Store file descriptor in edi

    ; Generate COW code based on parsed commands
    ; Example: "moo" for 'm', "MOO" for 'M', etc.

    ; Write generated COW code to output file

    ; Close output file
    mov eax, 6         ; sys_close syscall number
    mov ebx, edi       ; File descriptor
    int 0x80

    ; Exit program
    mov eax, 1         ; sys_exit syscall number
    xor ebx, ebx       ; Exit code 0
    int 0x80
