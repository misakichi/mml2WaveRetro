
// MFCApplication1Dlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "Mml2WavApp.h"
#include "Mml2WavDlg.h"
#include "afxdialogex.h"
#include <memory>
#pragma comment(lib,"Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	, hWaveOut_(nullptr)
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
	cboFloatType_.SetCurSel(0);
	cboSampleRate_.SetCurSel(0);
	txtNoise_.SetWindowText("0");
	txtDutySwictTiming_.SetWindowText("0");

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
		str.Format("%d", ratio);
		lstDuty_.AddString(str);
	}
}


bool CMml2WavDlg::genWavData(WavData& dest)
{
	if (toneData_.randomRange < 0 || toneData_.randomRange>100)
	{
		MessageBox("ノイズ範囲が範囲外（0～100%）");
		return false;
	}

	int sampleRate = 44100;
	auto genWrapper = [&](auto pcmGenerator)
	{
		switch (cboSampleRate_.GetCurSel())
		{
		case 0: sampleRate = 44100;	break;
		case 1: sampleRate = 48000;	break;
		case 2: sampleRate = 96000;	break;
		case 3: sampleRate = 192000; break;
		}												

		CString mml;
		txtMml_.GetWindowText(mml);
		
		std::vector<std::remove_pointer_t<decltype(pcmGenerator.get())>::TypedCommand> commands;
		size_t errPos;
		if (!pcmGenerator->compileMml(mml, commands, &errPos))
		{
			CString msg;
			msg.Format("MML %lld文字目でエラー", errPos);
			MessageBox(msg);
			return false;
		}
		pcmGenerator->setTone(0, toneData_);
		pcmGenerator->addCommand(commands);

		if (pcmGenerator->ready(sampleRate) == false)
		{
			MessageBox("出力に必要なデータが足りません");
			return false;
		}

		dest.data = pcmGenerator->generate<2>();
		pcmGenerator = nullptr;
		return true;

	};
	std::shared_ptr<MmlUtility::WavGenerator<>> generator;
	bool ret = false;
	switch (cboFloatType_.GetCurSel())
	{
	case 0: ret = genWrapper(std::make_unique<MmlUtility::WavGenerator<>>()); break;
	case 1: ret = genWrapper(std::make_unique<MmlUtility::WavGenerator<float>>()); break;
	case 2: ret = genWrapper(std::make_unique<MmlUtility::WavGenerator<double>>()); break;
	}
	if (!ret)
		return false;

	WAVEFORMATEX& format = dest.format;
	memset(&format, 0, sizeof(format));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.wBitsPerSample = 16;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.nSamplesPerSec = sampleRate;    //標本化周波数
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = sizeof(format);

	return true;
}

void CALLBACK waveOutCallbackProc(
	HWAVEOUT  hwo,
	UINT      uMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2
)
{
}

void CMml2WavDlg::OnBnClickedBtnPlay()
{
	WavData data;
	if (!genWavData(data))
		return;

	pcmBuffer_.clear();
	pcmBuffer_.insert(pcmBuffer_.end(), data.data.begin(), data.data.end());
	WAVEHDR header = {};
	memset(&header, 0, sizeof(header));
	header.lpData = (char*)pcmBuffer_.data();
	header.dwBufferLength = (DWORD)(pcmBuffer_.size() * sizeof(int16_t));
	header.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP;
	header.dwLoops = 1;


	if (hWaveOut_)
		waveOutClose(hWaveOut_);
	hWaveOut_ = nullptr;

	auto result =  waveOutOpen(
		&hWaveOut_,
		-1,
		&data.format,
		(DWORD_PTR)waveOutCallbackProc,
		0,
		CALLBACK_FUNCTION
	);
	if (result != 0)
	{
		MessageBox("Wav出力デバイスのオープンに失敗しました");
		return;
	}
	result = waveOutPrepareHeader(hWaveOut_, &header, sizeof(header));
	if (result!=0)
		OutputDebugString("failed waveOutPrepareHeader.");

	result = waveOutWrite(hWaveOut_, &header, sizeof(header));
	if (result != 0)
		OutputDebugString("failed waveOutWrite.");
}


void CMml2WavDlg::OnCbnSelchangeCboCurve()
{
	toneData_.curve = decltype(toneData_.curve)(cboCurve_.GetCurSel());
}

void CMml2WavDlg::OnBnClickedBtnDutyRatioAdd()
{
	toneData_.curve = decltype(toneData_.curve)(cboCurve_.GetCurSel());
	toneData_.dutyRatio.push_back((float)(cboDuty_.GetCurSel() + 1));
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
	if (!genWavData(data))
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
	WAVE_FILE_HEADER header;
	header.riff = CHUNK("RIFF");
	header.riffSize = uint32_t(sizeof(header) + data.data.size()* sizeof(decltype(data.data)::value_type) - 8);
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
	header.dataChunkSize = uint32_t(data.data.size() * sizeof(decltype(data.data)::value_type));

	fwrite(&header, sizeof(header), 1, fp);
	fwrite(data.data.data(), sizeof(decltype(data.data)::value_type), data.data.size(), fp);
	fclose(fp);

}
