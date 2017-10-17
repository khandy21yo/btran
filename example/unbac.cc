// Start of Decompiler for BASIC-PLUS
// Programmed by Bill Lortz
// July 12, 1979
// Copyright @1979 by Bill Lortz

//
// Translated from Basic to C++ using btran
// on Friday, November 04, 2016 at 15:02:53
//

//
// Heavily edited by Kevin Handy to eliminate bugs
// from translation and to get operational (mostly) under Linux.
// November 2016.
//

//
// Copy into 'example' subdirectory of btran, and then
// g++ -o unbac -O0 -g -I ../lib  unbac.cc ../lib/libbtran.a
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <map>
#include "basicfun.h"

#include <sys/types.h>		// For mmap
#include <sys/stat.h>		// For mmap
#include <fcntl.h>		// For mmap
#include <sys/mman.h>		// For mmap

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>


#include <cmath>

#define INLINE 2
#define NEWLINE 1
#define DONE 3

//
// Function Prototypes
//
int main2(std::string i);
std::ofstream outfile;


//
// Function Prototypes
//
static int fnn(std::string &a);
static void fnist(int a, std::string e);
static int fng_n(void);
static int fng_t(int x, std::string v);
static int fnw(int a);
static int fnb(int a);




//
// Function Prototypes
//
static int fncheck(int a, int i);
static void fnnew_line(void);
static int fnscan(int t);
static std::string fnarg(void);
static std::string fnarg1(void);
static std::string fnclassify_push(int stmt);
static int fnpush(int a);
static int fnpop(void);
static std::string fnpush_V1(std::string a);
static std::string fnpop_V2(void);
static void fnclean_up(void);
static int fnr(int a);
static std::string fnvar(int a);
static void fnprint(std::string a);
static std::string fnindent(void);
static void fnformat(std::string b);
static void fnerror(int i, std::string e);

