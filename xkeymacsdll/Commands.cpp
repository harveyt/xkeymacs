// Commands.cpp: implementation of the CCommands class
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Commands.h"
#include "xkeymacsdll.h"
#include "Utils.h"
#include <math.h>
#include <Imm.h>
#pragma data_seg(".xkmcs")
DWORD CCommands::m_nNumericArgument = 1;
BOOL CCommands::m_bSetMark = FALSE;
BOOL CCommands::m_bC_ = FALSE;
BOOL CCommands::m_bC_x = FALSE;
BOOL CCommands::m_bM_ = FALSE;
BOOL CCommands::m_bM_x = FALSE;
BOOL CCommands::m_bDefaultNumericArgument = TRUE;
BOOL CCommands::m_bNegativeNumericArgument = FALSE;
BOOL CCommands::m_bC_u = FALSE;
SEARCH_DIRECTION CCommands::m_SearchDirection = NA;
int (*CCommands::m_LastKillCommand)() = NULL;
int (*CCommands::m_LastCommand)() = NULL;
CPtrList CCommands::m_CaretPos;
CPtrList CCommands::m_ScrollInfo;
CPtrList CCommands::m_FindText;
BOOL CCommands::m_bFirstFindDialog = FALSE;
BOOL CCommands::m_bTemporarilyDisableXKeymacs = FALSE;
CArray<CClipboardSnap *, CClipboardSnap *> CCommands::m_oClipboardData;
OriginalWindowPosition_t CCommands::m_OriginalWindowPosition[MAX_WINDOW] = {'\0'};
BOOL CCommands::m_bIsSu = FALSE;
#pragma data_seg()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCommands::CCommands()
{

}

CCommands::~CCommands()
{

}

void CCommands::DepressKey(BYTE bVk)
{
	CXkeymacsDll::DepressKey(bVk);
}

void CCommands::ReleaseKey(BYTE bVk)
{
	CXkeymacsDll::ReleaseKey(bVk);
}

void CCommands::Kdu(BYTE bVk1, BYTE bVk2, BYTE bVk3, BYTE bVk4)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

	if (CXkeymacsDll::Get326Compatible() || CUtils::IsAtok()) {
		bIsShiftDown = FALSE;
	}

	if (bIsCtrlDown) {
		CUtils::UpdateKeyboardState(VK_CONTROL, 0);
		ReleaseKey(VK_CONTROL);
	}
	if (bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	if (bIsShiftDown) {
		ReleaseKey(VK_SHIFT);
	}

	if (bVk1) {
		CXkeymacsDll::Kdu(bVk1, m_nNumericArgument);
	}
	if (bVk2) {
		CXkeymacsDll::Kdu(bVk2, m_nNumericArgument);
	}
	if (bVk3) {
		CXkeymacsDll::Kdu(bVk3, m_nNumericArgument);
	}
	if (bVk4) {
		CXkeymacsDll::Kdu(bVk4, m_nNumericArgument);
	}

	if (bIsShiftDown) {
		DepressKey(VK_SHIFT);
	}
	if (bIsAltDown) {
		DepressKey(VK_MENU);
	}
	if (bIsCtrlDown) {
		DepressKey(VK_CONTROL);
		CUtils::UpdateKeyboardState(VK_CONTROL, 1);
	}
}

void CCommands::SdKduSu(BYTE bVk1, BYTE bVk2, BYTE bVk3, BYTE bVk4)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

	if (bIsCtrlDown) {
		ReleaseKey(VK_CONTROL);
	}
	if (bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	if (!bIsShiftDown) {
		DepressKey(VK_SHIFT);
	}

	CXkeymacsDll::Kdu(bVk1, m_nNumericArgument);
	if (bVk2) {
		CXkeymacsDll::Kdu(bVk2, m_nNumericArgument);
	}
	if (bVk3) {
		CXkeymacsDll::Kdu(bVk3, m_nNumericArgument);
	}
	if (bVk4) {
		CXkeymacsDll::Kdu(bVk4, m_nNumericArgument);
	}

	if (!bIsShiftDown) {
		if (CUtils::IsShuriken()) {
			m_bIsSu = TRUE;
		} else {
			ReleaseKey(VK_SHIFT);
		}
	}
	if (bIsAltDown) {
		DepressKey(VK_MENU);
	}
	if (bIsCtrlDown) {
		DepressKey(VK_CONTROL);
	}
}

void CCommands::Su()
{
	if (m_bIsSu) {
		ReleaseKey(VK_SHIFT);
		m_bIsSu = FALSE;
	}
}

void CCommands::CdKduCu(BYTE bVk1, BYTE bVk2)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);
//	if (!bVk1) {
//		CUtils::Log("%d, %d, %d", CXkeymacsDll::IsDown(VK_MENU, FALSE), CXkeymacsDll::IsDown(VK_LMENU, FALSE), CXkeymacsDll::IsDown(VK_RMENU, FALSE));
//	}
	if (!bIsCtrlDown) {
		DepressKey(VK_CONTROL);
		CUtils::UpdateKeyboardState(VK_CONTROL, 1);
	}
	if (bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	if (bIsShiftDown && !m_bSetMark) {
		ReleaseKey(VK_SHIFT);
	}

	if (bVk1) {
		CXkeymacsDll::Kdu(bVk1, m_nNumericArgument);
	}
	if (bVk2) {
		CXkeymacsDll::Kdu(bVk2, m_nNumericArgument);
	}

	if (bIsShiftDown && !m_bSetMark) {
		if (!CUtils::IsXPorLater()) {	// FIXME	Windows XP said VK_SHIFT was pressed physically after a user press VK_RSHIFT, so...
			DepressKey(VK_SHIFT);
		}
	}
	if (bIsAltDown) {
		DepressKey(VK_MENU);
	}
	if (!bIsCtrlDown) {
		CUtils::UpdateKeyboardState(VK_CONTROL, 0);
		ReleaseKey(VK_CONTROL);
	}
}

void CCommands::CdSdKduSuCu(BYTE bVk)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

	if (!bIsCtrlDown) {
		DepressKey(VK_CONTROL);
		CUtils::UpdateKeyboardState(VK_CONTROL, 1);
	}
	if (bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	if (!bIsShiftDown) {
		DepressKey(VK_SHIFT);
	}

	CXkeymacsDll::Kdu(bVk, m_nNumericArgument);

	if (!bIsShiftDown) {
		ReleaseKey(VK_SHIFT);
	}
	if (bIsAltDown) {
		DepressKey(VK_MENU);
	}
	if (!bIsCtrlDown) {
		CUtils::UpdateKeyboardState(VK_CONTROL, 0);
		ReleaseKey(VK_CONTROL);
	}
}

void CCommands::AdKduAu(BYTE bVk1, BYTE bVk2, BYTE bVk3)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

	if (bIsCtrlDown) {
		ReleaseKey(VK_CONTROL);
	}

	if (!bIsAltDown) {
		DepressKey(VK_MENU);
	}

	if (bIsShiftDown) {
		ReleaseKey(VK_SHIFT);
	}

	CXkeymacsDll::Kdu(bVk1, m_nNumericArgument);
	if (bVk2) {
		CXkeymacsDll::Kdu(bVk2, m_nNumericArgument);
	}
	if (bVk3) {
		CXkeymacsDll::Kdu(bVk3, m_nNumericArgument);
	}

	if (bIsShiftDown) {
		if (!CUtils::IsXPorLater()) {	// FIXME	Windows XP said VK_SHIFT was pressed physically after a user press VK_RSHIFT, so...
			DepressKey(VK_SHIFT);
		}
	}

	if (!bIsAltDown) {
		ReleaseKey(VK_MENU);
	}

	if (bIsCtrlDown) {
		DepressKey(VK_CONTROL);
	}
}

void CCommands::AdSdKduSuAu(BYTE bVk1)
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
	BOOL bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

	if (bIsCtrlDown) {
		ReleaseKey(VK_CONTROL);
	}

	if (!bIsAltDown) {
		DepressKey(VK_MENU);
	}

	if (!bIsShiftDown) {
		DepressKey(VK_SHIFT);
	}

	CXkeymacsDll::Kdu(bVk1, m_nNumericArgument);

	if (!bIsShiftDown) {
		ReleaseKey(VK_SHIFT);
	}

	if (!bIsAltDown) {
		ReleaseKey(VK_MENU);
	}

	if (bIsCtrlDown) {
		DepressKey(VK_CONTROL);
	}
}

// C-a: Home
int CCommands::BeginningOfLine()
{
	int nNumericArgument = 1;
	if (!m_bDefaultNumericArgument || m_bNegativeNumericArgument) {
		nNumericArgument = m_nNumericArgument;
		if (m_bNegativeNumericArgument) {
			if (nNumericArgument) {
				nNumericArgument *= -1;
			} else {
				nNumericArgument = -1;
			}
		}
	}
	ClearNumericArgument();

	if (nNumericArgument <= 0) {
		nNumericArgument = -1 * nNumericArgument + 1;
		while (nNumericArgument--) {
			ReleaseKey(VK_CONTROL);	// why?
			while (MoveCaret(VK_UP) != GOTO_HOOK);
		}
		ReleaseKey(VK_CONTROL);	// why?
		while (MoveCaret(VK_HOME) != GOTO_HOOK);
		DepressKey(VK_CONTROL);	// why?
	} else if (nNumericArgument == 1) {
		return Reset(MoveCaret(VK_HOME));
	} else if (2 <= nNumericArgument) {
		nNumericArgument -= 2;
		while (nNumericArgument--) {
			ReleaseKey(VK_CONTROL);	// why?
			while (MoveCaret(VK_DOWN) != GOTO_HOOK);
		}
		ReleaseKey(VK_CONTROL);	// why?
		while (MoveCaret(VK_END) != GOTO_HOOK);
		while (MoveCaret(VK_RIGHT) != GOTO_HOOK);
		DepressKey(VK_CONTROL);	// why?
	} else {
		ASSERT(0);
	}
	return Reset(GOTO_HOOK);
}

// C-b: Left
int CCommands::BackwardChar()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return ForwardChar();
	}

	return Reset(MoveCaret(VK_LEFT));
}

// M-b: backward-word
int CCommands::BackwardWord()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return ForwardWord();
	}

	return Reset(MoveCaret(VK_LEFT, TRUE));
}

// C-e: End
int CCommands::EndOfLine()
{
	int nNumericArgument = 1;
	if (!m_bDefaultNumericArgument || m_bNegativeNumericArgument) {
		nNumericArgument = m_nNumericArgument;
		if (m_bNegativeNumericArgument) {
			if (nNumericArgument) {
				nNumericArgument *= -1;
			} else {
				nNumericArgument = -1;
			}
		}
	}
	ClearNumericArgument();
	if (nNumericArgument <= 0) {
		nNumericArgument *= -1;
		while (nNumericArgument--) {
			ReleaseKey(VK_CONTROL);	// why?
			while (MoveCaret(VK_UP) != GOTO_HOOK);
		}
		ReleaseKey(VK_CONTROL);	// why?
		while (MoveCaret(VK_HOME) != GOTO_HOOK);
		while (MoveCaret(VK_LEFT) != GOTO_HOOK);
		DepressKey(VK_CONTROL);	// why?
	} else if (nNumericArgument == 1) {
		return (MoveCaret(VK_END));
	} else if (2 <= nNumericArgument) {
		--nNumericArgument;
		while (nNumericArgument--) {
			ReleaseKey(VK_CONTROL);	// why?
			while (MoveCaret(VK_DOWN) != GOTO_HOOK);
		}
		ReleaseKey(VK_CONTROL);	// why?
		while (MoveCaret(VK_END) != GOTO_HOOK);
		DepressKey(VK_CONTROL);	// why?
	} else {
		ASSERT(0);
	}
	return Reset(GOTO_HOOK);
}

// C-f: Right
int CCommands::ForwardChar()
{
//	TCHAR szWindowText[0x100] = {'\0'};
//	GetWindowText(GetForegroundWindow(), szWindowText, sizeof(szWindowText));
//	CUtils::Log("C-f: %s", szWindowText);

	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return BackwardChar();
	}

	return Reset(MoveCaret(VK_RIGHT));
}

// M-f: forward-word
int CCommands::ForwardWord()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return BackwardWord();
	}

	return Reset(MoveCaret(VK_RIGHT, TRUE));
}

// C-n: Down
int CCommands::NextLine()
{
//	CUtils::Log(_T("C-n"));

	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return PreviousLine();
	}

	return Reset(MoveCaret(VK_DOWN));
}

// C-o: open-line
int CCommands::OpenLine()
{
	static const int DEFAULT_NUMERIC_ARGUMENT = -1;
	static int nNumericArgument = DEFAULT_NUMERIC_ARGUMENT;

	if (nNumericArgument == DEFAULT_NUMERIC_ARGUMENT) {
		nNumericArgument = m_nNumericArgument;
		ClearNumericArgument();
	}

	if (CUtils::IsHidemaru()
	 || CUtils::IsTuruKameMail()) {
		static int nStep = 0;

		switch (nStep) {
		case 0:
			CdKduCu('M');
			nStep = 1;
			return Reset(GOTO_RECURSIVE);
		case 1:
			nStep = 0;
			Kdu(VK_UP, VK_END);
			if (--nNumericArgument) {
				return Reset(GOTO_RECURSIVE);
			}
			break;
		}
	} else {
		while (nNumericArgument--) {
			Kdu(VK_RETURN, VK_UP, VK_END);
		}
	}

	nNumericArgument = DEFAULT_NUMERIC_ARGUMENT;
	return Reset(GOTO_HOOK);
}

// C-p: Up
int CCommands::PreviousLine()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return NextLine();
	}

	return Reset(MoveCaret(VK_UP));
}

// M-%
int CCommands::QueryReplace()
{
	ClearNumericArgument();
	if (CUtils::IsBecky()) {
		CdKduCu('Q', 'A');
	} else if (CUtils::IsEclipse()
			|| CUtils::IsInternetExplorer()
			|| CUtils::IsLotusNotes()
			|| CUtils::IsSleipnir()) {
		CdKduCu('F');
	} else if (CUtils::IsAutla()
			|| CUtils::IsBorlandCppBuilder()
			|| CUtils::IseMemoPad()
			|| CUtils::IsNami2000()
			|| CUtils::IsStoryEditor()) {
		CdKduCu('R');
	} else if (CUtils::IsEdLeaf()) {
		AdKduAu('E', 'E');
	} else if (CUtils::IsPHPEditor()) {
		AdKduAu('S', 'F');
	} else {
		CdKduCu('H');
	}
	return Reset(GOTO_HOOK);
}

// C-M-%
int CCommands::QueryReplaceRegexp()
{
	return QueryReplace();
}

// M-<:
int CCommands::BeginningOfBuffer()
{
	ClearNumericArgument();
	return Reset(MoveCaret(VK_HOME, TRUE));
}

// M->:
int CCommands::EndOfBuffer()
{
	ClearNumericArgument();
	return Reset(MoveCaret(VK_END, TRUE));
}

// C-h: Back space
int CCommands::DeleteBackwardChar()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return DeleteChar();
	}

	Kdu(VK_BACK);
	m_bSetMark = FALSE;
	return Reset(GOTO_HOOK);
}

