package temp;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.Reducer;

public class LineIndexerDriver {

	public static void main(String[] args) throws Exception {
		JobClient client = new JobClient();
		JobConf conf = new JobConf(LineIndexerDriver.class);
		conf.setJobName("LineIndexer");
		
		// The keys are words (strings):
		conf.setOutputKeyClass(Text.class);
	    // The values are offsets+line (strings):
		conf.setOutputValueClass(Text.class);

		conf.setInputPath(new Path(args[0]));
		conf.setOutputPath(new Path(args[1]));

		conf.setMapperClass(LineIndexerMapper.class);

		conf.setReducerClass(LineIndexerReducer.class);

		if (args.length < 2) {
	        System.out.println("Usage: LineIndexer <input path> <output path>");
	        System.exit(0);
	    }
	    
		client.setConf(conf);
		try {
			JobClient.runJob(conf);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
