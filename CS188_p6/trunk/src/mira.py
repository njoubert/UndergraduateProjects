# Mira implementation
import util
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

    featuresClass1 = []
    featuresClass2 = []
    featuresOdds = []

    return featuresClass1,featuresClass2,featuresOdds

