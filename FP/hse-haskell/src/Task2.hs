{-# LANGUAGE TypeOperators #-}

module Task2
    ( 
        distributivity,
        test_distributivity, 
        associator,
        test_associator,
        eitherAssoc,
        pairProd,
        weirdFunction,
        eitherAssoc2,
        distr
    ) where

import Data.Either

-------------------------------------------------------------------------------

mkleft :: a -> (Either a b, Either a c)
mkleft x = (Left x, Left x) 

mkright :: (b, c) -> (Either a b, Either a c)
mkright (x, y) = (Right x, Right y) 

distributivity
  :: Either a (b, c)
  -> (Either a b, Either a c)
distributivity = either mkleft mkright


testl = Left "test" :: Either String (Int, Float)
testr = Right (3, 3.14) :: Either String (Int, Float)

test_distributivity = do
  putStrLn "Test distributivity"
  putStr "\tArg: "
  putStrLn (show testl)
  putStr "\tRes: "
  putStrLn (show (distributivity testl))
  putStr "\tArg: "
  putStrLn (show testr)
  putStr "\tRes: "
  putStrLn (show (distributivity testr))

-------------------------------------------------------------------------------

associator
  :: (a, (b, c))
  -> ((a, b), c)
associator (x, (y, z)) = ((x, y), z)

testa = ("test", (42, 3.14)) :: (String, (Int, Float))

test_associator = do
  putStrLn "Test associator"
  putStr "\tArg: "
  putStrLn (show testa)
  putStr "\tRes: "
  putStrLn (show (associator testa))

-------------------------------------------------------------------------------

ha :: a -> Either (Either a b) c
ha x = Left (Left x)
hb :: b -> Either (Either a b) c
hb x = Left (Right x)
hc :: c -> Either (Either a b) c
hc x = Right x
hbc :: (Either b c) -> Either (Either a b) c
hbc = either hb hc

ea_lr :: Either a (Either b c) -> Either (Either a b) c
ea_lr = either ha hbc


rha = Left
rhb :: b -> Either a (Either b c)
rhb x = Right (Left x)
rhc x = Right (Right x)
rhab = either rha rhb
ea_rl :: Either (Either a b) c -> Either a (Either b c)
ea_rl = either rhab rhc 


type (<->) a b = (a -> b, b -> a)

eitherAssoc
  :: Either a (Either b c)
  <-> Either (Either a b) c
eitherAssoc = (ea_lr, ea_rl)

-------------------------------------------------------------------------------

pairProd
  :: (a -> b)
  -> (c -> d)
  -> (a,c)
  -> (b,d)
pairProd f g (a, c) = (f a, g c)

-------------------------------------------------------------------------------

weirdFunction
  :: (d -> d -> b)
  -> (a -> b -> c)
  -> (d -> b)
  -> d -> b
weirdFunction _ _ f d = f d

-------------------------------------------------------------------------------

eitherAssoc2
  :: Either a (Either b c)
  -> Either (Either a b) c
eitherAssoc2 = ea_lr 

-------------------------------------------------------------------------------

distr
  :: (a -> b -> c)
  -> (a -> b)
  -> a -> c
distr f g a = f a (g a)
