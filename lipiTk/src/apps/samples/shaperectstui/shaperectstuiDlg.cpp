/*****************************************************************************************
* Copyright (c) 2006 Hewlett-Packard Development Company, L.P.
* Permission is hereby granted, free of charge, to any person obtaining a copy of 
* this software and associated documentation files (the "Software"), to deal in 
* the Software without restriction, including without limitation the rights to use, 
* copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
* Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*****************************************************************************************/
/************************************************************************
 * SVN MACROS
 *
 * $LastChangedDate:  $
 * $Revision:  $
 * $Author:  $
 *
 ************************************************************************/
/************************************************************************
 * FILE DESCR	: 
 * 
 * AUTHOR		:  Balaji MNA
 * Version		:  1.0
 * Date			:  10-Jan-2010
 * CHANGE HISTORY:
 * Author       Date            Description of change
 ************************************************************************/
#include "stdafx.h"
#include "shaperectstui.h"
#include "shaperectstuiDlg.h"
#include "LTKShapeRecoResult.h"
#include "LTKConfigFileReader.h"
#include "LTKStringUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAPPING_PATH_STRING		SEPARATOR + "shaperectstui.cfg"
#define LIPIENGINE_PATH_STRING	SEPARATOR + "lipiengine.cfg"
#define IDT_RECOGNIZE_TIMER 1000
#define DELAY_TIME          650
#define MAX_CHOICE          4
#define MENU_ID             WM_USER+10
#define UNICODE_MAPFILE     SEPARATOR + "config" + SEPARATOR + "unicodeMapfile"

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CShaperectstuiDlg::CShaperectstuiDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShaperectstuiDlg::IDD, pParent), m_PrevPenPoint(0,0),
	m_IsDrawing(false),
	m_IsProjectLoaded(false),
	m_IsMappingFileFound(false),
	m_word(L""),
	m_DisplayMsg(L"")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_DrawAreaRect.left=40;
	m_DrawAreaRect.top=72+20;
	m_DrawAreaRect.right=190;
	m_DrawAreaRect.bottom=225;
}

void CShaperectstuiDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_CHOICE1, m_DisplayChoice[0]);
	DDX_Control(pDX, IDC_BUTTON_CHOICE2, m_DisplayChoice[1]);
	DDX_Control(pDX, IDC_BUTTON_CHOICE3, m_DisplayChoice[2]);
	DDX_Control(pDX, IDC_BUTTON_CHOICE4, m_DisplayChoice[3]);
	DDX_Text(pDX, IDC_EDIT_WORD, m_word);
	DDX_Control(pDX, IDC_EDIT_WORD, m_TextEdit);
	DDX_Text(pDX, IDC_DISPLAY_MSG, m_DisplayMsg);
}

BEGIN_MESSAGE_MAP(CShaperectstuiDlg, CDialog)
	//{{AFX_MSG_MAP(CShaperectstuiDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DELCHAR, &CShaperectstuiDlg::OnBnClickedButtonDelchar)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CShaperectstuiDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_CHOICE1, &CShaperectstuiDlg::OnBnClickedButtonChoice1)
	ON_BN_CLICKED(IDC_BUTTON_CHOICE2, &CShaperectstuiDlg::OnBnClickedButtonChoice2)
	ON_BN_CLICKED(IDC_BUTTON_CHOICE3, &CShaperectstuiDlg::OnBnClickedButtonChoice3)
	ON_BN_CLICKED(IDC_BUTTON_CHOICE4, &CShaperectstuiDlg::OnBnClickedButtonChoice4)
	ON_COMMAND(ID_PROJECTS_EXIT, &CShaperectstuiDlg::OnProjectsExit)
	ON_CONTROL_RANGE(BN_CLICKED,MENU_ID, MENU_ID+10, OnButtonClicked)
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CShaperectstuiDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	SetWindowText(L"Shape Recog Test UI");

	try
	{
	char *environmentVal = getenv("LIPI_ROOT");
	if(NULL == environmentVal) // Added by Jitender, 10/17/2011, exitr if LIPI_ROOT env var is not there.
	{
		MessageBox(L"LIPI_ROOT enviornment variable does not exist.");

        // same as double-clicking on main window close box
        ASSERT(AfxGetApp()->m_pMainWnd != NULL);
        AfxGetApp()->m_pMainWnd->SendMessage(WM_CLOSE);
		exit(1);
	}

	m_lipiRoot = environmentVal;

	//set lipi root
	m_ShapeRecognizer.setEnv(m_lipiRoot);
	}
	catch( CException *e)
	{
		MessageBox(L"Failed to read Enviornment Variable LIPI_ROOT.");
		 e->Delete();
	}

	SetMenuBar();

	//Load lipiinterface
	if(m_ShapeRecognizer.loadLipiInterface() != SUCCESS)
		return false;

	return TRUE; 
}

void CShaperectstuiDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CShaperectstuiDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

	}
	else
	{
		CDialog::OnPaint();
	}
	//Draw writing area
	HPEN hPen,hOld;
	hPen = ::CreatePen (PS_SOLID, 2 ,RGB (0.5, 0.5, 0)); 
	hOld = (HPEN)SelectObject (dc, hPen);

	Rectangle (dc,m_DrawAreaRect.left,m_DrawAreaRect.top,
		m_DrawAreaRect.right,m_DrawAreaRect.bottom);

	HRGN hrgn = CreateRectRgn(m_DrawAreaRect.left,m_DrawAreaRect.top,
		m_DrawAreaRect.right,m_DrawAreaRect.bottom);

	SelectObject (dc,hOld);
	DeleteObject (hPen);
}

HCURSOR CShaperectstuiDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
int CShaperectstuiDlg::SetMenuBar(void)
{
	m_Menubar.LoadMenu(IDR_MENU1);
	BOOL flag = SetMenu(&m_Menubar);
	
	CMenu* menu = GetMenu();
	CMenu* subMenu = menu->GetSubMenu(0);
	CMenu* projectMenu = subMenu->GetSubMenu(0);

	
	string mapFile = m_lipiRoot + PROJECTS_PATH_STRING + LIPIENGINE_PATH_STRING;

	//	opening the lipiengine config file
	ifstream cfgFileHandle(mapFile.c_str());

	//	checking if the file open was successful
	if(!cfgFileHandle)
	{
		return FAILURE;
	}

	string line = "";						//	a line read from the config file

	vector<string> strTokens;				//	string list found in the line read

	int i =0;

	//	reading lines of the config file
	while(getline(cfgFileHandle, line, NEW_LINE_DELIMITER))
	{
		// trim the line
		LTKStringUtil::trimString(line);

		if(line.empty())
		{
    		//	skipping over empty line
			continue;
		}
		else
		{
			LTKStringUtil::tokenizeString(line,  "=",  strTokens);
			
			if(strTokens.size() == 2)
			{
				LTKStringUtil::trimString(strTokens[0]);
				LTKStringUtil::trimString(strTokens[1]);
				
				m_cfgFileMap[strTokens[0]] = strTokens[1];

				TCHAR item[MAX_PATH] = {0} ;
				//_tcscpy(item,strTokens[0].c_str());
				mbstowcs(item,strTokens[0].c_str(),MAX_PATH);
				projectMenu->InsertMenu(i,MF_BYPOSITION,MENU_ID+i,item);
				i++;
			}
		}
	}
	return 0;	
}
void CShaperectstuiDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	KillTimer(IDT_RECOGNIZE_TIMER);
	m_IsDrawing = true;

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
		(point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);
		m_PrevPenPoint = penpoint;
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CShaperectstuiDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(false == m_IsDrawing)
		return;

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
	   (point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CDC* pDC = GetDC();

		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);

		HPEN hPen,hOld;
		hPen = ::CreatePen (PS_SOLID, 2 ,RGB (0.5, 0.5, 0)); 
		hOld = (HPEN)pDC->SelectObject( hPen);

		if(m_PrevPenPoint.x != 0 && m_PrevPenPoint.y != 0)
		{
			pDC->MoveTo(m_PrevPenPoint.x,m_PrevPenPoint.y);
			pDC->LineTo(point.x,point.y);
		}
			
		m_PrevPenPoint = penpoint;

		pDC->SelectObject (hOld);
		DeleteObject (hPen);	
	}	
	CDialog::OnMouseMove(nFlags, point);
}

