#pragma once

#include <afxwin.h>
#include <string>
#include <fstream>

using namespace std;

class ScreenShot {
public:
	int updateTime = 60;
	HWND hwnd;
public:
	ScreenShot() {
		hwnd = GetDesktopWindow();
	}

	void setUpdateTime(int time) {
		updateTime = time;
	}

	void setWindow(string title) {
		HWND thwnd = ::FindWindow(0, CString(title.c_str()));
		if (thwnd == NULL)
			return;
		hwnd = thwnd;
	}

	void TakeScreenShot2(string filename) {
		RECT DesktopParams;
		HDC DevC = GetDC(hwnd);
		if (DevC == NULL) {
			hwnd = GetDesktopWindow();
			DevC = GetDC(hwnd);
		}
		GetWindowRect(hwnd, &DesktopParams);
		DWORD Width = DesktopParams.right - DesktopParams.left;
		DWORD Height = DesktopParams.bottom - DesktopParams.top;

		HDC CaptureDC = CreateCompatibleDC(DevC);
		HBITMAP hBitmap = CreateCompatibleBitmap(DevC, Width, Height);
		SelectObject(CaptureDC, hBitmap);
		BitBlt(CaptureDC, 0, 0, Width, Height, DevC, 0, 0, SRCCOPY | CAPTUREBLT);

		vector<BYTE> buf;
		IStream *stream = NULL;
		HRESULT hr = CreateStreamOnHGlobal(0, TRUE, &stream);
		CImage image;
		ULARGE_INTEGER liSize;

		// screenshot to jpg and save to stream
		image.Attach(hBitmap);
		image.Save(stream, Gdiplus::ImageFormatJPEG);
		IStream_Size(stream, &liSize);
		DWORD len = liSize.LowPart;
		IStream_Reset(stream);
		buf.resize(len);
		IStream_Read(stream, &buf[0], len);
		stream->Release();

		// just testing if the buf contains the correct data
		fstream fi(filename, fstream::binary | fstream::out);
		fi.write(reinterpret_cast<const char*>(&buf[0]), buf.size() * sizeof(BYTE));
		fi.close();
	}

	void TakeScreenShot(const char* BmpName)
	{
		RECT DesktopParams;
		HDC DevC = GetDC(hwnd);
		GetWindowRect(hwnd, &DesktopParams);
		DWORD Width = DesktopParams.right - DesktopParams.left;
		DWORD Height = DesktopParams.bottom - DesktopParams.top;

		DWORD FileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBTRIPLE) + 1 * (Width*Height * 4));
		char *BmpFileData = (char*)GlobalAlloc(0x0040, FileSize);

		PBITMAPFILEHEADER BFileHeader = (PBITMAPFILEHEADER)BmpFileData;
		PBITMAPINFOHEADER  BInfoHeader = (PBITMAPINFOHEADER)&BmpFileData[sizeof(BITMAPFILEHEADER)];

		BFileHeader->bfType = 0x4D42; // BM
		BFileHeader->bfSize = sizeof(BITMAPFILEHEADER);
		BFileHeader->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		BInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		BInfoHeader->biPlanes = 1;
		BInfoHeader->biBitCount = 24;
		BInfoHeader->biCompression = BI_RGB;
		BInfoHeader->biHeight = Height;
		BInfoHeader->biWidth = Width;

		RGBTRIPLE *Image = (RGBTRIPLE*)&BmpFileData[sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)];

		HDC CaptureDC = CreateCompatibleDC(DevC);
		HBITMAP CaptureBitmap = CreateCompatibleBitmap(DevC, Width, Height);
		SelectObject(CaptureDC, CaptureBitmap);
		BitBlt(CaptureDC, 0, 0, Width, Height, DevC, 0, 0, SRCCOPY | CAPTUREBLT);
		GetDIBits(CaptureDC, CaptureBitmap, 0, Height, Image, (LPBITMAPINFO)BInfoHeader, DIB_RGB_COLORS);

		DWORD Junk;
		HANDLE FH = CreateFileA(BmpName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
		WriteFile(FH, BmpFileData, FileSize, &Junk, 0);
		CloseHandle(FH);
		GlobalFree(BmpFileData);
	}

};