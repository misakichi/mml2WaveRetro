#include "pch.h"
#include "WavGenerator.h"

#define _USE_MATH_DEFINES
#include <math.h>

static constexpr int toneScaleFreqX100[] =
{
	27500,
	29135,
	30868,
	32703,
	34648,
	36708,
	38891,
	41203,
	43654,
	46249,
	48999,
	51913,
	55000,
	58270,
	61735,
	65406,
	69296,
	73416,
	77782,
	82407,
	87307,
	92499,
	97999,
	103826,
	110000,
	116541,
	123471,
	130813,
	138591,
	146832,
	155563,
	164814,
	174614,
	184997,
	195998,
	207652,
	220000,
	233082,
	246942,
	261626,
	277183,
	293665,
	311127,
	329628,
	349228,
	369994,
	391995,
	415305,
	440000,
	466164,
	493883,
	523251,
	554365,
	587330,
	622254,
	659255,
	698456,
	739989,
	783991,
	830609,
	880000,
	932328,
	987767,
	1046502,
	1108731,
	1174659,
	1244508,
	1318510,
	1396913,
	1479978,
	1567982,
	1661219,
	1760000,
	1864655,
	1975533,
	2093005,
	2217461,
	2349318,
	2489016,
	2637020,
	2793826,
	2959955,
	3135963,
	3322438,
	3520000,
	3729310,
	3951066,
	4186009,
};

WavGenerator::WavGenerator()
{
	for (int i = 0; i < 360 * SinTableResolution; i++)
	{
		sinTable_[i] = int16_t(sin(i * M_PI / 180) * SinTableValueResolution);
	}
}
std::vector<WavGenerator::NoteCommand> WavGenerator::compileMml(const char* mml)
{
	std::vector<NoteCommand> commands;
	auto end = mml + strlen(mml);
	auto p = mml;
	
	struct State {
		int oct = 4;
		int v = 255;
		int curve = 0;
		int len = 4;
		int bpm = 120;
	} state;

	auto getNum = [&p]()
	{
		char num[32] = {};
		auto dst = num;
		while (*p >= '0' && *p <= '9')
		{
			*dst++ = *p++;
		}
		return atoi(num);
	};

	while(p<end)
	{
		auto c = *p++;
		if ((c >= 'A' && c <= 'G')||c=='R')
		{
			int offset = state.oct * 12;
			switch (c)
			{
			case 'A': offset += EToneScale::ToneScaleA; break;
			case 'B': offset += EToneScale::ToneScaleB; break;
			case 'C': offset += EToneScale::ToneScaleC; break;
			case 'D': offset += EToneScale::ToneScaleD; break;
			case 'E': offset += EToneScale::ToneScaleE; break;
			case 'F': offset += EToneScale::ToneScaleF; break;
			case 'G': offset += EToneScale::ToneScaleG; break;
			case 'R': offset = -1;
			}


			auto opt = *p;
			if (offset >= 0)
			{
				if (opt == '#' || opt == '+')
				{
					++p;
					offset++;
				}
				else if (opt == '-')
				{
					++p;
					offset--;
				}
			}
			assert(offset==-1 || offset >= EToneScale::ToneScaleA);
			if (offset > 0)
				offset -= EToneScale::ToneScaleA;

			int len = getNum();
			if (len == 0)
				len = state.len;

			len = NoteLengthResolutio / len;

			int addPeriod = 0;
			if (*p == '.')
			{
				p++;
				len += len/2;
			}
			int slur = 0;

			NoteCommand newCmd;
			newCmd.toneOffset = offset;
			newCmd.length = len;
			newCmd.Slur = slur;
			newCmd.vol = state.v;
			newCmd.waveCurve = state.curve;
			newCmd.bpm = state.bpm;
			commands.push_back(newCmd);
		}
		else if (c == 'L')
		{
			state.len = getNum();
		}
		else if (c == 'O')
		{
			state.oct = getNum();
		}
		else if (c == 'V')
		{
			state.v = getNum();
		}
		else if (c == 'T')
		{
			state.bpm = getNum();
		}
		else if (c == '@')
		{
			state.curve = getNum();
		}


	}

	return commands;
}
void WavGenerator::setTone(int no, const ToneData& tone)
{
	tones_[no] = tone;
}

bool WavGenerator::ready(uint32_t sampleRate)
{
	if (tones_.empty())
		return false;
	for (auto& tone : tones_)
		if (tone.second.dutyRatio.empty())
			return false;
	if(commands_.empty())
		return false;

	sampleRate_ = sampleRate;

	status_.commandIdx = 0;
	status_.noteProcedSamples = 0;
	status_.noteSamples = 0;
	status_.recentLength.clear();
	
	return true;
}

