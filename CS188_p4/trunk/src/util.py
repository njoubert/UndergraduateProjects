import random, inspect

def sample(distribution):
  """
  Sample a value from a U(0,1) distribution, and use this to generate
  a random sample from this particular distribution.
  """
  cumulative = 0.0
  uniform_sample = random.random()
  for entry, prob in distribution.items():
    cumulative += prob
    if cumulative >= uniform_sample:
      return entry
  raise 'sampling error: bad distribution'

def sampleMultiple(distribution, n):
  keys = distribution.keys()
  cumulativeProbs = []
  total = 0
  for key in keys:
    value = distribution.getCount(key)
    total += value
    cumulativeProbs.append(total)
  if total > 1.0001: raise 'sampling error: bad distribution'
  if total < 0.9999: raise 'sampling error: bad distribution'

  samples = []
  for i in range(n):
    uniform_sample = random.random()
    if uniform_sample > total: uniform_sample = total
    samples.append(keys[binarySearch(cumulativeProbs, uniform_sample)])
  return samples

def binarySearch(list, value):
  low = 0
  high = len(list)-1
  while True:
    if low == high:
      return low
    mid = (low + high) / 2
    if value <= list[mid]:
      high = mid
    elif value > list[mid]:
      low = mid + 1
  raise 'sampling error: bad distribution'


def listToDistribution(list):
  distribution = Counter()
  for item in list:
    distribution.incrementCount(item, 1.0)
  return normalize(distribution)

def normalize(counter):
  """
  normalize a counter by dividing each value by the sum of all values
  """
  normalizedCounter = Counter()
  total = float(counter.totalCount())
  for key in counter.keys():
    value = counter.getCount(key)
    normalizedCounter.setCount(key, value / total)
  return normalizedCounter
    
def maxes(counter):
  """
  returns the max and a list of all equivalent argmaxes
  """
  max, argmaxes = None, []
  for x in counter.keys():
    c = counter.getCount(x)
    if max == None or c > max:
      max = c
  for x in counter.keys():
    c = counter.getCount(x)
    if c == max:
      argmaxes.append(x)
  return max, argmaxes

def manhattanDistance( x, y ):
  """
  returns the city-block distance between points x and y
  """
  return abs( x[0] - y[0] ) + abs( x[1] - y[1] )

def manhattanDistanceWithMax( x, y, max):
  return min(manhattanDistance(x,y), max)

#===============================================================================
#def cartesianProduct(n, values):
#  """
#  Recursively generates all sequences of length n
#  """
#  if n == 0: return
#  for s in values:
#    if n == 1:
#      yield[s]
#    else:
#      for rest in cartesianProduct(n-1, values):
#        yield[s] + rest
#===============================================================================

def choices(n, values, start = None):
  """
  Recursively generates all sets of size n
  """
  list = []
  if n == 0: return None
  foundStart = (start == None)
  for s in values:
    if s == start:
      foundStart = True
      continue
    if not foundStart: continue
    if n == 1:
      list.append([s])
    else:
      rests = choices(n-1, values, s)
      for rest in rests:
        list.append([s] + rest)
  return list
      
def factorials(n, values):
  """
  Recursively generates all lists of size n
  """
  list = []
  if n == 0: return None
  for s in values:
    if n == 1:
      list.append([s])
    else:
      rests = factorials(n-1, values)
      for rest in rests:
        list.append([s] + rest)
  return list
      
def cartesianJoint(distributionList):
  if len(distributionList) == 0: return None
  result = Counter()
  if len(distributionList) == 1:
    distribution = distributionList[0]
    for key, value in distribution.items():
      result[(key,)] = value
    return result
  else:
    distribution = distributionList[0]
    rest = distributionList[1:]
    restResult = cartesianJoint(rest)
    for key, value in distribution.items():
      for keyRest, valueRest in restResult.items():
        result[(key,)+keyRest] = value * valueRest
    return result


