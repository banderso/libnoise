// noise.c

#define internal static
#define global static
#define local static

/*
 * This code is based on Stefan Gustavson's paper "Simplex Noise Demystified"
 * (http://staffwww.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf)
 * 
 * I claim no owner ship of any of the ideas here.
 */

internal int perlin_grad3[12][3] = {
  { 1, 1, 0},{-1, 1, 0},{ 1,-1, 0},
  {-1,-1, 0},{ 1, 0, 1},{-1, 0, 1},
  { 1, 0,-1},{-1, 0,-1},{ 0, 1, 1},
  { 0,-1, 1},{ 0, 1,-1},{ 0,-1,-1}
};

internal int perlin_p[256] = {
  151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
  140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
  247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
   57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
   74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
   60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
   65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
  200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
   52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
  207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
  119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
  129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
  218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
   81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
  184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
  222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

internal int perlin_perm[512] = {0};

internal int fastfloor(float x) {
  return x > 0 ? (int)x : (int)x - 1;
}

internal int dot(int *gradient, float x, float y, float z) {
  return gradient[0] * x + gradient[1] * y + gradient[2] * z;
}

internal float mix(float a, float b, float t) {
  return (1 - t) * a + t * b;
}

internal float fade(float t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

internal int gradient_index(int x, int y, int z, int index_a, int index_b, int index_c) {
  return perlin_perm[x + index_a + perlin_perm[y + index_b + perlin_perm[z + index_c]]] % 12;
}

void perlin_init(void) {
  for(int i = 0, len = 512; i < len; ++i) {
    perlin_perm[i] = perlin_p[i & 255];
  }
}

float perlin(float x, float y, float z) {
  // unit grid cell point
  int grid_x = fastfloor(x);
  int grid_y = fastfloor(y);
  int grid_z = fastfloor(z);

  // relative grid cell
  x = x - grid_x;
  y = y - grid_y;
  z = z - grid_z;

  // wrap integer cell
  grid_x = grid_x & 255;
  grid_y = grid_y & 255;
  grid_z = grid_z & 255;

  // hashed gradient indices
  int grad_index0 = gradient_index(grid_x, grid_y, grid_z, 0, 0, 0);
  int grad_index1 = gradient_index(grid_x, grid_y, grid_z, 0, 0, 1);
  int grad_index2 = gradient_index(grid_x, grid_y, grid_z, 0, 1, 0);
  int grad_index3 = gradient_index(grid_x, grid_y, grid_z, 0, 1, 1);
  int grad_index4 = gradient_index(grid_x, grid_y, grid_z, 1, 0, 0);
  int grad_index5 = gradient_index(grid_x, grid_y, grid_z, 1, 0, 1);
  int grad_index6 = gradient_index(grid_x, grid_y, grid_z, 1, 1, 0);
  int grad_index7 = gradient_index(grid_x, grid_y, grid_z, 1, 1, 1);

  // noise contributions from the eight corners
  float n0 = dot(perlin_grad3[grad_index0], x,     y,     z);
  float n1 = dot(perlin_grad3[grad_index1], x - 1, y,     z);
  float n2 = dot(perlin_grad3[grad_index2], x,     y - 1, z);
  float n3 = dot(perlin_grad3[grad_index3], x - 1, y - 1, z);
  float n4 = dot(perlin_grad3[grad_index4], x,     y,     z - 1);
  float n5 = dot(perlin_grad3[grad_index5], x - 1, y,     z - 1);
  float n6 = dot(perlin_grad3[grad_index6], x,     y - 1, z - 1);
  float n7 = dot(perlin_grad3[grad_index7], x - 1, y - 1, z - 1);

  // fade curve for each indice
  float u = fade(x);
  float v = fade(y);
  float w = fade(z);

  // interpolate along x
  float nx0 = mix(n0, n4, u);
  float nx1 = mix(n1, n5, u);
  float nx2 = mix(n2, n6, u);
  float nx3 = mix(n3, n7, u);

  // interpolate along y
  float nxy0 = mix(nx0, nx2, v);
  float nxy1 = mix(nx1, nx3, v);

  // interpolate along z
  float nxyz = mix(nxy0, nxy1, w);

  return nxyz;
}
