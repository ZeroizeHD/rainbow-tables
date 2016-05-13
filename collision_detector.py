# In order to compute the collision percentage,
# just uncomment from createRT.c the 2 printfs
# to the create_chain function. Then compile  
# again, and run "./rt.out 5 > passes.csv".
# Finally run "python collision_detector.py".

words =  set(open('passes.csv').read().split())

unique = len(words)
print "Unique ", unique, "/ 10000 words."
print ((1-(unique/ float(10000))) * 100), "% collisions"
