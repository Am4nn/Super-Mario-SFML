#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::cout << "...";

    std::ifstream f1("C:/Users/125ar/source/repos/SFML-Game-3/SFML-Game-3/res/shaders/asd");
    std::ifstream f2("C:/Users/125ar/source/repos/SFML-Game-3/SFML-Game-3/res/shaders/dsa");

    std::ofstream o("C:/Users/125ar/source/repos/SFML-Game-3/SFML-Game-3/res/shaders/output");

    std::string s1, s2;
    int cnt = 1;

    while (f1.good() && f2.good())
    {
        f1 >> s1;
        f2 >> s2;
        if (s1 != s2)
        {
            o << cnt << "   " << s1 << " != " << s2 << '\n';
        }
        cnt++;
    }
    std::cout << "Done\n";
    return 0;
}