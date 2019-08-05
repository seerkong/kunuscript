#ifndef KON_KSON_NODE_H
#define KON_KSON_NODE_H

#include <stdio.h>
#include <tbox/tbox.h>
#include "../prefix/config.h"
#include "../string/kx_stringbuffer.h"
#include "../container/hashtable/kx_hashtable.h"
#include "../container/vector/kx_vector.h"

////
// type alias start

#if KON_64_BIT
typedef unsigned int kon_tag_t;
typedef unsigned long long kon_uint_t;
typedef long long kon_int_t;
#else
typedef unsigned short kon_tag_t;
typedef unsigned int kon_uint_t;
typedef int kon_int_t;
#endif

typedef unsigned char kon_uint8_t;
typedef unsigned int kon_uint32_t;
typedef int kon_int32_t;


// type alias end
////



////
// tagging system start

// tagging system
//   bits end in     1:  fixnum
//                  00:  pointer
//                 010:  string cursor (optional)
//                0110:  immediate symbol (optional)
//            00001110:  immediate flonum (optional)
//            00011110:  char
//            00101110:  reader label (optional)
//            00111110:  unique immediate (NULL, TRUE, FALSE)


#define KON_FIXNUM_BITS 1
#define KON_POINTER_BITS 2
#define KON_STRING_CURSOR_BITS 3
#define KON_IMMEDIATE_BITS 4
#define KON_EXTENDED_BITS 8

#define KON_FIXNUM_MASK ((1<<KON_FIXNUM_BITS)-1)
#define KON_POINTER_MASK ((1<<KON_POINTER_BITS)-1)
#define KON_STRING_CURSOR_MASK ((1<<KON_STRING_CURSOR_BITS)-1)
#define KON_IMMEDIATE_MASK ((1<<KON_IMMEDIATE_BITS)-1)
#define KON_EXTENDED_MASK ((1<<KON_EXTENDED_BITS)-1)

#define KON_POINTER_TAG 0
#define KON_FIXNUM_TAG 1
#define KON_STRING_CURSOR_TAG 2
#define KON_ISYMBOL_TAG 6
#define KON_IFLONUM_TAG 14
#define KON_CHAR_TAG 30
#define KON_READER_LABEL_TAG 46
#define KON_EXTENDED_TAG 62

// 00111110前面的高位字节作为n
// 2: 1000111110
// 比如3： 1100111110
// 8: 100000111110
#define KON_MAKE_IMMEDIATE(n)  ((KN) ((n<<KON_EXTENDED_BITS) \
                                          + KON_EXTENDED_TAG))

// 62 0x3e unknown, container empty placeholder
// like objective c [NSNull null]
#define KON_UKN  KON_MAKE_IMMEDIATE(0)
// 318 0x13e
#define KON_TRUE   KON_MAKE_IMMEDIATE(1)
// 574 0x23e
#define KON_FALSE  KON_MAKE_IMMEDIATE(2)
// 830 0x33e, undefined, no value here a kind of exception
// like javascript undefined
#define KON_NULL   KON_MAKE_IMMEDIATE(3)
// 1086 0x43e ref end of a data structure like list, tree, graph ...
// representes a no value struct/class instance pointer
// KON_IS_LIST(KON_NIL) == KON_TRUE
#define KON_NIL   KON_MAKE_IMMEDIATE(4)
#define KON_EOF    KON_MAKE_IMMEDIATE(5) /* 1342 0x53e */

// tagging system end
////

