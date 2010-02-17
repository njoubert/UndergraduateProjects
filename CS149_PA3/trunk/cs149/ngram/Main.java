package cs149.ngram;


import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInput;
import java.io.DataOutput;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.util.ArrayList;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.*;
import org.apache.hadoop.mapreduce.Reducer.Context;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;


public class Main {
	
	public static class HTMLInputFormat extends FileInputFormat<Text,Text> {

		@Override
		public RecordReader<Text, Text> createRecordReader(InputSplit split,
				TaskAttemptContext arg1) throws IOException,
				InterruptedException {
			PageRecordReader reader = new PageRecordReader();
			reader.initialize(split, arg1);
			return reader;
		}
		
		protected boolean isSplitable() {
			return false;
		}
		
	}
	
	
	/**
	 * A record consisting of a single page. The input should be a single file?
	 * @author njoubert
	 *
	 */
	public static class PageRecordReader extends RecordReader<Text,Text> {

		private FileSplit split;
		private FileSystem fs;
		private long offset;
		private long totLength;
		
		private FSDataInputStream currentStream;
	    private BufferedReader currentReader;
	    
	    private String currentPageTitle = null;
	    
	    private String key;
	    private String value;
	    
		@Override
		public void initialize(InputSplit arg0, TaskAttemptContext context)
				throws IOException, InterruptedException {
			this.split = (FileSplit) arg0;
			fs = FileSystem.get(context.getConfiguration());
			this.offset = split.getStart();
			this.totLength = split.getLength();
			
		    currentStream = fs.open(split.getPath());
		    currentStream.seek(offset);
		    currentReader = new BufferedReader(new InputStreamReader(currentStream));
		    key = "randommapkey";
		}

		@Override
		public void close() throws IOException { }


		@Override
	    public float getProgress() throws IOException {
			return ((float)getPos()) / totLength;
		}

		private float getPos() throws IOException {
			long currentOffset = currentStream == null ? 0 : currentStream.getPos();
		    return offset + currentOffset;
		}

		@Override
		public boolean nextKeyValue() throws IOException, InterruptedException {
			//assume we have an buffer that we can read from. trying to find a page.
			
//			String line;
//			int pageStart;
//			while ((line = currentReader.readLine()) != null) {
//				if ((pageStart = line.indexOf("<title>")) != -1) {
//					//found a title
//				
//					 
//				}
//				
//				
//			}
			
			
			
			int count = 0;
			String line = null;
			String output = "";
			while (count < 10 && (line = currentReader.readLine()) != null) {
				output += line;
			}
			if (line == null)
				return false;
			value = output;
			return true;
			
		}
		@Override
		public Text getCurrentKey() throws IOException, InterruptedException {
			return new Text(key);
		}

		@Override
		public Text getCurrentValue() throws IOException, InterruptedException {
			return new Text(value);
		}
		
		
	}
	
	
	
	
	/**
	 * Generates n-grams of length n from tokens in tokens.
	 * @param tokens
	 * @param n
	 * @return arraylist of n-grams etc. "
	 */
	public static ArrayList<String> getNGrams(ArrayList<String> tokens, int n) {
		ArrayList<String> ngrams = new ArrayList<String>();
		for (int i = 0; i < tokens.size() - n; i++) {
			StringBuffer toWrite = new StringBuffer();
			for(int j=0; j<n; j++) {
				toWrite.append(tokens.get(i+j));
				if(j != n-1) {
					toWrite.append(' ');
				}
			}
			ngrams.add(toWrite.toString());
		}
		return ngrams;
		
	}
	
	public static class NGramMapper extends Mapper<Text, Text, Text, Text> {
		private ArrayList<String> queryNGrams;
		private int n;
		
		public void setup(Context context) {
			Configuration conf = context.getConfiguration();
			String query = conf.get("query");
			n = conf.getInt("number", -1);
			Tokenizer tok = new Tokenizer(query);
			ArrayList<String> tokens = new ArrayList<String>();
			while (tok.hasNext()) {
				tokens.add(tok.next());
			}
			queryNGrams = getNGrams(tokens, n);
		}
		
