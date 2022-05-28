#pragma once

#include "Primitive.h"

class IImporter {
   public:
    virtual ~IImporter() {}
    virtual void ReadData(std::vector<std::unique_ptr<IPrimitive>>& primitives) = 0;
};

class Importer : public IImporter {
   public:
    Importer(const std::filesystem::path&) {}
    void ReadData(std::vector<std::unique_ptr<IPrimitive>>&) override {}
};

class IExporter {
   public:
    virtual ~IExporter() {}
    virtual void SaveData(std::vector<std::unique_ptr<IPrimitive>>& primitives) = 0;
};

class Exporter : public IExporter {
   public:
    Exporter(const std::filesystem::path&) {}
    void SaveData(std::vector<std::unique_ptr<IPrimitive>>&) override {}
};