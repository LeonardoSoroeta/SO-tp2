#ifndef __GAME_H__
#define __GAME_H__

#include <graphics.h>

typedef struct {
	float x;
	float y;
	float direction_x;
	float direction_y;
	float speed;
	int radius;
	gfx_ellipse *gfx;
} bullet_t;

typedef struct {
	float x;
	float y;
	float direction_x;
	float direction_y;
	float speed;
	int radius;
	gfx_ellipse *gfx;
} asteroid_t;

typedef enum {

	game_status_pre_screensaver,
	game_status_screensaver_setup,
	game_status_screensaver,
	game_status_post_screen_saver,
	game_status_pre_transition_to_fullscreen,
	game_status_transition_to_fullscreen,
	game_status_post_transition_to_fullscreen,
	game_status_pre_menu,
	game_status_menu,
	game_status_post_menu,
	game_status_pre_transition_from_fullscreen,
	game_status_transition_from_fullscreen,
	game_status_post_transition_from_fullscreen,
	game_status_pre_playing,
	game_staus_playing,
	game_status_post_playing
} game_status_t;

void game();

#endif