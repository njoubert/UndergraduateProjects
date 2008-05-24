package temp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.*;
import org.apache.hadoop.io.Text;
import java.util.StringTokenizer;

public class QueryMapper extends MapReduceBase implements Mapper {
	private Path queryPath;
	private String keywords;
    
    public void configure(JobConf job) {
    	try {
    		queryPath = (DistributedCache.getLocalCacheFiles(job))[0];
    	} catch (IOException e) {
    		System.err.println("Caught exception while getting cached files.\n");
		}
    	try {
        	BufferedReader readQuery = new BufferedReader(new FileReader(queryPath.toString()));
        	keywords = (readQuery.readLine()).toLowerCase();
        	keywords = keywords.replaceAll(" and ", " ");
        	keywords = keywords.replaceAll(" or ", " ");
        	keywords = keywords.replaceAll(" not ", " ");
        	StringTokenizer queryitr = new StringTokenizer(keywords);
        	while(queryitr.hasMoreTokens()) {
        		String keyword = queryitr.nextToken();
        		keywords = keywords.replaceAll(keyword, KeywordFilter.filter(keyword, null));
        	}
    	} catch (IOException e) {
    		System.err.println("Caught exception while reading query.\n");
    	}
    }
	
	public void map(WritableComparable key, Writable values, OutputCollector output, Reporter reporter) throws IOException {
		String line = values.toString();
		StringTokenizer keyworditr = new StringTokenizer(keywords);
		StringTokenizer keyitr = new StringTokenizer(line);
		String word = keyitr.nextToken();
		while(keyworditr.hasMoreTokens()) {
			String keyword = keyworditr.nextToken();
			if (word.equals(keyword)){
				String allValues = line.substring(word.length() + 1);
				StringTokenizer valueitr = new StringTokenizer(allValues, "^");
				while(valueitr.hasMoreTokens()) {
					String value = valueitr.nextToken();
					output.collect(new Text(value), new Text(word));
				}
				break;
			}
		}
	}
}