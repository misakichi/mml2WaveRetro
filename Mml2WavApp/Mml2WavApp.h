//
//  Mml2WavApp.h
//
//  Copyright (c) 2023 misakichi kaminagare.
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file "LICENSE" or copy at
//  http://www.boost.org/LICENSE_1_0.txt
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
