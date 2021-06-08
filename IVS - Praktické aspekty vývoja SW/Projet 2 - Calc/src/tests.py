"""@brief Tests for math operations
   
   @author IVS-DREAM-TEAM
   
   @file tests.py
"""

import pytest
import random
import math
from lib.mathlib import *



def test_add_basic():
    """Basic adition tests
    """    
    assert add(2,3) == 5
    assert add(99, 1) == 100
    assert add(15,15) == add(20,10)
    assert add(10000, 20000) == 30000
    assert add(100000000, 1) == 100000001  

def test_add_inf():
    """Adition tests with infinity
    """    
    assert add(5,float('inf')) == float('inf')
    assert add(float('inf'),float('inf')) == float('inf')
    assert math.isnan(add(float('-inf'), float('inf')))


def test_add_zero():
    """Adition tests with zero as parameter
    """    
    assert add(121, 0) == 121
    assert add(0,19) == 19
    assert add(0,0) == 0
    assert add(0,8) == add(8,0)

def test_add_errors():
    """Adition tests with wrong type of parameter
    """    
    with pytest.raises(TypeError):
        add(8, 'a')
        add('a', 8)
        add('a', 'a')

def test_add_negative():
    """Adition tests for negative numbers
    """    
    assert add(-1,0) == -1
    assert add(-1,-3) == -4
    assert add(1,-3) == -2
    assert add(-100,3) == -97




def test_sub_basic():
    """Basic subtraction tests
    """    
    assert sub(1,3) == -2
    assert sub(17,5) == 12
    assert sub(5,5) == 0
    assert sub(10000005,5) == 10000000

def test_sub_zero():
    """Subtraction tests with zero
    """    
    assert sub(1,0) == 1
    assert sub(0,0) == 0
    assert sub(0,10) == -10
    
def test_sub_inf():
    """Subtraction test with infinity as parameter
    """    
    assert sub(8,float('inf')) == float('-inf')

def test_sub_negative():
    """Subtraction test with negative parameter
    """    
    assert sub(-1,-5) == 4
    assert sub(-1,5) == -6
    assert sub(1,-15) == 16

def test_sub_decimal():
    """Subtraction test with decimal parameter
    """    
    assert sub(-1.22222,1) == -2.22222
    assert sub(1.22222,-1.22222) == 2.44444
    assert sub(1.22222,1.22222) == 0
    assert sub(0,0.123456789) == -0.123456789
    assert sub(0,0.1234567894) == -0.123456789   #zaokruhlenie na dol na deviatom mieste
    assert sub(0,0.1234567815) == -0.123456782   #zaokruhlenie na hor na deviatom mieste



def test_mul_basic():
    """Basic multiplying tests
    """    
    assert mul(1,5) == 5
    assert mul(34,7) == 238
    assert mul(12345,1) == 12345
    assert mul(12345,45678) == 563894910
    assert mul(3,6) == mul(6,3)

def test_mul_zero():
    """Multiplying tests with zero as parameter
    """    
    assert mul(0,9) == 0
    assert mul(71,0) == 0
    assert mul(0,0) == 0
    assert mul(0,-16) == 0
    assert mul(-97,0) == 0
    assert mul(-37.2332,0) == 0
    assert mul(0,-545) == 0

def test_mul_negative():
    """Multiplying tests with negative parameters
    """    
    assert mul(-17,-31) == 527
    assert mul(4,-19) == -76
    assert mul(-68,26) == -1768

def test_mul_decimal():
    """Multiplying tests with decimal parameter
    """    
    assert mul(23.6556,456) == 10786.9536
    assert mul(687.592,783.68) == 538852.09856
    assert mul(6.123456,3.68123) == 22.541849931    #zaokruhlene na hor 22,541849930 88
    assert mul(3.7526982, 8.462) == 31,755332168    #zaokruhlenie na dol 31,755332168 4



def test_div_basic():
    """Basic divison tests
    """    
    assert div(4,2) == 2
    assert div(3,9) == 0.333333333
    assert div (123135498,1) == 123135498
    assert div (123135498,10) == 12313549.8

def test_div_zero():
    """Divison tests with zero as parameter
    """    
    with pytest.raises(ZeroDivisionError):
        div(45685,0)
        div(0,0)

def test_div_negative():
    """Divison tests with negative parameter
    """    
    assert div(81,-9) == -9
    assert div(-125,25) == -5
    assert div(-10000,-10) == 1000

def test_div_decimal():
    """Division tests with decimal parameter
    """    
    assert div(125,0.5) == 250
    assert div(444.44,2) == 222.22
    assert div(259.568,9) == 28.840888889   # 8 zaokrulenie na hor
    assert div(3529456.45,128.175) == 27536.231324361 # 2 zaokruhlenie na dol



def test_fact_basic():
    """Basic test factorial
    """    
    assert fact(1) == 1
    assert fact(7) == 5040
    assert fact(13) == 6227020800
    

def test_fact_zero():
    """Zero factorial test
    """    
    assert fact(0) == 1

def test_fact_negative():
    """Negative factorial tests
    """    
    with pytest.raises(ValueError):
        fact(-1)
        fact(-8)

def test_fact_decimal():
    """Decimal factorial test
    """    
    with pytest.raises(TypeError):
        fact(1.7)
        fact(0.8)




def test_exponent_basic():
    """Basic tests for exponent operation
    """    
    assert exp(1,7) == 1
    assert exp(3,7) == 2187
    assert exp(15,7) == 170859375

def test_exponent_zero():
    """Exponent tests with 0 as parameter
    """    
    assert exp(9,0) == 1
    assert exp(98797,0) == 1
    assert exp(0,0) == 1


def test_root_basic():
    """Basic tests for n-th root operation
    """    
    assert root(2, 1) == 1
    assert root(3,1) == 1
    assert root(2, 4) == 2
    assert root(2, 9) == 3
    assert root(3, 27) == 3
    assert root(4, 16) == 2
    assert root(5, 243) == 3



def test_root_zero():
    """N-th root tests with zero as argument
    """    
    with pytest.raises(ValueError):
        root(0, 42)
        root(0, 0)
        root(0, -50)
        root(42, 0)


def test_root_negative():
    """N-th root with negative arguments
    """    
    assert root(3, -27) == -3
    assert root(3, -8) == -2
    with pytest.raises(ValueError):
        root(2, -42)
        root(2, -50)
        root(4, -36)


def test_mod_basic():
    """Basic tests for operation modulo
    """    
    assert mod(51651351, 2) == 1
    assert mod(15, 3) == 0
    assert mod(36, 36) == 0
    assert mod(115, 42) == 31
    assert mod(115, 165) == 115
    assert mod(238, 420) == 238


def test_mod_zero():
    """Modulo operation with zero argument
    """    
    with pytest.raises(ZeroDivisionError):
        math.isnan(mod(42, 0))
        math.isnan(mod(0,0))
    assert mod(0, 42) == 0


def test_mod_negative():
    """Modulo operation with negative argument
    """    
    assert mod(-2, 2) == 0
    assert mod(2, -2) == 0
    assert mod(-2, -2) == 0
    with pytest.raises(ZeroDivisionError):
        mod(-42, 0)


def test_mod_decimal():
    """Modulo operation with decimal argument
    """    
    with pytest.raises(TypeError):
        mod(0, 4.2)
        mod(42, 4.2)
        mod(4.2, 2)
        mod(4.2, 4.2)