// C-d: Delete
int CCommands::DeleteChar()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return DeleteBackwardChar();
	}

	m_bSetMark = FALSE;
	Kdu(VK_DELETE);
	return Reset(GOTO_HOOK);
}

int CCommands::KillLineForAllFormat()
{
	return KillLine(TRUE, KillLineForAllFormat);
}

// C-k: Shift-End, Shift-Delete, if the cursor is not at EOL (is End Of Line)
//		Shift-Right, Cut, if the cursor is at EOL
int CCommands::KillLine()
{
	return KillLine(FALSE);
}

int CCommands::KillLine(BOOL bAllFormat, int (*pCommand)())
{
	static BOOL bKeepAllFormat = FALSE;

	// Do not try to do these command at once.
	// Clipboard has old data till you go out this function.
	static int nStep = 0;
	static int nPrevStep = 0;
	static int nRepeat = 0;
	static CString szClipboardText;
	static BOOL bContinuous = FALSE;
	static BOOL bTopOfFile = FALSE;

	if (nStep == nPrevStep) {
		++nRepeat;
	} else {
		nRepeat = 0;
		nPrevStep = nStep;
	}

	switch (nStep) {
	case 0:		// Clear Selection and get next character to confirm the cursor is at the EOF or not.
		XK_LOG(_T("C-k: 0"));
		bTopOfFile = FALSE;

		if (!m_bDefaultNumericArgument || m_bNegativeNumericArgument) {
			if (m_bNegativeNumericArgument || m_nNumericArgument == 0) {
				// Shift+Up x -n Shift+Home Ctrl+X
				SdKduSu(VK_UP);
				ClearNumericArgument();
				SdKduSu(VK_HOME);
			} else {
				// Shift+Down x (n - 1) Shift+End Shift+Right Ctrl+X
				--m_nNumericArgument;
				SdKduSu(VK_DOWN);
				ClearNumericArgument();
				SdKduSu(VK_END, VK_RIGHT);
			}
			CdKduCu('X');
			nStep = 6;
			return Reset(GOTO_RECURSIVE);
		}

		if (m_LastCommand == pCommand) {
			bContinuous = TRUE;
		} else {
			bContinuous = FALSE;
			if (m_bSetMark) {
				m_bSetMark = FALSE;
				DeactivateMark();
			}
		}

		if (CUtils::IsDirector()
		 || CUtils::IsEudora()) {
			nStep = 1;
			return Reset(GOTO_RECURSIVE);
		}

		if (CUtils::IsLotus123()) {
			// C-k: Shift-End, Control-x
			SdKduSu(VK_END);
			CdKduCu('X');
			return Reset(GOTO_HOOK);
		}

		CaptureClipboardData(0, CUtils::IsLotusNotes());

		if (!CopyNextCharacter()) {
			XK_LOG(_T("Error: CopyNextCharacter"));
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 4;
		return Reset(GOTO_RECURSIVE);
	case 1:		// Get back character to confirm the cursor is at the TOF or not only for Macromedia Director.
		XK_LOG(_T("C-k: 1"));
		nStep = 0;
		CaptureClipboardData();

		if (!CopyBackCharacter()) {
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:		// Confirm the cursor is at the TOF or not only for Macromedia Director.
		{
			XK_LOG(_T("C-k: 2"));
			nStep = 0;
			Su();

			if (CUtils::IsTOF()) {
				bTopOfFile = TRUE;
			}

			// CopyBackCharacter move cursor right
			Kdu(VK_LEFT);
			RestoreClipboardData();

			nStep = 3;
			return Reset(GOTO_RECURSIVE);
		}
	case 3:		// Get next character to confirm the cursor is at the EOF or not.
		XK_LOG(_T("C-k: 3"));
		nStep = 0;

		if (CUtils::IsLotus123()) {
			// C-k: Shift-End, Control-x
			SdKduSu(VK_END);
			CdKduCu('X');
			return Reset(GOTO_HOOK);
		}

		CaptureClipboardData();

		if (!CopyNextCharacter()) {
			XK_LOG(_T("Error: CopyNextCharacter"));
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 4;
		return Reset(GOTO_RECURSIVE);
	case 4:		// Cut line
		XK_LOG(_T("C-k: 4"));
		nStep = 0;
		Su();

		if ((CUtils::IsDirector()
		  || CUtils::IsEudora())
		 && !bTopOfFile) {
			// CopyNextCharacter move cursor left
			Kdu(VK_RIGHT);
		}

		CUtils::GetClipboardText(&szClipboardText);
		XK_LOG(_T("%x, %s"), szClipboardText.GetAt(0), szClipboardText);

		if (szClipboardText.IsEmpty()) {	// EOF
			XK_LOG(_T("C-k: EOF"));
			if (CUtils::IsLotusWordPro()
			 || CUtils::IsMicrosoftFrontPage()
			 || CUtils::IsMicrosoftWord()) {
				SdKduSu(VK_RIGHT);
				Cut_();
				nStep = 5;
				return Reset(GOTO_RECURSIVE);
			} else if (CUtils::IsLotusNotes()) {
				Kdu(VK_END);
				ReleaseKey(VK_CONTROL);	// Why is it needed?
				SdKduSu(VK_DOWN, VK_HOME);
				Copy_();
				DepressKey(VK_CONTROL);	// Why is it needed?
				nStep = 5;
				return Reset(GOTO_RECURSIVE);
			} else {
				// default
				Kdu(VK_RIGHT);
				RestoreClipboardData();
				XK_LOG(_T("C-k: 4-1"));
				return Reset(GOTO_HOOK);
			}
		}

		if (szClipboardText.GetAt(0) != VK_RETURN) {
			XK_LOG(_T("C-k: szClipboardText.GetAt(0) != VK_RETURN"));
			if (CUtils::IsDirector()) {
				ReleaseKey(VK_CONTROL);
				SdKduSu(VK_END);
				Copy_();
				DepressKey(VK_CONTROL);
			} else if (CUtils::IseMemoPad()
					|| CUtils::IsHidemaru()	// Hidemaru clears mark after copy.
					|| CUtils::IsOpenOffice()
					|| CUtils::IsTuruKameMail()) {
				SdKduSu(VK_END);
				Cut_();
			} else if (CUtils::IsLotusWordPro()
					|| CUtils::IsMicrosoftWord()) {
				SdKduSu(VK_END, VK_LEFT);
				Cut_();
			} else {
				SdKduSu(VK_END);
				Copy_();
			}
		} else {	// EOL
			XK_LOG(_T("C-k: EOL"));
			if (CUtils::IsDirector()) {
				ReleaseKey(VK_CONTROL);
				SdKduSu(VK_RIGHT);
				Copy_();
				DepressKey(VK_CONTROL);
			} else if (CUtils::IsEclipse()
					|| CUtils::IsStoryEditor()) {
				SdKduSu(VK_RIGHT);
				Copy_();
			} else if (CUtils::IseMemoPad()
					|| CUtils::IsHidemaru()	// Hidemaru clears mark after copy.
					|| CUtils::IsOpenOffice()
					|| CUtils::IsTuruKameMail()) {
				SdKduSu(VK_RIGHT);
				Cut_();
			} else {
				SdKduSu(VK_RIGHT);
				Copy_();
			}
		}

		nStep = 5;
		return Reset(GOTO_RECURSIVE);
	case 5:		// input return if XKeymacs not only line but also line feed code and set the kill-line data on the clipboard.
		XK_LOG(_T("C-k: 5"));
		nStep = 0;

		if (1000 < nRepeat) {
			XK_LOG(_T("C-k: 5-1"));
			nStep = 6;
			return Reset(GOTO_RECURSIVE);
		}

		XK_LOG(_T("C-k: 5-1-1"));
		Su();
		XK_LOG(_T("C-k: 5-1-2"));
		CaptureClipboardData(1, CUtils::IsLotusNotes());
		XK_LOG(_T("C-k: 5-1-3"));

//		if (m_oClipboardData.GetSize() < 1 || m_oClipboardData[0] == NULL) {
////			XK_LOG(_T("C-k: 5-2"));
//			nStep = 6;
//			return Reset(GOTO_RECURSIVE);
//		} else if (m_oClipboardData.GetSize() < 2 || m_oClipboardData[1] == NULL) {
////			XK_LOG(_T("C-k: 5-3"));
//			nStep = 5;
//			return Reset(GOTO_RECURSIVE);
//		}

		if (CUtils::IseMemoPad()
		 || CUtils::IsHidemaru()	// Hidemaru clears mark after copy.
		 || CUtils::IsLotusWordPro()
		 || CUtils::IsMicrosoftWord ()
		 || CUtils::IsOpenOffice()
		 || CUtils::IsTuruKameMail()) {
			// do nothing
			XK_LOG(_T("C-k: 5-4"));
		} else {
			// This changed will be required to avoid Thunderbird bug.
			// if (0 < FindReturnFromClipboardData(1) && !CUtils::IsExcel() && !CUtils::IsThunderbird()) {
			if (0 < FindReturnFromClipboardData(1) && !CUtils::IsExcel()) {
				XK_LOG(_T("C-k: 5-5"));
				SdKduSu(VK_LEFT);
				Copy_();
				nStep = 5;
				return Reset(GOTO_RECURSIVE);
			} else {
				Kdu(VK_DELETE);
				XK_LOG(_T("C-k: 5-6"));
			}
		}

		bKeepAllFormat = bAllFormat;
		if (!bKeepAllFormat
		 && !m_oClipboardData[1]->IsAllMergeableFormat()) {
			XK_LOG(_T("C-k: 5-7"));
			bKeepAllFormat = TRUE;
		}

		if (bContinuous && !bKeepAllFormat) {
			XK_LOG(_T("C-k: 5-8"));
			*m_oClipboardData[0] += *m_oClipboardData[1];
		} else {
			XK_LOG(_T("C-k: 5-9"));
			*m_oClipboardData[0] = *m_oClipboardData[1];
		}
		RestoreClipboardData(0);

		XK_LOG(_T("C-k: 5-10"));
		nStep = 6;
		return Reset(GOTO_RECURSIVE);
	case 6:		// add data into kill-ring
		XK_LOG(_T("C-k: 6"));
		nStep = 0;
		Su();

		if (bContinuous && bKeepAllFormat) {
			CXkeymacsDll::AddKillRing(FALSE);
		} else {
			CXkeymacsDll::AddKillRing();
		}
		return Reset(GOTO_HOOK);
		break;
	}

	return Reset(GOTO_HOOK);
}

// C-x u: C-/: C-_: Ctrl-Z
int CCommands::Undo()
{
	if (CUtils::IsMicrosoftWord()) {
		static BOOL bIsCtrlDown;
		static BOOL bIsAltDown;
		static BOOL bIsShiftDown;

		bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
		bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
		bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

		if (!bIsCtrlDown) {
			DepressKey(VK_CONTROL);
		}
		if (bIsAltDown) {
			ReleaseKey(VK_MENU);
		}
		if (bIsShiftDown) {
			ReleaseKey(VK_SHIFT);
		}

		PostMessage(GetFocus(), WM_KEYDOWN, 'Z', 0);

		if (bIsShiftDown) {
			DepressKey(VK_SHIFT);
		}
		if (bIsAltDown) {
			DepressKey(VK_MENU);
		}
		if (!bIsCtrlDown) {
			ReleaseKey(VK_CONTROL);
		}

	} else {
		CdKduCu('Z');
	}
	return Reset(GOTO_HOOK);
}

// C-x C-c: Alt-Space, Alt-C
int CCommands::SaveBuffersKillEmacs()
{
	ClearNumericArgument();

	HWND hWnd = GetForegroundWindow();
	if (hWnd) {
		OriginalWindowPosition_t *pOriginalWindowPosition = GetOriginalWindowPosition(hWnd);
		if (pOriginalWindowPosition && pOriginalWindowPosition->bMax[ROLL_UP_UNROLL]) {
			RollUpUnroll();
		}
	}

	if (CUtils::IsConsole()) {
		SystemMenu(CMD_CLOSE);
	} else if (CUtils::IsExplorer()) {
		if (!CUtils::IsXPorLater()) {
			ReleaseKey(VK_CONTROL);
		}
		AdKduAu(VK_F4);
	} else if (CUtils::IsInternetExplorer()
			|| CUtils::IsJavaW()) {
		AdKduAu(VK_F4);
	} else {
		AdKduAu(VK_SPACE, 'C');
	}
	return Reset(GOTO_HOOK);
}

// C-z: Alt-Space, Alt-N
int CCommands::IconifyOrDeiconifyFrame()
{
	ClearNumericArgument();
	if (CUtils::IsConsole()) {
		SystemMenu(CMD_MINIMIZE);
	} else {
		AdKduAu(VK_SPACE, 'N');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Restore()
{
	ClearNumericArgument();
	if (CUtils::IsConsole()) {
		SystemMenu(CMD_RESTORE);
	} else {
		AdKduAu(VK_SPACE, 'R');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Move()
{
	ClearNumericArgument();
	if (CUtils::IsConsole()) {
		SystemMenu(CMD_MOVE);
	} else {
		AdKduAu(VK_SPACE, 'M');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Size()
{
	ClearNumericArgument();
	if (CUtils::IsConsole()) {
		SystemMenu(CMD_SIZE);
	} else {
		AdKduAu(VK_SPACE, 'S');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Minimize()
{
	return IconifyOrDeiconifyFrame();
}

int CCommands::Maximize()
{
	ClearNumericArgument();

	HWND hWnd = GetForegroundWindow();
	if (!hWnd) {
		return Reset(GOTO_HOOK);
	}

	WINDOWPLACEMENT wndpl = {sizeof(WINDOWPLACEMENT)};
	if (!GetWindowPlacement(hWnd, &wndpl)) {
		return Reset(GOTO_HOOK);
	}

	if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
		wndpl.showCmd = SW_RESTORE;
	} else {
		wndpl.showCmd = SW_SHOWMAXIMIZED;
	}

	SetWindowPlacement(hWnd, &wndpl);
	return Reset(GOTO_HOOK);
}

OriginalWindowPosition_t* CCommands::GetOriginalWindowPosition(HWND hWnd)
{
	const int nOriginalWindowPosition = sizeof(m_OriginalWindowPosition) / sizeof(m_OriginalWindowPosition[0]);

	for (int i = 0; i < nOriginalWindowPosition; ++i) {
		if (m_OriginalWindowPosition[i].hWnd == hWnd) {
			return &m_OriginalWindowPosition[i];
		}
	}

	for (int j = 0; j < nOriginalWindowPosition; ++j) {
		if (m_OriginalWindowPosition[j].hWnd == 0) {
			m_OriginalWindowPosition[j].hWnd = hWnd;
			memset(&m_OriginalWindowPosition[(j + 1) % nOriginalWindowPosition], 0, sizeof(m_OriginalWindowPosition[0]));
			return &m_OriginalWindowPosition[j];
		}
	}

	ASSERT(0);
	return NULL;
}

int CCommands::Maximize(MAXIMIZE_DIRECTION direction)
{
	ClearNumericArgument();

	HWND hWnd = GetForegroundWindow();
	if (!hWnd) {
		return Reset(GOTO_HOOK);
	}

	RECT workarea = {'\0'};
	if (!SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0)) {
		return Reset(GOTO_HOOK);
	}

	RECT window = {'\0'};
	if (!GetWindowRect(hWnd, &window)) {
		return Reset(GOTO_HOOK);
	}

	OriginalWindowPosition_t *pOriginalWindowPosition = GetOriginalWindowPosition(hWnd);
	if (!pOriginalWindowPosition) {
		return Reset(GOTO_HOOK);
	}

	int X = window.left;
	int Y = window.top;
	int nWidth = window.right - window.left;
	int nHeight = window.bottom - window.top;

	switch (direction) {
	case VERTICAL:
		if (pOriginalWindowPosition->bMax[direction]) {
			Y = pOriginalWindowPosition->nOriginalY;
			nHeight = pOriginalWindowPosition->nOriginalHeight;
		} else {
			pOriginalWindowPosition->nOriginalY = Y;
			pOriginalWindowPosition->nOriginalHeight = nHeight;

			Y = workarea.top;
			nHeight = workarea.bottom - workarea.top;
		}
		break;
	case HORIZONTAL:
		if (pOriginalWindowPosition->bMax[direction]) {
			X = pOriginalWindowPosition->nOriginalX;
			nWidth = pOriginalWindowPosition->nOriginalWidth;
		} else {
			pOriginalWindowPosition->nOriginalX = X;
			pOriginalWindowPosition->nOriginalWidth = nWidth;

			X = workarea.left;
			nWidth = workarea.right - workarea.left;
		}
		break;
	case ROLL_UP_UNROLL:
		if (pOriginalWindowPosition->bMax[direction]) {
			nHeight = pOriginalWindowPosition->nOriginalHeight;
		} else {
			pOriginalWindowPosition->nOriginalHeight = nHeight;

			nHeight = 0x15;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	MoveWindow(hWnd, X, Y, nWidth, nHeight, TRUE);
	pOriginalWindowPosition->bMax[direction] = !pOriginalWindowPosition->bMax[direction];
	return Reset(GOTO_HOOK);
}

int CCommands::MaximizeVertically()
{
	return Maximize(VERTICAL);
}

int CCommands::MaximizeHorizontally()
{
	return Maximize(HORIZONTAL);
}

// C-Space: 
int CCommands::SetMarkCommand()
{
	ClearNumericArgument();
	if (m_bSetMark) {
		DeactivateMark();
	}
	m_bSetMark = TRUE;

	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_MARK);
	}

	return Reset(GOTO_HOOK);
}

// C-g: Keyboard quit
int CCommands::KeyboardQuit()
{
	ClearNumericArgument();
	if (CUtils::IsFindDialog()) {
		Kdu(VK_ESCAPE);
	}
	if (m_bSetMark) {
		m_bSetMark = FALSE;
		DeactivateMark();
	}
	SetTemporarilyDisableXKeymacs(FALSE);
	return Reset(GOTO_HOOK);
}

DWORD CCommands::ClickCaret()
{
	HWND hWnd = GetFocus();

	POINT CaretPos = {'\0'};
	if (!GetCaretPos(&CaretPos)) {
		return GetLastError();
	}
//	XK_LOG(_T("client: x = %d, y = %d"), CaretPos.x, CaretPos.y);

	if (!ClientToScreen(hWnd, &CaretPos)) {
		return GetLastError();
	}
//	XK_LOG(_T("screen: x = %d, y = %d"), CaretPos.x, CaretPos.y);

	POINT Delta = {0, 1};
	if (CUtils::IsSakuraEditor()) {
		Delta.x = 1;
		Delta.y = 29;	// OK: 29-42 NG: 28, 43
	}

	const POINT clickPoint = {CaretPos.x + Delta.x, CaretPos.y + Delta.y};
	return Click(&clickPoint);
}

DWORD CCommands::DeactivateMark()
{
	if (CUtils::IsOpenOffice()
	 || CUtils::IsVisualBasicEditor()) {
		// GetCaretPos always returen (x,y) = (0,0) on OpenOffice, so ...
		// GetCaretPos always returen (x,y) = (-2199,*) on M$ Excel VBE, so ...
		Kdu(VK_ESCAPE);
		return ERROR_SUCCESS;	// i.e. return 0;
	}
	if (CUtils::IsFlash()
	 || CUtils::IsInternetExplorer()
	 || CUtils::IsMicrosoftPowerPoint()
	 || CUtils::IsMicrosoftWord()
	 || CUtils::IsSleipnir()
	 || CUtils::IsThunderbird()) {
		// GetCaretPos always returen (x,y) = (0,0) on M$ Word and Thunderbird, so ...
		// GetCaretPos always returen start point on IE (and Sleipnir that uses IE), so ...
		Kdu(VK_RIGHT);
		return ERROR_SUCCESS;	// i.e. return 0;
	}
	if (CUtils::IsDirector()) {
		// GetCaretPos always returen (x,y) = (0,0) on Macromedia Director, so ...
		Kdu(VK_RIGHT, VK_LEFT);
		return ERROR_SUCCESS;	// i.e. return 0;
	}
	if (CUtils::IsExplorer()) {
		AdKduAu('E', 'A');
		AdKduAu('E', 'I');
		return ERROR_SUCCESS;	// i.e. return 0;
	}
	if (CUtils::IsVisualStudioDotNet()) {	// Ctrl + Click select a word on Visual Studio .NET
		ReleaseKey(VK_CONTROL);
		return ClickCaret();
	}

	return ClickCaret();
}

// C-x C-f: Alt-F, Alt-O
int CCommands::FindFile()
{
	ClearNumericArgument();
	if (CUtils::IsFlash()
	 || CUtils::IsSleipnir()) {
		CdKduCu('O');
	} else if (CUtils::IsVisualStudioDotNet()) {
		AdKduAu('F', 'O', 'F');
	} else {
		AdKduAu('F', 'O');
	}
	return Reset(GOTO_HOOK);
}

// C-i: Tab
int CCommands::TabToTabStop()
{
	ClearNumericArgument();
	if (m_bSetMark) {
		m_bSetMark = FALSE;
	}
	Kdu(VK_TAB);
	return Reset(GOTO_HOOK);
}

// C-m: Enter
int CCommands::Newline()
{
	Kdu(VK_RETURN);
	return Reset(GOTO_HOOK);
}

// C-x h: Ctrl-End, Ctrl-Shift-Home
int CCommands::MarkWholeBuffer()
{
	ClearNumericArgument();
	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_SELECT_ALL);
	} else if (CUtils::IsDreamweaver()
			|| CUtils::IsFireworks()
			|| CUtils::IsFlash()
			|| CUtils::IsIllustrator()
			|| CUtils::IsPaint()
			|| CUtils::IsPhotoshop()) {
		CdKduCu('A');
	} else if (CUtils::IsExplorer()
			|| CUtils::IsReget()) {
		Kdu(VK_END);
		m_bSetMark = TRUE;
		return Reset(MoveCaret(VK_HOME));
	} else {
		CdKduCu(VK_END);
		m_bSetMark = TRUE;
		return Reset(MoveCaret(VK_HOME, TRUE));
	}
	return Reset(GOTO_HOOK);
}

// C-x C-p:
int CCommands::MarkPage()
{
	return MarkWholeBuffer();
}

// C-x C-s: Alt-F, Alt-S
int CCommands::SaveBuffer()
{
	ClearNumericArgument();
	if (CUtils::IsFlash()
	 || CUtils::IsSleipnir()
	 || CUtils::IsThunderbird()) {
		CdKduCu('S');
		return Reset(GOTO_HOOK);
	} else {
		// default
		AdKduAu('F', 'S');
		return Reset(GOTO_HOOK);
	}
}

// C-v: Page Down
int CCommands::ScrollUp()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return ScrollDown();
	}

	if (m_bDefaultNumericArgument) {
		return Reset(MoveCaret(VK_NEXT));
	} else {
		while (m_nNumericArgument--) {
			SendMessage(GetFocus(), WM_VSCROLL, SB_LINEDOWN, NULL);
		}
	}
	return Reset(GOTO_HOOK);
}

// M-v: Page Up
int CCommands::ScrollDown()
{
	if (m_bNegativeNumericArgument) {
		m_bNegativeNumericArgument = FALSE;
		return ScrollUp();
	}

	if (m_bDefaultNumericArgument) {
		if (CUtils::IsFirefox()
		 || CUtils::IsVisualCpp()) {
			CdKduCu(0);	// dummy to clear Alt effect.
		}
		return Reset(MoveCaret(VK_PRIOR));
	} else {
		while (m_nNumericArgument--) {
			SendMessage(GetFocus(), WM_VSCROLL, SB_LINEUP, NULL);
		}
	}
	return Reset(GOTO_HOOK);
}

BOOL CCommands::bC_()
{
	return m_bC_;
}

void CCommands::bC_(BOOL b)
{
	m_bC_ = b;
	CXkeymacsDll::ModifyShell_NotifyIcon(CTRL_ICON, m_bC_);
}

BOOL CCommands::bC_x()
{
	return m_bC_x;
}

void CCommands::bC_x(BOOL b)
{
	m_bC_x = b;
	CXkeymacsDll::ModifyShell_NotifyIcon(CX_ICON, m_bC_x);
}

BOOL CCommands::bM_x()
{
	return m_bM_x;
}

void CCommands::bM_x(const BOOL b)
{
	m_bM_x = b;

	if (b) {
		CXkeymacsDll::ModifyM_xTip("");
	} else {
		CXkeymacsDll::ModifyM_xTip(NULL);
	}
}

// C-x: only set flag
int CCommands::C_x()
{
	if (CXkeymacsDll::GetEnableCUA() && m_bSetMark) {
		Cut();
	} else {
		bC_x(TRUE);
	}
	return Reset(GOTO_HOOKX);
}

// C-x C-w: Alt-F, Alt-A
int CCommands::WriteFile()
{
	ClearNumericArgument();
	if (CUtils::IsSleipnir()) {
		CdKduCu('S');
	} else if (CUtils::IsFlash()) {
		CdSdKduSuCu('S');
	} else {
		AdKduAu('F', 'A');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Paste()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_PASTE);
	} else {
		CdKduCu('V');
	}
	return Reset(GOTO_HOOK);
}

// C-w: Ctrl-X
int CCommands::KillRegion()
{
	static int nStep = 0;
	switch (nStep) {
	case 0:
		m_bSetMark = FALSE;
		ClearNumericArgument();
		CdKduCu('X');
		if (CUtils::IsExplorer()) {
			return Reset(GOTO_HOOK);
		}
		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:
		nStep = 0;
		CXkeymacsDll::AddKillRing();
		return Reset(GOTO_HOOK);
	}
	// dummy
	return Reset(GOTO_HOOK);
}

// M-w: Ctrl-C
int CCommands::KillRingSave()
{
	static int nStep = 0;
	switch (nStep) {
	case 0:
		ClearNumericArgument();
		m_bSetMark = FALSE;
		if (CUtils::IsConsole()) {
			SystemMenuEdit(CMD_COPY);
			return Reset(GOTO_HOOK);
		} else if (CUtils::IsMicrosoftPowerPoint()) {	// PowerPoint did not want to deactivate-mark. Temporary code?
			CdKduCu('X');
		} else {
			CdKduCu('C');
		}
		if (CUtils::IsExplorer()) {
			return Reset(GOTO_HOOK);
		}
		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:
		nStep = 0;
		CXkeymacsDll::AddKillRing();
		if (CUtils::IsMicrosoftPowerPoint()) {	// Because DeactivateMark move object like graph.
			CdKduCu('V');
		} else {
			DeactivateMark();
		}
		return Reset(GOTO_HOOK);
	}
	// dummy
	return Reset(GOTO_HOOK);
}

// C-y: Ctrl-V
int CCommands::Yank()
{
	m_bSetMark = FALSE;
	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_PASTE);
	} else {
		static CClipboardSnap* pSnap = NULL;
		if (!m_bDefaultNumericArgument) {
			DecreaseNumericArgument();
			CXkeymacsDll::IncreaseKillRingIndex(m_nNumericArgument);
			pSnap = CXkeymacsDll::GetKillRing(pSnap);
		} else if (pSnap) {
			pSnap = CXkeymacsDll::GetKillRing(pSnap);
		} else {
			if (CUtils::IsExcel() && m_LastKillCommand == KillRegion) {
				// do nothing to work C-w not Copy but Cut
			} else if (CUtils::IsLotusNotes() && (m_LastKillCommand == KillRegion || m_LastKillCommand == KillRingSave)) {
				// do nothing
			} else {
				pSnap = CXkeymacsDll::GetKillRing(pSnap, FALSE);
			}
		}
		CdKduCu('V');
		if (pSnap) {
			return Reset(GOTO_RECURSIVE);
		}
	}
	return Reset(GOTO_HOOK);
}

// M-y: yank-pop
int CCommands::YankPop()
{
	ClearNumericArgument();

	if (m_LastCommand == YankPop
	 || m_LastCommand == Yank) {
		static CClipboardSnap* pSnap = NULL;
		if (!pSnap) {
			CClipboardSnap* pOldSnap = NULL;
			pOldSnap = CXkeymacsDll::GetKillRing(pOldSnap);
			if (pOldSnap) {
				for (int i = 0; i < pOldSnap->GetCount(); ++i) {
					CdKduCu('Z');
				}
			} else {
				CdKduCu('Z');
			}
			CXkeymacsDll::IncreaseKillRingIndex();
		}
		pSnap = CXkeymacsDll::GetKillRing(pSnap);
		CdKduCu('V');
		if (pSnap) {
			return Reset(GOTO_RECURSIVE);
		}
	}

	return Reset(GOTO_HOOK);
}

// M-!: cmd.exe/command.com
int CCommands::ShellCommand()
{
	if (!CUtils::Run(_tgetenv(_T("XKEYMACS_SHELL")))) {
		CUtils::Run(_tgetenv(_T("COMSPEC")));
	}
	return Reset(GOTO_HOOK);
}

int CCommands::Ignore()
{
	Reset();	// If you delete this line, Ignore() works as Enable/Disable XKeymacs only on Release build. Why?
	return Reset(GOTO_HOOK);
}

void CCommands::bM_(BOOL b)
{
	m_bM_ = b;
	CXkeymacsDll::ModifyShell_NotifyIcon(META_ICON, m_bM_);
}

BOOL CCommands::bM_()
{
	return m_bM_;
}

// C-[
int CCommands::Meta()
{
	bM_(TRUE);
	return Reset(GOTO_HOOKX);
}

// Esc
int CCommands::MetaEscape()
{
	Meta();
	return Reset(GOTO_DO_NOTHING);
}

// Alt
int CCommands::MetaAlt()
{
	return Reset(GOTO_DO_NOTHING);
}

// C-0
int CCommands::NumericArgument0()
{
	return NumericArgument(0);
}

// C-1
int CCommands::NumericArgument1()
{
	return NumericArgument(1);
}

// C-2
int CCommands::NumericArgument2()
{
	return NumericArgument(2);
}

// C-3
int CCommands::NumericArgument3()
{
	return NumericArgument(3);
}

// C-4
int CCommands::NumericArgument4()
{
	return NumericArgument(4);
}

// C-5
int CCommands::NumericArgument5()
{
	return NumericArgument(5);
}

// C-6
int CCommands::NumericArgument6()
{
	return NumericArgument(6);
}

// C-7
int CCommands::NumericArgument7()
{
	return NumericArgument(7);
}

// C-8
int CCommands::NumericArgument8()
{
	return NumericArgument(8);
}

// C-9
int CCommands::NumericArgument9()
{
	return NumericArgument(9);
}

// C-#
int CCommands::NumericArgument(int n)
{
	if (m_bDefaultNumericArgument) {
		m_nNumericArgument = n;
		m_bDefaultNumericArgument = FALSE;
	} else {
		m_nNumericArgument = m_nNumericArgument * 10 + n;
	}
	return Reset(GOTO_HOOK0_9);
}

// C--, M--
int CCommands::NumericArgumentMinus()
{
	m_bNegativeNumericArgument = !m_bNegativeNumericArgument;
	return Reset(GOTO_HOOK0_9);
}

// C-u
int CCommands::UniversalArgument()
{
	if (m_bC_u) {
		m_nNumericArgument *= 4;
	} else {
		m_nNumericArgument = 4;
		m_bC_u = TRUE;
	}
	return Reset(GOTO_HOOKX);
}

// change function name and member variable name like bUniversalArgument()
BOOL CCommands::bC_u()
{
	return m_bC_u;
}

void CCommands::bC_u(BOOL b)
{
	m_bC_u = b;
}

int CCommands::Repeat(BYTE bVk)
{
	if (m_bDefaultNumericArgument) {
		return Reset(GOTO_DO_NOTHING);
	} else {
		CXkeymacsDll::Kdu(bVk, m_nNumericArgument);
		return Reset(GOTO_HOOK);
	}
}

// C-l: Recenter
int CCommands::Recenter()
{
	if (CUtils::IsBecky()) {
		static int nStep = 0;
		RECT ClientRect = {'\0'};
		GetClientRect(GetFocus(), &ClientRect);
		static int gap = INT_MAX;

		static POINT CaretPos = {'\0'};
		GetCaretPos(&CaretPos);

		switch (nStep) {
		case 0:
			nStep = 0;	// omaginai
			if (CaretPos.y < ClientRect.bottom / 2) {
				SendMessage(GetFocus(), WM_VSCROLL, SB_LINEUP, NULL);
			} else if (ClientRect.bottom / 2 < CaretPos.y) {
				SendMessage(GetFocus(), WM_VSCROLL, SB_LINEDOWN, NULL);
			} else {
				gap = INT_MAX;
				return Reset(GOTO_HOOK);
				break;
			}

			Kdu(VK_DOWN);
			nStep = 1;
			return Reset(GOTO_RECURSIVE);

		case 1:
			nStep = 0;
			// When a client screen can display 2n lines, the center line is (n + 1)th line.
			int newgap = CaretPos.y - ClientRect.bottom / 2;
			if (fabs((double)gap) <= fabs((double)newgap)) {
				if ((fabs((double)gap) == fabs((double)newgap)) && (gap != newgap) && newgap < 0) {
					SendMessage(GetFocus(), WM_VSCROLL, SB_LINEUP, NULL);
				}
				gap = INT_MAX;
				return Reset(GOTO_HOOK);
				break;
			}

			gap = newgap;
			return Reset(GOTO_RECURSIVE);
		}
	} else if (CUtils::IsMicrosoftWord()
			|| CUtils::IsThunderbird()) {
		RECT ClientRect = {'\0'};
		GetClientRect(GetFocus(), &ClientRect);
//		XK_LOG(_T("top = %d, bottom = %d, left = %d, right = %d"), ClientRect.top, ClientRect.bottom, ClientRect.left, ClientRect.right);

		POINT CaretPos = {'\0'};
		GetCaretPos(&CaretPos);

		static const int LENGTH_PER_SCROLL = 32;

		if (CaretPos.y < ClientRect.bottom / 2) {
			VScroll(SB_LINEUP, (ClientRect.bottom / 2 - CaretPos.y) / (LENGTH_PER_SCROLL / 2) + 1);
		} else if (ClientRect.bottom / 2 < CaretPos.y) {
			VScroll(SB_LINEDOWN, (CaretPos.y - ClientRect.bottom / 2) / (LENGTH_PER_SCROLL / 2));
		}

		return Reset(GOTO_HOOK);
	} else {
		RECT ClientRect = {'\0'};
		GetClientRect(GetFocus(), &ClientRect);
		int gap = INT_MAX;

//		XK_LOG(_T("top = %d, bottom = %d, left = %d, right = %d"), ClientRect.top, ClientRect.bottom, ClientRect.left, ClientRect.right);

		for (;;) {
			POINT CaretPos = {'\0'};
			GetCaretPos(&CaretPos);

			if (CaretPos.y < ClientRect.bottom / 2) {
				VScroll(SB_LINEUP);
			} else if (ClientRect.bottom / 2 < CaretPos.y) {
				VScroll(SB_LINEDOWN);
			} else {
				break;
			}

			// When a client screen can display 2n lines, the center line is (n + 1)th line.
			int newgap = CaretPos.y - ClientRect.bottom / 2;
			if (fabs((double)gap) <= fabs((double)newgap)) {
				if ((fabs((double)gap) == fabs((double)newgap)) && (gap != newgap) && newgap < 0) {
					VScroll(SB_LINEUP);
				}
				break;
			}
			gap = newgap;
		}
		return Reset(GOTO_HOOK);
	}
	return Reset(GOTO_HOOK);
}

// C-t: Shift-Left, Shift-Delete, Right Shift-Insert, if the cursor is not at EOL (is End Of Line)
//		Left, Shift-Right, Shift-Delete, Left, Shift-Insert, Right, if the cursor is at EOL
int CCommands::TransposeChars()
{
	ClearNumericArgument();	// tmp

	// Do not try to do these command at once.
	// Clipboard has old data till you go out this function.
	static int nStep = 0;
	switch (nStep) {
	case 0:		// Clear Selection
		if (m_bSetMark) {
			m_bSetMark = FALSE;
			Kdu(VK_RIGHT, VK_LEFT);
			nStep = 1;
			return Reset(GOTO_RECURSIVE);
		} else {
			// Do nothing. Go to case 1:
		}
	case 1:		// Get back character to confirm the cursor is at TOF or not.
		nStep = 0;
		CaptureClipboardData();

		// simple version
		if (CUtils::IsLotus123()) {
			// C-t: Left, Shift-Right, Shift-Delete, Left, Shift-Insert, Right
			Kdu(VK_LEFT);
			SdKduSu(VK_RIGHT, VK_DELETE);
			Kdu(VK_LEFT);
			SdKduSu(VK_INSERT);
			Kdu(VK_RIGHT);
			nStep = 7;
			return Reset(GOTO_RECURSIVE);
		}

		if (!CopyBackCharacter()) {
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:		// Get next character to confirm the cursor is at the EOF or not.
		{
			nStep = 0;
			Su();

			if (CUtils::IsTOF()) {
				Kdu(VK_LEFT);
				nStep = 7;
				return Reset(GOTO_RECURSIVE);
			}

			if (CUtils::IsDirector()) {
				// CopyBackCharacter move cursor right
				Kdu(VK_LEFT);
				ReleaseKey(VK_CONTROL);
			}

			if (CUtils::IsEudora()) {
				// CopyBackCharacter move cursor right
				Kdu(VK_LEFT);
			}

			if (!CopyNextCharacter()) {
				nStep = 7;
				return Reset(GOTO_RECURSIVE);
			}

			if (CUtils::IsDirector()) {
				DepressKey(VK_CONTROL);
			}

			nStep = 3;
			return Reset(GOTO_RECURSIVE);
		}
	case 3:		// Cut Left Character
		{
			nStep = 0;
			Su();

			CString szClipboardText;
			CUtils::GetClipboardText(&szClipboardText);
//			XK_LOG(_T("%x, %d, %s"), szClipboardText.GetAt(0), szClipboardText.GetLength(), szClipboardText);

			if (CUtils::IsDirector()
			 ||	CUtils::IsEudora()) {
				// CopyNextCharacter move cursor left
				Kdu(VK_RIGHT);
			}

			if (!szClipboardText.IsEmpty()
			 && szClipboardText.GetAt(0) != VK_RETURN) {
				if (CUtils::IseMemoPad()
				 || CUtils::IsFlash()
				 || CUtils::IsK2Editor()
				 || CUtils::IsShuriken()) {
					SdKduSu(VK_LEFT);
					CdKduCu('X');
				} else if (CUtils::IsDirector()) {
					ReleaseKey(VK_CONTROL);
					SdKduSu(VK_LEFT);
					CdKduCu('X');
					Kdu(VK_RIGHT);
					DepressKey(VK_CONTROL);
				} else {
					SdKduSu(VK_LEFT, VK_DELETE);
				}
				nStep = 4;
				return Reset(GOTO_RECURSIVE);
			} else {
				if (szClipboardText.IsEmpty()) {	// EOF
					if (CUtils::IsLotusNotes()
					 || CUtils::IsLotusWordPro()
					 || CUtils::IsMicrosoftFrontPage()
					 || CUtils::IsMicrosoftWord()) {
						// These applications say "EOL is EOF".
						Kdu(VK_END);
					} else {
						// default
						Kdu(VK_RIGHT);
					}
				}

				nStep = 5;
				return Reset(GOTO_RECURSIVE);
			}

			nStep = 7;
			return Reset(GOTO_RECURSIVE);
		}
	case 4:		// Paste the cut character
		nStep = 0;
		Su();
		Kdu(VK_RIGHT);
		if (CUtils::IsDirector()
		 || CUtils::IseMemoPad()
		 || CUtils::IsFlash()
		 || CUtils::IsK2Editor()
		 || CUtils::IsShuriken()) {
			CdKduCu('V');
		} else {
			SdKduSu(VK_INSERT);
		}
		nStep = 7;
		return Reset(GOTO_RECURSIVE);
	case 5:		// Cut character at the EOF.
		nStep = 0;
		if (CUtils::IsDirector()
		 || CUtils::IseMemoPad()
		 || CUtils::IsFlash()
		 || CUtils::IsK2Editor()
		 || CUtils::IsShuriken()) {
			SdKduSu(VK_LEFT);
			CdKduCu('X');
		} else {
			SdKduSu(VK_LEFT, VK_DELETE);
		}
		nStep = 6;
		return Reset(GOTO_RECURSIVE);
	case 6:		// Paste the cut character near the EOF.
		nStep = 0;
		Su();
		Kdu(VK_LEFT);
		if (CUtils::IsDirector()
		 || CUtils::IseMemoPad()
		 || CUtils::IsFlash()
		 || CUtils::IsK2Editor()
		 || CUtils::IsShuriken()) {
			DepressKey(VK_CONTROL);		// Why is this code needed?
			CdKduCu('V');
		} else {
			SdKduSu(VK_INSERT);
		}
		Kdu(VK_RIGHT);
		nStep = 7;
		return Reset(GOTO_RECURSIVE);
	case 7:		// Set the original data on the clipboard.
		nStep = 0;
		Su();
		RestoreClipboardData();
		return Reset(GOTO_HOOK);
	}
	return Reset(GOTO_HOOK);
}

void CCommands::Cut_()
{
	CdKduCu('X');
}

int CCommands::Cut()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	Cut_();
	return Reset(GOTO_HOOK);
}

void CCommands::Copy_()
{
	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_COPY);
	} else if (CUtils::IsEclipse()
			|| CUtils::IsStoryEditor()) {
		CdKduCu(VK_INSERT);
	} else {
		CdKduCu('C');
	}
}

