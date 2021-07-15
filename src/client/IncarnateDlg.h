#ifndef _CLIENT_INCARNATEDLG_H
#define _CLIENT_INCARNATEDLG_H

#include <libamanita/sdl/aWindow.h>
#include "../game/Player.h"

class aIconButton;
class aButton;
class ItemToolTip;

class IncarnateDlg : public aWindow,public aMouseMotionListener {
aObject_Instance(IncarnateDlg)

private:
	static const SDL_Rect slots1[SLOTS];

	Player *pl;
	aIconButton *chrb[7],*attb[7];
	aButton *ok,*reset,*races;
	int pts,prevrace,icon,skin,hair,face;
	ItemToolTip *itt;

public:
	static const int Width;
	static const int Height;

	static const char *strchr[];
	static const char *strgender[];

	IncarnateDlg();
	virtual ~IncarnateDlg();

	void setPlayer(Player *p);
	void updatePlayer();
	void updateButtons();
	void paint(time_t time);
	bool mouseMove(aMouseMotionEvent &mme);
	bool actionPerformed(aActionEvent &ae);
};



#endif
