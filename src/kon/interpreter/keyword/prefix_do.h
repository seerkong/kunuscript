
#include "../../prefix.h"
#include "../types.h"

// like scheme (begin expr1 expr2 ...)
KonTrampoline* KN_EvalPrefixDo(KonState* kstate, KN expression, KonEnv* env, KonContinuation* cont);
