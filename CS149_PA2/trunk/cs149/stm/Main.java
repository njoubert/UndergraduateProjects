package cs149.stm;

import java.util.SortedSet;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Main {

    private static final int DEFAULT_KEY_RANGE = 10000;
    private static final int DEFAULT_READ_PCT = 95;
    private static final String DEFAULT_THREADS_PATTERN = "1..16";
    private static final int DEFAULT_WARMUP_MILLIS = 1000;
    private static final int DEFAULT_TIMED_MILLIS = 5000;

    private static final IntSet[] implementations = {
            new CoarseLockTreap(),
            //new STMTreap(),
    };

    private int _keyRange = DEFAULT_KEY_RANGE;
    private int _readPct = DEFAULT_READ_PCT;
    private SortedSet<Integer> _threadCounts = parseCountPattern(DEFAULT_THREADS_PATTERN);
    private int _warmupMillis = DEFAULT_WARMUP_MILLIS;
    private int _timedMillis = DEFAULT_TIMED_MILLIS;

    // constructor parses the parameters
    public Main(final String[] args) {
        for (String mainArg : args) {
            final int p = mainArg.indexOf('=');
            if (p < 0) {
                exitUsage();
            }
            final String paramName = mainArg.substring(0, p);
            final String paramArg = mainArg.substring(p + 1);
            if (paramName.equals("--key-range")) {
                _keyRange = Integer.parseInt(paramArg);
            }
            else if (paramName.equals("--read-pct")) {
                _readPct = Integer.parseInt(paramArg);
            }
            else if (paramName.equals("--threads")) {
                _threadCounts = parseCountPattern(paramArg);
            }
            else if (paramName.equals("--warmup")) {
                _warmupMillis = Integer.parseInt(paramArg);
            }
            else if (paramName.equals("--timed")) {
                _timedMillis = Integer.parseInt(paramArg);
            }
            else {
                exitUsage();
            }
        }
    }

    private static final Pattern powersPattern = Pattern.compile("(\\d+)\\.\\.(\\d+)");
    private static final Pattern rangePattern = Pattern.compile("(\\d+)-(\\d+)");

    private static SortedSet<Integer> parseCountPattern(final String pat) {
        final SortedSet<Integer> result = new TreeSet<Integer>();

        final int comma = pat.indexOf(',');
        if (comma >= 0) {
            result.addAll(parseCountPattern(pat.substring(0, comma)));
            result.addAll(parseCountPattern(pat.substring(comma + 1)));
            return result;
        }

        final Matcher pMatch = powersPattern.matcher(pat);
        if (pMatch.matches()) {
            final int min = Integer.parseInt(pMatch.group(1));
            final int max = Integer.parseInt(pMatch.group(2));
            for (int n = 1; n <= max; n *= 2) {
                if (n >= min) {
                    result.add(n);
                }
            }
            return result;
        }

        final Matcher rMatch = rangePattern.matcher(pat);
        if (rMatch.matches()) {
            final int min = Integer.parseInt(rMatch.group(1));
            final int max = Integer.parseInt(rMatch.group(2));
            for (int n = min; n <= max; ++n) {
                result.add(n);
            }
            return result;
        }

        result.add(Integer.parseInt(pat));
        return result;
    }

    private static void exitUsage() {
        System.out.println(
                "Usage:\n" +
                "  java -javaagent:deuceAgent-1.2.0.jar " + Main.class.getName() + " [options]\n" +
                "Where options are:\n" +
                "  --key-range=N     The number of unique keys to use (default " + DEFAULT_KEY_RANGE + ")\n" +
                "  --read-pct=N      The percentage of ops that are reads (default " + DEFAULT_READ_PCT + ")\n" +
                "  --threads=P       Set the number of threads from the pattern P (default " + DEFAULT_THREADS_PATTERN + ")\n" +
                "                N - use N threads\n" +
                "                A,B - use the thread counts from pattern A, then pattern B\n" +
                "                M..N - use all powers of two between M and N, inclusive\n" +
                "                M-N - use integers between M and N, inclusive\n" +
                "  --warmup=N        Warm up each experiment for N milliseconds (default " + DEFAULT_WARMUP_MILLIS + ")\n" +
                "  --timed=N         Run each experiment for N milliseconds (default " + DEFAULT_TIMED_MILLIS + ")\n");
        System.exit(-1);
    }


    public void run() {
        for (int pass = 0; pass < 2; ++pass) {
            System.out.println("---------- " + (pass == 0 ? "warmup" : "timed run"));
            for (IntSet impl : implementations) {
                for (int n : _threadCounts) {
                    // run an experiment
                    new Experiment(n, _keyRange, _readPct, impl, (pass == 0 ? _warmupMillis : _timedMillis)).run();
                }
            }
        }
    }

    public static void main(final String[] args) {
        new Main(args).run();
    }
}