class Counter(dict):
  """
  A counter keeps track of counts for a set of keys.
  
  The counter class is an extension of the standard python
  dictionary type.  It is specialized to have number values  
  (integers or floats), and includes a handful of additional
  functions to ease the task of counting data.  In particular, 
  all keys are defaulted to have value 0.  Using a dictionary:
  
  a = {}
  print a['test']
  
  would give an error, while the Counter class analogue:
    
  >>> a = Counter()
  >>> print a.getCount('test')
  0
  
  returns the default 0 value. Note that to reference a key 
  that you know is contained in the counter, 
  you can still use the dictionary syntax:
    
  >>> a = Counter()
  >>> a['test'] = 2
  >>> print a['test']
  2
  
  The counter also includes additional functionality useful in implementing
  the classifiers for this assignment.  Two counters can be added,
  subtracted or multiplied together.  See below for details.  They can
  also be normalized and their total count and arg max can be extracted.
  """
  def incrementCount(self, key, count):
    """
    Increases the count of key by the specified count.  If 
    the counter does not contain the key, then the count for
    key will be set to count.
    
    Return the counter as well.
    
    >>> a = Counter()
    >>> a.incrementCount('test', 1)
    >>> a.getCount('hello')
    0
    >>> a.getCount('test')
    1
    """
    if key in self:
      self[key] += count
    else:
      self[key] = count
      
    return self
      
  def incrementAll(self, keys, count):
    """
    Increments all elements of keys by the same count.
    
    Return the counter as well.
    
    >>> a = Counter()
    >>> a.incrementAll(['one','two', 'three'], 1)
    >>> a.getCount('one')
    1
    >>> a.getCount('two')
    1
    """
    for key in keys:
      self.incrementCount(key, count)
      
    return self
      
  def setCount(self, key, count):
    """
    Sets the count of key to the specified count and return the counter.
    """
    self[key] = count
    
    return self

  def getCount(self, key):
    """
    Returns the count of key, defaulting to zero.
    
    >>> a = Counter()
    >>> print a.getCount('test')
    0
    >>> a['test'] = 2
    >>> print a.getCount('test')
    2
    """
    if key in self:
      return self[key]
    else:
      return 0
  
  def argMax(self):
    """
    Returns the key with the highest value.
    """
    all = self.items()
    values = [x[1] for x in all]
    maxIndex = values.index(max(values))
    return all[maxIndex][0]
  
  def sortedKeys(self):
    """
    Returns a list of keys sorted by their values.  Keys
    with the highest values will appear first.
    
    >>> a = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> a['third'] = 1
    >>> a.sortedKeys()
    ['second', 'third', 'first']
    """
    sortedItems = self.items()
    compare = lambda x, y:  sign(y[1] - x[1])
    sortedItems.sort(cmp=compare)
    return [x[0] for x in sortedItems]
  
  def totalCount(self):
    """
    Returns the sum of counts for all keys.
    """
    return sum(self.values())
  
  def normalize(self):
    """
    Edits the counter such that the total count of all
    keys sums to 1.  The ratio of counts for all keys
    will remain the same. Note that normalizing an empty 
    Counter will result in an error.
    
    Return the counter as well.
    """
    total = float(self.totalCount())
    for key in self.keys():
      self[key] = self[key] / total
      
    return self

  def multiplyAll(self, multiplier):
    """
    Multiply all counts by multiplier in place and return counter.
    """
    multiplier = float(multiplier)
    for key in self:
      self[key] *= multiplier
      
    return self
      
  def divideAll(self, divisor):
    """
    Divides all counts by divisor in-place and return counter.
    """
    divisor = float(divisor)
    for key in self:
      self[key] /= divisor
    
    return self

  def componentwiseMultiply(self, counter):
    """
    Return a new counter which is obtained by the componentwise
    multiplication of the two counters.
    """
    result = Counter()
    for key in self:
        if not (key in counter):
            continue
        result[key] = self[key]*counter[key]
        
    return result

  def __mul__(self, y ):
    """
    Multiplying two counters gives the dot product of their vectors where
    each unique label is a vector element.
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['second'] = 5
    >>> a['third'] = 1.5
    >>> a['fourth'] = 2.5
    >>> a * b
    14
    """
    sum = 0
    for key in self:
      if not (key in y):
        continue
      sum += self[key] * y[key]      
    return sum
      
  def __iadd__(self, y):
    """
    Adding another counter to a counter increments the current counter
    by the values stored in the second counter.
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> a += b
    >>> a.getCount('first')
    1
    """ 
    for key, value in y.items():
      self.incrementCount(key, value)
      
    return self   
      
  def __add__( self, y ):
    """
    Adding two counters gives a counter with the union of all keys and
    counts of the second added to counts of the first.
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> (a + b).getCount('first')
    1
    """
    addend = Counter()
    for key in self:
      if key in y:
        addend[key] = self[key] + y[key]
      else:
        addend[key] = self[key]
    for key in y:
      if key in self:
        continue
      addend[key] = y[key]
    return addend
    
  def __sub__( self, y ):
    """
    Subtracting a counter from another gives a counter with the union of all keys and
    counts of the second subtracted from counts of the first.
    
    >>> a = Counter()
    >>> b = Counter()
    >>> a['first'] = -2
    >>> a['second'] = 4
    >>> b['first'] = 3
    >>> b['third'] = 1
    >>> (a - b).getCount('first')
    -5
    """      
    addend = Counter()
    for key in self:
      if key in y:
        addend[key] = self[key] - y[key]
      else:
        addend[key] = self[key]
    for key in y:
      if key in self:
        continue
      addend[key] = -1 * y[key]
    return addend
    
