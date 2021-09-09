#pragma once

class BasicCombobox : public CComboBox {
private:
	CFont font_;
public:
	BasicCombobox();
	void SetMyFont();
};