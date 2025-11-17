typedef struct {
    int x;
    int y;
} int_coord;

typedef struct {
    float x;
    float y;
} float_coord;

void define_colours();



// macros

#define CLAMP_MAX(x, n) ((x) > (n) ? (n) : (x))
#define CLAMP_MIN(x, n) ((x) < (n) ? (n) : (x))
