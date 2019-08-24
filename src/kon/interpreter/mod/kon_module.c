#include "kon_number.h"
#include "kon_string.h"
#include "kon_list.h"
#include "kon_vector.h"
#include "kon_table.h"
#include "kon_cell.h"
#include "kon_accessor.h"

#include "kon_module.h"

KN KonModule_Export(KonState* kstate, KonEnv* env)
{
    KonAccessor* module = (KonAccessor*)KN_MakeDirAccessor(kstate, "drw", NULL);
    
    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "number",
        (KN)KonNumber_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "string",
        (KN)KonString_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "list",
        (KN)KonList_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "vector",
        (KN)KonVector_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "table",
        (KN)KonTable_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "cell",
        (KN)KonCell_Export(kstate, env)
    );

    KN_DirAccessorPutKeyProperty(
        kstate,
        (KN)module,
        "accessor",
        (KN)KonAccessor_Export(kstate, env)
    );

    return (KN)module;
}
