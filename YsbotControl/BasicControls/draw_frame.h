#pragma once

extern void DrawMyLine(CWindowDC *dc, COLORREF color, int x0, int y0, int x1,
	int y1);
extern void DrawMyFrame(CWindowDC *dc, int item_number, int r = 220,
	int g = 220, int b = 220);