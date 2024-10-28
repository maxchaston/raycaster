#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

SDL_Window* window;
SDL_Renderer* renderer;

#define MOVE_SPEED 0.1
#define ROTATION_SPEED 5
/* #define WINDOW_X 1504 */
/* #define WINDOW_Y 2256 */
float WINDOW_X = 800;
float WINDOW_Y = 800;

#define MAP_X 20
#define MAP_Y 20

#define FOV 90.0
#define RENDER_LIMIT 5


/**
	 , , , , , , , , , , , , , , , , , , , ,
	 , , , , , , , , , , , , , , , , , , , ,
	 , , , , , , , , ,#,#,#,#,#,#,#, , , , ,
	 , , , , , , , ,#, , , , , , ,#, , , , ,
	 , , , , , , ,#, , , , , , , ,#, , , , ,
	 , , , ,#,#,#, , , , , , , , ,#, , , , ,
	 , , , ,#, , , , , , , , , , ,#, , , , ,
	 , , ,#, , , , , , , , , , , ,#, , , , ,
	 , , ,#, , , , , , , , , , , ,#, , , , ,
	 , , ,#, , , , , , , , , , , ,#, , , , ,
	 , , ,#, , , , , , , , , , , ,#, , , , ,
	 , , , ,#, , , , , , , , , , ,#, , , , ,
	 , , , , ,#, , , , , , , , ,#, , , , , ,
	 , , , , ,#, , , , , , , ,#, , , , , , ,
	 , , , , ,#, , , , , , ,#, , , , , , , ,
	 , , , , ,#, , , , , ,#, , , , , , , , ,
	 , , , , ,#,#,#,#,#,#, , , , , , , , , ,
	 , , , , , , , , , , , , , , , , , , , ,
	 , , , , , , , , , , , , , , , , , , , ,
	 , , , , , , , , , , , , , , , , , , , 
 **/
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

// Data structures ///////////////////////////////////////////////////////////////////////////////////////////

typedef struct player_state {
	float x; // x and y are based on map coords
	float y;
	float angle; // angle in degrees
} player_state;
player_state p_s;

typedef struct position {
	float x;
	float y;
} position;

typedef struct raycast_ret {
	float distance;
	bool side;
} raycast_ret;

// Functions ////////////////////////////////////////////////////////////////////////////////////////////////
// setup and teardown ///////////////////////////////////////////////////////////////////////////////////////
void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("raycasting", SDL_WINDOWPOS_UNDEFINED,
														SDL_WINDOWPOS_UNDEFINED,
														WINDOW_X,
														WINDOW_Y,
														SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
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

// util functions ///////////////////////////////////////////////////////////////////////////////////////////
float deg_to_rad(float deg)
{
	return deg/360*2*M_PI;
}

float angle_calc(float angle, float change)
{
	return fmod(angle + change+360, 360);
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
		new_angle = angle_calc(p_s.angle, -ROTATION_SPEED);
		p_s.angle = new_angle;
		break;
	case 4: // clockwise rotation
		new_angle = angle_calc(p_s.angle, ROTATION_SPEED);
		p_s.angle = new_angle;
		break;
	}
}

float sind(float angle)
{
	return sinf(deg_to_rad(angle));
}

float cosd(float angle)
{
	return cosf(deg_to_rad(angle));
}

float tand(float angle)
{
	return tanf(deg_to_rad(angle));
}

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

position pos2win(position pos)
{
	float win_x = pos.x * (float)WINDOW_X/MAP_X;
	float win_y = WINDOW_Y - pos.y*(float)WINDOW_Y/MAP_Y;
	return (position){win_x,win_y};
}

void draw_line_pos(position p1, position p2)
{
	position p1w = pos2win(p1);
	position p2w = pos2win(p2);
	SDL_RenderDrawLine(renderer, p1w.x, p1w.y, p2w.x, p2w.y);
}

void draw_box_pos(position pos, float size)
{
	pos.x-=size/2;
	pos.y+=size/2;
	position box_pos_win = pos2win(pos);
	SDL_Rect rect = {box_pos_win.x, box_pos_win.y, size*WINDOW_X/MAP_X, size*WINDOW_Y/MAP_Y};
	SDL_RenderFillRect(renderer, &rect);
}

// rendering functions ////////////////////////////////////////////////////////////////////////////////////

