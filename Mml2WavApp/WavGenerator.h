#pragma once
#ifndef _WAV_GENERATOR_H
#define _WAV_GENERATOR_H

//
//  WavGenerator.h
//
//  Copyright (c) 2023 misakichi kaminagare.
//  https://github.com/misakichi/mml2WaveRetro
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file "LICENSE" or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//

#include <string>
#include <vector>
#include <unordered_map>
#include <array>
#include "FixFloat.h"

namespace MmlUtility
{
	enum class ErrorReson : int
	{
		NoError = 0,														
		UnknownCommand = (int)0x80000000,				   //不明なコマンド
		IllegalCommandNextSlur,					   //スラーのあとに続けれるコマンドではない
		IllegalValue,																	   //値が不正
		IllegalValueTempo,
		IllegalValueLength,
		IllegalFormatEnvelopeCommand,//エンベロープコマンドの書式が間違っている
		IllegalParameterRange,//パラメータの値が範囲外
		SlurNotClose,//スラーが繋がってないまま終わってる
		ToneLevelOutOfRangeLower,//音程が低すぎる
		ToneLevelOutOfRangeUpper,//音程が高すぎる
		WaveNoOutOfRange,
		WaveDefineToneNoOutOfRange,//使えない音色番号
		WaveDefineWaveTypeNoOutOfRange,//使えない波形種別
		WaveDefineFreqFlucOutOfRange, //周波数変動範囲外
		WaveDefineLevelNoiseOutOfRange, //レベルノイズランダム範囲外
		WaveDefineDutyCycleOutOfRange, //Duty切り替えタイミング範囲外
		WaveDefineDutyRatioOutOfRange, //Duty比範囲外
		WaveDefineNoneDuty,	 //デューティー比指定がない
		WaveDefineOverDuties,//デューティー比指定が多すぎる
		PanOutOfRange,//パン値が範囲外
		VolumeOutOfRange,//ボリューム値が範囲外
		RequireNumberError, //数値が入るところなのに数値じゃない
		IliegalCommandInTuplet,
		TupletNoteOver,
		IliegalTupletCloseOnNoTuplet,
		NoteNothingTupletCloseOnNoTuplet,
		IliegalTupletOpenInTuplet,
		IlieagalLoopCommandInTuplet,		
		IlieagalLoopEndNotLoop,
		LoopNestOver,
		InvalidLoopNums,
		
		IliegalLfoType,
		IllegalFormatLfoCommand,//エンベロープコマンドの書式が間違っている
		LfoStartGreatorBeforeStart,

		CanNotStartMacroInTuple,
		CanNotStartMacroInLoop,
		CanNotStartMacroInMacroo,
		CanNotEndMacroInTuple,
		CanNotEndMacroInLoop,
		CanNotCallMacroInTuple,
		IliegalMacroCharacter,
		CurrentPositionAfterInfinityLoop,
		NotFountToneLoop,
	};

	constexpr int LfoSettingParams = 6;

	//波形タイプ
	enum class EWaveCurveType {
		Rectangle,
		Triangle,
		Saw,
		Sin,
		Noise,
	};

	//音色データ
	struct ToneData {
		EWaveCurveType curve = EWaveCurveType::Rectangle;
		float cycle = 0;
		float randomRange = 0;
		float levelNoise = 0;
		std::vector<float> dutyRatio; //通常の波形が50(%)
	};

	template<typename CalcT = CFixFloat<int64_t, 16>>
	class BiQuadLpf
	{
	public:
		using CalcType = CalcT;
		BiQuadLpf();

		CalcType lpf(CalcType value, CalcType freq, int samplerate);

		CalcType in[2];
		CalcType out[2];
	};

	template<typename CalcT = CFixFloat<int64_t, 16>>
	class MmlCommand
	{
	public:
		using CalcType = CalcT;

		MmlCommand() { memset(buffer, 0,sizeof(buffer)); isCurrent = 0; }
		MmlCommand(const MmlCommand& src)
		{
			command = src.command;
			isCurrent = src.isCurrent;
			memcpy(buffer, src.buffer, sizeof(buffer));
		}
		MmlCommand& operator = (const MmlCommand& src)
		{
			command = src.command;
			isCurrent = src.isCurrent;
			memcpy(buffer, src.buffer, sizeof(buffer));
			return *this;
		}

		enum class ECommand : int {
			NOP=-1,
			ToneNote,		//cdefgab
			Tempo,			//T
			Volume,			//V
			Pan,			//P
			ToneType,		//@
			SlurCrossPoint,	//@S
			EnvelopeDefine,	//@ED
			EnvelopeCall,	//@EC
			WaveDefine,		//@W
			Loop,			//[]
			LFO,			//@L
		};

