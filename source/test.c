#include "test.h"

#ifdef __cplusplus
extern "C" 
{
#endif

void c_func(void)
{
    char* byte = malloc(sizeof(*byte));
    free(byte);


    byte = xmalloc(sizeof(*byte));
    free(byte);

    printf("CALLED SUCCESSFULLY\n");

    ArenaAllocator aa;

    ArenaAllocator_init(&aa);
    ArenaAllocator_delete(&aa);

    struct S {
        int x;
    };

    struct S X = {.x = 5 };

    printf("%d\n", X.x);
}

#ifdef __cplusplus
}
#endif
