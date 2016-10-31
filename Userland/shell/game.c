#include <game.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <libc.h>
#include <graphics.h>
#include <tweens.h>
#include <math.h>
#include <string.h>

#define DELTA 2
#define DRAW 20

#define ASTEROID_MAX_INTERVAL 50000

#define MAX_BULLET 20
#define MAX_ASTEROID 20
#define SHIP_RADIUS 20
#define BULLET_RADIUS 2
#define ASTEROID_RADIUS 10
#define BULLET_SPEED 5
#define ASTEROID_SPEED 3
#define X_CENTER (VGA_WIDTH/2)
#define Y_CENTER (VGA_HEIGHT/2)

#define NULL ((void*)0)

static game_status_t game_status;

static gfx_context *ctx;
static gfx_rect *rect;
static gfx_rect *rect2;

static int plane_fd;
static bmp_header_t* plane_header;

static uint64_t _time = 0;
static uint64_t _rel_time = 0;
static uint64_t last_time = 0;

static int update(event_id id, uint64_t a, uint64_t b, uint64_t c);
static int key_handler(event_id id, uint64_t a, uint64_t b, uint64_t c);
static int mouse_handler(event_id id, uint64_t x, uint64_t y, uint64_t flag);

static char *key_buffer;
static uint8_t insert_index = 0;
static uint8_t read_index = 0;

static char listening_to_mouse = 0;
static uint16_t mouse_x = 0;
static uint16_t mouse_y = 0;

static char listen_to_keys = 0;

static bullet_t ** bullets;
static asteroid_t ** asteroids;

static int game_fd;

static uint32_t* screen_backup;

static uint16_t random_state = 0xACE1u;  /* Any nonzero start state will work. */

static void restore_shell();
static inline void seed();
static inline float rand();

static inline int sign(float value);
static inline float abs(float value);

static inline int sign(float value){
	return (value < 0 ) ? -1 : 1;
}

static inline float abs(float value){
	return value * sign(value);
}

static inline void seed() {
	random_state = _time % 65536;
}
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register
static inline float rand(){
	uint16_t bit;
	bit  = ((random_state >> 0) ^ (random_state >> 2) ^ (random_state >> 3) ^ (random_state >> 5) ) & 1;
	random_state = (random_state >> 1) | (bit << 15);
	return ((float)random_state)/65536;
}

// static int player_drawn = 0;

bullet_t *new_bullet(void);
asteroid_t *new_asteroid(void);
static int check_collisions(void);
void bullet_set_position(bullet_t* bullet);
void asteroid_set_position(asteroid_t* asteroid);
void game_over_fn();

static gfx_bmp* example_bmp;

int sound_fd;

void game_sig_handler(int sig);

void game_sig_handler(int sig) {
	// fprintf(STDERR, "s ");
	if (sig == 19) {
		// make_beep();
	}
}

void game() {
	// gfx_node* node;
	// gfx_node* node2;
	char c = 'a';
	int i = 0;


	game_fd = mkfifo("prueba");
	sound_fd = mkfifo("game2sound");

	signal(&game_sig_handler);

	key_buffer = (char*)calloc(512, sizeof(char));

	plane_fd = fopen("plane.bmp");
	plane_header = (bmp_header_t*)start(plane_fd);

	game_status = game_status_screensaver_setup;


	/**
	 *
	 *	Initiating context
	 * 
	 */

	ctx = begin_context(VGA_WIDTH/2, VGA_HEIGHT, VGA_WIDTH/2, 0);

	/**
	 *
	 *
	 *	Registering listeners
	 * 
	 */

	register_event(TIMER_TICK, &update);
	register_event(KEYBOARD, &key_handler);
	register_event(MOUSE, &mouse_handler);


	// Clear
	ctx_clear(ctx);

	/**
	 *
	 *
	 *  Pre allocate elements
	 * 
	 */


	bullets = (bullet_t**)calloc(MAX_BULLET, sizeof(bullet_t*));
	// pre allocate bullets
	for (i = 0; i < MAX_BULLET; i++) {
		bullets[i] = new_bullet();
	}
	
	asteroids = (asteroid_t**)calloc(MAX_ASTEROID, sizeof(asteroid_t*));
	// pre allocate asteroids
	for (i = 0; i < MAX_ASTEROID; i++) {
		asteroids[i] = new_asteroid();
	}

	seed();

	while (read(game_fd, &c, 1)) {
		

		// fprintf(STDERR, "read\n");

		if (c == 0x55) {
			
			printf("Just a sec... Backing screen up\n");

			screen_backup = screenshot();

			game_status = game_status_pre_transition_to_fullscreen;
		}
	}

	stop(getpid());

	while (1);
}

