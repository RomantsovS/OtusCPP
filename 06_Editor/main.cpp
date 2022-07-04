#include <iostream>

#include "Redactor.h"
#include "RedactorView.h"
#include "RedactorController.h"

void GUILoadFromFile(RedactorController* controller);
void GUISaveToFile(RedactorController* controller);
void GUIAddPrimitive(RedactorController* controller, const std::string& name);
void GUIRemovePrimitive(RedactorController* controller);

int main([[maybe_unused]] int argc, [[maybe_unused]] char const* argv[]) {
    Redactor redactor;
    RedactorView redactor_view(&redactor);
    RedactorController redactor_controller(&redactor, &redactor_view);

    redactor_controller.OnLoad();
    
    GUILoadFromFile(&redactor_controller);

    GUIAddPrimitive(&redactor_controller, "first");
    
    GUISaveToFile(&redactor_controller);
    
    return 0;
}

void GUILoadFromFile(RedactorController* controller) {
    controller->LoadFromFile();
}

void GUISaveToFile(RedactorController* controller) {
    controller->SaveToFile();
}

void GUIAddPrimitive(RedactorController* controller, const std::string& name) {
    controller->AddPrimitive(name);
}

void GUIRemovePrimitive(RedactorController* controller) {
    controller->RemovePrimitive();
}