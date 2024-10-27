#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window* window;
SDL_Renderer* renderer;

#define MOVE_SPEED 0.1
#define ROTATION_SPEED 5
#define WINDOW_X 800
#define WINDOW_Y 800

#define MAP_X 20
#define MAP_Y 20

#define FOV 90

float sind(float angle);
float cosd(float angle);
float tand(float angle);

float fix_zero(float num)
{
	if (num==0)
	{
		return 0.00000001;
	} else
	{
		return num;
	}
}


int map[MAP_X*MAP_Y] = 
{
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
	0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,
	0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
	0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

typedef struct player_state {
	float x; // x and y are based on map coords
	float y;
	float angle; // angle in rads
} player_state;
player_state p_s;

typedef struct position {
	float x;
	float y;
} position;

void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("doomv2", SDL_WINDOWPOS_UNDEFINED,
														SDL_WINDOWPOS_UNDEFINED,
														WINDOW_X,
														WINDOW_Y,
														SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	p_s.x = MAP_X/2;
	p_s.y = MAP_Y/2;
	p_s.angle = 0;
}

void teardown()
{
	SDL_DestroyWindow(window);
}

float deg_to_rad(float deg)
{
	return deg/360*2*M_PI;
}

void modify_player_state(int move_type)
{
	// 1: forward, 2: back, 3: counter-clockwise rotation, 4: clockwise rotation
	switch (move_type)
	{
		float new_x;
		float new_y;
		float new_angle;
	case 1: // forward
		new_x = p_s.x + (sin(deg_to_rad(p_s.angle))*MOVE_SPEED);
		new_y = p_s.y + (cos(deg_to_rad(p_s.angle))*MOVE_SPEED);
		p_s.x = new_x;
		p_s.y = new_y;
		break;
	case 2: // backwards
		new_x = p_s.x - (sin(deg_to_rad(p_s.angle))*MOVE_SPEED);
		new_y = p_s.y - (cos(deg_to_rad(p_s.angle))*MOVE_SPEED);
		p_s.x = new_x;
		p_s.y = new_y;
		break;
	case 3: // counter-clockwise rotation
		new_angle = fmod(p_s.angle - ROTATION_SPEED+360, 360);
		p_s.angle = new_angle;
		break;
	case 4: // clockwise rotation
		new_angle = fmod(p_s.angle + ROTATION_SPEED+360, 360);
		p_s.angle = new_angle;
		break;
	}
}

position raycast(position pos, float angle)
{
}


void handle_events(int *run_flag)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{	
		case SDL_QUIT:
			*run_flag=0;
			break;
		}
	}

	// poll keyboard
	// needs to be ran after events processed
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (keystates[SDL_SCANCODE_W])
		modify_player_state(1);
	if (keystates[SDL_SCANCODE_S])
		modify_player_state(2);
	if (keystates[SDL_SCANCODE_A])
		modify_player_state(3);
	if (keystates[SDL_SCANCODE_D])
		modify_player_state(4);
}

void clear_screen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}

