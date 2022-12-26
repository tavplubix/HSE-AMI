
module Task3
    ( 
        test_nat
    ) where

import Control.Exception

-------------------------------------------------------------------------------

data Nat = Zero | Succ Nat

natToInt :: Nat -> Int
natToInt Zero = 0
natToInt (Succ x) = 1 + natToInt(x)

instance Show Nat where
  show x = show (natToInt x)

instance Eq Nat where
  Zero == Zero = True
  Zero == Succ(x) = False
  Succ(x) == Zero = False
  Succ(x) == Succ(y) = x == y

instance Ord Nat where
  Zero <= _ = True
  Succ(x) <= Zero = False
  Succ(x) <= Succ(y) = x <= y

add x Zero = x
add x (Succ y) = add (Succ x) y

mul _ Zero = Zero
mul x (Succ y) = x + (mul x y)

sub x Zero = x
sub Zero (Succ x) = error "Result is not a natural number"
sub (Succ x) (Succ y) = sub x y

fromInt x | x < 0 = error "Cannot convert negative integer to natural number"
fromInt 0 = Zero
fromInt x | x > 0 = Succ (fromInt (x - 1))

instance Num Nat where
  x + y       = add x y
  x * y       = mul x y
  x - y       = sub x y
  abs         = id
  signum Zero     = Zero
  signum (Succ x)     = Succ Zero
  fromInteger = fromInt

instance Enum Nat where
  toEnum   = fromInt
  fromEnum = natToInt

-------------------------------------------------------------------------------

mberr x = Control.Exception.catch (x) handler
    where
        handler :: Control.Exception.ErrorCall -> IO ()
        handler e = putStrLn (head(lines(show e )))

t1 d f x = do
    putStr "\t"
    putStr d
    putStr " "
    putStr (show x)
    putStr ":\t"
    mberr (putStrLn (show (f (fromInt x))))
t2 d f x y = do
    putStr "\t"
    putStr d
    putStr " "
    putStr (show x)
    putStr " "
    putStr (show y)
    putStr ":\t"
    mberr (putStrLn (show (f (fromInt x) (fromInt y))))

test_nat  = do
    putStrLn "Test Nat:"
    t1 "show" id 0
    t1 "show" id 1
    t1 "show" id (-3)
    t1 "show" id 42
    t2 "==" (==) 0 2
    t2 "==" (==) 2 0
    t2 "==" (==) 2 2
    t2 "<" (<) 0 42
    t2 "<" (<) 0 0
    t2 "<" (<) 42 3
    t2 "+" (+) 1 0
    t2 "+" (+) 0 1
    t2 "+" (+) 40 2
    t2 "*" (*) 0 0
    t2 "*" (*) 0 1
    t2 "*" (*) 1 0
    t2 "*" (*) 6 7
    t2 "*" (*) 7 6
    t2 "-" (-) 0 0
    t2 "-" (-) 2 0
    t2 "-" (-) 0 2
    t2 "-" (-) 42 2
    t2 "-" (-) 2 42
    t2 "-" (-) 42 42
    t1 "abs" abs 0
    t1 "abs" abs 1
    t1 "sgn" signum 0
    t1 "sgn" signum 1

