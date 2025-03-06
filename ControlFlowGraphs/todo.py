from lang import *


def test_min(m, n):
    """
    Stores in the variable 'answer' the minimum of 'm' and 'n'

    Examples:
        >>> test_min(3, 4)
        3
        >>> test_min(4, 3)
        3
    """
    env = Env({"m": m, "n": n,  "zero": 0})
    m_min = Add("answer", "m", "zero")
    n_min = Add("answer", "n", "zero")
    p = Lth("p", "n", "m")
    b = Bt("p", n_min, m_min)
    p.add_next(b)
    interp(p, env)
    return env.get("answer")
# print(test_min(3,1100))

def test_fib(n):
    """
    Stores in the variable 'answer' the n-th number of the Fibonacci sequence.

    Examples:
        >>> test_fib(2)
        2
        >>> test_fib(3)
        3
        >>> test_fib(6)
        13
    """
    env = Env({"c": 0, "N": n, "fib0": 0, "fib1": 1, "zero": 0, "one": 1})
    i0 = Lth("p", "c", "N")
    i2 = Add("aux", "fib1", "zero")
    i3 = Add("fib1", "aux", "fib0")
    i4 = Add("fib0", "aux", "zero")
    i5 = Add("c", "c", "one")
    i6 = Add("answer", "fib1", "zero")
    i1 = Bt("p", i2, i6)
    i0.add_next(i1)
    i2.add_next(i3)
    i3.add_next(i4)
    i4.add_next(i5)
    i5.add_next(i0)
    interp(i0, env)
    return env.get("answer")


def test_min3(x, y, z):
    """
    Stores in the variable 'answer' the minimum of 'x', 'y' and 'z'

    Examples:
        >>> test_min3(3, 4, 5)
        3
        >>> test_min3(5, 4, 3)
        3
    """
    # env = Env({"cond" : test_min(x,y) == test_min(y,z), "Ans" : test_min(x,z),"Y": y, "zero": 0})

    # b1 = Add("answer","Y","zero")
    # b2 = Add("answer","Ans","zero")
    # i0 = Bt("cond",b1,b2)
    
    # interp(i0, env)
    a = test_min(x,y)
    b = test_min(a,z)

    env = Env({"B" : b, "zero" : 0})
    i0 = Add("answer","B","zero")

    interp(i0, env)
    return env.get("answer")

print(test_min3(4, 4, 5))

def test_div(m, n):
    """
    Stores in the variable 'answer' the integer division of 'm' and 'n'.

    Examples:
        >>> test_div(30, 4)
        7
        >>> test_div(4, 3)
        1
        >>> test_div(1, 3)
        0
    """
    env = Env({"M" : m, "N" : n, "sum" : n, "count": 0, "zero" : 0,"one" : 1})

    p = Lth("p","sum","M")
    i1 = Add("sum","sum","N")
    i2 = Add("count","count","one")
    i3 = Add("answer","count","zero")
    i0 = Bt("p",i1,i3)

    p.add_next(i0)
    i1.add_next(i2)
    i2.add_next(p)
    interp(p,env)
    return env.get("answer")
print(test_div(1, 3))

def test_fact(n):
    """
    Stores in the variable 'answer' the factorial of 'n'.

    Examples:
        >>> test_fact(3)
        6
    """
    env = Env({"N" : n, "res" : 1, "count": 1,"one" : 1,"zero":0})

    p = Geq("p","N","count")
    i1 = Mul("res","res","count")
    i2 = Add("count","count","one")
    i3 = Add("answer","res","zero")
    i0 = Bt("p",i1,i3)

    p.add_next(i0)
    i1.add_next(i2)
    i2.add_next(p)
    interp(p,env)
    return env.get("answer")
print(test_fact(-1))