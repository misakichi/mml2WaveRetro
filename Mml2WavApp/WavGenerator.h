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
		UnknownCommand = (int)0x80000000,				   //�s���ȃR�}���h
		IllegalCommandNextSlur,					   //�X���[�̂��Ƃɑ������R�}���h�ł͂Ȃ�
		IllegalValue,																	   //�l���s��
		IllegalValueTempo,
		IllegalValueLength,
		IllegalFormatEnvelopeCommand,//�G���x���[�v�R�}���h�̏������Ԉ���Ă���
		IllegalParameterRange,//�p�����[�^�̒l���͈͊O
		SlurNotClose,//�X���[���q�����ĂȂ��܂܏I����Ă�
		ToneLevelOutOfRangeLower,//�������Ⴗ����
		ToneLevelOutOfRangeUpper,//��������������
		WaveNoOutOfRange,
		WaveDefineToneNoOutofRange,//�g���Ȃ����F�ԍ�
		WaveDefineNoneDuty,	 //�f���[�e�B�[��w�肪�Ȃ�
		WaveDefineOverDuties,//�f���[�e�B�[��w�肪��������
		PanOutOfRange,//�p���l���͈͊O
		VolumeOutOfRange,//�{�����[���l���͈͊O
		RequireNumberError, //���l������Ƃ���Ȃ̂ɐ��l����Ȃ�
		IliegalCommandInTuplet,
		TupletNoteOver,
		IliegalTupletCloseOnNoTuplet,
		NoteNothingTupletCloseOnNoTuplet,
		IliegalTupletOpenInTuplet,
		IlieagalLoopCommandInTuplet,		
		IlieagalLoopEndNotLoop,
		LoopNestOver,
		NotSupportInfinityLoop,
		
		IliegalLfoType,
		IllegalFormatLfoCommand,//�G���x���[�v�R�}���h�̏������Ԉ���Ă���
		LfoStartGreatorBeforeStart,
	};

	constexpr int LfoSettingParams = 6;

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
		float cycle = 0;
		float randomRange = 0;
		float levelNoise = 0;
		std::vector<float> dutyRatio; //�ʏ�̔g�`��50(%)
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
			CalcType length;			//NoteLengthResolutio�P�ʂ̒���
			CalcType noteTotalLength;	//�X���[���l���������v�̒���
			int SlurToCmdIdx;			//���̉��Ɛڑ�����
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
			int type;	//0:���� 1:���� 2:���ʊJ�n%start 3:�����J�n%start
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
	//#define USE_CALCED_SIN_TABLE
	template<typename CalcT = CFixFloat<int64_t, 16>>
	class WavGenerator
	{
	public:
		enum { ToneMax = 32 };
		using CalcType = CalcT;
		using TypedCommand = MmlCommand<CalcType>;
		WavGenerator();
		//�����ň������̒����̕���\(�P��)
		static constexpr int NoteLengthResolutio = 256; //ex. 256=256�������܂�
		static constexpr int LoopNestMax = 32;
		static constexpr int EnvelopeMax = 256;

		inline void setVolumeMax(uint32_t volumeMax) { volumeMax_ = volumeMax; }
		inline bool compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint = nullptr, ErrorReson* reason = nullptr, size_t current=0);
		inline void addCommand(const TypedCommand& command);
		inline void addCommand(std::vector<TypedCommand> commands);
		inline void setTone(int no, const ToneData& tone);
		inline bool ready(uint32_t sampleRate);

		template<int Channels>
		inline std::vector<int16_t> generate(size_t* currentOffset=nullptr);
	private:
#ifdef USE_CALCED_SIN_TABLE
		static constexpr int SinTableResolution = 100;
		CalcType sinTable_[360 * SinTableResolution];	//0.01�x���݂�sin�e�[�u���A���ʂ�10000�{�l
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
			CalcType percent;	//0���Ɠ��삵�Ȃ�
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
			CalcType slurCrossPoint; //�X���[�̈ڍs�_


			int noteSamples;		//���ݏ������̃R�}���h�̃T���v����
			int noteProcedSamples;	//�����ς�

			int toneSamples;		//���݂̉��̑��T���v����
			int toneCurSample;		//���݂̉��̃T���v���ʒu

			bool toneOff;
			CalcType baseFreqSamples;

			int waveStep; //�g�`�̃v���X�����}�C�i�X����(0=+ 1=-)
			CalcType waveFreqDiv2; //�g�`�̔����v���}�C�̕Е������̃T���v����
			CalcType waveDiv2InSample; //�g�`�̔����v���}�C�̕Е��������ǂꂾ���������ꂽ��
			int isSlurFrom;
			CalcType slurTo;	//���̃m�[�g�ƃX���[�̎��́A���̃m�[�g��baseFreqSamples
			
			Envelope envelopes[EnvelopeMax];
			Envelope* currentEnvelope = &envelopes[0];

			LfoState lfoTone;
			LfoState lfoVolume;

			int loopNum[LoopNestMax];
		};
		PlayStatus status_;
		uint32_t sampleRate_;
		uint32_t volumeMax_ = 255; //Max of "V" command value


	};

	struct GenerateMmlToPcmResult
	{
		ErrorReson result;
		size_t errBank;
		size_t errPos;
		std::vector<int16_t> pcm;
		size_t pcmStartOffset;
	};
	template<unsigned Channels, typename CalcT = CFixFloat<int64_t, 16>, int Banks = 1>
	bool generateMmlToPcm(GenerateMmlToPcmResult& dest, const std::string& prepareSharedMml, const std::array<std::string, Banks>& bankMml, int sampleRate = 48000, size_t currentBank=0, size_t currentCursor=0);

#include "WavGenerator.inl"
}
