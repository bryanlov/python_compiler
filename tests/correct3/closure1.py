def function(n):
	def function1(x):
		def function2(y):
			return x + y + n
		x = x + 5
		return function2
	n = n + 2
	return function1

x = function(2)
y = x(4)
z = y(9)

print z
