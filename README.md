### Love Case

Dependences (for Debian-based distros):

	sudo apt install liballegro5.2 liballegro-ttf5.2 liballegro-image5.2 liballegro-audio5.2 liballegro-acodec5.2 liballegro-video5.2

The game uses CMake as build system, so its building process is pretty typical.

Building dependences:

	sudo apt install liballegro5-dev liballegro-ttf5-dev liballegro-image5-dev liballegro-audio5-dev liballegro-acodec5-dev liballegro-video5-dev
	git submodule update --init --recursive

Compilation:

	mkdir build; cd build
	cmake ..
	make

Running (from top directory):

	build/src/lovecase

Installation (as root):

	make install

https://github.com/dos1/lovecase/

Written by Sebastian Krzyszkowiak <dos@dosowisko.net> for Warsaw Game School Game Jam 2016

Love Case uses libsuperderpy engine: https://github.com/dos1/libsuperderpy/
