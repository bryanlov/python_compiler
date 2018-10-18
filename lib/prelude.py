# The standard prelude.

# NOTE: You may want to phase this in gently, with just a portion of the
#       definitions at first, to keep the sizes of your trees under control
#       during development.

# The builtin classes have no __init__ method, and so cannot be allocated
# directly.  Instances are created by other constructs.

class str:
    pass

class int:
    pass

class bool:
    pass

class range:
    pass

class list of [$T]:
    pass

class dict of [$Key, $Value]:
    pass

class tuple0:
    pass

class tuple1 of [$T0]:
    pass

class tuple2 of [$T1, $T2]:
    pass

class tuple3 of [$T1, $T2, $T3]:
    pass


# Type bool

# truth replaces bool(...) in regular Python.  truth(x) is True iff x is a
# true value, according to the Python spec.

def truth(x)::bool:
    native "__truth__"

def __not__(x)::bool:
    native "__not__"

# Type range

def xrange(low::int, high::int)::range:
    native "__xrange__"

def len(r::range)::int:
    native "__len__range__"

# Type int

def __add__(x:: int, y::int)::int:
    native "__add__int__"

def __sub__(x::int, y::int)::int:
    native "__sub__int__"

def __mul__(x::int, y::int)::int:
    native "__mul__int__"

def __floordiv__(x::int, y::int)::int:
    native "__floordiv__int__"

def __mod__(x::int, y::int)::int:
    native "__mod__int__"

def __pow__(x::int, y::int)::int:
    native "__pow__int__"

def __neg__(x::int)::int:
    native "__neg__int__"

def __pos__(x::int)::int:
    native "__pos__int__"

def __lt__(x::int, y::int)::bool:
    native "__lt__int__"

def __gt__(x::int, y::int)::bool:
    native "__gt__int__"

def __le__(x::int, y::int)::bool:
    native "__le__int__"

def __ge__(x::int, y::int)::bool:
    native "__ge__int__"

def __eq__(x::int, y::int)::bool:
    native "__eq__int__"

def __ne__(x::int, y::int)::bool:
    native "__ne__int__"

# toint is the replacement for 'int' as a function in our dialect.
def toint(x::str)::int:
    native "__toint__str__"

# Type str

def __add__(x:: str, y::str)::str:
    native "__add__str__"

def __mul__(x::str, y::int)::str:
    native "__lmul__str__"

def __mul__(x::int, y::str)::str:
    native "__rmul__str__"

def __lt__(x::str, y::str)::bool:
    native "__lt__str__"

def __gt__(x::str, y::str)::bool:
    native "__gt__str__"

def __le__(x::str, y::str)::bool:
    native "__le__str__"

def __ge__(x::str, y::str)::bool:
    native "__ge__str__"

def __eq__(x::str, y::str)::bool:
    native "__eq__str__"

def __ne__(x::str, y::str)::bool:
    native "__ne__str__"

def __getitem__(S::str, k::int)::str:
    native "__getitem__str__"

def __getslice__(S::str, L::int, U::int)::str:
    native "__getslice__str__"

def len(S::str)::int:
    native "__len__str__"

# tostr is the replacement for 'str' as a function in our dialect.
def tostr(x)::str:
    native "__tostr__"

# Type list

def __getitem__(S::list of $a, k::int)::$a:
    native "__getitem__list__"

def __getslice__(S::list of $a, L::int, U::int)::list of $a:
    native "__getslice__list__"

def __setitem__(S::list of $a, k::int, val::$a)::$a:
    native "__setitem__list__"

def __setslice__(S::list of $a, a::int, b::int, val::list of $a)::list of $a:
    native "__setslice__list__"

def len(S::list of $a)::int:
    native "__len__list__"

# Replaces sys.argv
def argv()::list of str:
    native "__argv__"

# Type file

def readline()::str:
    native "__readline__"

def read()::str:
    native "__read__"

# Type dict

def __getitem__(D::dict of [int, $b], x::int)::$b:
    native "__getitem__dict__int__"

def __getitem__(D::dict of [bool, $b], x::bool)::$b:
    native "__getitem__dict__bool__"

def __getitem__(D::dict of [str, $b], x::str)::$b:
    native "__getitem__dict__str__"

def __setitem__(S::dict of [int, $b], k::int, val::$b)::$b:
    native "__setitem__dict__int__"

def __setitem__(S::dict of [bool, $b], k::bool, val::$b)::$b:
    native "__setitem__dict__bool__"

def __setitem__(S::dict of [str, $b], k::str, val::$b)::$b:
    native "__setitem__dict__str__"

def len(D::dict of [int, $b])::int:
    native "__len__dict__int__"

def len(D::dict of [bool, $b])::int:
    native "__len__dict__bool__"

def len(D::dict of [str, $b])::int:
    native "__len__dict__str__"

# Backwards from regular Python

def __in__(x::int, D::dict of [int, $b])::bool:
    native "__contains__dict__int__"

def __in__(x::bool, D::dict of [bool, $b])::bool:
    native "__contains__dict__bool__"

def __in__(x::str, D::dict of [str, $b])::bool:
    native "__contains__dict__str__"

# Backwards from regular Python

def __notin__(x::int, D::dict of [int, $b])::bool:
    native "__notcontains__dict__int__"

def __notin__(x::bool, D::dict of [bool, $b])::bool:
    native "__notcontains__dict__bool__"

def __notin__(x::str, D::dict of [str, $b])::bool:
    native "__notcontains__dict__str__"


# General

def __is__(x, y)::bool:
    native "__is__"

def __isnot__(x, y)::bool:
    native "__isnot__"


# Special definitions

def __choose__(x::list of $a)::$a:
    native "__choose__list__"

def __choose__(x::range)::int:
    native "__choose__list__"

def __choose__(x::dict of [int, $b])::int:
    native "__choose__dict__"

def __choose__(x::dict of [str, $b])::str:
    native "__choose__dict__"

def __choose__(x::dict of [bool, $b])::bool:
    native "__choose__dict__"

def __choose__(x::str)::str:
    native "__choose__str__"


# Extra credit

  # Garbage collection
def gc ():
    native "__gc__"
