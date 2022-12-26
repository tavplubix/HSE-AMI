module Hw3Task2
    ( 
        pmap,
        test_pmap
    ) where

import Control.Parallel.Strategies

-------------------------------------------------------------------------------

pmap :: (a -> b) -> [a] -> Eval [b]
pmap _ [] = return []
pmap f list = do { resHead <- rpar (f (head list)); resTail <- pmap f (tail list); return (resHead:resTail) }


slowFib :: Int -> Int
slowFib 0 = 0
slowFib 1 = 1
slowFib n = slowFib (n - 1) + slowFib (n - 2)


test :: Bool
test
  = let hugeNumber = 50
    in  runEval (pmap slowFib [1, hugeNumber, 5]) !! 2 == 5


test_pmap = do
  putStrLn "Test pmap"
  putStr "\tRes: "
  putStrLn (show test)