def sign( x ):
  """
  Returns 1 or -1 depending on the sign of x
  """
  if( x >= 0 ):
    return 1
  else:
    return -1

def factorial(m):
  t = 1
  if m != 0:
    while m != 1:
      t *= m
      m = m - 1
  return t

def choose(m, n):
  value = float(factorial(m)/(factorial(m-n)*factorial(n)))
  return value

def copyMatrix(m):
  """
  make a deep copy of matrix m
  """
  return [r[:] for r in m]

def getMatrixDims(m):
  """
  get dimensions of rectangular matrix m
  """
  rows = len(m)
  cols = len(m[0])
  return rows,cols

def raiseNotDefined():
  raise Exception("Method not implemented: %s" % inspect.stack()[1][3])    

def nearestPoint( pos ):
  """
  Finds the nearest grid point to a position (discretizes).
  """
  ( current_row, current_col ) = pos

  grid_row = int( current_row + 0.5 ) 
  grid_col = int( current_col + 0.5 ) 
  return ( grid_row, grid_col )     

def arrayInvert(array):
  """
  Inverts a matrix stored as a list of lists.
  """
  result = [[] for i in array]
  for outer in array:
    for inner in range(len(outer)):
      result[inner].append(outer[inner])
  return result

def matrixAsList( matrix, value = True ):
  """
  Turns a matrix into a list of coordinates matching the specified value
  """
  rows, cols = len( matrix ), len( matrix[0] )
  cells = []
  for row in range( rows ):
    for col in range( cols ):
      if matrix[row][col] == value:
        cells.append( ( row, col ) )
  return cells

import heapq
class PriorityQueue:
  """
    Implements a priority queue data structure. Each inserted item
    has a priority associated with it and the client is usually interested
    in quick retrieval of the lowest-priority item in the queue. This
    data structure allows O(1) access to the lowest-priority item.
  """
    
  def __init__(self):
    """
      heap: A binomial heap storing [priority,item]
      lists. 
      
      dict: Dictionary storing item -> [priorirty,item]
      maps so we can reach into heap for a given 
      item and update the priorirty and heapify
    """
    self.heap = []
    self.dict = {}
      
  def push(self,item,priority):
    """
        Sets the priority of the 'item' to
    priority. If the 'item' is already
    in the queue, then its key is changed
    to the new priority, regardless if it
    is higher or lower than the current 
    priority.
    """
    if item in self.dict:
      self.dict[item][0] = priority
      heapq.heapify(self.heap)
    else:
      pair = [priority,item]
      heapq.heappush(self.heap,pair)
      self.dict[item] = pair
      
  def getPriority(self,item):
    """
        Get priority of 'item'. If 
    'item' is not in the queue returns None
    """
    if not item in self.dict:
      return None
    return self.dict[item][0]
      
  def pop(self):
    """
      Returns lowest-priority item in priority queue, or
      None if the queue is empty
    """
    if self.isEmpty(): return None
    (priority,item) = heapq.heappop(self.heap)
    del self.dict[item]
    return item  
  
  def isEmpty(self):
    """
        Returns True if the queue is empty
    """
    return len(self.heap) == 0