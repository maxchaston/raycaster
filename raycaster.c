#include <SDL2/SDL.h>
#include <math.h>
#define MAP_HEIGHT 20
#define MAP_WIDTH 20
#define MAP_SIZE MAP_HEIGHT*MAP_WIDTH
int map[400] = 
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

struct player_info
{
	float x;
	float y;
	float angle;
};

struct line_info
{
	float gradient;
	float y_intercept;
};

SDL_Window* window;
SDL_Renderer* renderer;
struct player_info pinfo;
void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Doom", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
			800,
			800,
			SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
}
void teardown()
{
	SDL_DestroyWindow(window);
}
void draw_2d_map()
{
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	for (int i=0; i<MAP_SIZE; i++)
	{
		if(map[i])
		{
			int map_x = i%MAP_WIDTH;
			int map_y = i/MAP_WIDTH;
			int draw_x = map_x*40;
			int draw_y = map_y*40;
			SDL_RenderFillRect(renderer, &((SDL_Rect){draw_x, draw_y, 40, 40}));
		}
	}
	SDL_RenderPresent(renderer);
}

void draw_2d_player()
{

}

float deg_to_rad(float deg_angle)
{
	return deg_angle*360/2*M_PI;
}

void clear_screen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
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
}
int main()
{
	pinfo.x=10;
	pinfo.y=10;
	pinfo.angle=0;
	init();
	int run_flag=1;
	while(run_flag)
	{
		clear_screen();
		draw_2d_map();
		handle_events(&run_flag);
	}
	teardown();
	return 0;
}
