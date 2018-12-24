#define ENTITY_NAME Thing
entity_begin()
    entity_field(vec2, position)
    entity_field(f32,  speed)
    entity_field(u32,  health)
    entity_field(u32,  damage)
entity_end(64)
