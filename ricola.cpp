#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include <SDL2/SDL.h>

#include <chrono>
#include <thread>

#include "disassembler.h"
#include "chip8.h"
//#include "display.h"


#define A 0xa
#define B 0xb
#define C 0xc
#define D 0xd
#define E 0xe
#define F 0xf


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;
SDL_Surface* hello = NULL;



int main (int argc, char** argv)
{
	
	if (argc != 2)
	{
		printf("Need to pass in CHIP-8 program file!\n");
		exit(1);
	}



	Chip8* chip8 = new Chip8(argv[1]);


	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! Error: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Ricola Chip-8", 0, 0, 1024, 512, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("SDL window couldn't be created! Error: %s\n", SDL_GetError());
		exit(2);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);



	bool quit = false;
	SDL_Event e;

	//while (chip8->pc < (chip8->fsize + 0x200))
	while (!quit)
	{
		Disassemble(chip8->memory, chip8->pc);
		
		
		chip8->DecodeExec();

		while (SDL_PollEvent(&e) != 0)
		{

			switch (e.type)
			{
				case SDL_QUIT: quit = true; break;
				case SDL_KEYDOWN:
				{
					uint8_t* kp = chip8->keypad;
	
					switch (e.key.keysym.sym)
					{
						case SDLK_KP_0: *(kp) = 1; break;
						case SDLK_KP_1: *(kp+1) = 1; break;
						case SDLK_KP_2: *(kp+2) = 1; break;
						case SDLK_KP_3: *(kp+3) = 1; break;
						case SDLK_KP_4: *(kp+4) = 1; break;
						case SDLK_KP_5: *(kp+5) = 1; break;
						case SDLK_KP_6: *(kp+6) = 1; break;
						case SDLK_KP_7: *(kp+7) = 1; break;
						case SDLK_KP_8: *(kp+8) = 1; break;
						case SDLK_KP_9: *(kp+9) = 1; break;
						case SDLK_KP_PERIOD: *(kp+A) = 1; break;
						case SDLK_KP_DIVIDE: *(kp+B) = 1; break;
						case SDLK_KP_MULTIPLY: *(kp+C) = 1; break;
						case SDLK_KP_MINUS: *(kp+D) = 1; break;
						case SDLK_KP_PLUS: *(kp+E) = 1; break;
						case SDLK_KP_ENTER: *(kp+F) = 1; break;
					}
					break;
				}
				case SDL_KEYUP:
				{

					uint8_t* kp = chip8->keypad;
					switch (e.key.keysym.sym)
					{
						case SDLK_KP_0: *(kp) = 0; break;
						case SDLK_KP_1: *(kp+1) = 0; break;
						case SDLK_KP_2: *(kp+2) = 0; break;
						case SDLK_KP_3: *(kp+3) = 0; break;
						case SDLK_KP_4: *(kp+4) = 0; break;
						case SDLK_KP_5: *(kp+5) = 0; break;
						case SDLK_KP_6: *(kp+6) = 0; break;
						case SDLK_KP_7: *(kp+7) = 0; break;
						case SDLK_KP_8: *(kp+8) = 0; break;
						case SDLK_KP_9: *(kp+9) = 0; break;
						case SDLK_KP_PERIOD: *(kp+A) = 0; break;
						case SDLK_KP_DIVIDE: *(kp+B) = 0; break;
						case SDLK_KP_MULTIPLY: *(kp+C) = 0; break;
						case SDLK_KP_MINUS: *(kp+D) = 0; break;
						case SDLK_KP_PLUS: *(kp+E) = 0; break;
						case SDLK_KP_ENTER: *(kp+F) = 0; break;
					}
					break;
				}
			}


		}

		SDL_UpdateTexture(texture, nullptr, chip8->display, sizeof(chip8->display[0]) * 64);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
			
		//std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}



	//printf("done, with file size %d\n", fsize);
	//debug();


	exit(0);

}