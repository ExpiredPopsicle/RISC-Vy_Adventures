#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>


struct CPUState
{
    struct {
        uint32_t registers[32];
        uint32_t program_counter;
    } registers;
};

#define DECODE_R_TYPE()                         \
    uint32_t funct7;                            \
    uint32_t funct3;                            \
    uint32_t rs1;                               \
    uint32_t rs2;                               \
    uint32_t rd;                                \
    uint32_t opcode;

void write_register(
    struct CPUState *state,
    uint32_t rd,
    uint32_t value)
{
    if(rd >= 32) {
        // TODO: THROW ERROR.
        assert(0);
        return;
    }

    if(rd == 0) {
        return;
    }

    state->registers.registers[rd] = value;
}

uint32_t read_register(
    const struct CPUState *state,
    uint32_t rs)
{
    if(rs >= 32) {
        // TODO: THROW ERROR.
        assert(0);
        return 0;
    }

    return state->registers.registers[rs];
}


void dump_state(const struct CPUState *state)
{
    printf("pc: %u\n", state->registers.program_counter);

    for(uint32_t i = 0; i < 32; i++) {
        printf("x%.2u: %.8x\n",
            i, state->registers.registers[i]);
    }
}

void (*CPUInstruction)(uint32_t instruction);

void cpu_instruction_op_imm(
    struct CPUState *state,
    uint32_t instruction)
{
    uint32_t imm;
    uint32_t funct3;
    uint32_t rs1;
    uint32_t rd;
    uint32_t opcode;

    imm    = (instruction & 0b11111111111100000000000000000000) >> 20;
    rs1    = (instruction & 0b00000000000011111000000000000000) >> 15;
    funct3 = (instruction & 0b00000000000000000111000000000000) >> 13;
    rd     = (instruction & 0b00000000000000000000111110000000) >>  7;
    opcode = (instruction & 0b00000000000000000000000001111111) >>  0;

    printf("instruction: %b\n", instruction);
    printf("opcode:      %b\n", opcode);
    printf("rd:          %b (%u)\n", rd, rd);
    printf("funct3:      %b\n", funct3);
    printf("rs1:         %b (%u)\n", rs1, rs1);
    printf("imm:         %b (%u)\n", imm, imm);


    switch(funct3)
    {
        case 0b000:
            // ADDI
            write_register(
                state,
                rd,
                imm + read_register(
                    state, rs1));
            break;

        case 0b010:
            // SLTI
            break;

        case 0b011:
            // SLTIU
            break;

        case 0b100:
            // XORI
            break;

        case 0b110:
            // ORI
            break;

        case 0b111:
            // ANDI
            break;

        case 0b001:
        case 0b101:
            // SLLI
            // SRLI/SRAI
        {
            uint32_t shamt = imm & 0b11111;
        }
            break;
    }


    // DECODE_I_TYPE();

    // 0b0010011
}

void execute_instruction(
    struct CPUState *state,
    uint32_t instruction)
{
    switch((instruction & 0b1111100) >> 2)
    {
        case 0b00100:
            cpu_instruction_op_imm(state, instruction);
            break;
        default:
            // TODO: Throw error.
            assert(0);
    }
}

int main(int argc, char *argv[])
{
    uint32_t memory[] = {
        0x00000013, // nop
        0x00308093, // addi    x1, x1, 3
    };

    struct CPUState state;
    memset(&state, 0, sizeof(state));

    while(state.registers.program_counter < sizeof(memory) / sizeof(memory[0])) {
        uint32_t the_instruction = memory[state.registers.program_counter];
        cpu_instruction_op_imm(&state, the_instruction);
        state.registers.program_counter += 1;
    }

    // the_instruction = 0x00308093;

    // cpu_instruction_op_imm(&state, the_instruction);

    dump_state(&state);

    return 0;
}
