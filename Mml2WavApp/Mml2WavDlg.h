
// MFCApplication1Dlg.h : ヘッダー ファイル
//

#pragma once

#include "WavGenerator.h"
#include <array>

struct WaveOutParam;

class CEditEx : public CEdit
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
};
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

	CEditEx txtMml_;
	CListBox lstDuty_;
	CComboBox cboDuty_;
	CComboBox cboCurve_;
	CEditEx txtNoise_;
	MmlUtility::ToneData toneData_;
	CEdit txtDutySwictTiming_;
	CComboBox cboFloatType_;
	CComboBox cboSampleRate_;
	CTabCtrl tabBank_;
	CComboBox cboToneNumber_;
	CButton chkLoop_;
	CButton chkFromCurrent_;
	CButton chkCurrentBank_;
	CButton chkInvalidateTempoCommand_;
	CButton chkDivideImportVol_;
	CEdit txtLevelNoise_;


	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnDutyRatioAdd();
	afx_msg void OnBnClickedBtnDutyRatioDel();
	afx_msg void OnEnChangeTxtNoise();
	afx_msg void OnEnChangeTxtDutySwitchTiming();
	afx_msg void OnBnClickedBtnOutput();
	afx_msg void OnTcnSelchangeTabBank(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeTxtMml();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtn();
	afx_msg void OnBnClickedBtnEnvelopeCmd();
	afx_msg void OnBnClickedBtnEnvTemplateDefault();
	afx_msg void OnBnClickedBtnEnvTemplateNone();
	afx_msg void OnBnClickedBtnEnvTemplateStrongAtack();
	afx_msg void OnBnClickedBtnEnvTemplateLongRelease();

	afx_msg void OnBnClickedBtnEnvTemplateStrongAtack2();
	afx_msg void OnBnClickedBtnEnvTemplateLongRelease3();
	afx_msg void OnBnClickedBtnEnvTemplateLongRelease4();
	afx_msg void OnBnClickedBtnClearTabs();
	afx_msg void OnBnClickedBtnImportExternalMmlFromClipboard();
	afx_msg void OnBnClickedBtnWaveTest();

	afx_msg void OnCmdExit();
	afx_msg void OnCmdNewFile();
	afx_msg void OnCmdOpenFile();
	afx_msg void OnCmdSaveAsFile();
	afx_msg void OnCmdSaveFile();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void RefreshDutyList();
	void SetEnvelopeTemplate(int no);
	bool genWavData(WavData& dest, bool checkMml=false);
	void play(bool isCheckWave);
	bool closeFile();

	void loadMml(CString str, bool isVolumeControl, bool isImport = false);
	void setWindowText();

	CString genWaveCommand();

	std::vector<WaveOutParam*> playing_;

	enum { BANKS=5};
	std::string shared_;
	std::array<std::string, BANKS> bank_;

	CString filePath_;
	bool isDirty_ = false;
	HACCEL hAcc_;

public:
	afx_msg void OnAccPlay();
	afx_msg void OnAccStop();
private:
	CButton chkEmulate8Bit_;
};
