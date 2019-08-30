#include "primary.h"


KN KonList_Cons(KonState* kstate, KN self, KN other)
{
    return KN_CONS(kstate, self, other);
}

KN KonList_Car(KonState* kstate, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CAR(self);
}

KN KonList_Cdr(KonState* kstate, KN self)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(self)) {
        self = KN_UNBOX_QUOTE(self);
    }
    return KN_CDR(self);
}

KN KonList_Caar(KonState* kstate, KN self)
{
    return KN_CAAR(self);
}

KN KonList_Cadr(KonState* kstate, KN self)
{
    return KN_CADR(self);
}

KN KonList_Cdar(KonState* kstate, KN self)
{
    return KN_CDAR(self);
}

KN KonList_Cddr(KonState* kstate, KN self)
{
    return KN_CDDR(self);
}

KN KonList_Caaar(KonState* kstate, KN self)
{
    return KN_CAAAR(self);
}

KN KonList_Caadr(KonState* kstate, KN self)
{
    return KN_CAADR(self);
}

KN KonList_Cadar(KonState* kstate, KN self)
{
    return KN_CADAR(self);
}

KN KonList_Caddr(KonState* kstate, KN self)
{
    return KN_CADDR(self);
}

KN KonList_Cdaar(KonState* kstate, KN self)
{
    return KN_CDAAR(self);
}

KN KonList_Cdadr(KonState* kstate, KN self)
{
    return KN_CDADR(self);
}

KN KonList_Cddar(KonState* kstate, KN self)
{
    return KN_CDDAR(self);
}

KN KonList_Cdddr(KonState* kstate, KN self)
{
    return KN_CDDDR(self);
}

KN KonList_Cadddr(KonState* kstate, KN self)
{
    return KN_CADDDR(self);
}

KN KonList_Cddddr(KonState* kstate, KN self)
{
    return KN_CDDDDR(self);
}


