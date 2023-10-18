
#define _USE_MATH_DEFINES
#include <math.h>

template<typename CalcT>
inline WavGenerator<CalcT>::WavGenerator()
{
	for (int i = 0; i < 360 * SinTableResolution; i++)
	{
		sinTable_[i] = CalcType(sin(i * M_PI / 180.0));
	}
}

template<typename CalcT>
inline std::vector<MmlCommand<CalcT>> WavGenerator<CalcT>::compileMml(const char* mml)
{
	static const CalcType toneScaleFreq[] =
	{
		CalcType(27.500),
		CalcType(29.135),
		CalcType(30.868),
		CalcType(32.703),
		CalcType(34.648),
		CalcType(36.708),
		CalcType(38.891),
		CalcType(41.203),
		CalcType(43.654),
		CalcType(46.249),
		CalcType(48.999),
		CalcType(51.913),
		CalcType(55.000),
		CalcType(58.270),
		CalcType(61.735),
		CalcType(65.406),
		CalcType(69.296),
		CalcType(73.416),
		CalcType(77.782),
		CalcType(82.407),
		CalcType(87.307),
		CalcType(92.499),
		CalcType(97.999),
		CalcType(103.826),
		CalcType(110.000),
		CalcType(116.541),
		CalcType(123.471),
		CalcType(130.813),
		CalcType(138.591),
		CalcType(146.832),
		CalcType(155.563),
		CalcType(164.814),
		CalcType(174.614),
		CalcType(184.997),
		CalcType(195.998),
		CalcType(207.652),
		CalcType(220.000),
		CalcType(233.082),
		CalcType(246.942),
		CalcType(261.626),
		CalcType(277.183),
		CalcType(293.665),
		CalcType(311.127),
		CalcType(329.628),
		CalcType(349.228),
		CalcType(369.994),
		CalcType(391.995),
		CalcType(415.305),
		CalcType(440.000),
		CalcType(466.164),
		CalcType(493.883),
		CalcType(523.251),
		CalcType(554.365),
		CalcType(587.330),
		CalcType(622.254),
		CalcType(659.255),
		CalcType(698.456),
		CalcType(739.989),
		CalcType(783.991),
		CalcType(830.609),
		CalcType(880.000),
		CalcType(932.328),
		CalcType(987.767),
		CalcType(1046.502),
		CalcType(1108.731),
		CalcType(1174.659),
		CalcType(1244.508),
		CalcType(1318.510),
		CalcType(1396.913),
		CalcType(1479.978),
		CalcType(1567.982),
		CalcType(1661.219),
		CalcType(1760.000),
		CalcType(1864.655),
		CalcType(1975.533),
		CalcType(2093.005),
		CalcType(2217.461),
		CalcType(2349.318),
		CalcType(2489.016),
		CalcType(2637.020),
		CalcType(2793.826),
		CalcType(2959.955),
		CalcType(3135.963),
		CalcType(3322.438),
		CalcType(3520.000),
		CalcType(3729.310),
		CalcType(3951.066),
		CalcType(4186.009),
	};

	std::vector<MmlCommand<CalcType>> commands;
	auto end = mml + strlen(mml);
	auto p = mml;
	
	struct State {
		int oct = 4;
		int len = 4;
	} state;

	MmlCommand<CalcType> defaultVol;
	defaultVol.command = MmlCommand<CalcType>::ECommand::Volume;
	defaultVol.vol = volumeMax_ / 2;
	commands.push_back(defaultVol);

	MmlCommand<CalcType> defaultTempo;
	defaultVol.command = MmlCommand<CalcType>::ECommand::Tempo;
	defaultVol.bpm = 120;;
	commands.push_back(defaultVol);

	MmlCommand<CalcType> defaultToneType;
	defaultVol.command = MmlCommand<CalcType>::ECommand::ToneType;
	defaultVol.waveCurve = 0;
	commands.push_back(defaultToneType);

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
			MmlCommand<CalcType> cmd;
			cmd.command = MmlCommand<CalcType>::ECommand::ToneNote;
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
			if (*p == '&')
			{
				p++;
				slur = 1;
			}

			cmd.note.toneFreq = offset <= 0 ? 0 : toneScaleFreq[offset];
			cmd.note.length = len;
			cmd.note.Slur = slur;
			commands.push_back(cmd);
		}
		else if (c == 'L')
		{
			state.len = getNum();
		}
		else if (c == 'O')
		{
			state.oct = getNum();
		}
		else if (c == '<')
		{
			state.oct--;
		}
		else if (c == '>')
		{
			MmlCommand<CalcType> cmdVol;
			cmdVol.command = MmlCommand<CalcType>::ECommand::Volume;
			cmdVol.vol = getNum();
			commands.push_back(cmdVol);

		}
		else if (c == 'T')
		{
			MmlCommand<CalcType> cmdTempo;
			cmdTempo.command = MmlCommand<CalcType>::ECommand::Tempo;
			cmdTempo.bpm = getNum();
			commands.push_back(cmdTempo);

		}
		else if (c == '@')
		{
			MmlCommand<CalcType> cmdType;
			cmdType.command = MmlCommand<CalcType>::ECommand::ToneType;
			cmdType.waveCurve = getNum();
			commands.push_back(cmdType);
		}


	}

	return commands;
}

