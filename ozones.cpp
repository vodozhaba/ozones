// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <SFML/Graphics.hpp>
#include "cpu.h"
#include "ram.h"

using namespace ozones;

int main()
{
    sf::RenderWindow app(sf::VideoMode(1024, 960), "OzoNES");
    while (app.isOpen() || app.isOpen())
    {
        sf::Event event;
        while (app.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                app.close();
        }
        app.clear();
        app.display();
    }
    return EXIT_SUCCESS;
}
