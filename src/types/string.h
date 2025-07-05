#pragma once

class String final {
public:
    String(const char* str);

    String(unsigned long str_size);

    String(const char* str, unsigned long str_size);

    ~String();

    operator const char*() const;

private:
    char* const _str;
};
