#include <stdarg.h>
#include <string.h> 

#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int _i = 0;

	while (is_digit(**s))
		_i = ((_i << 3) + (_i << 1)) + ((*((*s)++)) ^ 48);
	return _i;
}

#define ZEROPAD	1
#define SIGN	2
#define PLUS	4
#define SPACE	8
#define LEFT	16
#define SPECIAL	32
#define SMALL	64

#define do_div(n, base) ({\
int __res;\
__asm__("divl %4":"=a" (n),"=d" (__res):"0" (n),"1" (0),"r" (base));\
__res; })

static char *number(char *str, int num, int base, int size, int precision, int type)
{
	char c, sign, tmp[36];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int _i;
	
	(type & SMALL) && (digits = "0123456789abcdefghijklmnopqrstuvwxyz");
	(type & LEFT) && (type &= ~ZEROPAD);
	
	if (base < 2 || base > 36)	return 0;
	
	c = (type & ZEROPAD) ? '0' : ' ' ;
	(type & SIGN && num < 0) && (
		sign = '-', num = -num, true
	) || (
		sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0)
	);
	
	(sign) && (--size);
	(type & SPECIAL) && (
		(base == 16) && (
			size -= 2, true
		) || (
			(base == 8) && (--size)
		)
	);
	
	_i = 0;
	if (num == 0)
		tmp[_i++] = '0';
	else while (num != 0)
		tmp[_i++] = digits[do_div(num, base)];
	
	(_i > precision) && (precision = _i);
	size -= precision; 
    
	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0)	*str++ = ' ';
	
	(sign) && (*str++ = sign);
	(type & SPECIAL) && (
		(base == 8) && (
			*str++ = '0', true
		) || (
			(base == 16) && (
				*str++ = '0',
				*str++ = digits[33]
			)
		)
	);
	
	if (!(type & LEFT))
		while (size-- >0)	*str++ = c;
    
	while (_i < precision--)	*str++ = '0';
	while (_i-- > 0)			*str++ = tmp[_i];
    while(size-->0)				*str++ = ' ';
    
	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str, *s;
	int len, _i, flags, field_width, precision, qualifier, *ip;

	for (str = buf; *fmt; ++fmt)
	{
		if (*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}
		flags = 0;
		repeat:
			++fmt;
			switch (*fmt)
			{
				case '-': flags |= LEFT;	goto repeat;
				case '+': flags |= PLUS;	goto repeat;
				case ' ': flags |= SPACE;	goto repeat;
				case '#': flags |= SPECIAL;	goto repeat;
				case '0': flags |= ZEROPAD;	goto repeat;
			}
		
		field_width = -1;
		(is_digit(*fmt)) && (
			field_width = skip_atoi(&fmt), true
		) || (
			(*fmt == '*') && (
				field_width = va_arg(args, int),
				(field_width < 0) && (
					field_width = -field_width,
					flags |= LEFT
				)
			)
		);
		
		precision = -1;
		(*fmt == '.') && (
			++fmt,
			(is_digit(*fmt)) && (
				precision = skip_atoi(&fmt), true
			) || (
				(*fmt == '*') && (
					precision = va_arg(args, int)
				)
			),
			(precision < 0) && (precision = 0)
		);

		qualifier = -1;
		(*fmt == 'h' || *fmt == 'l' || *fmt == 'L') && (
			qualifier = *fmt, ++fmt
		);

		switch (*fmt) {
			case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)	*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)		*str++ = ' ';
			break;

		case 's':
			s = va_arg(args, char*);
			len = strlen(s);
			(precision < 0) && (
				precision = len, true
			) || (
				(len > precision) && (
					len = precision
				)
			);

			if (!(flags & LEFT))
				while (len < field_width--)	*str++ = ' ';
			for (_i = 0; _i < len; ++_i)		*str++ = *s++;
			while (len < field_width--)		*str++ = ' ';
			break;

		case 'o':
			str = number(str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			break;

		case 'p':
			(field_width == -1) && (
				field_width = 8,
				flags |= ZEROPAD
			);
			str = number(
				str, (unsigned long) 	va_arg(args, void*), 
				16, field_width, precision, flags
			);
			break;

		case 'x':
			flags |= SMALL;
		case 'X':
			str = number(
				str, va_arg(args, unsigned long), 16,
				field_width, precision, flags
			);
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			str = number(
				str, va_arg(args, unsigned long), 10,
				field_width, precision, flags
			);
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);
			break;

		default:
			(*fmt != '%') && (*str++ = '%');
			(*fmt) && (*str++ = *fmt, true) || (--fmt);
			break;
		}
	}
	
	*str = '\0';
	return str - buf;
}

