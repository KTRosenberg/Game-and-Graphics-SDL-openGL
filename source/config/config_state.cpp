#ifndef WORLD_STATE_CONFIG_HPP
    #define WORLD_STATE_CONFIG_HPP

    #if (RELEASE_MODE == 1)
        #define IS_CONSTANT const
    #else
        #define IS_CONSTANT
    #endif

    #define configure_begin(namespace__) namespace namespace__ {
    #define configure_end() }

    struct ConfigVar {
        PROPERTY_TYPE type;
        const char* name;
        const char* name_space;
        union {
            PROPERTY_KINDS
        };
        void* ptr;
    };

    // template <typename T>
    // inline void ConfigVar_modify(T val);
    // template <typename T>
    // inline T ConfigVar_access(void);

    #define configure_var(type__, namespace__, name__, default_value__) \
        configure_begin(namespace__) \
        extern IS_CONSTANT type__ name__; \
        configure_end() \

    #define configure_var_mutable(type__, namespace__, name__, default_value__) \
        configure_begin(namespace__) \
        extern type__ name__; \
        configure_end() \

    #include "./config_movement.cpp"
    #undef configure_var
    #undef configure_var_mutable

#endif


#ifdef WORLD_STATE_CONFIG_IMPLEMENTATION
    #undef WORLD_STATE_CONFIG_IMPLEMENTATION


    #define configure_var(type__, namespace__, name__, default_value__) \
        configure_begin(namespace__) \
        IS_CONSTANT type__ name__ = default_value__; \
        configure_end() \

    #define configure_var_mutable(type__, namespace__, name__, default_value__) \
        configure_begin(namespace__) \
        type__ name__ = default_value__; \
        configure_end() \

    #include "./config_movement.cpp"
    #undef configure_var
    #undef configure_var_mutable

    #undef configure_begin
    #undef configure_end

    #if (RELEASE_MODE == 0)
        #define configure_begin(namespace__)
        #define configure_end()

        #define configure_var(type__, namespace__, name__, default_value__) \
            {PROPERTY_TYPE::PROP_##type__, #name__, #namespace__, .f_##type__ = default_value__, (void*)&namespace__::name__},

        #define configure_var_mutable(type__, namespace__, name__, default_value__) \
            {PROPERTY_TYPE::PROP_##type__, #name__, #namespace__, .f_##type__ = default_value__, (void*)&namespace__::name__},

        ConfigVar config_state[] = {
            #include "./config_movement.cpp"
        };

        #undef configure_var
        #undef configure_var_mutable

        #undef configure_begin
        #undef configure_end
    #else
        ConfigVar config_state[] = {};        
    #endif

#endif