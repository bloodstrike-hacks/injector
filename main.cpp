#include "Includes.h"
#include "Unity/Unity.h"

#define F(type, address) (*(type*)(address))
#define M(type, address, ...) ((type(*)(__VA_ARGS__))(address))

static ptr libGame;

void* PyEval_GetGlobals() {
    ptr x8 = libGame + 0x793c000 + 0x410;
    if (!x8) return 0;
    x8 = F(ptr, x8 + 0x240);
    if (!x8) return 0;
    x8 = F(ptr, x8 + 0x38);
    if (!x8) return 0;
    x8 = F(ptr, x8 + 0x8);
    if (!x8) return 0;
    return F(void*, x8 + 0x8);
}

#include "draw.h"
void *__1__(void *) {
    sleep(3);
    
    if (!fdlopen("libGame.so", 0)) return 0; // Ensure this is main "com.netease.newspike" process
    
    ifstream iloader("/data/data/com.netease.newspike/python/loader.py", ios::binary);
    if (!iloader.is_open()) {
        LOGI("!iloader.is_open");
        return 0;
    }
    vector<char> loader((istreambuf_iterator<char>(iloader)), istreambuf_iterator<char>());
    loader.push_back('\0');
    iloader.close();
    
    libGame = absoluteAddress("libGame.so");
    
    auto PyRun_StringFlags = M(void*, libGame + 0x44e36e4, const char*, int, void*, void*, void*);
    //auto PyEval_GetGlobals = ((void*(*)())(libGame + 0x4485988));
    auto PyGILState_Ensure = M(void*, libGame + 0x44e1d6c);
    auto PyGILState_Release = M(void*, libGame + 0x44e1e24);
    
    LOGI("GETTING GLOBALS");
    static void* globals;
    while (!(globals = PyEval_GetGlobals())) continue;
    LOGI("GOT GLOBALS");
    
    sleep(8);
    
    LOGI("ENSURING");
    auto x0 = PyGILState_Ensure();
    LOGI("RUNNING");
    PyRun_StringFlags(&loader[0], 257, globals, globals, NULL);
    LOGI("RELEASING...");
    PyGILState_Release(x0);
    LOGI("RELEASED");
    
    __IMGUI__();
    
    LOGI("RETURNING NOW MY GODDY GOD");
    return 0;
}

__attribute__((constructor))
void __0__() { pthread(__1__); }

