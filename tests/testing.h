#ifndef __mette_testing_included
#define __mette_testing_included

extern void __test_fail(const char *test_name, const char *cond);
extern void __test_fail_msg(const char *test_name, const char *text, ...);

#define expect_cond(cond) \
	((expr) ? (void)0 : __test_fail(__TEST_NAME, #expr))

#define expect_eq_int(a,b) \
	((a) == (b) ? (void)0 : __test_fail_msg(__TEST_NAME, "Expectation failed: %s is not equal to %s.", #a, #b))

#endif // __mette_testing_included
