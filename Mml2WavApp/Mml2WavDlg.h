
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
	struct WavData
	{
		WAVEFORMATEX format;
		std::vector<int16_t> data;
	};

	CEdit txtMml_;
	CListBox lstDuty_;
	CComboBox cboDuty_;
	CComboBox cboCurve_;
	CEdit txtNoise_;
	ToneData toneData_;
	CEdit txtDutySwictTiming_;

	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnDutyRatioAdd();
	afx_msg void OnBnClickedBtnDutyRatioDel();
	afx_msg void OnEnChangeTxtNoise();
	afx_msg void OnEnChangeTxtDutySwitchTiming();
	afx_msg void OnBnClickedBtnOutput();

	void RefreshDutyList();
	bool genWavData(WavData& dest);


	HWAVEOUT hWaveOut_;
	std::vector<int16_t> pcmBuffer_;

	CComboBox cboFloatType_;
	CComboBox cboSampleRate_;
};
