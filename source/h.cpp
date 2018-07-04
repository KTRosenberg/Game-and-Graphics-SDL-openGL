# 1 "config/config_state.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 175 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "config/config_state.cpp" 2
# 13 "config/config_state.cpp"
    struct ConfigVar {
        PROPERTY_TYPE type;
        const char* name;
        const char* name_space;
        union {
            PROPERTY_KINDS
        };
        void* ptr;

        template <typename T>
        inline void ConfigVar_modify(T val)
        {

        }






    };
# 50 "config/config_state.cpp"
# 1 "config/./config_movement.cpp" 1
# 11 "config/./config_movement.cpp"
namespace wee { extern f64 WEEf64; }
namespace wee { extern i32 WEEi32; }
namespace ha { extern char HAchar; }


namespace physics { extern f64 GRAVITY_DEFAULT; }
namespace physics { extern f64 GRAVITY_TERRESTRIAL; }
namespace physics { extern f64 GRAVITY_OUTER_SPACE; }
namespace physics { extern f64 gravity; }
# 51 "config/config_state.cpp" 2

