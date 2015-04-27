// noise.h

extern float perlin_init(void);
extern float perlin(float x, float y, float z);
//extern float simplex(float x, float y);

extern float gaussian(float mean, float variance);
extern float brownian(float amplitude, float frequency);
