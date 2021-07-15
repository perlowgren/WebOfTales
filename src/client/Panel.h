#ifndef _CLIENT_PANEL_H
#define _CLIENT_PANEL_H

#include <libamanita/sdl/aTextfield.h>



class Panel : public aComponent,public aMouseListener,public aMouseMotionListener,public aActionListener {
aObject_Instance(Panel)

private:
	aTextfield *chatTextField;

public:
	Panel();
	virtual ~Panel();

	void paint(time_t time);

	int getChatTextLength() { return chatTextField->length(); }
	const char *getChatText() { return chatTextField->getText(); }
	void clearChatText() { chatTextField->removeAll(); }
	bool chatKeyDown(aKeyEvent &ke) { return chatTextField->keyDown(ke); }

	bool mouseDown(aMouseEvent &me);
	bool mouseMove(aMouseMotionEvent &mme);
	bool mouseDrag(aMouseMotionEvent &mme);
	bool actionPerformed(aActionEvent &ae);
};



#endif