////
// types start
typedef enum {
    // determined by tagging system
    KON_T_FIXNUM = 1,     //   1
    KON_T_POINTER,        // 000
    KON_T_IMMDT_SYMBOL,   // 0110
    KON_T_CHAR,               // 00011110
    KON_T_UNIQUE_IMMDT,   // 00111110
    KON_T_BOOLEAN,    // a sub type of unique immediate
    KON_T_UKN,
    KON_T_NULL,
    KON_T_NIL,

    // determined by tagging system and ((KonBase*)x)->Tag
    KON_T_NUMBER,
    KON_T_PAIRLIST, // KON_T_NIL AND KON_T_PAIR

    // determined by ((KonBase*)x)->Tag
    KON_T_STATE,
    KON_T_FLONUM,
    KON_T_BIGNUM,
    KON_T_PAIR,
    KON_T_SYMBOL,
    KON_T_SYNTAX_MARKER,
    KON_T_BYTES,
    KON_T_STRING,
    KON_T_VECTOR,
    KON_T_TABLE,
    KON_T_CELL,
    KON_T_QUOTE,
    KON_T_QUASIQUOTE,
    KON_T_EXPAND,
    KON_T_UNQUOTE,
    KON_T_ENV,
    KON_T_ATTR_SLOT,
    KON_T_MSG_DISPATCHER,
    // KON_T_CONTINUATION,
    // KON_T_TRAMPOLINE,
    KON_T_PROCEDURE,
    KON_T_CPOINTER,
    KON_T_EXCEPTION
} KonType;

typedef struct KonState KonState;
typedef struct KonSymbol KonSymbol;
typedef struct KonSyntaxMarker KonSyntaxMarker;
typedef struct KonString KonString;
typedef struct KonTable KonTable;
typedef struct KonVector KonVector;
typedef struct KonPair KonPair;
typedef struct KonCell KonCell;
typedef struct KonQuote KonQuote;
typedef struct KonQuasiquote KonQuasiquote;
typedef struct KonExpand KonExpand;
typedef struct KonUnquote KonUnquote;
typedef struct KonEnv KonEnv;
typedef struct KonAttrSlot KonAttrSlot;
typedef struct KonMsgDispatcher KonMsgDispatcher;
typedef struct KonProcedure KonProcedure;


typedef struct KonBase {
    KonType Tag;
    
    // boxed fixnum
    // for types in KonType enum, the MsgDispatcherId is same as KonType
    unsigned int MsgDispatcherId;

    
    char IsMarked;
} KonBase;

typedef volatile union _Kon* KN;

typedef enum {
    KON_SYM_PREFIX_WORD, // !ass
    KON_SYM_SUFFIX_WORD, // ^ass
    KON_SYM_WORD,  // abc
    KON_SYM_VARIABLE,    // @abc
    KON_SYM_IDENTIFIER, // $abc
    KON_SYM_STRING, // $''
    KON_SYM_SLOT, // /tag /. /.. /~
    KON_SYM_EXEC_MSG, // .add 5 2;
} KonSymbolType;

struct KonSymbol {
    KonBase Base;
    const char* Data;
    KonSymbolType Type;
};

typedef enum {
    KON_QUOTE_IDENTIFER,    // @abc
    KON_QUOTE_SYMBOL,       // @'zhang san'
    KON_QUOTE_VECTOR,        // @[1 2 3]
    KON_QUOTE_LIST,         // @{1 2 3}
    KON_QUOTE_TABLE,        // @(:a 1 :b 2)
    KON_QUOTE_CELL          // @<ojb (:a 1 :b 2)>
} KonQuoteType;

struct KonQuote {
    KonBase Base;
    KN Inner;
    KonQuoteType Type;
};


typedef enum {
    KON_QUASI_VECTOR,        // $[1 2 3]
    KON_QUASI_LIST,         // ${1 2 3}
    KON_QUASI_TABLE,        // $(:a 1 :b 2)
    KON_QUASI_CELL          // $<ojb (:a 1 :b 2)>
} KonQuasiquoteType;

struct KonQuasiquote {
    KonBase Base;
    KN Inner;
    KonQuasiquoteType Type;
};




typedef enum {
    KON_EXPAND_REPLACE,          // $.abc
    KON_EXPAND_SEQ,        // $~.[1 2 3]
    KON_EXPAND_KV        // $%.(:a 1 :b 2)
} KonExpandType;

struct KonExpand {
    KonBase Base;
    KN Inner;
    KonExpandType Type;
};

