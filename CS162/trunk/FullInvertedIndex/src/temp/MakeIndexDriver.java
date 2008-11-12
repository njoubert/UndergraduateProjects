package temp;

import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;

public class MakeIndexDriver {

	public static void main(String[] args) throws Exception {
		JobClient client = new JobClient();
		JobConf conf = new JobConf(MakeIndexDriver.class);
		conf.setJobName("MakeIndex");
		
		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(Text.class);

		conf.setMapperClass(MakeIndexMapper.class);
		conf.setReducerClass(MakeIndexReducer.class);
		
		if (args.length < 3) {
			System.out.println("Usage: MakeIndex <input path> <output path> <stopwordsFile>");
			System.exit(0);
		}
	    
		conf.setInputPath(new Path(args[0]));
		conf.setOutputPath(new Path(args[1]));
		
		DistributedCache.addCacheFile(new Path(args[2]).toUri(), conf);
		
		client.setConf(conf);
		try {
			JobClient.runJob(conf);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}