#pragma once

#include <memory>
#include "Redactor.h"

class IRedactorView {
   public:
    virtual ~IRedactorView() {}
    virtual void Render() const = 0;
};

class RedactorView : public IRedactorView {
   public:
   RedactorView(Redactor* redactor) : m_redactor(redactor) {}

    void Render() const override;
    private:
    Redactor* m_redactor = nullptr;
};

void RedactorView::Render() const {
    if(m_redactor->GetData().empty()) {
        std::cout <<"empty doc\n";
        return;
    }

    for (const auto& p : m_redactor->GetData()) {
        std::cout << p->GetName() << std::endl;
    }
}