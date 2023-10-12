#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class WavGenerator
{
public:
	WavGenerator();
	//内部で扱う音の長さの分解能(単位)
	static constexpr int NoteLengthResolutio = 256; //ex. 256=256分音符まで

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
		std::vector<int> dutyRatio; //通常の波形が50(%)
	};
	struct NoteCommand
	{
		int toneOffset;		//tone offset from A0.
		int length;			//NoteLengthResolutio単位の長さ
		uint8_t Slur;		//前の音と接続する
		uint8_t waveCurve;
		int bpm;
		int vol;
	};

	std::vector<NoteCommand> compileMml(const char* mml);
	void addCommand(const NoteCommand& command);
	void addCommand(std::vector<NoteCommand> commands);
	void setTone(int no, const ToneData& tone);
	bool ready(uint32_t sampleRate);
	std::vector<int16_t> generate(int samples, bool loop);
private:
	static constexpr int SinTableResolution = 100;
	static constexpr int SinTableValueResolution = 10000;
	int16_t sinTable_[360 * SinTableResolution];	//0.01度刻みのsinテーブル、結果は10000倍値

	std::string mml_;
	std::unordered_map<int, ToneData> tones_;
	std::vector<NoteCommand> commands_;
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
		int noteSamples;	//現在処理中のコマンドのサンプル数
		int noteProcedSamples; //処理済み
	};
	PlayStatus status_;
	uint32_t sampleRate_;
};

