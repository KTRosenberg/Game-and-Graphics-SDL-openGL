#include "hello.hpp"

// control the insignia with the mouse

int main(int argc, char* args[])
{
    // window to which to render
    SDL_Window* window = nullptr;
    // renderer
    SDL_Renderer* rend = nullptr;
    // path to image to render
    const char* IMG_PATH = "./ktr_insignia.bmp";
    // pointer to image to load
    SDL_Surface* bmp = nullptr;
    // loaded texture using renderer
    SDL_Texture* tex = nullptr;
    // the rectangle for collision
    SDL_Rect target;

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        sdl_quit_on_error_gracefully("SDL could not initialize");
    }
    // create the window
    if (!(window = SDL_CreateWindow("The Moving KTR Insignia",
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
    // load the image
    if (!(bmp = SDL_LoadBMP(IMG_PATH)))
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Image could not be loaded");
    }
    // get a texture
    if (!(tex = SDL_CreateTextureFromSurface(rend, bmp)))
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Texture could not be created from surface");
    }

    // free the surface (no longer needed)
    SDL_FreeSurface(bmp);
    
    // disable the cursor
    SDL_ShowCursor(SDL_DISABLE);

    // define target
    target.w = (SCREEN_WIDTH/2)/IMG_PROPORTION;
    const int OFFSET_W = target.w/2;
    target.h = (SCREEN_HEIGHT/2)/IMG_PROPORTION;
    const int OFFSET_H = target.h/2;
    
    target.x = (SCREEN_WIDTH/2) - (target.w/2);
    target.y = (SCREEN_HEIGHT/2)- (target.h/2);
    
    SDL_Event event;
    int mouse_inside_window = 0;
    int keep_running = 1;
    // main loop
    while (keep_running)
    {
        while (SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT) 
            {
                keep_running = false;
                break;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_ENTER)
                {
                    mouse_inside_window = 1;
                }
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE)
                {
                    mouse_inside_window = 0;
                }
            }

            if (mouse_inside_window)
            {
                int mouse_x = 0;
                int mouse_y = 0;
                SDL_GetMouseState(&mouse_x, &mouse_y);
                
                target.x = mouse_x - OFFSET_W;
                target.y = mouse_y - OFFSET_H;
            
                if (target.x < 0)
                {
                    target.x = 0;
            
                    #if (PRINTOUT)
                    puts("OUT_LEFT");
                    #endif
                }
                else if (target.x + target.w >= SCREEN_WIDTH)
                {
                    target.x = SCREEN_WIDTH - target.w;
            
                    #if (PRINTOUT)
                    puts("OUT_RIGHT");
                    #endif

                }
                if (target.y < 0)
                {
                    target.y = 0;
                
                    #if (PRINTOUT) 
                    puts("OUT_UP");
                    #endif
                }
                else if (target.y + target.h >= SCREEN_HEIGHT)
                {
                    target.y = SCREEN_HEIGHT - target.h;
            
                    #if (PRINTOUT)
                    puts("OUT_DOWN");
                    #endif
                }
            }
        
            #if (PRINTOUT)
            printf("\tACTUAL: %d %d\n", target.x, target.y);
            printf("\tOFFSETS: %d %d\n", 
                   target.x + target.w, target.y + target.h);
            #endif

            // clear renderer
            SDL_RenderClear(rend);
            // draw texture
            SDL_RenderCopy(rend, tex, nullptr, &target);
            // update screen
            SDL_RenderPresent(rend);
        }
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();

    exit(EXIT_SUCCESS);
}

// OLD : image automatically moves to the right

/*
int main(int argc, char* args[])
{
    // window to which to render
    SDL_Window* window = nullptr;
    // surface for the window
    SDL_Surface* screen_surface = nullptr;
    
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        sdl_quit_on_error_gracefully("SDL could not initialize");
    }
    // create the window
    if (!(window = SDL_CreateWindow("Hello SDL World", 
                                    SDL_WINDOWPOS_UNDEFINED, 
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN)))
    {
        sdl_quit_on_error_gracefully("Window could not be created");
    }
    
    // initialize the surface
    screen_surface = SDL_GetWindowSurface(window);
    // fill blue
    SDL_FillRect(screen_surface, nullptr, 
                 SDL_MapRGB(screen_surface->format, 0x87, 0xCE, 0xEB));
    
    // reflect changes
    SDL_UpdateWindowSurface(window);
    
    // keep window alive
    SDL_Delay(WAIT_MILLI);
    
    // delete window
    SDL_DestroyWindow(window);
    
    // quit SDL
    SDL_Quit();
    
    return 0;
}
*/

// OLD: static image

/*
int main(int argc, char* args[])
{
    // window to which to render
    SDL_Window* window = nullptr;
    // renderer
    SDL_Renderer* rend = nullptr;
    // path to image to render
    const char* IMG_PATH = "./ktr_insignia.bmp";
    // pointer to image to load
    SDL_Surface* bmp = nullptr;
    // loaded texture using renderer
    SDL_Texture* tex = nullptr;

    SDL_Rect target;

    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        sdl_quit_on_error_gracefully("SDL could not initialize");
    }
    // create the window
    if (!(window = SDL_CreateWindow("Hello SDL World",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SCREEN_WIDTH, SCREEN_HEIGHT,
                                    SDL_WINDOW_SHOWN)))
    {
        sdl_quit_on_error_gracefully("Window could not be created");
    }
    // get a renderer
    if (!(rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
    {
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Renderer could not be created");
    }
    // load the image
    if (!(bmp = SDL_LoadBMP(IMG_PATH)))
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Image could not be loaded");
    }
    // get a texture
    if (!(tex = SDL_CreateTextureFromSurface(rend, bmp)))
    {
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(window);
        sdl_quit_on_error_gracefully("Texture could not be created from surface");
    }

    // free the surface (no longer needed)
    SDL_FreeSurface(bmp);

    // define target
    target.w = (SCREEN_WIDTH/2)/IMG_PROPORTION;
    target.h = (SCREEN_HEIGHT/2)/IMG_PROPORTION;
    target.x = (SCREEN_WIDTH/2) - (target.w/2);
    target.y = (SCREEN_HEIGHT/2)- (target.h/2);

    const int MOVE_AMOUNT = 1;

    SDL_Event event;
    while (1)
    {
        SDL_PollEvent(&event);

        if (event.type == SDL_QUIT) break;

        // clear renderer
        SDL_RenderClear(rend);
        // draw texture
        SDL_RenderCopy(rend, tex, nullptr, &target);
        // update screen
        SDL_RenderPresent(rend);

        target.x += MOVE_AMOUNT;
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(window);
    SDL_Quit();

    exit(EXIT_SUCCESS);
}
*/
