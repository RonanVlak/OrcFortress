#ifndef RUIMTE_RENDERER_H
#define RUIMTE_RENDERER_H
#include "Player.h"
#include "Ruimte.h"
#include <string>

class Ruimte;
class Spelobject;

class RuimteRenderer
{
public:
	// Render room to an ASCII map. Returns empty string if room == nullptr.
	static std::string render(const Ruimte* room, const Player& player);
};

#endif