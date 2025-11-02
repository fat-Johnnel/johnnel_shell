#include "build_in.h"
#include <termios.h>   
#include <unistd.h> 
#include <sys/ioctl.h>  
#include <stdlib.h>
#include <algorithm>
#include <fstream>

using namespace std;

struct termios g_orig_termios;

int g_screen_lines;
int g_screen_cols;

vector<string> g_lines; 
int g_top_line = 0; 
int g_left_col = 0;

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void reset_terminal() {

    tcsetattr(STDIN_FILENO, TCSANOW, &g_orig_termios);
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}


void set_terminal() {

    if (tcgetattr(STDIN_FILENO, &g_orig_termios) == -1) die("tcgetattr");
    atexit(reset_terminal);

    struct termios new_terminal = g_orig_termios;
    new_terminal.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    new_terminal.c_iflag &= ~(IXON | ICRNL);
    new_terminal.c_oflag &= ~(OPOST);

    new_terminal.c_cc[VMIN] = 1;
    new_terminal.c_cc[VTIME] = 0;
    
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_terminal) == -1) die("tcsetattr");
}

// 获取终端窗口大小
void get_screen() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        perror("ioctl(TIOCGWINSZ)");
        g_screen_lines = 24;
        g_screen_cols = 80;
    } else {
        g_screen_lines = ws.ws_row;
        g_screen_cols = ws.ws_col;
    }
}

// 将文件加载到 g_lines
void load_file(const char *filename) {
    ifstream file(filename, ios::in);
    if (!file.is_open()) {
        perror("打开文件失败");
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        g_lines.push_back(line);
    }
    file.close();
}

// 绘制整个屏幕
void draw_screen() {
    get_screen();
    string ab;

    // \x1b[?25l 隐藏光标
    ab += "\x1b[?25l"; 
    // \x1b[H 将光标移动到左上角
    ab += "\x1b[H";

    for (int y = 0; y < g_screen_lines; y++) {
        int file_line_index = g_top_line + y;

        if (file_line_index >= g_lines.size()) {
            // 文件结束
            ab += "~\r\n";
        } else {
            // 绘制文件行
            const string& line = g_lines[file_line_index];
            int len = line.length();

            if (g_left_col < len) {
                // 计算要写入的长度
                int len_to_write = min(len - g_left_col, g_screen_cols);
                ab += line.substr(g_left_col, len_to_write);
            }
            ab += "\r\n";
        }
    }

    ab += "\x1b[?25h"; 

    write(STDOUT_FILENO, ab.c_str(), ab.length());
}


int reader() {
    char c;
    ssize_t n;

    n = read(STDIN_FILENO, &c, 1);
    
    if (n == -1) {
        die("read");
    }

    if (c == '\x1b') { 
        char seq[2];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 'U'; // Up
                case 'B': return 'D'; // Down
                case 'C': return 'R'; // Right
                case 'D': return 'L'; // Left
            }
        }
        return '\x1b'; // 返回 Esc
    } else {
        return c; // 返回普通字符
    }
}

// 处理按键输入
void process_input() {
    int c = reader();
    switch (c) {
        case 'q':
            write(STDOUT_FILENO, "\x1b[2J", 4); // 清屏
            write(STDOUT_FILENO, "\x1b[H", 3);  // 光标归位
            exit(0);
            break;
        case 'U': // 上
            if (g_top_line > 0) g_top_line--;
            break;
        case 'D': // 下
            // 确保不会滚动过头
            if (g_top_line < g_lines.size() - 1) g_top_line++;
            break;
        case 'L': // 左
            if (g_left_col > 0) g_left_col--;
            break;
        case 'R': // 右
            g_left_col++;
            break;
    }
}


int main(int argc, char ** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    load_file(argv[1]);
    set_terminal();

    while (1) {
        draw_screen();
        process_input();
    }

    return 0; 
}