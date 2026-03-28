#pragma once

struct PivotPoint {
    int x, y;
};

struct UVCoords {
    float u1, v1, u2, v2;
};

struct Color
{
    float r, g, b, a;
    void normalize() {
        r = r/255;
        g = g/255;
        b = b/255;
        a = a/255;
    }
    void print(const char* prefix = "") const {
        std::cout << prefix << ": Color: {" << r << ", " << g << ", " << b << ", " << a << "}" << std::endl;
    }
};

inline bool operator>(const Color& c1, const Color& c2) {
    return  c1.r > c2.r && 
            c1.g > c2.g && 
            c1.b > c2.b && 
            c1.a > c2.a;
}

inline bool operator<(const Color& c1, const Color& c2) {
    return  c1.r < c2.r && 
            c1.g < c2.g && 
            c1.b < c2.b && 
            c1.a < c2.a;
}

inline bool operator!=(const Color& c1, const Color& c2) {
    return  c1.r != c2.r && 
            c1.g != c2.g && 
            c1.b != c2.b && 
            c1.a != c2.a;
}