int CCommands::Copy()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	Copy_();
	return Reset(GOTO_HOOK);
}

// Copy next character of the cursor if the caret is NOT at end of the file
// Move the caret to left if caret is at end of the file
BOOL CCommands::CopyNextCharacter()
{
	if (!CUtils::OpenClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!EmptyClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!CloseClipboard()) {
		ASSERT(0);
		return FALSE;
	}

	SdKduSu(VK_RIGHT);
	Copy_();
	Kdu(VK_LEFT);

	return TRUE;
}

BOOL CCommands::CopyBackCharacter()
{
	if (!CUtils::OpenClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!EmptyClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!CloseClipboard()) {
		ASSERT(0);
		return FALSE;
	}

	SdKduSu(VK_LEFT);
	Copy_();
	Kdu(VK_RIGHT);

	return TRUE;
}

BOOL CCommands::CopyPreviousLine()
{
	if (!CUtils::OpenClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!EmptyClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!CloseClipboard()) {
		ASSERT(0);
		return FALSE;
	}

	SdKduSu(VK_UP);
	Copy_();

	return TRUE;
}

BOOL CCommands::CopyCurrentLine()
{
	if (!CUtils::OpenClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!EmptyClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!CloseClipboard()) {
		ASSERT(0);
		return FALSE;
	}

	SdKduSu(VK_HOME);
	Copy_();

	return TRUE;
}

