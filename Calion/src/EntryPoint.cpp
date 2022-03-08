#include "Editor.h"

int main(void)
{
    Editor* editor = new Editor({ "Calion - Scene Editor", 1440, 810, 4 }, GraphicsAPI::API::OpenGL);
    editor->gameLoop();
    delete editor;

    return 0;
}