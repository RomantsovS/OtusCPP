#pragma once

#include <memory>

class IRedactorView {
   public:
    virtual ~IRedactorView() {}
    virtual void Render(const std::vector<std::unique_ptr<IPrimitive>>& primitives) const = 0;
};

class RedactorView : public IRedactorView {
   public:
    void Render(const std::vector<std::unique_ptr<IPrimitive>>& primitives) const override;
};

void RedactorView::Render(const std::vector<std::unique_ptr<IPrimitive>>&) const {
    // for (const auto& p : primitives) {
    // }
}