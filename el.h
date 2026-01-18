void *
memset (void *dest, int c, long unsigned int n) {
	unsigned char *_dest = dest;
	while (n-- > 0)
		*_dest++ = (unsigned char)c;
	return dest;
}

enum {
	R_C = 8,
	r_C = 8,
	p_C = 1000,
	c_C = 1000,
	o_C = 20
};

long
syscall (long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	long ret;

	register long r10 __asm__("r10") = a4;
	register long r8  __asm__("r8")  = a5;
	register long r9  __asm__("r9")  = a6;

	asm volatile (
		"syscall"
		: "=a"(ret)
		: "a"(n),
		  "D"(a1),
		  "S"(a2),
		  "d"(a3),
		  "r"(r10),
		  "r"(r8),
		  "r"(r9)
		: "rcx", "r11", "memory"
	);

	return ret;
}

int
itoa (unsigned long long n, char *s, int len)
{
	if (n == 0) {
		s[--len] = '0';
		return len;
	}

	while (n > 0 && len > 0) {
		s[--len] = (n % 10) + '0';
		n /= 10;
	} 

	return len;
}

int
interpret (unsigned char *pp, unsigned char *end, unsigned char *p)
{
	unsigned int i, ptr, loop, len;
	unsigned char ch, num[2], n64[8], *str, where, whe64, o[o_C], c[c_C];
	unsigned short r[r_C];
	unsigned long long R[R_C];

	memset(c, 0, c_C);
	memset(r, 0, r_C * sizeof *r);
	memset(R, 0, R_C * sizeof *R);

	ptr = 0;
	i = 0;
	
	while (pp < end)
	{
		ch = *pp;
		if (ch == 'r') {
			if (++pp >= end) return 1;
			where = *pp;
			if (where >= r_C) return 1;
			if (++pp >= end) return 1;
			num[0] = *pp;
			if (++pp >= end) return 1;
			num[1] = *pp;
			r[where] = (num[0] << 8) | num[1];
		}
		else if (ch == 'R') {
			if (++pp >= end) return 1;
			if (*pp == '&') {
				if (++pp >= end) return 1;
				if (*pp >= R_C) return 1;
				str = &c[ptr];
				R[*pp] = (long long)str;
			}
			else if (*pp == 'l') {
				if (++pp >= end) return 1;
				where = *pp;
				if (where >= R_C) return 1;
				for (i = 0; i < 8; i++) {
					if (++pp >= end) return 1;
					n64[i] = *pp;
				}
				R[where] =
					((unsigned long long)n64[0] << 56) |
					((unsigned long long)n64[1] << 48) |
					((unsigned long long)n64[2] << 40) |
					((unsigned long long)n64[3] << 32) |
					((unsigned long long)n64[4] << 24) |
					((unsigned long long)n64[5] << 16) |
					((unsigned long long)n64[6] << 8)  |
					((unsigned long long)n64[7]);
			}
			else if (*pp == 'r') {
				if (++pp >= end) return 1;
				whe64 = *pp;
				if (whe64 >= R_C) return 1;
				if (++pp >= end) return 1;
				where = *pp;
				if (where >= r_C) return 1;
				R[whe64] = r[where];
			}
			else if (*pp == 'R') {
				if (++pp >= end) return 1;
				whe64 = *pp;
				if (whe64 >= R_C) return 1;
				if (++pp >= end) return 1;
				where = *pp;
				if (where >= R_C) return 1;
				R[whe64] = R[where];
			}
			else return 1;
		}
		else if (ch == '+') {
			if (++pp >= end) return 1;
			num[0] = *pp;
			c[ptr] += num[0];
		}
		else if (ch == '-') {
			if (++pp >= end) return 1;
			num[0] = *pp;
			c[ptr] -= num[0];
		}
		else if (ch == '>') {
			if (++pp >= end) return 1;
			num[0] = *pp;
			if (++pp >= end) return 1;
			num[1] = *pp;
			ptr += (num[0] << 8) | num[1];
		}
		else if (ch == '<') {
			if (++pp >= end) return 1;
			num[0] = *pp;
			if (++pp >= end) return 1;
			num[1] = *pp;
			ptr -= (num[0] << 8) | num[1];
		}
		else if (ch == '.') write(1, c+ptr, 1);
		else if (ch == ',') read(0, c+ptr, 1);
		else if (ch == '[') {
			if (c[ptr] == 0){
				loop=1;
				while (loop > 0 && ++pp < end) {
					if(*pp == '[') loop++;
					else if(*pp == ']') loop--;
				}
			}
		}
		else if (ch == ']') {
			if(c[ptr] != 0) {
				loop=1;
				while(loop > 0 && --pp >= p) {
					if (*pp == '[') loop--;
					else if (*pp == ']') loop++;
				}
			}
		}
		else if (ch == '@') {
			if (++pp >= end) return 1;
			if (*pp >= r_C) return 1;
			r[*pp] = c[ptr];
		}
		else if (ch == 'p') {
			if (++pp >= end) return 1;
			if (*pp == 'R') {
				if (++pp >= end) return 1;
				if (*pp >= R_C) return 1;
				c[ptr] = R[*pp];
			}
			else if (*pp == 'r') {
				if (++pp >= end) return 1;
				if (*pp >= r_C) return 1;
				c[ptr] = r[*pp];
			}
		}
		else if (ch == '#') {
			if (++pp >= end) return 1;
			if (*pp == 'R') {
				if (++pp >= end) return 1;
				if (*pp >= R_C) return 1;
				memset(o, 0, o_C);
				len = itoa(R[*pp], o, o_C);
				write(1, o + len, o_C - len);
			}
			else if (*pp == 'r') {
				if (++pp >= end) return 1;
				if (*pp >= r_C) return 1;
				memset(o, 0, o_C);
				len = itoa(r[*pp], o, 5);
				write(1, o + len, 5 - len);
			}
			else if (*pp == 'c') {
				memset(o, 0, o_C);
				len = itoa(c[ptr], o, 3);
				write(1, o + len, 3 - len);
			}
			else return 1;
		}
		else if (ch == 'x') {
			r[7] = syscall
			(r[0], r[1], r[2], r[3], r[4], r[5], r[6]);
		}
		else if (ch == 'X') {
			R[7] = syscall
			(R[0], R[1], R[2], R[3], R[4], R[5], R[6]);
		}

		pp++;
		if (ptr >= c_C) return 1;
		if (ptr < 0)   return 1;
	}

	return 0;
}
