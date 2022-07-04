#pragma once

#include "Redactor.h"
#include "RedactorView.h"

class RedactorController {
   public:
    RedactorController(Redactor* redactor, RedactorView* view) : m_redactor(redactor), m_view(view) {}

    void OnLoad() { m_view->Render(); }

    void LoadFromFile() {
        m_redactor->Import("data.txt");
    }

    void SaveToFile() {
        m_redactor->Export("data.txt");
    }

    void AddPrimitive(const std::string& name) {
        m_redactor->AddPrimitive(std::make_unique<Primitive>(name));
        m_view->Render();
    }

    void RemovePrimitive() {
        m_redactor->RemovePrimitive(0);
    }

   private:
    Redactor* m_redactor = nullptr;
    RedactorView* m_view = nullptr;
};