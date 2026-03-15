#include <iostream>
#define WIDTH 64
#define HEIGHT 64
#include <tuple>
#include <fstream> 

/*
im adding this to deal with graphics 
i need to make it make a screen now, 
implimenting the functions from there into main should be easy

perhaps make a function that changes the color while its rendering
*/

enum Color{
    WHITE,
    BLACK,
    RED,
    GREEN,
    BLUE
};

void render(int x1, int y1, int x2, int y2, Color c);

Color v_memory[WIDTH * HEIGHT];

void set_color(int x, int y, Color c) {
    if(x < 0 || x >= WIDTH) return;
    if(y < 0 || y >= HEIGHT) return;
    v_memory[y*WIDTH + x] = c;
}

Color get_color(int x, int y) {
    return (Color)v_memory[WIDTH*y + x];
}

void draw_rect(int x1, int y1, int x2, int y2, Color c) {
    render(x1,y1,x2,y2,c);
}

void draw_line_horizontal(int x1, int x2, int y, Color c) {
    render(x1,y,x2,y,c);
}

void draw_line_vertical(int y1, int y2, int x, Color c) {
    render(x,y1,x,y2,c);
}

void clear_screen(Color c) {
    render(0,0,WIDTH-1,HEIGHT-1, c);
}

void render(int x1, int y1, int x2, int y2, Color c) {
    if(x1 < 0 || x2 >= WIDTH) return;
    if(y1 < 0 || y2 >= HEIGHT) return;

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            set_color(x,y,c);
        }
    }
}

void print_screen() {
    for(int i = 0; i < WIDTH*HEIGHT; i++) {
        std::cout << v_memory[i];
        if(i%64 == 0) {
            std::cout << std::endl;
        }
    }
}

void save_screen() {
    std::string filename;
    std::cout << "name image: ";
    std::cin >> filename;
    filename += ".txt";
    std::ofstream Image(filename);
    if (Image.is_open()) {  
        for(int i = 0; i < WIDTH*HEIGHT; i++) {
            Image << v_memory[i];
            if(i%64 == 0) {
                Image << std::endl;
            }
        }   
    } else {
        std::cout << "file failed to open" << std::endl;
    }
    Image.close();
}

// for testing, isnt included in the header file
int main() {
    draw_rect(5,5,30,30,BLACK);
    print_screen();
    return 0;
}