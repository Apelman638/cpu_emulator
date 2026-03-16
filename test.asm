bldsc

rect 1 20 20 50 50 
print

A = 13
func move
movy 0 1
upd
print 
endf move

rep: A move
save
# appears to not actually move the object for movx, but movy works