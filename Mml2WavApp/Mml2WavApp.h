
// MFCApplication1.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CMml2WavApp:
// このクラスの実装については、MFCApplication1.cpp を参照してください
//

class CMml2WavApp : public CWinApp
{
public:
	CMml2WavApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CMml2WavApp theApp;
