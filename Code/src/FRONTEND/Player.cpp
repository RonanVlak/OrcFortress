#include "FRONTEND/Player.h"
#include "BACKEND/Goudklomp.h"
#include "BACKEND/GrondstofObject.h"
#include "BACKEND/Spelobject.h"
#include "BACKEND/WapenObject.h"
#include "BACKEND/WapenRustingObject.h"
#include "RandomEngine.h"
#include <algorithm>
#include <cstring>
#include <iostream>

Player::Player()
	: mNaam(""), mLevenspunten(20), mGoud(0), mGodmode(false), mLokaleX(0), mLokaleY(0), mRuimteX(0), mRuimteY(0)
{
}

void Player::wijzigLevenspunten(int delta)
{
	if (mGodmode && delta < 0)
		return;
	mLevenspunten += delta;
	if (mLevenspunten < 0)
		mLevenspunten = 0;
}

int Player::takeDamage(int damage)
{
	int effectiveDamage = damage - (mGedragenWapenrusting ? mGedragenWapenrusting->getBescherming() : 0);
	if (effectiveDamage < 0)
		effectiveDamage = 0;
	wijzigLevenspunten(-effectiveDamage);
	return effectiveDamage;
}

void Player::setPositie(int x, int y, int ruimteX, int ruimteY)
{
	mLokaleX = x;
	mLokaleY = y;
	mRuimteX = ruimteX;
	mRuimteY = ruimteY;
}

void Player::voegItemToe(std::unique_ptr<Spelobject> item)
{
	if (item)
		mInventaris.push_back(std::move(item));
}

std::unique_ptr<Spelobject> Player::verwijderItem(const std::string& naam)
{
	auto it = std::find_if(mInventaris.begin(), mInventaris.end(),
						   [naam](const std::unique_ptr<Spelobject>& obj) { return obj->getNaam() == naam; });
	if (it == mInventaris.end())
		return nullptr;

	std::unique_ptr<Spelobject> result = std::move(*it);
	mInventaris.erase(it);
	return result;
}

bool Player::heeftItem(const std::string& naam) const
{
	return std::any_of(mInventaris.begin(), mInventaris.end(),
					   [naam](const std::unique_ptr<Spelobject>& obj) { return obj->getNaam() == naam; });
}

int Player::telGrondstof(char code) const
{
	int count = 0;
	for (const auto& item : mInventaris)
	{
		const GrondstofObject* g = dynamic_cast<const GrondstofObject*>(item.get());
		if (g && g->getCode() == code)
			++count;
	}
	return count;
}

void Player::verwijderGrondstoffen(char code, int aantal)
{
	int removed = 0;
	auto it = mInventaris.begin();
	while (it != mInventaris.end() && removed < aantal)
	{
		if (auto g = dynamic_cast<GrondstofObject*>(it->get()); g && g->getCode() == code)
		{
			it = mInventaris.erase(it);
			++removed;
		}
		else
		{
			++it;
		}
	}
}

bool Player::draagItem(const std::string& naam)
{
	// Try to equip as weapon first
	if (std::any_of(mInventaris.begin(), mInventaris.end(), [naam](const std::unique_ptr<Spelobject>& obj)
					{ return dynamic_cast<WapenObject*>(obj.get()) && obj->getNaam() == naam; }))
	{
		draagWapen(naam);
		return true;
	}

	// Try to equip as armor
	if (std::any_of(mInventaris.begin(), mInventaris.end(), [naam](const std::unique_ptr<Spelobject>& obj)
					{ return dynamic_cast<WapenRustingObject*>(obj.get()) && obj->getNaam() == naam; }))
	{
		draagWapenrusting(naam);
		return true;
	}

	// Not wearable
	return false;
}

void Player::draagWapen(const std::string& naam)
{
	auto it = std::find_if(mInventaris.begin(), mInventaris.end(),
						   [naam](const std::unique_ptr<Spelobject>& obj) { return obj->getNaam() == naam; });
	if (it == mInventaris.end())
		return;

	WapenObject* w = dynamic_cast<WapenObject*>(it->get());
	if (!w)
		return;

	// Move current weapon back to inventory
	if (mGedragenWapen)
		mInventaris.push_back(std::move(mGedragenWapen));

	// Equip new weapon
	mGedragenWapen = std::unique_ptr<WapenObject>(static_cast<WapenObject*>(it->release()));
	mInventaris.erase(it);
}

void Player::draagWapenrusting(const std::string& naam)
{
	auto it = std::find_if(mInventaris.begin(), mInventaris.end(),
						   [naam](const std::unique_ptr<Spelobject>& obj) { return obj->getNaam() == naam; });
	if (it == mInventaris.end())
		return;

	WapenRustingObject* r = dynamic_cast<WapenRustingObject*>(it->get());
	if (!r)
		return;

	if (mGedragenWapenrusting)
		mInventaris.push_back(std::move(mGedragenWapenrusting));

	mGedragenWapenrusting = std::unique_ptr<WapenRustingObject>(static_cast<WapenRustingObject*>(it->release()));
	mInventaris.erase(it);
}

int Player::aanval() const
{
	int minDmg = 0, maxDmg = 2;
	if (mGedragenWapen)
	{
		minDmg = mGedragenWapen->getMinimumSchade();
		maxDmg = mGedragenWapen->getMaximumSchade();
	}

	int roll = RandomEngine::getRandomInt(minDmg, maxDmg);
	return roll;
}

bool Player::eetMedicinaleWortel()
{
	const std::string& wortelNaam = "Medicinale Wortel";
	const int herstel = 10;
	const int maxHP = 20;

	// Check if player owns at least one wortel
	if (!heeftItem(wortelNaam))
		return false;

	// Consume one wortel
	verwijderItem(wortelNaam);

	// Heal the player (respecting max HP unless godmode)
	int oudeHP = mLevenspunten;
	int nieuweHP = oudeHP + herstel;
	if (nieuweHP > maxHP)
		nieuweHP = maxHP;

	mLevenspunten = nieuweHP;
	return true;
}

void Player::updateGoudklomp()
{
	int goudklompCount = 0;

	// Count number of Goudklomp objects in inventory
	for (const auto& item : mInventaris)
	{
		if (dynamic_cast<Goudklomp*>(item.get()))
		{
			++goudklompCount;
		}
	}

	// Adjust gold count to match number of Goudklomp items
	int verschil = goudklompCount - mGoud;

	if (verschil > 0)
	{
		voegGoudToe(verschil);
	}
	else if (verschil < 0)
	{
		verwijderGoud(-verschil);
	}
}

void Player::reset(const std::string& naam, int hp, int goud)
{
	mNaam = naam;
	mLevenspunten = hp;
	mGoud = goud;
	mGodmode = false;
	mGedragenWapen.reset();
	mGedragenWapenrusting.reset();
	mInventaris.clear();
	mRuimteX = 0;
	mRuimteY = 0;
	mLokaleX = 0;
	mLokaleY = 0;
}
