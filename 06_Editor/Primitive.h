#pragma once

class IPrimitive {
   public:
    virtual ~IPrimitive() {}
};

class Primitive : public IPrimitive {
};