package org.yahoo.beatz.main;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Stroke;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.ByteOrder;
import java.util.List;
import java.util.Map;

import javax.imageio.stream.FileImageInputStream;
import javax.swing.JComponent;
import javax.swing.JFrame;

import org.yahoo.beatz.main.algorithms.BeatAlgorithm;
import org.yahoo.beatz.main.algorithms.RmsStdDevCutter;


public class BeatsAnalyzer {

	private File wavFile;
	private boolean verbose;
	private FileImageInputStream istream;
	
	private int chunkId;
	private int chunkSize;
	private int format;
	private int subChunk1Id;
	private int subChunk1Size;
	private short audioFormat;
	private short numChannels;
	private int sampleRate;
	private int byteRange;
	private short blockAlign;
	private short bitsPerSample;
	private int subChunk2Id;
	private int subChunk2Size; 
	private int samplesPerChannel;
	
	private short[] left;
	private short[] right;
	
	private int samplesPerBin;
	private double[] leftbeats;
	private double[] rightbeats;
	
	private JFrame frame;
	private WaveComponent comp;
	
	                   
	public BeatsAnalyzer(File wavFile, boolean verbose, boolean gui, boolean writeFile) {
		this.verbose = verbose;
		
		minimal("Initializing BeatsAnalyzer\n");
		verbose("File: " + wavFile.getAbsoluteFile());
		setWavFile(wavFile);
		
		if (gui)
			makeGui();
		
		RmsStdDevCutter algorithm = new RmsStdDevCutter(left, sampleRate, (int) bitsPerSample, samplesPerChannel);
		algorithm.setVerbose(verbose);
		algorithm.analyze();
		leftbeats = algorithm.getBeats();
		this.samplesPerBin = algorithm.getSamplesPerBin();
		if (gui) {
			comp.setRMS(algorithm.getBins());
			comp.setBeats(true);
		}
		
		
		
		if (writeFile)
			try {
				minimal("Writing output file...");
				File textoutput = new File(wavFile.getAbsoluteFile() + ".beatz");
				FileOutputStream ostream = new FileOutputStream(textoutput);
				OutputStreamWriter writer = new OutputStreamWriter(ostream);
//				Map<Integer, Double> map = algorithm.getTimestampMap();
//				for (Integer key : map.keySet()) {
//					writer.write(key + " - " + map.get(key) + "\n");
//				}
				List<Integer> ts = algorithm.getTimestampList();
				for (Integer el : ts) {
					writer.write(((double)el)/1000 + "\n");
				}
				writer.close();
				minimal("Output file written to " + textoutput.getAbsolutePath());
			} catch (Exception e) {
				System.err.println(e);
			}
		
		
		minimal("BeatsAnalyzer successfully processed file.");
	}

	/**
	 * Reads in and verifies header of given wave file.
	 * @throws IOException if anything is not correct, with the right message.
	 */
	private void checkWavFile() throws IOException{
		if (wavFile == null)
			return;

		istream = new FileImageInputStream(wavFile); 
		istream.setByteOrder(ByteOrder.BIG_ENDIAN);
		chunkId = istream.readInt();
		istream.setByteOrder(ByteOrder.LITTLE_ENDIAN);
		chunkSize = istream.readInt();
		istream.setByteOrder(ByteOrder.BIG_ENDIAN);
		format = istream.readInt();
		subChunk1Id = istream.readInt();
		istream.setByteOrder(ByteOrder.LITTLE_ENDIAN);
		subChunk1Size = istream.readInt();
		audioFormat = istream.readShort();
		numChannels = istream.readShort();
		sampleRate = istream.readInt();
		byteRange = istream.readInt();
		blockAlign = istream.readShort();
		bitsPerSample = istream.readShort();
		
		if (!(chunkId == 0x52494646)) 
			throw new IOException("First chunkId is not valid. Expected RIFF");
		verbose("chunkId == RIFF");
		if (!(chunkSize + 8 == wavFile.length()))
			throw new IOException("File size does not match chunkSize");
		verbose("chunkSize + 8 == wave size");
		if (!(format == 0x57415645))
			throw new IOException("File was not WAVE format");	
		verbose("format == WAVE");
		if (!(subChunk1Id == 0x666d7420))
			throw new IOException("Subchunk 1 id is not valid. Excected fmt");
		verbose("subChunk1Id == fmt");
		if (!(subChunk1Size == 16))
			throw new IOException("Subchunk 1 size is not 16. Can't read this wave.");
		verbose("subChunk1Size == 16");
		if (!(audioFormat == 1))
			throw new IOException("Compressed WAV files are not supported.");
		verbose("PCM Wave File == true");
		verbose("numChannels = " + numChannels);
		verbose("sampleRate = " + sampleRate);
		verbose("byteRange = " + byteRange);
		verbose("blockAlign = " + blockAlign);
		verbose("bitsPerSample = " + bitsPerSample);
		
		istream.setByteOrder(ByteOrder.BIG_ENDIAN);
		subChunk2Id = istream.readInt();
		istream.setByteOrder(ByteOrder.LITTLE_ENDIAN);
		subChunk2Size = istream.readInt();
		
		samplesPerChannel = subChunk2Size / blockAlign;
		
		if (!(subChunk2Id == 0x64617461))
			throw new IOException("Subchunk 2 id is not valid. Expected data");
		verbose("subChunk2Id == data");
		verbose("SubChunk2 Size: " + subChunk2Size);
		verbose("File size: " + wavFile.length());
		minimal("Valid Wave File Header.\n");

	}
	
