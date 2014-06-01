#ifndef __bool_true_false_are_defined
#define	__bool_true_false_are_defined	1

#define	false	0
#define	true	1

#define	bool	_Bool
#if __STDC_VERSION__ < 199901L && __GNUC__ < 3 && !defined(__INTEL_COMPILER)
typedef	int	_Bool;
#endif

#endif /* __bool_true_false_are_defined */
