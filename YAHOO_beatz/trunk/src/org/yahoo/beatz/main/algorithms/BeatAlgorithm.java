package org.yahoo.beatz.main.algorithms;

public interface BeatAlgorithm {

	/**
	 * Analyzes the channel and sets the internal variables to find beats.
	 * @return
	 */
	public abstract void analyze();
	
	public abstract void setVerbose(boolean verbose);
	
	public abstract double[] getBins();

}