class Kai:
	name = ("string", "char")


	def __init__(self, first, last):
		self.name = (first, last)
		print "shaking chair"
		print self.name

	def lift(self):
		print self.name
		print Kai.name

lilKai = Kai("Kai", "Zhao")

lilKai.lift()

def sleep(kai):
	x = "12"
	print kai.name
	print "sleeps for " + x + " hours"

sleep(lilKai)

print "wotman"