		struct NoteParam {
			CalcType toneFreq;			//tone freq.
			CalcType length;			//NoteLengthResolutio単位の長さ
			CalcType noteTotalLength;	//スラーを考慮した合計の長さ
			int SlurToCmdIdx;			//次の音と接続する
		};
		struct EnvelopeParam {
			CalcType atackTime;		//msec
			CalcType decayTime;		//msec
			CalcType releaseTime;	//msec
			int atackLevel;
			int sustainLevel;
			int no;
		};
		struct LoopCommand {
			int type; //0:start, 1:end, 2:exit
			int num;
			int pairIndex;
			int id;
		};
		struct WaveDefine {
			enum { MaxDuties = 8 };
			int no;
			int type;
			int duties;
			float random;
			float levelNoise;
			float cycle;
			float duty[MaxDuties];
		};
		struct LFO
		{
			int type;	//0:音量 1:音程 2:音量開始%start 3:音程開始%start
			int settings;
			struct Setting
			{
				float start;			 //msec or %
				int lerp;
				float cycleMsec;
				float percent;
			};
			Setting setting[LfoSettingParams];
		};
		union {
			NoteParam note;
			EnvelopeParam envelope;
			WaveDefine wave;
			LoopCommand loop;
			LFO lfo;
			uint8_t waveCurve;
			CalcType slurCrossPoint;
			int bpm;
			int vol;
			int pan; //127=center
			int64_t buffer[13];
		};
		static_assert(sizeof(buffer) >= sizeof(note));
		static_assert(sizeof(buffer) >= sizeof(envelope));
		static_assert(sizeof(buffer) >= sizeof(wave));
		static_assert(sizeof(buffer) >= sizeof(loop));
		static_assert(sizeof(buffer) >= sizeof(lfo));
		ECommand command;
		int isCurrent;
	};

	template<unsigned Channels, typename Type>
	struct Sample
	{
		Type	sample[Channels];
	};

	//#define USE_CALCED_SIN_TABLE
	template<typename CalcT = CFixFloat<int64_t, 16>>
	class WavGenerator
	{
	public:
		static constexpr int ToneMax = 32;
		using CalcType = CalcT;
		using TypedCommand = MmlCommand<CalcType>;
		WavGenerator();
		//内部で扱う音の長さの分解能(単位)
		static constexpr int NoteLengthResolutio = 256; //ex. 256=256分音符まで
		static constexpr int LoopNestMax = 32;
		static constexpr int EnvelopeMax = 256;

		inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
		inline bool compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint = nullptr, ErrorReson* reason = nullptr, size_t current=0);
		inline void addCommand(const TypedCommand& command);
		inline void addCommand(std::vector<TypedCommand> commands);
		inline void setTone(int no, const ToneData& tone);
		inline bool ready(uint32_t sampleRate, bool loopPlay=false);

		template<unsigned Channels, typename Type>
		inline std::vector<Type> generateSamples(unsigned samples);

		template<unsigned Channels, typename Type>
		Sample<Channels, Type> generate(bool* isCurrent=nullptr, bool loopWait = false);

		void setLoop(bool isLoop) { loopPlay_ = isLoop; }
		void setDisableInfinityLoop(bool disableInfinityLoop) { disableInfinityLoop_ = disableInfinityLoop; }
		bool isPlayEnd() const { return loopPlay_ == false && isEndCommand(); }
		bool isEndCommand() const { return status_.commandIdx >= commands_.size(); }
		size_t currentCommandIndex()const { return status_.commandIdx; }
		size_t commandCount()const { return  commands_.size(); }

	private:
#ifdef USE_CALCED_SIN_TABLE
		static constexpr int SinTableResolution = 100;
		CalcType sinTable_[360 * SinTableResolution];	//0.01度刻みのsinテーブル、結果は10000倍値
#endif
		std::string mml_;
		std::array<ToneData,ToneMax> tones_;
		std::vector<TypedCommand> commands_;
		enum EToneScale {
			ToneScalePrevB = -1,
			ToneScaleC,
			ToneScaleCS,
			ToneScaleD,
			ToneScaleDS,
			ToneScaleE,
			ToneScaleF,
			ToneScaleFS,
			ToneScaleG,
			ToneScaleGS,
			ToneScaleA,
			ToneScaleAS,
			ToneScaleB,
			ToneScaleNextA,
			Num,
		};

