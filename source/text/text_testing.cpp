#include <unordered_map>
struct hash_ptrstr {
    size_t operator()(const char *s) const
    {
        // http://www.cse.yorku.ca/~oz/hash.html
        size_t h = 5381;
        int c;
        while ((c = *s++))
            h = ((h << 5) + h) + c;
        return h;
    }
};

struct cmp_ptrstr {
    bool operator()(char const * const a, char const * const b) const
    {
        return strcmp(a, b) < 0;
    }
};
typedef std::unordered_map<
    const char* const, usize, hash_ptrstr, cmp_ptrstr
> Map_Str2ID;

struct Font_Info {

};
struct Glyph_Info {
    uint16  id;
   
    float64 x_tex_coord;
    float64 y_tex_coord;
    float64 x_max_tex_coord;
    float64 y_max_tex_coord;

    float64 x_offset;
    float64 y_offset;

    float64 size_x;
    float64 size_y;

    float64 x_advance;
};

void Glyph_Info_init(Glyph_Info* glyph_info, 
    u16 id, float64 x_tex_coord, float64 y_tex_coord,
    float64 x_tex_size, float64 y_tex_size,
    float64 x_offset, float64 y_offset,
    float64 size_x, float64 size_y,
    float64 x_advance
) {
    glyph_info->id = id;
    glyph_info->x_tex_coord     = x_tex_coord;
    glyph_info->y_tex_coord     = y_tex_coord;
    glyph_info->x_offset        = x_offset;
    glyph_info->y_offset        = y_offset;
    glyph_info->size_x          = size_x;
    glyph_info->size_y          = size_y;
    glyph_info->x_max_tex_coord = x_tex_size + x_tex_coord;
    glyph_info->y_max_tex_coord = y_tex_size + y_tex_coord;
    glyph_info->x_advance       = x_advance;
}

typedef u16 Font_ID;

enum struct SYSTEM_FONT_NAME : u16 {
    CALIBRI,
    SYSTEM_FONT_COUNT
};
struct Font_Catalogue {
    Array<Font_Info, 8> fonts;
    Font_ID             count;
    Map_Str2ID          font_map;
};
void Font_Catalogue_init(Font_Catalogue* font_Catalogue) {
    font_Catalogue->count = 0;
    Array_init_to_cap(&font_Catalogue->fonts);
}
Font_ID Font_Catalogue_load(Font_Catalogue* font_Catalogue, char* font_name) {
    const Font_ID next_ID = font_Catalogue->count;
    font_Catalogue->count += 1;

    // initialize font here
    font_Catalogue->font_map.insert(
        {font_name, next_ID}
    );

    return next_ID;
}
struct Rectangle_Extent {
	int w;
	int h;
};
struct Core {
	Rectangle_Extent screen;
	bool is_running;
	input_sys::Input input;
	Font_Catalogue text;
} core;

void text_render() {
	Font_Catalogue* text = &core.text;

	
}

#ifdef USE_METAL
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#endif

int text_testing_main(int argc, char* argv[])
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) < 0) {
        fprintf(stderr, "%s\n", "SDL could not initialize");
        return EXIT_FAILURE;
    }

    #ifdef USE_METAL
    return EXIT_SUCCESS;
    #endif

    // OpenGL initialization ///////////////////////////////////////////////////
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    core.screen.w = SCREEN_WIDTH;
	core.screen.h = SCREEN_HEIGHT;

    // create the window
    if (NULL == (window = SDL_CreateWindow(
        WINDOW_HEADER,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        core.screen.w, core.screen.h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)))
    {
        fprintf(stderr, "Window could not be created\n");
        return EXIT_FAILURE;
    }

    window_state.focused = true;
    window_state.minimized = false;

    //SDL_SetWindowFullscreen(window, true);
    
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
      fprintf(stderr, "SDL_image could not initialize, SDL_image Error: %s\n", IMG_GetError());
        SDL_DestroyWindow(window);
        return EXIT_FAILURE;
    }
    
    program_data.context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();



    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, core.screen.w, core.screen.h);

    #ifdef SDL_H
    {
        SDL_version compiled;
        SDL_version linked;

        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        
        printf(
            "COMPILED AGAINST SDL VERSION %d.%d.%d.\n",
            compiled.major, compiled.minor, compiled.patch
        );
        printf(
            "LINKED AGAINST SDL VERSION %d.%d.%d.\n",
            linked.major, linked.minor, linked.patch
        );
    }
    #endif

    printf("USING GL VERSION: %s\n", glGetString(GL_VERSION));

    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        printf("window size=[%d,%d]\n", w, h);
    }


    input_sys::init(&core.input);

    #define CONTROLLER_MAPPING_FILE "./mapping/gamecontrollerdb.txt"
    if (SDL_GameControllerAddMappingsFromFile(CONTROLLER_MAPPING_FILE) < 0) {
        fprintf(stderr, "FAILED TO LOAD CONTROLLER MAPPINGS FROM %s\n", CONTROLLER_MAPPING_FILE);
    }

    mat4 mat_ident(1.0f);
    mat4 mat_projection = glm::ortho(
        0.0f, 
        1.0f * core.screen.w, 
        1.0f * core.screen.h,
        0.0f,
        0.0f, 
        1.0f * 10.0f
    );

    core.is_running = true;

    Font_Catalogue_init(&core.text);

    SDL_Event event;
    while (core.is_running) {
        // INPUT /////////////////////////////////
        if (!poll_input_events(&core.input, &event)) {
            core.is_running = false;
            continue;
        } else if (window_state.minimized) {
            continue;
        } else if (window_state.restored) {
            window_state.restored = false;
            SDL_GL_SwapWindow(window);
            continue;
        }

        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        text_render();

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(program_data.context);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

	return 0;
}