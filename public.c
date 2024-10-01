#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../p4-interp.h"

/* check for null-pointer handling */
START_TEST (C_error_decode_NULL_cond_valA)
{
    y86_t cpu;
    y86_inst_t inst;
    y86_register_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    inst.type = NOP;
    valE = decode_execute (&cpu, NULL, &inst, NULL);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == INS);
}
END_TEST

/* check decoding and execution of the HALT instruction */
START_TEST (C_func_decode_halt)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_register_t valA = 0;
    y86_register_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    inst.type = HALT;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == HLT);
}
END_TEST

/* check decoding and execution of the NOP instruction */
START_TEST (C_func_decode_nop)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_register_t valA = 0;
    y86_register_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    inst.type = NOP;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == AOK);
}
END_TEST

/* helper function: extract the given register value from a CPU state */
static y86_register_t _check_get_reg (y86_t *cpu, y86_rnum_t num)
{
    switch (num) {
        case 0: return cpu->rax; break;
        case 1: return cpu->rcx; break;
        case 2: return cpu->rdx; break;
        case 3: return cpu->rbx; break;
        case 4: return cpu->rsp; break;
        case 5: return cpu->rbp; break;
        case 6: return cpu->rsi; break;
        case 7: return cpu->rdi; break;
        case 8: return cpu->r8; break;
        case 9: return cpu->r9; break;
        case 10: return cpu->r10; break;
        case 11: return cpu->r11; break;
        case 12: return cpu->r12; break;
        case 13: return cpu->r13; break;
        case 14: return cpu->r14; break;

        default: break;
    }
    return 0;
}

/* check decoding and execution of the IRMOVQ instruction */
START_TEST (C_func_decode_irmovq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_register_t valA = 0;
    y86_register_t valE = 0;

    /* assemble test instruction and initial CPU state */
    inst.type = IRMOVQ;
    inst.size = 10;
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;
    inst.value = 0 + rand();
    inst.opcode = IRMOVQ << 4;
    inst.rb = (y86_rnum_t)(rand() % BADREG);		/* random register */

    /* test: load a positive value */
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == (y86_register_t)inst.value);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);

    /* test: load a negative value */
    inst.value = -inst.value;
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == (y86_register_t)inst.value);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);

    /* test: load a zero value */
    inst.value = 0;
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == (y86_register_t)inst.value);
    ck_assert (cpu.sf == 0);
    ck_assert (cpu.zf == 0);
    ck_assert (_check_get_reg (&cpu, inst.rb) == 0);
}
END_TEST

/* helper function: set the given register value in a CPU state */
static void _check_set_reg (y86_t *cpu, y86_rnum_t num, y86_register_t val)
{
    switch (num) {
        case 0: cpu->rax = val; break;
        case 1: cpu->rcx = val; break;
        case 2: cpu->rdx = val; break;
        case 3: cpu->rbx = val; break;
        case 4: cpu->rsp = val; break;
        case 5: cpu->rbp = val; break;
        case 6: cpu->rsi = val; break;
        case 7: cpu->rdi = val; break;
        case 8: cpu->r8 = val; break;
        case 9: cpu->r9 = val; break;
        case 10: cpu->r10 = val; break;
        case 11: cpu->r11 = val; break;
        case 12: cpu->r12 = val; break;
        case 13: cpu->r13 = val; break;
        case 14: cpu->r14 = val; break;
        default: break;
    }
}

/* check decoding and execution of the OPq instructions */
START_TEST (C_func_decode_opq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_register_t valA = 0;
    y86_register_t valE = 0;
    y86_register_t expected_valE = 0;
    y86_register_t regs[BADREG];
    uint8_t i = 0;

    memset (&inst, 0, sizeof (inst));
    inst.type = OPQ;
    inst.size = 2;

    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;
    for (i = RAX; i < BADREG; i++)
    {
        /* set registers to random values */
        regs[i] = (((int64_t)rand()) << 32) | rand();
        _check_set_reg (&cpu, i, regs[i]);
    }

    /* addition */
    inst.ra = (y86_rnum_t)(rand() % BADREG);
    inst.rb = (y86_rnum_t)(rand() % BADREG);
    inst.op = ADD;
    expected_valE = _check_get_reg (&cpu, inst.ra) + _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* subtraction */
    inst.ra = (y86_rnum_t)(rand() % BADREG);
    inst.rb = (y86_rnum_t)(rand() % BADREG);
    inst.op = SUB;
    expected_valE = _check_get_reg (&cpu, inst.rb) - _check_get_reg (&cpu, inst.ra);
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* bitwise AND */
    inst.ra = (y86_rnum_t)(rand() % BADREG);
    inst.rb = (y86_rnum_t)(rand() % BADREG);
    inst.op = AND;
    expected_valE = _check_get_reg (&cpu, inst.ra) & _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));

    /* bitwise XOR */
    inst.ra = (y86_rnum_t)(rand() % BADREG);
    inst.rb = (y86_rnum_t)(rand() % BADREG);
    inst.op = XOR;
    expected_valE = _check_get_reg (&cpu, inst.ra) ^ _check_get_reg (&cpu, inst.rb);
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == expected_valE);
    ck_assert (regs[inst.ra] == _check_get_reg (&cpu, inst.ra));
    ck_assert (regs[inst.rb] == _check_get_reg (&cpu, inst.rb));
}
END_TEST

/* check handling of invalid OPq instruction */
START_TEST (C_error_decode_invalid_opq)
{
    y86_t cpu;
    y86_inst_t inst;
    bool cond = false;
    y86_register_t valA = 0;
    y86_register_t valE = 0;

    memset (&inst, 0, sizeof (inst));
    memset (&cpu, 0, sizeof (cpu));
    cpu.stat = AOK;

    inst.type = OPQ;
    inst.op = BADOP;
    valE = decode_execute (&cpu, &cond, &inst, &valA);
    ck_assert (valE == 0 || valE != 0);
    ck_assert (cpu.stat == INS);
}
END_TEST

void public_tests (Suite *s)
{
    TCase *tc_public = tcase_create ("Public");
    tcase_add_test (tc_public, C_error_decode_NULL_cond_valA);
    tcase_add_test (tc_public, C_func_decode_halt);
    tcase_add_test (tc_public, C_func_decode_nop);
    tcase_add_test (tc_public, C_func_decode_irmovq);
    tcase_add_test (tc_public, C_func_decode_opq);
    tcase_add_test (tc_public, C_error_decode_invalid_opq);
    suite_add_tcase (s, tc_public);
}

