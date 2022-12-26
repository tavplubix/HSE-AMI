{-# LANGUAGE InstanceSigs #-}
{-# LANGUAGE Arrows #-}

module Hw2Task2
    ( 
        SignalFunction,
        integral,
        someFunction,
        test_runSignalFunction
    ) where

import Prelude hiding (id, (.))
import Control.Category
import Control.Arrow

-------------------------------------------------------------------------------

data SignalFunction a b = SignalFunction ((a, Double) -> (SignalFunction a b, b))

instance Category SignalFunction where
    id :: SignalFunction a a
    id = SignalFunction (\(a, d) -> (id, a))
    (.) :: SignalFunction b c -> SignalFunction a b -> SignalFunction a c
    (SignalFunction f) . (SignalFunction g) = SignalFunction (\(a, d) -> (sfg (a, d), c (a, d)))
        where 
            b ad = snd (g ad)
            sg ad = fst (g ad)
            c ad = snd (f (b ad, snd ad))
            sf ad = fst (f (b ad, snd ad))
            sfg ad = (sf ad) . (sg ad)

instance Arrow SignalFunction where
    arr :: (a -> b) -> SignalFunction a b
    arr f = SignalFunction (\(a, d) -> (arr f, f a))
    first :: SignalFunction a b -> SignalFunction (a, c) (b, c) 
    first (SignalFunction f) = SignalFunction (\((a, c), d) -> (first (sf (a, d)), (b (a, d), c)))
        where
            sf ad = fst (f ad)
            b ad = snd (f ad)



addint prevI prevY nextY deltaX = prevI + 0.5*(prevY + nextY)*deltaX 

integralStep :: Double -> Double -> SignalFunction Double Double
integralStep prevI prevY = SignalFunction (\(nextY, deltaX) -> (integralStep (partI nextY deltaX) nextY, partI nextY deltaX))
    where 
        partI nextY deltaX = addint prevI prevY nextY deltaX

integral :: SignalFunction Double Double
integral = integralStep 0 0



someFunction :: SignalFunction (Double, Double) (Double, Double)
someFunction = proc (x, y) -> do 
    x2 <- arr (\a -> a * 2) -< x 
    y3 <- arr (\a -> a * 3) -< y 
    x2y3 <- arr (\(a, b) -> a + b) -< (x2, y3)
    resI <- integral -< x2y3
    arr id -< (resI, x2y3)


runSignalFunction :: SignalFunction a b -> a -> [(a, Double)] -> [b]
runSignalFunction (SignalFunction f) atZero inputs = outputs (fst(f (atZero, 0))) inputs
    where
        outputs _ [] = []
        outputs (SignalFunction f) (i:inputs) = (o f i) : outputs (nf f i) inputs
        nf f i = fst (f i)
        o f i = snd (f i)


-------------------------------------------------------------------------------


data_ = [ ((1, 2), 0.1), ((3, 4), 0.2), ((5, 6), 0.3) ]

expected = [ (0.4, 8), (3, 18), (9.9, 28) ]

got = runSignalFunction someFunction (0, 0) data_

test_runSignalFunction = do
  putStrLn "Test runSignalFunction"
  putStr "\tData: "
  putStrLn (show data_)
  putStr "\tGot: "
  putStrLn (show got)
  putStr "\tExpected: "
  putStrLn (show expected)
