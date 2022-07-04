#pragma once

class IPrimitive {
   public:
    virtual ~IPrimitive() {}

    virtual std::string GetName() const = 0;
};

class Primitive : public IPrimitive {
    public:
    Primitive(const std::string name) : m_name(name) {}
    std::string GetName() const override {
        return m_name;
    }

    private:
    std::string m_name;
};