typedef enum {
    KON_UNQUOTE_REPLACE,          // @.abc
    KON_UNQUOTE_SEQ,        // @~.{[1 2 3]}
    KON_UNQUOTE_KV        // @%.{$(:a $var :b 2)}
} KonUnquoteType;

struct KonUnquote {
    KonBase Base;
    KN Inner;
    KonUnquoteType Type;
};


typedef enum {
    KON_SYNTAX_MARKER_APPLY,        // %
    KON_SYNTAX_MARKER_PIPE,         // |
    KON_SYNTAX_MARKER_CLAUSE_END    // ;
} KonSyntaxMarkerType;

// eg: % | ;
struct KonSyntaxMarker {
    KonBase Base;
    KonSyntaxMarkerType Type;
};

struct KonPair {
    KonBase Base;
    KonPair* Prev;
    KN Body;
    KonPair* Next;
};

struct KonCell {
    KonBase Base;
    KN Name;
    KonVector* Vector;
    KonTable* Table;
    KonPair* List;
    KonCell* Next;
    KonCell* Prev;
};

struct KonEnv {
    KonBase Base;
    KonEnv* Parent;
    KxHashTable* Bindings;
    KxHashTable* MsgDispatchers;
};

struct KonAttrSlot {
    KonBase Base;
    bool IsDir;
    bool OpenToChildren;
    bool OpenToSibling;
    bool CanWrite;
    bool IsProc;    // func, blk, lambda
    bool IsMethod;  // func(self, p2, p3)
    KN Value;
    KxHashTable* Folder;
};

struct KonMsgDispatcher {
    KonBase Base;
    KN Config;
    char* Name;
    KonProcedure* OnApplyArgs;  // % p1 p2;
    KonProcedure* OnSelectPath;  // /abc /efg
    KonProcedure* OnMethodCall; // . push 1 2;
    KonProcedure* OnVisitVector;  // []
    KonProcedure* OnVisitTable; // ()
    KonProcedure* OnVisitCell;  // <>
};

typedef enum {
    // KON_PRIMARY_FUNC,   // high order native func
    KON_NATIVE_FUNC,
    KON_NATIVE_OBJ_METHOD,
    // dynamic scope.
    // make by !func
    KON_COMPOSITE_FUNC,
    // lexial(static) scope
    // make by !lambda
    KON_COMPOSITE_LAMBDA,
    // a block of code, can be assigned to a var, and call
    // vars lookup in eval env. like a func without params
    // make by !blk
    KON_COMPOSITE_BLK,
    KON_COMPOSITE_OBJ_METHOD,
} KonProcedureType;

typedef KN (*KonNativeFuncRef)(KonState* kstate, KN argList);
typedef KN (*KonNativeObjMethodRef)(KonState* kstate, void* objRef, KN argList);

struct KonProcedure {
    KonBase Base;
    KonProcedureType Type;
    int ParamNum;   // exclude ... arg
    bool HasVariableParam;   // if have ... in arg list
    union {
        KonNativeFuncRef NativeFuncRef;

        KonNativeObjMethodRef NativeObjMethod;

        struct {
            KonPair* ArgList;
            KonPair* Body;
            KonEnv* LexicalEnv;
        } Composite;
    };
};

struct KonState {
    KonBase Base;
    KonEnv* RootEnv;
    unsigned int LastMsgDispatcherId;
};

typedef struct KonFlonum {
    KonBase Base;
    double Flonum;
} KonFlonum;

// TODO replace to kon string impl
struct KonString {
    KonBase Base;
    KxStringBuffer* String;
};

struct KonVector {
    KonBase Base;
    KxVector* Vector;
};

struct KonTable {
    KonBase Base;
    KxHashTable* Table;
};

union _Kon {
    KonBase KonBase;
    KonState KonState;
    KonSymbol KonSymbol;
    KonSyntaxMarker KonSyntaxMarker;
    KonString KonString;
    KonTable KonTable;
    KonVector KonVector;
    KonPair KonPair;
    KonCell KonCell;
    KonQuote KonQuote;
    KonQuasiquote KonQuasiquote;
    KonExpand KonExpand;
    KonUnquote KonUnquote;
    KonEnv KonEnv;
    KonAttrSlot KonAttrSlot;
    KonProcedure KonProcedure;
};

