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
#pragma once
#include "ShapeRecognizer.h"
#include "afxwin.h"

typedef struct sre_id{
	int sreid;
	vector<CString> value;
}StructSRE;

class CShaperectstuiDlg : public CDialog
{
public:
	CShaperectstuiDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_SHAPERECTSTUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:
	HICON m_hIcon;
	RECT  m_DrawAreaRect;
	CString m_word;

	CMenu           m_Menubar;

	CButton			m_DisplayChoice[4];
	vector<vector<CPoint>> m_penstrokes;  
	vector<CPoint>  m_PenPointsVec;
	CPoint          m_PrevPenPoint;
	bool            m_IsDrawing;
	CFont			m_pFont;
	string			m_lipiRoot;
	CShapeRecognizer	m_ShapeRecognizer;
	vector<StructSRE>	m_SREID_Value;
	bool			m_IsProjectLoaded;
	bool            m_IsMappingFileFound;
	string          m_logicalName;
	string          m_fontName;
	bool			m_IsFont; //added by jitender to support non unicode values
	map<int,CString> m_UnicodeValue;
	CEdit m_TextEdit;

	stringStringMap m_cfgFileMap;
	CString m_DisplayMsg;

	//{{AFX_MSG(CShaperectstuiDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonDelchar();
	afx_msg void OnBnClickedButtonSend();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonChoice1();
	afx_msg void OnBnClickedButtonChoice2();
	afx_msg void OnBnClickedButtonChoice3();
	afx_msg void OnBnClickedButtonChoice4();
	afx_msg void OnButtonClicked(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void recognizeStrokes(void);
	int  loadUniCodeFile(string logicalname);
	void OnLoadPreBuildRecognizer();
	int loadMapping(string& mapFile);
	void ChangeText(int choice);
    void ClearDisplayChoice();
	int SetMenuBar(void);
	void displayMessage(const CString& message);
	//int doesFilesExists(string fileName);
public:
	afx_msg void OnProjectsExit();
};

