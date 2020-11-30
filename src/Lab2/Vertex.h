#pragma once

namespace Lab2 {
    struct Vertex {
        float x,y;
        Vertex(float x =0, float y = 0);
        /**
         *
         * @param a
         * @param b
         * @param c
         * @return True if the line ac is on the right side of ab
         */
        static bool left(const Vertex& a, const Vertex& b, const Vertex& c);
        /**
         *
         * @param a
         * @param b
         * @param c
         * @return True if the line ac is one the left side of ab.
         */
        static bool right(const Vertex& a, const Vertex& b, const Vertex& c);
        static bool on(const Vertex& a, const Vertex& b, const Vertex& c);

        float len();
        Vertex operator+(const Vertex& other) const;
        Vertex operator-(const Vertex& other) const;
        bool operator==(const Vertex& other) const;
        bool operator==(const Vertex* other) const;
    //private:
        static double z_cross_product(Vertex& a, Vertex& b);
    };
}

