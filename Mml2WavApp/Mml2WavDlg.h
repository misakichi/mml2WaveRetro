
// MFCApplication1Dlg.h : ヘッダー ファイル
//

#pragma once

#include "WavGenerator.h"

// CMml2WavDlg ダイアログ
class CMml2WavDlg : public CDialogEx
{
// コンストラクション
public:
	CMml2WavDlg(CWnd* pParent = nullptr);	// 標準コンストラクター

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MAINFORM };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnSelchangeCboCurve();

	DECLARE_MESSAGE_MAP()
private:
	CEdit txtMml_;
	CListBox lstDuty_;
	CComboBox cboDuty_;
	CComboBox cboCurve_;
	WavGenerator::ToneData toneData_;

	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnDutyRatioAdd();
	afx_msg void OnBnClickedBtnDutyRatioDel();

	void RefreshDutyList();


	HWAVEOUT hWaveOut_;
	std::vector<int16_t> pcmBuffer_;
};