		public void map(Text key, Text value, Context context) throws IOException, InterruptedException {
			String doc = value.toString();
			int index = 0;
			System.out.println("Mapper got value: " + doc);
			int start = doc.indexOf("<title>",index);
			
			String bestPage = null;
			int bestPageScore = -1;
			
			while(start != -1) {
					
				int end = doc.indexOf("</title>", start);
				String pageTitle = doc.substring(start+7, end);
				int nextTitle = doc.indexOf("<title>", end);
				String page;
				if(nextTitle == -1) {
					page = doc.substring(end+8);
				} else {
					page = doc.substring(end+8,nextTitle);
				}
				
				//Do shit with page
				Tokenizer tok = new Tokenizer(page);
				ArrayList<String> tokens = new ArrayList<String>();
				while (tok.hasNext()) {
					tokens.add(tok.next());
				}
				ArrayList<String> myNGrams = getNGrams(tokens, n);
				int score = 0;
				for(String gram : myNGrams) {	
					if(queryNGrams.contains(gram)) {
						score++;
					}
				}
				
				if (bestPageScore < score || (bestPageScore == score && (bestPage == null || bestPage.compareTo(pageTitle) < 0))) {
					bestPage = pageTitle;
					bestPageScore = score;
				}
				
				start = nextTitle;
					
			}
			
			context.write(new Text("best pages"), new Text(bestPageScore + " " + bestPage));
		
		}
		
	}
	
	public static class NGramReducer extends Reducer<Text,Text,Text,IntWritable> {
		
		public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
		    
			String bestPage = null;
			int bestPageScore = -1;
			
		      for (Text val : values) {
		    	  String v = new String(val.toString());
		    	  int score = Integer.parseInt(v.substring(0, v.indexOf(" ")));
		    	  String pageTitle = v.substring(v.indexOf(" ")+1);
		    	  
					if (bestPageScore < score || (bestPageScore == score && (bestPage == null || bestPage.compareTo(pageTitle) < 0))) {
						bestPage = pageTitle;
						bestPageScore = score;
					}
//
//				    IntWritable result = new IntWritable(score);
//					context.write(new Text(pageTitle), result);

		      }
		      IntWritable result = new IntWritable(bestPageScore);
		      context.write(new Text(bestPage), result);
		}
	}
	
	private static String readFileAsString(String filePath) throws java.io.IOException {
	    byte[] buffer = new byte[(int) new File(filePath).length()];
	    BufferedInputStream f = new BufferedInputStream(new FileInputStream(filePath));
	    f.read(buffer);
	    return new String(buffer);
	}

	
	/**
	 * @param args
	 */
	public static void main(String[] args) {		
		if (args.length != 4) {
			System.out.println("USAGE: <n>, <query file>, <input dir>, <output dir>");
			System.exit(1);
		}

		/*
		 * Grabbing arguments
		 */
		
		int n = Integer.parseInt(args[0]);
		String queryFile = args[1];
		String inputDir = args[2];
		String outputDir = args[3];
		System.out.println("Calculating " + n + "-grams from " + queryFile + ", using input " + inputDir + " and output " + outputDir);
		
		String queryString = "";
		try {
			queryString = readFileAsString(queryFile);
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("You gave us an incorrect query file path, so we will now crash.");
			System.exit(1);
		}
		
		
		Configuration conf = new Configuration();
	    conf.setInt("number", n);
	    conf.set("query", queryString);
		
		Job job;
		
		try {
			job = new Job(conf, "ngramifier");
		    job.setJarByClass(Main.class);
		    job.setMapperClass(NGramMapper.class);
		    job.setReducerClass(NGramReducer.class);
		    job.setOutputKeyClass(Text.class);
		    job.setOutputValueClass(Text.class);
		    job.setInputFormatClass(FileInputFormat.class);
		    FileInputFormat.addInputPath(job, new Path(inputDir));
		    FileOutputFormat.setOutputPath(job, new Path(outputDir));
		    System.exit(job.waitForCompletion(true) ? 0 : 1);
		
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Your IO is messed up, so we will now crash.");
		} catch (InterruptedException e) {
			e.printStackTrace();
			System.out.println("You interrupted us, so we will now crash.");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			System.out.println("You made us not find a class, so we will now crash.");
		}
	}

}