// types end
////

KON_API KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag);
#define KON_ALLOC_TYPE_TAG(kstate,t,tag)  ((t *)KON_AllocTagged(kstate, sizeof(t), tag))
#define KON_FREE(kstate, ptr) KON_GC_FREE(ptr)


// inline KN KON_AllocTagged(KonState* kstate, size_t size, kon_uint_t tag)
// {
//   KN res = (KN) KON_GC_MALLOC(size);
//   if (res && ! kon_is_exception(res)) {
//     ((KonBase*)res)->Tag = tag;
//   }
//   return res;
// }


// TODO change to self managed gc,malloc,free
#define KON_GC_MALLOC malloc
#define KON_GC_FREE free

////
// predicates




// #define KonDef(t)          struct Kon##t *
#define CAST_Kon(t, v)          ((struct Kon##t *)v)
#define KON_TYPE(x)      kon_type((KN)(x))
#define KON_PTR_TYPE(x)     (((KonBase*)(x))->Tag)



#define kon_is_true(x)    ((x) != KON_FALSE)
#define kon_is_false(x)      ((x) == KON_FALSE)

#define kon_is_null(x)    ((x) == KON_NULL)
#define kon_is_nil(x)    ((x) == KON_NIL)
#define kon_is_pointer(x) (((kon_uint_t)(size_t)(x) & KON_POINTER_MASK) == KON_POINTER_TAG)
#define KON_IS_FIXNUM(x)  (((kon_uint_t)(x) & KON_FIXNUM_MASK) == KON_FIXNUM_TAG)

#define kon_is_immediate_symbol(x) (((kon_uint_t)(x) & KON_IMMEDIATE_MASK) == KON_ISYMBOL_TAG)
#define kon_is_char(x)    (((kon_uint_t)(x) & KON_EXTENDED_MASK) == KON_CHAR_TAG)
#define kon_is_reader_label(x) (((kon_uint_t)(x) & KON_EXTENDED_MASK) == KON_READER_LABEL_TAG)
#define kon_is_boolean(x) (((x) == KON_TRUE) || ((x) == KON_FALSE))

#define KON_GET_PTR_TAG(x)      (((KonBase*)x)->Tag)

#define kon_check_tag(x,t)  (kon_is_pointer(x) && (KON_PTR_TYPE(x) == (t)))

// #define kon_slot_ref(x,i)   (((KN)&((x)->Value))[i])
// #define kon_slot_set(x,i,v) (((KN)&((x)->Value))[i] = (v))

#define KON_IS_FLONUM(x)      (kon_check_tag(x, KON_T_FLONUM))
#define kon_flonum_value(f) (((KonFlonum*)f)->Flonum)
#define kon_flonum_value_set(f, x) (((KonFlonum*)f)->Flonum = x)

#define kon_is_bytes(x)         (kon_check_tag(x, KON_T_BYTES))
#define kon_is_string(x)        (kon_check_tag(x, KON_T_STRING))
#define KON_IS_SYMBOL(x)        (kon_check_tag(x, KON_T_SYMBOL))
#define kon_is_variable(x)      (kon_check_tag(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_VARIABLE)
#define KON_IS_WORD(x)      (kon_check_tag(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_WORD)
#define KON_IS_PREFIX_MARCRO(x) (kon_check_tag(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_PREFIX_WORD)
#define kon_is_syntax_marker(x) (kon_check_tag(x, KON_T_SYNTAX_MARKER))
#define KON_IS_SYM_SLOT(x) (kon_check_tag(x, KON_T_SYMBOL) && ((KonSymbol*)x)->Type == KON_SYM_SLOT)

#define KON_IS_ATTR_SLOT(x) (kon_check_tag(x, KON_T_ATTR_SLOT))