void CShaperectstuiDlg::OnLButtonUp(UINT nFlags, CPoint point)
{

	if((point.x > m_DrawAreaRect.left && point.x < m_DrawAreaRect.right) &&
		(point.y > m_DrawAreaRect.top && point.y < m_DrawAreaRect.bottom))
	{
		CPoint penpoint(point.x,point.y);	
		m_PenPointsVec.push_back(penpoint);

		m_penstrokes.push_back(m_PenPointsVec);

		m_PrevPenPoint.x = 0;
		m_PrevPenPoint.y = 0;
	}
	
	m_IsDrawing = false;

	ClearDisplayChoice();
	SetTimer(IDT_RECOGNIZE_TIMER, DELAY_TIME, NULL);
}

void CShaperectstuiDlg::ClearDisplayChoice()
{
	m_DisplayChoice[0].SetWindowTextW(L"");
	m_DisplayChoice[1].SetWindowTextW(L"");
	m_DisplayChoice[2].SetWindowTextW(L"");
	m_DisplayChoice[3].SetWindowTextW(L"");
}

void CShaperectstuiDlg::OnBnClickedButtonDelchar()
{
	UpdateData(true);
	
	if(!m_word.IsEmpty())
	{
		m_word = m_word.Mid(0,m_word.GetLength() -1);

		ClearDisplayChoice();

		UpdateData(false);
	}

}
void CShaperectstuiDlg::OnBnClickedButtonSend()
{
	UpdateData(true);
	HWND hWnd = ::FindWindow(L"Notepad",NULL);
	
	if(!(hWnd == NULL || m_word.IsEmpty()))
	{
		if(::SetForegroundWindow(hWnd) != 0)
		{
			INPUT inp[2];
			memset(inp,0,sizeof(INPUT));
			inp[0].type = INPUT_KEYBOARD;
			inp[0].ki.dwFlags = KEYEVENTF_UNICODE; // to avoid shift, and so on
			inp[1] = inp[0];
			inp[1].ki.dwFlags |= KEYEVENTF_KEYUP;

			for (int i=0; i<m_word.GetLength(); i++) {
				inp[0].ki.wScan = inp[1].ki.wScan = (WORD)m_word.GetAt(i);
				SendInput(2, inp, sizeof(INPUT));
			}
		}
	}

	ClearDisplayChoice();

	m_word = L"";
	UpdateData(false);

	//::SetForegroundWindow(this->m_hWnd);
}
void CShaperectstuiDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == IDT_RECOGNIZE_TIMER)
	{
		KillTimer(IDT_RECOGNIZE_TIMER);

		if(m_IsProjectLoaded)
		{
			displayMessage(L"Recognizing");
			recognizeStrokes();
		}
		else
		{
			displayMessage(L"Please load project...");
		}

		InvalidateRect(LPCRECT(&m_DrawAreaRect),TRUE);

		m_PrevPenPoint.x = 0;
		m_PrevPenPoint.y = 0;

		m_PenPointsVec.clear();
		m_penstrokes.clear();
			
		displayMessage(L"");
	}

	CDialog::OnTimer(nIDEvent);
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 10-Dec-2008
* NAME			: recognizeStrokes
* DESCRIPTION	: Recognize the strokes and displays the top most choices
* ARGUMENTS		: none
* RETURNS		: none 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/