void CCommands::DecreaseNumericArgument()
{
	if (m_bNegativeNumericArgument) {
		++m_nNumericArgument;
	} else {
		if (m_nNumericArgument == 0) {
			++m_nNumericArgument;
			m_bNegativeNumericArgument = TRUE;
		} else {
			--m_nNumericArgument;
		}
	}
}

void CCommands::ClearNumericArgument()
{
	m_bDefaultNumericArgument = TRUE;
	m_nNumericArgument = 1;
	m_bNegativeNumericArgument = FALSE;
}

void CCommands::Reset()
{
	Reset(GOTO_HOOK);
}

int CCommands::Reset(int rc)
{
	switch(rc) {
	case GOTO_HOOK:
		ClearNumericArgument();
		m_bC_u = FALSE;
	case GOTO_HOOK0_9:
		bC_(FALSE);
		bC_x(FALSE);
		bM_(FALSE);
		bM_x(FALSE);
	case GOTO_HOOKX:
		break;

	case GOTO_RECURSIVE:
		break;

	case GOTO_DO_NOTHING:
		break;

	default:
		break;
	}
	return rc;
}

// C-q
int CCommands::EnableOrDisableXKeymacs()
{
	// dummy
	ASSERT(0);
	static int i = 0;	// If there is not this 2 line, EnableOrDisableXKeymacs, EnableXKeymacs and DisableXKeymacs has same function pointer.
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::EnableXKeymacs()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::DisableXKeymacs()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::GetNumericArgument()
{
	return m_nNumericArgument;
}

// C-r
int CCommands::IsearchBackward()
{
	return Search(BACKWARD);
}

// C-s
int CCommands::IsearchForward()
{
	return Search(FORWARD);
}

void CCommands::OpenFindDialog()
{
	if (CUtils::IsAutla()
	 || CUtils::IsBorlandCppBuilder()
	 || CUtils::IsCodeWarrior()
	 || CUtils::IsDana()
	 || CUtils::IseMemoPad()
	 || CUtils::IsJmEditor()
	 || CUtils::IsK2Editor()
	 || (CUtils::IsNotepad() && CUtils::IsNTor9x())
	 || CUtils::IsOedit()
	 || CUtils::IsOpenJane()
	 || CUtils::IsPHPEditor()
	 || CUtils::IsStoryEditor()
	 || CUtils::IsTeraPad()) {
		AdKduAu('S', 'F');
	} else if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_FIND);
	} else if (CUtils::IsSakuraEditor()) {
		AdKduAu('S', VK_RETURN);
	} else if (CUtils::IsNami2000()) {
		AdKduAu('D', 'F');
	} else if (CUtils::IsVisualStudioDotNet()) {
		AdKduAu('E', 'F', 'F');
	} else if (CUtils::IsDirector()) {
		AdKduAu('E', 'F', 'T');
	} else if (CUtils::IsAdobeReader()) {
		AdKduAu('E', 'S');
	} else if (CUtils::IsDWFM()
			|| CUtils::IsEggExplorer()
			|| CUtils::IsExplorer()
			|| CUtils::IsOutlookExpress()) {
		Kdu(VK_F3);
	} else if (CUtils::IsHidemaru()
			|| CUtils::IsLunascape()
			|| CUtils::IsMicrosoftPowerPoint()
			|| CUtils::IsMozilla()
			|| CUtils::IsNetscape()
			|| CUtils::IsOpera()
			|| CUtils::IsSleipnir()) {
		CdKduCu('F');
	} else if (CUtils::IsDreamweaver()
			|| CUtils::IsezHTML()
			|| CUtils::IsFlash()) {
		DepressKey(VK_CONTROL);		// Why is this coad needed?
		CdKduCu('F');
	} else if (CUtils::IsIPMessenger()) {
		DepressKey(VK_CONTROL);
		PostMessage(GetFocus(), WM_KEYDOWN, 'F', 0);
		PostMessage(GetFocus(), WM_KEYUP, 'F', 0);
		ReleaseKey(VK_CONTROL);
	} else {
		// default
		AdKduAu('E', 'F');
	}

	// Set Direction if Find dialog already exist
	if (CUtils::IsDWFM()
	 || CUtils::IsEggExplorer()
	 || CUtils::IsExplorer()
	 || CUtils::IsFirefox()
	 || CUtils::IsHidemaru()
	 || CUtils::IsK2Editor()
	 || CUtils::IsLotusWordPro()
	 || CUtils::IsMicrosoftPowerPoint()
	 || CUtils::IsMozilla()
	 || CUtils::IsMSDN()
	 || CUtils::IsNetscape()
	 || CUtils::IsOpera()
	 || CUtils::IsSakuraEditor()) {
	} else if (CUtils::IsInternetExplorer()
			|| CUtils::IsSleipnir()) {
		// I want to set direction but IE does not allow me.
	} else {
		SetSearchDirection();
	}
}

