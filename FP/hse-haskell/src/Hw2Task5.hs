{-# LANGUAGE InstanceSigs #-}

module Hw2Task5
    ( 
        MonadTrans,
        MaybeT,
        ContT,
        StateT
    ) where

import Control.Monad
import Data.Functor

-------------------------------------------------------------------------------

class MonadTrans n where
    lift :: Monad m => m a -> n m a



newtype MaybeT m a = MaybeT { runMaybeT :: m (Maybe a) }

instance MonadTrans MaybeT where
    --lift :: m a -> MaybeT m a
    lift ma = MaybeT ( (fmap Just) ma )


newtype ContT r m a = ContT { runContT :: (a -> m r) -> m r }

instance MonadTrans (ContT r) where
    --lift :: m a -> (ContT r) m a
    lift ma = ContT (\a2mr -> (ma >>= a2mr))


newtype StateT s m a = StateT { runStateT :: s -> m (a, s) }

instance MonadTrans (StateT s) where
    --lift :: m a -> (StateT s) m a
    lift m = StateT (\s -> (mas s))
        where
            mas s = m >>= (\a -> return (a, s))


