#ifndef SREADWRITE_EXTERNAL_H
#define SREADWRITE_EXTERNAL_H

#ifndef NOMINMAX
#define NOMINMAX
#define MUSTUNDEFNOMINMAX
#endif

#include "dep/nc.h"
#include "dep/sreadwrite.h"
#include "dep/ppfile.h"

#ifdef MUSTUNDEFNOMINMAX
#undef NOMINMAX
#endif

#endif