	private void readWavFile() throws IOException {
		if (istream == null)
			throw new IOException("No InputStream to read from!");
		minimal("Reading wavefile...");
		left = new short[samplesPerChannel];
		right = new short[samplesPerChannel];
		int sampleCounter = 0;
		int percentage = 0;
		if (bitsPerSample == 16) {
			while (sampleCounter < samplesPerChannel) {
				if (verbose && (int) (((double) sampleCounter / (double) samplesPerChannel) * 100) > percentage) {
					percentage = (int) (((double) sampleCounter / (double) samplesPerChannel) * 100);
					verbose(percentage + "% done");
				}
				left[sampleCounter] = istream.readShort();
				if (numChannels == 2)
					right[sampleCounter] = istream.readShort();
				sampleCounter += 1;
			}
		} else if (bitsPerSample == 8) {
			while (sampleCounter < samplesPerChannel) {
				if (verbose && (int) (((double) sampleCounter / (double) samplesPerChannel) * 100) > percentage) {
					percentage = (int) (((double) sampleCounter / (double) samplesPerChannel) * 100);
					verbose(percentage + "% done");
				}
				left[sampleCounter] = (short) istream.readByte();
				if (numChannels == 2)
					right[sampleCounter] = (short) istream.readByte();
				sampleCounter += 1;
			}			
		}
		minimal("Reading done!");

		
	}
	
	private void writeHeader() {
		
	}

	private void setWavFile(File wavFile) {
		this.wavFile = wavFile;
		try {
			checkWavFile();
			readWavFile();
		} catch (IOException e) {
			istream = null;
			this.wavFile = null;
			System.err.println(e);
			System.out.println("File check failed. Aborted analysis");
		}	
	}
	
	private void makeGui() {
		frame = new JFrame();
		comp = new WaveComponent();
		frame.setSize(new Dimension(800, 600));
		frame.add(comp);
		frame.setVisible(true);
		
	}
	
	public class WaveComponent extends JComponent {

		private double[] rms;
		private boolean beatsAvailable = false;
		
		protected void paintComponent(Graphics g) {
			
			Color blue = new Color(44, 117, 102);
			Color red = new Color(164, 33, 33);
			Graphics2D g2 = (Graphics2D) g;
			double norm = (this.getHeight()-2)/2/Math.pow(2, bitsPerSample);
			double factor = this.getWidth()/(double)samplesPerChannel;
			g2.setStroke(new BasicStroke(3));
			g2.setColor(Color.WHITE);
			g2.fillRect(0, 0, this.getWidth(), this.getHeight());
			g2.setColor(blue);
			g2.setStroke(new BasicStroke(1));
			int pos = 0;
			
			for (int i = 1; i<left.length*factor-1; i++) {
				g2.drawLine(pos, (int) (left[(int)((i-1)/factor)]*norm + this.getHeight()/4), ++pos, (int) (left[(int)(i/factor)]*norm + this.getHeight()/4));
			}
			g2.setColor(red);
			pos = 0;
			for (int i = 1; i<right.length*factor-1; i++) {
				g2.drawLine(pos, (int) (right[(int)((i-1)/factor)]*norm + 3*this.getHeight()/4), ++pos, (int) (right[(int)(i/factor)]*norm + 3*this.getHeight()/4));
			}
			
			g2.setStroke(new BasicStroke(1));
			g2.setColor(Color.GREEN);
			pos = 0;
			if (samplesPerBin > 0) {
				factor = this.getWidth()/(double)rms.length;
				for (int i = 1; i<rms.length*factor-1;i++) {
					g2.drawLine(pos, (int) (this.getHeight() - (rms[(int)((i-1)/factor)]*norm)), ++pos, (int) (this.getHeight() - (rms[(int)((i)/factor)]*norm)));
				}
			}
			pos = 0;
			g2.setStroke(new BasicStroke(3));
			g2.setColor(Color.BLACK);
			if (beatsAvailable) {
				factor = this.getWidth()/(double)leftbeats.length;
				for (int i = 1; i<leftbeats.length*factor-1;i++) {
					g2.drawLine(pos, (int) (this.getHeight() - (leftbeats[(int)((i-1)/factor)]*norm)), ++pos, (int) (this.getHeight() - (leftbeats[(int)((i)/factor)]*norm)));
				}				
			}
			g2.setStroke(new BasicStroke(3));
			g2.setColor(Color.BLACK);
			g2.drawRect(1, 1, this.getWidth()-3, this.getHeight()/2);
			g2.drawRect(1, this.getHeight()/2, this.getWidth()-3, this.getHeight()/2-1);
			
		}
		
		public void setRMS(double[] rms) {
			this.rms = rms;
			repaint();
		}
		
		public void setBeats(boolean available) {
			this.beatsAvailable = available;
			repaint();
		}
		
	}

	
	private void verbose(String msg) {
		if (verbose)
			System.out.println(msg);
	}
	private void minimal(String msg) {
		System.out.println(msg);
	}
	
}
