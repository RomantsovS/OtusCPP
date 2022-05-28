#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <vector>

#include "IO.h"
#include "Primitive.h"
#include "RedactorView.h"

class Redactor {
   public:
    Redactor();

    void Import(const std::filesystem::path& filename);
    void Export(const std::filesystem::path& filename);

    void Render() const;
    size_t AddPrimitive(std::unique_ptr<IPrimitive> prim);
    void RemovePrimitive(size_t index);

   private:
    std::vector<std::unique_ptr<IPrimitive>> primitives;
    std::unique_ptr<IRedactorView> view;
};

Redactor::Redactor() {
    view = std::make_unique<RedactorView>();
}

void Redactor::Import(const std::filesystem::path& filename) {
    auto importer = std::make_unique<Importer>(filename);

    importer->ReadData(primitives);
}

void Redactor::Export(const std::filesystem::path& filename) {
    auto exporter = std::make_unique<Exporter>(filename);

    exporter->SaveData(primitives);
}

void Redactor::Render() const { view->Render(primitives); }

size_t Redactor::AddPrimitive(std::unique_ptr<IPrimitive> prim) {
    primitives.push_back(std::move(prim));
    return primitives.size() - 1;
}

void Redactor::RemovePrimitive(size_t index) {
    primitives.erase(std::next(primitives.begin(), index));
}