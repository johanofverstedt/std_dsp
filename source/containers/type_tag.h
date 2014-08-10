
#ifndef STD_DSP_TYPE_TAG
#define STD_DSP_TYPE_TAG

namespace std_dsp {
	struct split_channels_tag {};
	struct interleaved_channels_tag {};

	enum channels_mode {
		channels_mode_split,
		channels_mode_interleaved,
	};
}

#endif
