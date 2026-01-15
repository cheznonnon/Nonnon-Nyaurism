// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html




#define N_GITHUB_REPO


#ifdef N_GITHUB_REPO


#define N_WAV_FORMAT_FLOAT_ON

#include "../nonnon/neutral/wav.c"
#include "../nonnon/neutral/wav/all.c"
#include "../nonnon/neutral/fft.c"


#include "../nonnon/bridge/gdi.c"


#include "../nonnon/mac/window.c"
#include "../nonnon/mac/image.c"
#include "../nonnon/mac/sound.c"

#include "../nonnon/mac/n_button.c"


#include "../nonnon/neutral/filer.c"


#else  // #ifdef N_GITHUB_REPO


#define N_WAV_FORMAT_FLOAT_ON

#include "../../nonnon/neutral/wav.c"
#include "../../nonnon/neutral/wav/all.c"
#include "../../nonnon/neutral/fft.c"


#include "../../nonnon/bridge/gdi.c"


#include "../../nonnon/mac/window.c"
#include "../../nonnon/mac/image.c"
#include "../../nonnon/mac/sound.c"

#include "../../nonnon/mac/n_button.c"


#include "../../nonnon/neutral/filer.c"


#endif // #ifdef N_GITHUB_REPO