#define KON_IS_PAIR(x)       (kon_check_tag(x, KON_T_PAIR))
#define kon_is_vector(x)     (kon_check_tag(x, KON_T_VECTOR))
#define kon_is_table(x)     (kon_check_tag(x, KON_T_TABLE))
#define kon_is_cell(x)     (kon_check_tag(x, KON_T_CELL))

#define kon_is_quote(x)    (kon_check_tag(x, KON_T_QUOTE))
#define kon_is_quasiquote(x)    (kon_check_tag(x, KON_T_QUASIQUOTE))
#define kon_is_expand(x)    (kon_check_tag(x, KON_T_EXPAND))
#define kon_is_unquote(x)    (kon_check_tag(x, KON_T_UNQUOTE))


#define kon_is_env(x)        (kon_check_tag(x, KON_T_ENV))
#define kon_is_continuation(x)        (kon_check_tag(x, KON_T_CONTINUATION))
#define kon_is_trampoline(x)        (kon_check_tag(x, KON_T_TRAMPOLINE))
#define kon_is_cpointer(x)   (kon_check_tag(x, KON_T_CPOINTER))
#define kon_is_exception(x)  (kon_check_tag(x, KON_T_EXCEPTION))


/// either immediate (NUM,BOOL,NIL) or a fwd
static inline KonType kon_type(KN obj) {
  if (KON_IS_FIXNUM(obj) || KON_IS_FLONUM(obj))  return KON_T_NUMBER;
  if (obj == KON_TRUE || obj == KON_FALSE) return KON_T_BOOLEAN;
  if (obj == KON_UKN)  return KON_T_UKN;
  if (obj == KON_NULL)  return KON_T_NULL;
  if (obj == KON_NIL)  return KON_T_NIL;
  return (((KonBase*)(obj))->Tag);
}

// predicates end
////

////
// data structure util start

#define KON_NEG_ONE KON_MAKE_FIXNUM(-1)
#define KON_ZERO    KON_MAKE_FIXNUM(0)
#define KON_ONE     KON_MAKE_FIXNUM(1)
#define KON_TWO     KON_MAKE_FIXNUM(2)
#define KON_THREE   KON_MAKE_FIXNUM(3)
#define KON_FOUR    KON_MAKE_FIXNUM(4)
#define KON_FIVE    KON_MAKE_FIXNUM(5)
#define KON_SIX     KON_MAKE_FIXNUM(6)
#define KON_SEVEN   KON_MAKE_FIXNUM(7)
#define KON_EIGHT   KON_MAKE_FIXNUM(8)
#define KON_NINE    KON_MAKE_FIXNUM(9)
#define KON_TEN     KON_MAKE_FIXNUM(10)


#define KON_MAKE_FIXNUM(n)    ((KN) ((((kon_int_t)(n))*(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS)) | KON_FIXNUM_TAG))
#define KON_UNBOX_FIXNUM(n)   (((kon_int_t)((kon_uint_t)(n) & ~KON_FIXNUM_TAG))/(kon_int_t)((kon_int_t)1<<KON_FIXNUM_BITS))

static inline KN KON_MAKE_FLONUM(KonState* kstate, double num) {
  KonFlonum* result = (KonFlonum*)KON_AllocTagged(kstate, sizeof(KonFlonum), KON_T_FLONUM);
  result->Flonum = num;
  return (result);
}
#define KON_UNBOX_FLONUM(n) ((KonFlonum*)n)->Flonum

#define KON_FIELD(x, type, field) (((type *)x)->field)

#define kon_make_character(n)  ((KN) ((((kon_int_t)(n))<<KON_EXTENDED_BITS) + KON_CHAR_TAG))
#define kon_unbox_character(n) ((int) (((kon_int_t)(n))>>KON_EXTENDED_BITS))

#define KON_UNBOX_STRING(str) (((KonString*)str)->String)

#define KON_UNBOX_VECTOR(str) (((KonVector*)str)->Vector)

#define KON_UNBOX_TABLE(str) (((KonVector*)str)->Vector)

