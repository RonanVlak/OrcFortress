#include "SpelobjectFactory.h"
#include "Afvalmand.h"
#include "CustomVector.h"
#include "Deur.h"
#include "Goudklomp.h"
#include "GrondstofObject.h"
#include "Kist.h"
#include "Logger.h"
#include "Meubilair.h"
#include "RandomEngine.h"
#include "WapenObject.h"
#include "WapenRustingObject.h"
#include <cstring>
#include <sqlite3.h>

// A tiny GenericSpelobject used when we don't have a concrete subclass implemented
class GenericSpelobject : public Spelobject
{
public:
	GenericSpelobject(const char* naam) : Spelobject(naam) {}

	Spelobject* clone() const override
	{
		GenericSpelobject* copy = new GenericSpelobject(mNaam);
		copyBeschrijvingInto(copy);
		copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
		return copy;
	}

	char getKaartChar() const override { return '?'; }
};

void SpelobjectFactory::setTemplates(CustomVector<Spelobject*>& templates)
{
	mTemplates = &templates; // pointer to external list owned by facade
}

Spelobject* SpelobjectFactory::createSpelobject(sqlite3_stmt* stmt)
{
	try
	{
		if (!stmt)
			return nullptr;
		const char* naam = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		const char* type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

		// read numeric columns
		int minSchade = sqlite3_column_int(stmt, 2);
		int maxSchade = sqlite3_column_int(stmt, 3);
		int bescherming = sqlite3_column_int(stmt, 4);

		// knutselgrondstof: text column (letter code like 'I', 'G', 'L', etc.)
		const unsigned char* knutselText = sqlite3_column_text(stmt, 5);
		char knutselCode = (knutselText && knutselText[0]) ? static_cast<char>(knutselText[0]) : '\0';

		// aantalgrondstof: int column (how many of the material needed)
		int aantalGrondstof = sqlite3_column_int(stmt, 6);

		// dispatch by type and set crafting requirements
		Spelobject* obj = nullptr;

		if (!type)
		{
			obj = new GenericSpelobject(naam);
		}
		else if (std::strcmp(type, "wapen") == 0)
		{
			obj = new WapenObject(naam, minSchade, maxSchade);
		}
		else if (std::strcmp(type, "wapenrusting") == 0)
		{
			obj = new WapenRustingObject(naam, bescherming);
		}
		else if (std::strcmp(type, "meubilair") == 0)
		{
			// In the DB, Deur/Kist/Afvalmand are typed as 'meubilair'.
			// Differentiate by name so we instantiate the correct subclass.
			if (std::strcmp(naam, "Deur") == 0)
			{
				obj = new Deur(naam, Direction::Noord); // orientation set on placement
			}
			else if (std::strcmp(naam, "Kist") == 0)
			{
				obj = new Kist(naam);
			}
			else if (std::strcmp(naam, "Afvalmand") == 0)
			{
				obj = new Afvalmand(naam);
			}
			else
			{
				// other furniture-like things (Bed, Tafel, etc.) can be generic
				obj = new Meubilair(naam);
			}
		}
		else if (std::strcmp(type, "anders") == 0 || std::strcmp(type, "goudklomp") == 0)
		{
			// Separate object for gold nuggets as per assignment
			obj = new Goudklomp(naam);
		}
		else
		{
			// fallback generic
			obj = new GenericSpelobject(naam);
		}

		// Apply crafting requirements for non-grondstof items as defined by DB.
		// (Harmless on grondstof too, but mainly relevant for crafted items like Goudklomp, weapons, etc.)
		if (obj && knutselCode != '\0')
		{
			obj->setKnutselVereisten(static_cast<int>(knutselCode), aantalGrondstof);
		}

		return obj;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "SpelobjectFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in SpelobjectFactory::createSpelobject");
		return nullptr;
	}
}

