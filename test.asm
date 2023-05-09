# load the number of times we want to multiply the other number by
ldi 3
swd

# loop:
ldi 5
swb

# perform addition and store into e
add
swe

# load count
swd
swb
ldi 1
swc

# count down
sub
swd

# load sum of previous addition into c for next iteration
swe
swc

# jmp to 4 (ldi 5)
ldi 4
jnz

# swap c into a for return
swc

# halt
hlt