void restore_shell() {

	uint16_t i = 0;
	uint16_t j = 0;

	for (i = 0; i < VGA_HEIGHT; i++) {
		for (j = 0; j < VGA_WIDTH; j++) {
			set_pixel_color(ctx, j, i, screen_backup[i*VGA_WIDTH+j]);
		}
	}
}

static int key_handler(event_id id, uint64_t ascii, uint64_t b, uint64_t c) {

	if (listen_to_keys) {
		key_buffer[insert_index++] = (char)ascii;
		return 1;
	}

	return 0;
}

static int mouse_handler(event_id id, uint64_t x, uint64_t y, uint64_t flag) {

	if (listening_to_mouse) {

		mouse_x = x;
		mouse_y = y;

		if (flag & 0x1) {
			// Left Click
			// tenes que tener un flag de disparo
		}

		if (flag & 0x2) {
			// Right Click
		}

		if (flag & 0x4) {
			// Middle Click
		}

	} else {
		// algo??
	}

	return 0;
}

static int update(event_id id, uint64_t a, uint64_t b, uint64_t c) {

	char cmd;
	int i = 0;

	_time += DELTA;

	if (_time % DRAW == 0) {

		// draw_polygon(ctx, 20, 20, 6, 30, 0xffffff);

		switch (game_status) {

			/**
			 *
			 * 
			 * Before screensaver
			 *
			 * 
			 */
			case game_status_pre_screensaver:
				restore_shell();
				cont(1001);
				// cont(1002);
				game_status = game_status_screensaver;
				ctx_clear(ctx);
				example_bmp->visible = 1;
				break;

			/**
			 *
			 *
			 * Screensave setup
			 * 
			 */
			case game_status_screensaver_setup: {

				rect           = rect_create(ctx, 0, 0, 10, 10, 0xdead96);
				rect2          = rect_create(ctx, 0, 10, 10, 10, 0x96dead);
				example_bmp    = bmp_create(ctx, plane_header, 50, 50);

				add_node(ctx, node_create((void*)rect, shape_type_rect));
				add_node(ctx, node_create((void*)rect2, shape_type_rect));
				add_node(ctx, node_create((void*)example_bmp, shape_type_bmp));

				game_status = game_status_screensaver;

				break;
			}
			/**
			 *
			 *	Screen saver animation
			 * 
			 */
			case game_status_screensaver:
				rect->x = _time/DELTA % (VGA_WIDTH/2-20);
				rect->y = _time/DELTA % (VGA_HEIGHT-20);

				example_bmp->x = _time/DELTA % (VGA_WIDTH/2-64);

				rect2->x = (VGA_WIDTH/2-20) - _time/DELTA % (VGA_WIDTH/2-20);
				rect2->y = (VGA_HEIGHT-20) - _time/DELTA % (VGA_HEIGHT-20);
				break;

			/**
			 *
			 * Before transition
			 * 
			 */
			case game_status_pre_transition_to_fullscreen:
				// reset time
				_rel_time = 0;

				// hide spaceship
				example_bmp->visible = 0;

				// remove_node(ctx, example_bmp);
				game_status = game_status_transition_to_fullscreen;
				break;

			/**
			 *
			 *	Transition description
			 * 
			 */
			case game_status_transition_to_fullscreen:
				ctx->width = ease_in_sine( (double)_rel_time/DRAW, VGA_WIDTH/2, VGA_WIDTH - VGA_WIDTH/2, 2*DRAW );
				ctx->x     = ease_in_sine( (double)_rel_time/DRAW, VGA_WIDTH/2, 0 - VGA_WIDTH/2, 2*DRAW );

				rect->x = ctx->x + 20;
				rect2->x = ctx->x + 20;

				if (ctx->x <= 0) {
					game_status = game_status_post_transition_to_fullscreen;
				}
				_rel_time += DRAW;
				break;

			/**
			 *
			 * 
			 * After transition
			 *
			 * 
			 */
			case game_status_post_transition_to_fullscreen:
				ctx->width = VGA_WIDTH;
				ctx->x = 0;
				rect->visible = 0;
				rect2->visible = 0;
				// remove_node(ctx, rect);
				// remove_node(ctx, rect2);
				game_status = game_status_pre_menu;
				break;

			/**
			 *
			 *	Before menu
			 * 
			 */
			case game_status_pre_menu:
				ctx_clear(ctx);
				_rel_time = 0;
				game_status = game_status_menu;
				listen_to_keys = 1;
				listening_to_mouse = 1;
				
				example_bmp->visible = 1;

				// example_bmp = bmp_create(ctx, plane_header, 50, 50);
				// add_node(ctx, node_create((void*)example_bmp, shape_type_bmp));

				example_bmp->x = (VGA_WIDTH-example_bmp->header->width)/2;
				example_bmp->y = (VGA_HEIGHT-example_bmp->header->height)/2;
				// add_node(ctx, player_node);


				break;

			/**
			 *
			 *
			 *	Menu description
			 * 
			 */
			case game_status_menu: {
				int found = 0;


				/********************************/
				// if(!player_drawn){
				// 	gfx_ellipse *player_gfx = ellipse_create(ctx, X_CENTER, Y_CENTER, SHIP_RADIUS, SHIP_RADIUS, 0x96dead);
				// 	add_node(ctx, node_create((void*)player_gfx, shape_type_ellipse));
				// 	player_drawn++;
				// }
				/********************************/

				for (i = 0; i < MAX_ASTEROID; ++i){
					
					if (asteroids[i]->gfx->visible) {
						
						asteroids[i]->x += asteroids[i]->direction_x * (asteroids[i])->speed;
						asteroids[i]->y += asteroids[i]->direction_y * asteroids[i]->speed;
						(asteroids[i]->gfx)->xc = asteroids[i]->x;
						(asteroids[i]->gfx)->yc = asteroids[i]->y;

						// if (asteroids[i]->x < 0) {
						// 	asteroids[i]->gfx->visible = 0;
						// }

						// if (asteroids[i]->x < 0 || asteroids[i]->x > VGA_WIDTH) {
						// 	asteroids[i]->gfx->visible = 0;
						// }
/*
						if (asteroids[i]->x - asteroids[i]->radius/2 < 0 || asteroids[i]->x + asteroids[i]->radius/2 > VGA_WIDTH) {
							asteroids[i]->gfx->visible = 0;
						}

						if (asteroids[i]->y - asteroids[i]->radius/2 < 0 || asteroids[i]->y + asteroids[i]->radius/2 > VGA_HEIGHT) {
							asteroids[i]->gfx->visible = 0;
						}*/
					}
				}

				for (i = 0; i < MAX_BULLET; ++i){
					
					if (bullets[i]->gfx->visible) {
						
						bullets[i]->x += bullets[i]->direction_x * (bullets[i])->speed;
						bullets[i]->y += bullets[i]->direction_y * bullets[i]->speed;
						bullets[i]->gfx->xc = bullets[i]->x;
						bullets[i]->gfx->yc = bullets[i]->y;

						if (bullets[i]->x - bullets[i]->radius < 0 || bullets[i]->x + bullets[i]->radius > VGA_WIDTH) {
							bullets[i]->gfx->visible = 0;
						}

						if (bullets[i]->y - bullets[i]->radius < 0 || bullets[i]->y + bullets[i]->radius > VGA_HEIGHT) {
							bullets[i]->gfx->visible = 0;
						}

					}
				}
				if ( read_index < insert_index ) {

					cmd = key_buffer[read_index++];

					switch (cmd) {
						case ' ':
							for (int i = 0; i < MAX_BULLET && !found; ++i){
								if (!bullets[i]->gfx->visible){
									found = 1;
									bullet_set_position(bullets[i]);
									bullets[i]->gfx->visible = 1;
								}
							}
							break;
						case 'q':
							cmd = '\0';
							write(game_fd, &cmd, 1);
							game_status = game_status_post_menu;
							listen_to_keys = 0;
							listening_to_mouse = 0;
							break;
						default:
							break;
					}


				}
				if(rand()*ASTEROID_MAX_INTERVAL < _time - last_time){
					for (int i = 0; i < MAX_ASTEROID && !found; ++i){							
						if (!asteroids[i]->gfx->visible){
							found = 1;
							asteroid_set_position(asteroids[i]);
							asteroids[i]->gfx->visible = 1;
							last_time = _time;
						}
					}
				}

				if(check_collisions()){
							cmd = '\0';
							write(game_fd, &cmd, 1);
							game_status = game_status_post_menu;
							listen_to_keys = 0;
							listening_to_mouse = 0;
				}

				// if (check_collisions()) {
				// 	cmd = '\0';
				// 	write(game_fd, &cmd, 1);
				// 	game_status = game_status_post_menu;
				// 	listen_to_keys = 0;
				// 	listening_to_mouse = 0;
				// }
				_rel_time += DRAW;
				break;
			}

			/**
			 *
			 *	After Menu
			 *
			 * 
			 */
			case game_status_post_menu: {
				// remove_node(ctx, player_node);
				
				int i = 0;

				ctx_clear(ctx);


				/**
				 *
				 *
				 *
				 *   SACAR TODO DE LA PANTALLA ACA
				 *
				 * 
				 */

				 for (i = 0; i < MAX_BULLET; i++) {
				 	bullets[i]->gfx->visible = 0;
				 }

				 for (i = 0; i < MAX_ASTEROID; i++) {
				 	asteroids[i]->gfx->visible = 0;
				 }


				_rel_time = 0;
				game_status = game_status_pre_transition_from_fullscreen;
				break;
			}

			/**
			 *
			 * Before returning to shell
			 *
			 * 
			 */
			case game_status_pre_transition_from_fullscreen:
				_rel_time = 0;
				game_status = game_status_transition_from_fullscreen;
				rect->visible = 1;
				rect2->visible = 1;
				example_bmp->visible = 0;
				break;

			/**
			 *
			 *
			 *	Back to shell transition
			 * 
			 */
			case game_status_transition_from_fullscreen:
				ctx->width = ease_in_sine( (double)_rel_time/DRAW, VGA_WIDTH, VGA_WIDTH/2 - VGA_WIDTH, 2*DRAW );
				ctx->x = ease_in_sine( (double)_rel_time/DRAW, 0, VGA_WIDTH/2, 2*DRAW );

				if (ctx->x >= VGA_WIDTH/2) {
					game_status = game_status_post_transition_from_fullscreen;
				}
				_rel_time += DRAW;
				break;

			/**
			 *
			 *	After transition to shell
			 * 
			 */
			case game_status_post_transition_from_fullscreen:
				ctx->width = VGA_WIDTH/2;
				ctx->x = VGA_WIDTH/2;
				game_status = game_status_pre_screensaver;
				break;
			/**
			 *
			 *	Should not happen
			 * 
			 */
			default:
				printf("upss\n");
				fprintf(STDERR, "upss\n");
				break;

		}

		context_tick(ctx);
	}


	return 0;
}

