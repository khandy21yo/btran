//
// Source: Standard Input
// Translated from Basic to C++ using btran
// on Friday, May 24, 2019 at 23:59:57
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "basicfun.h"



int main(int argc, char **argv)
{
	int c = 0;	// Cells filled in
	int h = 0;
	int i = 0;
	int j = 0;
	int q = 0;
	int r = 0;
	int s = 0;
	int v = 0;
	int x;
	int z = 0;

	int w[26][26] = { 0 };		// Cells used
	int v_V1[26][26] = { 0 };	// Walls (0=none, 1=bottom,
       					//  2=right, 4=both)

	// ARKABLE PROGRAM BY JACK HAUBER, LOOMIS SCHOOL, WINDS OR , CT
	// ODELED FOR B AS IC-PLUS BY DAVE AHL, DIGITAL
	std::cout <<
		"THIS PROGRAM PRINTS OUT A MAZE OF DIMENSIONS UP TO 23 X 23."
		<< std::endl;
	std::cout << "RUN THE PROGRAM AND THEN TRY TO NEGOTIATE THE MAZE!" <<
		std::endl;
	std::cout << std::endl;
	srand(time(0));
L_120:;
	std::cout << "YOUR WIDTH? ";
	std::cin >> h;
	if (std::cin.fail()) { std::cin.clear(); 
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
	std::cout << "AND LENGTH? ";
	std::cin >> v;
	if (std::cin.fail()) { std::cin.clear(); 
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
	std::cout << std::endl;
	if (h < 2 || h > 23 || v < 2 || v > 23)
	{
		std::cout << "MEANINGLESS DIMENSIONS, TRY AGAIN" << std::endl;
		std::cout << std::endl;
		goto L_120;
	}

	std::cout << std::endl;
	std::cout << std::endl;
	q = 0;
	z = 0;
	x = (rand() % h + 1);
	for (i = 1; i <= h; i++)
	{
		if (i == x)
		{
			std::cout << ":  ";
		}
		else
		{
			std::cout << ":--";
		}
	}
	std::cout << ":" << std::endl;
	c = 1;
	w[x][1] = c;
	c = c + 1;
	r = x;
	s = 1;
	goto L_260;
L_210:;
	if (r != h)
	{
		r = r + 1;
	}
	else if (s != v)
	{
		r = 1;
		s = s + 1;
	}
	else
	{
		r = 1;
		s = 1;
	}
L_250:;
	if (w[r][s] == 0)
	{
		goto L_210;
	}
L_260:;
	if (r - 1 == 0)
	{
		goto L_530;
	}
	if (w[r - 1][s] != 0)
	{
		goto L_530;
	}
	if (s - 1 == 0)
	{
		goto L_390;
	}
	if (w[r][s - 1] != 0)
	{
		goto L_390;
	}
	if ((r != h) && (w[r + 1][s] == 0))
	{
		switch (rand() % 3)
		{
		case 0:
			goto L_790;
		case 1:
			w[r][s - 1] = c;
			goto L_830;
		case 2:
			goto L_860;
		}
	}

	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_370;
		}
	}
	else if (z == 1)
	{
		goto L_370;
	}
	else
	{
		q = 1;
	}

	switch(rand() % 3)
	{
	case 0:
		goto L_790;
	case 1:
		w[r][s - 1] = c;
		goto L_830;
	case 2:
		goto L_910;
	}
L_370:;
	switch(rand() % 2)
	{
	case 0:
		goto L_790;
	case 1:
		w[r][s - 1] = c;
		goto L_830;
	}
L_390:;
	if (r == h)
	{
		goto L_470;
	}
	if (w[r + 1][s] != 0)
	{
		goto L_470;
	}
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_450;
		}
	}
	else
	{
		if (z == 1)
		{
			goto L_450;
		}
		q = 1;
	}

	switch(rand() % 3)
	{
	case 0:
		goto L_790;
	case 1:
		goto L_860;
	case 2:
		goto L_910;
	}
L_450:;
	switch(rand() % 2)
	{
	case 0:
		goto L_790;
	case 1:
		goto L_860;
	}
