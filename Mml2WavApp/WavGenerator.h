#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class WavGenerator
{
public:
	WavGenerator();
	//�����ň������̒����̕���\(�P��)
	static constexpr int NoteLengthResolutio = 256; //ex. 256=256�������܂�

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
		std::vector<int> dutyRatio; //�ʏ�̔g�`��50(%)
	};
	struct NoteCommand
	{
		int toneOffset;		//tone offset from A0.
		int length;			//NoteLengthResolutio�P�ʂ̒���
		uint8_t Slur;		//�O�̉��Ɛڑ�����
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
	int16_t sinTable_[360 * SinTableResolution];	//0.01�x���݂�sin�e�[�u���A���ʂ�10000�{�l

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
		int noteSamples;	//���ݏ������̃R�}���h�̃T���v����
		int noteProcedSamples; //�����ς�
	};
	PlayStatus status_;
	uint32_t sampleRate_;
};