		struct RecentLength {
			int bpm;
			CalcType length;
			uint64_t samples;
		};
		struct Envelope
		{
			Envelope& operator=(const Envelope& src) {
				envelopeAtackSamples = src.envelopeAtackSamples;
				envelopeDecaySamples = src.envelopeDecaySamples;
				envelopeReleaseSamples = src.envelopeReleaseSamples;
				envelopeAtackLevel = src.envelopeAtackLevel;
				envelopeSustainLevel = src.envelopeSustainLevel;
				return *this;
			}
			CalcType envelopeAtackSamples;		//sample
			CalcType envelopeDecaySamples;		//sample
			CalcType envelopeReleaseSamples;	//sample
			CalcType envelopeAtackLevel;
			CalcType envelopeSustainLevel;

		};
		struct PlayStatus;
		struct LfoState {
			void updateSetting(const PlayStatus* playStatus, int sampleRate);
			const struct MmlCommand<CalcType>::LFO* lfoParams;
			int settingIndex;
			CalcType freqSample;
			CalcType sampleProgress;
			CalcType percent;	//0だと動作しない
		};
		struct PlayStatus {
			std::vector<RecentLength> recentLength;
			int commandIdx;


			int bpm;
			int pan;
			CalcType volume;
			int toneIndex;
			int dutyIndex;
			int dutyCycleRq;
			CalcType slurCrossPoint; //スラーの移行点


			int noteSamples;		//現在処理中のコマンドのサンプル数
			int noteProcedSamples;	//処理済み

			int toneSamples;		//現在の音の総サンプル数
			int toneCurSample;		//現在の音のサンプル位置

			bool toneOff;
			CalcType baseFreqSamples;

			int waveStep; //波形のプラス側かマイナス側か(0=+ 1=-)
			CalcType waveFreqDiv2; //波形の半分プラマイの片方側分のサンプル数
			CalcType waveDiv2InSample; //波形の半分プラマイの片方側分がどれだけ処理されたか
			int isSlurFrom;
			CalcType slurTo;	//次のノートとスラーの時の、次のノートのbaseFreqSamples
			CalcType duty; //ノイズ用duty維持
			
			Envelope envelopes[EnvelopeMax];
			Envelope* currentEnvelope = &envelopes[0];

			LfoState lfoTone;
			LfoState lfoVolume;

			int loopNum[LoopNestMax];
		};
		PlayStatus status_;
		uint32_t sampleRate_;
		bool loopPlay_ = false;
		bool disableInfinityLoop_ = false;
		uint32_t volumeMax_ = 255; //Max of "V" command value

		BiQuadLpf<CalcType> lpf_;
	};

	struct GenerateMmlToPcmResult
	{
		ErrorReson result = ErrorReson::NoError;
		size_t errBank = 0;
		size_t errPos = 0;
		::std::vector<int16_t> pcm;
		size_t pcmStartOffset;
	};

	template<typename CalcT = CFixFloat<int64_t, 16>, int Banks = 1>
	class MultiBankMml 
	{
	public:
		using CalcType = CalcT;
		inline GenerateMmlToPcmResult compile(const std::string& prepareSharedMml, const std::array<std::string, Banks>& bankMml, int sampleRate = 48000, size_t currentBank = 0, size_t currentCursor = 0);
		inline void skipToCurrent();		
		template<unsigned Channel, typename Type> inline typename std::vector<Type> generate(int samples);
		void setLoop(bool loop) {
			for (auto& bank : bank_)
				bank.setLoop(loop);
		}
		void setDisableInfinityLoop(bool disableInfinityLoop)
		{
			for (auto& bank : bank_)
				bank.setDisableInfinityLoop(disableInfinityLoop);
		}

		bool isPlayEnd() const {
			bool ret = true;
			for (int i = 0; i < Banks; i++)
			{
				ret &= bank_[i].isPlayEnd();
			}
			return ret;
		}

		size_t currentCommandIndex(int bank)
		{
			if (bank >= 0 && bank < Banks)
				return bank_[bank].currentCommandIndex();
			else
				return 0;
		}
		size_t commandCount(int bank)
		{
			if (bank >= 0 && bank < Banks)
				return bank_[bank].commandCount();
			else
				return 0;
		}

	private:
		WavGenerator<CalcT> bank_[Banks];
		bool compiled_ = false;
		size_t currentBank_;
		size_t currentCursor_;
	};

	template<unsigned Channels, typename CalcT = CFixFloat<int64_t, 16>, int Banks = 1>
	inline bool generateMmlToPcm(GenerateMmlToPcmResult& dest, const std::string& prepareSharedMml, const std::array<std::string, Banks>& bankMml, int sampleRate = 48000, size_t currentBank=0, size_t currentCursor=0);

#include "WavGenerator.inl"
}

#endif