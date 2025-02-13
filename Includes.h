#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <map>
#include <vector>
#include <tuple>
#include <list>
#include <thread>
#include <set>
#include <unistd.h>
#include <pthread.h>
#include <android/log.h>
#include <cstdint>

#include <chrono>
#include <deque>

using namespace std;

#define LOG_TAG "bloodstrike"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define ptr uintptr_t
#define DWORD uintptr_t
#define ulong uint64_t
#define dword uint32_t
#define qword uint64_t

#define ARGB(a, r, g, b) r << 0 | g << 8 | b << 16 | a << 24

#define WHITE              ImColor(255, 255, 255)
#define RED                ImColor(255, 0, 0)
#define GREEN              ImColor(0, 255, 0)
#define LIME               ImColor(0, 255, 0)
#define BLUE               ImColor(0, 0, 255)
#define BLACK              ImColor(0, 0, 0)
#define PURPLE             ImColor(128, 0, 128)
#define GREY               ImColor(128, 128, 128)
#define YELLOW             ImColor(255, 255, 0)
#define ORANGE             ImColor(255, 165, 0)
#define DARKGREEN          ImColor(0, 100, 0)
#define PINK               ImColor(255, 192, 203)
#define BROWN              ImColor(165, 42, 42)
#define CYAN               ImColor(0, 255, 255)

void pthread(void* (*func)(void*)) {pthread_t x; pthread_create(&x, 0, func, 0);}

u_long htol(string strx) {const char* str = strx.c_str(); u_long res = 0; char c; while ((c = *str++)) {char v = (c & 0xF) + (c >> 6) | ((c >> 3) & 0x8); res = (res << 4) | (u_long) v;} return res;}
ptr absoluteAddress(const char *libraryName, uintptr_t offset=0) {
    bool found=false;
    LOGI("Searching %s until found...", libraryName);
    while (!found) {
        ifstream maps("/proc/self/maps");
        string map;
        while (getline(maps, map)) {
            if (strstr(map.c_str(), libraryName)) {
                LOGI("Library found! Exiting loop...");
                istringstream line(map);
                string start;
                getline(line, start, '-');
                return htol(start)+offset;
            }
        }
    }
    return 0;
}
