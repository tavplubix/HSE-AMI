{-# LANGUAGE InstanceSigs #-}

module Hw2Task1
    ( 
        Reader,
        Writer,
        State
    ) where

import Data.Monoid

-------------------------------------------------------------------------------

newtype Reader r a = Reader { runReader :: r -> a }

instance Functor (Reader r) where
    fmap :: (a -> b) -> (Reader r) a -> (Reader r) b
    fmap f ra = Reader (f . (runReader ra))

instance Applicative (Reader r) where
    pure  :: a -> (Reader r) a
    pure a = Reader (\x -> a)
    (<*>) :: (Reader r) (a -> b) -> (Reader r) a -> (Reader r) b
    (<*>) rf ra = Reader r2b
        where
            --r2b :: r -> b
            r2b r = (r2f r) (r2a r)
            --r2f :: r -> (a -> b)
            r2f = runReader rf
            --r2a :: r -> a 
            r2a = runReader ra


instance Monad (Reader r) where
    return :: a -> (Reader r) a
    return a = Reader (\x -> a)
    (>>=) :: (Reader r) a -> (a -> (Reader r) b) -> (Reader r) b
    (>>=) ra a2rb = Reader r2b
        where
            --r2a :: r -> a
            r2a = runReader ra
            --r2rb :: r -> (Reader r) b
            r2rb r = a2rb (r2a r) 
            --r2b :: r -> b
            r2b r = (runReader (r2rb r)) r
            

ask :: Reader r r
ask = Reader id

local
  :: (r -> r)
  -> Reader r a
  -> Reader r a
local f ra = Reader r2a
    where
        r2a r = runReader ra (f r)

-------------------------------------------------------------------------------

newtype Writer w a = Writer { runWriter :: (a, w) }

tell
  :: Monoid w
  => w
  -> Writer w ()
tell w = Writer ((), w)

listen
  :: Monoid w
  => Writer w a
  -> Writer w (w, a)
listen wa = Writer ((snd taw, fst taw), snd taw)
    where
        taw = runWriter wa 

pass
  :: Monoid w
  => Writer w (a, w -> w)
  -> Writer w a
pass wtaf = Writer (fst taf, (snd taf) (snd ttw))
    where
        ttw = runWriter wtaf
        taf = fst ttw

instance Functor (Writer w) where
    fmap :: (a -> b) -> (Writer w) a -> (Writer w) b
    fmap f wa = Writer (f (fst taw), snd taw)
        where 
            --taw :: (a, w)
            taw = runWriter wa

instance Monoid w => Applicative (Writer w) where
    pure  :: a -> (Writer w) a
    pure a = Writer (a, mempty)
    (<*>) :: (Writer w) (a -> b) -> (Writer w) a -> (Writer w) b
    (<*>) wf wa = Writer ((fst tfw) (fst taw), w)
        where 
            --taw :: (a, w)
            taw = runWriter wa
            --tfw :: (a -> b, w)
            tfw = runWriter wf
            --w :: w
            w = mappend (snd taw) (snd tfw)

instance Monoid w => Monad (Writer w) where
    return :: a -> (Writer w) a
    return a = Writer (a, mempty)
    (>>=) :: (Writer w) a -> (a -> (Writer w) b) -> (Writer w) b
    (>>=) wa a2wb = Writer (fst tbw, w)
        where
            w = mappend (snd taw) (snd tbw)
            taw = runWriter wa
            tbw = runWriter wb
            wb = a2wb (fst taw)

            

-------------------------------------------------------------------------------

newtype State s a = State { runState :: s -> (a, s) }

get :: State s s
get = State (\s -> (s, s))

put :: s -> State s ()
put ns = State (\s -> ((), ns))

instance Functor (State s) where
    fmap :: (a -> b) -> (State s) a -> (State s) b
    fmap f sa = State s2tbs
        where
            s2tas = runState sa
            b s = f (fst (s2tas s))
            s2tbs s = (b s, (snd (s2tas s)))

instance Applicative (State s) where
    pure :: a -> (State s) a
    pure a = State (\s -> (a, s)) 
    (<*>) :: (State s) (a -> b) -> (State s) a -> (State s) b
    (<*>) sf sa = State (\s -> (b s, nns s))
        where 
            b s = (f s) (a s)
            f s = fst ((runState sf) s)
            ns s = snd ((runState sf) s)
            a s = fst ((runState sa) (ns s))
            nns s = snd ((runState sa) (nns s))


instance Monad (State s) where
    return :: a -> (State s) a
    return a = State (\s -> (a, s)) 
    (>>=) :: (State s) a -> (a -> (State s) b) -> (State s) b
    (>>=) sa a2sb= State (\s -> (b s, nns s))
        where
            a s = fst ((runState sa) s)
            ns s = snd ((runState sa) s)
            sb s = a2sb (a s)
            b s = fst ((runState (sb s)) (ns s))
            nns s = snd ((runState (sb s)) (ns s))

