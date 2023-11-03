#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "FixFloat.h"

namespace MmlUtility
{
	enum class ErrorReson : int
	{
		NoError = 0,
		UnknownCommand = (int)0x80000000,
		IllegalCommandNextSlur,
		IllegalValue,
		IllegalFormatEnvelopeCommand,
		IllegalParameterRange,
		SlurNotClose,
		ToneLevelOutOfRangeLower,
		ToneLevelOutOfRangeUpper,
		PanOutOfRange,
		VolumeOutOfRange,
	};


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
		int cycle = 0;
		int randomRange = 0;
		std::vector<float> dutyRatio; //通常の波形が50(%)
	};
	template<typename CalcT = CFixFloat<int64_t, 16>>
	class MmlCommand
	{
	public:
		using CalcType = CalcT;
		MmlCommand() {}
		MmlCommand(const MmlCommand& src)
		{
			command = src.command;
			memcpy(buffer, src.buffer, sizeof(buffer));
		}
		MmlCommand& operator = (const MmlCommand& src)
		{
			command = src.command;
			memcpy(buffer, src.buffer, sizeof(buffer));
			return *this;
		}

		enum class ECommand : int {
			ToneNote,		//cdefgab
			Tempo,			//T
			Volume,			//V
			Pan,			//P
			ToneType,		//@
			SlurCrossPoint,	//@S
			Envelope,		//@E
		};

		struct NoteParam {
			CalcType toneFreq;	//tone freq.
			int length;			//NoteLengthResolutio単位の長さ
			int SlurToCmdIdx;	//次の音と接続する
		};
		struct EnvelopeParam {
			int atackTime;		//msec
			int decayTime;		//msec
			int releaseTime;	//msec
			int atackLevel;
			int sustainLevel;
		};
		union {
			NoteParam note;
			EnvelopeParam envelope;
			uint8_t waveCurve;
			CalcType slurCrossPoint;
			int bpm;
			int vol;
			int pan; //127=center
			int64_t buffer[8];
		};
		ECommand command;

	};
	//#define USE_CALCED_SIN_TABLE
	template<typename CalcT = CFixFloat<int64_t, 16>>
	class WavGenerator
	{
	public:

		using CalcType = CalcT;
		using TypedCommand = MmlCommand<CalcType>;
		WavGenerator();
		//内部で扱う音の長さの分解能(単位)
		static constexpr int NoteLengthResolutio = 256; //ex. 256=256分音符まで

		inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
		inline bool compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint = nullptr, ErrorReson* reason = nullptr);
		inline void addCommand(const TypedCommand& command);
		inline void addCommand(std::vector<TypedCommand> commands);
		inline void setTone(int no, const ToneData& tone);
		inline bool ready(uint32_t sampleRate);

		template<int Channels>
		inline std::vector<int16_t> generate();
	private:
#ifdef USE_CALCED_SIN_TABLE
		static constexpr int SinTableResolution = 100;
		CalcType sinTable_[360 * SinTableResolution];	//0.01度刻みのsinテーブル、結果は10000倍値
#endif
		std::string mml_;
		std::unordered_map<int, ToneData> tones_;
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
			uint64_t length;
			uint64_t samples;
		};
		struct PlayStatus {
			std::vector<RecentLength> recentLength;
			int commandIdx;


			int bpm;
			int pan;
			CalcType volume;
			int toneIndex;
			int dutyIndex;
			CalcType slurCrossPoint; //スラーの移行点


			int noteSamples;	//現在処理中のコマンドのサンプル数
			int noteProcedSamples; //処理済み

			bool toneOff;
			CalcType baseFreqSamples;

			int waveStep; //波形のプラス側かマイナス側か(0=+ 1=-)
			CalcType waveFreqDiv2; //波形の半分プラマイの片方側分のサンプル数
			int waveDiv2InSample; //波形の半分プラマイの片方側分がどれだけ処理されたか
			int isSlurFrom;
			CalcType slurTo;	//次のノートとスラーの時の、次のノートのbaseFreqSamples

			CalcType envelopeAtackSamples;		//sample
			CalcType envelopeDecaySamples;		//sample
			CalcType envelopeReleaseSamples;	//sample
			CalcType envelopeAtackLevel;
			CalcType envelopeSustainLevel;
		};
		PlayStatus status_;
		uint32_t sampleRate_;
		uint32_t volumeMax_ = 255; //Max of "V" command value


	};

	struct GenerateMmlToPcmResult
	{
		ErrorReson result;
		size_t errPos;
		std::vector<int16_t> pcm;
	};
	template<unsigned Channels, typename CalcT = CFixFloat<int64_t, 16>, int Banks = 1>
	bool generateMmlToPcm(GenerateMmlToPcmResult& dest, const std::string& prepareSharedMml, const std::array<std::string, Banks>& bankMml, int sampleRate = 48000);

#include "WavGenerator.inl"
}
