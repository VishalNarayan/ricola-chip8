#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "disassembler.h"


void Disassemble(uint8_t *buffer, uint16_t pc)
{
	uint8_t *code = &buffer[pc];
	uint16_t inst = (code[0] << 8) | code[1];

	printf("0x%x: %04x | ", pc, inst);

	uint8_t firstnib = inst >> 12;

	uint8_t x = (inst & 0x0F00) >> 8;
	uint8_t y = (inst & 0x00F0) >> 4;

	uint8_t n = inst & 0x000F;
	uint8_t nn = inst & 0x00FF;
	uint16_t nnn = inst & 0x0FFF;
	

	switch (firstnib)
	{
		case 0x0: 
			{
				switch (inst)
				{
					case 0x00e0: printf("Nib 0: Clear screen.\n"); break;
					case 0x00ee: printf("Nib 0: Return.\n"); break;
					default: printf("Nib 0: Machine code routine %03x\n", nnn); break;
				}
				break;
			}
		case 0x1: printf("Nib 1 (JUMP): NNN is %03x\n", nnn); break;
		case 0x2: printf("Nib 2 (CALL): NNN is %03x\n", nnn); break;
		case 0x3: printf("Nib 3 (SKIP == ): register is %x\n", x); break;
		case 0x4: printf("Nib 4 (SKIP != ): register is %x\n", x); break;
		case 0x5: printf("Nib 5 (SKIPV == ): registers are %x and %x\n", x, y); break;
		case 0x6: printf("Nib 6 (SET): set register %x to %02x\n", x, nn); break;
		case 0x7: printf("Nib 7 (ADD): add value %02x to register %x\n", nn, x); break;
		case 0x8: 
			{
				switch (n)
				{
					case 0x0: printf("Nib 8 (SET): Set V%x to V%x\n", x, y); break;
					case 0x1: printf("Nib 8 (OR): V%x = V%x OR V%x", x, x, y); break;
					case 0x2: printf("Nib 8 (AND): V%x = V%x AND V%x", x, x, y); break;
					case 0x3: printf("Nib 8 (XOR): V%x = V%x XOR V%x", x, x, y); break;
					case 0x4: printf("Nib 8 (ADD): V%x = V%x + V%x", x, x, y); break;
					case 0x5: printf("Nib 8 (SUB): V%x = V%x - V%x", x, x, y); break;
					case 0x6: printf("Nib 8 (SHIFT): Store LSB in VF and shift V%x right by 1\n", x); break;
					case 0x7: printf("Nib 8 (SUB): V%x = V%x - V%x", x, y, x); break;
					case 0xe: printf("Nib 8 (SHIFT): Store MSB in VF and shift V%x left by 1\n", x); break;
				}
				break;
			}
		case 0x9: printf("Nib 9 (SKIPV != ): registers are %x and %x\n", x, y); break;
		case 0xa: printf("Nib A (SET index reg): set index reg to %03x\n", nnn); break;
		case 0xb: printf("Nib B (JUMP Addr): Jump to address %03x + value at reg 0\n", nnn); break;
		case 0xc: printf("Nib C (RAND): Set reg %x to random # AND %02x\n", x, nn);
		case 0xd: printf("Nib D (DRAW): draw sprite %x px tall, at x-value from reg %x and y-value from reg %x\n", n, x, y); break;
		case 0xe: 
			{
				switch (nn)
				{	
					case 0x9e: printf("Nib E (SKIP): skip one instruction if key corresponding to V%x is pressed.\n", x); break;
					case 0xa1: printf("Nib E (SKIP): skip if key corresponding to V%x is not pressed.\n", x); break;
				}
				break;
			}
		case 0xf:
			{
				switch (nn)
				{
					case 0x07: printf("Nib F: Set V%x to current value of delay timer\n", x); break;
					case 0x0a: printf("Nib F: Block and wait for key input, place in V%x and continue\n", x); break;
					case 0x15: printf("Nib F: Set delay timer to value in V%x\n", x); break;
					case 0x18: printf("Nib F: Set sound timer to value in V%x\n", x); break;
					case 0x1e: printf("Nib F: Add value in V%x to index reg\n", x); break;
					case 0x29: printf("Nib F: Set index reg to address of hex char in V%x\n", x); break;
					case 0x33: printf("Nib F: Convert value in V%x to BCD, store each digit starting from address in index reg\n", x); break;
					case 0x55: printf("Nib F: Store registers to memory: V0 to V%x in I\n", x); break;
					case 0x65: printf("Nib F: Store memory in registers: I in V0 to V%x\n", x); break;
				}
				break;
			}
	}
}

