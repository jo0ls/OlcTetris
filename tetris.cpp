
// https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_Tetris.cp
// Credit to javidx one lone coder for the original version. 
// I tweaked the border, block character, colors, and you use UP or z to rotate.
// and space to slam a piece down.

#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
using namespace std;

wstring tetromino[7];
const int nFieldWidth = 12; // making these consts will stop a warning.
const int nFieldHeight = 18;
const int nScreenWidth = 80;
const int nScreenHeight = 30;
unsigned char* pField = nullptr;

int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px;
	case 1: return 12 + py - (px * 4);
	case 2: return 15 - (py * 4) - px;
	case 3: return 3 - py + (px * 4);
	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			int pi = Rotate(px, py, nRotation);
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0) 
						return false;
				}
			}
		}
	return true;
}

int main()
{
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L"..X.");
	tetromino[1].append(L"....");

	tetromino[2].append(L"....");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"..X.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".X..");
	tetromino[3].append(L"....");

	tetromino[4].append(L".X..");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L".X..");
	tetromino[5].append(L".X..");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"....");

	tetromino[6].append(L"..X.");
	tetromino[6].append(L"..X.");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L"....");

	pField = new unsigned char[nFieldWidth * nFieldHeight];
	int part;
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
		{
			// more wall parts.
			part = 0;
			if (x == 0)
			{
				// this is the left, so maybe left corner or wall.
				part = (y == nFieldHeight - 1) ? 9 : 10;
			}
			else if (x == nFieldWidth - 1)
			{
				// this is the right, corner or wall
				part = (y == nFieldHeight - 1) ? 11 : 12;
			}
			else if (y == nFieldHeight - 1)
			{
				// bottom
				part = 13;
			}
			pField[y * nFieldWidth + x] = part;
		}

	// Change from a 1D array of chars to display, to a 2D array of structures.
	// The structure will record the char to use and the color, set in the Attribute property.
	// This is 3 bit color. Black, Blue, Green, Cyan, Red, Magenta, Yellow, White and a strong/bold option.
	// These colors are customisable in the console properties though.
	CHAR_INFO screen[nScreenHeight][nScreenWidth];
	for (int x = 0; x < nScreenWidth; x++)
		for (int y = 0; y < nScreenHeight; y++)
		{
			// initialise 2D array to blank/black
			screen[y][x].Char.UnicodeChar = L' ';
			screen[y][x].Attributes = 0;
		}
	HANDLE  hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool gameOver = false;
	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;
	bool bKey[6];								// allow for up key too
  bool bRotateHold = true;
	int iSlamHold = 10;
	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;
	vector<int> vLines;

	// The stuff to call WriteConsoleOutput, declare here as it is reused each time we call.
	COORD bufferSize;
	bufferSize.X = nScreenWidth;
	bufferSize.Y = nScreenHeight;
	COORD zero;
	zero.X = 0;
	zero.Y = 0;
	SMALL_RECT rect;
	rect.Left = 2;
	rect.Top = 2;
	rect.Right = nScreenWidth + 2; // put the 2 offset here?
	rect.Bottom = nScreenHeight + 2;
	SMALL_RECT* pRect = &rect;

	while (!gameOver)
	{
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		for (int k = 0; k < 6; k++)  // bigger loop for UP as well
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26Z\x20"[k]))) != 0; // UP will also rotate // space to slam down?

		if (bKey[0])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			{
				nCurrentX += 1;
			}
		}
		if (bKey[1])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			{
				nCurrentX -= 1;
			}
		}
		if (bKey[2])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY += 1;
			}
		}
		if (bKey[3] || bKey[4])  // UP will rotate. change +- to alter rotation direction
		{
			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY));
			bRotateHold = true;
		}
		else
		{
			bRotateHold = false;		
		}
		
		if (iSlamHold < 1 && bKey[5])
		{
			// SPACE to slam it down
			while (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY++;
			}
			// make it run through the usual code after piece moved down
			bForceDown = true;
			nSpeedCounter = nSpeed;
			iSlamHold = 10; 
		}
		else
		{
			iSlamHold -= 1;  // wait at least 10 ticks between slams, otherwise it is way too fast
		}
	
		
		if (bForceDown )
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			{
				nCurrentY++;
			}
			else
			{
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10) nSpeed--;

				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						if (bLine)
						{
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;
				gameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
			nSpeedCounter = 0;
		}

		for (int x = 0; x < nFieldWidth; x++)  // 12
			for (int y = 0; y < nFieldHeight; y++) //  18
			{
				int index = pField[y * nFieldWidth + x];
				wchar_t piece = L" \x2588\x2588\x2588\x2588\x2588\x2588\x2588*'|'|-"[index];  // so put in solid blocks of color instead of A-G
				screen[y][x].Char.UnicodeChar = piece;
				// We want walls to be the same color, they have part numbers > 7
				screen[y][x].Attributes = (index > 7) ? 10 : index;
			}

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
			{
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
				{
					screen[nCurrentY + py][nCurrentX + px].Char.UnicodeChar = L'\x2588'; // block
					screen[nCurrentY + py][nCurrentX + px].Attributes = nCurrentPiece + 1;
				}
			}

		
	  // have to draw the score out differently.
		std::string str = "SCORE: " + std::to_string(nScore);
		for (int i = 0; i < static_cast<int>(str.length()); i++)
		{
			screen[2][nFieldWidth + 6 + i].Char.UnicodeChar = str[i];
			screen[2][nFieldWidth + 6 + i].Attributes = 15;
		}
		std::string str2 = "SPEED: " + std::to_string(nSpeedCounter);
		for (int i = 0; i < static_cast<int>(str2.length()); i++)
		{
			screen[3][nFieldWidth + 6 + i].Char.UnicodeChar = str2[i];
			screen[3][nFieldWidth + 6 + i].Attributes = 15;
		}

		
		if (!vLines.empty())
		{
			WriteConsoleOutput(hConsole, &screen[0][0], bufferSize, zero, pRect);
			this_thread::sleep_for(400ms);

			for (auto& v : vLines)
			{
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			}

			vLines.clear();
		}

	// And the different API call.
		WriteConsoleOutput(hConsole, &screen[0][0], bufferSize, zero, pRect);
	}

	CloseHandle(hConsole);
	cout << "Game Over Score:" << nScore << endl;
	system("pause");

	return 0;
}

