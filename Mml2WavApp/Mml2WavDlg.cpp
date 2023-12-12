//
//  Mml2WavDlg.cpp
//
//  Copyright (c) 2023 misakichi kaminagare.
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file "LICENSE" or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include "pch.h"
#include "framework.h"
#include "Mml2WavApp.h"
#include "Mml2WavDlg.h"
#include "afxdialogex.h"
#include <memory>
#include <io.h>
#pragma comment(lib,"Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'A' && (GetKeyState(VK_CONTROL)&0x8000)!=0)
	{
		CString str;
		GetWindowText(str);
		SetSel(0, str.GetLength(), TRUE);
		return TRUE;
	}
	else
	{
		return CEdit::PreTranslateMessage(pMsg);
	}
}


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMml2WavDlg ダイアログ



CMml2WavDlg::CMml2WavDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMml2WavDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXT_MML, txtMml_);
	DDX_Control(pDX, IDC_LST_DUTY_RATIO, lstDuty_);
	DDX_Control(pDX, IDC_CBO_DUTY_RATIO, cboDuty_);
	DDX_Control(pDX, IDC_CBO_CURVE, cboCurve_);
	DDX_Control(pDX, IDC_TXT_NOISE, txtNoise_);
	DDX_Control(pDX, IDC_TXT_DUTY_SWITCH_TIMING, txtDutySwictTiming_);
	DDX_Control(pDX, IDC_CBO_CALC_TYPE, cboFloatType_);
	DDX_Control(pDX, IDC_CBO_SAMPLE_RATE, cboSampleRate_);
	DDX_Control(pDX, IDC_TAB_BANK, tabBank_);
	DDX_Control(pDX, IDC_CBO_TONE_NUMBER, cboToneNumber_);
	DDX_Control(pDX, IDC_CHK_LOOP, chkLoop_);
	DDX_Control(pDX, IDC_CHK_START_FROM_CURRENT, chkFromCurrent_);
	DDX_Control(pDX, IDC_CHK_CURRENT_BANK, chkCurrentBank_);
	DDX_Control(pDX, IDC_CHK_EXTERNAL_IMPORT_INVALIDATE_TEMPO_COMMAND, chkInvalidateTempoCommand_);
	DDX_Control(pDX, IDC_CHK_DIVIDE_IMPORT_VOL_ENABLE_BANKS, chkDivideImportVol_);
	DDX_Control(pDX, IDC_TXT_LEVEL_NOISE, txtLevelNoise_);
	DDX_Control(pDX, IDC_CHK_EMULATE_8BIT, chkEmulate8Bit_);
}

BEGIN_MESSAGE_MAP(CMml2WavDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CMml2WavDlg::OnBnClickedBtnPlay)
	ON_BN_CLICKED(IDC_BTN_DUTY_RATIO_ADD, &CMml2WavDlg::OnBnClickedBtnDutyRatioAdd)
	ON_BN_CLICKED(IDC_BTN_DUTY_RATIO_DEL, &CMml2WavDlg::OnBnClickedBtnDutyRatioDel)
	ON_CBN_SELCHANGE(IDC_CBO_CURVE, &CMml2WavDlg::OnCbnSelchangeCboCurve)
	ON_EN_CHANGE(IDC_TXT_NOISE, &CMml2WavDlg::OnEnChangeTxtNoise)
	ON_EN_CHANGE(IDC_TXT_DUTY_SWITCH_TIMING, &CMml2WavDlg::OnEnChangeTxtDutySwitchTiming)
	ON_BN_CLICKED(IDC_BTN_OUTPUT, &CMml2WavDlg::OnBnClickedBtnOutput)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_BANK, &CMml2WavDlg::OnTcnSelchangeTabBank)
	ON_EN_CHANGE(IDC_TXT_MML, &CMml2WavDlg::OnEnChangeTxtMml)
	ON_BN_CLICKED(IDC_BUTTON1, &CMml2WavDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN__STOP, &CMml2WavDlg::OnBnClickedBtn)
	ON_BN_CLICKED(IDC_BTN_ENVELOPE_CMD, &CMml2WavDlg::OnBnClickedBtnEnvelopeCmd)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_DEFAULT, &CMml2WavDlg::OnBnClickedBtnEnvTemplateDefault)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_NONE, &CMml2WavDlg::OnBnClickedBtnEnvTemplateNone)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_STRONG_ATACK, &CMml2WavDlg::OnBnClickedBtnEnvTemplateStrongAtack)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_LONG_RELEASE, &CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_STRONG_ATACK2, &CMml2WavDlg::OnBnClickedBtnEnvTemplateStrongAtack2)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_LONG_RELEASE3, &CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease3)
	ON_BN_CLICKED(IDC_BTN_ENV_TEMPLATE_LONG_RELEASE4, &CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease4)
	ON_BN_CLICKED(IDC_BTN_CLEAR_TABS, &CMml2WavDlg::OnBnClickedBtnClearTabs)
	ON_BN_CLICKED(IDC_BTN_IMPORT_EXTERNAL_MML_FROM_CLIPBOARD, &CMml2WavDlg::OnBnClickedBtnImportExternalMmlFromClipboard)
	ON_BN_CLICKED(IDC_BTN_WAVE_TEST, &CMml2WavDlg::OnBnClickedBtnWaveTest)
	ON_COMMAND(ID_MNU_EXIT, &CMml2WavDlg::OnCmdExit)
	ON_COMMAND(ID_MNU_NEW_FILE, &CMml2WavDlg::OnCmdNewFile)
	ON_COMMAND(ID_MNU_OPEN_FILE, &CMml2WavDlg::OnCmdOpenFile)
	ON_COMMAND(ID_MNU_SAVE_AS_FILE, &CMml2WavDlg::OnCmdSaveAsFile)
	ON_COMMAND(ID_MNU_SAVE_FILE, &CMml2WavDlg::OnCmdSaveFile)
	ON_COMMAND(ID_ACC_EXIT, &CMml2WavDlg::OnCmdExit)
	ON_COMMAND(ID_ACC_NEW_FILE, &CMml2WavDlg::OnCmdNewFile)
	ON_COMMAND(ID_ACC_OPEN_FILE, &CMml2WavDlg::OnCmdOpenFile)
	ON_COMMAND(ID_ACC_SAVE_FILE, &CMml2WavDlg::OnCmdSaveFile)
	ON_COMMAND(ID_ACC_PLAY, &CMml2WavDlg::OnAccPlay)
	ON_COMMAND(ID_ACC_STOP, &CMml2WavDlg::OnAccStop)
