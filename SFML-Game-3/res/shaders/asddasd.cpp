#include <iostream>
#include <map>

int main()
{
    std::multimap<int, char> mm;

    mm.insert(std::pair<int, char>(0, 'c')); // we can't use [] operator
    mm.insert(std::pair<int, char>(0, 'g'));
    mm.insert(std::pair<int, char>(0, 's'));
    std::cout << "std::multimap<> allows duplicate keys : \n";
    for (auto p : mm)
    {
        std::cout << p.first << " " << p.second << '\n';
    }

    std::map<int, char> m;

    m[0] = 'c';
    m[0] = 'g';
    m.insert(std::pair<int, char>(0, 's')); // we can use [] as well as .insert() method
    std::cout << "std::map<> doesn't allows duplicate keys : \n";
    for (auto p : m)
    {
        std::cout << p.first << " " << p.second << '\n';
    }
    return 0;
}