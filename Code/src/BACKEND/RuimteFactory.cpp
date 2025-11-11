#include "RuimteFactory.h"
#include "Deur.h"
#include "Direction.h"
#include "GrondstofObject.h"
#include "Logger.h"
#include "Ruimte.h"

// Helper: is within outer two rings
static bool is_outer_two_rings(int x, int y, int w, int h) { return (x < 2 || y < 2 || x >= w - 2 || y >= h - 2); }

Ruimte* RuimteFactory::createStartRuimte(int width, int height)
{
	try
	{
		Ruimte* ruimte = new Ruimte(width, height);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (is_outer_two_rings(x, y, width, height))
				{
					ruimte->setTile(x, y, '0'); // rock tile
				}
				else
				{
					ruimte->setTile(x, y, '.'); // floor
				}
			}
		}
		return ruimte;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "RuimteFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}

	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in RuimteFactory::createStartRuimte");
		return nullptr;
	}
}

// Generic new room: fill with rock everywhere;
// no doors. Player must craft & place them.
Ruimte* RuimteFactory::createRuimte(int width, int height)
{
	try
	{
		Ruimte* ruimte = new Ruimte(width, height);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				ruimte->setTile(x, y, '0'); // rock tile
			}
		}
		return ruimte;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "RuimteFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}

	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in RuimteFactory::createRuimte");
		return nullptr;
	}
}