/**
 * easings
 * @see http://www.gizma.com/easing/
 */

bullet_t * new_bullet(){
	bullet_t * bullet;
	// float rel_x;
	// float rel_y;
	// float distance;

	bullet = malloc(sizeof(bullet_t));
	// rel_x = mouse_x - X_CENTER;
	// rel_y = mouse_y - Y_CENTER;
	// distance = sqrt(rel_y * rel_y + rel_x * rel_x);

	// bullet->direction_x= rel_x / distance;
	// bullet->direction_y= rel_y / distance;

	bullet->radius = BULLET_RADIUS;
	bullet->speed = BULLET_SPEED;

	// bullet->x = bullet->direction_x * SHIP_RADIUS + bullet->radius * bullet->direction_x + X_CENTER;
	// bullet->y = bullet->direction_y * SHIP_RADIUS + bullet->radius * bullet->direction_y + Y_CENTER;
	
	bullet->gfx = ellipse_create(ctx, 10, 10, BULLET_RADIUS*2, BULLET_RADIUS*2, 0x330066);
	bullet->gfx->visible = 0;

	add_node(ctx, node_create((void*)bullet->gfx, shape_type_ellipse));

	return bullet;
}

void bullet_set_position(bullet_t* bullet) {

	float rel_x;
	float rel_y;
	float distance;

	rel_x = mouse_x - X_CENTER;
	rel_y = mouse_y - Y_CENTER;
	distance = sqrt(rel_y * rel_y + rel_x * rel_x);

	bullet->direction_x = rel_x / distance;
	bullet->direction_y = rel_y / distance;

	bullet->radius = BULLET_RADIUS;
	bullet->speed = BULLET_SPEED;

	bullet->x = bullet->direction_x * SHIP_RADIUS + bullet->radius * bullet->direction_x + X_CENTER;
	bullet->y = bullet->direction_y * SHIP_RADIUS + bullet->radius * bullet->direction_y + Y_CENTER;

}

