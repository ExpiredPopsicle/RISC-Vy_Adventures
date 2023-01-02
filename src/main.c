#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <malloc.h>

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

uint32_t sign_extend(uint32_t value, uint32_t length)
{
    uint32_t extend_bit = (value >> (length - 1)) & 1;
    uint32_t extension = 0xffffffff << length;

    printf("value:      %.32b\n", value);
    printf("extend_bit: %.32b\n", extend_bit);
    printf("extension:  %.32b\n", extension);

    uint32_t extend_or_not[2] = {
        0,
        extension
    };

    uint32_t result = value | extend_or_not[extend_bit];

    printf("result:     %.32b\n", result);
    return result;
}

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
    funct3 = (instruction & 0b00000000000000000111000000000000) >> 12;
    rd     = (instruction & 0b00000000000000000000111110000000) >>  7;
    opcode = (instruction & 0b00000000000000000000000001111111) >>  0;

    printf("instruction: %b\n", instruction);
    printf("opcode:      %b\n", opcode);
    printf("rd:          %b (%u)\n", rd, rd);
    printf("funct3:      %b\n", funct3);
    printf("rs1:         %b (%u)\n", rs1, rs1);
    printf("imm:         %b (%u)\n", imm, imm);

    uint32_t imm_value_signextended =
        sign_extend(imm, 12);

    uint32_t rs1_value =
        read_register(state, rs1);

    printf("imm_value_signextended: %b (%u)\n",
        imm_value_signextended, imm_value_signextended);

    switch(funct3)
    {
        case 0b000: // ADDI

            write_register(
                state,
                rd,
                imm_value_signextended + rs1_value);
            break;

        case 0b010: // SLTI

            write_register(
                state,
                rd,
                (int32_t)imm_value_signextended >= (int32_t)rs1_value);
            break;

        case 0b011: // SLTIU

            write_register(
                state,
                rd,
                imm_value_signextended >= rs1_value);
            break;

        case 0b100: // XORI

            write_register(
                state,
                rd,
                imm_value_signextended ^ rs1_value);
            break;

        case 0b110: // ORI

            write_register(
                state,
                rd,
                imm_value_signextended | rs1_value);
            break;

        case 0b111: // ANDI

            write_register(
                state,
                rd,
                imm_value_signextended & rs1_value);
            break;

        case 0b001:
        case 0b101:
            // SLLI
            // SRLI/SRAI
        {
            uint32_t shamt = imm & 0b11111;

            uint32_t shift_result;
            if(funct3 == 1) {
                shift_result = rs1_value << shamt;
            } else {
                shift_result = rs1_value >> shamt;
            }

            printf("imm:       %b\n", imm);
            printf("rs1_value: %b\n", rs1_value);
            if((imm & 0b10000000000) && (rs1_value & 1)) {

                uint32_t arithmetic_extension = 0xffffffff >> 32 - shamt;
                printf(
                    "kjsdcnjksdfcnjksdcnjkdcnaskdjcnasdc %b\n",
                    arithmetic_extension);

                shift_result |= arithmetic_extension;
            }

            write_register(state, rd, shift_result);

        }
            break;
    }

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

void *load_file(
    const char *filename,
    size_t *out_length)
{
    FILE *in_file = fopen(filename, "rb");
    assert(in_file); // TODO: Actual error.

    size_t input_buffer_size = 256;
    char *output_buffer = (char *)malloc(input_buffer_size);
    assert(output_buffer); // TODO: Actual error.

    int in_byte;
    size_t write_ptr = 0;
    while((in_byte = fgetc(in_file)) != EOF) {

        output_buffer[write_ptr++] = in_byte;

         if(write_ptr >= input_buffer_size) {

            size_t old_input_buffer_size = input_buffer_size;
            void *old_buffer = output_buffer;

            input_buffer_size <<= 1;

            // TODO: Actual error.
            assert(input_buffer_size > old_input_buffer_size);

            output_buffer = realloc(
                output_buffer, input_buffer_size);

            assert(output_buffer); // TODO: Actual error.

            if(!output_buffer) {
                free(old_buffer);
            }

        }
    }

    *out_length = write_ptr;
    fclose(in_file);
    return output_buffer;
}

int main(int argc, char *argv[])
{
    // uint32_t memory[] = {
    //     0x00000013, // nop
    //     0x00308093, // addi    x1, x1, 3
    // };

    size_t mem_size = 0;
    void *memory = load_file("test/test.asm.bin", &mem_size);
    uint32_t program_length = mem_size / 4;

    printf("program_length: %u\n", program_length);

    struct CPUState state;
    memset(&state, 0, sizeof(state));

    while(state.registers.program_counter < program_length) {
        uint32_t the_instruction = ((uint32_t*)memory)[state.registers.program_counter];
        cpu_instruction_op_imm(&state, the_instruction);
        state.registers.program_counter += 1;
    }

    dump_state(&state);

    free(memory);


    uint32_t sign_extend_me = 0b0001011;
    sign_extend(sign_extend_me, 4);

    return 0;
}
