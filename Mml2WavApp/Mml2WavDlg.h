
// MFCApplication1Dlg.h : ヘッダー ファイル
//

#pragma once

#include "WavGenerator.h"
#include <array>

struct WaveOutParam;
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
	struct WavData
	{
		WAVEFORMATEX format;
		size_t startSample;
		std::vector<int16_t> data;
	};

	CEdit txtMml_;
	CListBox lstDuty_;
	CComboBox cboDuty_;
	CComboBox cboCurve_;
	CEdit txtNoise_;
	MmlUtility::ToneData toneData_;
	CEdit txtDutySwictTiming_;

	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnDutyRatioAdd();
	afx_msg void OnBnClickedBtnDutyRatioDel();
	afx_msg void OnEnChangeTxtNoise();
	afx_msg void OnEnChangeTxtDutySwitchTiming();
	afx_msg void OnBnClickedBtnOutput();

	void RefreshDutyList();
	bool genWavData(WavData& dest);

	CComboBox cboFloatType_;
	CComboBox cboSampleRate_;
	CTabCtrl tabBank_;

	std::vector<WaveOutParam*> playing_;

	enum { BANKS=5};
	std::string shared_;
	std::array<std::string, BANKS> bank_;
public:
	afx_msg void OnTcnSelchangeTabBank(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeTxtMml();
private:
	CComboBox cboToneNumber_;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtn();
private:
	CButton chkLoop_;
public:
	CButton chkFromCurrent_;
};
