#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <cstdlib>
#include <cinttypes>
#include <string>

#define PRINTOUT 0

#define WAIT_MILLI 1000
#define TIME_UNIT     1

#define FADE_SPEED 0.1

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 256;

double sind(double angle)
{
    double angleradians = angle * M_PI / 180.0f;
    return sin(angleradians) * M_PI / 180.0f;
}

int ignore_mouse_movement(void* not_used, SDL_Event* event)
{
	return (event->type == SDL_MOUSEMOTION) ? 0 : 1;
}

void draw_bg_rect(SDL_Rect* fill_rect_ptr, SDL_Renderer* rend)
{
	// draw a background rectangle
	SDL_SetRenderDrawColor(rend, 0x00, 0x80, 0x00, 0xFF);
	SDL_RenderFillRect(rend, fill_rect_ptr);
}

void move_bg_rect(SDL_Rect& fill_rect)
{
    static uint8_t mov_right  = 1;
    static uint8_t mov_up     = 1;
    static const int mov_by = 4;
        
	if (mov_right)
	{
		fill_rect.x += mov_by;
		if (fill_rect.x + fill_rect.w >= SCREEN_WIDTH)
		{
			fill_rect.x = SCREEN_WIDTH - fill_rect.w;
			mov_right = !mov_right;
		}
	}
	else
	{
		fill_rect.x -= mov_by;
		if (fill_rect.x <= 0)
		{
			fill_rect.x = 0;
			mov_right = !mov_right;
		}
	}
	
	if (mov_up)
	{
		fill_rect.y -= mov_by;
		if (fill_rect.y <= 0)
		{
			fill_rect.y = 0;
			mov_up = !mov_up;
		}
	}
	else
	{
		fill_rect.y += mov_by;
		if (fill_rect.y + fill_rect.h >= SCREEN_HEIGHT)
		{
			fill_rect.y = SCREEN_HEIGHT - fill_rect.h;
			mov_up = !mov_up;
		}
	}
}

void sdl_quit_on_error_gracefully(std::string message)
{
	printf("%s: SDL_ERROR: %s\n", message.c_str(), SDL_GetError());
	SDL_Quit();
	exit(-1);
}

// credit for solution: 
// http://stackoverflow.com/questions/15752122/texture-fading-in-sdl2-using-sdl-image/15769305#15769305
void update_alpha(SDL_Texture* texture,
				  double elapsedTime,
				  uint8_t* alpha_ptr, float* alpha_calc_ptr)
{
	uint8_t alpha = 0;
	double alpha_calc = 0;
	static uint8_t from_transparent = 0;
	
    // Check there is a texture
    if (texture && alpha_ptr && alpha_calc_ptr)
    {
        // Set the alpha of the texture
        alpha = *alpha_ptr;
		alpha_calc = *alpha_calc_ptr;
        SDL_SetTextureAlphaMod(texture, alpha);
    }
    else
    {
    	return;
    }
    
    if (!from_transparent)
    {
    	// Update the alpha value
		if (alpha > SDL_ALPHA_TRANSPARENT) 
		{
			alpha_calc -= FADE_SPEED*elapsedTime;
			alpha = alpha_calc;
		}

		if (alpha <= SDL_ALPHA_TRANSPARENT) 
		{
			alpha = SDL_ALPHA_TRANSPARENT;
			alpha_calc = (float)SDL_ALPHA_TRANSPARENT;
			from_transparent = !from_transparent;
		}
    }
    else
    {
		// Update the alpha value
		if (alpha < SDL_ALPHA_OPAQUE) 
		{
			alpha_calc += FADE_SPEED*elapsedTime;
			alpha = alpha_calc;
		}

		if (alpha >= SDL_ALPHA_OPAQUE) 
		{
			alpha = SDL_ALPHA_OPAQUE;
			alpha_calc = (float)SDL_ALPHA_OPAQUE;
			from_transparent = !from_transparent;
		}
    }
    
    #if (PRINTOUT)
    printf("elapsedTime %f\n", elapsedTime);
    printf("INC:   %f\n", FADE_SPEED*elapsedTime);
    printf("ALPHA: %u\n", (unsigned int)alpha);
    printf("CALC : %f\n", alpha_calc);
    if (!from_transparent)
    {
    	puts("OPAQUE->TRANSPARENT");
    }
    else
    {
        puts("TRANSPARENT->OPAQUE");
    }
    #endif
    
    *alpha_ptr = alpha;
    *alpha_calc_ptr = alpha_calc;
}

