[bits 32]

global start
extern run_game

start:

    call run_game

.hang:
    jmp .hang
