#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <time.h>

#include "chip8.h"

#define A 0xa
#define B 0xb
#define C 0xc
#define D 0xd
#define E 0xe
#define F 0xf


Chip8::Chip8(char* filename)
{


	FILE *f = fopen(filename, "rb");
	if (f == NULL)
	{
		printf("Error: couldn't open %s\n", filename);
		exit(1);
	}


	// Get file size
	fseek(f, 0L, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0L, SEEK_SET);

	fread(memory+0x200, fsize, 1, f);
	fclose(f);



	pc = 0x200;

	sp = -1;

	delay_timer = 0;
	sound_timer = 0;

	for (int i = 0; i < 16; i++)
	{
		V[i] = 0;
	}


	// Store fontset in memory from 0x50 to 0x9F
	memcpy(memory+0x50, chip8_fontset, sizeof(chip8_fontset));
}


void Chip8::debug() 
{
	printf("====================\n");
	printf("Debug Output:\n");
	printf("Var Regs:\n");
	for (int i = 0; i < 16; i++)
		printf("V%d\t", i);
	printf("\n");
	for (int i = 0; i < 16; i++)
		printf("%x\t", V[i]);
	printf("\n");

	printf("Index register:\n");
	printf("%x\t", I);
	printf("\n");

}



void Chip8::Unimplemented(uint16_t inst)
{
	printf("Unimplemented instruction: %04x\n", inst);
}

void Chip8::DecodeExec()
{
	uint8_t *code = &memory[pc];
	uint16_t inst = (code[0] << 8) | code[1];

	uint8_t firstnib = inst >> 12;

	uint8_t x = (inst & 0x0F00) >> 8;
	uint8_t y = (inst & 0x00F0) >> 4;

	uint8_t n = inst & 0x000F;
	uint8_t nn = inst & 0x00FF;
	uint16_t nnn = inst & 0x0FFF;
	
	//printf("running: %x\n", inst);

	pc = pc + 2;

	switch (firstnib)
	{
		case 0x0: 
			{
				switch (inst)
				{
					case 0x00e0: for (int i = 0; i < 128; i+=2) display[i] = 0; break;
					case 0x00ee: 
					{
						// RETurn from a subroutine, pop stack
						// should fail if stack pointer is  < -1
						pc = stack[sp];
						sp--;
						if (sp < -1)
						{
							fprintf(stderr, "Error: missing stack trace\n");
							exit(1);
						}
						break;

					}
					default: Unimplemented(inst); break; // Machine Language Routine - Not necessary
				}
				break;
			}
		case 0x1: pc = nnn; break;
		case 0x2: 
		{
			// CALL subroutine and add current pc to stack 
			// should fail if stack pointer is 16
			sp++;
			if (sp > 15)
			{
				fprintf(stderr, "Error: stack overflow\n");
				exit(1);
			}
			stack[sp] = pc;
			pc = nnn;

			break;
		}
		case 0x3: if (V[x] == nn) pc += 2; printf("hi: %x\n", V[x]); break;
		case 0x4: if (V[x] != nn) pc += 2; break;
		case 0x5: if (V[x] == V[y]) pc += 2; break;
		case 0x6: V[x] = nn; break;
		case 0x7: V[x] += nn; break;
		case 0x8: 
			{
				switch (n)
				{
					case 0x0: V[x] = V[y]; break;
					case 0x1: V[x] |= V[y]; break;
					case 0x2: V[x] &= V[y]; break;
					case 0x3: V[x] ^= V[y]; break;
					case 0x4:  
					{
						if (V[x] + V[y] > 0xff)
							V[F] = 1;
						else 
							V[F] = 0;
						V[x] += V[y];
						break;
					}
					case 0x5: 
					{
						if (V[x] >= V[y])
							V[F] = 1;
						else
							V[F] = 0;
						V[x] -= V[y];
						break;
					}
					case 0x6: V[F] = V[x] & 0x1; V[x] >>= 1; break;
					case 0x7: 
					{
						if (V[y] >= V[x])
							V[F] = 1;
						else 
							V[F] = 0;
						V[x] = V[y] - V[x];
						break;
					}
					case 0xe: V[F] = (V[x] & 0x80) >> 7; V[x] <<= 1; break;
				}
				break;
			}
		case 0x9: if (V[x] != V[y]) pc += 2; break;
		case 0xa: I = nnn; break;
		case 0xb: pc = V[0] + nnn; break;
		case 0xc: 
		{
			srand(time(0));
			uint8_t rando = rand() % 256;
			V[x] = rando & nn;
			break;
		}
		case 0xd: 
		{
			uint16_t x_coord = V[x] % 64;
			uint16_t y_coord = V[y] % 32;
			V[F] = 0;

			//printf("DRAWING at (%x, %x), %x pixels tall\n", x_coord, y_coord, n); 
			for (unsigned int i = 0; i < n; i++)
			{
				uint8_t sprite = memory[I + i];	
				for (unsigned int bit = 0; bit < 8; bit++)
				{
					uint16_t spritePixel = sprite & (0x80 >> bit);
					if (spritePixel != 0)
					{
						uint32_t *currentPixel = &display[(x_coord+bit) + 64*(y_coord+i)];
						if (*currentPixel == 1)
							V[F] = 1;
						*currentPixel ^= 0xFFFFFFFF;
					}
				}				
			}
			break;

		}
		case 0xe: 
			{
				switch (nn)
				{	
					case 0x9e: if (keypad[V[x]]) pc += 2; break;
					case 0xa1: if (!keypad[V[x]]) pc += 2; break;
				}
				break;
			}
		case 0xf:
			{
				switch (nn)
				{
					case 0x07: V[x] = delay_timer; printf("delay: %x\n", delay_timer); break;
					case 0x0a: Unimplemented(inst); break;
					case 0x15: delay_timer = V[x]; break;
					case 0x18: sound_timer = V[x]; break;
					case 0x1e: I += V[x]; break;
					case 0x29: I = 0x50 + (5*V[x]); break;
					case 0x33: 
					{
						memory[I+2] = V[x]%10; 
						memory[I+1] = (V[x] / 10) % 10; 
						memory[I] = (V[x] / 100) % 10;
						//printf("%d: %d, %d, %d\n", V[x], memory[I+2], memory[I+1], memory[I]);
						break;
					}
					case 0x55: for (int i = 0; i < x+1; i++) memory[I+i] = V[i]; break;
					case 0x65: for (int i = 0; i < x+1; i++) V[i] = memory[I+i]; break;
					
				}
				break;
			}
	}
}

