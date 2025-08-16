#pragma once

// ReSharper disable once CppInconsistentNaming
class DNS final {
public:
    static unsigned int Resolve(const char* hostname);
};