#include "FRONTEND/RuimteRenderer.h"
#include "BACKEND/Ruimte.h"
#include "BACKEND/Spelobject.h"
#include "BACKEND/Vijand.h"
#include "Logger.h"

std::string RuimteRenderer::render(const Ruimte* room, const Player& player)
{
	try
	{
		if (!room)
			throw std::runtime_error("Null ruimte ontvangen bij render()");

		const int w = room->getWidth();
		const int h = room->getHeight();

		std::string map;
		map.reserve((w + 1) * h);

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				Spelobject* obj = room->getAt(x, y);
				char ch = obj ? obj->getKaartChar() : room->getTile(x, y); // object wins over terrain
				map.push_back(ch);
			}
			map.push_back('\n');
		}

		// Overlay player 'P'
		const int px = player.getLocalX();
		const int py = player.getLocalY();
		if (px >= 0 && py >= 0 && px < w && py < h)
		{
			map[py * (w + 1) + px] = 'P';
		}

		// Overlay enemies 'X'
		const auto& vijanden = room->getVijanden();
		for (std::size_t i = 0; i < vijanden.size(); ++i)
		{
			const int vx = vijanden[i]->getX();
			const int vy = vijanden[i]->getY();
			if (vx >= 0 && vy >= 0 && vx < w && vy < h)
			{
				map[vy * (w + 1) + vx] = 'X';
			}
		}

		return map;
	}
	catch (const std::exception& e)
	{
		Logger::getInstance().logError(std::string("Fout in RuimteRenderer::render: ") + e.what());
		return "";
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in RuimteRenderer::render");
		return "";
	}
}
