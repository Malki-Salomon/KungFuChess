#include "pch.h"
#include "EventBus.h"

// Intentionally empty: subscribe()/publish() are templates and must be
// defined in EventBus.h to be visible at each call site, so there is no
// non-template implementation left to put here. This translation unit
// exists so EventBus.cpp can still be registered as a project file per the
// header/source pairing convention used elsewhere in Core.
