#include "stdafx.h"
#include "comboboxes.h"

BasicCombobox::BasicCombobox() {
	font_.CreateFont(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY, FIXED_PITCH |
		FF_MODERN, _T("Arial"));
}

void BasicCombobox::SetMyFont() {
	SetFont(&font_);
}

 