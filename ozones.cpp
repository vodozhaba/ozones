// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <fstream>
#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include "cpu.h"
#include "ines.h"
#include "ram.h"
#include "rom.h"

using namespace ozones;

int main()
{
    auto ram = std::make_shared<Ram>(2048);
    std::ifstream rom;
    rom.open("/home/vodozhaba/nestest.nes", std::ios::in | std::ios::binary);
    INesHeader header;
    rom.read((char*) &header, sizeof(header));
    auto prg_rom = std::make_shared<Rom>(rom, 16384 * header.prg_rom_size);
    ram->Map(prg_rom, 0xC000, 0, 32768);
    Cpu cpu(ram);
    while(true) {
        cpu.Tick();
    }
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
