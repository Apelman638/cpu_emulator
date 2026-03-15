#ifndef GPU_EMULATOR_H_
#define GPU_EMULATOR_H_

#define WIDTH 64
#define HEIGHT 64

enum Color{
    WHITE,
    BLACK,
    RED,
    GREEN,
    BLUE
};

Color v_memory[WIDTH * HEIGHT];

void set_color(int x, int y, Color c);
Color get_color(int x, int y);
void draw_rect(int x1, int y1, int x2, int y2, Color c);
void draw(int x1, int y1, int x2, int y2, Color c);
void draw_line_horizontal(int x1, int x2, int y, Color c);
void draw_line_vertical(int y1, int y2, int x, Color c);
void clear_screen(Color c);
void print_screen();
void save_screen();
void open_image();

#endif 