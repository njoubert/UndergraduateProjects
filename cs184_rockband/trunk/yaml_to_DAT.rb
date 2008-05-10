#!/usr/bin/ruby

#Converts a YAML array to a DAT file

require 'yaml'

file = ARGV[0] if ARGV[0] and File.exists?(ARGV[0])

if (!file)
	puts "No input file given!"
	exit
end

input = YAML.load_file(file)

input.each {|v| puts v}