int CCommands::Search(SEARCH_DIRECTION direction)
{
//	XK_LOG(_T("%s"), CUtils::GetApplicationName());

	if (CUtils::IsIllustrator()
	 || CUtils::IsPaint()
	 || CUtils::IsPhotoshop()) {
		return Reset(GOTO_DO_NOTHING);
	}

	ClearNumericArgument();

	if ((direction != FORWARD) && (direction != BACKWARD)) {
		ASSERT(0);
	}

	if (CUtils::IsVisualCpp()
	 || CUtils::IsVisualStudioDotNet()) {
		switch (direction) {
		case FORWARD:
			CdKduCu('I');
			break;
		case BACKWARD:
			CdSdKduSuCu('I');
			break;
		default:
			ASSERT(0);
			break;
		}
		return Reset(GOTO_HOOK);
	}

	if (!CUtils::IsFindDialog()) {
//		XK_LOG(_T("Find Dialog is not."));

		// F3 or Shift+F3
		if (m_LastCommand == IsearchForward
		 || m_LastCommand == IsearchBackward) {
			if (CUtils::IsBecky()
			 || CUtils::IsFirefox()
			 || CUtils::IsHidemaru()
			 || CUtils::IsPHPEditor()) {
				if (m_LastCommand == IsearchForward && direction != FORWARD
				 || m_LastCommand == IsearchBackward && direction != BACKWARD) {
					// do nothing
				} else if (direction == FORWARD) {
					Kdu(VK_F3);
				} else if (direction == BACKWARD) {
					SdKduSu(VK_F3);
				}
				return Reset(GOTO_HOOK);
			} else if (CUtils::IsBorlandCppBuilder()) {
				if (m_LastCommand == IsearchForward && direction != FORWARD
				 || m_LastCommand == IsearchBackward && direction != BACKWARD) {
					AdKduAu('S', 'F');
					SetSearchDirection(direction);
					Kdu(VK_RETURN);
				} else {
					Kdu(VK_F3);
				}
				return Reset(GOTO_HOOK);
			}
		}

		m_SearchDirection = direction;
		OpenFindDialog();
	} else {
//		XK_LOG(_T("Find Dialog is opened."));

		if (CUtils::IsHidemaru()
		 || CUtils::IsK2Editor()
		 || CUtils::IsOedit()
		 || CUtils::IsSakuraEditor()) {
			BYTE bDirection = 0;

			if (direction == FORWARD) {
				bDirection = GetDirectionForwardKey();
			} else if (direction == BACKWARD) {
				bDirection = GetDirectionBackwardKey();
			} else {
				ASSERT(0);
			}
			AdKduAu(bDirection);
		} else if (CUtils::IsBorlandCppBuilder()) {
			if (m_SearchDirection != direction) {
				SetSearchDirection(direction);
			} else {
				Kdu(VK_RETURN);
			}
		} else if (CUtils::IsJmEditor()
				|| CUtils::IsPHPEditor()) {
			Kdu(VK_RETURN);
		} else if (CUtils::IsAutla()) {
			if (direction == FORWARD) {
				Kdu(VK_F3);
			} else if (direction == BACKWARD) {
				SdKduSu(VK_F3);
			} else {
				ASSERT(0);
			}
		} else {
			if (m_SearchDirection != direction) {
				// Set Direction if Find dialog already exist
				if (CUtils::IsConsole()) {
					SetSearchDirection(BACKWARD);	// bad style
					m_SearchDirection = direction;
				} else {
					SetSearchDirection(direction);
				}
				return Reset(GOTO_HOOK);
			}

			// "Find Next(F)" if Find dialog already exist
			if (CUtils::IsStoryEditor()) {
				AdKduAu('F');
			} else {
				Kdu(VK_RETURN);
			}

			// open Find dialog again
			CString szDialogTitle;
			CUtils::GetFindDialogTitle(&szDialogTitle);
			if (CUtils::IsNami2000()) {
				AdKduAu('D', 'F');
				SetSearchDirection();
			} else if (CUtils::IsOutlook()
					&& szDialogTitle.Compare(_T("����"))) {
				// Outlook (w/o Japanese version)
				AdKduAu('E', 'F');
			} else if (CUtils::IsVisualCpp()) {
				CdKduCu('F');
				DepressKey(VK_CONTROL);		// Why is this need???
			} else if (CUtils::IsCodeWarrior()
					|| CUtils::IsDana()
					|| CUtils::IsStoryEditor()
					|| CUtils::IsTeraPad()) {
				AdKduAu('S', 'F');
			} else if (CUtils::IsezHTML()) {
				CdKduCu('F');
			}

			// set cursor at end of FindWhat
			if (CUtils::IsMozilla()
			 || CUtils::IsNetscape()) {
				// do nothing
			} else if (CUtils::IsMicrosoftWord()) {
				CUtils::UpdateKeyboardState(VK_CONTROL, 0);
				ReleaseKey(VK_CONTROL);

				AdKduAu('N');
				Kdu(VK_ESCAPE);
				Kdu(VK_END);

				DepressKey(VK_CONTROL);
				CUtils::UpdateKeyboardState(VK_CONTROL, 1);
			} else {
				AdKduAu(GetFindWhatKey());
			}
			Kdu(VK_END);
		}
	}
	return Reset(GOTO_HOOK);
}

void CCommands::SetSearchDirection(SEARCH_DIRECTION direction)
{
//	XK_LOG(_T("SetSearchDirection: %d"), direction);
	if ((direction != FORWARD) && (direction != BACKWARD)) {
		return;
	}

	m_SearchDirection = direction;

	if (GetDirectionForwardKey() == 0 && GetDirectionBackwardKey() == 0) {
		Kdu(VK_END);
		return;
	} else if (CUtils::IsConsole()) {
		if (direction == BACKWARD) {	// bad style
			Kdu(VK_TAB, VK_TAB);
			Kdu(VK_RIGHT);
			SdKduSu(VK_TAB, VK_TAB);
		}
	} else if (CUtils::IsMicrosoftWord()) {
		CUtils::UpdateKeyboardState(VK_CONTROL, 0);
		ReleaseKey(VK_CONTROL);

		AdKduAu('M');
		AdKduAu(0xBA);	// VK_OEM_1	Used for miscellaneous characters; it can vary by keyboard. 
						//			Windows 2000/XP: For the US standard keyboard, the ';:' key
		Kdu(VK_UP, VK_UP);
		if (direction == BACKWARD) {
			Kdu(VK_DOWN);
		}
		AdKduAu('N');
		Kdu(VK_ESCAPE);
		Kdu(VK_END);

		DepressKey(VK_CONTROL);
		CUtils::UpdateKeyboardState(VK_CONTROL, 1);
	} else if (CUtils::IsLotusNotes()) {
		BYTE bDirection	= 0;

		if (direction == FORWARD) {
			bDirection = GetDirectionForwardKey();
		} else if (direction == BACKWARD) {
			bDirection = GetDirectionBackwardKey();
		}

		AdKduAu(GetSearchOptionKey(), bDirection, GetSearchOptionKey());
		{
			CString szDialogTitle;
			CUtils::GetFindDialogTitle(&szDialogTitle);

			if (!szDialogTitle.Compare(_T("����"))
			 || !szDialogTitle.Compare(_T("Find"))) {
				SdKduSu(VK_TAB);
			} else {
				SdKduSu(VK_TAB, VK_TAB);
			}
		}
		Kdu(VK_END);
	} else if (CUtils::IsBecky()) {
		AdKduAu(GetDirectionForwardKey());
		if (direction == BACKWARD) {
			Kdu(VK_UP);
		}
		Kdu(VK_TAB, VK_TAB, VK_TAB, VK_TAB);
		Kdu(VK_END);
	} else if (CUtils::IsMozilla()
			|| CUtils::IsNetscape()) {
		Kdu(VK_TAB, VK_TAB, VK_TAB, VK_SPACE);
		SdKduSu(VK_TAB, VK_TAB, VK_TAB);
	} else if (CUtils::IsezHTML()) {
		BYTE bDirection	= 0;

		if (direction == FORWARD) {
			bDirection = GetDirectionForwardKey();
		} else if (direction == BACKWARD) {
			bDirection = GetDirectionBackwardKey();
		}

		AdKduAu(bDirection);
		SdKduSu(VK_TAB, VK_TAB, VK_TAB, VK_TAB);
	} else {
		// default
		BYTE bDirection	= 0;

		if (direction == FORWARD) {
			bDirection = GetDirectionForwardKey();
		} else if (direction == BACKWARD) {
			bDirection = GetDirectionBackwardKey();
		}

//		XK_LOG(_T("aSetSearchDirection: Direction = %c, FindWhatKey = %c"), bDirection, GetFindWhatKey());
		AdKduAu(bDirection, GetFindWhatKey());
	}
}

// C-x C-j, C-o, C-\, C-BACKSLASH
int CCommands::ToggleInputMethod()
{
	SetInputMethodOpenStatus(TOGGLE_INPUT_METHOD);
	return Reset(GOTO_HOOK);
}

int CCommands::CompleteToggleInputMethod()
{
	SetInputMethodOpenStatus(TOGGLE_INPUT_METHOD, TRUE);
	return Reset(GOTO_HOOK);
}

int CCommands::OpenInputMethod()
{
	SetInputMethodOpenStatus(OPEN_INPUT_METHOD);
	return Reset(GOTO_HOOK);
}

int CCommands::CloseInputMethod()
{
	SetInputMethodOpenStatus(CLOSE_INPUT_METHOD);
	return Reset(GOTO_HOOK);
}

int CCommands::CompleteCloseInputMethod()
{
	SetInputMethodOpenStatus(CLOSE_INPUT_METHOD, TRUE);
	return Reset(GOTO_HOOK);
}

void CCommands::SetInputMethodOpenStatus(INPUT_METHOD_OPEN_STATUS status, BOOL isComplete)
{
	ClearNumericArgument();
	HKL hKL = GetKeyboardLayout(0);
	if (CUtils::IsConsole()) {
		Kdu(VK_KANJI);
	} else if (ImmIsIME(hKL)) {
		// default
		HWND hWnd = GetFocus();
		HIMC hIMC = ImmGetContext(hWnd);

		if (isComplete && ImmGetOpenStatus(hIMC)) {
			ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
		}

		switch (status) {
		case CLOSE_INPUT_METHOD:
			ImmSetOpenStatus(hIMC, FALSE);
			break;
		case OPEN_INPUT_METHOD:
			ImmSetOpenStatus(hIMC, TRUE);
			break;
		case TOGGLE_INPUT_METHOD:
			ImmSetOpenStatus(hIMC, !ImmGetOpenStatus(hIMC));
			break;
		default:
			ASSERT(0);
			break;
		}

		ImmReleaseContext(hWnd, hIMC);
	} else {
		Kdu(VK_KANJI);
	}
}

// C-x k: Ctrl+F4
int CCommands::KillBuffer()
{
	CdKduCu(VK_F4);
	return Reset(GOTO_HOOK);
}

BOOL CCommands::IsKillCommand(int (*nFunctionPointer)())
{
	for (int nCommandID = 1; nCommandID < sizeof(Commands) / sizeof(Commands[0]); ++nCommandID) {
		if (Commands[nCommandID].fCommand == nFunctionPointer) {
			return !_tcsncmp(Commands[nCommandID].szCommandName, _T("kill-"), 5);
		}
	}
	return FALSE;
}

void CCommands::SetLastCommand(int (__cdecl *LastCommand)(void))
{
	m_LastCommand = LastCommand;

	if (IsKillCommand(LastCommand)) {
		m_LastKillCommand = LastCommand;
	}
}

