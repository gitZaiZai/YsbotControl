#pragma once

class NewButton : public CMFCButton {
public:
	NewButton();
	void SetMyFont();
private:
	CFont font;
};