static int sgn(int x)
{
	if (x < 0)
	{
		return -1;
	}
	else if (x > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//
// Current position of print head on io channel.
// Doesn't work.
// Not something that Unix/C++ keeps track of.
//
static int Ccpos(int x)
{
	return 0;
}

//
// CVT%$
//
// Convert an integer into a two character string
//
static std::string cvtia(int i)
{
	std::string Result = "  ";
	Result[0] = i & 255;
	Result[1] = (i / 256) & 255;
	return Result;
}

//
// CVT$%
//
// Convert a two character string into an integer value.
//
static int cvtai(std::string a)
{
	return (unsigned char)(a[0]) + (unsigned char)(a[1]) * 256;
}

//
// Convert string to float.
// Only works with IEEE floating point on host system.
//
static double cvtaf(std::string a)
{
	union
	{
		char xxa[8];
		float xxb;
	} xxx;
	memset(xxx.xxa, 0, 9);
	xxx.xxa[0] = a[0];
	xxx.xxa[1] = a[1];
	xxx.xxa[2] = a[2];
	xxx.xxa[3] = a[3];
	if (a.size() == 8)
	{
		xxx.xxa[4] = a[4];
		xxx.xxa[5] = a[5];
		xxx.xxa[6] = a[6];
		xxx.xxa[7] = a[7];
	}

	return xxx.xxb / 4.0;
}

//
// doesn't work.
// Doesn't matter.
//
static std::string cvtfa(double d)
{
	return "1234";
}

//
// global vars
//
static int laslin;
static int width;
static std::string c_trm;
static int f_ind;
static int f_for;
static int i_ind;
static int f_cond;
static int stsp;
static int a_V4;
static int bufflg;
static int s_V8;
static int eptr;
static int spda;
static int l_state = 0;
static std::string term;
static int scalef;
static double scalef_;
static int spst;
static int flen;
static std::string ffil;
static int z_a1_t;
static int z_a1_t1;
static int z_a1_op;
static int z_a_op;
static int ecod;
static int bcod;
static int stmt;
static int s_V9[11];
static std::string c_fil;
static int i;
static int slash_n;
static std::string e_trm;
static int t;
static int l;
static int sp;
static int a_V2;
static std::string e;
static int f;
static std::string v;
static int z_a1_l_op;
static int isp;
static std::string s;
std::string c_def;

//  Virtual
static unsigned const char *p;
static int stack_V10[3276];
static std::string stack[3276];
static int stmt_V11[3276];

std::map<long, std::string> var;

//
// Data Statement
//
struct opcodestr {
	int typ2;
	int typ1;
	int len;
	const char *dsc;
} opcode[] = {
	{9,	2,	1,	"9"},		// 0
	{9,	2,	1,	""},		// 1
	{9,	2,	1,	""},		// 2
	{5,	1,	1,	"STOP"},	// 3
	{4,	0,	1,	""},		// 4
	{4,	0,	1,	""},		// 5
	{2,	1,	1,	"SYS"},		// 6
	{2,	1,	1,	"TAB"},		// 7
	{2,	1,	1,	"POS"},		// 8
	{2,	1,	1,	"CHR$"},	// 9
	{5,	11,	1,	""},		// 10
	{2,	1,	1,	"ASCII"},	// 11
	{2,	1,	1,	"DATE$"},	// 12
	{2,	1,	1,	"TIME$"},	// 13
	{2,	1,	1,	"TIME"},	// 14
	{2,	1,	1,	"PEEK"},	// 15
	{5,	2,	1,	"SLEEP "},	// 16
	{5,	2,	1,	"WAIT "},	// 17
	{0,	1,	3,	""},		// 18
	{5,	1,	1,	"RANDOMIZE"},	// 19
	{5,	1,	1,	"RESTORE"},	// 20
	{5,	3,	3,	"RESUME "},	// 21
	{5,	6,	0,	"GOSUB "},	// 22
	{5,	12,	3,	"CHANGE"},	// 23
	{5,	12,	3,	"CHANGE"},	// 24
	{5,	7,	3,	"MAT READ "},	// 25
	{5,	7,	3,	"MAT PRINT "},	// 26
	{5,	7,	3,	"MAT PRINT "},	// 27
	{5,	7,	3,	"MAT INPUT "},	// 28
	{5,	7,	3,	"ZER"},		// 29
	{5,	7,	3,	"CON"},		 // 30
	{5,	7,	3,	"IDN"},		// 31
	{5,	7,	5,	"TRN("},	// 32
	{5,	7,	5,	"INV("},	// 33
	{5,	7,	5,	"*"},		// 34
	{5,	7,	5,	"*"},		// 35
	{5,	7,	5,	""},		// 36
	{5,	7,	7,	"*"},		// 37
	{5,	7,	7,	"+"},		// 38
	{5,	7,	7,	"-"},		// 39
	{5,	8,	1,	" "},		// 40
	{5,	8,	1,	" FOR INPUT "},	// 41
	{5,	8,	1,	" FOR OUTPUT "}, // 42
	{5,	4,	1,	"NAME"},	// 43
	{5,	2,	1,	"KILL "},	// 44
	{9,	0,	1,	"14"},		// 45
	{5,	5,	1,	"CHAIN "},	// 46
	{2,	1,	1,	"LEFT"},	// 47
	{5,	6,	0,	"GOTO "},	// 48
	{5,	3,	3,	"ON ERROR GOTO "},		 // 49
	{6,	4,	1,	""},		// 50
	{2,	2,	3,	""},		// 51
	{9,	0,	1,	""},		// 52
	{9,	0,	1,	""},		// 53
	{5,	11,	4,	""},		// 54
	{5,	1,	1,	"END"},		// 55
	{5,	7,	3,	"MAT PRINT "},	// 56
	{4,	0,	1,	""},		// 57
	{4,	0,	1,	""},		// 58
	{4,	0,	1,	""},		// 59
	{4,	0,	1,	""},		// 60
	{4,	0,	1,	""},		// 61
	{4,	0,	1,	""},		// 62
	{1,	50,	1,	"=="},		// 63
	{9,	8,	1,	"81"},		// 64
	{9,	8,	1,	"07"},		// 65
	{9,	8,	1,	""},		// 66
	{9,	8,	1,	"98"},		// 67
	{4,	0,	1,	""},		// 68
	{4,	0,	1,	""},		// 69
	{4,	0,	1,	""},		// 70
	{4,	0,	1,	""},		// 71
	{4,	0,	1,	""},		// 72
	{4,	0,	1,	""},		// 73
	{4,	0,	1,	""},		// 74
	{4,	0,	1,	""},		// 75
	{4,	0,	1,	""},		// 76
	{0,	0,	1,	"0."},		// 77
	{0,	0,	1,	"1."},		// 78
	{4,	0,	3,	""},		// 79
	{4,	0,	3,	""},		// 80
	{0,	0,	1,	"1%"},		// 81
	{4,	0,	1,	""},		// 82
	{4,	0,	1,	""},		// 83
	{4,	0,	1,	""},		// 84
	{4,	0,	1,	""},		// 85
	{4,	0,	1,	""},		// 86
	{7,	1,	1,	""},		// 87
	{7,	1,	3,	""},		// 88
	{2,	1,	1,	"CVT%$"},	// 89
	{2,	1,	1,	"CVTF$"},	// 90
	{2,	1,	1,	"CVT$%"},	// 91
	{2,	1,	1,	"CVT$F"},	// 92
	{3,	0,	3,	"LSET "},	// 93
	{3,	2,	3,	"LSET "},	// 94
	{3,	4,	3,	"LSET "},	// 95
	{3,	0,	3,	"RSET "},	// 96
	{3,	2,	3,	"RSET "},	// 97
	{3,	4,	3,	"RSET "},	// 98
	{9,	5,	3,	""},		// 99
	{9,	6,	3,	""},		// 100
	{9,	7,	3,	""},		// 101
	{5,	9,	2,	"PUT #"},	// 102
	{5,	9,	2,	"GET #"},	// 103
	{3,	1,	1,	""},		// 104
	{2,	1,	1,	"MAGTAPE"},	// 105
	{5,	2,	1,	"UNLOCK #"},	// 106
	{2,	1,	1,	"XLATE"},	// 107
	{2,	2,	1,	""},		// 108
	{2,	1,	1,	"CVT$$"},	// 109
	{9,	2,	1,	""},		// 110
	{2,	1,	1,	"STRING$"},	// 111
	{2,	1,	1,	"BUFSIZ"},	// 112
	{2,	1,	1,	"SUM$"},	// 113
	{2,	1,	1,	"DIF$"},	// 114
	{2,	1,	1,	"PROD$"},	// 115
	{2,	1,	1,	"QUO$"},	// 116
	{2,	1,	1,	"COMP%"},	// 117
	{2,	1,	1,	"PLACE$"},	// 118
	{2,	1,	1,	"NUM1$"},	// 119
	{0,	2,	3,	""},		// 120
	{2,	0,	1,	""},		// 121
	{2,	1,	1,	"SPEC%"},	// 122
	{2,	0,	1,	""},		// 123
	{4,	0,	1,	""},		// 124
	{4,	0,	1,	""},		// 125
	{4,	0,	1,	""},		// 126
	{4,	0,	1,	""},		// 127
	{1,	60,	1,	"+"},		// 128
	{1,	60,	1,	"+"},		// 129
	{1,	60,	1,	"-"},		// 130
	{1,	60,	1,	"-"},		// 131
	{4,	0,	1,	""},		// 132
	{4,	0,	1,	""},		// 133
	{1,	70,	1,	"*"},		// 134
	{1,	70,	1,	"*"},		// 135
	{1,	70,	1,	"/"},		// 136
	{1,	70,	1,	"/"},		// 137
	{2,	1,	1,	"RAD$"},	// 138
	{2,	1,	1,	"SWAP%"},	// 139
	{1,	80,	1,	"^"},		// 140
	{1,	80,	1,	"^"},		// 141
	{1,	50,	1,	"="},		// 142
	{1,	50,	1,	"="},		// 143
	{1,	50,	1,	"="},		// 144
	{1,	50,	1,	"=="},		// 145
	{1,	50,	1,	">"},		// 146
	{1,	50,	1,	">"},		// 147
	{1,	50,	1,	">"},		// 148
	{1,	50,	1,	">="},		// 149
	{1,	50,	1,	">="},		// 150
	{1,	50,	1,	">="},		// 151
	{1,	50,	1,	"<"},		// 152
	{1,	50,	1,	"<"},		// 153
	{1,	50,	1,	"<"},		// 154
	{1,	50,	1,	"<="},		// 155
	{1,	50,	1,	"<="},		// 156
	{1,	50,	1,	"<="},		// 157
	{1,	50,	1,	"<>"},		// 158
	{1,	50,	1,	"<>"},		// 159
	{1,	50,	1,	"<>"},		// 160
	{1,	91,	1,	"-"},		// 161
	{1,	91,	1,	"-"},		// 162
	{0,	3,	3,	""},		// 163
	{0,	4,	3,	""},		// 164
	{0,	5,	3,	""},		// 165 push
	{3,	0,	3,	""},		// 166
	{3,	0,	3,	""},		// 167
	{3,	0,	3,	""},		// 168
	{3,	1,	3,	""},		// 169
	{3,	1,	3,	""},		// 170
	{3,	1,	3,	""},		// 171
	{0,	6,	3,	""},		// 172
	{0,	7,	3,	""},		// 173
	{3,	2,	3,	""},		// 174
	{3,	4,	3,	""},		// 175
	{3,	3,	3,	""},		// 176
	{3,	5,	3,	""},		// 177
	{3,	0,	3,	""},		// 178
	{3,	0,	3,	""},		// 179
	{3,	1,	3,	""},		// 180
	{3,	1,	3,	""},		// 181
	{3,	2,	3,	""},		// 182
	{3,	2,	3,	""},		// 183
	{3,	3,	3,	""},		// 184
	{3,	3,	3,	""},		// 185
	{3,	4,	3,	""},		// 186
	{3,	4,	3,	""},		// 187
	{3,	5,	3,	""},		// 188
	{3,	5,	3,	""},		// 189
	{0,	0,	1,	"0%"},		// 190
	{2,	0,	1,	""},		// 191
	{2,	0,	1,	""},		// 192
	{2,	0,	1,	""},		// 193
	{2,	1,	1,	"SIN"},		// 194
	{2,	1,	1,	"COS"},		// 195
	{2,	1,	1,	"ATN"},		// 196
	{2,	1,	1,	"SQR"},		// 197
	{2,	1,	1,	"EXP"},		// 198
	{2,	1,	1,	"LOG"},		// 199
	{2,	1,	1,	"LOG10"},	// 200
	{2,	1,	1,	"RND"},		// 201
	{2,	1,	1,	"FIX"},		// 202
	{2,	1,	1,	"TAN"},		// 203
	{2,	1,	1,	"LEN"},		// 204
	{2,	1,	1,	"SPACE$"},	// 205
	{2,	1,	1,	"RIGHT"},	// 206
	{2,	1,	1,	"MID"},		// 207
	{2,	1,	1,	"INSTR"},	// 208
	{2,	1,	1,	"NUM$"},	// 209
	{2,	1,	1,	"VAL"},		// 210
	{2,	1,	1,	"SGN"},		// 211
	{2,	1,	1,	"INT"},		// 212
	{2,	1,	1,	"ABS"},		// 213
	{7,	2,	3,	""},		// 214
	{5,	3,	3,	"GOTO "},	// 215
	{7,	2,	3,	"IF "},		// 216
	{4,	0,	3,	""},		// 217
	{7,	2,	3,	"UNLESS "},	// 218
	{4,	0,	1,	""},		// 219
	{5,	10,	0,	"DEF"},		// 220
	{5,	1,	1,	"FNEND"},	// 221
	{5,	3,	3,	"GOSUB "},	// 222
	{5,	1,	1,	"RETURN"},	// 223
	{1,	40,	1,	"NOT "},	// 224
	{1,	30,	1,	" AND "},	// 225
	{1,	20,	1,	" OR "},	// 226
	{1,	20,	1,	" XOR "},	// 227
	{1,	10,	1,	" IMP "},	// 228
	{1,	10,	1,	" EQV "},	// 229
	{8,	0,	1,	""},		// 230
	{8,	0,	1,	""},		// 231
	{8,	0,	1,	""},		// 232
	{1,	60,	1,	"+"},		// 233
	{6,	4,	1,	""},		// 234
	{6,	1,	7,	""},		// 235
	{6,	3,	7,	""},		// 236
	{6,	1,	7,	""},		// 237
	{6,	3,	7,	""},		// 238
	{6,	0,	7,	""},		// 239
	{6,	2,	7,	""},		// 240
	{6,	0,	7,	""},		// 241
	{6,	2,	7,	""},		// 242
	{6,	4,	7,	""},		// 243
	{6,	4,	7,	""},		// 244
	{6,	5,	7,	"NEXT"},	// 245
	{6,	5,	7,	"NEXT"},	// 246
	{5,	2,	1,	"CLOSE "},	// 247
	{9,	0,	1,	""},		// 248
	{9,	0,	1,	""},		// 249	PRINT
	{9,	4,	1,	""},		// 250
	{9,	3,	1,	""},		// 251
	{9,	3,	1,	""},		// 252
	{9,	3,	1,	""},		// 253
	{9,	2,	1,	""},		// 254
	{9,	2,	1,	"2"},		// 255
	{0,	3,	1,	"("},		// 256
	{0,	1,	1,	""},		// 257
	{0,	0,	0,	0}};


static int fnn(std::string &a)
{
	int i1;
	int Result;

	i1 = (a.find("/", i) + 1);
	if (i1 == 0)
	{
		i1 = a.size() + 1;
	}
	Result = std::stoi(basic::mid(a, i + 3, i1 - 4));

	a = a.substr(0, i - 1) + basic::right(a, i1);
	return Result;
}

// Insert variable V$ (referenced by address A%) into hash table.
static void fnist(int a, std::string e)
{
	if (var.find(a) != var.end())
	{
		std::cout << "%Multiple Definition" << e << '\t' <<
			var[a] << std::endl;
	}
	else
	{
		var[a] = e;
	}
}

// Gets a variable name from symbol table at address A%
static int fng_n(void)
{
	int Result;

L_15310:;
	a_V2--;
	i = fnb(a_V2);
	if ((i & 128) == 0)
	{
		if (i)
		{
			v += std::string(1, (char)i);
		}
		if (f == 0)
		{
			if (i & i < 32)
			{
				f = (i - 10) / 2 + 1;
			}
		}
		goto L_15310;
	}
	Result = a_V2 & -2;
	return Result;
}

// Classifies variable name V$ with flag at address X% return next
// link to different type (same name), and inserts variable into hash
// table
static int fng_t(int x, std::string v)
{
	int Result;

	a_V2 = x - 2;
	Result = fnw(a_V2);
	i = fnb(x);
	if (i & 8)
	{
		x = 26;
	}
	else
	{
		if (i & 1)
		{
			x = 2;
		}
		else
		{
			if (i & 4)
			{
				x = 6;
			}
			else
			{
				x = flen * 2;
			}
		}
	}
	if (i & 1)
	{
		v += "%";
	}
	if (i & 4)
	{
		v += "$";
	}
	if (i & 8)
	{
		v += "(";
	}
	if (i & 16)
	{
		v[0]+= 32;
//		v[0] |= 128;
	}
	fnist(a_V2 - x - spda, v);
	return Result;
}

static int fnw(int a)
{
	int Result;

	Result = (unsigned int)(p[a - 512]) +
		(unsigned int)(p[a - 512 + 1]) * 256;
	// Make negative? Sign extend
	if (Result & 0x8000)
	{
		Result |= -1 << 16;
	}
	else
	{
		Result &= 0xffff;
	}
	return Result;
}

static int fnr(int a)
{
	int Result;
	if (a & 1)
	{
		Result = (fnw(a - 1) & -256) + (fnw(a + 1) & 255);
	}
	else
	{
		Result = basic::Swap(fnw(a));
	}
	Result &= 0xffff;
	if (Result & 0x8000)
	{
		Result |= -1 << 16;
	}

	return Result;
}

static int fnb(int a)
{
	return p[a - 512];
}


int main(int argc, char **argv)
{
	std::string a;
	std::string a1;
	std::string b;
	int bp2 = 0;
	std::string c;
	std::string d;
	std::string e;
	int f1 = 0;
	int i1 = 0;
	std::string job;
	int q = 0;
	std::string s1;
	int vtbl = 0;
	int x = 0;

int fd, pagesize;

	BStack(20);
	s1 = "$";
	std::cout << "UNBAC - " << '\t' << basic::Qdate(0) << "  "
		<< basic::Qtime(0) << std::endl;
	std::cout << std::endl;
	std::cout << "Type \"?\" for help." << std::endl;
	job = basic::right(std::to_string(100 + e[0] / 2), 2);
L_1010:;
	std::cout << "[]";
	getline(std::cin, a);
	a = basic::edit(a, 7);	// was 39
	if (a == "?")
	{
		goto L_2000;
	}
	if (a == "")
	{
		goto L_1010;
	}
	BGosub(L_10000);
	if (f1)
	{
		goto L_1010;
	}
	i = (a.find("=", 0) + 1);
	if (i == 0)
	{
		a1 = a;
		i = (a1.find(".", 0) + 1);
		if (i)
		{
			a1 = a1.substr(0, i - 1);
		}
		goto L_1030;
	}
	a1 = a.substr(0, i - 1);
	a = basic::right(a, i + 1);
L_1030:;
	i = (a.find(".", 0) + 1);
	if (i == 0)
	{
		a += ".bac";
	}
	i = (a1.find(".", 0) + 1);
	if (i == 0)
	{
		a1 += ".bas";
	}

	fd = open(a.c_str(), O_RDONLY);
	if (fd == -1)
	{
		std::cerr << "Unable to open '" << a << "'" << std::endl;
		exit(1);
	}
//	pagesize = getpagesize();
	pagesize = 65536;
	p = (unsigned char *)mmap((caddr_t)0, pagesize, PROT_READ,
		MAP_SHARED, fd, 0);
	if (p == 0)
	{
		std::cerr << "Unable to mmap '" << a << "'" << std::endl;
		exit(1);
	}


	sp = fnw(514);
	q = fnb(sp + 1);
	scalef = (256 - fnb(sp + 39)) & 255;
	if (q < 1 || q > 4)
	{
		std::cout << "?Version # out of range (" <<
			std::to_string(q) << ")!" <<
			std::endl;
		goto L_1010;
	}

	f1 = std::stoi(opcode[65].dsc);
	spda = fnw(sp + 28);
	flen = fnb(sp + 38);
	vtbl = spda + 1214;
	spst = fnw(sp + 24) - fnw(spda + 2);
	for (l = 65; l <= 90; l++)
	{
		x = vtbl + (l - 65) * 2;
		i = fnw(x);
		while (i & -2)
		{
			v =(char)l;
			a_V2 = x = i + x;
			i = fng_n();
			a_V2 = 0;
			while (i)
			{
				i = fng_t(a_V2 + i, v);
			}
			i = fnw(x);
		}
	}
	a_V2 = std::stod(opcode[l - 26].dsc);
	f1 = f1 ^ a_V2;
	fnist(-24, "DET");
	fnist(-60, "ERL");
	fnist(-26, "ERR");
	fnist(-58, "LINE");
	fnist(-30, "NUM");
	fnist(-34, "NUM2");
	fnist(-16, "PI");
	fnist(-54, "RECOUNT");
	fnist(-62, "STATUS");
	if (f == 0)
	{
		f = fabs(sgn(f1 - f1 / 7 * 7 + 48 - spst)) * 2;
	}
	c_def = "DEF ";
	if (bp2)
	{
		c_def = "DEF* ";
	}
	if (bp2)
	{
		opcode[8].dsc = "CCPOS";
	}
	BGosub(L_13000);

	main2(job + a1 + "=" + a);
	exit(0);


L_2000:;
	std::cout << std::endl;
	std::cout << "Command format is:  source.file=compile.file/option(s)" << std::endl;
	std::cout << "Where:\t\"source.file\" is for the output of the decompiler." << std::endl;
	std::cout << "\t\"compiled.file\" is the input to the decompiler." << std::endl;
	std::cout << "\t\"option(s)\" can be one or more of the following:" << std::endl;
	std::cout << "\t\t/N - Used to inhibit the heading that is ""normally put" << std::endl;
	std::cout << "\t\t     at the end of the first line in the "" program." << std::endl;
	std::cout << "\t      /W:n - Used to set the width of output lines."" This number is just" << std::endl;
	std::cout << "\t\t     a guideline for the program, it can and will"" be exceeded" << std::endl;
	std::cout << "\t\t     occasionally. If this option is not"" specified, the width" << std::endl;
	std::cout << "\t\t     will be set to 80." << std::endl;
	std::cout << "\t       /CO - Used to disable the automatic indenting"" of Conditionals on" << std::endl;
	std::cout << "\t\t     output (i.e. putting in a tab for each"" level of IF/THEN" << std::endl;
	std::cout << "\t\t     statement.)" << std::endl;
	std::cout << "\t\t/F - Used to disable the automatic indenting"" of FOR loops on" << std::endl;
	std::cout << "\t\t     output (i.e. outputting a tab for ""each level of FOR loop)." << std::endl;
	std::cout << "\t      /I:n - Set the maximum depth of indenting.  If"" this option is not" << std::endl;
	std::cout << "\t\t     specified, then n is set to (width-30)/8." << std::endl;
	std::cout << "\t\t/2 - Used to output BASIC-PLUS 2 compatible ""function" << std::endl;
	std::cout << "\t\t     names and definitions." << std::endl;
	goto L_1010;
L_10000:;
	f1 = 0;
	i = (a.find("/N", 0) + 1);
	if (i == 0)
	{
		i = (a.find("/n", 0) + 1);
	}
	if (i == 0)
	{
		slash_n = 0;
	}
	else
	{
		slash_n = -1;
		BGosub(L_12000);
	}
	i = (a.find("/W:", 0) + 1);
	if (i == 0)
	{
		i = (a.find("/w:", 0) + 1);
	}
	if (i == 0)
	{
		width = 80;
	}
	else
	{
		width = fnn(a);
		if (width < 40)
		{
			f1 = -1;
			std::cout << "?Width too small." << std::endl;
		}
	}
	i = (a.find("/2", 0) + 1);
	if (i == 0)
	{
		bp2 = 0;
	}
	else
	{
		bp2 = -1;
		BGosub(L_12000);
	}
	i = (a.find("/CO", 0) + 1);
	if (i == 0)
	{
		i = (a.find("/co", 0) + 1);
	}
	if (i == 0)
	{
		f_cond = 0;
	}
	else
	{
		f_cond = -1;
		BGosub(L_12000);
	}
	i = (a.find("/F", 0) + 1);
	if (i == 0)
	{
		i = (a.find("/f", 0) + 1);
	}
	if (i == 0)
	{
		f_for = 0;
	}
	else
	{
		f_for = -1;
		BGosub(L_12000);
	}
	BReturn;

L_12000:;
	i1 = (a.find("/", i) + 1);
	if (i1 == 0)
	{
		i1 = a.size() + 1;
	}
	a = a.substr(0, i - 1) + basic::right(a, i1);
	BReturn;

L_13000:;
	if (scalef >= 0 && scalef < 7)
	{
		BReturn;

	}
	std::cout << "%Program has an unusual Scale Factor" <<
		scalef << std::endl;
	std::cout << " Resetting to 0" << std::endl;
	scalef = 0;
	BReturn;
}

//*********************************************************************



// Compares FNB%(STMT%(arg1%)) with arg2%, sets EPTR%=arg1%
static int fncheck(int a, int i)
{
	int Result;

	eptr = a;
	if (fnb(stmt_V11[eptr]) != i)
	{
		Result = -1;
	}
	else
	{
		Result = 0;
	}
	return Result;
}

//
// Set up a new line number
//
static void fnnew_line(void)
{
	i_ind = 0;
	if (l_state == INLINE || l_state == NEWLINE)
	{
		if (l_state == NEWLINE)
		{
			fnprint("!");
		}
		if (slash_n == 0)
		{
			fnprint(c_trm + "\t" +
				"  ! Program name: " +
				c_fil.substr(0, (c_fil.find(".", 0) + 1) - 1) + "\t\t" +
				"Compiled with SCALE " +
				std::to_string(scalef) +
				" on V" + std::string(1, (char)fnb(sp + 2)) +
				std::string(1, (char)fnb(sp + 3)) + "." +
				std::string(1, (char)fnb(sp)) + c_trm + "\t" +
				"  ! Decompiled on " + basic::Qdate(0) +
				" at " + basic::Qtime(0));
		}
		slash_n = -1;
		fnprint(e_trm);
	}
	l_state = NEWLINE;

	//
	// Output line number
	//
	fnprint(std::to_string(l) +
		basic::Qstring(f_ind + i_ind + 1 + (t == 3), 9) + "  ");
}

// Scans from current code pointer (BCOD%) to end of code (ECOD%) ptr
// keeping track of starting addresses of Op Codes in STMT%().
// Returns when Op Code type is greater than T% or the current address
// is either the same as the top of the "modifier" stack or at the end
// Value returned:  >0 - Op Code that caused the return.
//		   -1 - At end of line
//		   -2 - At address on top of modifier stack
// EPTR% is incremented for each item put in STMT%().  STMT% is set
// to the address of the code (or address) that caused a return.
// If more data is on the line then BCOD% is automatically incremented
// to skip over the offending instruction (except for variable length
// instructions like DEF's and ON statments.
static int fnscan(int t_V16)
{
	int Result;
	int s0;
	int zop;

	s0 = s_V9[s_V8];
	if (bcod == s0)
	{
		goto L_15650;
	}
L_15610:;
	stmt = bcod;
	if (bcod >= ecod)
	{
		Result = -1;
		goto L_15690;
	}
	zop = fnb(bcod);
	bcod = bcod + opcode[zop].len;
	if (opcode[zop].typ2 <= t_V16)
	{
		eptr++;
		stmt_V11[eptr] = stmt;
		if (bcod == s0)
		{
			goto L_15650;
		}
		goto L_15610;
	}
	Result = zop;
	if (zop != 87 && zop != 214)
	{
		goto L_15690;
	}
L_15650:;
	Result = -2;
L_15690:;
	return Result;
}

// This function pulls LET statements and expressions off the
// stack (It actually only pulls off the assignment statements and
// then calls FNARG1$ to get the expression.
static std::string fnarg(void)
{
	std::string Result;
	int eptr1;
	std::string z_a;
	std::string z_e;
	int z_a_t = 0;
	int z_a_t1 = 0;

	BStack(20);
	Result = "";
	if (eptr == 0)
	{
		goto L_15790;
	}
	BGosub(L_13200);
	if (z_a_op == 249)
	{
		eptr--;
		BGosub(L_13100);
		s = s.substr(0, s.size() - 2);
		Result = std::string("PRINT ") + s + ";";
		goto L_15790;
	}
	if (z_a_t > 3)
	{
		fnerror(15710, cvtia(z_a_op));
		eptr--;
		goto L_15790;
	}
	z_a = "";
	if (z_a_t < 3)
	{
		goto L_15760;
	}
	if (z_a_t1 & 1)
	{
		fnerror(15720, cvtia(z_a_op));
		eptr--;
		goto L_15790;
	}
	eptr1 = eptr;
	eptr--;
	z_e = "";
	if (z_a_t1 & 6)
	{
		z_e = fnarg1() + ")";
		if (z_a_t1 & 4)
		{
			z_e = fnarg1() + "," + z_e;
		}
	}
	z_a = opcode[z_a_op].dsc + fnvar(fnr(stmt_V11[eptr1] + 1)) + z_e;
L_15745:;
	if (eptr == 0)
	{
		Result = z_a + "=";
		goto L_15790;
	}
L_15750:;
	BGosub(L_13200);
	if (z_a_t > 3)
	{
		fnerror(15750, cvtia(z_a_op));
		eptr--;
		Result = z_a + "=";
		goto L_15790;
	}
L_15760:;
	if (z_a_t < 3)
	{
		Result = z_a + "=" + fnarg1();
		goto L_15790;
	}
	if (z_a_op == 104)
	{
		eptr--;
		goto L_15750;
	}
	eptr1 = eptr;
	eptr--;
	e = "";
	if (z_a_t1 & 6)
	{
		e = fnarg1() + ")";
		if (z_a_t1 & 4)
		{
			e = fnarg1() + "," + e;
		}
	}
	z_a = z_a + "," + fnvar(fnr(stmt_V11[eptr1] + 1)) + e;
	goto L_15745;
	Result = z_a;
	goto L_15790;
	fnerror(15789, cvtia(z_a_op));
L_15790:;
	return Result;

L_13100:;
	s = std::string("#") + fnarg1() + ", ";
	if (s == "#0%, ")
	{
		s = "";
	}
	BReturn;

L_13200:;
	z_a_op = fnb(stmt_V11[eptr]);
	z_a_t1 = opcode[z_a_op].typ1;
	z_a_t = opcode[z_a_op].typ2;
	BReturn;

}

//
// This function converts the Reverse Polish items scanned to
// algebraic notation by scanning backwards through the list.
//
static std::string fnarg1()
{
	std::string o_V17;
	std::string Result;
	int z_a1_eptr1;
	int z_a1_n;
	int z_a1_t2;

	BStack(20);
	z_a1_l_op = fnpush(257);	// Push end-of-stack op code
L_15810:;
	z_a1_eptr1 = eptr;
	BGosub(L_13500);	// Classify the current op code pointer to by EPTR%
	if (z_a1_op == -1)
	{
		// Done or error.
		goto L_15899;
	}
	switch(z_a1_t)
	{
	case 0:
		goto L_15820;
	case 1:
		goto L_15830;
	case 2:
		goto L_15860;
	default:
		std::cerr << "ON-GOTO out of range" << std::endl; abort();	// Out of range
	}
L_15820:;
	o_V17 = opcode[fnpop()].dsc +
		fnclassify_push(stmt_V11[z_a1_eptr1]) +
		o_V17;
	while (z_a1_l_op == 256 || z_a1_l_op == 161 ||
		z_a1_l_op == 162 || z_a1_l_op == 224)
	{
		o_V17 = opcode[fnpop()].dsc + o_V17;
	}
	if (z_a1_l_op != 257)
	{
		goto L_15810;
	}
	Result = o_V17;
	goto L_15899;
L_15830:;
	z_a1_t2 = opcode[stack_V10[isp]].typ1;
	if (z_a1_t1 >= (opcode[z_a1_l_op].typ1) &&
		z_a1_op != 224 & (z_a1_t2 != z_a1_t1 | (z_a1_t2 & 1)))
	{
		z_a1_l_op = fnpush(z_a1_op);
		goto L_15810;
	}
	o_V17 = ")" + o_V17;
	a_V4 = fnpush(256);
	z_a1_l_op = fnpush(z_a1_op);
	goto L_15810;
	// Classify the current op code pointed to by EPTR%
	// No function argument paramater.
L_15860:;
	if (z_a1_t1 == 0)
	{
		goto L_15810;
	}
	a_V4 = fnpush(z_a1_eptr1) + fnpush(z_a1_op);
	BGosub(L_13500);
	if (z_a1_t | z_a1_t1 > 1)
	{
		fnerror(15860, "");
		goto L_15899;
	}
	if (z_a1_op == 81)
	{
		z_a1_n = 1;
	}
	else
	{
		if (z_a1_op == 190)
		{
			z_a1_n = 0;
		}
		else
		{
			z_a1_n = fnr(stmt_V11[eptr + 1] + 1);
		}
	}
	o_V17 = ")" + o_V17;
	a_V4 = fnpush(z_a1_n);
	while (z_a1_n & 3)
	{
		z_a1_n = fnpush(z_a1_n / 4);
		o_V17 = "," + fnarg1() + o_V17;
		z_a1_n = fnpop();
	}
	o_V17 = basic::right(o_V17, 2);
	if (fnpop() & 3)
	{
		o_V17 = std::string("(") + o_V17;
	}
	z_a1_op = fnpop();
	z_a1_t1 = opcode[z_a1_op].typ1;
	z_a1_eptr1 = fnpop();
	if (z_a1_t1 == 1)
	{
		o_V17 = opcode[z_a1_op].dsc + o_V17;
	}
	else
	{
		if (z_a1_t1 == 2)
		{
			o_V17 = fnvar(fnr(stmt_V11[z_a1_eptr1] + 1) + 4) + o_V17;
		}
	}
	z_a1_l_op = 256;
	while (z_a1_l_op == 256 || z_a1_l_op == 161 ||
		 z_a1_l_op == 162 || z_a1_l_op == 224)
	{
		o_V17 = opcode[fnpop()].dsc + o_V17;
	}
	if (z_a1_l_op != 257)
	{
		goto L_15810;
	}
	Result = o_V17;
	// We know that major group is 0.
	// Special 0%,1%,0.,1.
L_15899:;
	return Result;

	// Pulls off next item pointed to by EPTR% and classifies it for
	// FNARG1$ into the Variables -->> Z.A1.OP%  Z.A1.T%  Z.A1.T1%
	// If there is no next item, or Z.A1.T% > 2% then error is printed if
	// it exists and -1 is returned for Z.A1.OP%
L_13500:;
	if (eptr == 0)
	{
		z_a1_op = -1;
		BReturn;

	}
	z_a1_op = fnb(stmt_V11[eptr]);
	eptr--;
	z_a1_t = opcode[z_a1_op].typ2;
	z_a1_t1 = opcode[z_a1_op].typ1;
	if (z_a1_t > 2)
	{
		fnerror(13510, cvtia(z_a1_t));
		z_a1_op = -1;
	}
	BReturn;
}

// This routine returns the argument that the op code is pushing on
// the BASIC-PLUS stack.
static std::string fnclassify_push(int stmt_V18)
{
	std::string Result;
	std::string z_c_a;
	int z_c_l;
	int z_c_n;
	int z_c_op;
	int z_c_t1;

	z_c_op = fnb(stmt_V18);
	z_c_t1 = opcode[z_c_op].typ1;	// We know that major group is 0.
	if (z_c_t1 == 0)
	{
		// Special 0%,1%,0.,1.
		Result = opcode[z_c_op].dsc;
		goto L_15999;
	}
	if (z_c_t1 == 1)
	{
		// Integer immediate.
		Result = std::to_string(fnr(stmt_V18 + 1)) + "%";
		goto L_15999;
	}
	if (z_c_t1 == 2)
	{
		// Short-Floating immediate.
		z_c_a = std::to_string(cvtaf(ffil +
			cvtia(fnr(stmt_V18 + 1))) / scalef_);
		if ((z_c_a.find(".", 0) + 1) == 0)
		{
			z_c_a += ".";
		}
		Result = z_c_a;
		goto L_15999;
	}
	if (z_c_t1 == 3)
	{
		// Floating variable (possibly constant)
		Result = z_c_a = fnvar(fnr(stmt_V18 + 1));
		if (z_c_a != "")
		{
			goto L_15999;
		}
		z_c_n = fnr(stmt_V18 + 1) + spda;
		for (int loop = z_c_n; loop <= z_c_n + flen * 2 - 2; loop += 2)
		{
			z_c_a = cvtia(fnw(loop)) + z_c_a;
		}
		z_c_a = std::to_string(cvtaf(z_c_a) / scalef_);
		if ((z_c_a.find(".", 0) + 1) == 0)
		{
			z_c_a += ".";
		}
		Result = z_c_a;
		goto L_15999;
	}
	if (z_c_t1 == 4)
	{
		// Integer  variable (possibly a constant on early versions)
		Result = z_c_a = fnvar(fnr(stmt_V18 + 1));
		if (z_c_a != "")
		{
			goto L_15999;
		}
		Result = std::to_string(fnw(fnr(stmt_V18 + 1) + spda)) + "%";
		goto L_15999;
	}
	if (z_c_t1 == 5)
	{
		// String variables or constants.
		Result = z_c_a = fnvar(fnr(stmt_V18 + 1));
		if (z_c_a != "")
		{
			goto L_15999;
		}
		z_c_n = fnr(stmt_V18 + 1);
		z_c_l = fnw(z_c_n + 4 + spda) - 1;
		z_c_n = z_c_n + fnw(z_c_n + spda + 2) + spst;
		for (int loop = z_c_n; loop <= z_c_n + z_c_l; loop++)
		{
			z_c_a += (char)fnb(loop);
		}
		if ((z_c_a.find("\"", 0) + 1))
		{
			Result = "'" + z_c_a + "'";
		}
		else
		{
			Result = "\"" + z_c_a + "\"";
		}
		goto L_15999;
	}
	// Single subscript arrays (float, int, or $)
	a_V4 = fnpush(z_a1_l_op);
	if (z_c_t1 == 6)
	{
		Result = fnvar(fnr(stmt_V18 + 1)) + fnarg1() + ")";
		a_V4 = fnpop();
		goto L_15999;
		// Two subscript arrays.
	}
	z_c_a = fnpush_V1(fnarg1());
	Result = fnvar(fnr(stmt_V18 + 1)) + fnarg1() + "," + fnpop_V2() + ")";
	a_V4 = fnpop();
L_15999:;
	return Result;
}

// Pushes argument on stack
static int fnpush(int a)
{
	isp++;
	stack_V10[isp] = a;
	return a;
}

// Pops argument from stack and sets Z.A1.L.OP% & function to value.
static int fnpop(void)
{
	z_a1_l_op = stack_V10[isp];
	isp--;
	return z_a1_l_op;
}

// Pushes string onto string stack
static std::string fnpush_V1(std::string a)
{
	stsp++;
	stack[stsp] = a;
	return "";
}

// Pops string from string stack.
static std::string fnpop_V2(void)
{
	return stack[stsp--];
}

// Calls FNARG$ until stack is empty
static void fnclean_up(void)
{
	std::string z_c_a;
	int z_c_i;

	BStack(20);
	if (eptr)
	{
		z_c_i = 0;
		while (eptr)
		{
			z_c_i++;
			z_c_a = fnpush_V1(fnarg());
		}
		for (int loop = z_c_i; loop >= 1; loop -= 1)
		{
			BGosub(L_13010);
			fnprint(fnpop_V2());
		}
	}
	return;
L_13010:;
	outfile << term;
	term = "";

	if (s_V8 == 0)
	{
		if (l_state == INLINE)
		{
			outfile << c_trm + fnindent() + "\\ ";
		}
	}
	l_state = INLINE;
	BReturn;
}


// This function searches for the variable name given is address
static std::string fnvar(int a)
{
	std::string z_v_a;

	if (var.find(a) != var.end())
	{
		z_v_a = var[a];
	}
	else
	{
		z_v_a = "";
	}
	if (z_v_a[0] >= 'a')
	{
		z_v_a = "FN" + basic::edit(z_v_a, 32);
	}
	return z_v_a;
}

// Outputs data to output file or buffers it depending whether we are
// in a statement modifier or not.
static void fnprint(std::string a_V25)
{
	if (s_V8 == 0)
	{
		fnformat(a_V25);
		goto L_16499;
	}
	if (bufflg == 0)
	{
		bufflg = stsp + 1;
	}
	for (a_V4 = 1; a_V4 <= a_V25.size(); a_V4 += 32)
	{
		stsp++;
		stack[stsp] = basic::mid(a_V25, a_V4, 32);
	}
L_16499:;
}

// This function indents one <tab> for each count in I.IND% and F.IND%
static std::string fnindent(void)
{
	int z_i_i;

	z_i_i = 0;
	if (f_cond == 0)
	{
		z_i_i = i_ind;
	}
	if (f_for == 0)
	{
		z_i_i += f_ind;
	}
	return basic::Qstring(1 + z_i_i, 9);
}

// This function edits an output string to try and prevent it from
// exceeding the width specified by WIDTH%
static void fnformat(std::string b_V26)
{
	std::string z_p_a;
	int z_p_c;
	int z_p_i;
	int z_p_k;
	int z_p_l_s;
	int z_p_p;

	while (b_V26.size())
	{
		z_p_i = (b_V26.find(c_trm, 0) + 1);
		if (z_p_i == 0)
		{
			z_p_i = b_V26.size() + 1;
		}
		z_p_a = b_V26.substr(0, z_p_i - 1);
		b_V26 = basic::right(b_V26, z_p_i);
		while (z_p_a.size())
		{
			z_p_p = Ccpos(2);
			z_p_l_s = 0;
			for (z_p_k = 1; z_p_k <= z_p_a.size(); z_p_k++)
			{
				z_p_c = basic::mid(z_p_a, z_p_k, 1)[0];
				if ((z_p_c & 127) == 9)
				{
					z_p_i = 8 - (z_p_p & 7);
				}
				else
				{
					if ((z_p_c & 127) > 31)
					{
						z_p_i = 1;
					}
					else
					{
						z_p_i = 0;
					}
				}
				if (z_p_i + z_p_p > width && z_p_l_s > 1)
				{
					outfile << z_p_a.substr(0, z_p_l_s - 1) <<
						c_trm << fnindent() << "\t";
					z_p_a = basic::right(z_p_a, z_p_l_s);
					goto L_16680;
				}
				if (z_p_c > 127)
				{
					z_p_l_s = z_p_k;
				}
				z_p_p += z_p_i;
			}
			outfile << z_p_a;
			z_p_a = "";
L_16680:;
		}
		if (b_V26.substr(0, c_trm.size()) == c_trm)
		{
			outfile << c_trm;
			b_V26 = basic::right(b_V26, c_trm.size() + 1);
		}
	}
}

// Prints out error messages
static void fnerror(int i_V29, std::string e_V28)
{
	std::cout << "%Error encountered processing line" << laslin <<
		'\t' << "L=" << i_V29 << std::endl;
	while (e_V28.size())
	{
		std::cout << cvtai(e_V28) << '\t';
		e_V28 = basic::right(e_V28, 3);
	}
	std::cout << std::endl;
}

int main2(std::string i)
{
	std::string a;
	std::string b;
	int c = 0;
	int i_V6 = 0;
	int ifthen = 0;
	int j = 0;
	int lptr = 0;
	std::string modif;
	std::string o;
	int op = 0;
	int q = 0;
	std::string s_fil;
	int s1[11];
	int s2[11];
	int scth;
	int spta = 0;
	int st = 0;
	int stflg = 0;
	std::string term1;
	std::string v;

	BStack(20);

	// State constants
	i = basic::right(i, 3);
	i_V6 = (i.find("=", 0) + 1);
	s_fil = i.substr(0, i_V6 - 1);
	c_fil = basic::right(i, i_V6 + 1);
	outfile.open(s_fil.c_str());

	sp = fnw(514);
	q = fnb(sp + 1);
	spda = fnw(sp + 28);
	spta = fnw(sp + 30);
	scth = spta;
	flen = fnb(sp + 38);
	spst = fnw(sp + 24) - fnw(spda + 2);
	s2[0] = -1;
	l_state = DONE;
	lptr = scth;
	eptr = 0;
	e_trm = "\n";
	c_trm = " &\n";
	width = width - 2;
	ffil = basic::Qstring(cvtfa(0.).size() - 2, 0);
	scalef_ = pow(10., scalef);


	// New line header dispatch area.
L_2100:;
	i_V6 = fnw(lptr);
	if (i_V6)
	{
		lptr = lptr + i_V6;
		l = fnw(lptr + 10);
		t = fnb(lptr + 9);
		outfile << term;
		term = "";
		if (eptr)
		{
			if (t != 7)
			{
				fnclean_up();
			}
		}
		if (l != laslin)
		{
			fnnew_line();
		}
		laslin = l;
		bcod = fnw(lptr + 2) + lptr;
		ecod = bcod + fnw(lptr + 4);
		if (t == 7)
		{
			goto L_2500;
		}
		if (t == 1)
		{
			goto L_2300;
		}
		if (t == 4)
		{
			goto L_2400;
		}
		goto L_2500;
	}

	//
	// Finish up processing.
	//
	outfile << term;
	term = "";
	fnclean_up();
	if (l_state == NEWLINE)
	{
		outfile << "!";
	}
	outfile << std::endl;
	outfile.close();
	exit(0);

	// Handles the termination of statement modifiers
L_2200:;
	if (s_V8 == 0)
	{
		s_V8 = 1;
		stflg = 0;
		if (eptr)
		{
			fnprint(fnarg());
		}
		BGosub(L_13300);
		fnformat(o);
		fnformat(term + modif);
		modif = term = "";
		s_V8 = 0;
		goto L_2500;
	}
	else
	{
		if (s_V9[s_V8] != bcod)
		{
			goto L_2500;
		}
		else
		{
			if (s2[s_V8] == 0)
			{
				s_V8--;
				stflg = 0;
				goto L_2200;
			}
		}
	}
	op = fnb(stmt);
	if (op == 87)
	{
		goto L_2270;
	}
	if (op != 214)
	{
		s_V8--;
		goto L_2200;
	}
	l = fnr(stmt + 1) + bcod;
	if (l != s1[s_V8])
	{
		s_V8--;
		goto L_2200;
	}
	l = s2[s_V8];
	s_V8--;
	if (l < 0)
	{
		modif = modif.substr(0, modif.size() + l) +
			"UNTIL " + basic::right(modif, modif.size() + l + 8);
	}
	else
	{
		modif = modif.substr(0, modif.size() - l) +
			"WHILE " + basic::right(modif, modif.size() - l + 4);
	}
	goto L_2200;
L_2270:;
	if (eptr)
	{
		fnprint(fnarg());
	}
	stflg = 0;
	BGosub(L_13300);
	i_V6 = s2[s_V8];
	if (s_V8)
	{
		i_V6 = sgn(i_V6) * i_V6;
	}
	l = s2[s_V8 - 1];
	if (s_V8 - 1)
	{
		l = sgn(l) * l;
	}
	if (ifthen == 0)
	{
		e = fnpush_V1("");
	}
	c = -s_V8 * (~f_cond);
	o = term = fnpush_V1(basic::mid(modif, modif.size() - i_V6 + 1, i_V6 - l - 1) + " " +
		"THEN" + c_trm + fnindent() +
		basic::Qstring(c, 9) + "  " + fnpop_V2() + o + term +
		modif.substr(0, modif.size() - i_V6 - 1) + c_trm +
		fnindent() + basic::Qstring(c - 1, 9) + "  ELSE" + c_trm +
		fnindent() + basic::Qstring(c, 9) + "  ");
	s_V8--;
	modif = basic::right(modif, modif.size() - l);
	ifthen++;
	if (s_V8 == 0)
	{
		fnformat(fnpop_V2());
		ifthen = 0;
		l_state = NEWLINE;
		i_ind++;
	}
	goto L_2500;
	// Process DIM statements
L_2300:;
	BGosub(L_13010);
	l_state = NEWLINE;
	fnprint("DIM ");
	c = 0;
	for (l = bcod + 2; l <= ecod - 3; l += 2)
	{
		t = fnr(l);
		a = fnvar(t);
		t = t + spda;
		i_V6 = fnb(t + 10);
		if (i_V6 != c)
		{
			if (l_state != NEWLINE)
			{
				outfile << c_trm + fnindent() + "\\ DIM ";
			}
			if (i_V6)
			{
				outfile << std::string("#") +
					std::to_string(i_V6 / 2) + "%";
			}
			c = i_V6;
			l_state = INLINE;
		}
		if (l_state == INLINE)
		{
			outfile << ", ";
		}
		l_state = INLINE;
		e = a + std::to_string(fnw(t + 22)) + "%";
		if (fnw(t + 24))
		{
			e += "," + std::to_string(fnw(t + 24)) + "%";
		}
		e = e + ")";
		if (fnb(t + 28) & 4)
		{
			if (c)
			{
				e += "=" + std::to_string(2 * fnw(t + 20)) + "%";
			}
		}
		if (Ccpos(2) + e.size() >= width)
		{
			outfile << c_trm + fnindent() + "\t";
		}
		outfile << e;
	}
	l_state = INLINE;
	goto L_2100;
	// DATA statement processing routine
L_2400:;
	BGosub(L_13000);
	outfile << "DATA\t";
	c = 0;
	e = "";
	for (j = bcod + 1; j <= ecod - 1; j++)
	{
		i_V6 = fnb(j);
		e = e + std::string(1, (char)i_V6);
		if (i_V6 == 34)
		{
			if (c == 0)
			{
				c = 2;
			}
			else
			{
				if (c == 2)
				{
					c = 0;
				}
			}
		}
		else
		{
			if (i_V6 == 39)
			{
				if (c == 0)
				{
					c = 1;
				}
				else
				{
					if (c == 1)
					{
						c = 0;
					}
				}
			}
			else
			{
				if (i_V6 == 44 && c == 0)
				{
					if (Ccpos(2) + e.size() >= width)
					{
						outfile << c_trm << fnindent() << "\t";
					}
					outfile << e;
					e = "";
				}
			}
		}
	}
	outfile << e;
	goto L_2100;
	// "Ordinary" statement dispatch area
	// At branch address on top of stack
L_2500:;
	op = fnscan(3);
	if (op == -1)
	{
		goto L_2100;
	}
	if (op == -2)
	{
		goto L_2200;
	}
	st = opcode[op].typ1;
	t = opcode[op].typ2;
	if (t != 8 && t != 9 && op != 28)
	{
		stflg = 0;
	}
	if (t == 4)
	{
		std::cout << "%Undefined OP Code" << op <<
			"in line" << laslin << std::endl;
	}
L_2510:;
	if (t != 5)
	{
		switch(t)
		{
		case 6:
			goto L_2600;
		case 7:
			goto L_2700;
		case 8:
			goto L_2800;
		case 9:
			goto L_2900;
		default:
			std::cerr << "ON-GOTO out of range" << std::endl; abort();	// Out of range
		}
	}
	else
	{
		if (stflg & 4)
		{
			fnprint(term);
		}
		if ((stflg & ~260) == 0)
		{
			stflg = stflg & 256;
			if (op == 28 || stflg == 0)
			{
				switch(st)
				{
				case 1:
					goto L_10000;
				case 2:
					goto L_10100;
				case 3:
					goto L_10200;
				case 4:
					goto L_10300;
				case 5:
					goto L_10400;
				case 6:
					goto L_10500;
				case 7:
					goto L_10600;
				case 8:
					goto L_10700;
				case 9:
					goto L_10800;
				case 10:
					goto L_10900;
				case 11:
					goto L_11000;
				case 12:
					goto L_11100;
				case 13:
					goto L_11200;
				default:
					std::cerr << "ON-GOTO out of range "<< st  << std::endl; abort();	// Out of range
				}
			}
		}
	}
	fnerror(2520, cvtia(stflg));
	stflg = 0;
	goto L_2510;
	// For loop processing area
	// Ignore these critters
L_2600:;
	if (op == 50)
	{
		goto L_2500;
	}
	if (st == 4 || st == 5)
	{
		if (eptr)
		{
			fnclean_up();
		}
		if (st == 4)
		{
			goto L_2500;
		}
		f_ind--;
		BGosub(L_13010);
		fnprint(opcode[245].dsc);
		i_V6 = fnr(stmt + 1);
		if (i_V6)
		{
			fnprint(" " + fnvar(i_V6));
		}
		goto L_2500;
	}
	s = "";
	i_V6 = fnr(stmt + 1);
	if (i_V6)
	{
		if ((fnr(stmt + 3) & 1) == 0)
		{
			s = " STEP " + basic::right(fnarg(), 2);
		}
		if ((st & 2) == 0)
		{
			s = " TO " + basic::right(fnarg(), 2) + s;
		}
		s = "FOR " + fnarg() + s;
	}
	if (eptr)
	{
		fnclean_up();
	}
	l = stmt;
	if (st & 2)
	{
		a_V4 = fnscan(3);
		if (a_V4 != 234)
		{
			goto L_2680;
		}
		if (fnb(stmt_V11[eptr]) != 121)
		{
			s = s + std::string(sgn(s.size()), ' ') +
				"WHILE " + fnarg1();
		}
		else
		{
			eptr--;
			s = s + std::string(sgn(s.size()), ' ') +
				"UNTIL " + fnarg1();
		}
	}
	if (st & 1)
	{
		modif = " " + s + modif;
	}
	else
	{
		BGosub(L_13010);
		fnprint(s);
		f_ind++;
		goto L_2500;
	}
	if (s_V8 == 0)
	{
		BGosub(L_13000);
	}
	s_V8++;
	s_V9[s_V8] = fnr(l + 5) + bcod;
	s1[s_V8] = l;
	s2[s_V8] = 0;
	goto L_2500;
	// Expected op 234, got an A%
L_2680:;
	fnerror(2680, cvtia(a_V4));
	goto L_2500;
	// Conditionals (IF THEN/UNLESS THEN/IF/UNLESS/WHILE/UNTIL) processing
	// Unexpected else
L_2700:;
	if (st != 1)
	{
		goto L_2730;
	}
	if (op == 87)
	{
		fnerror(2700, "");
		goto L_2500;
	}
	if (fnb(stmt_V11[eptr]) != 121)
	{
		s = opcode[216].dsc + fnarg1() + " THEN ";
	}
	else
	{
		eptr--;
		s = opcode[218].dsc + fnarg1() + " THEN ";
	}
	fnclean_up();
	BGosub(L_13010);
	l_state = NEWLINE;
	i_ind++;
	fnprint(s + c_trm + fnindent() + "  ");
	goto L_2500;
	// Unexpected branch in line.
L_2730:;
	if (op == 214)
	{
		fnerror(2730, cvtia(stmt) + cvtia(s_V9[s_V8]));
		goto L_2500;
	}
	if (fnb(stmt_V11[eptr]) == 121)
	{
		eptr--;
		op = 218;
	}
	s = fnarg1();
	a_V4 = fnpush(stmt_V11[eptr + 1]);
	fnclean_up();
	BGosub(L_13000);
	s_V8++;
	s_V9[s_V8] = fnr(stmt + 1) + bcod;
	s1[s_V8] = fnpop();
	if (op == 216)
	{
		modif = std::string(" ") + opcode[216].dsc + s + modif;
		s2[s_V8] = modif.size() - 1;
		goto L_2500;
	}
	modif = std::string(" ") + opcode[218].dsc + s + modif;
	s2[s_V8] = -modif.size() + 1;
	goto L_2500;
	// Process READ statements
L_2800:;
	if (stflg & 1)
	{
		fnprint(", ");
	}
	else
	{
		BGosub(L_13000);
		fnprint("READ ");
		stflg = 1;
		// Wierd expressiong
	}
	a_V4 = fnscan(2);
	if (a_V4 < 0)
	{
		goto L_2830;
	}
	eptr++;
	stmt_V11[eptr] = stmt;
	st = opcode[a_V4].typ1;
	t = opcode[a_V4].typ2;
	if (t != 3 | st & 1)
	{
		goto L_2830;
	}
	s = fnarg();
	if (s == "")
	{
		fnerror(2810, "");
		fnprint(s);
		goto L_2500;
	}
	fnprint(s.substr(0, s.size() - 1));
	goto L_2500;
	// Illegal Op Code A%
L_2830:;
	fnerror(2830, cvtia(a_V4));
	goto L_2500;

	//
	// PRINT/PRINT USING/INPUT/INPUT LINE/FIELD
	//
L_2900:;
	stflg = stflg & -2;
	if (st == 0)
	{
		eptr++;
		stmt_V11[eptr] = stmt;
		if (stflg & 4)
		{
			fnprint(term);
		}
		if (stflg & 4)
		{
			term = term1 = "";
		}
		stflg = stflg & ~4;
		if (op != 53)
		{
			goto L_2500;
		}
		eptr--;
		stflg = 0;
		goto L_2500;
	}
	if (stflg)
	{
		goto L_2970;
	}
	if (st == 3 || st == 4)
	{
		if (fncheck(eptr, 248))
		{
			goto L_3030;
		}
		if (eptr < 7 - st)
		{
			goto L_3030;
		}
		if (st == 3)
		{
			if (fncheck(eptr - 1, 52))
			{
				goto L_3030;
			}
		}
		if (fncheck(eptr - 1, 45))
		{
			goto L_3030;
		}
		eptr--;
		BGosub(L_13100);
		BGosub(L_13000);
		BGosub(L_13800);
		if (i_V6 != a_V4 || a_V4 < 2)
		{
			goto L_3030;
		}
		e = "";
		if (st == 4)
		{
			e = "LINE ";
		}
		fnprint("INPUT " + e + s + a);
		term1 = ",";
		stflg = 2;
		if (st == 4)
		{
			stflg = 0;
		}
		goto L_2500;
		// Remove the 0% stacked on there.
	}
L_2930:;
	if (st > 4 && st != 8)
	{
		BGosub(L_13400);
		e = fnarg1();
		if (fncheck(eptr, 249))
		{
			goto L_3030;
		}
		eptr--;
		BGosub(L_13100);
		BGosub(L_13000);
		fnprint(std::string("FIELD ") + s + v);
		term = term1 = "";
		stflg = 8;
		goto L_2500;
	}
L_2940:;
	if (st != 2 && st != 8)
	{
		goto L_3030;
	}
	if (op == 110)
	{
		a = fnarg1();
		BGosub(L_13100);
		BGosub(L_13000);
		fnprint(std::string("PRINT ") + s + "RECORD " + a);
		term1 = ",";
		term = ";";
		stflg = 4;
		goto L_2500;
	}
	if (op != 1 && op != 2 && op != 67)
	{
		a = fnarg1();
	}
	if (st == 8)
	{
		e = fnarg1();
	}
	if (st == 8)
	{
		e = "USING " + fnarg1();
	}
	if (st == 8 && op != 67)
	{
		a = e + ", " + a;
	}
	if (op == 67)
	{
		a = e;
	}
	if (fncheck(eptr, 249))
	{
		goto L_3030;
	}
	if (fnb(stmt_V11[eptr - 1]) == 52)
	{
		eptr = eptr - 2;
		BGosub(L_13100);
		if (op)
		{
			goto L_3030;
		}
		BGosub(L_13000);
		if (op == 1)
		{
			a = ",";
		}
		if (op == 2)
		{
			s = s.substr(0, s.size() - 2);
		}
		fnprint("INPUT " + s + a);
		term = term1 = ";";
		if (op == 1)
		{
			term = term1 = "";
		}
		stflg = 2;
		goto L_2500;
	}
	eptr--;
	BGosub(L_13100);
	if (op == 1)
	{
		a = ",";
	}
	if (op == 2)
	{
		a = "";
	}
	BGosub(L_13000);
	if (op == 2)
	{
		s = s.substr(0, s.size() - 2);
	}
	fnprint(std::string("PRINT ") + s + a);
	term = term1 = ";";
	if (op == 1 || op == 2)
	{
		term = term1 = "";
	}
	if (st == 8)
	{
		term1 = ",";
	}
	stflg = 4;
	if (op == 2)
	{
		stflg = 0;
	}
	goto L_2500;
L_2970:;
	if (st > 4 && st != 8)
	{
		if ((stflg & 12) == 0)
		{
			goto L_3030;
		}
		if (stflg & 4)
		{
			goto L_2930;
		}
		BGosub(L_13400);
		if (eptr)
		{
			goto L_3030;
		}
		fnprint("," + v);
		goto L_2500;
	}
	if (st == 3)
	{
		if ((stflg & 2) == 0)
		{
			goto L_3030;
		}
		if (eptr)
		{
			if (fncheck(eptr, 248) == 0)
			{
				if (fncheck(eptr - 1, 52) == 0)
				{
					if (fncheck(eptr - 1, 45) == 0)
					{
						eptr--;
					}
				}
			}
		}
		BGosub(L_13800);
		if (i_V6 != a_V4 || a_V4 < 2 | eptr)
		{
			goto L_3030;
		}
		fnprint(term1 + a);
		term1 = ",";
		term = "";
		goto L_2500;
	}
	if (st != 2 && st != 8)
	{
		goto L_3030;
	}
	if ((stflg & 6) == 0)
	{
		goto L_3030;
	}
	if (op == 110)
	{
		goto L_2940;
	}
	a = "";
	if (op != 1 && op != 2 && op != 67)
	{
		a = fnarg1();
	}
	if (op == 1)
	{
		a = ",";
	}
	if (eptr == 0)
	{
		BGosub(L_13900);
		if (st == 8)
		{
			term1 = ",";
		}
		goto L_2500;
	}
	if (fncheck(eptr, 249))
	{
		goto L_3030;
	}
	if (fnb(stmt_V11[eptr - 1]) == 52)
	{
		eptr--;
	}
	eptr--;
	if (eptr)
	{
		goto L_3030;
	}
	BGosub(L_13900);
	goto L_2500;
L_3030:;
	fnerror(3030, cvtia(op) + cvtia(st) + cvtia(stflg));
	goto L_2500;
	// Process Simple Statements (containing no expressions i.e. STOP)
L_10000:;
	fnclean_up();
	stflg = 0;
	BGosub(L_13000);
	fnprint(opcode[op].dsc);
	goto L_2500;
	// Process Simple Single Expression Statements.
L_10100:;
	a = fnarg1();
	fnclean_up();
	BGosub(L_13000);
	fnprint(opcode[op].dsc + a);
	goto L_2500;
	// Process GOTO's/GOSUB's/RESUME's/ON ERROR GOTO's
L_10200:;
	l = fnr(stmt + 1);
	if (l)
	{
		l = fnw(l + spta + 10);
	}
	fnclean_up();
	BGosub(L_13000);
	e = "";
	if (op != 21 | l)
	{
		e = std::to_string(l);
	}
	fnprint(opcode[op].dsc + e);
	goto L_2500;
	// Process NAME xx AS yy statements
L_10300:;
	a = " AS " + fnarg1();
	a = std::string("NAME ") + fnarg1() + a;
	fnclean_up();
	BGosub(L_13000);
	fnprint(a);
	goto L_2500;
	// Process Two argument statements (CHAIN X$ X%)
L_10400:;
	a = fnarg1();
	a = fnarg1() + " " + a;
	fnclean_up();
	BGosub(L_13000);
	fnprint(opcode[op].dsc + a);
	goto L_2500;
	// Process ON x GOTO/ON x GOSUB statments
L_10500:;
	a = fnarg1();
	fnclean_up();
	BGosub(L_13000);
	l = fnb(stmt + 1);
	fnprint(std::string("ON ") + a + " " +
		opcode[op].dsc + std::to_string(fnw(spta + 10 + fnr(l + stmt))));
	for (i_V6 = l - 2; i_V6 >= 2; i_V6 -= 2)
	{
		fnprint("," + std::to_string(fnw(spta + 10 + fnr(i_V6 + stmt))));
	}
	bcod = bcod + l + 2;
	goto L_2500;
	// MAT statements
	// Mat PRINT
L_10600:;
	if (op == 26 || op == 27 || op == 56)
	{
		BGosub(L_13600);
		if (fncheck(eptr, 249))
		{
			goto L_10699;
		}
		eptr--;
		BGosub(L_13100);
		BGosub(L_13000);
		if (op == 26)
		{
			a = a + ",";
		}
		if (op == 27)
		{
			a = a + ";";
		}
		fnprint(opcode[op].dsc + s + b + a);
		if (fnb(bcod) != 2)
		{
			goto L_10699;
		}
		bcod++;
		goto L_2500;
	}
	// Mat A=IDN,CON,ZER,etc.
	if (op >= 29 && op <= 31)
	{
		BGosub(L_13600);
		if (a.size())
		{
			b = b.substr(0, b.size() - 1);
		}
		if (a.size())
		{
			a = std::string("(") + a;
		}
		BGosub(L_13000);
		fnprint(std::string("MAT ") + b + "=" + opcode[op].dsc + a);
		goto L_2500;
	}
	// Mat A=TRN(B),INV(B)
	if (op == 32 || op == 33)
	{
		BGosub(L_13000);
		a = fnvar(fnr(stmt + 1));
		b = fnvar(fnr(stmt + 3));
		fnprint(std::string("MAT ") +
			b.substr(0, b.size() - 1) + "=" +
			opcode[op].dsc + a.substr(0, a.size() - 1) + ")");
		goto L_2500;
	}
	// Mat A=B
	if (op == 36)
	{
		BGosub(L_13000);
		a = fnvar(fnr(stmt + 1));
		a = a.substr(0, a.size() - 1);
		b = fnvar(fnr(stmt + 3));
		b = b.substr(0, b.size() - 1);
		fnprint(std::string("MAT ") + b + "=" + a);
		goto L_2500;
	}
	// Mat A=(K)*B
	if (op == 34 || op == 35)
	{
		a = fnarg1();
		BGosub(L_13000);
		b = fnvar(fnr(stmt + 1));
		a = std::string("(") + a + ")*" + b.substr(0, b.size() - 1);
		b = fnvar(fnr(stmt + 3));
		b = b.substr(0, b.size() - 1);
		fnprint(std::string("MAT ") + b + "=" + a);
		goto L_2500;
	}
	// MAT A=B*C, B-C, B+C
	if (op >= 37 && op <= 39)
	{
		BGosub(L_13000);
		a = fnvar(fnr(stmt + 1));
		a = a.substr(0, a.size() - 1);
		b = fnvar(fnr(stmt + 3));
		b = b.substr(0, b.size() - 1);
		s = fnvar(fnr(stmt + 5));
		s = s.substr(0, s.size() - 1);
		fnprint(std::string("MAT ") + s + "=" + b + opcode[op].dsc + a);
		goto L_2500;
	}
	if (op != 25 && op != 28)
	{
		goto L_10699;
	}
	else
	{
		BGosub(L_13600);
		if (op == 25)
		{
			BGosub(L_13000);
			fnprint(opcode[op].dsc + b + a);
			goto L_2500;
		}
	}
	if (stflg & 256)
	{
		s = ", ";
	}
	else
	{
		if (fncheck(eptr, 248))
		{
			goto L_10699;
		}
		if (fncheck(eptr - 1, 45))
		{
			goto L_10699;
		}
		eptr--;
		s = "";
		BGosub(L_13100);
		s = opcode[op].dsc + s;
		BGosub(L_13000);
	}
	fnprint(s + b + a);
	stflg = stflg | 256;
	goto L_2500;
	// Mat statement error.
L_10699:;
	fnerror(10699, cvtia(op));
	goto L_2500;
	// Process OPEN statments
L_10700:;
	a = "";
	if (fnb(stmt_V11[eptr]) != 108)
	{
		a = ", MODE " + fnarg1();
		if (a == ", MODE 0%")
		{
			a = "";
		}
		goto L_10720;
	}
	eptr--;
	a = ", MODE " + fnarg1();
L_10720:;
	e = fnarg1();
	if (e != "0%")
	{
		a = ", FILESIZE " + e + a;
	}
	e = fnarg1();
	if (e != "0%")
	{
		a = ", CLUSTERSIZE " + e + a;
	}
	e = fnarg1();
	if (e != "0%")
	{
		a = ", RECORDSIZE " + e + a;
	}
	a = std::string("AS FILE ") + fnarg1() + a;
	a = std::string("OPEN ") + fnarg1() + opcode[op].dsc + a;
	BGosub(L_13000);
	fnprint(a);
	goto L_2500;
	// Process GET/PUT statments
L_10800:;
	a = "";
	e = "0%";
	if (q > 3)
	{
		e = fnarg1();
	}
	if (e != "0%")
	{
		a = ", USING " + e;
	}
	if (q > 3 || op == 102)
	{
		e = fnarg1();
	}
	if (e != "0%")
	{
		a = ", COUNT " + e;
	}
	if (fnb(stmt_V11[eptr]) == 123)
	{
		eptr--;
		a = ", BLOCK " + fnarg1() + a;
		goto L_10820;
	}
	if (q > 3)
	{
		e = fnarg1();
	}
	e = fnarg1();
	if (e != "0%")
	{
		a = ", RECORD " + e + a;
	}
L_10820:;
	a = opcode[op].dsc + fnarg1() + a;
	fnclean_up();
	BGosub(L_13000);
	fnprint(a);
	if (q < 4)
	{
		bcod--;
	}
	goto L_2500;
	// Process Function DEF's
L_10900:;
	fnclean_up();
	BGosub(L_13000);
	l = fnw(fnr(stmt + 1) + spda + 2);
	t = 2;
	while (l & 3)
	{
		t = t + 2;
		l = l / 4;
	}
	s = ")";
	for (l = 4; l <= t; l += 2)
	{
		s = "," + fnvar(fnr(stmt + l)) + s;
	}
	s = basic::right(s, 2);
	if (t != 2)
	{
		s = std::string("(") + s;
	}
	fnprint(c_def + fnvar(fnr(stmt + 1) + 4) + s);
	bcod = bcod + t + 2;
	goto L_2500;
L_11000:;
	if (op != 10)
	{
		fnclean_up();
	}
	goto L_2500;
	// Process Change statements
L_11100:;
	a = fnvar(fnr(stmt + 1));
	a = a.substr(0, a.size() - 1);
	if (op == 23)
	{
		a = fnarg1() + " TO " + a;
	}
	else
	{
		a += " TO ";
		fnclean_up();
		a_V4 = fnscan(2);
		if (a_V4 < 0)
		{
			goto L_11120;
		}
		eptr++;
		stmt_V11[eptr] = stmt;
		e = fnarg();
		a = a + e.substr(0, e.size() - 1);
	}
L_11110:;
	fnclean_up();
	BGosub(L_13000);
	fnprint(std::string("CHANGE ") + a);
	goto L_2500;
	// Illegal op code A% in CHANGE
L_11120:;
	fnerror(11120, cvtia(a_V4));
	goto L_11110;
L_11200:;
	exit(0);
L_13000:;
	outfile << term;
	term = "";
	if (eptr)
	{
		fnclean_up();
	}
	goto L_13020;
L_13010:;
	outfile << term;
	term = "";
L_13020:;
	if ((s_V8 == 0) && (l_state == INLINE))
	{
		outfile << c_trm + fnindent() + "\\ ";
	}
	l_state = INLINE;
	BReturn;

L_13100:;
	s = std::string("#") + fnarg1() + ", ";
	if (s == "#0%, ")
	{
		s = "";
	}
	BReturn;

L_13300:;
	o = "";
	if (bufflg == 0)
	{
		BReturn;

	}
	for (a_V4 = bufflg; a_V4 <= stsp; a_V4++)
	{
		o = o + stack[a_V4];
	}
	stsp = bufflg - 1;
	bufflg = 0;
	BReturn;

	// Get Length and Variable for field statments.
L_13400:;
	v = fnvar(fnr(stmt + 1));
	if (st > 5)
	{
		a = fnarg1() + ")";
		if (st == 7)
		{
			a = fnarg1() + "," + a;
		}
		v = v + a;
	}
	v = fnarg1() + " AS " + v;
	BReturn;

	// Routine to process the dimensions that are stacked for MAT statements
L_13600:;
	s = "," + fnarg1();
	b = fnarg1();
	if (s == ",0%")
	{
		s = "";
	}
	if (s == ",(NOT 0%)" && b == "0%")
	{
		b = s = "";
	}
	a = b + s;
	b = fnvar(fnr(stmt + 1));
	if (a.size() == 0)
	{
		b = b.substr(0, b.size() - 1);
	}
	if (a.size())
	{
		a = a + ")";
	}
	BReturn;

	// Get Variable name for input statments.
L_13800:;
	a_V4 = fnscan(2);
	eptr++;
	stmt_V11[eptr] = stmt;
	a = fnarg();
	a_V4 = a.size();
	i_V6 = (a.find("=", 0) + 1);
	a = a.substr(0, i_V6 - 1);
	BReturn;

	// Print data
L_13900:;
	if (op == 1 || op == 2)
	{
		term = term1 = "";
	}
	if (op != 67)
	{
		fnprint(term1 + a);
	}
	if (op != 1 && op != 2)
	{
		term = term1 = ";";
	}
	BReturn;
}
