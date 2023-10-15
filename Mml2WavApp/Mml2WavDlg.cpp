
// MFCApplication1Dlg.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "Mml2WavApp.h"
#include "Mml2WavDlg.h"
#include "afxdialogex.h"

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
	WavGenerator<>* generator = new WavGenerator<>();
	CString mml;
	txtMml_.GetWindowText(mml);
	auto commands = generator->compileMml(mml);
	generator->setTone(0, toneData_);
	generator->addCommand(commands);

	if (generator->ready(44100) == false)
	{
		MessageBox("出力に必要なデータが足りません");
		delete generator;
		return;
	}

	auto pcm = generator->generate(INT_MAX, false);
	pcmBuffer_.clear();
	pcmBuffer_.insert(pcmBuffer_.end(), pcm.begin(), pcm.end());
	delete generator;

	WAVEFORMATEX format = {};
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.wBitsPerSample = 16;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.nSamplesPerSec = 44100;    //標本化周波数
	format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
	format.cbSize = sizeof(format);

	WAVEHDR header = {};
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
		&format,
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
	toneData_.dutyRatio.push_back(cboDuty_.GetCurSel() + 1);
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
	toneData_.randomRange = atoi(str);
}


void CMml2WavDlg::OnEnChangeTxtDutySwitchTiming()
{
	CString str;
	txtDutySwictTiming_.GetWindowText(str);
	toneData_.cycle = atoi(str);
}