L_470:;
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_790;
		}
		goto L_500;
	}
	if (z == 1)
	{
		goto L_790;
	}
	q = 1;
	goto L_500;
L_500:;
	switch(rand() % 2)
	{
	case 0:
		goto L_790;
	case 1:
		goto L_910;
	}
L_530:;
	if (s - 1 == 0)
	{
		goto L_670;
	}
	if (w[r][s - 1] != 0)
	{
		goto L_670;
	}
	if (r == h)
	{
		goto L_610;
	}
	if (w[r + 1][s] != 0)
	{
		goto L_610;
	}
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_590;
		}
		goto L_570;
	}
	if (z == 1)
	{
		goto L_590;
	}
	q = 1;
L_570:;
	switch(rand() % 3)
	{
	case 0:
		w[r][s - 1] = c;
		goto L_830;
	case 1:
		goto L_860;
	case 2:
		goto L_910;
	}
L_590:;
	switch(rand() % 2)
	{
	case 0:
		w[r][s - 1] = c;
		goto L_830;
	case 1:
		goto L_860;
	}
L_610:;
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			w[r][s - 1] = c;
			goto L_830;
		}
	}
	else if (z == 1)
	{
		w[r][s - 1] = c;
		goto L_830;
	}
	else
	{
		q = 1;
	}

	switch(rand() % 2)
	{
	case 0:
		w[r][s - 1] = c;
		goto L_830;
	case 1:
		goto L_910;
	}
L_670:;
	if (r == h)
	{
		goto L_740;
	}
	if (w[r + 1][s] != 0)
	{
		goto L_740;
	}
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_860;
		}
		switch(rand() % 2)
		{
		case 0:
			goto L_860;
		case 1:
			goto L_910;
		}
	}
	if (z == 1)
	{
		goto L_860;
	}
	q = 1;
	goto L_830;
L_740:;
	if (s != v)
	{
		if (w[r][s + 1] != 0)
		{
			goto L_210;
		}
		goto L_910;
	}
	if (z == 1)
	{
		goto L_210;
	}
	q = 1;
	goto L_910;
L_790:;
	w[r - 1][s] = c;
	c = c + 1;
	v_V1[r - 1][s] = 2;
	r = r - 1;
	if (c == h * v + 1)
	{
		goto L_1010;
	}
	q = 0;
	goto L_260;
L_830:;
	c = c + 1;
	v_V1[r][s - 1] = 1;
	s = s - 1;
	if (c == h * v + 1)
	{
		goto L_1010;
	}
	q = 0;
	goto L_260;
L_860:;
	w[r + 1][s] = c;
	c = c + 1;
	if (v_V1[r][s] == 0)
	{
		v_V1[r][s] = 2;
	}
	else
	{
		v_V1[r][s] = 3;
	}

	r = r + 1;
	if (c == h * v + 1)
	{
		goto L_1010;
	}
	goto L_530;
L_910:;
	if (q == 1)
	{
		z = 1;
		if (v_V1[r][s] == 0)
		{
			v_V1[r][s] = 1;
			q = 0;
			r = 1;
			s = 1;
			goto L_250;
		}
		else
		{
			v_V1[r][s] = 3;
			q = 0;
			goto L_210;
		}
	}
	w[r][s + 1] = c;
	c = c + 1;
	if (v_V1[r][s] == 0)
	{
		v_V1[r][s] = 1;
	}
	else
	{
		v_V1[r][s] = 3;
	}
	s = s + 1;
	if (c == h * v + 1)
	{
		goto L_1010;
	}
	goto L_260;
L_1010:;
	for (j = 1; j <= v; j++)
	{
		std::cout << "I";
		for (i = 1; i <= h; i++)
		{
			if ((v_V1[i][j] & 2) == 0)
			{
				std::cout << "  I";
			}
			else
			{
				std::cout << "   ";
			}
		}
		std::cout << std::endl;
		for (i = 1; i <= h; i++)
		{
			if ((v_V1[i][j] & 1) == 0)
			{
				std::cout << ":--";
			}
			else
			{
				std::cout << ":  ";
			}
		}
		std::cout << ":" << std::endl;
	}

	return EXIT_SUCCESS;
}
