#include "Kist.h"

Kist::Kist(const char* naam) : Meubilair(naam), mItems() {}

Spelobject* Kist::clone() const
{
	Kist* copy = new Kist(getNaam());
	copyBeschrijvingInto(copy);
	copy->setKnutselVereisten(mKnutselGrondstof, mAantalKnutselGrondstof);
	// deep copy contained items
	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		Spelobject* it = mItems[i];
		if (it)
			copy->addItem(it->clone());
	}
	return copy;
}

char Kist::getKaartChar() const { return '@'; }

void Kist::addItem(Spelobject* item) { mItems.push_back(item); }

std::size_t Kist::getItemCount() const
{
	return mItems.size();
}

const Spelobject* Kist::getItemAt(std::size_t index) const
{
    if (index >= mItems.size())
        return nullptr;
    return mItems[index];
}

CustomVector<Spelobject*>& Kist::getItems() { return mItems; }

Spelobject* Kist::takeItem(int index)
{
	if (mItems.isEmpty())
	{
		return nullptr;
	}

	// Retrieve ownership of the item at the specified index
	Spelobject* obj = mItems.extract(index);

	// Ownership now belongs to the caller
	return obj;
}
