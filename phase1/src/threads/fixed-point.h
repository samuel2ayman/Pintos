#ifndef __THREAD_FIXED_POINT_H
#define __THREAD_FIXED_POINT_H

/* Basic definitions of fixed point. */
typedef int fixed_t;
/* 15 LSB used for fractional part. */
#define FX_F (1<<14)

#define fixed_N_to_fx(n)            (n * FX_F)

#define fixed_fx_to_N_zero(x)       (x / FX_F)

#define fixed_fx_to_N_near(x)       ((x>=0)? (x+FX_F/2)/FX_F : (x-FX_F/2)/FX_F)

#define fixed_add_x_y(x,y)          (x + y)

#define fixed_sub_x_y(x,y)          (x - y)

#define fixed_add_x_n(x,n)          (x + n * FX_F) 

#define fixed_sub_x_n(x,n)          (x - n * FX_F)

#define fixed_mult_x_n(x,n)         (x * n ) 

#define fixed_mult_x_y(x,y)         (((__INT64_TYPE__) x) * y / FX_F)

#define fixed_div_x_y(x,y)          (((__INT64_TYPE__) x) * FX_F / y)

#define fixed_div_x_n(x,n)          (x / n )

#endif