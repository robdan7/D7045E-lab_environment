#pragma once
namespace Lab2 {
    struct Color {
        float r,g,b;
        bool operator ==(const Color* other) {
            return this->r == other->r && this->g == other->g && this->b == other->b;
        }
        bool operator == (const Color& other) {
            return this->r == other.r && this->g == other.g && this->b == other.b;
        }
    };
}
