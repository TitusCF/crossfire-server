/*
 * static char *rcsid_sqrt_c =
 *   "$Id$";
 */

/*
 * Based on (n+1)^2 = n^2 + 2n + 1
 * given that	1^2 = 1, then
 *		2^2 = 1 + (2 + 1) = 1 + 3 = 4
 * 		3^2 = 4 + (4 + 1) = 4 + 5 = 1 + 3 + 5 = 9
 * 		4^2 = 9 + (6 + 1) = 9 + 7 = 1 + 3 + 5 + 7 = 16
 *		...
 * In other words, a square number can be express as the sum of the
 * series n^2 = 1 + 3 + ... + (2n-1)
 */
int
isqrt(n)
int n;
{
	int result, sum, prev;
	result = 0;
	prev = sum = 1;
	while (sum <= n) {
		prev += 2;
		sum += prev;
		++result;
	}
	return result;
}
