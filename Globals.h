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

// Configuration -----------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Real Engine"
#define ENGINE_DESCRIPTION "Best Engine EUW"
#define AUTHOR "Definitely not blitzcrank"
#define REPOSITORY "https://github.com/AlbertVVila/Engine"
#define LICENSE "https://github.com/AlbertVVila/Engine/blob/master/LICENSE"
