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

const Color background_color = WHITE;

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
        std::cout << v_memory[i-1];
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
        return;
    }
    filename += ".txt";
    system("mkdir -p images");
    std::ofstream Image("./images/" + filename);
    if (Image.is_open()) {  
        for(int i = 1; i <= WIDTH*HEIGHT; i++) {
            Image << v_memory[i];
            if(i%64 == 0) {
                Image << std::endl;
            } // for some reason at the very end a random number is present
            // 54532656
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
    if(filename == ".cancel") {
        return;
    }
    system("mkdir -p images");

    std::ifstream image("./images/" + filename + ".txt");   
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

void update_screen() { // jesus christ its 1230 am
    clear_screen(background_color); // clears background first so it gets overwritten
    
    for (auto obj : all_objects) { // updates all objects        
        for (auto pixel : obj.pixels) {    
            set_color(pixel.x, pixel.y, pixel.c);
        }
    }
}

// for testing, isnt included in the header file
int main() {
    draw_rect(5,5,30,30,BLACK);
    print_screen();
    save_screen();
    move_obj_x(all_objects[0], 1);
    move_obj_y(all_objects[0], 1);
    update_screen();
    print_screen();
    save_screen();

    return 0;
}