// M-d: 
int CCommands::KillWord()
{
	if (CUtils::IsMicrosoftPowerPoint()
	 || CUtils::IsThunderbird()) {
		CdKduCu(VK_DELETE);
		return Reset(GOTO_HOOK);
	}

	static int nStep = 0;
	static CString szClipboardText;
	static int nWhiteSpace = 0;
	switch (nStep) {
	case 0:
//		XK_LOG(_T("M-d: 0"));
		ClearNumericArgument();
		szClipboardText.Empty();
		nWhiteSpace = 0;
		CdSdKduSuCu(VK_RIGHT);
		CdKduCu('X');
		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:
		{
//			XK_LOG(_T("M-d: 1"));
			nStep = 0;
			CUtils::GetClipboardText(&szClipboardText);
			int nWordEnd = szClipboardText.GetLength();
			int nFirstSpace = szClipboardText.Find(_T(' '));
//			XK_LOG(_T("M-d: 1-1 _%s_%c"), szClipboardText, szClipboardText.GetAt(szClipboardText.GetLength() - 1));
			if (nFirstSpace == 0) {
//				XK_LOG(_T("M-d: 1-1-1"));
				nStep = 0;
				return Reset(GOTO_RECURSIVE);
			} else if (0 < nFirstSpace) {
//				XK_LOG(_T("M-d: 1-1-2"));
				nWordEnd = nFirstSpace;	// (nFirstSpace + 1) - 1
			}
			int nFirstTab = szClipboardText.Find(_T('\t'));
			if (nFirstTab == 0) {
//				XK_LOG(_T("M-d: 1-2-1"));
				nStep = 0;
				return Reset(GOTO_RECURSIVE);
			} else if (0 < nFirstTab && nFirstTab < nWordEnd) {
//				XK_LOG(_T("M-d: 1-2-2"));
				nWordEnd = nFirstTab;	// (nFirstTab + 1) - 1
			}
			if (nWordEnd == szClipboardText.GetLength()) {
//				XK_LOG(_T("M-d: 1-3-1"));
				nStep = 4;
			} else {
//				XK_LOG(_T("M-d: 1-3-2"));
				CString szEndWhiteSpace;
				nWhiteSpace = szClipboardText.GetLength() - nWordEnd;
				szEndWhiteSpace = szClipboardText.Right(nWhiteSpace);
				CUtils::SetClipboardText(&szEndWhiteSpace);
				szClipboardText = szClipboardText.Left(nWordEnd);
				nStep = 2;
			}
			return Reset(GOTO_RECURSIVE);
		}
	case 2:
//		XK_LOG(_T("M-d: 2"));
		nStep = 0;
		CdKduCu('V');
		nStep = 3;
		return Reset(GOTO_RECURSIVE);
	case 3:
//		XK_LOG(_T("M-d: 3"));
		nStep = 0;
		m_bDefaultNumericArgument = FALSE;
		m_nNumericArgument = nWhiteSpace;
		Kdu(VK_LEFT);
		nStep = 4;
		return Reset(GOTO_RECURSIVE);
	case 4:
//		XK_LOG(_T("M-d: 4"));
		nStep = 0;
		CUtils::SetClipboardText(&szClipboardText);
		nStep = 5;
		return Reset(GOTO_RECURSIVE);
	case 5:
//		XK_LOG(_T("M-d: 5"));
		nStep = 0;
		CXkeymacsDll::AddKillRing();
		m_bSetMark = FALSE;
		return Reset(GOTO_HOOK);
	}
	// dummy
	return Reset(GOTO_HOOK);
}

// M-DEL
int CCommands::BackwardKillWord()
{
	CdSdKduSuCu(VK_LEFT);
	Cut_();
	return Reset(GOTO_HOOK);
}

// Select All
int CCommands::SelectAll()
{
	if (CUtils::IsConsole()) {
		SystemMenuEdit(CMD_SELECT_ALL);
	} else {
		CdKduCu('A');
	}
	return Reset(GOTO_HOOK);
}

// C-x (
int CCommands::StartKbdMacro()
{
	if (CUtils::IsMicrosoftWord()) {
		AdKduAu('T', 'M', 'R');
	} else if (CUtils::IsHidemaru()) {
		SdKduSu(VK_F1);
	} else {
		CXkeymacsDll::DefiningMacro(TRUE);
	}
	return Reset(GOTO_HOOK);
}

// C-x )
int CCommands::EndKbdMacro()
{
	if (CUtils::IsMicrosoftWord()) {
		AdKduAu('T', 'M', 'R');
	} else if (CUtils::IsHidemaru()) {
		SdKduSu(VK_F1);
	} else {
		CXkeymacsDll::DefiningMacro(FALSE);
	}
	return Reset(GOTO_HOOK);
}

// C-x e
int CCommands::CallLastKbdMacro()
{
	if (CUtils::IsMicrosoftWord()) {
		AdKduAu('T', 'M', 'M');
	} else if (CUtils::IsHidemaru()) {
		int n = 1;
		if (!m_bDefaultNumericArgument) {
			n = m_nNumericArgument;
		}
		ClearNumericArgument();
		while (n--) {
			AdKduAu('M', 'P');
		}
	} else {
		while (m_nNumericArgument--) {
			CXkeymacsDll::CallMacro();
		}
	}
	return Reset(GOTO_HOOK);
}

int CCommands::SwitchBetweenInputLocales()
{
	BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);

	// Alt+Shift
	if (bIsCtrlDown) {
		ReleaseKey(VK_CONTROL);
	}
	DepressKey(VK_MENU);	// why?
	ReleaseKey(VK_MENU);
	DepressKey(VK_MENU);
	DepressKey(VK_SHIFT);
	ReleaseKey(VK_SHIFT);
	if (!bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	if (bIsCtrlDown) {
		DepressKey(VK_CONTROL);
	}

	return Reset(GOTO_HOOK);
}

// C-x b
int CCommands::SwitchToBuffer()
{
	ClearNumericArgument();
	// GetWindowInfo does not get information to find out if the window is MDI or SDI.
	CdKduCu(VK_TAB);
	return Reset(GOTO_HOOK);
}

int CCommands::C_()
{
	return Reset(GOTO_HOOKX);
}

int CCommands::C_Eisu()
{
	bC_(TRUE);
	return Reset(GOTO_HOOKX);
}

int CCommands::Escape()
{
	ClearNumericArgument();
	Kdu(VK_ESCAPE);
	return Reset(GOTO_HOOK);
}

int CCommands::Tilde()
{
	ClearNumericArgument();
	if (CXkeymacsData::Is106Keyboard()) {
		SdKduSu(0xDE);	// VK_OEM_7		Used for miscellaneous characters; it can vary by keyboard. 
		Su();			//				Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
	} else {
		SdKduSu(0xC0);	// VK_OEM_3		Used for miscellaneous characters; it can vary by keyboard. 
		Su();			//				Windows 2000/XP: For the US standard keyboard, the '`~' key
	}
	return Reset(GOTO_HOOK);
}

int CCommands::BackQuote()
{
	ClearNumericArgument();
	if (CXkeymacsData::Is106Keyboard()) {
		SdKduSu(0xC0);
		Su();
	} else {
		ReleaseKey(VK_SHIFT);
		Kdu(0xC0);	// VK_OEM_3		Used for miscellaneous characters; it can vary by keyboard. 
					//				Windows 2000/XP: For the US standard keyboard, the '`~' key
	}
	return Reset(GOTO_HOOK);
}

BOOL CCommands::IsSetMark()
{
	return m_bSetMark;
}

BYTE CCommands::GetFindWhatKey()
{
	BYTE bFindWhat = 'N';

	if (CUtils::IsBecky()
	 || CUtils::IsDana()
	 || CUtils::IsEclipse()
	 || CUtils::IsezHTML()
	 || CUtils::IsLotusNotes()
	 || CUtils::IsVisualBasicEditor()) {
		bFindWhat = 'F';
	} else if (CUtils::IseMemoPad()) {
		bFindWhat = 'K';
	} else if (CUtils::IsHidemaru()
			|| CUtils::IsJmEditor()) {
		bFindWhat = 'S';
	} else if (CUtils::IsBorlandCppBuilder()
			|| CUtils::IsNami2000()
			|| CUtils::IsStoryEditor()
			|| CUtils::IsTeraPad()) {
		bFindWhat = 'T';
	} else if (CUtils::IsAdobeReader()
			|| CUtils::IsAutla()
			|| CUtils::IsCodeWarrior()
			|| CUtils::IsFirefox()
			|| CUtils::IsFlash()
			|| CUtils::IsIPMessenger()
			|| CUtils::IsK2Editor()
			|| CUtils::IsOedit()
			|| CUtils::IsOpera()
			|| CUtils::IsOpenJane()
			|| CUtils::IsPHPEditor()
			|| CUtils::IsSakuraEditor()) {
		bFindWhat = 0;
	}

	return bFindWhat;
}

BYTE CCommands::GetSearchOptionKey()
{
	BYTE bSearchOption = 0;

	if (CUtils::IsLotusNotes()) {
		bSearchOption = 'O';
	}

	return bSearchOption;
}

BYTE CCommands::GetDirectionForwardKey()
{
	BYTE bDirectionForward = 'D';
	CString szDialogTitle;
	CUtils::GetFindDialogTitle(&szDialogTitle);

	if (CUtils::IsAdobeReader()
	 || CUtils::IsAutla()
	 || CUtils::IsCodeWarrior()
	 || CUtils::IsDirector()
	 || CUtils::IsDreamweaver()
	 || CUtils::IsExcel()
	 || CUtils::IsFirefox()
	 || CUtils::IsFireworks()
	 || CUtils::IsFlash()
	 || CUtils::IsIPMessenger()
	 || CUtils::IsJmEditor()
	 || CUtils::IsLotus123()
	 || CUtils::IsMicrosoftPowerPoint()
	 || CUtils::IsOpera()
	 || CUtils::IsOpenJane()
	 || CUtils::IsStoryEditor()
	 || CUtils::IsVisualBasicEditor()
	 || CUtils::IsVisualStudioDotNet()
	 || CUtils::IsWordpad()) {
		bDirectionForward = 0;
	} else if (CUtils::IsLotusNotes()
			|| CUtils::IsNami2000()
			|| CUtils::IsSakuraEditor()) {
		bDirectionForward = 'D';
	} else if (CUtils::IsOedit()) {
		bDirectionForward = 'F';
	} else if (CUtils::IsBecky()) {
		if (!szDialogTitle.Compare(_T("�{��������"))) {
			bDirectionForward = 'O';
		} else {
			bDirectionForward = 0;
		}
	} else if (CUtils::IsK2Editor()) {
		bDirectionForward = 'L';
	} else if (CUtils::IseMemoPad()
			|| CUtils::IsHidemaru()) {
		bDirectionForward = 'N';
	} else if (CUtils::IsEclipse()) {
		bDirectionForward = 'O';
	}

	if (CUtils::IsOutlook()
	 && !szDialogTitle.Compare(_T("Find"))) {
		// Outlook (English version)
		bDirectionForward = 0;
	}

	return bDirectionForward;
}

BYTE CCommands::GetDirectionBackwardKey()
{
	BYTE bDirectionBackward	= 'U';
	CString szDialogTitle;
	CUtils::GetFindDialogTitle(&szDialogTitle);

	if (CUtils::IsAdobeReader()
	 || CUtils::IsAutla()
	 || CUtils::IsCodeWarrior()
	 || CUtils::IsDirector()
	 || CUtils::IsDreamweaver()
	 || CUtils::IsExcel()
	 || CUtils::IsFirefox()
	 || CUtils::IsFireworks()
	 || CUtils::IsFlash()
	 || CUtils::IsIPMessenger()
	 || CUtils::IsJmEditor()
	 || CUtils::IsLotus123()
	 || CUtils::IsMicrosoftPowerPoint()
	 || CUtils::IsOpera()
	 || CUtils::IsOpenJane()
	 || CUtils::IsStoryEditor()
	 || CUtils::IsVisualBasicEditor()
	 || CUtils::IsWordpad()) {
		bDirectionBackward = 0;
	} else if (CUtils::IsBorlandCppBuilder()
			|| CUtils::IsEclipse()
			|| CUtils::IseMemoPad()
			|| CUtils::IsLotusNotes()
			|| CUtils::IsNami2000()) {
		bDirectionBackward = 'B';
	} else if (CUtils::IsBecky()) {
		if (!szDialogTitle.Compare(_T("�{��������"))) {
			bDirectionBackward = 'B';
		} else {
			bDirectionBackward = 0;
		}
	} else if (CUtils::IsHidemaru()
			|| CUtils::IsOedit()) {
		bDirectionBackward = 'P';
	} else if (CUtils::IsK2Editor()
			|| CUtils::IsSakuraEditor()) {
		bDirectionBackward = 'U';
	}

	if (CUtils::IsOutlook()
	 && !szDialogTitle.Compare(_T("Find"))) {
		// Outlook (English version)
		bDirectionBackward = 0;
	}

	return bDirectionBackward;
}

void CCommands::GetCaretPosition()
{
//	XK_LOG(_T("GetClipboardTextLength = %d"), CUtils::GetClipboardTextLength());

	try {
		POINT *pCaretPos = new POINT;
		if (pCaretPos && GetCaretPos(pCaretPos)) {
//			XK_LOG(_T("x = %d, y = %d"), pCaretPos->x, pCaretPos->y);
			m_CaretPos.AddTail(pCaretPos);
		} else {
			delete pCaretPos;
			pCaretPos = NULL;
		}
	}
	catch (CMemoryException* e) {
		e->Delete();
//		XK_LOG("GetCaretPosition: pCaretPos: 'new' threw an exception");
	}

	try {
		SCROLLINFO *pScrollInfo = new SCROLLINFO;
		if (pScrollInfo) {
			memset(pScrollInfo, 0, sizeof(SCROLLINFO));
			pScrollInfo->cbSize = sizeof(SCROLLINFO);
			pScrollInfo->fMask = SIF_ALL;
			if (GetScrollInfo(GetFocus(), SB_VERT, pScrollInfo)) {
//				XK_LOG(_T("page = %d, pos = %d, max = %d, min = %d, trackpos = %d"), pScrollInfo->nPage, pScrollInfo->nPos, pScrollInfo->nMax, pScrollInfo->nMin, pScrollInfo->nTrackPos);
				m_ScrollInfo.AddTail(pScrollInfo);
			} else {
//				XK_LOG(_T("GetScrollInfo Error: %d"), GetLastError());
				delete pScrollInfo;
				pScrollInfo = NULL;
			}
		}
	}
	catch (CMemoryException* e) {
		e->Delete();
//		XK_LOG("GetCaretPosition: pScrollInfo: 'new' threw an exception");
	}

	try {
		CString* pClipboardText = new CString;
		if (pClipboardText) {
			CUtils::GetClipboardText(pClipboardText);
			CUtils::SetClipboardText(pClipboardText);
			m_FindText.AddTail(pClipboardText);
		}
	}
	catch (CMemoryException* e) {
		e->Delete();
//		XK_LOG("GetCaretPosition: pClipboardText: 'new' threw an exception");
	}
}

