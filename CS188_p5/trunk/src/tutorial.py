from ghostbusters import *
import util, sensorDistributions, ghostbusterAgent
import staticInferenceModule, dynamicInferenceModule

"""
Ghostbusters Tutorial

This tutorial is intended to help you get started with the project 
by introducing you to the game objects and command line options. 

1. Examine the setup below. We begin by specifying an instance
   of the Game class. Note that we specify some agent arguments
   and let the Game instance build an agent. For more details
   about what each option does, run ghostbusters.py -h from the
   command line.
   
2. Run the code below and examine the Game Basics output. Note that 
   possible ghost positions and busting options are the same and that 
   they are each a list of (tuple of tuples). This is because a bust
   object is really a tuple of ghosts, though currently there is
   only 1 ghost. Try setting numghosts=2 and look at the output.
   Note that now there are more ghost positions than busting options.
   This is because you multiple ghosts can occupy the same square
   but you can only bust a square once. These data-structures are
   created by the functions util.factorials and util.choices.
   
3. Next, set numghosts back to 1 and run again, paying attention to
   the 3 output distributions. These are the three ways in which
   we can access the distributions underlying our game.
   
   game.getGhostTupleDistribution() gets the prior probability of a ghost
   variable being true. Notice that the prior distribution over ghost 
   positions are uniform. Try setting numGhosts=2. What happens to
   the priors?
   
   game.getReadingDistributionGivenGhostTuple(ghostTuple, observation)
   returns the probability distribution over possible sensor readings
   given a ghostTuple (tuple of ghost positions) and an observation
   (a board position). This function represents our knowledge of
   the underlying sensor quality. With a deterministic sensor, these
   probabilities are either 1 or 0 since the sensors give perfect
   information. Try changing the sensor to the noisy distribution
   (uncomment the line of code, below) and look at how the 
   distributions change.
   
   game.getGhostTupleDistributionGivenPreviousGhostTuple(ghostTuple) returns the
   probability distribution over new ghost positions given old
   ghost positions. Since currently, there is no motion, you'll
   notice that the probability of staying still is 1. Try adding
   some motion by changing noise=0.5. Now, half the time ghosts
   will stay still, and the other half of the time, they'll try
   to take a step in a random direction.
   
   If you are curious, take a look at these functions in
   ghostbusters.Game to see how they work, though you actually
   don't need to understand this code to do the project.
   
4. Lastly, open up a command line window and try some of these
   sample ghostbusters runs:
     
   python ghostbusters.py
   python ghostbusters.py -w
   python ghostbusters.py -w -k 2
   python ghostbusters.py -w -n .5
   python ghostbusters.py -w -m circle -n .2
   python ghostbusters.py -h
   
5. Ok. You are now ready for anything. Good luck with your coding.
   
"""

# game settings
layout = Layout((2, 2))
numghosts = 1
sensors = Sensors(sensorDistributions.deterministicSensorReadingDistribution)
#sensors = Sensors(sensorDistributions.noisySensorReadingDistribution)
motiontype = 'basic'
noise = 0
motion = Motion(layout, motiontype, noise)

# set up agent to be created by the game
player = 'human'
inference = 'exact'

timed = False
if motiontype != 'basic' or noise > 0:
  timed = True
agentBuilder = None
if not timed:
  if player == 'human':
    agentBuilder = lambda game: ghostbusterAgent.StaticKeyboardAgent(staticInferenceModule.ExactStaticInferenceModule(game), game)
  if player == 'vpi':
    agentBuilder = lambda game: ghostbusterAgent.StaticVPIAgent(staticInferenceModule.ExactStaticInferenceModule(game), game)
else:
  if inference == 'exact':
    agentBuilder = lambda game: ghostbusterAgent.DynamicKeyboardAgent(dynamicInferenceModule.ExactDynamicInferenceModule(game), game)
  if inference == 'approximate':
    agentBuilder = lambda game: ghostbusterAgent.DynamicKeyboardAgent(dynamicInferenceModule.ApproximateDynamicInferenceModule(game, options['samples']), game)
if agentBuilder == None:
  raise 'Agent not specd correctly!'

game = Game(agentBuilder, layout, numghosts, sensors, motion)

print 'Game Basics:'
print 'ghosts:', game.getNumGhosts()
print 'board positions:', game.getLocations()
print 'number of board positions:', game.getNumLocations() 
print 'possible ghost positions:', game.getGhostTuples()
print 'busting options:', game.getBustingOptions()
print 'possible sensor readings:', Readings.getReadings()
print

print 'Priors P(true position):'
priors = game.getInitialDistribution()
for pos in priors.keys():
  print 'position', pos, ': prior =', priors[pos]
print

print 'P(observation | ghost positions)'
observationPosition = (0, 0)
print 'observing:', observationPosition
for ghost in game.getGhostTuples():
  dist = game.getReadingDistributionGivenGhostTuple(ghost, observationPosition)
  print 'ghost(s) at', ghost, ':', dist
print

print 'P(new ghost position | old ghost position)'
for ghost in game.getGhostTuples():
  dist = game.getGhostTupleDistributionGivenPreviousGhostTuple(ghost)
  print 'ghost(s) at', ghost, ':', dist