void CShaperectstuiDlg::recognizeStrokes(void)
{
	displayMessage(L"Recognizing...");
	cout << "classid:" << "Recognizing" << endl;

	if(0 != m_PenPointsVec.size() && 0 != m_UnicodeValue.size() )
	{
		vector<LTKShapeRecoResult> resultSet;
		cout << "classid:" << "start Recognizing" << endl;
		if( FAILURE == m_ShapeRecognizer.recognize(m_penstrokes,MAX_CHOICE,resultSet))
		{
			displayMessage(L"Wrong gesture!");
			return;
		}
		
		//Display recognized values on choice boxes
		int i = 0;
		cout << "classid:" << "before loop Recognizing" << endl;
		for(vector<LTKShapeRecoResult>::iterator t = resultSet.begin(); 
			(t < resultSet.end()) && (i < MAX_CHOICE) ; t++)
		{
			//if(m_IsFont)
			cout << "classid:" << "in loop Recognizing" << endl;
				m_DisplayChoice[i++].SetWindowTextW(m_UnicodeValue[(*t).getShapeId()]);
				cout << "classid:" << m_UnicodeValue[(*t).getShapeId()] << endl;
			//else
				//m_DisplayChoice[i++].SetWindowTextW(m_UnicodeValue[(*t)]); //added by jitender to support non unicode values

		}

		displayMessage(L" ");
	}
	CString getValue;
	m_DisplayChoice[0].GetWindowTextW(getValue);
	m_word += getValue; 
	UpdateData(false);
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: loadUniCodeFile
* DESCRIPTION	: Method to load unicode map file  
* ARGUMENTS		: void
* RETURNS		: return 0 - if project successfully loads 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CShaperectstuiDlg::loadUniCodeFile(string project)
{
	if(project.empty())
		return -1;

	char strSep[] = " ()\r";
	char *strToken;
	char *strSearch = (char *)project.c_str();
	
	strToken = strtok(strSearch, strSep);
	if(!strToken)
	{
		return -1;
	}

	strToken[strlen(strToken)] = L'\0';
	string projectname(strToken);

	string mapFileUnicode = m_lipiRoot + PROJECTS_PATH_STRING + projectname + UNICODE_MAPFILE ;
	mapFileUnicode +=  "_" + projectname + ".ini";

	// added by jitender to load unicode.cfg incase unicode file is not available/////////////////

	
	string mapFileClassName = m_lipiRoot + PROJECTS_PATH_STRING + projectname + UNICODE_MAPFILE ;
	mapFileClassName += ".cfg";

	const char* tempUnicode = mapFileUnicode.c_str();
	const char* tempClass = mapFileClassName.c_str();

	string mapFile;
	if( -1 != (_access( tempUnicode, 0 )) )
		mapFile = mapFileUnicode;
	else if( -1 != (_access( tempClass, 0 )))
		mapFile = mapFileClassName;
	else
	{
		printf(" class id mapping file does not exist, exiting");
		return -1;
	}

	/*if( 0 == doesFilesExists(mapFileUnicode))
		mapFile = mapFileUnicode;
	else if( 0 ==doesFilesExists(mapFileClassName))
		mapFile = mapFileClassName;
	else
	{
		printf(" class id mapping file does not exist, exiting");
		return -1;
	}*/
////////////////////////////////////////////////////////////////////////////////////////////
	loadMapping(mapFile);

	m_IsMappingFileFound = true;

	//Set font
	if(!m_fontName.empty())
	{
		CString szFontNm(m_fontName.c_str());

		m_pFont.DeleteObject();
		szFontNm.Trim();

		m_pFont.CreateFont(35,0, 0, 0, 400, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		  DEFAULT_PITCH | FF_DONTCARE, szFontNm);

		for(int i = 0; i< MAX_CHOICE;i++)
			m_DisplayChoice[i].SetFont(&m_pFont,TRUE);

	}


	return SUCCESS;
}
/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 13-Dec-2008
* NAME			: loadMapping
* DESCRIPTION	: load mapping files
* ARGUMENTS		: string - map path
* RETURNS		: return 0 - after successfull retrieves values for SRE-ID 
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
int CShaperectstuiDlg::loadMapping(string& mapFile)
{
	LTKConfigFileReader configfilereader(mapFile);

	if (SUCCESS != configfilereader.getConfigValue("Font",m_fontName))
		m_IsFont = false;
	else
		m_IsFont = true;

	m_UnicodeValue.clear();

	int i = 0;
	bool flag = true;
	while(flag)
	{
		char key[10]={0};
		sprintf(key,"%d",i);
		string value;
		configfilereader.getConfigValue(key,value);
		if(value.empty())
			flag = false;
		else
		{
			CString strValue(value.c_str());
			if(m_IsFont)
			{
				long choice = (long)_tcstol(strValue,NULL,16);
				CString setValue;
				setValue.Format(L"%c",(TCHAR)choice);
				m_UnicodeValue[i] = setValue;
			}
			else
				m_UnicodeValue[i] = strValue;
		}
		i++;
	}
	return SUCCESS;
}
void CShaperectstuiDlg::OnBnClickedButtonChoice1()
{
	ChangeText(0);
}

void CShaperectstuiDlg::OnBnClickedButtonChoice2()
{
	ChangeText(1);
}

void CShaperectstuiDlg::OnBnClickedButtonChoice3()
{
	ChangeText(2);
}

void CShaperectstuiDlg::OnBnClickedButtonChoice4()
{
	ChangeText(3);
}
void CShaperectstuiDlg::ChangeText(int choice)
{
	CString getValue;
	m_DisplayChoice[choice].GetWindowTextW(getValue);
	m_word = m_word.Mid(0,m_word.GetLength()-1);
	m_word += getValue;

	m_TextEdit.SetFocus();
	UpdateData(false);
}
void CShaperectstuiDlg::OnClose()
{
	m_ShapeRecognizer.unloadModelData();
	CDialog::OnClose();
}

void CShaperectstuiDlg::OnProjectsExit()
{
	//m_ShapeRecognizer.unloadModelData();
	//CDialog::OnClose();
	OnClose();
	DestroyWindow();
}

/**********************************************************************************
* AUTHOR		: Balaji MNA
* DATE			: 19-Dec-2008
* NAME			: displayMessage
* DESCRIPTION	: display message on dialog box [Currently messages are not getting displayed in 
*                                                user interface]
* ARGUMENTS		: CString - message
* RETURNS		:  
* NOTES			: 
* CHANGE HISTROY
* ************************************************************************************/
void CShaperectstuiDlg::displayMessage(const CString& message)
{
	m_DisplayMsg = message;
	UpdateData(false);	
}

void CShaperectstuiDlg::OnButtonClicked(UINT nID)
{
	CString name;
	if(m_Menubar.GetMenuString(nID,name,0) != -1)
	{
		if(!name.IsEmpty() )
		{
			// Convert a TCHAR string to a LPCSTR
			CT2CA pszConvertedAnsiString (name);
			// construct a std::string using the LPCSTR input
			std::string projname (pszConvertedAnsiString);
			m_logicalName = projname;

			displayMessage(L"Loading project...");

			//Clear previous projects
			CString strSetTitle("shape recognition ");
			this->SetWindowTextW(strSetTitle);
			m_ShapeRecognizer.unloadModelData();

			//Load model data
			if(FAILURE == m_ShapeRecognizer.loadModelData(projname))
			{
				displayMessage(L"Failed to load project!");
				return ;
			}

			//Set Title
			if(!m_logicalName.empty())
			{
				CString strTitle("shape recognition: ");
				strTitle += m_logicalName.c_str();

				this->SetWindowTextW(strTitle);
			}

			//Load Map file
			displayMessage(L"Load map file");
			loadUniCodeFile(m_cfgFileMap[m_logicalName]);

			displayMessage(L"");
			m_IsProjectLoaded = true;
		}
	}
}

// Checks if given file exists

//int doesFilesExists(string fileName)
//{
//	WIN32_FIND_DATA FindFileData;
//	HANDLE hFind;
//
//	hFind = FindFirstFile(fileName, &FindFileData);
//    if (hFind == INVALID_HANDLE_VALUE) 
//    {
//	  printf ("FindFirstFile failed (%d)\n", GetLastError());
//	  return -1;
//    } 
//    else 
//    {
//	  _tprintf (TEXT("The first file found is %s\n"), FindFileData.cFileName);
//	  FindClose(hFind);
//    }
//	return 0;
//}