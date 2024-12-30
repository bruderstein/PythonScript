#include "stdafx.h"
#include "Utility.h"

HBITMAP ConvertIconToBitmap(LPCWSTR file_path) {
	HBITMAP hBitmap = nullptr;
	HICON hIcon = (HICON)LoadImage(nullptr, file_path, IMAGE_ICON, 16, 16, LR_COLOR | LR_LOADFROMFILE);
	if (hIcon) {
		ICONINFO iconInfo;
		if (GetIconInfo(hIcon, &iconInfo)) {
			HDC hdc = GetDC(nullptr);
			if (hdc) {
				HDC hdcMem = CreateCompatibleDC(hdc);
				if (hdcMem) {
					BITMAP bm{};
					if (GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm)) {
						hBitmap = CreateCompatibleBitmap(hdc, 16, 16);
						if (hBitmap) {
							HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
							DrawIconEx(hdcMem, 0, 0, hIcon, 16, 16, 0, nullptr, DI_NORMAL);
							SelectObject(hdcMem, hbmOld);
						}
					}
					DeleteDC(hdcMem);
				}
				ReleaseDC(nullptr, hdc);
			}
			DeleteObject(iconInfo.hbmColor);
			DeleteObject(iconInfo.hbmMask);
		}
		DestroyIcon(hIcon);
	}
	return hBitmap;
}
