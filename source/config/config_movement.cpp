// #define configure_begin()
// configure_var(f64, physics, gravity),
// configure_var(f64, physics, GRAVITY_DEFAULT),
// configure_var(f64, physics, GRAVITY_TERRESTRIAL),
// configure_var(f64, physics, GRAVITY_OUTER_SPACE)

// namespace physics {

// }

configure_var(f64, wee, WEEf64, 0.0)
configure_var(i32, wee, WEEi32, 1)
configure_var(char, ha, HAchar, '2')

#define grav_default 0.21875
configure_var(f64, physics, GRAVITY_DEFAULT, grav_default)
configure_var(f64, physics, GRAVITY_TERRESTRIAL, grav_default)
configure_var(f64, physics, GRAVITY_OUTER_SPACE, grav_default / 2)
configure_var_mutable(f64, physics, gravity, grav_default)


