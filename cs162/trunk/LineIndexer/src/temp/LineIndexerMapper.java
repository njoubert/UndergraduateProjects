package temp;

import java.io.IOException;

import org.apache.hadoop.io.Writable;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.io.Text;
import java.util.StringTokenizer;

public class LineIndexerMapper extends MapReduceBase implements Mapper {
	private final static Text word = new Text();
	private final static Text summary = new Text();
	
	public void map(WritableComparable key, Writable values, OutputCollector output, Reporter reporter) throws IOException {
		String line = values.toString();
		summary.set(key.toString() + ":" + line);
		StringTokenizer itr = new StringTokenizer(line.toLowerCase());
		while(itr.hasMoreTokens()) {
			word.set(itr.nextToken());
			output.collect(word, summary);
		}
	}
}