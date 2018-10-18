def func1(x):
	return x

def func2(x, y):
	return y + func1(x)

z = 10
w = 5
a = func2(z, w)

print a