asteroid_t * new_asteroid() {
	
	asteroid_t *asteroid;

	asteroid = malloc(sizeof(asteroid_t));

	asteroid->radius = ASTEROID_RADIUS;//(1+rand()*10);
	asteroid->speed = ASTEROID_SPEED; //(1+rand()*4)/**/*0/**/;
	
	asteroid->gfx = ellipse_create(ctx, 100, 100, asteroid->radius*2, asteroid->radius*2, 0x123456);
	asteroid->gfx->visible = 0;

	add_node(ctx, node_create((void*)asteroid->gfx, shape_type_ellipse));
	return asteroid;
}

void asteroid_set_position(asteroid_t * asteroid){

	float angle;
	float dist_x;
	float dist_y;

	angle = rand()*PI+PI;//TODO *2 -PI

	asteroid->direction_x = cos(angle);
	asteroid->direction_y = sin(angle);

	dist_x = abs(X_CENTER / asteroid->direction_x);
	dist_y = abs(Y_CENTER / asteroid->direction_y);
	
	if (dist_x > dist_y) {

		asteroid->x = -dist_y * asteroid->direction_x + X_CENTER; 
		asteroid->y = -sign(asteroid->direction_y) * Y_CENTER + Y_CENTER;

	} else {

		asteroid->x = -sign(asteroid->direction_x) * X_CENTER + X_CENTER;
		asteroid->y = -dist_x * asteroid->direction_y + Y_CENTER;

	}
}

