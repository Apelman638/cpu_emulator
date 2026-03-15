#include <iostream>
#include <tuple>
#include <fstream> 
#include <algorithm>
#include <vector>
#define WIDTH 64
#define HEIGHT 64

/*
im adding this to deal with graphics 
i need to make it make a screen now, 
implimenting the functions from there into main should be easy

perhaps make a function that changes the color while its drawing
*/

enum Color{
    WHITE,
    BLACK,
    RED,
    GREEN,
    BLUE
};

void draw(int x1, int y1, int x2, int y2, Color c);

Color v_memory[WIDTH * HEIGHT];

struct Pixel {
    int x;
    int y;
    Color c;
};

class Object {
    public: 
        std::vector<Pixel> pixels;
        void add_pixels(int x, int y, Color c) {
            pixels.push_back(Pixel{x, y, c});
        }
};

std::vector<Object> all_objects;

void set_color(int x, int y, Color c) {
    if(x < 0 || x >= WIDTH) return;
    if(y < 0 || y >= HEIGHT) return;
    v_memory[y*WIDTH + x] = c;
}

Color get_color(int x, int y) {
    return (Color)v_memory[WIDTH*y + x];
}

void draw_rect(int x1, int y1, int x2, int y2, Color c) {
    draw(x1,y1,x2,y2,c);
}

void draw_line_horizontal(int x1, int x2, int y, Color c) {
    draw(x1,y,x2,y,c);
}

void draw_line_vertical(int y1, int y2, int x, Color c) {
    draw(x,y1,x,y2,c);
}

void clear_screen(Color c) {
    draw(0,0,WIDTH-1,HEIGHT-1, c);
    all_objects.pop_back();
}

void draw(int x1, int y1, int x2, int y2, Color c) {
    if(x1 < 0 || x2 >= WIDTH) return;
    if(y1 < 0 || y2 >= HEIGHT) return;
    Object obj; 
    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            set_color(x,y,c);
            obj.add_pixels(x,y,c);
        }
    }
    all_objects.push_back(obj);
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
    system("mkdir -p .images");
    std::ofstream Image("./.images/" + filename);
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

void open_image() {
    std::string filename;
    std::cout << "image to open: ";
    std::cin >> filename;
    system("mkdir -p .images");

    std::ifstream image("./.images/" + filename + ".txt");   
    if (!image) {
        std::cerr << "Image not found" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(image, line)) {
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        int j = 0;
        for(int i = 0; i < line.length()-1; i++, j++) {
            line[i] -= '0';
            v_memory[j] = static_cast<Color>(line[i]);
        }
    }
}

// for testing, isnt included in the header file
int main() {
    draw_rect(5,5,30,30,BLACK);
    save_screen();
    return 0;
}