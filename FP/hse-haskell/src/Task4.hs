module Task4
    ( 
        iterateElement,
        fibonacci,
        factorial,
        mapFix
    ) where

import Data.Function

-------------------------------------------------------------------------------

prepend x list = x:list

iterateElement :: a -> [a]
iterateElement x = fix (prepend x)

fibonacci :: Integer -> Integer
fibonacci = fix (\fib x -> if x == 0 then 0 else if x == 1 then 1 else (fib x-1) + (fib x-2))

factorial :: Integer -> Integer
factorial = fix (\fact x -> if x == 0 then 1 else x * (fact x-1))

mapFix :: (a -> b) -> [a] -> [b]
mapFix = undefined

-------------------------------------------------------------------------------


