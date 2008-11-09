import util
from ghostbusters import *

"""
sensorReadingDistribution[t] is a distribution for observations
given a true distance of t. sensorReadingDistribution[1][YELLOW], for
example, is the probability that the sensor gives a YELLOW reading
given that the true distance from the sensor to the closest
ghost is 1.
"""

deterministicSensorReadingDistribution = {
  0: util.Counter( {Readings.RED: 1} ),
  1: util.Counter( {Readings.ORANGE: 1} ),
  2: util.Counter( {Readings.ORANGE: 1} ),
  3: util.Counter( {Readings.YELLOW: 1} ),
  4: util.Counter( {Readings.YELLOW: 1} ),
  5: util.Counter( {Readings.GREEN: 1} ),
  6: util.Counter( {Readings.GREEN: 1} ) }

noisySensorReadingDistribution = {
  0: util.Counter( {Readings.RED: .9, Readings.ORANGE: .1} ),
  1: util.Counter( {Readings.RED: .1, Readings.ORANGE: .6, Readings.YELLOW: .25, Readings.GREEN: .05} ),
  2: util.Counter( {Readings.RED: .1, Readings.ORANGE: .6, Readings.YELLOW: .25, Readings.GREEN: .05} ),
  3: util.Counter( {Readings.RED: .05, Readings.ORANGE: .15, Readings.YELLOW: .5, Readings.GREEN: .3} ),
  4: util.Counter( {Readings.RED: .05, Readings.ORANGE: .15, Readings.YELLOW: .5, Readings.GREEN: .3} ),
  5: util.Counter( {Readings.RED: .01, Readings.ORANGE: .05, Readings.YELLOW: .3, Readings.GREEN: .64} ),
  6: util.Counter( {Readings.RED: .01, Readings.ORANGE: .05, Readings.YELLOW: .3, Readings.GREEN: .64} ) }

noisySensorReadingDistribution2 = {
  0: util.Counter( {Readings.RED: .9, Readings.ORANGE: .1} ),
  1: util.Counter( {Readings.RED: .1, Readings.ORANGE: .75, Readings.YELLOW: .1, Readings.GREEN: .05} ),
  2: util.Counter( {Readings.RED: .1, Readings.ORANGE: .75, Readings.YELLOW: .1, Readings.GREEN: .05} ),
  3: util.Counter( {Readings.RED: .05, Readings.ORANGE: .1, Readings.YELLOW: .75, Readings.GREEN: .1} ),
  4: util.Counter( {Readings.RED: .05, Readings.ORANGE: .1, Readings.YELLOW: .75, Readings.GREEN: .1} ),
  5: util.Counter( {Readings.RED: .01, Readings.ORANGE: .05, Readings.YELLOW: .1, Readings.GREEN: .84} ),
  6: util.Counter( {Readings.RED: .01, Readings.ORANGE: .05, Readings.YELLOW: .1, Readings.GREEN: .84} ) }

