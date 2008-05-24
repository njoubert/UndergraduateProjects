package temp;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.WritableComparable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.io.Text;

public class QueryReducer extends MapReduceBase implements Reducer {

	private Path queryPath;
	private String query;
	private String allValues;
    
    public void configure(JobConf job) {
    	try {
    		queryPath = (DistributedCache.getLocalCacheFiles(job))[0];
    	} catch (IOException e) {
    		System.err.println("Caught exception while getting cached files.\n");
		}
    	try {
        	BufferedReader readQuery = new BufferedReader(new FileReader(queryPath.toString()));
        	query = (readQuery.readLine()).toLowerCase();
        	StringTokenizer queryitr = new StringTokenizer(query);
        	while(queryitr.hasMoreTokens()) {
        		String keyword = queryitr.nextToken();
        		if (!keyword.equals("and") && !keyword.equals("or") && !keyword.equals("not"))
        			query = query.replaceAll(keyword, KeywordFilter.filter(keyword, null));
        	}
    	} catch (IOException e) {
    		System.err.println("Caught exception while reading query.\n");
    	}
    }
	
	public void reduce(WritableComparable key, Iterator values, OutputCollector output, Reporter reporter) throws IOException {
		boolean first = true;
        StringBuilder toReturn = new StringBuilder();
        while(values.hasNext()){
            if(!first)
                toReturn.append(' ');
            first=false;
            toReturn.append(values.next().toString());
        }
        allValues = toReturn.toString();
        StringTokenizer validqueryitr = new StringTokenizer(query);
        if (!validQuery(validqueryitr))
        	return;
        StringTokenizer checkqueryitr = new StringTokenizer(query);
        if (checkQuery(checkqueryitr, false)) {
        	output.collect(key, new Text(""));
        	//output.collect(key, new Text(allValues));
        }
	}
	
	private boolean validQuery(StringTokenizer remainingQuery) {
		while(remainingQuery.hasMoreTokens()) {
			String nextElement = remainingQuery.nextToken();
			if (nextElement.equals("not"))
				if (remainingQuery.hasMoreTokens())
					continue;
				else
					return false;
			else if (nextElement.equals("and") || nextElement.equals("or"))
				return false;
			else {
				if (remainingQuery.hasMoreTokens()) {
					String followingElement = remainingQuery.nextToken();
					if (!followingElement.equals("and") && !followingElement.equals("or") && !followingElement.equals("not"))
						return false;
					if (followingElement.equals("not"))
						return false;
					if (!remainingQuery.hasMoreTokens() && (followingElement.equals("and") || followingElement.equals("or")))
						return false;
				}
			}
		}
		return true;
	}
	
	private boolean checkQuery(StringTokenizer remainingQuery, boolean notFlag) {
		boolean prevItem = false;
		if (!remainingQuery.hasMoreTokens())
			return true;
		String nextElement = remainingQuery.nextToken();
		if (nextElement.equals("and") || nextElement.equals("or"))
			return false;
		else if (nextElement.equals("not"))
			return checkQuery(remainingQuery, !notFlag);
		else {
			StringTokenizer valueitr = new StringTokenizer(allValues);
			while(valueitr.hasMoreTokens()) 
				if ((valueitr.nextToken()).equals(nextElement)){
					prevItem = true;
					break;
				}
			if (!remainingQuery.hasMoreTokens())
				return ((!notFlag && prevItem) || (notFlag && !prevItem));
			nextElement = remainingQuery.nextToken();
			if (nextElement.equals("and"))
				return ((!notFlag && prevItem) || (notFlag && !prevItem)) && checkQuery(remainingQuery, false);
			else if (nextElement.equals("or"))
				return ((!notFlag && prevItem) || (notFlag && !prevItem)) || checkQuery(remainingQuery, false);
			else
				return false;
		}
	}
}
	