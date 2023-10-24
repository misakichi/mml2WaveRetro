
#define _USE_MATH_DEFINES
#include <math.h>

template<typename CalcT>
inline MmlUtility::WavGenerator<CalcT>::WavGenerator()
{
#ifdef USE_CALCED_SIN_TABLE
	for (int i = 0; i < 360 * SinTableResolution; i++)
	{
		sinTable_[i] = CalcType(sin(i * M_PI / 180.0 / 100));
	}
#endif
}

template<typename CalcT>
inline bool MmlUtility::WavGenerator<CalcT>::compileMml(const char* mml, std::vector<TypedCommand>& dest, size_t* errorPoint, ErrorReson* reason)
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

	TypedCommand cmdPan;
	cmdPan.command = TypedCommand::ECommand::Pan;
	cmdPan.pan = 128;
	commands.push_back(cmdPan);


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

	int slurFromCmdIndex = -1;
	while (p < end)
	{
		auto c = *p++;
		if ((c >= 'A' && c <= 'G') || c=='X' || c == 'R' || c == 'N')
		{
			TypedCommand cmd;
			cmd.command = TypedCommand::ECommand::ToneNote;

			if (c == 'X')
			{
				cmd.note.toneFreq = getNumF();
				//長さ変更がある
				if (*p == ':')
					p++;
			}
			else if (c == 'N')
			{
				int  offset= getNumI();
				if (offset < 0)
					return genError(ErrorReson::ToneLevelOutOfRangeLower);
				if (offset >= sizeof(toneScaleFreq) / sizeof(toneScaleFreq[0]))
					return genError(ErrorReson::ToneLevelOutOfRangeUpper);

				cmd.note.toneFreq = toneScaleFreq[offset];

				//長さ変更がある
				if (*p == ':')
					p++;
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
			if (slurFromCmdIndex != -1 && cmd.note.toneFreq==0)
			{
				//スラーの次が音符じゃない
				return genError(ErrorReson::IllegalCommandNextSlur);
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

			if (slurFromCmdIndex != -1)
			{
				commands[slurFromCmdIndex].note.SlurToCmdIdx = (int)commands.size();
			}

			if (*p == '&')
			{
				p++;
				slurFromCmdIndex = (int)commands.size();
			}
			else
			{
				slurFromCmdIndex = -1;
			}

			cmd.note.length = len;
			cmd.note.SlurToCmdIdx = -1;
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
		else if (c == 'L')
		{
			state.len = getNumI();
		}
		else if (slurFromCmdIndex != -1)
		{
			//スラーの次が音程系、音符系じゃない
			return genError(ErrorReson::IllegalCommandNextSlur);
		}
		else if (c == 'P')
		{
			TypedCommand cmdPan;
			cmdPan.command = TypedCommand::ECommand::Pan;
			cmdPan.pan = getNumI();
			if (cmdPan.pan < 0 || cmdPan.pan>256)
				return genError(ErrorReson::PanOutOfRange);
			commands.push_back(cmdPan);
	}
		else if (c == 'V')
		{
			TypedCommand cmdVol;
			cmdVol.command = TypedCommand::ECommand::Volume;
			cmdVol.vol = getNumI();
			if (cmdPan.vol < 0 || cmdPan.vol>255)
				return genError(ErrorReson::VolumeOutOfRange);
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

	if (slurFromCmdIndex != -1)
	{
		return genError(ErrorReson::SlurNotClose);
	}
	dest.swap(commands);
	return true;
}

template<typename CalcT>
inline void MmlUtility::WavGenerator<CalcT>::setTone(int no, const ToneData& tone)
{
	tones_[no] = tone;
}

template<typename CalcT>
inline bool MmlUtility::WavGenerator<CalcT>::ready(uint32_t sampleRate)
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
inline void  MmlUtility::WavGenerator<CalcT>::addCommand(const TypedCommand& command)
{
	commands_.push_back(command);
}
template<typename CalcT>
inline void MmlUtility::WavGenerator<CalcT>::addCommand(std::vector<TypedCommand> commands)
{
	commands_.insert(commands_.end(), commands.begin(), commands.end());
}

template<typename CalcT>
template<int Channels>
inline std::vector<int16_t> MmlUtility::WavGenerator<CalcT>::generate()
{
	static_assert(Channels == 1 || Channels == 2);

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

	while (1)
	{
	//REPROC1:
		if (status_.commandIdx >= commands_.size())
		{
			return result;
		}
		const auto& cmd = commands_[status_.commandIdx];
		if (cmd.command != TypedCommand::ECommand::ToneNote)
		{
			switch (cmd.command)
			{
			case TypedCommand::ECommand::Volume:
				status_.volume = CalcType(cmd.vol) / volumeMax_;
				break;
			case TypedCommand::ECommand::Pan:
				status_.pan = cmd.pan;
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
				auto random = CalcType(100 - tone->randomRange) + (CalcType(rand()) / RAND_MAX) * (tone->randomRange * 2);
				auto freq = baseFreqSamples * random / 200;
				status_.waveFreqDiv2 = freq;

			}
			//通常時はbaseとdutyから決定
			else
			{
				auto duty = tone->dutyRatio[status_.dutyIndex % tone->dutyRatio.size()];
				auto dutyCt = CalcType(status_.waveStep == 0 ? duty : 100 - duty);
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
			status_.isSlurFrom = status_.slurTo != 0;

			//以前がスラーなら進行位置維持する
			if (status_.slurTo == 0)
			{
				status_.waveStep = 0;
				status_.waveDiv2InSample = 0;
				div2Set(&tones_[status_.toneIndex], status_.baseFreqSamples);
			}

			status_.slurTo = 0;
			//次がスラー
			if (cmd.note.SlurToCmdIdx != -1)
			{
				auto& nextCmd = commands_[cmd.note.SlurToCmdIdx];
				status_.slurTo = CalcType(sampleRate_) / nextCmd.note.toneFreq;
			}

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
						baseFreqSamples = Lerp(status_.baseFreqSamples, status_.slurTo, CalcType(1)-ratio);
					}
				}


				//マイナス側が終了した
				if (status_.waveStep == 1)
				{
					if (restSample < baseFreqSamples && cmd.note.SlurToCmdIdx == -1)
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


				div2Set(tone, baseFreqSamples);

				//半周期開始
				status_.waveDiv2InSample = 0;

			}
			else
			{
				//スラーを少しずつでも反映する場合はここを有効にする
				//if (status_.slurTo != 0)
				//{
				//	auto baseFreqSamples = status_.baseFreqSamples;
				//	auto crossSamples = (status_.slurCrossPoint * status_.noteSamples);
				//	if (crossSamples >= restSample)
				//	{
				//		auto ratio = restSample / crossSamples;
				//		baseFreqSamples = Lerp(status_.baseFreqSamples, status_.slurTo, CalcType(1) - ratio);
				//	}
				//	div2Set(tone, baseFreqSamples);
				//}
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
#ifdef USE_CALCED_SIN_TABLE
					inRatio *= CalcType(180);
					inRatio += status_.waveStep == 0 ? CalcType(0) : CalcType(180);
					level = sinTable_[int(inRatio*100)%36000];
#else
					inRatio *= CalcType(M_PI);
					inRatio += status_.waveStep == 0 ? CalcType(0) : CalcType(M_PI);
					level = CalcType(sinf((float)inRatio));
#endif
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
			CalcType adsr = status_.envelopeSustainLevel;
			if (fNowSample < status_.envelopeAtackSamples)
			{
				//A
				if (status_.isSlurFrom == 0)
				{
					adsr = fNowSample / status_.envelopeAtackSamples * status_.envelopeAtackLevel;
				}
			}
			else if (fNowSample < status_.envelopeAtackSamples + status_.envelopeDecaySamples)
			{
				//D
				if (status_.isSlurFrom == 0)
				{
					auto ratio = (fNowSample - status_.envelopeAtackSamples) / status_.envelopeDecaySamples;
					adsr = Lerp(status_.envelopeAtackLevel, status_.envelopeSustainLevel, ratio);
				}
			}
			else if (restSample < status_.envelopeReleaseSamples)
			{
				//R
				if (status_.slurTo == 0)
				{
					adsr = restSample / status_.envelopeReleaseSamples * status_.envelopeSustainLevel;
				}
			}

			sample = (std::max)(SHORT_MIN, (std::min)(SHORT_MAX, (int)(level * adsr * status_.volume * SHORT_MAX)));
		}
		if constexpr (Channels == 1)
		{
			result.push_back(sample);
		}
		else if constexpr (Channels==2)
		{
			
			//L
			result.push_back(sample * (256 - status_.pan) / 256);
			//R
			result.push_back(sample * (status_.pan) / 256);
		}

		status_.waveDiv2InSample++;
		status_.noteProcedSamples++;
		if (status_.noteProcedSamples == status_.noteSamples)
			status_.commandIdx++;

	}


	return result;
}


template<unsigned Channels, typename CalcT, int Banks>
bool MmlUtility::generateMmlToPcm(GenerateMmlToPcmResult& dest, const std::string& prepareSharedMml, const std::array<std::string, Banks>& bankMml, int sampleRate)
{
	static_assert(Banks > 0);
	static_assert(Channels == 1 || Channels == 2);

	std::vector<int16_t> result;
	//基本値
	ToneData defaultTone[10] = {};
	defaultTone[0].curve = EWaveCurveType::Rectangle;
	defaultTone[0].dutyRatio.push_back(50);
	defaultTone[1].curve = EWaveCurveType::Triangle;
	defaultTone[1].dutyRatio.push_back(50);
	defaultTone[2].curve = EWaveCurveType::Saw;
	defaultTone[2].dutyRatio.push_back(50);
	defaultTone[3].curve = EWaveCurveType::Sin;
	defaultTone[3].dutyRatio.push_back(50);
	defaultTone[4].curve = EWaveCurveType::Noise;
	defaultTone[4].dutyRatio.push_back(50);
	defaultTone[5].curve = EWaveCurveType::Rectangle;
	defaultTone[5].dutyRatio.push_back(33.33333333f);
	defaultTone[6].curve = EWaveCurveType::Rectangle;
	defaultTone[6].dutyRatio.push_back(25);
	defaultTone[7].curve = EWaveCurveType::Rectangle;
	defaultTone[7].dutyRatio.push_back(12.5f);
	defaultTone[8].curve = EWaveCurveType::Rectangle;
	defaultTone[8].dutyRatio.push_back(50);
	defaultTone[8].randomRange = 50;
	defaultTone[9].curve = EWaveCurveType::Triangle;
	defaultTone[9].dutyRatio.push_back(50);
	defaultTone[8].randomRange = 50;

	//デフォルトセッティング
	//120bpm
	//四分音符
	//オクターブ4
	//音色0
	//音量128
	//スラークロスポイント1/4
	//エンベロープ Atack:15ms Decay:15ms Release:30ms AtackLevel:255 SustainLevel:200
	//パン　128=センター
	const char* defaultSetting = "T120O4L4V128@0@S4@E15:15:30:255:200P128";

	//処理に時間がかかるならここはマルチスレッド化すると良い
	for (int i = 0; i < Banks; i++)
	{
		WavGenerator<CalcT> generator;
		std::vector<WavGenerator<CalcT>::TypedCommand> commands;
		std::string mml = prepareSharedMml + bankMml[i];
		for (int i = 0; i < 10; i++)
			generator.setTone(i, defaultTone[i]);

		generator.setVolumeMax(255);
		generator.ready(sampleRate);
		if (!generator.compileMml(mml.c_str(), commands, &dest.errPos, &dest.result))
		{
			return false;
		}
		generator.addCommand(commands);
		auto pcm = generator.generate<Channels>();
		if (pcm.size() > result.size())
			result.resize(pcm.size());
		auto dst = result.begin();
		for (auto src : pcm)
			*dst++ += src;
	}

	dest.errPos = 0;
	dest.result = ErrorReson::NoError;
	dest.pcm.swap(result);


}
