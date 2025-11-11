#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <vector>

#include "BACKEND/Spelobject.h"
#include "BACKEND/WapenObject.h"
#include "BACKEND/WapenRustingObject.h"

class Player
{
public:
	Player();
	~Player() = default;

	// Player stats (levenspunten, goud, etc.)
	const std::string& getNaam() const { return mNaam; };

	void setNaam(const std::string& naam) { mNaam = naam; };

	int getLevenspunten() const { return mLevenspunten; }

	void setLevenspunten(int hp) { mLevenspunten = hp; }

	int takeDamage(int damage);

	int getGoud() const { return mGoud; }

	bool isGodmodeActief() const { return mGodmode; }

	void toggleGodmode() { mGodmode = !mGodmode; }

	// Position (positie in ruimte en wereld)
	int getLocalX() const { return mLokaleX; }

	int getLocalY() const { return mLokaleY; }

	int getRuimteX() const { return mRuimteX; }

	int getRuimteY() const { return mRuimteY; }

	void setPositie(int x, int y, int ruimteX, int ruimteY);

	// Inventory management (inventaris beheer)
	void voegItemToe(std::unique_ptr<Spelobject> item);
	std::unique_ptr<Spelobject> verwijderItem(const std::string& naam);
	bool heeftItem(const std::string& naam) const;
	int telGrondstof(char code) const;
	void verwijderGrondstoffen(char code, int aantal);

	const std::vector<std::unique_ptr<Spelobject>>& getInventaris() const { return mInventaris; }

	// Equipment (uitrusting)
	bool draagItem(const std::string& naam);
	void draagWapen(const std::string& naam);
	void draagWapenrusting(const std::string& naam);

	WapenObject* getGedragenWapen() const { return mGedragenWapen.get(); }

	WapenRustingObject* getGedragenWapenrusting() const { return mGedragenWapenrusting.get(); }

	// Eten
	bool eetMedicinaleWortel();

	// Goudklomp
	void updateGoudklomp();

	// Aanval
	int aanval() const;

	// Reset the player
	void reset(const std::string& naam, int hp, int goud);

private:
	void voegGoudToe(int hoeveelheid) { mGoud += hoeveelheid; }

	void verwijderGoud(int hoeveelheid) { mGoud -= hoeveelheid; }

	void wijzigLevenspunten(int delta);

	std::string mNaam;
	int mLevenspunten;
	int mGoud;
	bool mGodmode;

	int mLokaleX, mLokaleY; // Local position in room
	int mRuimteX, mRuimteY; // Room coordinates

	std::vector<std::unique_ptr<Spelobject>> mInventaris;
	std::unique_ptr<WapenObject> mGedragenWapen;
	std::unique_ptr<WapenRustingObject> mGedragenWapenrusting;
};

#endif // PLAYER_H