static int check_collisions(){
	int game_over = 0;
	int i, j, deleted;
	float rel_x, rel_y, center_distance, radius_sum;
	// char hit = 0x15;

	for (i = 0; i < MAX_ASTEROID; ++i) {

		deleted = 0;

		if (asteroids[i]->gfx->visible) {
			
			for (j = 0; j < MAX_BULLET; ++j) {

				if (bullets[j]->gfx->visible) {

					rel_x = asteroids[i]->x - bullets[j]->x;
					rel_y = asteroids[i]->y - bullets[j]->y;

					center_distance = sqrt(rel_y * rel_y + rel_x * rel_x);
					radius_sum = asteroids[i]->radius + bullets[j]->radius;

					if (center_distance <= radius_sum) {
						
						cont(1002);

						// make_beep();

						// write(sound_fd, &hit, 1);

						asteroid_t* asteroid = asteroids[i];
						bullet_t* bullet = bullets[j];

						asteroid->gfx->yc = Y_CENTER*3;
						asteroid->gfx->xc = X_CENTER*3;

						bullet->gfx->yc = Y_CENTER*3;
						bullet->gfx->xc = X_CENTER*3;

						ellipse_tick(ctx, asteroid->gfx);
						ellipse_tick(ctx, bullet->gfx);

						asteroids[i]->gfx->visible = 0;
						bullets[j]->gfx->visible = 0;

						// free(asteroid->gfx);
						// free(bullet->gfx);
						// free(asteroid);
						// free(bullet);
						deleted++;
					}
				}
			}

			if (!deleted) {

				rel_x = asteroids[i]->x - X_CENTER;
				rel_y = asteroids[i]->y - Y_CENTER;

				center_distance = sqrt(rel_y * rel_y + rel_x * rel_x);
				radius_sum = asteroids[i]->radius + SHIP_RADIUS;

				if (center_distance <= radius_sum) {
					game_over = 1;
					game_over_fn();
				}
			}
		}
	}
	return game_over;
}

void game_over_fn() {
	int i;
	bullet_t* bullet;
	asteroid_t* asteroid;

	for (i = 0; i < MAX_BULLET; i++ ) {
		bullet = bullets[i];

		if (bullet->gfx->visible) {
			bullet->gfx->yc = Y_CENTER*3;
			bullet->gfx->xc = X_CENTER*3;

			ellipse_tick(ctx, bullet->gfx);

			bullet->gfx->visible = 0;
		}
	}

	for (i = 0; i < MAX_ASTEROID; i++ ) {
		asteroid = asteroids[i];

		if (asteroid->gfx->visible) {
			asteroid->gfx->yc = Y_CENTER*3;
			asteroid->gfx->xc = X_CENTER*3;

			ellipse_tick(ctx, asteroid->gfx);

			asteroid->gfx->visible = 0;
		}
	}

	example_bmp->x = X_CENTER*3;
	example_bmp->y = Y_CENTER*3;

	bmp_tick(ctx, example_bmp);

	example_bmp->visible = 0;
}
