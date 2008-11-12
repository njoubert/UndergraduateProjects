package temp;

import java.io.IOException;
import java.util.Iterator;

import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.io.Text;


public class LineIndexerReducer extends MapReduceBase implements Reducer {

	public void reduce(WritableComparable key, Iterator values, OutputCollector output, Reporter reporter) throws IOException {
		boolean first = true;
        StringBuilder toReturn = new StringBuilder();
        while(values.hasNext()){
            if(!first)
                toReturn.append('^');
            first=false;
            toReturn.append(values.next().toString());
        }
        output.collect(key, new Text(toReturn.toString()));
	}

}