raycast_ret raycast(position pos, float angle)
{
	bool ray_collision = false;
	float ray_length = 0;
	float gradient;
	if (angle==0)
		angle = 0.0001;
	if (angle==180)
		angle = 180.001;
	/* if (angle==0 || angle==180) */
	/* 	gradient = 9999; // hacky workaround */
	/* else */
	gradient = cosd(angle)/sind(angle);;
	float c = pos.y - gradient*pos.x;

	/* while (ray_length<RENDER_LIMIT) */
	while (true)
	{
		bool on_x = (pos.x == floor(pos.x));
		bool on_y = (pos.y == floor(pos.y));
		if (on_x || on_y) // on a line
		{
			SDL_SetRenderDrawColor(renderer, 160, 32, 240, 255); // GREEN
			/* draw_box_pos(pos, 0.2); */
			position map_pos;

			if (on_x)
				map_pos = (position){(int)pos.x - ((angle>180) ? 1 : 0), (int)pos.y+1};
			if (on_y)
				map_pos = (position){(int)pos.x, (int)pos.y + ((angle>270 || angle<90) ? 1 : 0)};

			if (map[(int)map_pos.x + (MAP_Y-(int)map_pos.y)*MAP_X])
			{
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					map_pos.x+=0.5;
					map_pos.y-=0.5;
					/* draw_box_pos(map_pos, 0.2); */
					return (raycast_ret){ray_length, on_x};
			}
		}

		float testing_x_x = (angle<=180) ? floor(pos.x+1) : ceil(pos.x-1);
		float testing_y_y = (angle>270 || angle<90) ? floor(pos.y+1) : ceil(pos.y-1);

		float testing_x_y = testing_x_x*gradient + c;
		float testing_x_dist = sqrtf(pow(testing_x_x - pos.x, 2) + pow(testing_x_y - pos.y, 2));

		float testing_y_x = (testing_y_y - c)/gradient;
		float testing_y_dist = sqrt(pow(testing_y_x - pos.x, 2) + pow(testing_y_y - pos.y, 2));

		if (testing_x_dist<testing_y_dist) // hit horizontal line first
		{
			pos.x = testing_x_x;
			pos.y = testing_x_y;
			ray_length += testing_x_dist;
		} else { // hit vertical line first
			pos.x = testing_y_x;
			pos.y = testing_y_y;
			ray_length += testing_y_dist;
		}
	}
	return (raycast_ret){-1, false};
}

void clear_screen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}

void draw_2d()
{
	// map blocks
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // GREEN
	for (int x=0; x<MAP_X; x++)
	{
		for (int y=0; y<MAP_Y; y++)
		{
			position block_win_pos = pos2win((position){x, y});
			if (map[x+((MAP_Y-y)*MAP_X)])
			{
				SDL_Rect map_rect = {block_win_pos.x, block_win_pos.y, WINDOW_X/MAP_X, WINDOW_Y/MAP_Y};
				SDL_RenderFillRect(renderer, &map_rect);
			}
		}
	}

	// rays
	int ray_num=100;
	for (int r=0; r<ray_num; r++) // iterating through rays
	{
		float ray_angle = angle_calc(p_s.angle-(FOV/2), FOV/(ray_num-1)*r);
		position ray1_win_pos = pos2win((position){p_s.x, p_s.y});
		/* position ray2_win_pos = pos2win((position){p_s.x + sind(ray_angle)*RENDER_LIMIT, p_s.y + cosd(ray_angle)*RENDER_LIMIT}); */
		position ray2_win_pos = pos2win((position){p_s.x + sind(ray_angle)*raycast((position){p_s.x, p_s.y}, ray_angle).distance, p_s.y + cosd(ray_angle)*raycast((position){p_s.x, p_s.y}, ray_angle).distance});
		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // BLUE
		SDL_RenderDrawLine(renderer, ray1_win_pos.x, ray1_win_pos.y, ray2_win_pos.x, ray2_win_pos.y);
	}

	// player
	position player_win_pos = pos2win((position){p_s.x, p_s.y});
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // RED
	float player_rect_width = 20.0;
	SDL_Rect player_rect = {p_s.x * (WINDOW_X/MAP_X) - (player_rect_width/2), WINDOW_Y - p_s.y * (WINDOW_Y/MAP_Y) - (player_rect_width/2), player_rect_width, player_rect_width};
	SDL_RenderFillRect(renderer, &player_rect);
	/* position player_dir_line_win_pos = pos2win((position){p_s.x + sind(p_s.angle), p_s.y + cosd(p_s.angle)}); */
	/* SDL_RenderDrawLine(renderer, player_win_pos.x, player_win_pos.y, player_dir_line_win_pos.x, player_dir_line_win_pos.y); */

	// grid
	SDL_SetRenderDrawColor(renderer, 50, 50, 50, 0); // grey
	for (int x=0; x<MAP_X; x++)
	{
		position x1 = (position){x,0};
		position x2 = (position){x,MAP_Y};
		draw_line_pos(x1, x2);
	}
	for (int y=0; y<MAP_Y; y++)
	{
		position y1 = (position){0,y};
		position y2 = (position){MAP_X,y};
		draw_line_pos(y1, y2);
	}
}

// event handling /////////////////////////////////////////////////////////////////////////////////////////
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
		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				int w = 0;
				int h = 0;
				SDL_GetWindowSize(window, &w, &h);
				WINDOW_X = (float)w;
				WINDOW_Y = (float)h;
			}
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

void draw_3d()
{
	for (int r=0; r<WINDOW_X; r++) // iterating through rays
	{
		float ray_angle = angle_calc(p_s.angle-(FOV/2), FOV/(WINDOW_X-1)*r);
		raycast_ret ret = raycast((position){p_s.x, p_s.y}, ray_angle);
		float distance = ret.distance;
		SDL_SetRenderDrawColor(renderer, 0, 0, (ret.side) ? 255 : 100, 255); // BLUE
		if (distance != -1)
			SDL_RenderDrawLine(renderer, r, WINDOW_Y/2 - 1/(distance/3)*WINDOW_X/MAP_X, r, WINDOW_Y/2 + 1/(distance/3)*WINDOW_Y/MAP_Y);
	}
}

int main()
{
	init();
	int run_flag=1;
	while(run_flag)
	{
		handle_events(&run_flag);
		clear_screen();
		draw_2d();
		draw_3d();
		/* draw_minimap(true); */
		SDL_RenderPresent(renderer);
		/* printf("x:%f\ty:%f\tangle:%f\r", p_s.x, p_s.y, p_s.angle); */
		/* fflush(stdout); */
		SDL_Delay(1000/60);
	}
	teardown();
	return 0;
}
