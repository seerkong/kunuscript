#include "env.h"
#include "../kson/hashmap.h"

Kon* KON_MakeRootEnv(Kon* kstate)
{
    Kon* env = KON_AllocTagged(kstate, sizeof(KonEnv), KON_ENV);
    env->Value.Env.Parent = KON_NULL;
    env->Value.Env.Bindings = KON_HashMapInit(20);

    // math
    KON_EnvDefine(kstate, env, "+",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryPlus)
    );
    KON_EnvDefine(kstate, env, "-",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMinus)
    );
    KON_EnvDefine(kstate, env, "*",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMultiply)
    );
    KON_EnvDefine(kstate, env, "/",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDivide)
    );
    KON_EnvDefine(kstate, env, "mod",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryMod)
    );
    KON_EnvDefine(kstate, env, "lt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerThan)
    );
    KON_EnvDefine(kstate, env, "lte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryLowerOrEqual)
    );
    KON_EnvDefine(kstate, env, "gt",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterThan)
    );
    KON_EnvDefine(kstate, env, "gte",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryGreaterOrEqual)
    );
    

    // IO
    KON_EnvDefine(kstate, env, "newline",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryNewline)
    );
    KON_EnvDefine(kstate, env, "display",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplay)
    );
    KON_EnvDefine(kstate, env, "displayln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryDisplayln)
    );
    KON_EnvDefine(kstate, env, "write",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWrite)
    );
    KON_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryWriteln)
    );

    KON_EnvDefine(kstate, env, "stringify",
        MakeNativeProcedure(kstate, KON_NATIVE_FUNC, KON_PrimaryStringify)
    );

    return env;
}

Kon* KON_MakeChildEnv(Kon* kstate, Kon* parentEnv)
{
    Kon* env = KON_AllocTagged(kstate, sizeof(KonEnv), KON_ENV);
    env->Value.Env.Parent = parentEnv;
    env->Value.Env.Bindings =  KON_HashMapInit(20);
    return env;
}

Kon* KON_EnvDefine(Kon* kstate, Kon* env, char* key, Kon* value)
{
    KON_HashMapPut(env->Value.Env.Bindings, key, value);
    return KON_TRUE;
}

Kon* KON_EnvLookup(Kon* kstate, Kon* env, char* key)
{
    Kon* value = (Kon*)KON_HashMapGet(env->Value.Env.Bindings, key);
    if (value) {
        return value;
    }
    else if (env->Value.Env.Parent == KON_NULL) {
        return KON_NULL;
    }
    else {
        return KON_EnvLookup(kstate, env->Value.Env.Parent, key);
    }
}
