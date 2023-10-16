#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "FixFloat.h"

//�g�`�^�C�v
enum class EWaveCurveType {
	Rectangle,
	Triangle,
	Saw,
	Sin,
	Noise,
};

//���F�f�[�^
struct ToneData {
	EWaveCurveType curve = EWaveCurveType::Rectangle;
	int cycle = 0;
	int randomRange = 0;
	std::vector<int> dutyRatio; //�ʏ�̔g�`��50(%)
};
template<typename CalcT = CFixFloat<int64_t, 16>>
struct NoteCommand
{
	using CalcType = CalcT;
	CalcType toneFreq;	//tone freq.
	int length;			//NoteLengthResolutio�P�ʂ̒���
	uint8_t Slur;		//���̉��Ɛڑ�����
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
	//�����ň������̒����̕���\(�P��)
	static constexpr int NoteLengthResolutio = 256; //ex. 256=256�������܂�

	inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
	inline std::vector<NoteCommand<CalcType>> compileMml(const char* mml);
	inline void addCommand(const NoteCommand<CalcType>& command);
	inline void addCommand(std::vector<NoteCommand<CalcType>> commands);
	inline void setTone(int no, const ToneData& tone);
	inline bool ready(uint32_t sampleRate);
	inline std::vector<int16_t> generate(int samples, bool loop);
private:
	static constexpr int SinTableResolution = 100;
	CalcType sinTable_[360 * SinTableResolution];	//0.01�x���݂�sin�e�[�u���A���ʂ�10000�{�l

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

		int noteSamples;	//���ݏ������̃R�}���h�̃T���v����
		int noteProcedSamples; //�����ς�

		bool toneOff;
		CalcType baseFreq;

		int waveStep; //�g�`�̃v���X�����}�C�i�X����(0=+ 1=-)
		CalcType waveFreqDiv2; //�g�`�̔����v���}�C�̕Е������̃T���v����
		int waveDiv2InSample; //�g�`�̔����v���}�C�̕Е��������ǂꂾ���������ꂽ��

		CalcType slurFrom;	//�O�̃m�[�g�ƃX���[�̎��́A�O�̃m�[�g��baseFreq
		CalcType slurTo;	//���̃m�[�g�ƃX���[�̎��́A���̃m�[�g��baseFreq
	};
	PlayStatus status_;
	uint32_t sampleRate_;
	uint32_t volumeMax_ = 255; //Max of "V" command value


};

#include "WavGenerator.inl"