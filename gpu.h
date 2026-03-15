#ifndef GPU_EMULATOR_H_
#define GPU_EMULATOR_H_

#define WIDTH 64
#define HEIGHT 64

enum Color{
    WHITE = 0xFFFFFF,
    BLACK = 0x000000,
    RED = 0xFF0000,
    GREEN = 0x008000,
    BLUE = 0x0000FF
};

extern Color v_memory[WIDTH * HEIGHT];

void set_color(int x, int y, Color c);
Color get_color(int x, int y);
void draw_rect(int x1, int y1, int x2, int y2, Color c);
void render(int x1, int y1, int x2, int y2, Color c);
void draw_line_horizontal(int x1, int x2, int y, Color c);
void draw_line_vertical(int y1, int y2, int x, Color c);
void clear_screen(Color c);

#endif 