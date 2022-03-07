#include "Editor.h"

int main(void)
{
    Editor* editor = new Editor({ "Calion - Scene Editor", 1440, 810, 4 }, Cala::RenderingAPI::OpenGL);
    editor->gameLoop();
    delete editor;

    return 0;
}