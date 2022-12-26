module Hw3Task1
    ( 
        parFib,
        test_parFib
    ) where

import Control.Parallel.Strategies

-------------------------------------------------------------------------------

fib :: Int -> Int
fib 0 = 0
fib 1 = 1
fib n = parFib n

parFib :: Int -> Int
parFib n = runEval ( do { prevFib <- rpar (fib (n-1)); prevPrevFib <- rpar (fib (n-2)); return (prevFib + prevPrevFib)} )

test_parFib = do
  putStrLn "Test parFib"
  putStr "\tFib 0: "
  putStrLn (show (fib 0))
  putStr "\tFib 1: "
  putStrLn (show (fib 1))
  putStr "\tFib 2: "
  putStrLn (show (fib 2))
  putStr "\tFib 3: "
  putStrLn (show (fib 3))
  putStr "\tFib 4: "
  putStrLn (show (fib 4))
  putStr "\tFib 10: "
  putStrLn (show (fib 10))
  putStr "\tFib 40: "
  putStrLn (show (fib 40))


