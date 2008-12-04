# Mira implementation
import util
import math
PRINT = True

class MiraClassifier:
  """
  Mira classifier.
  
  Note that the variable 'datum' in this code refers to a counter of features
  (not to a raw samples.Datum).
  """
  def __init__( self, legalLabels, max_iterations):
    self.legalLabels = legalLabels
    self.type = "mira"
    self.automaticTuning = False # Look at this flag to decide whether to choose C automatically ** use this in your train method **
    self.C = 0.001
    self.max_iterations = max_iterations
    self.weights = {}
    for label in legalLabels:
      self.weights[label] = util.Counter() # this is the data-structure you should use
  
  def train(self, trainingData, trainingLabels, validationData, validationLabels):
    """
    Outside shell to call your method. Do not modify this method.
    """  
      
    self.features = trainingData[0].keys() # this could be useful for your code later...
    
    if (self.automaticTuning):
        Cgrid = [0.001, 0.002, 0.003, 0.004, 0.005]
    else:
        Cgrid = [self.C]
        
    return self.trainAndTune(trainingData, trainingLabels, validationData, validationLabels, Cgrid)

  def score( self, datum, weights):
    vectors = util.Counter()
    for l in self.legalLabels:
      vectors[l] = weights[l] * datum  
    return vectors

  def calculateTau(self, c, wy, wyp, f):
    sumsquares = 0
    for key in f.keys():
      sumsquares += f.getCount(key)*f.getCount(key)
    subtracted = wyp - wy
    tau = float(subtracted * f + 1) / float(2*sumsquares)
    return min(c, tau)

  def trainAndTune(self, trainingData, trainingLabels, validationData, validationLabels, Cgrid):
    """
    See the project description for details how to update weight vectors for each label in training step. 
    
    Use the provided self.weights[label] datastructure so that 
    the classify method works correctly. Also, recall that a
    datum is a counter from features to values for those features
    (and thus represents a vector a values).

    This method needs to return the best parameter found in the list of parameters Cgrid
    (i.e. the parameter that yeilds best accuracy for the validation dataset)
    """
    # YOUR CODE HERE
    self.features = trainingData[0].keys()
    
    weightsList = []
      
    for c in Cgrid:
      weights = {}
      for label in self.legalLabels:
        weights[label] = util.Counter() # this is the data-structure you should use

      for iteration in range(self.max_iterations):
        print "Starting iteration ", iteration, " for c", c
        for i in range(len(trainingData)):
          ## YOUR CODE HERE
          y = trainingLabels[i]
          score = self.score(trainingData[i], weights)
          yprime = score.argMax()
          if yprime != y:
            #we got it wrong, so update...
            tau = self.calculateTau(c, weights[y], weights[yprime], trainingData[i])
            trainingDataTemp = trainingData[i]
            trainingDataTemp.multiplyAll(tau)
            weights[y] = weights[y] + trainingDataTemp
            weights[yprime] = weights[yprime] - trainingDataTemp
            
      weightsList.append((c, weights))
    
    bestAccuracy = float(0)
    for (c, weights) in weightsList:
      
      tempW = self.weights
      tempC = self.C
      
      self.weights = weights
      self.C = c
      
      guessedLabels = self.classify(validationData)
      netright = 0
      for i in range(len(guessedLabels)):
        if (guessedLabels[i] == validationLabels[i]):
          netright += 1
      accuracy = float(netright) / len(guessedLabels)
      if (accuracy < bestAccuracy or (accuracy == bestAccuracy and tempC < c)):
        self.weights = tempW
        self.C = tempC  
      else:
        bestAccuracy = accuracy
      
      print "  Validating accuracy of c = ", c, "gives us", accuracy*100, "%"
    
    
    print "  We picked c=", self.C    
    return self.C

  def classify(self, data ):
    """
    Classifies each datum as the label that most closely matches the prototype vector
    for that label.  See the project description for details.
    
    Recall that a datum is a util.counter... 
    """
    guesses = []
    for datum in data:
      vectors = util.Counter()
      for l in self.legalLabels:
        vectors[l] = self.weights[l] * datum
      guesses.append(vectors.argMax())
    return guesses

  
  def findHighOddsFeatures(self, class1, class2):
    """
    Returns:
    featuresClass1 -- the 100 largest weight features for class1 (as a list)
    featuresClass2 -- the 100 largest weight features for class2
    featuresOdds -- the 100 best features for difference in feature values
                     w_class1 - w_class2

    """

    featuresClass1 = self.weights[class1].sortedKeys()
    featuresClass2 = self.weights[class2].sortedKeys()
    featuresOdds = self.weights[class1] - self.weights[class2]
    featuresOdds = featuresOdds.sortedKeys()

    return featuresClass1[0:100],featuresClass2[0:100],featuresOdds[0:100]