BOOL CCommands::SetCaretPosition()
{
	{
//		TCHAR buf[0x100] = {'\0'};
//		GetWindowText(GetForegroundWindow(), buf, sizeof(buf));
//		XK_LOG(buf);
	}

	if (!m_ScrollInfo.IsEmpty()) {
		SCROLLINFO* pScrollInfo = (SCROLLINFO*)m_ScrollInfo.GetTail();
		delete pScrollInfo;
		pScrollInfo = NULL;
		m_ScrollInfo.RemoveTail();
	}
	if (!m_ScrollInfo.IsEmpty()) {
		SCROLLINFO* pScrollInfo = (SCROLLINFO*)m_ScrollInfo.GetTail();
		SendMessage(GetFocus(), WM_VSCROLL, SB_THUMBPOSITION | (pScrollInfo->nPos << 0x10), NULL);
	}

	BOOL bMoveCaret = FALSE;
	if (!m_CaretPos.IsEmpty()) {
		POINT* pCaretPos = (POINT*)m_CaretPos.GetTail();
		delete pCaretPos;
		pCaretPos = NULL;
		m_CaretPos.RemoveTail();
	}
	if (!m_CaretPos.IsEmpty()) {
		POINT* pCaretPos = (POINT*)m_CaretPos.GetTail();
		SetCaretPos(pCaretPos->x, pCaretPos->y);

//		XK_LOG(_T("set x = %d), y = %d", p->x, p->y);

		DeactivateMark();

		const int nPreviousFindTextLength = CUtils::GetClipboardTextLength();
		if (0 < nPreviousFindTextLength) {
			if (CUtils::IsWordpad()) {
				POSITION position = m_FindText.GetTailPosition();
				CString* pPreviousFindText = (CString*)m_FindText.GetPrev(position);
				CString* pNextFindText = (CString*)m_FindText.GetPrev(position);
				if (*pPreviousFindText == *pNextFindText) {
					for (int i = 0; i <= nPreviousFindTextLength; ++i) {
						Kdu(VK_LEFT);
					}
				} else {
					Kdu(VK_LEFT);
				}
			}
			bMoveCaret = TRUE;
		}
	} else {
		Kdu(VK_LEFT);
	}

//	XK_LOG(_T("scrollInfo.GetCount() = %d, szFindText.GetLength() = %d"), scrollInfo.GetCount(), szFindText.GetLength());

	if (!m_FindText.IsEmpty()) {
		CString *pFindText = (CString *)m_FindText.GetTail();
		delete pFindText;
		pFindText = NULL;
		m_FindText.RemoveTail();
	}
	if (!m_FindText.IsEmpty()) {
		CString *pFindText = (CString *)m_FindText.GetTail();
		CUtils::SetClipboardText(pFindText);
	}

	return bMoveCaret;
}

void CCommands::SetMark(BOOL bSetMark)
{
	m_bSetMark = bSetMark;
}

GOTO CCommands::MoveCaret(BYTE bVk, BOOL bCtrl)
{
	switch (bVk) {
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
		break;
	default:
		return CONTINUE;
	}

	struct {
		BYTE bVk;
		BOOL bCtrl;
		DWORD time;
	} static last;

	const DWORD time = GetTickCount();
//	XK_LOG(_T("%d - %d = %d, %d, %d"), time, last.time, time - last.time, CXkeymacsDll::GetMaxKeyInterval(), CXkeymacsDll::GetAccelerate());
	if (CXkeymacsDll::GetAccelerate() && bVk == last.bVk && bCtrl == last.bCtrl && time - last.time < CXkeymacsDll::GetMaxKeyInterval()) {
		NumericArgument(CXkeymacsDll::GetAccelerate());
	}
	last.bVk = bVk;
	last.bCtrl = bCtrl;
	last.time = time;

	if (bCtrl) {
		if (m_bSetMark) {
			CdSdKduSuCu(bVk);
		} else {
			CdKduCu(bVk);
		}
	} else {
		if (m_bSetMark) {
			if (CUtils::IsShuriken()) {
				static BOOL bIsShiftDown = FALSE;
				static int nStep = 0;
				switch (nStep) {
				case 0:
					{
						BOOL bIsCtrlDown = CXkeymacsDll::IsDown(VK_CONTROL, FALSE);
						BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);
						bIsShiftDown = CXkeymacsDll::IsDown(VK_SHIFT, FALSE);

						if (bIsCtrlDown) {
							ReleaseKey(VK_CONTROL);
						}
						if (bIsAltDown) {
							ReleaseKey(VK_MENU);
						}
						if (!bIsShiftDown) {
							DepressKey(VK_SHIFT);
						}

						CXkeymacsDll::Kdu(bVk, m_nNumericArgument);

						if (bIsAltDown) {
							DepressKey(VK_MENU);
						}
						if (bIsCtrlDown) {
							DepressKey(VK_CONTROL);
						}
						nStep = 1;
						return GOTO_RECURSIVE;
					}
				case 1:
					nStep = 0;
					if (!bIsShiftDown) {
						ReleaseKey(VK_SHIFT);
					}
					return GOTO_HOOK;
				}
			} else {
				SdKduSu(bVk);
			}
		} else {
			Kdu(bVk);
		}
	}
	return GOTO_HOOK;
}

int CCommands::PassThrough()
{
	Reset();	// Why?
	return Reset(GOTO_HOOK);
}

int CCommands::Redo()
{
	if (CUtils::IsCorelDRAW()) {
		CdSdKduSuCu('Z');
	} else {
		CdKduCu('Y');
	}
	return Reset(GOTO_HOOK);
}

int CCommands::TemporarilyDisableXKeymacs()
{
//	XK_LOG(_T("SetTemporarilyDisableXKeymacs"));
	SetTemporarilyDisableXKeymacs(TRUE);
	Reset();	// Why?
	return Reset(GOTO_HOOK);
}

BOOL CCommands::IsTemporarilyDisableXKeymacs()
{
	return m_bTemporarilyDisableXKeymacs;
}

void CCommands::SetTemporarilyDisableXKeymacs(BOOL bTemporarilyDisableXKeymacs)
{
	m_bTemporarilyDisableXKeymacs = bTemporarilyDisableXKeymacs;
	CXkeymacsDll::SetKeyboardHookFlag();
}

int CCommands::New()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	CdKduCu('N');
	return Reset(GOTO_HOOK);
}

int CCommands::Print()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	CdKduCu('P');
	return Reset(GOTO_HOOK);
}

int CCommands::Find()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	CdKduCu('F');
	return Reset(GOTO_HOOK);
}

int CCommands::FindNext()
{
	ClearNumericArgument();
	m_bSetMark = FALSE;
	Kdu(VK_F3);
	return Reset(GOTO_HOOK);
}

int CCommands::SystemMenu(const CONSOLE_MENU nDown)
{
//	XK_LOG(_T("SystemMenu"));
	if (!CUtils::IsConsole()) {
		return ERROR_SUCCESS;
	}

	RECT consoleWindowRect = {'\0'};
	if (!GetWindowRect(GetForegroundWindow(), &consoleWindowRect)) {
		return GetLastError();
	}
//	XK_LOG(_T("t = %d, l = %d"), consoleWindowRect.top, consoleWindowRect.left);

	const POINT clickPoint = {consoleWindowRect.left + 5, consoleWindowRect.top + 5};
	int rc = Click(&clickPoint);
	if (rc != ERROR_SUCCESS) {
		return rc;
	}

	if (nDown != CMD_EDIT) {
		for (int i = 0; i <= nDown; ++i) {
			Kdu(VK_DOWN);
		}
		Kdu(VK_UP);
		Kdu(VK_RETURN);
	}
	return ERROR_SUCCESS;	// i.e. return 0;
}

int CCommands::SystemMenuEdit(const CONSOLE_MENU_EDIT nDown)
{
	if (!CUtils::IsConsole()) {
		return ERROR_SUCCESS;
	}

	SystemMenu(CMD_EDIT);
	RECT consoleWindowRect = {'\0'};
	if (!GetWindowRect(GetForegroundWindow(), &consoleWindowRect)) {
		return GetLastError();
	}

	const int y = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU) * 6 + 20;// y = A + B * 6 + 20
	const POINT clickPoint = {consoleWindowRect.left + 50, consoleWindowRect.top + y};	// 146 <= y <= 161 on W2K	(146 + 161 + 1) / 2 = 154 = 19 + 19 x 6 + 21
	int rc = Click(&clickPoint);														// 157 <= y <= 172 on WXP	(157 + 172 + 1) / 2 = 165 = 26 + 20 x 6 + 19
	if (rc != ERROR_SUCCESS) {
		return rc;
	}

	for (int i = 0; i <= nDown; ++i) {
		Kdu(VK_DOWN);
	}
	Kdu(VK_UP);	// Why? Because Mark did not work well only one VK_DOWN (or seven VK_DOWNs).

	Kdu(VK_RETURN);
	return ERROR_SUCCESS;	// i.e. return 0;
}

int CCommands::Click(const POINT *const pClickPoint)
{
	BOOL bIsAltDown = CXkeymacsDll::IsDown(VK_MENU, FALSE);

	POINT CursorPos = {'\0'};
	if (!GetCursorPos(&CursorPos)) {
		return GetLastError();
	}

	if (!SetCursorPos(pClickPoint->x, pClickPoint->y)) {
		return GetLastError();
	}
	if (CUtils::IsConsole()
	 && bIsAltDown) {
		ReleaseKey(VK_MENU);
	}
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
	if (!SetCursorPos(CursorPos.x, CursorPos.y)) {
		return GetLastError();
	}
	return ERROR_SUCCESS;
}

// C-x C-t
int CCommands::TransposeLines()
{
	static int nStep = 0;
	switch (nStep) {
	case 0:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 0"));
		Kdu(VK_END);
		CaptureClipboardData();
		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 1"));
		if (!CopyNextCharacter()) {
			return Reset(GOTO_DO_NOTHING);
		}
		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 2"));
		Su();
		if (CUtils::IsEOF()) {
			Kdu(VK_END, VK_RETURN, VK_UP, VK_END);
		}
		Kdu(VK_HOME);
		nStep = 3;
		return Reset(GOTO_RECURSIVE);
	case 3:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 3"));
		if (!CopyBackCharacter()) {
			return Reset(GOTO_DO_NOTHING);
		}
		nStep = 4;
		return Reset(GOTO_RECURSIVE);
	case 4:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 4"));
		Su();
		if (CUtils::IsTOF()) {	// TOF
			Kdu(VK_DOWN, VK_END);
//			XK_LOG(_T("C-x C-t: TOF"));
			nStep = 5;
		} else {
			nStep = 7;
		}
		return Reset(GOTO_RECURSIVE);
	case 5:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 5"));
		if (!CopyNextCharacter()) {
			return Reset(GOTO_DO_NOTHING);
		}
		nStep = 6;
		return Reset(GOTO_RECURSIVE);
	case 6:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 6"));
		Su();
		if (CUtils::IsEOF()) {
			Kdu(VK_END, VK_RETURN, VK_UP, VK_END);
//			XK_LOG(_T("C-x C-t: EOF2"));
		}
		nStep = 7;
		return Reset(GOTO_RECURSIVE);
	case 7:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 7"));
		Kdu(VK_UP, VK_HOME);
		SdKduSu(VK_DOWN);
		CdKduCu('X');
		nStep = 8;
		return Reset(GOTO_RECURSIVE);
	case 8:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 8"));
		Su();
		Kdu(VK_DOWN);
		CdKduCu('V');
		nStep = 9;
		return Reset(GOTO_RECURSIVE);
	case 9:
		nStep = 0;
//		XK_LOG(_T("C-x C-t: 9"));
		RestoreClipboardData();
		break;
	}
	return Reset(GOTO_HOOK);
}

void CCommands::CaptureClipboardData(const int nID, const BOOL bTextOnly)
{
//	XK_LOG("CaptureClipboardData: 1");
	{
		int nSize = nID + 1;
		if (m_oClipboardData.GetSize() < nSize) {
//			XK_LOG("CaptureClipboardData: 2");
			try {
				m_oClipboardData.SetSize(nSize);
			}
			catch (CMemoryException* e) {
				e->Delete();
//				XK_LOG("CaptureClipboardData: SetSize() threw an exception");
			}
		}
	}

//	XK_LOG("CaptureClipboardData: 3");
	if (!m_oClipboardData[nID]) {
//		XK_LOG("CaptureClipboardData: 4");
		try {
			CClipboardSnap *pSnap = new CClipboardSnap;
			if (!pSnap) {
//				XK_LOG("CaptureClipboardData: 5");
				return;
			}
			m_oClipboardData.SetAt(nID, pSnap);
		}
		catch (CMemoryException* e) {
			e->Delete();
//			XK_LOG("CaptureClipboardData: 'new' threw an exception");
		}
	}

//	XK_LOG("CaptureClipboardData: 6: nID=%d, size=%d", nID, m_oClipboardData.GetSize());
	BOOL bCapture = m_oClipboardData[nID]->Capture(bTextOnly);
//	XK_LOG("CaptureClipboardData: 7");
	Sleep(0);	// for OpenOffice
	bCapture = m_oClipboardData[nID]->Capture(bTextOnly);	// for "office drawing shape format". Can CClipboardSnap care this problem?

//	XK_LOG("CaptureClipboardData: 8");
	if (!bCapture) {
//		XK_LOG(_T("Can not Capture in CaptureClipboardData"));
		delete m_oClipboardData[nID];
//		XK_LOG("CaptureClipboardData: 9");
		m_oClipboardData.SetAt(nID, NULL);
//		XK_LOG("CaptureClipboardData: 10");
	}
}

void CCommands::RestoreClipboardData(const int nID)
{
	if (nID + 1 <= m_oClipboardData.GetSize() && m_oClipboardData[nID]) {
		m_oClipboardData[nID]->Restore();
	}
}

int CCommands::FindReturnFromClipboardData(const int nID)
{
	if (nID + 1 <= m_oClipboardData.GetSize() && m_oClipboardData[nID]) {
		return m_oClipboardData[nID]->FindReturn();
	}
	return -1;
}

BOOL CCommands::IsEmptyClipboardData(const int nID)
{
	if (nID + 1 <= m_oClipboardData.GetSize() && m_oClipboardData[nID]) {
		return m_oClipboardData[nID]->IsEmpty();
	}
	return FALSE;
}

void CCommands::PrintFunctionName(int (*nFunctionPointer)())
{
	for (int nCommandID = 1; nCommandID < sizeof(Commands) / sizeof(Commands[0]); ++nCommandID) {
		if (Commands[nCommandID].fCommand == nFunctionPointer) {
			XK_LOG(_T("m_LastCommand: %s"), Commands[nCommandID].szCommandName);
		}
	}
}

int CCommands::CapsLock()
{
	ClearNumericArgument();
	Kdu(VK_CAPITAL);
	return Reset(GOTO_HOOK);
}

int CCommands::Underscore()
{
	ClearNumericArgument();
	if (CXkeymacsData::Is106Keyboard()) {
		SdKduSu(0xE2);	// VK_OEM_102	Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
		Su();
	} else {
		SdKduSu(0xBD);	// VK_OEM_MINUS	Windows 2000/XP: For any country/region, the '-' key
		Su();
	}
	return Reset(GOTO_HOOK);
}

int CCommands::RollUpUnroll()
{
	return Maximize(ROLL_UP_UNROLL);
}

