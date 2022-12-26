{-# LANGUAGE InstanceSigs #-}

module Hw2Task4
    ( 
        MyCont
    ) where


-------------------------------------------------------------------------------

newtype MyCont r a = MyCont { runCont :: (a -> r) -> r }

instance Functor (MyCont r) where
    fmap :: (a -> b) -> (MyCont r) a -> (MyCont r) b
    fmap a2b ma2r2r = MyCont ( \b2r -> r b2r)
        where
            r b2r = (runCont ma2r2r) (a2r b2r)
            a2r b2r = b2r . a2b


instance Applicative (MyCont r) where
    pure  :: a -> (MyCont r) a
    pure a = MyCont (\a2r -> a2r a)
    (<*>) :: (MyCont r) (a -> b) -> (MyCont r) a -> (MyCont r) b
    -- ((a -> b) -> r) -> r
    -- (a -> r) -> r
    -- (b -> r) -> r
    (<*>) ma2b2r2r ma = MyCont(\b2r -> r b2r)
        where
            r b2r = (runCont ma2b2r2r) (a2b2r b2r)
            a2b2r b2r a2b = (runCont ma) ( b2r . a2b)

instance Monad (MyCont r) where
    return :: a -> (MyCont r) a
    return a = MyCont (\a2r -> a2r a)
    (>>=) :: (MyCont r) a -> (a -> (MyCont r) b) -> (MyCont r) b
    (>>=) ma2r2r a2mb2r2r = MyCont (\b2r -> r b2r)
        where 
            r b2r = (runCont ma2r2r) (a2r b2r)
            a2r b2r a = (runCont (a2mb2r2r a)) b2r

