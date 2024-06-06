#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <windows.h>
#include <stdio.h>
#include <string>
#include <queue>
#include <list>

#pragma warning(disable: 4996)

#define RELEASE_MATRIX_VECTOR(vector) \
	if (!vector.empty()) \
        { \
        for (auto& item : vector) \
        { \
            item.release(); \
        } \
        vector.clear(); \
    }

#define RELEASE_TEXTURE_VECTOR(vector) \
	if (!vector.empty()) \
        { \
        for(SDL_Texture* item : vector) \
        { \
            if (item) \
            { \
                try \
                { \
                    Sleep(300); \
                    SDL_DestroyTexture(item); \
                } \
                catch(const std::exception& e) \
                { \
                    CONSOLE_LOG("Exception occurred during texture destroy: %s", e.what()); \
                } \
                catch(const std::runtime_error& e) \
                { \
                    CONSOLE_LOG("Runtime error occurred during texture destroy: %s", e.what()); \
                } \
                item = nullptr; \
            } \
        } \
        vector.clear(); \
    }

#endif /* __GLOBALS_H__ */