template<typename CalcT>
inline void WavGenerator<CalcT>::setTone(int no, const ToneData& tone)
{
	tones_[no] = tone;
}

template<typename CalcT>
inline bool WavGenerator<CalcT>::ready(uint32_t sampleRate)
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

template<typename CalcT>
inline void  WavGenerator<CalcT>::addCommand(const MmlCommand<CalcType>& command)
{
	commands_.push_back(command);
}
template<typename CalcT>
inline void WavGenerator<CalcT>::addCommand(std::vector<MmlCommand<CalcType>> commands)
{
	commands_.insert(commands_.end(), commands.begin(), commands.end());
}

template<typename CalcT>
inline std::vector<int16_t> WavGenerator<CalcT>::generate(int samples, bool loop)
{
	std::vector<int16_t> result;
	const ToneData* tone = nullptr; //nullの場合そのコマンドの情報を集め直すのだろう

	auto Abs = [](auto v)
	{
		return v < 0 ? -v : v;
	};

	while (samples > 0)
	{
	REPROC1:
		if (status_.commandIdx >= commands_.size())
		{
			if (!loop)
				return result;
			status_.commandIdx = 0;
		}
		const auto& cmd = commands_[status_.commandIdx];
		if (cmd.command != MmlCommand<CalcType>::ECommand::ToneNote)
		{
			switch (cmd.command)
			{
			case MmlCommand<CalcType>::ECommand::Volume:
				break;
			default:
				break;
			}
			status_.commandIdx++;
			continue;
		}
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
			lengthInfo.length += cmd.note.length;

			//sampleRate_ * 60 = 1分間のサンプル数
			//bpm=1分間の四分音符数
			//length=分解能単位の長さ
			//bpm * 4 / length 
			lengthInfo.samples = sampleRate_ * 60 * 4* lengthInfo.length / NoteLengthResolutio / lengthInfo.bpm;
		
			for (auto& li : status_.recentLength)
				samples += li.samples;

			status_.toneIndex = cmd.waveCurve;
			status_.dutyIndex = 0;
			status_.noteSamples = int(samples- oldSamples);
			status_.noteProcedSamples = 0;
			status_.waveStep = 0;
			status_.toneOff = cmd.note.toneFreq == 0;
			status_.baseFreq = CalcType(sampleRate_) / cmd.note.toneFreq;
			status_.waveFreqDiv2 = status_.baseFreq/2;
			status_.waveDiv2InSample = 0;

			status_.slurTo = 0;
			if (cmd.note.Slur != 0 && commands_.size() > status_.commandIdx + 1)
			{
				auto& nextCmd = commands_[status_.commandIdx + 1];
				status_.slurTo = CalcType(sampleRate_) / nextCmd.note.toneFreq;
			}
			
		}
		tone = &tones_[status_.toneIndex];
		CalcType toneSwitchSample = tone->cycle == 0 ? CalcType(0) : CalcType(sampleRate_) * tone->cycle / 1000;

		int16_t sample = 0;
	REPROC2:
		if (!status_.toneOff)
		{
			auto restSample = status_.noteSamples - status_.noteProcedSamples;
			//半周期が終わった
			if (CalcType(status_.waveDiv2InSample) > status_.waveFreqDiv2)
			{


				//マイナス側が終了した
				if (status_.waveStep == 1)
				{
					if (restSample < (int)status_.baseFreq && cmd.note.Slur==0)
					{
						//もう収めることができない
						status_.toneOff = true;
						goto REPROC2;
					}
					else
					{
						//毎回切替時はDuty比変更
						if (toneSwitchSample == 0)
						{
							++status_.dutyIndex;
						}
					}
				}

				//プラマイ切り替え
				status_.waveStep = (status_.waveStep + 1) % 2;
				//半周期開始
				status_.waveDiv2InSample = 0;
				//ランダム周波数時は次の半分の周波数を決定

				if (tone->randomRange != 0)
				{
					auto freq = (rand() % abs(tone->randomRange * 2)) - tone->randomRange;
					status_.waveFreqDiv2 = status_.baseFreq  + CalcType(freq) / 2;
				}
				//通常時はbaseとdutyから決定
				else
				{
					auto duty = tone->dutyRatio[status_.dutyIndex % tone->dutyRatio.size()];
					duty = status_.waveStep == 0 ? duty : 100 - duty;
					status_.waveFreqDiv2 = status_.baseFreq * duty / 100;

				}

			}
			//時間によるDuty比切り替え
			if (toneSwitchSample!=0 && toneSwitchSample * (status_.dutyIndex + 1) < status_.noteProcedSamples)
			{
				++status_.dutyIndex;
			}



			switch (tone->curve)
			{
			case EWaveCurveType::Rectangle:
			{
				if (status_.waveStep==0)
					sample = cmd.vol * SHORT_MAX / volumeMax_;
				else
					sample = -cmd.vol * SHORT_MAX / volumeMax_;
				break;
			}
			case EWaveCurveType::Triangle:
			{
				auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
				inRatio = CalcType(1) - Abs(inRatio - CalcType(0.5)) * 2;
				sample = (int)(inRatio * (cmd.vol * SHORT_MAX / volumeMax_));
				sample = status_.waveStep == 0 ? sample : -sample;
				break;
			}
			case EWaveCurveType::Saw:
			{
				auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
				if (status_.waveStep == 0)
					inRatio -= CalcType(1.0);
				sample = (int)(inRatio * (cmd.vol * SHORT_MAX / volumeMax_));
				break;
			}
			case EWaveCurveType::Sin:
			{
				auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
				inRatio *= M_PI;
				inRatio += status_.waveStep == 0 ? 0 : M_PI;
				sample = (int)(sinf(inRatio) * (cmd.vol * SHORT_MAX / volumeMax_));
				break;
			}
			case EWaveCurveType::Noise:
			{
				auto noise = (CalcType(rand()) / RAND_MAX - RAND_MAX / 2);
				sample = (int)(noise * (cmd.vol * SHORT_MAX / volumeMax_));
				break;
			}
			}
		}
		result.push_back(sample);
		--samples;

		status_.waveDiv2InSample++;
		status_.noteProcedSamples++;
		if (status_.noteProcedSamples == status_.noteSamples)
			status_.commandIdx++;

	}


	return result;
}