Spelobject* SpelobjectFactory::createByName(const char* naam) const
{
	try
	{
		if (!mTemplates)
			return nullptr;
		for (std::size_t i = 0; i < mTemplates->size(); ++i)
		{
			if (std::strcmp((*mTemplates)[i]->getNaam(), naam) == 0)
			{
				return (*mTemplates)[i]->clone();
			}
		}
		return nullptr;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "SpelobjectFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in SpelobjectFactory::createByName");
		return nullptr;
	}
}

Spelobject* SpelobjectFactory::createByCode(char code) const
{
	try
	{
		if (!mTemplates)
			return nullptr;
		for (std::size_t i = 0; i < mTemplates->size(); ++i)
		{
			GrondstofObject* g = dynamic_cast<GrondstofObject*>((*mTemplates)[i]);
			if (g && g->getCode() == code)
			{
				return g->clone();
			}
		}
		return nullptr;
	}
	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "SpelobjectFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}
	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in SpelobjectFactory::createByCode");
		return nullptr;
	}
}

void SpelobjectFactory::addDefaultGrondstoffen()
{
	if (!mTemplates)
		return;

	auto addIfMissing = [this](const char* naam, char code)
	{
		// avoid duplicates
		for (std::size_t i = 0; i < mTemplates->size(); ++i)
		{
			Spelobject* o = (*mTemplates)[i];
			if (o && std::strcmp(o->getNaam(), naam) == 0)
				return;
		}
		mTemplates->push_back(new GrondstofObject(naam, code));
	};

	addIfMissing("Gouderts", 'G');
	addIfMissing("Ijzererts", 'I');
	addIfMissing("Hout", 'H');
	addIfMissing("Leer", 'L');
	addIfMissing("Medicinale Wortel", 'W');
	addIfMissing("Rotzooi", 'R');
	addIfMissing("Metaal", 'M');
}

Spelobject* SpelobjectFactory::createRandomGrondstof() const
{
	try
	{
		if (!mTemplates || mTemplates->isEmpty())
			return nullptr;

		// Allowed grondstof names
		const char* allowedNames[] = {"Rotzooi", "Hout", "Ijzererts", "Medicinale Wortel", "Gouderts"};
		const int allowedCount = 5;

		// Count grondstof prototypes
		int count = 0;
		for (std::size_t i = 0; i < mTemplates->size(); ++i)
		{
			Spelobject* p = (*mTemplates)[i];
			if (p && p->isGrondstof())
				++count;
		}
		if (count == 0)
			return nullptr;

		bool pickedAllowed = false;

		while (!pickedAllowed)
		{
			// Pick Nth grondstof using RandomEngine
			int pick = RandomEngine::getRandomInt(0, count - 1);
			for (std::size_t i = 0; i < mTemplates->size(); ++i)
			{
				Spelobject* p = (*mTemplates)[i];
				if (p && p->isGrondstof())
				{
					if (pick-- == 0)
					{
						if (std::strcmp(p->getNaam(), allowedNames[0]) != 0 &&
							std::strcmp(p->getNaam(), allowedNames[1]) != 0 &&
							std::strcmp(p->getNaam(), allowedNames[2]) != 0 &&
							std::strcmp(p->getNaam(), allowedNames[3]) != 0 &&
							std::strcmp(p->getNaam(), allowedNames[4]) != 0)
						{
							continue; // pick again
						}
						pickedAllowed = true;
						return p->clone(); // clone the prototype
					}
				}
			}
		}
		// Shouldnt reach here
		return nullptr;
	}

	catch (const std::exception& e)
	{
		char buf[512];
		std::snprintf(buf, sizeof(buf), "SpelobjectFactory fout: %s", e.what());
		Logger::getInstance().logError(buf);
		return nullptr;
	}

	catch (...)
	{
		Logger::getInstance().logError("Onbekende fout in SpelobjectFactory::createRandomGrondstof");
		return nullptr;
	}
}

Spelobject* SpelobjectFactory::findPrototypeByName(const char* naam) const
{
	for (std::size_t i = 0; i < mTemplates->size(); ++i)
	{
		Spelobject* obj = (*mTemplates)[i];
		if (obj && std::strcmp(obj->getNaam(), naam) == 0)
			return obj;
	}
	return nullptr;
}
