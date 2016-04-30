#include "dbg.h"
#include "cpu.h"
#include "opcodes.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

unsigned char font_set[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

unsigned char key_map[16] = {
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_R,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C,
    SDL_SCANCODE_V
};

void initialise_cpu(chip8 * cpu) {
	memset(cpu->memory, 0, 4096); //reset memory
	memset(cpu->V, 0, 16); //reset registers
	memset(cpu->stack, 0, 16); //reset stack
	memset(cpu->key, 0, 16); //reset keys
	//defaults
	cpu->I = 0;
	cpu->pc = 0x200;
	cpu->sp = 0;
	cpu->delay_timer = 0;
	cpu->sound_timer = 0;
	cpu->opcode = 0;

	// load fonts
	for (int i = 0; i < 80; i++) {
		cpu->memory[i] = font_set[i];
	}

	clear_screen(cpu);

	srand(time(NULL)); //reset random seed

}


bool load_rom(chip8 * cpu, const char *rom_name) {
	FILE * file = fopen(rom_name, "rb"); //open file in binary mode
	if(!file) {
		log_err("File does not exist.");
		return false;
	}

	fseek(file, 0, SEEK_END);
	unsigned long buffer_size = ftell(file);
	rewind(file);

	log_info("Read %lu bytes from %s.", buffer_size, rom_name);

	char *buffer = (char *) malloc((buffer_size + 1) * sizeof(char)); //allocate memory for buffer
	fread(buffer, buffer_size, 1, file);

	for(int i = 0; i < buffer_size; i++) {
		cpu->memory[512 + i] = buffer[i];
	}

	return true;
	
}


void emulate_cycle(chip8 * cpu) {
	//fetch opcode
	cpu->opcode = cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];
	
	decode_opcode(cpu);

	update_timers(cpu);

}

void clear_screen(chip8 * cpu) {
	memset(cpu->gfx, 0, 64 * 32);

	cpu->draw_flag = true;

}

void update_timers(chip8 * cpu) {
	if(cpu->delay_timer > 0) {
		--cpu->delay_timer;
	}

	if(cpu->sound_timer > 0) {
		if(cpu->sound_timer == 1) {
			printf("BEEP\n");
			--cpu->sound_timer;
		}
	}
}











