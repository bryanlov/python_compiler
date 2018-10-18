class Student:
	all_students = ["Jerry", "is", "the", "man"]
	
	def __init__(self, name, has_name = True):
		self.name = name
		self.has_name= has_name
	def pass_ball(self, other_student):
		if self.has_name:
			self.has_name = False
			other_student.has_name = True
			return True
		else:
			return False

kai = Student("kai")
print Student.all_students

Student("bryan")
print Student.all_students

print kai.has_name
print Student.all_students[0]
