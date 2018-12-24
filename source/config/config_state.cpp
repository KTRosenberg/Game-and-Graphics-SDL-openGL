#ifndef WORLD_STATE_CONFIG_HPP
    #define WORLD_STATE_CONFIG_HPP

    #include <stdlib.h>
    #include <stdio.h>

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
            #define KIND(a, b) b PASTE(f_, a) ;
            PROPERTY_KINDS
            #undef KIND
        };
        void* ptr;
    };

    void ConfigState_load_runtime(void);

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

        //#include <unordered_map>
        char* seek_arg_list_begin(char* buff, char* buff_end)
        {
            static char stack[16];
            char* stack_marker = &stack[1];
            char* stack_end = &stack[16];

            char* marker = buff;
            bool single_comment = false;
            bool multi_comment = false;
            while (marker != buff_end) {
                if (single_comment && *marker != '\0' && *marker != '\n') {
                    marker += 1;
                    continue;
                }

                bool do_print = true;

                switch (*marker) {
                case '\0':
                    single_comment = false;
                    marker += 1;
                    continue;
                    break;
                case '\n':
                    single_comment = false;
                    marker += 1;
                    continue;
                    break;
                case '/':
                    if (*(stack_marker - 1) == '/') {
                        single_comment = true;
                    } else {
                        *stack_marker = '/';
                        stack_marker += 1;
                    }
                    break;
                case '*':
                    break;
                case '+':
                    break;
                case '-':
                    break;
                case '(':
                    break;
                case ')':
                    break;
                case ',':
                    break;
                case ' ':
                case '\t':
                case '\r':
                    do_print = false;
                    break;
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                case 'g':
                case 'h':
                case 'i':
                case 'j':
                case 'k':
                case 'l':
                case 'm':
                case 'n':
                case 'o':
                case 'p':
                case 'q':
                case 'r':
                case 's':
                case 't':
                case 'u':
                case 'v':
                case 'w':
                case 'x':
                case 'y':
                case 'z':
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                case 'G':
                case 'H':
                case 'I':
                case 'J':
                case 'K':
                case 'L':
                case 'M':
                case 'N':
                case 'O':
                case 'P':
                case 'Q':
                case 'R':
                case 'S':
                case 'T':
                case 'U':
                case 'V':
                case 'W':
                case 'X':
                case 'Y':
                case 'Z':
                case '_':
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                case '#':
                    break;
                default:
                    if (!single_comment && !multi_comment) {
                        
                    }
                    break;
                }

                if (do_print) {
                    std::cout << *marker;
                }

                marker += 1;
            }
            assert(buff != buff_end);

            return nullptr;
        }

        char* parse_arg_list(char* buff)
        {
            return nullptr;
        }

        void ConfigState_load_runtime(void)
        {
            static char buff[512];

            char  aliases[512];
            char* aliases_indices[512];

            FILE* fp = fopen("./config/config_movement.cpp", "r");
            assert(fp != nullptr);

            while (fgets(buff, sizeof(buff), fp) != nullptr) {
                buff[511] = '\0'; // temp

                //std::cout << buff << std::endl;
                char* args_begin = seek_arg_list_begin(buff, &buff[strlen(buff)]);


                buff[511] = '\0';

            }

            assert(!ferror(fp));


            file_io::flush_and_close_file(fp);
        }
    #else
        ConfigVar config_state[] = {};

        void ConfigState_load_runtime(void)
        {
            return;
        }      
    #endif

#endif