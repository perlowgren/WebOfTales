#ifndef _CLIENT_BATTLEDLG_H
#define _CLIENT_BATTLEDLG_H

#include <libamanita/sdl/aWindow.h>
#include "../game/Player.h"

class aIconButton;
class aButton;
class ItemToolTip;


class BattleDlg : public aWindow,public aMouseMotionListener {
aObject_Instance(BattleDlg)

private:

	int teamlen;

	struct slot {					/** < A slot in the battle dialog for information about one Character. */
		Character *ch;				/** < Character, all needed info about this character is stored in slot in case of death or dc. */
		int icon;					/** < Face of Character, face icon. */
		int hp,lp;					/** < HP and LP for Character. */
		int deal;					/** < Timer for dealing damage. */
		int soak;					/** < Timer for soaking damage. */
		struct effect {			/** < Struct for an effect, an icon showing a battle effect. */
			int ch;					/** < Index of Character dealing effect. */
			int type;				/** < Type of effect. */
			int value;				/** < Value of effect, typically damage. */
			int timer;				/** < Timer for floater to show. */
		} effects[4];
	} slots[18];
	ItemToolTip *itt;

public:
	static const int Width;
	static const int Height;

	BattleDlg();
	virtual ~BattleDlg();

	void setOpponents(Character *c1,Character *c2);
	int indexOf(Character *c);

	void handleEvent(uint32_t id1,uint32_t id2,uint8_t type,uint16_t effect,int8_t timer);
	void setEffect(int i1,int i2,int t,int v,int tm);

	void updateButtons();
	void paint(time_t time);
	bool mouseMove(aMouseMotionEvent &mme);
	bool actionPerformed(aActionEvent &ae);
};



#endif /* _CLIENT_BATTLEDLG_H */
