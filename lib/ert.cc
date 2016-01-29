//
// ert.cc
//
//	Function that will return the text value for an error
//	message.
//
// History
//
//	12/12/1998 - Kevin Handy
//		Original version
//
//	04/12/2000 - Kevin Handy
//		Make BasicErrorMessage static.
//
#include <cstring>
#include <cstdlib>
#include "bstring.h"

//
// The database
//
//! Database of error messages
static const char* BasicErrorMessage[] =
{
	"Unused",					// 0
	"?Bad directory for device",			// 1
	"?Illegal file name",				// 2
	"?Account or device in use",			// 3
	"?No room for user on device",			// 4
	"?Can't find file or account",			// 5
	"?Not a valid device",				// 6
	"?I/O channel already open",			// 7
	"?Device not available",			// 8
	"?I/O channel not open",			// 9
	"?Protection violation",			// 10
	"?End of file on device",			// 11
	"?Fatal system I/O failure",			// 12
	"?User data error on device",			// 13
	"?Device hung or write locked",			// 14
	"?Wait exhausted",				// 15
	"?Name or account now exists",			// 16
	"?Too many open files on unit",			// 17
	"?Illegal SYS() usage",				// 18
	"?Disk block is interlocked",			// 19
	"?Pack IDs don't match",			// 20
	"?Disk pack is not mounted",			// 21
	"?Disk pack is locked out",			// 22
	"?Illegal cluster size",			// 23
	"?Disk pack is private",			// 24
	"?Disk pack needs 'CLEANing'",			// 25
	"?Fatal disk pack mount error",			// 26
	"?I/O to detached keyboard",			// 27
	"?Programmable ^C trap",			// 28
	"?Corrupted file structure",			// 29
	"?Device not file-structured",			// 30
	"?Illegal byte count for I/O",			// 31
	"?No buffer space available",			// 32
	"?Odd address trap",				// 33
	"?Reserved instruction trap",			// 34
	"?Memory management violation",			// 35
	"?SP stack Overflow",				// 36
	"?Disk error during swap",			// 37
	"?Memory parity (or ECC) failure",		// 38
	"?Magtape select error",			// 39
	"?Magtape record length error",			// 40
	"?Non-res run-time system",			// 41
	"?Virtual buffer too large",			// 42
	"?Virtual array not on disk",			// 43
	"?Matrix or array too big",			// 44
	"?Virtual array not yet open",			// 45
	"?Illegal I/O channel",				// 46
	"?Line too long",				// 47
	"?Floating point error or overflow",		// 48
	"?Argument too large in EXP",			// 49
	"%Data format error",				// 50
	"?Integer error or overflow",			// 51
	"?Illegal number",				// 52
	"?Illegal argument in LOG",			// 53
	"?Imaginary square roots",			// 54
	"?Subscript out of range",			// 55
	"?Can't invert matrix",				// 56
	"?Out of data",					// 57
	"?ON statement out of range",			// 58
	"?Not enough data in record",			// 59
	"?Integer overflow, FOR loop",			// 60
	"?Division by 0",				// 61
	"?No run-time system",				// 62
	"?FIELD overflows buffer",			// 63
	"?Not a random access device",			// 64
	"?Illegal MAGTAPE() usage",			// 65
	"?Missing special feature",			// 66
	"?Illegal switch usage",			// 67
	"?Unused",					// 68
	"?Unused",					// 69
	"?Unused",					// 70
	"?Statement not found",				// 71
	"?RETURN without GOSUB",			// 72
	"?FNEND without function call",			// 73
	"?Undefined function called",			// 74
	"?Illegal symbol",				// 75
	"?Illegal verb",				// 76
	"?Illegal expression",				// 77
	"?Illegal mode mixing",				// 78
	"?Illegal IF statement",			// 79
	"?Illegal conditional clause",			// 80
	"?Illegal function name",			// 81
	"?Illegal dummy variable",			// 82
	"?Illegal FN redefinition",			// 83
	"?Illegal line number(s)",			// 84
	"?Modifier error",				// 85
	"?Can't compile statement",			// 86
	"?Expression too complicated",			// 87
	"?Arguments don't match",			// 88
	"?Too many arguments",				// 89
	"%Inconsistent function usage",			// 90
	"?Illegal DEF nesting",				// 91
	"?FOR without NEXT",				// 92
	"?NEXT without FOR",				// 93
	"?DEF without FNEND",				// 94
	"?FNEND without DEF",				// 95
	"?Literal string needed",			// 96
	"?Too few arguments",				// 97
	"?Syntax error",				// 98
	"?String is needed",				// 99
	"?Number is needed",				// 100
	"?Data type error",				// 101
	"?One or two dimensions only",			// 102
	"?Internal error in Run-Time Library.",		// 103
	"?RESUME and no error",				// 104
	"?Redimensioned array",				// 105
	"%Inconsistent subscript use",			// 106
	"?ON statement needs GOTO",			// 107
	"?End of statement not seen",			// 108
	" What?",					// 109
	"?Bad line number pair",			// 110
	"?Not enough available memory",			// 111
	"?Execute only file",				// 112
	"?Please use the RUN command",			// 113
	"?Can't CONTinue",				// 114
	"?File exists-RENAME/REPLACE",			// 115
	"?PRINT-USING format error",			// 116
	"?Matrix or array without DIM",			// 117
	"?Bad number in PRINT-USING",			// 118
	"?Illegal in immediate mode",			// 119
	"?PRINT-USING buffer overflow",			// 120
	"?Illegal statement",				// 121
	"?Illegal FIELD variable",			// 122
	" Stop",					// 123
	"?Matrix dimension error",			// 124
	"?Wrong math package",				// 125
	"?Maximum memory exceeded",			// 126
	"?SCALE factor interlock",			// 127
	"?Tape records not ANSI",			// 128
	"?Tape BOT detected",				// 129
	"?Key not changeable",				// 130
	"?No current record",				// 131
	"?Record has been deleted",			// 132
	"?Illegal usage for device",			// 133
	"?Duplicate key detected",			// 134
	"?Illegal usage",				// 135
	"?Illegal or illogical access",			// 136
	"?Illegal key attributes",			// 137
	"?File is locked",				// 138
	"?Invalid file options",			// 139
	"?Index not initialized",			// 140
	"?Illegal operation",				// 141
	"?Illegal record on file",			// 142
	"?Bad record identifier",			// 143
	"?Invalid key of reference",			// 144
	"?Key size too large",				// 145
	"?Tape not ANSI labelled",			// 146
	"?RECORD number exceeds maximum",		// 147
	"?Bad RECORDSIZE value on OPEN",		// 148
	"?Not at end of file",				// 149
	"?No primary key specified",			// 150
	"?Key field beyond end of record",		// 151
	"?Illogical record accessing",			// 152
	"?Record already exists",			// 153
	"?Record/bucket locked",			// 154
	"?Record not found",				// 155
	"?Size of record invalid",			// 156
	"?Record on file too big",			// 157
	"?Primary key out of sequence",			// 158
	"?Key larger than record",			// 159
	"?File attributes not matched",			// 160
	"?Move overflows buffer",			// 161
	"?Cannot open file",				// 162
	"?No file name",				// 163
	"?Terminal format file required",		// 164
	"?Cannot position to EOF",			// 165
	"?Negative fill or string length",		// 166
	"?Illegal record format",			// 167
	"?Illegal ALLOW clause",			// 168
	"?Unused",					// 169
	"?Index not fully optimized",			// 170
	"?RRV not fully updated",			// 171
	"?Record lock failed",				// 172
	"?Invalid RFA field",				// 173
	"?File expiration date not yet reached",	// 174
	"?Node name error",				// 175
	"%Negative or zero TAB",			// 176
	"%Too much data in record",			// 177
	"?System memory for file sharing exhausted",	// 178
	"?Unexpired file date",				// 179
	"?No support for operation in task",		// 180
	"?Decimal error or overflow",			// 181
	"?Network operation rejected",			// 182
	"?REMAP overflows buffer",			// 183
	"?Unaligned REMAP variable",			// 184
	"?RECORDSIZE overflows MAP buffer",		// 185
	"?Improper error handling",			// 186
	"?Illegal record locking clause",		// 187
	"?UNLOCK EXPLICIT requires RECORDSIZE 512",	// 188
	"%Too little data in record",			// 189
	"?Illegal network operation",			// 190
	"?Illegal terminal-format file operation",	// 191
	"?Illegal wait value",				// 192
	"?Detected deadlock while waiting for GET or FIND",// 193
	"?Not a BASIC error",				// 194
	"?Dimension number out of range",		// 195
	"?REMAP string is not static",			// 196
	"?Array too small",				// 197
	"?Unused",					// 198
	"?Unused",					// 199
	"?Unused",					// 200
	"?Unused",					// 201
	"?Unused",					// 202
	"?Unused",					// 203
	"?Unused",					// 204
	"?Unused",					// 205
	"?Unused",					// 206
	"?Unused",					// 207
	"?Unused",					// 208
	"?Unused",					// 209
	"?Unused",					// 210
	"?Unused",					// 211
	"?Unused",					// 212
	"?Unused",					// 213
	"?Unused",					// 214
	"?Unused",					// 215
	"?Unused",					// 216
	"?Unused",					// 217
	"?Unused",					// 218
	"?Unused",					// 219
	"?Unused",					// 220
	"?Unused",					// 221
	"?Unused",					// 222
	"?Unused",					// 223
	"?Unused",					// 224
	"?Unused",					// 225
	"?VAX GKS is not installed",			// 226
	"?String too long",				// 227
	"?Record attributes not matched",		// 228
	"?Differing use of LONG/WORD or SINGLE/DOUBLE qualifiers",// 229
	"?No fields in image",				// 230
	"?Illegal string image",			// 231
	"?Null image",					// 232
	"?Illegal numeric image",			// 233
	"?Numeric image for string",			// 234
	"?String image for numeric",			// 235
	"?TIME limit exceeded",				// 236
	"?First arg to SEG$ greater than second",	// 237
	"?Arrays must be same dimension",		// 238
	"?Arrays must be square",			// 239
	"?Cannot change array dimensions",		// 240
	"?Floating overflow",				// 241
	"?Floating underflow",				// 242
	"?CHAIN to non-existent line number",		// 243
	"?Exponentiation error",			// 244
	"?Illegal exit from DEF*",			// 245
	"?ERROR trap needs RESUME",			// 246
	"?Illegal RESUME to subroutine",		// 247
	"?Illegal return from subroutine",		// 248
	"?Argument out of bounds",			// 249
	"?Not implemented",				// 250
	"?Recursive subroutine call",			// 251
	"?FILE ACP failure",				// 252
	"?Directive error",				// 253
	"?Unused",					// 254
	"?Unused",					// 255
	"?Prompt/echo type not supported",		// 256
	"?Illegal transformation number",		// 257
	"?Illegal picture operation",			// 258
	"?Clipping must be set to \"ON\" or \"OFF\"",	// 259
	"?Transformation numbers are not different",	// 260
	"?Color indices are not contiguous",		// 261
	"?Illegal area style",				// 262
	"?Illegal text justification",			// 263
	"?Illegal text precision",			// 264
	"?Illegal text path",				// 265
	"?Illegal device identification number",	// 266
	"?Device type is not supported",		// 267
	"?Device is not open",				// 268
	"?Device is an output metafile",		// 269
	"?Device is an input metafile",			// 270
	"?Unused",					// 271
	"?Device and operation are incompatible",	// 272
	"?Coordinates are not within NDC space",	// 273
	"?Illegal line style",				// 274
	"?Illegal line size",				// 275
	"?Illegal point style",				// 276
	"?Illegal text width to height ratio",		// 277
	"?Illegal text height",				// 278
	"?Illegal area style index",			// 279
	"?Illegal color index",				// 280
	"?Number of coordinates is insufficient",	// 281
	"?Unit number is not defined for the device",	// 282
	"?Illegal echo area",				// 283
	"?Illegal initial value",			// 284
	"?Entered points not within a transformation",	// 285
	"?Unknown GKS error",				// 286
	"?Invalid character in string",			// 287
	"?String length is zero",			// 288
	"?Data overflow",				// 289
	"?Illegal count clause",			// 290
	"?Illegal color mix",				// 291
	"?Illegal device name in OPEN",			// 292
	"?User aborted input.  Locate point cancelled",	// 293
	"?Unused",					// 294
	"?Unused",					// 295
	"?Unused",					// 296
	"?Unused",					// 297
	"?Unused",					// 298
	"?Unused",					// 299
	"?Unused"					// 300
};

/*! Return error message

	Returns the text of an error message when given the error number
	\return String containing error message
*/
std::string& basic::ert(
	long error	//!< Error number
)
{
	const char* Text = "?Unused";

	if ((error >= 0) && (error <= 300))
	{
		Text = BasicErrorMessage[error];
	}
	std::string* Result = new std::string(Text);
	return *Result;
}
