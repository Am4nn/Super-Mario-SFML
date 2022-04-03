#include <bits/stdc++.h>

using namespace std;

class vec2
{
public:
    double x, y;
    vec2() {}
    vec2(double x, double y) : x(x), y(y) {}
};

double dot(vec2 a, vec2 b)
{
    return ((a.x * b.x) + (a.y * b.y));
}

float fract(float x)
{
    return x - (int)x;
}

float noise1d(float value)
{
    return cos(value + cos(value * 90.0) * 100.0) * 0.5 + 0.5;
}

float random2d(vec2 coord)
{
    return fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453);
}

int main()
{
    for (float x = 0; x <= 60 * 5; x++)
    {
        cout << x << " = " << noise1d(x) << '\n';
    }
    return 0;
}