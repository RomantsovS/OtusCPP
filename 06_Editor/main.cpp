#include <iostream>

#include "Redactor.h"

void GUILoadFromFile(Redactor* redactor);
void GUISaveToFile(Redactor* redactor);
void GUIAddPrimitive(Redactor* redactor);
void GUIRemovePrimitive(Redactor* redactor);

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    Redactor redactor;
    
    GUILoadFromFile(&redactor);

    GUIAddPrimitive(&redactor);

    redactor.Render();
    
    GUISaveToFile(&redactor);
    
    return 0;
}

void GUILoadFromFile(Redactor* redactor) {
    redactor->Import("data.txt");
}

void GUISaveToFile(Redactor* redactor) {
    redactor->Export("data.txt");
}

void GUIAddPrimitive(Redactor* redactor) {
    redactor->AddPrimitive(std::make_unique<Primitive>());
}

void GUIRemovePrimitive(Redactor* redactor) {
    redactor->RemovePrimitive(0);
}