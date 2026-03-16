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

i need to get an actual screen and not terminal

add drawing circle function
add delete object func
*/

enum Color{
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};
// color + 30 is the ansi color code

const Color background_color = BLACK;

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
    if(x < 0 || x >= WIDTH) {
        std::cerr << "Position does not exist" << std::endl;
        return;
    }
    if(y < 0 || y >= HEIGHT) {
        std::cerr << "Position does not exist" << std::endl;
        return;
    }
    v_memory[y*WIDTH + x] = c;
}

void build_screen() {
    for(int x = 0; x < WIDTH; x++) {
        for(int y = 0; y < HEIGHT; y++) {
            v_memory[y*WIDTH + x] = background_color;
        }
    }
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
    if(x1 < 0 || x2 >= WIDTH) {
        std::cerr << "Position does not exist" << std::endl;
        return;
    }
    if(y1 < 0 || y2 >= HEIGHT) {
        std::cerr << "Position does not exist" << std::endl;
        return;
    }
    Object obj; 
    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            set_color(x,y,c);
            obj.add_pixels(x,y,c);
        }
    }
    all_objects.push_back(obj);
}

void move_obj_x(Object &obj, int dx) {
    for(auto &pixel : obj.pixels){
        pixel.x += dx;
    }
}

void move_obj_y(Object &obj, int dy) {
    for(auto &pixel : obj.pixels){
        pixel.y += dy;
    }
}

void print_screen() {
    for(int i = 1; i <= WIDTH*HEIGHT; i++) {
        #if 0
        std::cout << "\033[" << v_memory[i-1] + 30 << 'm';
        std::cout << v_memory[i-1];
        std::cout << "\033[0m" << ' ';
        if(i%64 == 0) {
            std::cout << std::endl;
        } 
        #endif
        std::cout << v_memory[i-1] << ' ';
        if(i%64 == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << "\n" << std::endl;
}

void save_screen() {
    std::string filename;
    std::cout << "name image: ";
    std::cin >> filename;
    if(filename == ".cancel") {
        std::cout << "canceled" << std::endl;
        return;
    }
    filename += ".txt";
    system("mkdir -p images");
    std::ofstream Image("./images/" + filename);
    if (Image.is_open()) {  
        for(int i = 1; i <= WIDTH*HEIGHT; i++) {
            #if 0
            Image << "\033[" << v_memory[i] + 40 << "m  \033[0m";
            if(i%64 == 0) {
                Image << std::endl;
            } 
            #endif
            #if 1
            Image << v_memory[i-1] << ' ';
            if(i%64 == 0) {
                Image << std::endl;
            }
            #endif
        }   
    } else {
        std::cout << "file failed to open" << std::endl;
    }
    Image.close();
}

void open_image() { 
    // DOES NOT WORK
    std::string filename;
    std::cout << "image to open: ";
    std::cin >> filename;
    if(filename == ".cancel") {
        std::cout << "canceled" << std::endl;
        return;
    }
    system("mkdir -p images");

    std::ifstream image("./images/" + filename + ".txt");   
    if (!image) {
        std::cerr << "Image not found" << std::endl;
        return;
    }
    std::string line;
    int j = 0;
    while (std::getline(image, line)) {
        for(int i = 0; i < line.length(); i++, j++) {
            int value = line[i] - '0';
            v_memory[j] = static_cast<Color>(value);
        }
    }
}

void update_screen() { // jesus christ its 1230 am
    clear_screen(background_color); // clears background first so it gets overwritten
    
    for (auto obj : all_objects) { // updates all objects        
        for (auto pixel : obj.pixels) {    
            set_color(pixel.x, pixel.y, pixel.c);
        }
    }
}

#if 0
int main() {
    #if 1
    draw_rect(5,5,30,30, BLUE); //whichever object is created first gets is in a lower tier
    draw_rect(25,25,50,50, RED);
    draw_rect(27, 12, 40, 45, GREEN);
    print_screen();
    save_screen();
    move_obj_x(all_objects[0], 1);
    move_obj_y(all_objects[0], 1);
    update_screen();
    print_screen();
    save_screen();
    #endif
    #if 0
    open_image(); 
    print_screen();
    #endif

    return 0;
}
#endif