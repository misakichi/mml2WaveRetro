#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "FixFloat.h"

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
	std::vector<int> dutyRatio; //通常の波形が50(%)
};
template<typename CalcT = CFixFloat<int64_t, 16>>
struct NoteCommand
{
	using CalcType = CalcT;
	CalcType toneFreq;	//tone freq.
	int length;			//NoteLengthResolutio単位の長さ
	uint8_t Slur;		//次の音と接続する
	uint8_t waveCurve;
	int bpm;
	int vol;
};

template<typename CalcT=CFixFloat<int64_t,16>>
class WavGenerator
{
public:
	using CalcType = CalcT;
	WavGenerator();
	//内部で扱う音の長さの分解能(単位)
	static constexpr int NoteLengthResolutio = 256; //ex. 256=256分音符まで

	inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
	inline std::vector<NoteCommand<CalcType>> compileMml(const char* mml);
	inline void addCommand(const NoteCommand<CalcType>& command);
	inline void addCommand(std::vector<NoteCommand<CalcType>> commands);
	inline void setTone(int no, const ToneData& tone);
	inline bool ready(uint32_t sampleRate);
	inline std::vector<int16_t> generate(int samples, bool loop);
private:
	static constexpr int SinTableResolution = 100;
	CalcType sinTable_[360 * SinTableResolution];	//0.01度刻みのsinテーブル、結果は10000倍値

	std::string mml_;
	std::unordered_map<int, ToneData> tones_;
	std::vector<NoteCommand<CalcType>> commands_;
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

		int toneIndex;
		int dutyIndex;

		int noteSamples;	//現在処理中のコマンドのサンプル数
		int noteProcedSamples; //処理済み

		bool toneOff;
		CalcType baseFreq;

		int waveStep; //波形のプラス側かマイナス側か(0=+ 1=-)
		CalcType waveFreqDiv2; //波形の半分プラマイの片方側分のサンプル数
		int waveDiv2InSample; //波形の半分プラマイの片方側分がどれだけ処理されたか

		CalcType slurFrom;	//前のノートとスラーの時の、前のノートのbaseFreq
		CalcType slurTo;	//次のノートとスラーの時の、次のノートのbaseFreq
	};
	PlayStatus status_;
	uint32_t sampleRate_;
	uint32_t volumeMax_ = 255; //Max of "V" command value


};

#include "WavGenerator.inl"