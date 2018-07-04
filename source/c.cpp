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
    void* default_value;
    void* ptr;
};







# 1 "config/./config_movement.cpp" 1
# 11 "config/./config_movement.cpp"
namespace wee { extern f64 WEEf64; }
namespace wee { extern i32 WEEi32; }
namespace ha { extern char HAchar; }
# 27 "config/config_state.cpp" 2
# 42 "config/config_state.cpp"
# 1 "config/./config_movement.cpp" 1
# 11 "config/./config_movement.cpp"
namespace wee { f64 WEEf64 = 0.0; }
namespace wee { i32 WEEi32 = 1; }
namespace ha { char HAchar = '2'; }
# 43 "config/config_state.cpp" 2
# 54 "config/config_state.cpp"
ConfigVar config_state[] = {

# 1 "config/./config_movement.cpp" 1
# 11 "config/./config_movement.cpp"
{PROPERTY_TYPE::PROP_f64, "WEEf64", "wee", (void*)0.0, (void*)&wee::WEEf64},
{PROPERTY_TYPE::PROP_i32, "WEEi32", "wee", (void*)1, (void*)&wee::WEEi32},
{PROPERTY_TYPE::PROP_char, "HAchar", "ha", (void*)'2', (void*)&ha::HAchar},
# 56 "config/config_state.cpp" 2
};
