#ifndef ROTOLOGIC_RENDERER
#define ROTOLOGIC_RENDERER

#if !(UNITY_BUILD)
    #include "common_utils.hpp"
    #include "sd.hpp"
#endif

namespace sd {

struct Render_Context {
    // TODO
    Array<sd::Render_Batch<0>, 128> render_batches;
    //Shader_Registry shaders;
};

}

#endif

