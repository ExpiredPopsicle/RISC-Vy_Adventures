/*
    nop
    addi    x1, x1, 3
    addi    x1, x1, 3
    addi    x1, x1, 3
    addi    x1, x1, 3
    addi    x1, x1, 3
    addi    x1, x1, 3
    addi    ra, ra, 3

    addi    x1, x1, -1
*/
/*
    sltiu    x3, x1, -1
    slti     x2, x1, -1
    */



    addi x1, x1, 0x010

    xori x4, x1, -1
    ori  x5, x1, 0x100

    andi x6, x1, 0x111
    andi x7, x1, 0x101



    slli x8, x1, 0x1


    xori x1, x1, 0x011
    srai x9, x1, 0x5
