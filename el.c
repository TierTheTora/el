extern int  openat(int, char*, int, int);
extern void close(int);
extern int  write(int, void*, int);
extern int  read(int, void*, int);

#include "el.h"

int
main (int argc, char **argv)
{
	signed   int fd, i;
	unsigned char ch, *pp, *end, p[p_C];
	unsigned short r[r_C];
	unsigned long long R[R_C];

	memset(p, 0, p_C);

	if (argc != 2) return 1;

	fd = openat(-100, argv[1], 0, 0);
	if (fd < 0) return 1;

	i = 0;
	while (read(fd, &ch, 1) == 1) {
		p[i] = ch;
		i++;
	}

	pp = p;
	end = p+i;
	
	close(fd);

	return interpret(pp, end, p);
}
