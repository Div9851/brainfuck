#include <stdio.h>

#define PROGRAM_SIZE 65536
#define STACK_SIZE 32
#define BF_MEMORY_SIZE 30000

char program[PROGRAM_SIZE];
int stack[STACK_SIZE], sp = 0, label = 0;

void codegen(char *program) {
    printf("    .section bss\n");
    printf("    .lcomm memory, %d\n", BF_MEMORY_SIZE);
    printf("    .text\n");
    printf("    .globl main\n");
    printf("main:\n");
    printf("    push %%rbp\n");
    printf("    mov %%rsp, %%rbp\n");
    printf("    xor %%rbx, %%rbx\n");
    while (*program != '\0') {
        switch (*program) {
        case '+':
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb (%%rcx), %%dl\n");
            printf("    inc %%dl\n");
            printf("    movb %%dl, (%%rcx)\n");
            break;
        case '-':
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb (%%rcx), %%dl\n");
            printf("    dec %%dl\n");
            printf("    movb %%dl, (%%rcx)\n");
            break;
        case '>':
            printf("    inc %%rbx\n");
            break;
        case '<':
            printf("    dec %%rbx\n");
            break;
        case '.':
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb (%%rcx), %%dil\n");
            printf("    mov stdout(%%rip), %%rsi\n");
            printf("    call putc\n");
            break;
        case ',':
            printf("    mov stdin(%%rip), %%rdi\n");
            printf("    call getc\n");
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb %%al, (%%rcx)\n");
            break;
        case '[':
            printf(".L.begin.%d:\n", label);
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb (%%rcx), %%dl\n");
            printf("    test %%dl, %%dl\n");
            printf("    jz .L.end.%d\n", label);
            stack[sp++] = label++;
            break;
        case ']':
            int to = stack[--sp];
            printf("    lea memory(%%rip), %%rcx\n");
            printf("    add %%rbx, %%rcx\n");
            printf("    movb (%%rcx), %%dl\n");
            printf("    test %%dl, %%dl\n");
            printf("    jnz .L.begin.%d\n", to);
            printf(".L.end.%d:\n", to);
            break;
        }
        ++program;
    }
    printf("    xor %%rax, %%rax\n");
    printf("    pop %%rbp\n");
    printf("    ret\n");
}

int main(int argc, char **argv) {
    FILE *in = fopen(argv[1], "r");
    fread(program, 1, PROGRAM_SIZE, in);
    codegen(program);
}
