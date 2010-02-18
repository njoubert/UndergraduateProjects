Names: Niels Joubert, Suhaas Prasad
SUNetIDs: njoubert, suhaasp

We were unable to run dyn_hadoop due to hotbox cluster overloads.

In our implementation we split the input files into pages, then we map the
process of scoring each wikipedia page with the query. The page title and 
score key-value pairs are then reduced locally on the mapping machine using
a combiner class.  Then the outputs from each combiner is reduced to a final
list of top 20 pages.

In order to split the input files into pages, we wrote our own InputFormat
and associated recordReader. The input format assigns a input file to a
reader, which then parses the file for pages based on the <title> attribute.
Each record returned by the reader corresponds to a single page used in a 
single map call.

The mapper class computes all ngrams for the page it receives and then counts
the number of ngrams it has in common with the given query.  Each mapper
instance computes the query's ngrams on initialization. It then outputs the
page title and score to a static key.

The combiner calculates the top 20 pages outputted for a single mapper instance.
All the combiners then output to a reducer class, which outputs the final list.

This satisfies the performance constraints.