int CCommands::Retop()
{
	RECT ClientRect = {'\0'};
	GetClientRect(GetFocus(), &ClientRect);

	for (int i = 0; i < 0x10000; ++i) {
		POINT CaretPos = {'\0'};
		GetCaretPos(&CaretPos);
		if (ClientRect.top + 10 < CaretPos.y) {
			SendMessage(GetFocus(), WM_VSCROLL, SB_LINEDOWN, NULL);
		} else {
			return Reset(GOTO_HOOK);
		}
	}

	Kdu(VK_UP, VK_DOWN);
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierShift()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierShiftRepeat()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierCtrl()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierCtrlRepeat()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierAlt()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::OneShotModifierAltRepeat()
{
	// dummy
	ASSERT(0);
	static int i = 0;
	++i;
	return Reset(GOTO_HOOK);
}

int CCommands::CycleItems()
{
	ClearNumericArgument();
	AdKduAu(VK_ESCAPE);
	return Reset(GOTO_HOOK);
}

int CCommands::CycleItemsInversely()
{
	ClearNumericArgument();
	AdSdKduSuAu(VK_ESCAPE);
	return Reset(GOTO_HOOK);
}

// M-t
int CCommands::TransposeWords()
{
	static int nStep = 0;
	switch (nStep) {
	case 0:
		nStep = 0;
//		XK_LOG(_T("M-t: 0"));
		CdKduCu(VK_LEFT);
		CaptureClipboardData();
		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:
		nStep = 0;
//		XK_LOG(_T("M-t: 1"));
		CdSdKduSuCu(VK_RIGHT);
		CdKduCu('X');
		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:
		nStep = 0;
//		XK_LOG(_T("M-t: 2"));
		Su();
		CdKduCu(VK_RIGHT);
		CdKduCu('V');
		Kdu(VK_LEFT);
		nStep = 3;
		return Reset(GOTO_RECURSIVE);
	case 3:
		nStep = 0;
//		XK_LOG(_T("M-t: 3"));
		RestoreClipboardData();
		break;
	}
	return Reset(GOTO_HOOK);
}

BOOL CCommands::GetCaretPos(LPPOINT lpPoint)
{
	if (CUtils::IsMicrosoftWord()
	 || CUtils::IsThunderbird()) {
		IMECHARPOSITION ImeCharPosition = {sizeof(IMECHARPOSITION)};

		CallWindowProc((WNDPROC)GetWindowLong(GetFocus(), GWL_WNDPROC), GetFocus(), WM_IME_REQUEST, IMR_QUERYCHARPOSITION, (LPARAM)&ImeCharPosition);
//		XK_LOG(_T("ImeCharPosition: rc = %d, x = %d, y = %d"), 0, ImeCharPosition.pt.x, ImeCharPosition.pt.y);

		ScreenToClient(GetFocus(), &ImeCharPosition.pt);
//		XK_LOG(_T("ScreenToClient: rc = %d, x = %d, y = %d"), 0, ImeCharPosition.pt.x, ImeCharPosition.pt.y);

		*lpPoint = ImeCharPosition.pt;
//		XK_LOG(_T("x = %d, y = %d"), lpPoint->x, lpPoint->y);

		return TRUE;
	}

	return ::GetCaretPos(lpPoint);
}

LRESULT CCommands::VScroll(UINT nSBCode, const int nTimes)
{
	if (CUtils::IsMicrosoftWord()
	 || CUtils::IsThunderbird()) {
		RECT ClientRect = {'\0'};
		GetClientRect(GetFocus(), &ClientRect);

		POINT righttop = {ClientRect.right, ClientRect.top};
		ClientToScreen(GetFocus(), &righttop);
//		XK_LOG(_T("righttop: x = %d, y = %d"), righttop.x, righttop.y);

		POINT rightbottom = {ClientRect.right, ClientRect.bottom};
		ClientToScreen(GetFocus(), &rightbottom);
//		XK_LOG(_T("rightbottom: x = %d, y = %d"), rightbottom.x, rightbottom.y);

//		POINT CursorPos = {'\0'};
//		GetCursorPos(&CursorPos);
//		XK_LOG(_T("VScroll: x = %d, y = %d"), CursorPos.x, CursorPos.y);

		POINT scrollpos = {'\0'};

		switch (nSBCode) {
		case SB_LINEUP:
			if (CUtils::IsMicrosoftWord()) {
				scrollpos.x = righttop.x + 8;	// 0 to 16
				scrollpos.y = righttop.y - 9;	// 1 to 17
			} else if (CUtils::IsThunderbird()) {
				scrollpos.x = righttop.x - 10;	// 2 to 18
				scrollpos.y = righttop.y + 8;	// 0 to 16
			}
			break;
		case SB_LINEDOWN:
			if (CUtils::IsMicrosoftWord()) {
				scrollpos.x = rightbottom.x + 8;	//  0 to 16
				scrollpos.y = rightbottom.y - 60;	// 52 to 68
			} else if (CUtils::IsThunderbird()) {
				scrollpos.x = rightbottom.x - 10;	// 2 to 18
				scrollpos.y = rightbottom.y - 9;	// 1 to 17
			}
			break;
		default:
			return 1;
		}

		int i = 0;
		for (i = 0; i < nTimes; ++i) {
			Click(&scrollpos);
		}

		return 0;
	}

	return SendMessage(GetFocus(), WM_VSCROLL, nSBCode, NULL);
}

// M-q
int CCommands::FillParagraph()
{
	static const int nFillColumn = 70; // tmp
	static CString szClipboardText;
	static int nOldLineLength = 0;

	ClearNumericArgument();	// tmp

	static int nStep = 0;
	switch (nStep) {
	case 0:		// Clear Selection
//		XK_LOG(_T("M-q: 0"));
		nStep = 0;

		CaptureClipboardData();
		if (m_bSetMark) {
			m_bSetMark = FALSE;
		}
		Kdu(VK_HOME);
		nOldLineLength = 0;

		nStep = 1;
		return Reset(GOTO_RECURSIVE);
	case 1:		// Copy previous line
//		XK_LOG(_T("M-q: 1"));
		nStep = 0;

		if (!CopyPreviousLine()) {
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:		// Check previous line to find TOP (top of paragraph)
//		XK_LOG(_T("M-q: 2"));
		nStep = 0;

		CUtils::GetClipboardText(&szClipboardText);
//		XK_LOG(_T("M-q: 2-1: _%s_"), szClipboardText);
		if (szClipboardText.IsEmpty()) {
			// TOF
//			XK_LOG(_T("M-q: 2: TOF"));
			Kdu(VK_HOME);
			nStep = 3;
		} else if (szClipboardText == _T("\r\n")
				|| szClipboardText == _T("\r")
				|| szClipboardText == _T("\n")) {
			// blank line i.e. top of paragraph
			// UNIX: LF "\n"
			// WINDOWS: CR LF "\r\n"
			// MAC: CR "\r"
//			XK_LOG(_T("M-q: 2: TOP"));
			Kdu(VK_DOWN);
			nStep = 3;
		} else {
			// middle of paragraph
//			XK_LOG(_T("M-q: 2: MOP_%d_"), szClipboardText.GetLength());
//			XK_LOG(_T("M-q: 2: MOP_%c(%d)%c(%d)"), szClipboardText.GetAt(0), szClipboardText.GetAt(0), szClipboardText.GetAt(1), szClipboardText.GetAt(1));
			Kdu(VK_HOME);
			nStep = 1;
		}

		return Reset(GOTO_RECURSIVE);
	case 3:		// Go to EOL to copy a current line at the next step
//		XK_LOG(_T("M-q: 3"));
		nStep = 0;

		Kdu(VK_END);

		nStep = 4;
		return Reset(GOTO_RECURSIVE);
	case 4:		// Copy a current line
//		XK_LOG(_T("M-q: 4"));
		nStep = 0;

		if (!CopyCurrentLine()) {
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 5;
		return Reset(GOTO_RECURSIVE);
	case 5:		// Just release shift
//		XK_LOG(_T("M-q: 5"));
		nStep = 0;

		ReleaseKey(VK_SHIFT);

		nStep = 6;
		return Reset(GOTO_RECURSIVE);
	case 6:		// Check current line and fill paragraph
//		XK_LOG(_T("M-q: 6"));
		nStep = 0;

		CUtils::GetClipboardText(&szClipboardText);
		if (szClipboardText.GetLength() == nOldLineLength + 1) {
//			XK_LOG(_T("M-q: 6-1 Finished"));
			Kdu(VK_END, VK_BACK, VK_RETURN);
			nStep = 8;
		} else if (szClipboardText.GetLength() < nFillColumn) {
//			XK_LOG(_T("M-q: 6-2 This line is too small"));
			Kdu(VK_END, VK_SPACE, VK_DELETE, VK_HOME);
			nStep = 3;
		} else {
//			XK_LOG(_T("M-q: 6-3 %d_%s_%d"), szClipboardText.Left(nFillColumn).GetLength(), szClipboardText.Left(nFillColumn), szClipboardText.Left(nFillColumn).ReverseFind(_T(' ')));
			const int offset = szClipboardText.Left(nFillColumn).ReverseFind(_T(' '));
			if (offset == 0) {
//				XK_LOG(_T("M-q: 6-3-1 TOL is space"));
				Kdu(VK_DELETE);
			} else if (0 < offset) {
//				XK_LOG(_T("M-q: 6-3-2"));
				ReleaseKey(VK_MENU); // FIXME
				for (int i = 0; i < offset; ++i) {
					Kdu(VK_RIGHT);
				}
				nStep = 7;
				return Reset(GOTO_RECURSIVE);
			} else {
//				XK_LOG(_T("M-q: 6-3-3 No space in first fill-column"));
				const int offset = szClipboardText.Find(_T(' '));
				if (offset < 0) {
					Kdu(VK_HOME, VK_DOWN);
				} else {
					ReleaseKey(VK_MENU); // FIXME
					for (int i = 0; i < offset; ++i) {
						Kdu(VK_RIGHT);
					}
					nStep = 7;
					return Reset(GOTO_RECURSIVE);
				}
			}
			nStep = 3;
		}
		nOldLineLength = szClipboardText.GetLength();

		return Reset(GOTO_RECURSIVE);
	case 7:		// Linefeed
//		XK_LOG(_T("M-q: 7"));
		nStep = 0;

		Kdu(VK_RETURN, VK_DELETE);

		nStep = 3;
		return Reset(GOTO_RECURSIVE);
	case 8:		// Restore clipboard data
//		XK_LOG(_T("M-q: 8"));
		nStep = 0;

		RestoreClipboardData();

		return Reset(GOTO_HOOK);
	}
	return Reset(GOTO_HOOK);
}

// M-l
int CCommands::DowncaseWord()
{
	return CaseWord(DOWNCASE);
}

// M-u
int CCommands::UpcaseWord()
{
	return CaseWord(UPCASE);
}

// M-c
int CCommands::CapitalizeWord()
{
	return CaseWord(CAPITALIZE);
}

int CCommands::CaseWord(CASE_WORD nCase)
{
	ClearNumericArgument();	// tmp

	// Do not try to do these command at once.
	// Clipboard has old data till you go out this function.
	static GOTO nNext = GOTO_HOOK;
	static int nStep = 0;
	static int nLastWhiteSpace = 0;
	static BOOL bFirstCharacter = TRUE;
	switch (nStep) {
	case 0:		// Clear Selection
		nLastWhiteSpace = 0;

		if (m_bSetMark) {
			m_bSetMark = FALSE;
			Kdu(VK_RIGHT, VK_LEFT);
			nStep = 1;
			return Reset(GOTO_RECURSIVE);
		} else {
			// Do nothing. Go to case 1:
		}
	case 1:		// Get back character to confirm the cursor is at TOF or not.
		nStep = 0;

		CaptureClipboardData();

		if (!CutFollowingWord()) {
			return Reset(GOTO_DO_NOTHING);
		}

		nStep = 2;
		return Reset(GOTO_RECURSIVE);
	case 2:
		{
			nStep = 0;

			CString szClipboardText;
			CUtils::GetClipboardText(&szClipboardText);

			CString szTrimmed = szClipboardText;
			szTrimmed.TrimLeft();
			if (szClipboardText.IsEmpty()) {
				nNext = GOTO_HOOK;
				RestoreClipboardData();
				return Reset(GOTO_HOOK);
			} else if (szTrimmed.IsEmpty()) {
				if (nNext == GOTO_HOOK) {
					nNext = GOTO_RECURSIVE;
				} else {
					if (CUtils::IsHidemaru()) {
						nLastWhiteSpace = 1;
					}
					nNext = GOTO_HOOK;
				}
				CUtils::SetClipboardText(&szClipboardText);
				nStep = 3;
				return Reset(GOTO_RECURSIVE);
			} else if (CUtils::IsHidemaru()) {
				nNext = GOTO_RECURSIVE;
			} else {
				nNext = GOTO_HOOK;
			}

			switch (nCase) {
			case DOWNCASE:
				szClipboardText.MakeLower();
				break;
			case UPCASE:
				szClipboardText.MakeUpper();
				break;
			case CAPITALIZE:
				{
					for (int i = 0; i < szClipboardText.GetLength(); ++i) {
						if (_istspace(szClipboardText.GetAt(i))) {
							continue;
						} else if (bFirstCharacter) {
							bFirstCharacter = FALSE;
							if (_istlower(szClipboardText.GetAt(i))) {
								szClipboardText.SetAt(i, (TCHAR) toupper(szClipboardText.GetAt(i)));
							}
						} else {
							if (_istupper(szClipboardText.GetAt(i))) {
								szClipboardText.SetAt(i, (TCHAR) tolower(szClipboardText.GetAt(i)));
							}
						}
					}
					break;
				}
			default:
				ASSERT(0);
			}

			CUtils::SetClipboardText(&szClipboardText);

			nLastWhiteSpace = 0;
			for (int j = szClipboardText.GetLength() - 1; !szClipboardText.IsEmpty() && 0 <= j; --j) {
				if (_istspace(szClipboardText.GetAt(j))) {
					++nLastWhiteSpace;
				} else {
					break;
				}
			}

			nStep = 3;
			return Reset(GOTO_RECURSIVE);
		}
	case 3:
		{
			nStep = 0;

			Paste();

			for (int i = 0; i < nLastWhiteSpace; ++i) {
				Kdu(VK_LEFT);
			}

			nStep = 4;
			return Reset(GOTO_RECURSIVE);
		}
	case 4:
		nStep = 0;

		RestoreClipboardData();

		if (nNext == GOTO_HOOK) {
			bFirstCharacter = TRUE;
		}
		return Reset(nNext);
	}

	return Reset(GOTO_HOOK);
}

BOOL CCommands::CutFollowingWord()
{
	if (!CUtils::OpenClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!EmptyClipboard()) {
		ASSERT(0);
		return FALSE;
	}
	if (!CloseClipboard()) {
		ASSERT(0);
		return FALSE;
	}

	if (CUtils::IsHidemaru()) {
		// On Hidemaru, Cut_() does not work well after CdSdKduSuCu(). I do not know why.
		SdKduSu(VK_RIGHT);
	} else {
		CdSdKduSuCu(VK_RIGHT);
	}
	Cut_();

	return TRUE;
}

// M-x: execute-extended-command
int CCommands::ExecuteExtendedCommand()
{
//	XK_LOG(_T("ExecuteExtendedCommand"));
	bM_x(TRUE);
	return Reset(GOTO_HOOKX);
}
