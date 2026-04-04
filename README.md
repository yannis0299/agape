# Agape: First-order (for now) lambda calculus

## Target syntax

```
uncurry f xy = 
    match xy with
        (x,y) => f x y

curry f x y = f (x,y)

{- below is a pragma (and this is a comment btw) -}
{: infix(right, 9) :}
(.) f g x = g (f x)

fib = fib' 0 1
    where
        fib' u v n = fib' v (u + v) (n - 1)

fold f acc xs =
    match xs with
        [] => acc
        x :: xs => fold f (f x acc) xs

rev = fold (::) []

main = rev $ [1, 2, 3]
```
