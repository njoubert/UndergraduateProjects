package temp;

import java.io.IOException;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.io.Text;
import java.util.StringTokenizer;

public class MakeIndexMapper extends MapReduceBase implements Mapper {
	private final static Text word = new Text();
	private final static Text summary = new Text();
	
	private String inputFile;
	private  Path stopwordsPath;
	
	public void configure(JobConf job) {
		try {
			stopwordsPath = (DistributedCache.getLocalCacheFiles(job))[0];
    	} catch (IOException e) {
    		System.err.println("Caught exception while getting cached files.\n");
		}
		inputFile = job.get("map.input.file");
		inputFile = inputFile.substring(inputFile.lastIndexOf("/")+1);
	}
	
	public void map(WritableComparable key, Writable values, OutputCollector output, Reporter reporter) throws IOException {
		String line = values.toString();
		StringTokenizer itr = new StringTokenizer(line.toLowerCase());
		while(itr.hasMoreTokens()) {
			String scrubbed = KeywordFilter.filter(itr.nextToken(), stopwordsPath);
			if (scrubbed.equals(""))
				continue;
			else{
				word.set(scrubbed);
				summary.set(inputFile + ":" + key.toString() + ":" + line);
				output.collect(word, summary);
			}
		}
	}
}