END_MESSAGE_MAP()


// CMml2WavDlg メッセージ ハンドラー

BOOL CMml2WavDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	for (int i = 1; i < 100; i++)
	{
		CString str;
		str.Format("%d", i);
		cboDuty_.AddString(str);
	}
	cboCurve_.SetCurSel(0);
	cboDuty_.SetCurSel(49);
	cboFloatType_.SetCurSel(1);
	cboSampleRate_.SetCurSel(6);
	txtNoise_.SetWindowText("0");
	txtDutySwictTiming_.SetWindowText("0");

	tabBank_.InsertItem(0, "共通");
	tabBank_.InsertItem(1, "A");
	tabBank_.InsertItem(2, "B");
	tabBank_.InsertItem(3, "C");
	tabBank_.InsertItem(4, "D");
	tabBank_.InsertItem(5, "E");

	cboToneNumber_.SetCurSel(0);
	SetEnvelopeTemplate(0);
	GetDlgItem(IDC_TXT_IMPORT_VOL_MAX)->SetWindowText("15");
	GetDlgItem(IDC_TXT_WAVE_TEST)->SetWindowText("C");
	GetDlgItem(IDC_TXT_ENVELOPE_NO)->SetWindowText("0");
	GetDlgItem(IDC_TXT_LEVEL_NOISE)->SetWindowText("0");

	chkInvalidateTempoCommand_.SetCheck(1);
	chkDivideImportVol_.SetCheck(1);
	setWindowText();

	hAcc_ = LoadAccelerators(NULL, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CMml2WavDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMml2WavDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMml2WavDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMml2WavDlg::RefreshDutyList()
{
	lstDuty_.ResetContent();
	for (auto ratio : toneData_.dutyRatio)
	{
		CString str;
		str.Format("%f", ratio);
		lstDuty_.AddString(str);
	}
}



CMml2WavDlg::GeneratorWrapper* CMml2WavDlg::genWavReady(WavData& dest, bool checkMml)
{
	
	int sampleRate = 44100;
	auto readyGenerator = [&](auto dummyCalcTypeVariable, MmlUtility::GenerateMmlToPcmResult& result)->GeneratorWrapper*
	{			  
		CString rateStr;
		cboSampleRate_.GetWindowText(rateStr);
		rateStr.Replace(",", "");
		sampleRate = atoi(rateStr);
		bool ret = false;
		constexpr int TestTab = 1000;
		auto currentTab = tabBank_.GetCurSel() - 1;
		int curStart = 0, curEnd = 0;
		if(chkFromCurrent_.GetCheck()!=0)
			txtMml_.GetSel(curStart, curEnd);
		
		auto generator = new GeneratorWrapper();
		generator->reset<decltype(dummyCalcTypeVariable)>();
		if (chkCurrentBank_.GetCheck() == 0 && !checkMml)
		{
			result = generator->compile(shared_, bank_, sampleRate, currentTab, curStart);
		}
		else if (checkMml)
		{
			CString mml;
			GetDlgItem(IDC_TXT_WAVE_TEST)->GetWindowText(mml);
			std::array<std::string, BANKS> dummyBanks;
			dummyBanks[0] = genWaveCommand() + mml;
			result = generator->compile(shared_, dummyBanks, sampleRate, 0, 0);
		}
		else
		{
			if(currentTab<0)
			{
				MessageBox("共通タブのみを再生することはできません");
				result.errBank = -3;
				return nullptr;
			}

			std::array<std::string,BANKS> dummyBanks;
			dummyBanks[currentTab] = bank_[currentTab];
			result = generator->compile(shared_, dummyBanks, sampleRate, currentTab, curStart);
		}
		generator->skipToCurrent();

		
		if (result.result!= MmlUtility::ErrorReson::NoError)
		{
			std::string errMsg = "error:";
			if (result.errBank == -2)
			{
				errMsg += "内部MMLコード ";
				errMsg += std::to_string(result.errPos);
				errMsg += " 文字目";
			}
			else if (result.errBank == -1)
			{
				errMsg += "共通MMLコード ";
				errMsg += std::to_string(result.errPos);
				errMsg += " 文字目";
			}
			else if (result.errBank >=0)
			{
				if (!checkMml)
				{
					errMsg += "バンク";
					errMsg += ('A' + result.errBank);
				}
				errMsg += "\nMMLコード ";
				errMsg += std::to_string(result.errPos);
				errMsg += " 文字目";
			}

			if (result.errBank != -2 && result.errPos>=1)
			{
				if (!checkMml)
				{
					tabBank_.SetCurSel(result.errBank + 1);

					std::string* pStr;
					if (result.errBank == -1)
						pStr = &shared_;
					else
						pStr = &bank_[result.errBank];
					txtMml_.SetWindowText(pStr->c_str());
					txtMml_.SetSel((int)result.errPos-1, (int)result.errPos);
					txtMml_.SetFocus();
				}
			}
			using namespace MmlUtility;
			std::unordered_map< ErrorReson, const char*> errDetail;
			errDetail[ErrorReson::NoError] = "success";
			errDetail[ErrorReson::UnknownCommand] = "不明なコマンド";
			errDetail[ErrorReson::IllegalCommandNextSlur] = "スラーのあとに続けれるコマンドではない";
			errDetail[ErrorReson::IllegalValue] = "値が不正";
			errDetail[ErrorReson::IllegalValueTempo] = "テンポの値が不正";
			errDetail[ErrorReson::IllegalValueLength] = "長さの指定が不正";
			errDetail[ErrorReson::IllegalFormatEnvelopeCommand] = "エンベロープコマンドの書式が間違っている";
			errDetail[ErrorReson::IllegalParameterRange] = "パラメータの値が範囲外";
			errDetail[ErrorReson::SlurNotClose] = "スラーが繋がってないまま終わってる";
			errDetail[ErrorReson::ToneLevelOutOfRangeLower] = "音程が低すぎる";
			errDetail[ErrorReson::ToneLevelOutOfRangeUpper] = "音程が高すぎる";
			errDetail[ErrorReson::WaveNoOutOfRange] = "音色番号が範囲外";
			errDetail[ErrorReson::WaveDefineToneNoOutofRange] = "使えない音色番号";
			errDetail[ErrorReson::WaveDefineNoneDuty] = "デューティー比指定がない";
			errDetail[ErrorReson::WaveDefineOverDuties] = "ーティー比指定が多すぎる";
			errDetail[ErrorReson::PanOutOfRange] = "パン値が範囲外";
			errDetail[ErrorReson::VolumeOutOfRange] = "ボリューム値が範囲外";
			errDetail[ErrorReson::RequireNumberError] = "数値が入るところなのに数値じゃない";
			errDetail[ErrorReson::IliegalCommandInTuplet] = "連符中は単純な音符系とオクターブ変更のみ可能";
			errDetail[ErrorReson::TupletNoteOver] = "連符個数が多すぎる";
			errDetail[ErrorReson::NoteNothingTupletCloseOnNoTuplet] = "連符内に音符がない";
			errDetail[ErrorReson::IliegalTupletOpenInTuplet] = "連符内で連符を始めようとした";
			errDetail[ErrorReson::IlieagalLoopCommandInTuplet] = "連符内でループコマンドが出現した";
			errDetail[ErrorReson::IlieagalLoopEndNotLoop] = "ループ開始していないのにループ終了が来た";
			errDetail[ErrorReson::LoopNestOver] = "ループネスト数が限界を超えた";
			errDetail[ErrorReson::InvalidLoopNums] = "不正なループ回数指定";
			errDetail[ErrorReson::IliegalLfoType] = "存在しないLFOタイプ";
			errDetail[ErrorReson::IllegalFormatLfoCommand] = "LFOの書式が間違っている";
			errDetail[ErrorReson::LfoStartGreatorBeforeStart] = "LFOの開始位置がその前の開始位置より大きくないといけない";


			errDetail[ErrorReson::CanNotStartMacroInTuple]  = "連符の最中にマクロ記録を始めることはできません";
			errDetail[ErrorReson::CanNotStartMacroInLoop]  = "ループの最中にマクロ記録を始めることはできません";
			errDetail[ErrorReson::CanNotStartMacroInMacroo]  = "マクロのの最中にマクロ記録を始めることはできません";
			errDetail[ErrorReson::CanNotEndMacroInTuple]  = "連符の最中にマクロ記録を終えることはできません";
			errDetail[ErrorReson::CanNotEndMacroInLoop]  = "ループの最中にマクロ記録を終えることはできません";
			errDetail[ErrorReson::CanNotCallMacroInTuple] = "連符中にマクロを呼び出すことはできません";
			errDetail[ErrorReson::IliegalMacroCharacter] = "マクロの文字が不正です";
			errDetail[ErrorReson::CurrentPositionAfterInfinityLoop] = "現在位置が無限ループよりあとにある";
			errDetail[ErrorReson::NotFountToneLoop] = "ループ内に音がなるコマンドがない";

			errMsg += "\n";
			errMsg += errDetail[result.result];

			MessageBox(errMsg.c_str());
		}

		return generator;

	};
	MmlUtility::GenerateMmlToPcmResult result;
	bool ret = false;
	GeneratorWrapper* generator = nullptr;
	switch (cboFloatType_.GetCurSel())
	{
	case 0: generator = readyGenerator(CFixFloat<>(), result); break;
	case 1: generator = readyGenerator(float(0.0f), result); break;
	case 2: generator = readyGenerator(double(0.0), result); break;
	}
	if (generator==nullptr || result.result != MmlUtility::ErrorReson::NoError)
	{
		delete generator;
		return nullptr;
	}

	WAVEFORMATEX& format = dest.format;
	memset(&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.wBitsPerSample = 16;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.nSamplesPerSec = sampleRate;    //標本化周波数
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = sizeof(format);

	return generator;
}



void CALLBACK waveOutCallbackProc(
	HWAVEOUT  hwo,
	UINT      uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
	auto pThis = (CMml2WavDlg*)dwInstance;
	pThis->waveOutCallbackProc(hwo, uMsg);
}
void CMml2WavDlg::waveOutCallbackProc(
	HWAVEOUT  hwo,
	UINT      uMsg
)
{
	auto param = (WaveOutParam*)playing_;
	bool isWrite = false;
	switch (uMsg)
	{
	case WOM_OPEN:
		
		break;
	case WOM_DONE:
	{
		isWrite = param->generator_->isPlayEnd() == false;
		auto ridx = (param->bufferRIndex++) % WaveOutParam::Buffers;
		WAVEHDR& header = param->nowHeader[ridx];
		auto result = waveOutUnprepareHeader(hwo, &header, sizeof(header));
		if (result != 0)
			OutputDebugString("failed waveOutUnprepareHeader.");
		break;
	}
	case WOM_CLOSE:
		for(int i=0;i< WaveOutParam::Buffers;i++)
			waveOutUnprepareHeader(param->hWaveOut_, &param->nowHeader[i], sizeof(param->nowHeader[i]));
		param->hWaveOut_ = NULL;
		break;
	}

	if (isWrite && param->exitPlay_==false)
	{
		while(param->bufferWIndex - param->bufferRIndex < WaveOutParam::Buffers)
			writePcm();
	}
}

void CMml2WavDlg::OnBnClickedBtnWaveTest()
{
	play(true);
}

void CMml2WavDlg::OnBnClickedBtnPlay()
{
	play(false);
}
void CMml2WavDlg::play(bool isCheckWave)
{
	OnBnClickedBtn();

	WavData data;
	GeneratorWrapper* gen = genWavReady(data, isCheckWave);
	if (!gen)
		return;
	gen->setLoop(chkLoop_.GetCheck() != 0);
	//assert(playing_ == nullptr);
	auto playParam = new WaveOutParam();
	playParam->hWaveOut_ = nullptr;
	playParam->loopPlay_ = 0;
	playParam->generator_ = gen;
	playParam->bufferWIndex = 0;
	playParam->bufferRIndex = 0;
	playing_ = (playParam);


	auto result =  waveOutOpen(
		&playParam->hWaveOut_,
		-1,
		&data.format,
		(DWORD_PTR)::waveOutCallbackProc,
		(DWORD_PTR)this,
		CALLBACK_FUNCTION
	);

	if (result != 0)
	{
		MessageBox("Wav出力デバイスのオープンに失敗しました");
		return;
	}


	while (playParam->bufferWIndex - playParam->bufferRIndex < WaveOutParam::Buffers)
		writePcm();

}
#if 0
void CMml2WavDlg::preparePcm(bool isZero)
{
	auto param = (WaveOutParam*)playing_;
	int writeIdx = param->bufferPIndex % WaveOutParam::Buffers;
	WAVEHDR& header = param->nowHeader[writeIdx];
	memset(&header, 0, sizeof(header));
	header.lpData = (char*)param->pcmBuffer_[writeIdx];
	header.dwFlags = writeIdx == 0 ? WHDR_BEGINLOOP : 0;
	header.dwFlags = 0;// |= writeIdx == WaveOutParam::Buffers - 1 ? WHDR_ENDLOOP : 0;
	header.dwLoops = 0;

	if (isZero)
	{
		header.dwBufferLength = 0;
		param->pcmBuffer_[writeIdx][0] = 0;
	}
	else
	{
		header.dwBufferLength = StreamingBuffurSize;
		auto genPcm = param->generator_->generate<2, int16_t>(StreamingBuffurSize / 2 / sizeof(int16_t));
		memcpy(header.lpData, genPcm.data(), (std::min)(genPcm.size() * sizeof(int16_t), StreamingBuffurSize));
	}

	param->bufferPIndex++;
}
#endif

void CMml2WavDlg::writePcm(bool isZero)
{
	auto param = (WaveOutParam*)playing_;
	int writeIdx = param->bufferWIndex % WaveOutParam::Buffers;
	WAVEHDR& header = param->nowHeader[writeIdx];
	memset(&header, 0, sizeof(header));
	header.lpData = (char*)param->pcmBuffer_[writeIdx];
	header.dwLoops = 0;

	param->bufferWIndex++;

	header.dwBufferLength = StreamingBuffurSize;
	if (isZero)
	{
		memset(param->pcmBuffer_[writeIdx], 0, sizeof(param->pcmBuffer_[writeIdx]));
	}
	else
	{
		auto genPcm = param->generator_->generate<2, int16_t>(StreamingBuffurSize / 2 / sizeof(int16_t));
		memcpy(header.lpData, genPcm.data(), (std::min)(genPcm.size() * sizeof(int16_t), StreamingBuffurSize));
	}
	auto result = waveOutPrepareHeader(param->hWaveOut_, &header, sizeof(header));
	if (result != 0)
		OutputDebugString("failed waveOutPrepareHeader.");
	
	result = waveOutWrite(param->hWaveOut_, &header, sizeof(header));
	if (result != 0)
		OutputDebugString("failed waveOutWrite.");


}

void CMml2WavDlg::OnAccPlay()
{
	OnBnClickedBtn();
	play(false);
}


void CMml2WavDlg::OnAccStop()
{
	OnBnClickedBtn();
}

void CMml2WavDlg::OnBnClickedBtn()
{
	auto param = (WaveOutParam*)playing_;
	if(param)
	{
		if (param->hWaveOut_)
		{
			param->loopPlay_ = 0;
			param->exitPlay_ = true;
			while (param->bufferRIndex != param->bufferWIndex)
				Sleep(100);
			auto result = waveOutPause(param->hWaveOut_);
			result = waveOutClose(param->hWaveOut_);
			if (result != 0)
				OutputDebugString("failed waveOutClose.");
		}
	}

	while (playing_!=nullptr)
	{
		if (playing_->hWaveOut_ == NULL)
		{
			playing_ = nullptr;

		}
	}
}


void CMml2WavDlg::OnCbnSelchangeCboCurve()
{
	toneData_.curve = decltype(toneData_.curve)(cboCurve_.GetCurSel());
}

void CMml2WavDlg::OnBnClickedBtnDutyRatioAdd()
{
	CString str;
	cboDuty_.GetWindowText(str);
	toneData_.curve = decltype(toneData_.curve)(cboCurve_.GetCurSel());
	toneData_.dutyRatio.push_back(atof(str));
	RefreshDutyList();
}


void CMml2WavDlg::OnBnClickedBtnDutyRatioDel()
{
	int cur = lstDuty_.GetCurSel();
	if (cur >= 0 && cur < toneData_.dutyRatio.size())
		toneData_.dutyRatio.erase(toneData_.dutyRatio.begin() + cur);
	RefreshDutyList();
}



void CMml2WavDlg::OnEnChangeTxtNoise()
{
	CString str;
	txtNoise_.GetWindowText(str);
	auto noiseRnd = atoi(str);
	toneData_.randomRange = noiseRnd;
}


void CMml2WavDlg::OnEnChangeTxtDutySwitchTiming()
{
	CString str;
	txtDutySwictTiming_.GetWindowText(str);
	toneData_.cycle = atoi(str);
}


void CMml2WavDlg::OnBnClickedBtnOutput()
{
	WavData data;
	GeneratorWrapper* gen = genWavReady(data);
	if (!gen)
		return;

	CFileDialog dlg(FALSE, "wav", "output.wav", 6, "Wave File(*.wav)|*.wav||");
	if (dlg.DoModal() != IDOK)
		return;

	FILE* fp = nullptr;
	if (fopen_s(&fp, dlg.GetPathName(), "wb") == NOERROR && fp == NULL)
	{
		MessageBox("出力ファイルが開けなかった");
		return;
	}
	
	#define CHUNK(chunk) ((uint32_t(chunk[0])<<0)|(uint32_t(chunk[1])<<8)|(uint32_t(chunk[2])<<16)|(uint32_t(chunk[3])<<24))

	struct WAVE_FILE_HEADER {
		uint32_t riff;
		uint32_t riffSize;
		uint32_t format;
		uint32_t fmtChunk;
		uint32_t fmtChunkSize;
		uint16_t audioFormat;
		uint16_t channel;
		uint32_t sampleRate;
		uint32_t bps;//byte per 
		uint16_t blockSize;
		uint16_t bits;
		uint32_t dataChunk;
		uint32_t dataChunkSize;
	};
	std::vector<int16_t> pcm;
	gen->setLoop(false);
	gen->setDisableInfinityLoop(true);
	while (!gen->isPlayEnd())
	{
		auto generated = gen->generate<2, int16_t>(1);
		pcm.insert(pcm.end(), generated.begin(), generated.end());
	}

	WAVE_FILE_HEADER header;
	header.riff = CHUNK("RIFF");
	header.riffSize = uint32_t(sizeof(header) + pcm.size() * sizeof(int16_t) - 8);
	header.format = CHUNK("WAVE");
	header.fmtChunk = CHUNK("fmt ");
	header.fmtChunkSize = 16;
	header.audioFormat = 1;
	header.channel = data.format.nChannels;
	header.sampleRate = data.format.nSamplesPerSec;
	header.bps = data.format.nAvgBytesPerSec;
	header.blockSize = data.format.nBlockAlign;
	header.bits = data.format.wBitsPerSample;
	header.dataChunk = CHUNK("data");
	header.dataChunkSize = uint32_t(pcm.size() * sizeof(int16_t));

	fwrite(&header, sizeof(header), 1, fp);
	fwrite(pcm.data(), sizeof(int16_t), pcm.size(), fp);
	fclose(fp);

}

void CMml2WavDlg::setWindowText()
{
	CString title;
	title.Format("mml2wav Retro - %s %s", filePath_ == "" ? "無題" : filePath_, isDirty_ ? "*" : "");
	SetWindowText(title);

}

void CMml2WavDlg::OnTcnSelchangeTabBank(NMHDR* pNMHDR, LRESULT* pResult)
{
	auto index = tabBank_.GetCurSel();
	std::string* pStr;
	if (index == 0)
		pStr = &shared_;
	else
		pStr = &bank_[index-1];
	txtMml_.SetWindowText(pStr->c_str());
	*pResult = 0;
}


void CMml2WavDlg::OnEnChangeTxtMml()
{
	CString str;
	txtMml_.GetWindowText(str);
	auto index = tabBank_.GetCurSel();
	if(index==0)
		shared_= str;
	else
		bank_[index-1] = str;
	
	isDirty_ = true;

	setWindowText();
}

static std::string float2Str(float v)
{
	auto str = std::to_string(v);
	if (str.find('.') != std::string::npos)
	{
		char last = 0;
		do {
			last = str[str.length() - 1];
			if (last == '0' || last == '.')
				str.resize(str.length() - 1);
		} while (last == '0');
	}
	return str;
};

void CMml2WavDlg::OnBnClickedButton1()
{
	
	txtMml_.ReplaceSel(genWaveCommand());
	isDirty_ = true;
	setWindowText();
}

CString CMml2WavDlg::genWaveCommand()
{
	int toneNo = cboToneNumber_.GetCurSel();
	if(toneData_.dutyRatio.empty())
		MessageBox("Duty比が未設定");
	if (toneData_.randomRange < 0 || toneData_.randomRange>100)
	{
		MessageBox("ノイズ範囲が範囲外（0～100%）");
		return "";
	}

	CString levelNoiseStr;
	txtLevelNoise_.GetWindowText(levelNoiseStr);
	auto levelNoise = atof(levelNoiseStr);
	std::string waveCmd = "@W";
	waveCmd += std::to_string(toneNo);
	waveCmd += ':';
	waveCmd += std::to_string((int)toneData_.curve);

	if (toneData_.randomRange != 0)
	{
		waveCmd += 'R';
		waveCmd += float2Str(toneData_.randomRange);
	}
	if (levelNoise != 0)
	{
		waveCmd += 'N';
		waveCmd += float2Str(levelNoise);
	}
	if (toneData_.cycle != 0)
	{
		waveCmd += 'C';
		waveCmd += float2Str(toneData_.cycle);
	}
	for (auto duty : toneData_.dutyRatio)
	{
		waveCmd += ':';
		waveCmd += float2Str(duty);
	}

	waveCmd += "@";
	waveCmd += std::to_string(toneNo);

	return CString(waveCmd.c_str());

}


void CMml2WavDlg::SetEnvelopeTemplate(int no)
{
	struct EnvelopeSetting {
		float at,dt,rt;
		int al, sl;
	};
	constexpr EnvelopeSetting envelpeSetting[] = {
		{20,20,20,255,int(255 * 0.8)},
		{0,0,0,255,255},
		{50,50,20,255,int(255 * 0.5)},
		{20,80,20,255,int(255 * 0.5)},
		{10,10,100,int(255 * 0.9),int(255 * 0.8)},
		{20,20,150,int(255 * 0.9),int(255 * 0.8)},
		{30,20,200,int(255 * 0.9),int(255 * 0.8)},
	};
	GetDlgItem(IDC_TXT_ATACK_TIME)->SetWindowText(float2Str(envelpeSetting[no].at).c_str());
	GetDlgItem(IDC_TXT_DECAY_TIME)->SetWindowText(float2Str(envelpeSetting[no].dt).c_str());
	GetDlgItem(IDC_TXT_RELEASE_TIME)->SetWindowText(float2Str(envelpeSetting[no].rt).c_str());
	GetDlgItem(IDC_TXT_ATACK_LEVEL)->SetWindowText(std::to_string(envelpeSetting[no].al).c_str());
	GetDlgItem(IDC_TXT_SUSTAIN_LEVEL)->SetWindowText(std::to_string(envelpeSetting[no].sl).c_str());


}
void CMml2WavDlg::OnBnClickedBtnEnvelopeCmd()
{

	CString atackTimeStr;
	CString decayTimeStr;
	CString releaseTimeStr;
	CString atackLevelStr;
	CString sustainLevelStr;
	CString envNoStr;
	GetDlgItem(IDC_TXT_ATACK_TIME)->GetWindowText(atackTimeStr);
	GetDlgItem(IDC_TXT_DECAY_TIME)->GetWindowText(decayTimeStr);
	GetDlgItem(IDC_TXT_RELEASE_TIME)->GetWindowText(releaseTimeStr);
	GetDlgItem(IDC_TXT_ATACK_LEVEL)->GetWindowText(atackLevelStr);
	GetDlgItem(IDC_TXT_SUSTAIN_LEVEL)->GetWindowText(sustainLevelStr);
	GetDlgItem(IDC_TXT_ENVELOPE_NO)->GetWindowText(envNoStr);
	float atackTime = atof(atackTimeStr);
	float decayTime = atof(decayTimeStr);
	float releaseTime = atof(releaseTimeStr);
	int atackLevel = atoi(atackLevelStr);
	int sustainLevel = atoi(sustainLevelStr);
	int envNo = atoi(envNoStr);


	auto timeCheck = [this](const char* name, float value)->bool
	{
		if (value < 0)
		{
			CString msg;
			msg.Format("%sの時間が不正です(0ms～)", name);
			MessageBox(msg);
			return false;
		}
		return true;
	};
	auto levelCheck = [this](const char* name, int value)->bool
	{
		if (value < 0 || value>255)
		{
			CString msg;
			msg.Format("%sが不正です(0～255)", name);
			MessageBox(msg);
			return false;
		}
		return true;
	};	
	if (!timeCheck("atack time", atackTime))
		return;
	if (!timeCheck("decay time", decayTime))
		return;
	if (!timeCheck("release time", releaseTime))
		return;
	if (!timeCheck("atack time", atackTime))
		return;
	if (!levelCheck("atack level", atackLevel))
		return;
	if (!levelCheck("sustain level", sustainLevel))
		return;
	if (!levelCheck("番号", envNo))
		return;

	std::string envelopeCmd = "@ED";
	envelopeCmd += std::to_string(envNo) + ":";
	envelopeCmd += float2Str(atackTime) + ":";
	envelopeCmd += float2Str(decayTime) + ":";
	envelopeCmd += float2Str(releaseTime) + ":";
	envelopeCmd += std::to_string(atackLevel) + ":";
	envelopeCmd += std::to_string(sustainLevel);
	txtMml_.ReplaceSel(envelopeCmd.c_str());

	isDirty_ = true;
	setWindowText();

}


void CMml2WavDlg::OnBnClickedBtnEnvTemplateDefault()
{
	SetEnvelopeTemplate(0);
}


void CMml2WavDlg::OnBnClickedBtnEnvTemplateNone()
{
	SetEnvelopeTemplate(1);
}


void CMml2WavDlg::OnBnClickedBtnEnvTemplateStrongAtack()
{
	SetEnvelopeTemplate(2);
}


void CMml2WavDlg::OnBnClickedBtnEnvTemplateStrongAtack2()
{
	SetEnvelopeTemplate(3);
}

void CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease()
{
	SetEnvelopeTemplate(4);
}



void CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease3()
{
	SetEnvelopeTemplate(5);
}


void CMml2WavDlg::OnBnClickedBtnEnvTemplateLongRelease4()
{
	SetEnvelopeTemplate(6);
}


void CMml2WavDlg::OnBnClickedBtnClearTabs()
{
	OnCmdNewFile();
}


void CMml2WavDlg::OnBnClickedBtnImportExternalMmlFromClipboard()
{
	if (!IsClipboardFormatAvailable(CF_TEXT))
	{
		MessageBox("クリップボードにテキストがありません");
		return;
	}
	if (!OpenClipboard())
		return;

	auto hClip = GetClipboardData(CF_TEXT);
	if (hClip != NULL)
	{
		auto str = GlobalLock(hClip);
		if (str != NULL)
		{
			CString importStr((char*)str);
			loadMml(importStr, true, true);

			GlobalUnlock(hClip);

		}
	}
	CloseClipboard();
}

void CMml2WavDlg::loadMml(CString importStr, bool isVolumeControl, bool isImport)
{
	importStr = importStr.MakeUpper();

	auto startSharedPos = importStr.Find("SMML@");
	bool isSharedOk = 0;
	if (startSharedPos != -1)
	{
		startSharedPos += 5;
		auto endSharedPos = importStr.Find(";", startSharedPos);
		if (endSharedPos != -1)
		{
			auto sharedStr = importStr.Mid(startSharedPos, endSharedPos - startSharedPos);
			shared_ = sharedStr;
			importStr = importStr.Left(startSharedPos - 5) + importStr.Mid(endSharedPos + 1);
			isSharedOk = true;
		}
	}

	auto startPos = importStr.Find("MML@");
	if (startPos != -1)
		importStr = importStr.Mid(startPos + 4);
	auto endPos = importStr.Find(";");
	if (endPos != -1)
		importStr = importStr.Left(endPos);

	bool invalidateTempo = isImport && chkInvalidateTempoCommand_.GetCheck() != 0;

	importStr.Replace("\t", "");

	if (!isSharedOk && isImport)
	{
		if (importStr[0] == 'T')
		{
			int cnt = 1;
			while (importStr[cnt] >= '0' && importStr[cnt] <= '9')
				cnt++;
			shared_ = importStr.Left(cnt);
			importStr = importStr.Mid(cnt);
		}
		else
		{
			shared_ = "";
		}
	}

	CString maxVolStr;
	GetDlgItem(IDC_TXT_IMPORT_VOL_MAX)->GetWindowText(maxVolStr);
	int maxVol = atoi(maxVolStr);

	int enableBanks = 0;
	for (auto& bank : bank_)
	{
		auto splitPos = importStr.Find(',');
		if (splitPos != -1)
		{
			bank = importStr.Left(splitPos);
			importStr = importStr.Mid(splitPos + 1);
		}
		else
		{
			bank = importStr;
			importStr = "";
		}

		if (invalidateTempo)
		{
			int tcmd = -1;
			while ((tcmd = bank.find('T')) != std::string::npos)
			{
				int cnt = tcmd + 1;
				while (bank[cnt] >= '0' && bank[cnt] <= '9')
					cnt++;
				bank.erase(bank.begin() + tcmd, bank.begin() + cnt);
			}
		}
		if (bank.length() > 0)
			enableBanks++;
	}

	//事後音量調整
	if (isVolumeControl)
	{
		int baseMaxVol = 255;
		if (chkDivideImportVol_.GetCheck() && enableBanks > 0)
		{
			baseMaxVol /= enableBanks;
		}

		auto volModify = [baseMaxVol, maxVol](std::string& mml)
		{
			int vcmd = 0;
			while ((vcmd = mml.find('V', vcmd)) != std::string::npos)
			{
				int cnt = vcmd + 1;
				char buf[64] = {};
				char* p = buf;
				while (mml[cnt] >= '0' && mml[cnt] <= '9')
				{
					*p++ = mml[cnt];
					cnt++;
				}
				auto newVol = (std::min)(255, atoi(buf) * baseMaxVol / maxVol);
				auto newVolStr = std::to_string(newVol);
				mml.erase(mml.begin() + vcmd + 1, mml.begin() + cnt);
				mml.insert(mml.begin() + vcmd + 1, newVolStr.begin(), newVolStr.end());

				++vcmd;
			}
		};

		for (auto& bank : bank_)
			volModify(bank);
		volModify(shared_);
	}

	isDirty_ = false;
	auto index = tabBank_.GetCurSel();
	std::string* pStr;
	if (index == 0)
		pStr = &shared_;
	else
		pStr = &bank_[index - 1];
	txtMml_.SetWindowText(pStr->c_str());
	setWindowText();

}

bool CMml2WavDlg::closeFile()
{
	if (isDirty_)
	{
		auto ret = MessageBox("ファイルが変更されています、保存しますか？", "mml2wav retro", MB_YESNOCANCEL | MB_ICONQUESTION);
		if (ret == IDCANCEL)
		{
			return false;
		}
		else if (ret == IDYES)
		{
			OnCmdSaveFile();
		}
		else if (ret == IDNO)
		{
		}
	}
	txtMml_.SetWindowText("");
	shared_ = "";
	for (auto& bank : bank_)
		bank = "";
	isDirty_ = false;
	filePath_ = "";
	setWindowText();

	return true;
}

void CMml2WavDlg::OnCmdExit()
{
	CDialogEx::OnOK();
}

void CMml2WavDlg::OnCmdNewFile()
{
	if (!closeFile())
		return;

}


void CMml2WavDlg::OnCmdOpenFile()
{
	if (!closeFile())
		return;

	CString filePath = "";
	CFileDialog dlg(TRUE, "mml", "title.mml", 6, "mmlファイル(*.mml)|*.mml|テキストファイル(*.txt)|*.txt||", this);
	if (dlg.DoModal() == IDOK)
	{
		filePath = dlg.GetPathName();
	}
	else
	{
		return;
	}
	FILE* fp;
	if (fopen_s(&fp, filePath, "r") != 0 || fp==NULL)
	{
		MessageBox("失敗しました");
		return;
	}
	CString str;
	auto len = _filelength(_fileno(fp));
	char* buffer = new char[len + 1];
	buffer[len] = 0;
	fread(buffer, 1, len, fp);
	str += buffer;
	fclose(fp);
	filePath_ = filePath;

	loadMml(str, false);

}


void CMml2WavDlg::OnCmdSaveAsFile()
{
	filePath_ = "";
	OnCmdSaveFile();
}


void CMml2WavDlg::OnCmdSaveFile()
{
	auto fileChoice = [this]()
	{
		filePath_ = "";
		CFileDialog dlg(FALSE, "mml", "title.mml", 6, "mmlファイル(*.mml)|*.mml|テキストファイル(*.txt)|*.txt||", this);
		if (dlg.DoModal() == IDOK)
		{
			filePath_ = dlg.GetPathName();
		}
	};

	FILE* fp;
	while(fopen_s(&fp, filePath_, "w") != 0)
	{
		fileChoice();
		if (filePath_ == "")
			return;
	}
	//shared領域
	fputs("SMML@",fp);
	fputs(shared_.c_str(), fp);
	fputs(";", fp);
	fputs("MML@", fp);
	for (int i = 0; i < BANKS; i++)
	{
		fputs(bank_[i].c_str(), fp);
		fputs(i == BANKS - 1 ? ";" : ",", fp);
	}
	fclose(fp);
	isDirty_ = false;
	setWindowText();
}



BOOL CMml2WavDlg::PreTranslateMessage(MSG* pMsg)
{
	if (TranslateAccelerator(this->GetSafeHwnd(), hAcc_, pMsg))
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}

