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
		ToneType,		//@
		SlurCrossPoint,	//S
		Envelope,		//@E

	};

	struct NoteParam {
		CalcType toneFreq;	//tone freq.
		int length;			//NoteLengthResolutio�P�ʂ̒���
		uint8_t Slur;		//���̉��Ɛڑ�����
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
		int64_t buffer[8];
	};
	ECommand command;

};

template<typename CalcT=CFixFloat<int64_t,16>>
class WavGenerator
{
public:
	enum class ErrorReson : int
	{
		NoError = 0,
		UnknownCommand = (int)0x80000000,
		IllegalCommandNextSlur,
		IllegalValue,
		IllegalFormatEnvelopeCommand,
		IllegalParameterRange,
		ToneLevelOutOfRangeLower,
		ToneLevelOutOfRangeUpper
	};

	using CalcType = CalcT;
	using TypedCommand = MmlCommand<CalcType>;
	WavGenerator();
	//�����ň������̒����̕���\(�P��)
	static constexpr int NoteLengthResolutio = 256; //ex. 256=256�������܂�

	inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
	inline bool compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint = nullptr, ErrorReson* reason = nullptr);
	inline void addCommand(const TypedCommand& command);
	inline void addCommand(std::vector<TypedCommand> commands);
	inline void setTone(int no, const ToneData& tone);
	inline bool ready(uint32_t sampleRate);
	inline std::vector<int16_t> generate(int samples, bool loop);
private:
	static constexpr int SinTableResolution = 100;
	CalcType sinTable_[360 * SinTableResolution];	//0.01�x���݂�sin�e�[�u���A���ʂ�10000�{�l

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
		CalcType volume;
		int toneIndex;
		int dutyIndex;
		CalcType slurCrossPoint; //�X���[�̈ڍs�_


		int noteSamples;	//���ݏ������̃R�}���h�̃T���v����
		int noteProcedSamples; //�����ς�

		bool toneOff;
		CalcType baseFreqSamples;

		int waveStep; //�g�`�̃v���X�����}�C�i�X����(0=+ 1=-)
		CalcType waveFreqDiv2; //�g�`�̔����v���}�C�̕Е������̃T���v����
		int waveDiv2InSample; //�g�`�̔����v���}�C�̕Е��������ǂꂾ���������ꂽ��

		CalcType slurTo;	//���̃m�[�g�ƃX���[�̎��́A���̃m�[�g��baseFreqSamples

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

#include "WavGenerator.inl"