#pragma once

#include <algorithm>
#include <filesystem>
#include <memory>
#include <vector>

#include "IO.h"
#include "Primitive.h"

class Redactor {
   public:
    void Import(const std::filesystem::path& filename);
    void Export(const std::filesystem::path& filename) const ;

    size_t AddPrimitive(std::unique_ptr<IPrimitive> prim);
    void RemovePrimitive(size_t index);
    const std::vector<std::unique_ptr<IPrimitive>>& GetData() const;

   private:
    std::vector<std::unique_ptr<IPrimitive>> primitives;
};

void Redactor::Import(const std::filesystem::path& filename) {
    auto importer = std::make_unique<Importer>(filename);

    importer->ReadData(primitives);
}

void Redactor::Export(const std::filesystem::path& filename) const {
    auto exporter = std::make_unique<Exporter>(filename);

    exporter->SaveData(primitives);
}

size_t Redactor::AddPrimitive(std::unique_ptr<IPrimitive> prim) {
    primitives.push_back(std::move(prim));
    return primitives.size() - 1;
}

void Redactor::RemovePrimitive(size_t index) {
    primitives.erase(std::next(primitives.begin(), index));
}

const std::vector<std::unique_ptr<IPrimitive>>& Redactor::GetData() const {
    return primitives;
}