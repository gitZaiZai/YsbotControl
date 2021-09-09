#pragma once

class BasicButton : public CMFCButton 
{
	DECLARE_DYNAMIC(BasicButton)
private:
	CFont font_;

protected:
	DECLARE_MESSAGE_MAP()

public:
	BasicButton();
	CFont * GetMyFont();
	void SetMyIcon( CString icon ); 

};
