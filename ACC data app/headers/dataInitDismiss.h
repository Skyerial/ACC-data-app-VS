#pragma once
#include <Windows.h>

struct SMElement
{
	HANDLE hMapFile;
	unsigned char* mapFileBuffer;
};

// inline used because that means same variable can be used in all files that
// include dataInitDismiss.h without making multiple of those same variable
inline SMElement m_graphics;
inline SMElement m_physics;
inline SMElement m_static;

void initPhysics();
void initGraphics();
void initStatic();
void dismissAll();