#define KON_UNBOX_SYMBOL(s) (((KonSymbol*)s)->Data)

#define KON_UNBOX_QUOTE(s) (((KonQuote*)s)->Inner)

// list
#define KON_CONS(kstate, a, b) KON_Cons(kstate, NULL, 2, a, b)
#define kon_car(x)         (KON_FIELD(x, KonPair, Body))
#define kon_cdr(x)         (KON_FIELD(x, KonPair, Next))

#define kon_caar(x)      (kon_car(kon_car(x)))
#define kon_cadr(x)      (kon_car(kon_cdr(x)))
#define kon_cdar(x)      (kon_cdr(kon_car(x)))
#define kon_cddr(x)      (kon_cdr(kon_cdr(x)))
#define kon_caaar(x)     (kon_car(kon_caar(x)))
#define kon_caadr(x)     (kon_car(kon_cadr(x)))
#define kon_cadar(x)     (kon_car(kon_cdar(x)))
#define kon_caddr(x)     (kon_car(kon_cddr(x)))
#define kon_cdaar(x)     (kon_cdr(kon_caar(x)))
#define kon_cdadr(x)     (kon_cdr(kon_cadr(x)))
#define kon_cddar(x)     (kon_cdr(kon_cdar(x)))
#define kon_cdddr(x)     (kon_cdr(kon_cddr(x)))
#define kon_cadddr(x)    (kon_cadr(kon_cddr(x))) /* just these two */
#define kon_cddddr(x)    (kon_cddr(kon_cddr(x)))

#define kon_list1(kstate,a)        KON_CONS((kstate), (a), KON_NIL)


// data structure util end
////



// data structure apis start

KON_API KN KON_Stringify(KonState* kstate, KN source);
KN KON_ToFormatString(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// number
KON_API KN KON_FixnumStringify(KonState* kstate, KN source);

KON_API KN KON_MakeFlonum(KonState* kstate, double f);
KON_API KN KON_FlonumStringify(KonState* kstate, KN source);

// char
KON_API KN KON_CharStringify(KonState* kstate, KN source);

// string
KON_API KN KON_MakeString(KonState* kstate, const char* str);
KON_API KN KON_MakeEmptyString(KonState* kstate);
KON_API const char* KON_StringToCstr(KN str);
KON_API KN KON_StringStringify(KonState* kstate, KN source);

KON_API KN KON_VectorStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// symbol
KON_API KN KON_SymbolStringify(KonState* kstate, KN source);
KON_API const char* KON_SymbolToCstr(KN sym);

// list
// KON_API KN KON_MakePairList(KonState* kstate, ...);
KON_API KN KON_Cons(KonState* kstate, KN self, kon_int_t n, KN head, KN tail);
KON_API KN KON_PairList2(KonState* kstate, KN a, KN b);
KON_API KN KON_PairList3(KonState* kstate, KN a, KN b, KN c);
KON_API bool KON_IsPairList(KN source);
KN KON_PairListStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
KN KON_PairListRevert(KonState* kstate, KN source);
KN KON_PairListLength(KonState* kstate, KN source);

// table
KN KON_TableStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

// cell
KN KON_CellStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);

KN KON_SyntaxMarkerStringify(KonState* kstate, KN source);

// @
KN KON_QuoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// $
KN KON_QuasiquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[].
KN KON_ExpandStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);
// eg $[]e.
KN KON_UnquoteStringify(KonState* kstate, KN source, bool newLine, int depth, char* padding);


KN MakeNativeProcedure(KonState* kstate, KonProcedureType type, KonNativeFuncRef funcRef);

KN MakeMsgDispatcher(KonState* kstate);

KN MakeAttrSlotLeaf(KonState* kstate, KN value, char* mod);
KN MakeAttrSlotFolder(KonState* kstate, char* mod);

// data structure apis end

// common utils start
KON_API const char* KON_HumanFormatTime();

KN KON_VectorToKonPairList(KonState* kstate, KxVector* vector);

// common utils end


#endif