// not(kon_false) => true, not(other) => false
KN KN_PrimaryNot(KonState* kstate, KN item)
{
    if (KN_IS_FALSE(item)) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// pointer check
KN KN_PrimaryEqual(KonState* kstate, KN left, KN right)
{
    if (left == right) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

// basic types, boolean, number, char symbol, string value check
KN KN_PrimaryEqv(KonState* kstate, KN left, KN right)
{
    if (left == KN_UNDEF || left == KN_UKN) {
        return (left == right) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_NIL(left) || KN_IS_NIL(right)) {
        return (KN_IS_NIL(left) && KN_IS_NIL(right)) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_BOOLEAN(left) && KN_IS_BOOLEAN(right)) {
        return (left == right) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_FIXNUM(left) && KN_IS_FIXNUM(right)) {
        return (left == right) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_FLONUM(left) && KN_IS_FLONUM(right)) {
        return (KN_UNBOX_FLONUM(left) == KN_UNBOX_FLONUM(right)) ? KN_TRUE: KN_FALSE;
    }
    else if (KN_IS_CHAR(left) && KN_IS_CHAR(right)) {
        return (KN_UNBOX_CHAR(left) == KN_UNBOX_CHAR(right)) ? KN_TRUE: KN_FALSE;
    }
    // TODO KN_IS_BYTE
    else if (KN_IS_SYMBOL(left) && KN_IS_SYMBOL(right)) {
        const char* leftStr = KN_UNBOX_SYMBOL(left);
        const char* rightStr = KN_UNBOX_SYMBOL(right);
        return (strcmp(leftStr, rightStr) == 0) ? KN_TRUE: KN_FALSE;;
    }
    
    else if (KN_IS_STRING(left) && KN_IS_STRING(right)) {
        const char* leftStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(left));
        const char* rightStr = KxStringBuffer_Cstr(KN_UNBOX_STRING(right));
        return (strcmp(leftStr, rightStr) == 0) ? KN_TRUE: KN_FALSE;;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryEq(KonState* kstate, KN left, KN right)
{
    if (left == KN_UNDEF || left == KN_UKN
        || KN_IS_NIL(left)
        || KN_IS_BOOLEAN(left)
        || KN_IS_FIXNUM(left)
        || KN_IS_FLONUM(left)
        || KN_IS_CHAR(left)
        || KN_IS_SYMBOL(left)
        || KN_IS_STRING(left)
    ) {
        return KN_PrimaryEqv(kstate, left, right);
    }
    else {
        return KN_PrimaryEqual(kstate, left, right);
    }
}

KN KN_PrimaryNeq(KonState* kstate, KN left, KN right)
{
    KN result = KN_PrimaryEq(kstate, left, right);
    if (result == KN_FALSE) {
        return KN_TRUE;
    }
    else {
        return KN_FALSE;
    }
}

KN KN_PrimaryNewline(KonState* kstate)
{
    printf("\n");
    return KN_MAKE_FIXNUM(1);
}

KN KN_PrimaryDisplay(KonState* kstate, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KN_CAR(iter);

        KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");
        KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));

        iter = KN_CDR(iter);
    } while (iter != KN_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

KN KN_PrimaryDisplayln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryDisplay(kstate, args);
    KN_PrimaryNewline(kstate);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

// TODO select output
KN KN_PrimaryWrite(KonState* kstate, KN args)
{
    KN iter = args;
    KxStringBuffer* merged = KxStringBuffer_New();

    int state = 1; // 1 need verb, 2 need objects
    do {
        KN item = KN_CAR(iter);

        if (KN_IS_STRING(item)) {
            KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(item));
        }
        else {
            KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");
            KxStringBuffer_AppendStringBuffer(merged, KN_UNBOX_STRING(formated));
        }
        iter = KN_CDR(iter);
    } while (iter != KN_NIL);
    printf("%s", KxStringBuffer_Cstr(merged));
    int size = KxStringBuffer_Length(merged);
    return KN_MAKE_FIXNUM(size);
}

// TODO select output
KN KN_PrimaryWriteln(KonState* kstate, KN args)
{
    KN size = KN_PrimaryWrite(kstate, args);
    KN_PrimaryNewline(kstate);
    return KN_MAKE_FIXNUM(KN_UNBOX_FIXNUM(size) + 1);
}

KN KN_PrimaryStringify(KonState* kstate, KN item)
{
    KN formated = KN_ToFormatString(kstate, item, false, 0, "  ");

    return formated;
}

// TODO
KN KN_PrimaryParse(KonState* kstate, KN item)
{
    // return item;
    return KN_UNDEF;
}

KN KN_PrimaryIsTrue(KonState* kstate, KN item)
{
    return (KN_IS_TRUE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFalse(KonState* kstate, KN item)
{
    return (KN_IS_FALSE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNil(KonState* kstate, KN item)
{
    return (KN_IS_NIL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsNull(KonState* kstate, KN item)
{
    return (KN_IS_UNDEF(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsUkn(KonState* kstate, KN item)
{
    return (KN_IS_UKN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPointer(KonState* kstate, KN item)
{
    return (KN_IS_POINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFixnum(KonState* kstate, KN item)
{
    return (KN_IS_FIXNUM(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsFlonum(KonState* kstate, KN item)
{
    return (KN_IS_FLONUM(item)) ? KN_TRUE : KN_FALSE;
}


KN KN_PrimaryIsImdtSymbol(KonState* kstate, KN item)
{
    return (KN_IS_IMDT_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsChar(KonState* kstate, KN item)
{
    return (KN_IS_CHAR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBoolean(KonState* kstate, KN item)
{
    return (KN_IS_BOOLEAN(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsBytes(KonState* kstate, KN item)
{
    return (KN_IS_BYTES(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsString(KonState* kstate, KN item)
{
    return (KN_IS_STRING(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsSymbol(KonState* kstate, KN item)
{
    return (KN_IS_SYMBOL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVariable(KonState* kstate, KN item)
{
    return (KN_IS_VARIABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsIdentifier(KonState* kstate, KN item)
{
    return (KN_IS_IDENTIFIER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsWord(KonState* kstate, KN item)
{
    return (KN_IS_WORD(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsAttrSlot(KonState* kstate, KN item)
{
    return (KN_IS_ACCESSOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPair(KonState* kstate, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IS_PAIR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsPairList(KonState* kstate, KN item)
{
    // auto unbox QUOTE_LIST
    if (KN_IS_QUOTE_LIST(item)) {
        item = KN_UNBOX_QUOTE(item);
    }
    return (KN_IsPairList(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsVector(KonState* kstate, KN item)
{
    return (KN_IS_VECTOR(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsTable(KonState* kstate, KN item)
{
    return (KN_IS_TABLE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCell(KonState* kstate, KN item)
{
    return (KN_IS_CELL(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsEnv(KonState* kstate, KN item)
{
    return (KN_IS_ENV(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsProcedure(KonState* kstate, KN item)
{
    return (KN_IS_PROCEDURE(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsContinuation(KonState* kstate, KN item)
{
    return (KN_IS_CONTINUATION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsCpointer(KonState* kstate, KN item)
{
    return (KN_IS_CPOINTER(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryIsException(KonState* kstate, KN item)
{
    return (KN_IS_EXCEPTION(item)) ? KN_TRUE : KN_FALSE;
}

KN KN_PrimaryToVariable(KonState* kstate, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_VARIABLE;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryToIdentifier(KonState* kstate, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_IDENTIFIER;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryToSymString(KonState* kstate, KN item)
{
    char* symCstr = NULL;
    if (KN_IS_SYMBOL(item)) {
        symCstr = KN_UNBOX_SYMBOL(item);
    }
    else if (KN_IS_STRING(item)) {
        symCstr = KxStringBuffer_Cstr(KN_UNBOX_STRING(item));
    }
    else {
        return KN_UKN;
    }
    KonSymbol* value = KN_ALLOC_TYPE_TAG(kstate, KonSymbol, KN_T_SYMBOL);
    value->Type = KN_SYM_STRING;
    value->Data = utf8dup(symCstr);
    return value;
}

KN KN_PrimaryUnboxQuote(KonState* kstate, KN obj)
{
    if (KN_IS_QUOTE(obj)) {
        return ((KonQuote*)obj)->Inner;
    }
    else if (KN_IS_QUASIQUOTE(obj)) {
        return ((KonQuasiquote*)obj)->Inner;
    }
    else if (KN_IS_EXPAND(obj)) {
        return ((KonExpand*)obj)->Inner;
    }
    else if (KN_IS_UNQUOTE(obj)) {
        return ((KonUnquote*)obj)->Inner;
    }
    return obj;
}

KN KN_PrimaryGetDispatcherId(KonState* kstate, KN obj)
{
    return KN_MAKE_FIXNUM(KN_NodeDispacherId(kstate, obj));
}

KN KN_PrimarySetDispatcherId(KonState* kstate, KN obj, KN boxedId)
{
    unsigned int dispatcherId = KN_UNBOX_FIXNUM(boxedId);
    if (KN_IS_POINTER(obj)) {
        ((KonBase*)obj)->MsgDispatcherId = dispatcherId;
    }
    return KN_TRUE;
}

// init internal types dispatcher id
KN KN_PrimaryExportDispacherId(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "BooleanDispacher", KN_MAKE_FIXNUM(KN_T_BOOLEAN));
    KN_EnvDefine(kstate, env, "UnknownDispacher", KN_MAKE_FIXNUM(KN_T_UKN));
    KN_EnvDefine(kstate, env, "UndefinedDispacher", KN_MAKE_FIXNUM(KN_T_UNDEF));
    KN_EnvDefine(kstate, env, "SymbolDispacher", KN_MAKE_FIXNUM(KN_T_SYMBOL));
    KN_EnvDefine(kstate, env, "CharDispacher", KN_MAKE_FIXNUM(KN_T_CHAR));
    KN_EnvDefine(kstate, env, "NumberDispacher", KN_MAKE_FIXNUM(KN_T_NUMBER));
    KN_EnvDefine(kstate, env, "StringDispacher", KN_MAKE_FIXNUM(KN_T_STRING));
    KN_EnvDefine(kstate, env, "ListDispacher", KN_MAKE_FIXNUM(KN_T_PAIRLIST));
    KN_EnvDefine(kstate, env, "VectorDispacher", KN_MAKE_FIXNUM(KN_T_VECTOR));
    KN_EnvDefine(kstate, env, "TableDispacher", KN_MAKE_FIXNUM(KN_T_TABLE));
    KN_EnvDefine(kstate, env, "CellDispacher", KN_MAKE_FIXNUM(KN_T_CELL));
    KN_EnvDefine(kstate, env, "AttrSlotDispacher", KN_MAKE_FIXNUM(KN_T_ACCESSOR));

}


KN KN_PrimaryOpExport(KonState* kstate, KonEnv* env)
{
    KN_EnvDefine(kstate, env, "cons",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cons, 2, 0, 0)
    );
    KN_EnvDefine(kstate, env, "car",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Car, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cdr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "caar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cdar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "caaar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caaar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "caadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caadr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cadar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "caddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Caddr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cdaar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdaar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cdadr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdadr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cddar",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cdddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cdddr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cadddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cadddr, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "cddddr",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KonList_Cddddr, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "not",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNot, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "equal",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEqual, 2, 0, 0)
    );
    KN_EnvDefine(kstate, env, "eqv",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEqv, 2, 0, 0)
    );
    KN_EnvDefine(kstate, env, "eq",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryEq, 2, 0, 0)
    );
    KN_EnvDefine(kstate, env, "neq",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNeq, 2, 0, 0)
    );

    // IO
    KN_EnvDefine(kstate, env, "newline",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryNewline, 0, 0, 0)
    );
    KN_EnvDefine(kstate, env, "display",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryDisplay, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "displayln",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryDisplayln, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "write",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryWrite, 0, 1, 0)
    );
    KN_EnvDefine(kstate, env, "writeln",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryWriteln, 0, 1, 0)
    );

    KN_EnvDefine(kstate, env, "stringify",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryStringify, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "kon-from-str",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryParse, 1, 0, 0)
    );

    // predict
    KN_EnvDefine(kstate, env, "is-true",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsTrue, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-false",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFalse, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-nil",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsNil, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsNull, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-null",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsUkn, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-pointer",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPointer, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-fixnum",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFixnum, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-flonum",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsFlonum, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-imdt-symbol",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsImdtSymbol, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-char",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsChar, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-boolean",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsBoolean, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-bytes",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsBytes, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-string",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsString, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-symbol",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsSymbol, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-variable",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsVariable, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-identifier",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsIdentifier, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-word",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsWord, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-attr-slot",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsAttrSlot, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-pair",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPair, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-list",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsPairList, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-vector",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsVector, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-table",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsTable, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-cell",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsCell, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-env",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsEnv, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-procedure",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsProcedure, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-continuation",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsContinuation, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-cpointer",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsCpointer, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "is-exception",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryIsException, 1, 0, 0)
    );
    KN_EnvDefine(kstate, env, "unbox-quote",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryUnboxQuote, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "get-dispatcher-id",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryGetDispatcherId, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "set-dispatcher-id",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimarySetDispatcherId, 2, 0, 0)
    );

    KN_EnvDefine(kstate, env, "to-variable",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToVariable, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "to-identifier",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToIdentifier, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "to-sym-string",
        MakeNativeProcedure(kstate, KN_NATIVE_FUNC, KN_PrimaryToSymString, 1, 0, 0)
    );

    KN_EnvDefine(kstate, env, "get-env", env);

    KN_PrimaryExportDispacherId(kstate, env);
}