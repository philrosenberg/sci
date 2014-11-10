

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sun Aug 10 13:16:37 2014
 */
/* Compiler settings for idl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0595 
    protocol : dce , ms_ext, app_config, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "idl_h.h"

#define TYPE_FORMAT_STRING_SIZE   23                                
#define PROC_FORMAT_STRING_SIZE   121                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _idl_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } idl_MIDL_TYPE_FORMAT_STRING;

typedef struct _idl_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } idl_MIDL_PROC_FORMAT_STRING;

typedef struct _idl_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } idl_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const idl_MIDL_TYPE_FORMAT_STRING idl__MIDL_TypeFormatString;
extern const idl_MIDL_PROC_FORMAT_STRING idl__MIDL_ProcFormatString;
extern const idl_MIDL_EXPR_FORMAT_STRING idl__MIDL_ExprFormatString;

/* Standard interface: apex, ver. 1.0,
   GUID={0x054c8475,0x3e9f,0x45fc,{0xbd,0x55,0x72,0x32,0x18,0xe0,0x50,0x3c}} */


extern const MIDL_SERVER_INFO apex_ServerInfo;

extern const RPC_DISPATCH_TABLE apex_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE apex___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x054c8475,0x3e9f,0x45fc,{0xbd,0x55,0x72,0x32,0x18,0xe0,0x50,0x3c}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&apex_v1_0_DispatchTable,
    0,
    0,
    0,
    &apex_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE apex_v1_0_s_ifspec = (RPC_IF_HANDLE)& apex___RpcServerInterface;

extern const MIDL_STUB_DESC apex_StubDesc;


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const idl_MIDL_PROC_FORMAT_STRING idl__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure storeDoubleArray */

			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x8 ),	/* 8 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x3,		/* 3 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 24 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 26 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 28 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter arraySize */

/* 30 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 32 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter array */

/* 36 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 38 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 40 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Procedure checkLengthDoubleArray */

/* 42 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 44 */	NdrFcLong( 0x0 ),	/* 0 */
/* 48 */	NdrFcShort( 0x1 ),	/* 1 */
/* 50 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	NdrFcShort( 0x1c ),	/* 28 */
/* 56 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0x2,		/* 2 */
/* 58 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */
/* 64 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 66 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 68 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 70 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter arraySize */

/* 72 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 74 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure retreiveDoubleArray */

/* 78 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x2 ),	/* 2 */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	NdrFcShort( 0x8 ),	/* 8 */
/* 90 */	NdrFcShort( 0x0 ),	/* 0 */
/* 92 */	0x43,		/* Oi2 Flags:  srv must size, clt must size, has ext, */
			0x3,		/* 3 */
/* 94 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 96 */	NdrFcShort( 0x1 ),	/* 1 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter name */

/* 102 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 104 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 106 */	NdrFcShort( 0x4 ),	/* Type Offset=4 */

	/* Parameter arraySize */

/* 108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 110 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter array */

/* 114 */	NdrFcShort( 0x13 ),	/* Flags:  must size, must free, out, */
/* 116 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 118 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

			0x0
        }
    };

static const idl_MIDL_TYPE_FORMAT_STRING idl__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  4 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/*  8 */	NdrFcShort( 0x8 ),	/* 8 */
/* 10 */	0x29,		/* Corr desc:  parameter, FC_ULONG */
			0x0,		/*  */
/* 12 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 14 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 16 */	0xc,		/* FC_DOUBLE */
			0x5b,		/* FC_END */
/* 18 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 20 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const unsigned short apex_FormatStringOffsetTable[] =
    {
    0,
    42,
    78
    };


static const MIDL_STUB_DESC apex_StubDesc = 
    {
    (void *)& apex___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    idl__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x8000253, /* MIDL Version 8.0.595 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static const RPC_DISPATCH_FUNCTION apex_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE apex_v1_0_DispatchTable = 
    {
    3,
    (RPC_DISPATCH_FUNCTION*)apex_table
    };

static const SERVER_ROUTINE apex_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)storeDoubleArray,
    (SERVER_ROUTINE)checkLengthDoubleArray,
    (SERVER_ROUTINE)retreiveDoubleArray
    };

static const MIDL_SERVER_INFO apex_ServerInfo = 
    {
    &apex_StubDesc,
    apex_ServerRoutineTable,
    idl__MIDL_ProcFormatString.Format,
    apex_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

