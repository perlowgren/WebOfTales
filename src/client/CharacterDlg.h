#ifndef _CLIENT_CHARACTERDLG_H
#define _CLIENT_CHARACTERDLG_H

#include <libamanita/sdl/aWindow.h>
#include "../game/Enum.h"
#include "../game/Player.h"

class aImage;
class aTabset;
class aIconButton;
class ItemToolTip;

class CharacterDlg : public aWindow,public aMouseMotionListener {
aObject_Instance(CharacterDlg)

private:
	struct panel {
		aComponent *pane;
		aImage *bgimg;
		int bgindex;
	};

	static const SDL_Rect slots1[SLOTS];
	static const SDL_Rect slots2[SLOTS];
	static const SDL_Rect itemArea;

	Character *ch;
	Player *pl;
	aTabset *ts;
	int tab;
	panel panels[3];
	aIconButton *elemb[5];
	ItemToolTip *itt;

	void setPanel(int n);

public:
	static const int Width;
	static const int Height;

	static const char *strtabs[];
	static const char *strlvl[];
	static const char *strchr[];
	static const char *strgender[];
	static const char *strelem[];
	static const char *stratts[];
	static const char *strstats[];

	static const char *equipMsg[];

	CharacterDlg();
	virtual ~CharacterDlg();

	void setCharacter(Character *c);
	void setTab(int n);
	void paint(time_t time);
	bool mouseDown(aMouseEvent &me);
	bool mouseMove(aMouseMotionEvent &mme);
	bool actionPerformed(aActionEvent &ae);
};



#endif
