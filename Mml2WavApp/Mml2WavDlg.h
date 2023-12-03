
// MFCApplication1Dlg.h : ヘッダー ファイル
//

#pragma once

#include "WavGenerator.h"
#include <array>


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


	enum { BANKS = 5 };

	class GeneratorWrapper
	{
	public:
		template<typename T>
		void reset()
		{
			delete generator1_;
			delete generator2_;
			delete generator3_;
		}

		template<>
		void reset<CFixFloat<>>()
		{
			reset<void>();
			generator1_ = new MmlUtility::MultiBankMml<CFixFloat<>, BANKS>();
		}

		template<>
		void reset<float>()
		{
			reset<void>();
			generator2_ = new MmlUtility::MultiBankMml<float, BANKS>();
		}
		template<>
		void reset<double>()
		{
			reset<void>();
			generator3_ = new MmlUtility::MultiBankMml<double, BANKS>();
		}
		MmlUtility::GenerateMmlToPcmResult compile(const std::string& prepareSharedMml, const std::array<std::string, BANKS>& bankMml, int sampleRate = 48000, size_t currentBank = 0, size_t currentCursor = 0)
		{
			if (generator1_)
				return generator1_->compile(prepareSharedMml, bankMml, sampleRate, currentBank, currentCursor);
			if (generator2_)
				return generator2_->compile(prepareSharedMml, bankMml, sampleRate, currentBank, currentCursor);
			if (generator3_)
				return generator3_->compile(prepareSharedMml, bankMml, sampleRate, currentBank, currentCursor);
			return {};
		}
		inline void skipToCurrent()
		{
			if (generator1_)
				return generator1_->skipToCurrent();
			if (generator2_)
				return generator2_->skipToCurrent();
			if (generator3_)
				return generator3_->skipToCurrent();
			return;
		}
		template<unsigned Channel, typename Type> inline std::vector<Type> generate(int samples)
		{
			if (generator1_)
				return generator1_->generate<Channel,Type>(samples);
			if (generator2_)
				return generator2_->generate<Channel, Type>(samples);
			if (generator3_)
				return generator3_->generate<Channel, Type>(samples);
			return {};
		}
		void setLoop(bool loop)
		{
			if (generator1_)
				return generator1_->setLoop(loop);
			if (generator2_)
				return generator2_->setLoop(loop);
			if (generator3_)
				return generator3_->setLoop(loop);
		}
		void setDisableInfinityLoop(bool disableInfinityLoop)
		{
			if (generator1_)
				return generator1_->setDisableInfinityLoop(disableInfinityLoop);
			if (generator2_)
				return generator2_->setDisableInfinityLoop(disableInfinityLoop);
			if (generator3_)
				return generator3_->setDisableInfinityLoop(disableInfinityLoop);
		}
		bool isPlayEnd() const
		{
			if (generator1_)
				return generator1_->isPlayEnd();
			if (generator2_)
				return generator2_->isPlayEnd();
			if (generator3_)
				return generator3_->isPlayEnd();
			return false;
		}
	private:

		MmlUtility::MultiBankMml<CFixFloat<>, BANKS>* generator1_ = nullptr;
		MmlUtility::MultiBankMml<float, BANKS>* generator2_ = nullptr;
		MmlUtility::MultiBankMml<double, BANKS>* generator3_ = nullptr;
	};
public:
	static constexpr size_t StreamingBuffurSize = 2048;

	struct WaveOutParam {
		static constexpr size_t Buffers = 4;
		GeneratorWrapper* generator_ = NULL;
		HWAVEOUT hWaveOut_ = NULL;
		int bufferWIndex = 0;
		int bufferRIndex = 0;
		WAVEHDR nowHeader[Buffers] = {};
		int16_t pcmBuffer_[Buffers][2048];
		bool loopPlay_ = false;
		bool exitPlay_ = false;
	};
	void writePcm(bool isZero=false);
	void waveOutCallbackProc(
		HWAVEOUT  hwo,
		UINT      uMsg
	);

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
	GeneratorWrapper* genWavReady(WavData& dest, bool checkMml=false);
	void play(bool isCheckWave);
	bool closeFile();

	void loadMml(CString str, bool isVolumeControl, bool isImport = false);
	void setWindowText();

	CString genWaveCommand();

	volatile WaveOutParam* playing_;

	std::string shared_;
	std::array<std::string, BANKS> bank_;

	CString filePath_;
	bool isDirty_ = false;
	HACCEL hAcc_;

	afx_msg void OnAccPlay();
	afx_msg void OnAccStop();
	CButton chkEmulate8Bit_;

	//GeneratorWrapper* generator_;
};