int main(int argc, char* argv[])
{
    // window to which to render
    SDL_Window* window = nullptr;
    // renderer
    SDL_Renderer* rend = nullptr;
    // path to image to render
    const char* IMG_PATH = "sq.png";
    // pointer to image to load
    SDL_Surface* png = nullptr;
    // loaded texture using renderer
    SDL_Texture* tex = nullptr;
    // the rectangle for collision
    SDL_Rect target;
    double target_angle = 0.0;

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        sdl_quit_on_error_gracefully("SDL could not initialize");
    }
    // create the window
    if (!(window = SDL_CreateWindow("transparency test",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN)))
    {
        sdl_quit_on_error_gracefully("Window could not be created");
    }
    // get a renderer
    if (!(rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED 
                                                | SDL_RENDERER_PRESENTVSYNC)))
    {
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Renderer could not be created");
    }
    
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags))
    {
     	printf("SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        exit(EXIT_FAILURE);
    }
    // load the image
    if (!(png = IMG_Load(IMG_PATH)))
    {
        printf("SDL_image could not load %s, SDL_image Error: %s\n", IMG_PATH, IMG_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        exit(-1);
    }
    // get a texture
    if (!(tex = SDL_CreateTextureFromSurface(rend, png)))
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Texture could not be created from surface");
    }

    // free the surface (no longer needed)
    SDL_FreeSurface(png);
    
    // disable the cursor
    SDL_ShowCursor(SDL_DISABLE);
    // ignore mouse movement events
    SDL_SetEventFilter(ignore_mouse_movement, nullptr);
    
    // define target
    target.w = SCREEN_WIDTH;
    target.h = SCREEN_HEIGHT;    
    target.x = 0;
    target.y = 0;
    
	double angle_factor = 10.0;
    
    float fill_rect_scale = 0.5f;
    SDL_Rect fill_rect;
    fill_rect.w = (SCREEN_WIDTH/4 + 32)*fill_rect_scale;
    fill_rect.h = (SCREEN_HEIGHT/4)*fill_rect_scale;
    fill_rect.x = (SCREEN_WIDTH/2 - (fill_rect.w/2)) + SDL_GetTicks()%32 + 1;
    fill_rect.y = (SCREEN_HEIGHT/2 - (fill_rect.h/2)) + SDL_GetTicks()%32 + 1;
    uint8_t on_top = 0;

    
    
    uint8_t full = SDL_ALPHA_OPAQUE;
    float calc = full;
    uint8_t* alpha = &full;
    float* alpha_calc = &calc;
    
    uint32_t start_time = SDL_GetTicks();
    double elapsed = 0.0;
    int keep_running = 1;
    SDL_Event event;
    while (keep_running)
    {
        
        while (SDL_PollEvent(&event))
        {
        	if (event.type == SDL_QUIT) 
            {
                keep_running = false;
                break;
            }
    	}
    	
    	if (keep_running)
    	{
    		SDL_SetRenderDrawColor(rend, 0xFF, 0xFF, 0xFF, 0xFF);
			// clear renderer
			SDL_RenderClear(rend);
			
			if (!on_top)
			{
				draw_bg_rect(&fill_rect, rend);
			}
    	
    		update_alpha(tex, elapsed, alpha, alpha_calc);
			start_time = SDL_GetTicks();
			
			if (full == SDL_ALPHA_TRANSPARENT)
			{
				on_top = !on_top;
			}
			
			// draw texture
			//SDL_RenderCopy(rend, tex, nullptr, &target);
			SDL_RenderCopyEx(rend, tex, nullptr, &target, target_angle, nullptr, SDL_FLIP_NONE); 
			target_angle = (long)(target_angle + angle_factor + angle_factor*angle_factor*angle_factor*sind((double)((long)target_angle%180)))%360;

			if (on_top)
			{
				draw_bg_rect(&fill_rect, rend);
			}
			
			// update screen
			SDL_RenderPresent(rend);
					
			elapsed = (SDL_GetTicks() - start_time) / (double)(TIME_UNIT);
			
			move_bg_rect(fill_rect);

		}
    } 

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    
    IMG_Quit();
    SDL_Quit();

    exit(EXIT_SUCCESS);
}