void draw_2d(bool fixed, bool raycasting)
{
	if (fixed)
	{
		// map
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		for (int x=0; x<MAP_X; x++)
		{
			for (int y=0; y<MAP_Y; y++)
			{
				if (map[x+(y*MAP_X)])
				{
					SDL_Rect map_rect = {WINDOW_X/MAP_X * x, WINDOW_Y/MAP_Y * y, WINDOW_X/MAP_X, WINDOW_Y/MAP_Y};
					SDL_RenderFillRect(renderer, &map_rect);
				}
			}
		}
		// player
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		float player_rect_width = 20;
		SDL_Rect player_rect = {p_s.x * (WINDOW_X/MAP_X) - (player_rect_width/2), WINDOW_Y - p_s.y * (WINDOW_Y/MAP_Y) - (player_rect_width/2), player_rect_width, player_rect_width};
		SDL_RenderFillRect(renderer, &player_rect);
		SDL_RenderDrawLine(renderer, p_s.x * (WINDOW_X/MAP_X), WINDOW_Y - p_s.y * (WINDOW_Y/MAP_Y), p_s.x * (WINDOW_X/MAP_X) +(sin(deg_to_rad(p_s.angle))*30), WINDOW_Y - (p_s.y * (WINDOW_Y/MAP_Y)  + cos(deg_to_rad(p_s.angle))*30));
		if (raycasting)
		{
			// raycast lines
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			int raycast_line_count = 45;
			for (int i=0; i<raycast_line_count; i++)
			{
				float ray_angle = fmod(p_s.angle - (FOV/2) + (FOV/raycast_line_count*i)+360, 360);
				position raycast_pos = {p_s.x, p_s.y};
				position ray_terminus_pos = raycast(raycast_pos, ray_angle);
				SDL_RenderDrawLine(renderer, p_s.x * (WINDOW_X/MAP_X), WINDOW_Y - p_s.y * (WINDOW_Y/MAP_Y), ray_terminus_pos.x * (WINDOW_X/MAP_X), WINDOW_Y - ray_terminus_pos.y * (WINDOW_Y/MAP_Y));
			}
		}
	} else {
		// map
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		for (int x=0; x<MAP_X; x++)
		{
			for (int y=0; y<MAP_Y; y++)
			{
				if (map[x+(y*MAP_X)])
				{
					SDL_Rect map_rect = {WINDOW_X/MAP_X * x + (400 - p_s.x*(WINDOW_X/MAP_X)), WINDOW_Y/MAP_Y * y + (400-p_s.y*(WINDOW_Y/MAP_Y)), WINDOW_X/MAP_X, WINDOW_Y/MAP_Y};
					SDL_RenderFillRect(renderer, &map_rect);
				}
			}
		}
		// player
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		float player_rect_width = 20;
		SDL_Rect player_rect = {WINDOW_X/2-player_rect_width/2, WINDOW_Y/2-player_rect_width/2, player_rect_width, player_rect_width};
		SDL_RenderFillRect(renderer, &player_rect);
		SDL_RenderDrawLine(renderer, WINDOW_X/2, WINDOW_Y/2, WINDOW_X/2+(sin(deg_to_rad(p_s.angle))*30), WINDOW_Y/2-cos(deg_to_rad(p_s.angle))*30);
	}
}

void draw_minimap(bool fixed)
{
	if (fixed)
	{
		// draw map
		// draw map box outline
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(renderer, &((SDL_Rect) {0, 0, WINDOW_X*0.2, WINDOW_Y*0.2}));
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		for (int x=0; x<MAP_X; x++)
		{
			for (int y=0; y<MAP_Y; y++)
			{
				if (map[x+(y*MAP_X)])
				{
					SDL_Rect map_rect = {WINDOW_X/MAP_X * x * 0.2, WINDOW_Y/MAP_Y * y * 0.2, WINDOW_X/MAP_X*0.2, WINDOW_Y/MAP_Y*0.2};
					SDL_RenderFillRect(renderer, &map_rect);
				}
			}
		}
		// draw player
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		float player_rect_width = 20*0.2;
		SDL_Rect player_rect = {p_s.x*(WINDOW_X/MAP_X)*0.2-(player_rect_width/2), WINDOW_Y*0.2 - p_s.y*(WINDOW_Y/MAP_Y)*0.2-(player_rect_width/2), player_rect_width, player_rect_width};
		SDL_RenderFillRect(renderer, &player_rect);
		SDL_RenderDrawLine(renderer, p_s.x * (WINDOW_X/MAP_X) * 0.2, WINDOW_Y * 0.2 - p_s.y * (WINDOW_Y/MAP_Y) * 0.2, p_s.x * (WINDOW_X/MAP_X)*0.2+(sin(deg_to_rad(p_s.angle))*40*0.2), WINDOW_Y * 0.2 - p_s.y * (WINDOW_Y/MAP_Y)*0.2-cos(deg_to_rad(p_s.angle))*40*0.2);
	} else {
		// TODO implement centred minimap drawing
	}
}

void draw_3d()
{
	// one ray cast per pixel on screen
	for (int i=0; i<WINDOW_X; i++)
	{
		float ray_angle = p_s.angle - (FOV/2) + (FOV/WINDOW_X*i);
	}
}

float sind(float angle)
{
	return sin(deg_to_rad(angle));
}

float cosd(float angle)
{
	return cos(deg_to_rad(angle));
}

float tand(float angle)
{
	return tan(deg_to_rad(angle));
}

int main()
{
	init();
	int run_flag=1;
	while(run_flag)
	{
		handle_events(&run_flag);
		clear_screen();
		draw_2d(true, true);
		/* draw_3d(); */
		draw_minimap(true);
		SDL_RenderPresent(renderer);
		/* printf("x:%f\ty:%f\tangle:%f\r", p_s.x, p_s.y, p_s.angle); */
		/* fflush(stdout); */
		SDL_Delay(1000/60);
	}
	teardown();
	return 0;
}
