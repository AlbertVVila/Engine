#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Deletes a buffer
#define RELEASE( x ) \
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

#define MAX(x,y) ((x>y)?x:y)
#define MIN(x,y) ((x<y)?x:y)

// Configuration -----------
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 720
#define FULLSCREEN false
#define RESIZABLE true
#define BORDERLESS false
#define FULLSCREEN_DESKTOP false
#define VSYNC true
#define TITLE "Real Engine"
#define ENGINE_DESCRIPTION "Best Engine EUW"
#define AUTHOR "Definitely not blitzcrank"
#define REPOSITORY "https://github.com/AlbertVVila/Engine"
#define LICENSE "https://github.com/AlbertVVila/Engine/blob/master/LICENSE"
