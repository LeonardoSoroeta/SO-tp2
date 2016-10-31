#include <sound.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libc.h>
#include <math.h>
#include <string.h>
#include <command.h>

// void fx_sound();
// void bg_sound();

// static char fx_on = 1;

static float base_frequencies[] = {
	16.352, // C   0
	17.324, // C#  1
	18.354, // D   2
	19.445, // D#  3
	20.602, // E   4
	21.827, // F   5
	23.125, // F#  6
	24.500, // G   7
	25.957, // G#  8
	27.500, // A   9
	29.135, // A#  10
	30.868, // B   11
	0,			// Pause  12
};

// void sound() {

// 	pid_t fx_sound_pid;

// 	fx_sound_pid = fork("fx_sound");

// 	if (fx_sound_pid == 0) {
// 		fx_sound();
// 	} else {
// 		bg_sound();
// 	}

// }

// void fx_sound() {

// 	int game_fd;
// 	char cmd = 'a';

// 	// que parezca un accidente
// 	for (uint64_t i = 0; i < 3000000;i++);

// 	game_fd = fopen("game2sound");

// 	while (read(game_fd, &cmd, 1)) {

// 		switch (cmd) {
// 			case 0x13:
// 				// bg on/off
// 				break;
// 			case 0x14:
// 				// fx on/off
// 				break;
// 			case 0x15:
// 				// asterisk hit
// 				if (fx_on) {
// 					make_beep();
// 				}

// 				break;
// 			case 0x16:
// 				// game over
// 				break;
			
// 		}
// 	}

// 	while (1);
// }

// void sig_hndlr(int sig) {
// 	fprintf(STDERR, "sig:%d\n", sig);
// }

void sound() {
	
	int tune_fd;
	char *songs;
	song_t parsed_songs[20];
	note_t current_note;
	uint32_t one_note_length;
	int choice = 6;
	int i = 0;

	tune_fd = fopen("songs");

	songs = (char*)start(tune_fd);

	parse_sounds(songs, parsed_songs);

	one_note_length = 60000 / parsed_songs[choice-1].beat * 4;

	while (1) {

		for (i = 0; i < parsed_songs[choice-1].read_notes; i++) {

			current_note = parsed_songs[choice-1].notes[i];

			beepwo( one_note_length / current_note.duration, base_frequencies[current_note.pitch] * (1 << current_note.octave) );
		}
	}


	while (1);
}
