package org.yahoo.beatz.main.algorithms;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.imageio.stream.FileImageOutputStream;

public class RmsStdDevCutter implements BeatAlgorithm {

	
	private static final int milliSecsPerBin = 100;
	private boolean verbose;
	
	private short[] channel;
	private int sampleRate;
	private int bitsPerSample;
	private int samplesPerChannel;
	private int samplesPerBin;
	private double[] bins = null;
	private double[] beats = null;
	
	/**
	 * Creates a new algorithmic analyzer
	 * @param channel - the samples to analyze
	 * @param sampleRate - the samples per second in this channel
	 * @param bitsPerSample
	 * @param numOfSamples - should be the same as channel.lenth
	 */
	public RmsStdDevCutter(short[] channel, int sampleRate, int bitsPerSample, int samplesPerChannel) {
		this.channel = channel;
		this.sampleRate = sampleRate;
		this.bitsPerSample = bitsPerSample;
		this.samplesPerChannel = samplesPerChannel;
	}
	
	/* (non-Javadoc)
	 * @see org.yahoo.beatz.main.algorithms.BeatAlgorithm#analyze()
	 */
	public void analyze() {
		
		channel = lowpassFilter(channel);
		
		samplesPerBin = (int) ((double) sampleRate / 1000.0 * milliSecsPerBin);
		int numOfBins = (int) ((double) samplesPerChannel / (double) samplesPerBin);
		verbose("Samples in Channel: " + samplesPerChannel);
		verbose("Samples per Bin: " + samplesPerBin);
		verbose("Number of Bins: " + numOfBins);
		double[] bins = new double[numOfBins];
		int offset;
		for (int binCounter = 0; binCounter < numOfBins; binCounter++) {
			offset = binCounter * samplesPerBin;
			
			double total = 0;
			for (int sampleCounter = offset; sampleCounter < offset + samplesPerBin; sampleCounter++) {
				total += Math.pow(channel[sampleCounter], 2);
			}
			bins[binCounter] = Math.sqrt(total/samplesPerBin);
			verbose("Processing bin " + binCounter + " starting at sample " + offset + ": Value found = " + bins[binCounter]);
		}
		
		
		this.bins = bins;
		double mean = calculateMean(bins);
		double stddev = calculateStdDev(bins, mean);
		
		beats = new double[bins.length];
		for (int binCounter = 0; binCounter < numOfBins; binCounter++) {
			if (bins[binCounter] > 1.5*stddev + mean)
				beats[binCounter] = bins[binCounter];
			else
				beats[binCounter] = 0;
		}
		
	}
	
	
	private short[] lowpassFilter(short[] channel) {
		short[] lowpass = new short[channel.length];
		
		for (int i = 1; i < channel.length; i++) {
			lowpass[i] = (short) ((channel[i-1] + channel[i])/2);
		}
		
		return lowpass;
		
	}
	
	private double calculateMean(double[] bins) {
		double mean = 0;
		for (int i = 0; i < bins.length; i++)
			mean += bins[i];
		mean = mean / bins.length;
		return mean;
	}
	
	private double calculateStdDev(double[] bins, double mean) {
		double stddev = 0;
		for (int binCounter = 0; binCounter < bins.length; binCounter++) {
			stddev += Math.pow(bins[binCounter] - mean, 2);
		}
		stddev = Math.sqrt(stddev / bins.length);
		verbose("Calculated Std Deviation to be " + stddev);
		return stddev;
		
	}
	
	private void verbose(String msg) {
		if (verbose)
			System.out.println(msg);
	}

	public void setVerbose(boolean verbose) {
		this.verbose = verbose;
	}
	public double[] getBins() {
		return bins;
	}

	public int getSamplesPerBin() {
		return samplesPerBin;
	}

	public double[] getBeats() {
		return beats;
	}
	
	public List<Integer> getTimestampList() {
		if (beats == null)
			return null;
		List<Integer> list = new ArrayList<Integer>();
		int time = 0;
		for (int counter = 0; counter < beats.length; counter++) {
			if (beats[counter] > 0)
				list.add(time);
			time += milliSecsPerBin;
		}
		return list;
	}
	
	public Map<Integer, Double> getTimestampMap() {
		if (beats == null)
			return null;
		Map<Integer, Double> map = new HashMap<Integer, Double>();
		int time = 0;
		for (int counter = 0; counter < beats.length; counter++) {
			if (beats[counter] > 0)
				map.put(time, beats[counter]);
			time += milliSecsPerBin;
		}
		return map;
	}
	
}
