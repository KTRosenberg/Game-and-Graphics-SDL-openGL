#ifndef ROTOLOGIC_RENDERER
#define ROTOLOGIC_RENDERER

#if !(UNITY_BUILD)
    #include "common_utils.hpp"
    #include "sd.hpp"
#endif

struct Rendering_Context {
    // TODO
    //Dynamic_Array<sd::Render_Batch> render_layers;
    Shader_Registry shaders;
};

#endif

