#include "ImGui.h"

int Width, Height;
using namespace ImGui;

#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "Vector/Vector3.hpp"
#include <cstdint>

//struct Vector3 { float x, y, z; };
std::vector<Vector3> g_Entities;
int fifo_fd = 0;

int OpenFIFO() {
    const char* fifo_path = "/data/data/com.netease.newspike/cache/fifo";
    
    struct stat st;
    if(stat(fifo_path, &st) != 0) mkfifo(fifo_path, 0666);
    
    return open(fifo_path, O_RDONLY | O_NONBLOCK);
}

void PollFIFO(int fd) {
    static uint8_t buffer[65536];
    static size_t bytes_available = 0;
    
    ssize_t read_bytes = read(fd, buffer + bytes_available, sizeof(buffer) - bytes_available);
    if (read_bytes > 0) bytes_available += read_bytes;
    
    while (bytes_available >= sizeof(uint32_t)) {
        uint32_t count;
        memcpy(&count, buffer, sizeof(uint32_t));
        size_t packet_size = sizeof(uint32_t) + count * 3 * sizeof(float);
        
        if (bytes_available >= packet_size) {
            g_Entities.resize(count);
            memcpy(g_Entities.data(), buffer + sizeof(uint32_t), count * 3 * sizeof(float));
            
            size_t remaining = bytes_available - packet_size;
            memmove(buffer, buffer + packet_size, remaining);
            bytes_available = remaining;
        } else break;
    }
}

void noMore(ImDrawList* draw) {
    PollFIFO(fifo_fd);
    
    for (const auto& e : g_Entities) {
        ImVec2 From(Width / 2, (Height));
        ImVec2 To(e.X, e.Y);
        
        auto COLOR = WHITE;
        if (e.Z == 1.0) COLOR = RED;
        draw->AddLine(From, To, COLOR, 2);
    }
}

EGLBoolean (*_Draw)(EGLDisplay dpy, EGLSurface surface);
EGLBoolean Draw(EGLDisplay dpy, EGLSurface surface) {
    eglQuerySurface(dpy, surface, EGL_WIDTH, &Width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &Height);
    
    if (Width <= 0 || Height <= 0) return _Draw(dpy, surface);
    
    static bool setup = false;
    if (!setup) {
        setup = true;
        CreateContext();
        ImGui_ImplOpenGL3_Init("#version 300 es");
        
        ImGuiIO &io = ImGui::GetIO();
        ImFontConfig font_cfg;
        font_cfg.SizePixels = 30;
        io.Fonts->AddFontDefault(&font_cfg);
    }
    ImGuiIO& io = ImGui::GetIO();
    //DoTouch(io);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(Width, Height);
    NewFrame();

    auto draw = GetBackgroundDrawList();
    // draw->AddLine(ImVec2(2400,1080), ImVec2(0,0), WHITE, 0.1);
    noMore(draw);
    
    EndFrame();
    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
    
    return _Draw(dpy, surface);
}

void __IMGUI__() {
    HOOKI(dlsym(dlopen("libEGLv3.so", 4), "eglSwapBuffers"), Draw);
    
    fifo_fd = OpenFIFO();
    
    return;
}
