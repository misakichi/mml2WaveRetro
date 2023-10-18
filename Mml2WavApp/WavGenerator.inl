
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
inline bool WavGenerator<CalcT>::compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint, ErrorReson* reason)
{
	static const CalcType toneScaleFreq[] =
	{
		CalcType(32.703/2), //C  ド
		CalcType(34.648/2), //C#
		CalcType(36.708/2), //D
		CalcType(38.891/2), //D#
		CalcType(41.203/2), //E
		CalcType(43.654/2), //F
		CalcType(46.249/2), //F#
		CalcType(48.999/2), //G
		CalcType(51.913/2), //G#
		CalcType(27.500), //A  ラ
		CalcType(29.135), //A#
		CalcType(30.868), //B
		CalcType(32.703), //C  ド
		CalcType(34.648), //C#
		CalcType(36.708), //D
		CalcType(38.891), //D#
		CalcType(41.203), //E
		CalcType(43.654), //F
		CalcType(46.249), //F#
		CalcType(48.999), //G
		CalcType(51.913), //G#
		CalcType(55.000), //A  ラ
		CalcType(58.270), //A#
		CalcType(61.735), //B
		CalcType(65.406), //C  ド
		CalcType(69.296), //C#
		CalcType(73.416), //D
		CalcType(77.782), //D#
		CalcType(82.407), //E
		CalcType(87.307), //F
		CalcType(92.499), //F#
		CalcType(97.999), //G
		CalcType(103.826), //G#,
		CalcType(110.000), //A  ラ
		CalcType(116.541), //A#
		CalcType(123.471), //B
		CalcType(130.813), //C  ド
		CalcType(138.591), //C#
		CalcType(146.832), //D
		CalcType(155.563), //D#
		CalcType(164.814), //E
		CalcType(174.614), //F
		CalcType(184.997), //F#
		CalcType(195.998), //G
		CalcType(207.652), //G#
		CalcType(220.000), //A  ラ
		CalcType(233.082), //A#
		CalcType(246.942), //B
		CalcType(261.626), //C  ド
		CalcType(277.183), //C#
		CalcType(293.665), //D
		CalcType(311.127), //D#
		CalcType(329.628), //E
		CalcType(349.228), //F
		CalcType(369.994), //F#
		CalcType(391.995), //G
		CalcType(415.305), //G#
		CalcType(440.000), //A  ラ
		CalcType(466.164), //A#
		CalcType(493.883), //B
		CalcType(523.251), //C  ド
		CalcType(554.365), //C#
		CalcType(587.330), //D
		CalcType(622.254), //D#
		CalcType(659.255), //E
		CalcType(698.456), //F
		CalcType(739.989), //F#
		CalcType(783.991), //G
		CalcType(830.609), //G#
		CalcType(880.000),  //A  ラ
		CalcType(932.328),  //A#
		CalcType(987.767),  //B
		CalcType(1046.502), //C  ド
		CalcType(1108.731), //C#
		CalcType(1174.659), //D
		CalcType(1244.508), //D#
		CalcType(1318.510), //E
		CalcType(1396.913), //F
		CalcType(1479.978), //F#
		CalcType(1567.982), //G
		CalcType(1661.219), //G#
		CalcType(1760.000), //A  ラ
		CalcType(1864.655), //A#
		CalcType(1975.533), //B
		CalcType(2093.005), //C  ド
		CalcType(2217.461), //C#
		CalcType(2349.318), //D
		CalcType(2489.016), //D#
		CalcType(2637.020), //E
		CalcType(2793.826), //F
		CalcType(2959.955), //F#
		CalcType(3135.963), //G
		CalcType(3322.438), //G#
		CalcType(3520.000), //A
		CalcType(3729.310), //A#
		CalcType(3951.066), //B
	};

	std::vector<TypedCommand> commands;
	auto end = mml + strlen(mml);
	auto p = mml;
	
	struct State {
		int oct = 4;
		int len = 4;
	} state;

	TypedCommand defaultVol;
	defaultVol.command = TypedCommand::ECommand::Volume;
	defaultVol.vol = volumeMax_ / 2;
	commands.push_back(defaultVol);

	TypedCommand defaultTempo;
	defaultTempo.command = TypedCommand::ECommand::Tempo;
	defaultTempo.bpm = 120;;
	commands.push_back(defaultTempo);

	TypedCommand defaultToneType;
	defaultToneType.command = TypedCommand::ECommand::ToneType;
	defaultToneType.waveCurve = 0;
	commands.push_back(defaultToneType);

	TypedCommand slurCrossCmd;
	slurCrossCmd.command = TypedCommand::ECommand::SlurCrossPoint;
	slurCrossCmd.slurCrossPoint = CalcType(0.25);
	commands.push_back(slurCrossCmd);

	TypedCommand defaultADSR;
	defaultADSR.command = TypedCommand::ECommand::Envelope;
	defaultADSR.envelope.atackLevel = volumeMax_;
	defaultADSR.envelope.sustainLevel = int(volumeMax_ * 0.8);
	defaultADSR.envelope.atackTime = 20;
	defaultADSR.envelope.decayTime = 20;
	defaultADSR.envelope.releaseTime = 20;
	commands.push_back(defaultADSR);

	auto getNumF = [&p]() -> CalcType
	{
		bool isFloatPoint = false;
		char num[32] = {};
		auto dst = num;
		while (*p >= '0' && *p <= '9' || (*p=='.' && !isFloatPoint))
		{
			isFloatPoint |= *p == '.';
			*dst++ = *p++;
		}
		return isFloatPoint ? CalcType(atof(num)) : CalcType(atoi(num));
	};
	auto getNumI = [&p]() -> int
	{
		char num[32] = {};
		auto dst = num;
		while (*p >= '0' && *p <= '9')
		{
			*dst++ = *p++;
		}
		return atoi(num);
	};
	auto genError = [reason, errorPoint, &p, mml](ErrorReson _reason)
	{
		if (reason)
			*reason = _reason;
		if (errorPoint)
			*errorPoint = p - mml;
		return false;
	};

	int slur = 0;

	while (p < end)
	{
		auto c = *p++;
		if ((c >= 'A' && c <= 'G') || c=='X' || c == 'R')
		{
			TypedCommand cmd;
			cmd.command = TypedCommand::ECommand::ToneNote;

			if (c == 'X')
			{
				cmd.note.toneFreq = getNumF();
			}
			else if (c == 'R')
			{
				cmd.note.toneFreq = 0;
			}
			else
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
				}

				if (slur != 0 && offset < 0)
				{
					//スラーの次が音符じゃない
					return genError(ErrorReson::IllegalCommandNextSlur);
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
				if (offset < 0)
					return genError(ErrorReson::ToneLevelOutOfRangeLower);
				if (offset >= sizeof(toneScaleFreq)/sizeof(toneScaleFreq[0]))
					return genError(ErrorReson::ToneLevelOutOfRangeUpper);

				cmd.note.toneFreq = toneScaleFreq[offset];
			}

			int len = getNumI();
			if (len == 0)
				len = state.len;

			len = NoteLengthResolutio / len;

			int addPeriod = 0;
			if (*p == '.')
			{
				p++;
				len += len / 2;
			}

			slur = 0;
			if (*p == '&')
			{
				p++;
				slur = 1;
			}

			cmd.note.length = len;
			cmd.note.Slur = slur;
			commands.push_back(cmd);
		}
		else if (c == 'O')
		{
			state.oct = getNumI();
		}
		else if (c == '<')
		{
			state.oct--;
		}
		else if (c == '>')
		{
			state.oct++;
		}
		else if (slur != 0)
		{
			//スラーの次が音程系、音符系じゃない
			return genError(ErrorReson::IllegalCommandNextSlur);
		}
		else if (c == 'L')
		{
			state.len = getNumI();
		}
		else if (c == 'V')
		{
			TypedCommand cmdVol;
			cmdVol.command = TypedCommand::ECommand::Volume;
			cmdVol.vol = getNumI();
			commands.push_back(cmdVol);

		}
		else if (c == 'T')
		{
			TypedCommand cmdTempo;
			cmdTempo.command = TypedCommand::ECommand::Tempo;
			cmdTempo.bpm = getNumI();
			commands.push_back(cmdTempo);

		}
		else if (c == '@')
		{
			if (*p >= '0' && *p <= '9')
			{ //音色
				TypedCommand cmdType;
				cmdType.command = TypedCommand::ECommand::ToneType;
				cmdType.waveCurve = getNumI();
				commands.push_back(cmdType);
			}
			else if (*p == 'S')
			{
				++p;
				TypedCommand cmdTempo;
				cmdTempo.command = TypedCommand::ECommand::SlurCrossPoint;
				auto base = getNumF();
				if (base < 1)
					return genError(ErrorReson::IllegalValue);

				cmdTempo.slurCrossPoint = CalcType(1) / base;
				commands.push_back(cmdTempo);
			}
			else if (*p == 'E')
			{	//エンベロープ
				++p;
				TypedCommand cmdType;
				cmdType.command = TypedCommand::ECommand::Envelope;
				cmdType.envelope.atackTime = getNumI();
				if (*p++ != ':')
					return genError(ErrorReson::IllegalFormatEnvelopeCommand);
				cmdType.envelope.decayTime = getNumI();
				if (*p++ != ':')
					return genError(ErrorReson::IllegalFormatEnvelopeCommand);
				cmdType.envelope.releaseTime = getNumI();
				if (*p++ != ':')
					return genError(ErrorReson::IllegalFormatEnvelopeCommand);
				cmdType.envelope.atackLevel = getNumI();
				if(cmdType.envelope.atackLevel>255)
					return genError(ErrorReson::IllegalParameterRange);
				if (*p++ != ':')
					return genError(ErrorReson::IllegalFormatEnvelopeCommand);
				cmdType.envelope.sustainLevel = getNumI();
				if (cmdType.envelope.sustainLevel > 255)
					return genError(ErrorReson::IllegalParameterRange);
				commands.push_back(cmdType);
			}
		}
		else
		{
			//unknown
			return genError(ErrorReson::UnknownCommand);
		}

	}

	dest.swap(commands);
	return true;
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
inline void  WavGenerator<CalcT>::addCommand(const TypedCommand& command)
{
	commands_.push_back(command);
}
template<typename CalcT>
inline void WavGenerator<CalcT>::addCommand(std::vector<TypedCommand> commands)
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
	auto Lerp=[](CalcType a, CalcType b, CalcType ratio) ->CalcType
	{
		return a * (CalcType(1) - ratio) + b * ratio;
	};
	auto msecToSamples = [sampleRate = sampleRate_](CalcType msec)
	{
		auto sample = msec* sampleRate / 1000;
		return sample;
	};

	while (samples > 0)
	{
	//REPROC1:
		if (status_.commandIdx >= commands_.size())
		{
			if (!loop)
				return result;
			status_.commandIdx = 0;
		}
		const auto& cmd = commands_[status_.commandIdx];
		if (cmd.command != TypedCommand::ECommand::ToneNote)
		{
			switch (cmd.command)
			{
			case TypedCommand::ECommand::Volume:
				status_.volume = CalcType(cmd.vol) / volumeMax_;
				break;
			case TypedCommand::ECommand::Tempo:
				status_.bpm = cmd.bpm;
				break;
			case TypedCommand::ECommand::ToneType:
				status_.toneIndex = cmd.waveCurve;
				break;
			case TypedCommand::ECommand::SlurCrossPoint:
				status_.slurCrossPoint = cmd.slurCrossPoint;
				break;
			case TypedCommand::ECommand::Envelope:
				status_.envelopeAtackSamples = msecToSamples(CalcType(cmd.envelope.atackTime));
				status_.envelopeDecaySamples = msecToSamples(CalcType(cmd.envelope.decayTime));
				status_.envelopeReleaseSamples = msecToSamples(CalcType(cmd.envelope.releaseTime));
				status_.envelopeAtackLevel = CalcType(cmd.envelope.atackLevel) / volumeMax_;
				status_.envelopeSustainLevel = CalcType(cmd.envelope.sustainLevel) / volumeMax_;
				break;
			default:
				break;
			}
			status_.commandIdx++;
			continue;
		}

		auto div2Set = [this, Lerp](const ToneData* tone, const CalcType& baseFreqSamples)
		{
			//ランダム周波数時は次の半分の周波数を決定
			if (tone->randomRange != 0)
			{
				auto baseFreq = CalcType(sampleRate_ )/ baseFreqSamples;
				auto ratio = (CalcType(rand()) / RAND_MAX);//% abs(tone->randomRange)) - tone->randomRange / 2;
				auto rate = CalcType(Lerp(tone->randomRange, 100, ratio)) / 100;
				auto freq = baseFreq * rate;
				status_.waveFreqDiv2 = CalcType(sampleRate_) / (freq/2);

			}
			//通常時はbaseとdutyから決定
			else
			{
				auto duty = tone->dutyRatio[status_.dutyIndex % tone->dutyRatio.size()];
				duty = status_.waveStep == 0 ? duty : 100 - duty;
				status_.waveFreqDiv2 = baseFreqSamples * duty / 100;

			}
		};

		if (status_.noteProcedSamples == status_.noteSamples)
		{
			//新規コマンドです
			uint64_t oldSamples = 0;
			uint64_t samples = 0;
			for (auto& li : status_.recentLength)
				oldSamples += li.samples;

			if (status_.recentLength.empty() || status_.recentLength.back().bpm != status_.bpm)
			{
				status_.recentLength.push_back({ status_.bpm,0 });
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

			status_.dutyIndex = 0;
			status_.noteSamples = int(samples- oldSamples);
			status_.noteProcedSamples = 0;
			status_.toneOff = cmd.note.toneFreq == 0;
			status_.baseFreqSamples = status_.toneOff ? CalcType(0) : CalcType(sampleRate_) / cmd.note.toneFreq;
			status_.waveFreqDiv2 = status_.baseFreqSamples/2;
			//以前がスラーなら進行位置維持する
			if (status_.slurTo == 0)
			{
				status_.waveStep = 0;
				status_.waveDiv2InSample = 0;
			}
			else
			{
			}

			status_.slurTo = 0;
			//次がスラー
			if (cmd.note.Slur != 0 && commands_.size() > status_.commandIdx + 1)
			{
				auto& nextCmd = commands_[status_.commandIdx + 1];
				status_.slurTo = CalcType(sampleRate_) / nextCmd.note.toneFreq;
			}

			div2Set(&tones_[status_.toneIndex], status_.baseFreqSamples);
		}
		tone = &tones_[status_.toneIndex];
		CalcType toneSwitchSample = tone->cycle == 0 ? CalcType(0) : CalcType(sampleRate_) * tone->cycle / 1000;

		int16_t sample = 0;
	REPROC2:
		if (!status_.toneOff)
		{
			auto restSample = CalcType(status_.noteSamples - status_.noteProcedSamples);

			//半周期が終わった
			if (CalcType(status_.waveDiv2InSample) > status_.waveFreqDiv2)
			{
				//スラーで変動するので変数に入れる
				auto baseFreqSamples = status_.baseFreqSamples;
				if (status_.slurTo != 0)
				{
					auto crossSamples = (status_.slurCrossPoint * status_.noteSamples);
					if (crossSamples >= restSample)
					{
						auto ratio = restSample / crossSamples;
						baseFreqSamples = Lerp(status_.slurTo, status_.baseFreqSamples, CalcType(1)-ratio);
					}
				}


				//マイナス側が終了した
				if (status_.waveStep == 1)
				{
					if (restSample < baseFreqSamples && cmd.note.Slur==0)
					{
						//スラーOFF時もう収めることができないなら発音終了
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

				div2Set(tone, baseFreqSamples);
			}

			//時間によるDuty比切り替え
			if (toneSwitchSample!=0 && toneSwitchSample * (status_.dutyIndex + 1) < status_.noteProcedSamples)
			{
				++status_.dutyIndex;
			}


			//波形（-1～1)生成
			CalcType level(0);
			switch (tone->curve)
			{
			case EWaveCurveType::Rectangle:
				{
					if (status_.waveStep == 0)
						level = CalcType(1);
					else
						level = CalcType(-1);
					break;
				}
			case EWaveCurveType::Triangle:
				{
					auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
					inRatio = CalcType(1) - Abs(inRatio - CalcType(0.5)) * 2;
					level = status_.waveStep == 0 ? inRatio : -inRatio;
					break;
				}
			case EWaveCurveType::Saw:
				{
					auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
					if (status_.waveStep == 0)
						inRatio -= CalcType(1.0);
					level = inRatio;
					break;
				}
			case EWaveCurveType::Sin:
				{
					auto inRatio = CalcType(status_.waveDiv2InSample) / status_.waveFreqDiv2;
					inRatio *= CalcType(M_PI);
					inRatio += status_.waveStep == 0 ? CalcType(0) : CalcType(M_PI);
					level = CalcType(sinf((float)inRatio));
					break;
				}
			case EWaveCurveType::Noise:
				{
					auto noise = (CalcType(rand()) / RAND_MAX - CalcType(0.5)) * 2;
					level = noise;
					break;
				}
			}
			//ボリューム値ADSRによる音量調整
			auto fNowSample = CalcType(status_.noteProcedSamples);
			CalcType adsr;
			if (fNowSample < status_.envelopeAtackSamples)
			{
				//A
				adsr = fNowSample / status_.envelopeAtackSamples * status_.envelopeAtackLevel;
			}
			else if (fNowSample < status_.envelopeAtackSamples + status_.envelopeDecaySamples)
			{
				//D
				auto ratio = (fNowSample - status_.envelopeAtackSamples) / status_.envelopeDecaySamples;
				adsr = Lerp(status_.envelopeAtackLevel, status_.envelopeSustainLevel, ratio);
			}
			else if (restSample < status_.envelopeReleaseSamples)
			{
				//R
				adsr = restSample / status_.envelopeReleaseSamples * status_.envelopeSustainLevel;
			}
			else
			{
				//S
				adsr = status_.envelopeSustainLevel;
			}

			sample = (std::max)(SHORT_MIN, (std::min)(SHORT_MAX, (int)(level * adsr * status_.volume * SHORT_MAX)));
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