void  WavGenerator::addCommand(const NoteCommand& command)
{
	commands_.push_back(command);
}
void  WavGenerator::addCommand(std::vector<NoteCommand> commands)
{
	commands_.insert(commands_.end(), commands.begin(), commands.end());
}

std::vector<int16_t> WavGenerator::generate(int samples, bool loop)
{
	std::vector<int16_t> result;
	const ToneData* tone = nullptr; //nullの場合そのコマンドの情報を集め直すのだろう
	int freq;
	int numCycle;
	int enabledSampleX100;

	while (samples > 0)
	{
		if (status_.commandIdx >= commands_.size())
		{
			if (!loop)
				return result;
			status_.commandIdx = 0;
		}
		const auto& cmd = commands_[status_.commandIdx];
		if (status_.noteProcedSamples == status_.noteSamples)
		{
			//新規コマンドです
			uint64_t oldSamples = 0;
			uint64_t samples = 0;
			for (auto& li : status_.recentLength)
				oldSamples += li.samples;

			if (status_.recentLength.empty() || status_.recentLength.back().bpm != cmd.bpm)
			{
				status_.recentLength.push_back({ cmd.bpm,0 });
			}
			auto& lengthInfo = status_.recentLength.back();
			lengthInfo.length += cmd.length;

			//sampleRate_ * 60 = 1分間のサンプル数
			//bpm=1分間の四分音符数
			//length=分解能単位の長さ
			//bpm * 4 / length 
			lengthInfo.samples = sampleRate_ * 60 * 4* lengthInfo.length / NoteLengthResolutio / lengthInfo.bpm;
		
			for (auto& li : status_.recentLength)
				samples += li.samples;

			status_.noteSamples = int(samples- oldSamples);
			status_.noteProcedSamples = 0;
			tone = nullptr;
			
		}
		if (tone == nullptr)
		{
			tone = &tones_[cmd.waveCurve];

			freq = sampleRate_ * 100 / toneScaleFreqX100[cmd.toneOffset] * 100;
			numCycle = (status_.noteSamples * 100 / freq);
			enabledSampleX100 = numCycle * freq;
		}

		int16_t sample = 0;
		auto nowSampleX100 = status_.noteProcedSamples * 100;
		if (nowSampleX100 < enabledSampleX100)
		{
			auto toneIndex = (nowSampleX100 / freq) % tone->dutyRatio.size();
			auto inCyclePos = nowSampleX100 % freq;
			auto duty = tone->dutyRatio[toneIndex];

			auto cycleThreshold = freq * duty / 100;
			switch (tone->curve)
			{
			case EWaveCurveType::Rectangle:
			{
				if (cycleThreshold > inCyclePos)
					sample = cmd.vol * SHORT_MAX / 255;
				else
					sample = -cmd.vol * SHORT_MAX / 255;
				break;
			}
			case EWaveCurveType::Triangle:
			{
				if (cycleThreshold > inCyclePos)
				{
					auto cycleThresholdD2 = cycleThreshold >> 1;
					auto level = cycleThresholdD2 - abs(inCyclePos - cycleThresholdD2);
					sample = level * cmd.vol * SHORT_MAX / 255 / cycleThresholdD2;
				}
				else
				{
					auto inPos = inCyclePos - cycleThreshold;
					auto inThreshold = freq - cycleThreshold;
					auto inThresholdD2 = inThreshold >> 1;
					auto level = inThresholdD2 - abs(inPos - inThresholdD2);
					sample = -level * cmd.vol * SHORT_MAX / 255 / inThresholdD2;
				}
				break;
			}
			case EWaveCurveType::Saw:
				if (cycleThreshold > inCyclePos)
				{
					sample = (cycleThreshold - inCyclePos) * cmd.vol * SHORT_MAX / 255 / cycleThreshold;
				}
				else
				{
					auto inPos = inCyclePos - cycleThreshold;
					auto inThreshold = freq - cycleThreshold;
					sample = -inPos * cmd.vol * SHORT_MAX / 255 / inThreshold;
				}
				break;
			case EWaveCurveType::Sin:
			{
				int tablePos = 0;
				if (cycleThreshold > inCyclePos)
				{
					tablePos = inCyclePos * 180 * SinTableResolution / cycleThreshold;
				}
				else
				{
					auto inThreshold = freq - cycleThreshold;
					auto inPos = inCyclePos - cycleThreshold;
					tablePos = inPos * 180 * SinTableResolution / inThreshold;
					tablePos += 180 * SinTableResolution;
				}
				sample = cmd.vol * SHORT_MAX * sinTable_[tablePos] / (255 * SinTableValueResolution);
				break;
			}
			case EWaveCurveType::Noise:
				break;
			}
		}
		result.push_back(sample);
		--samples;

		status_.noteProcedSamples++;
		if (status_.noteProcedSamples == status_.noteSamples)
			status_.commandIdx++;

	}


	return result;
}