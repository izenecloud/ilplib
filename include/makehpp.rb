#!/usr/bin/ruby

def traverse(file_path)
    if File.directory? file_path
#      puts "entering " + file_path
      Dir.foreach(file_path) do |file| 
        if /\.h$/.match(file)
          puts "#include \"" + file_path + "/" + file + "\""
        end
        if file != "." and file != ".."
          traverse(file_path + "/" + file)
        end
      end
    end
end

puts "#ifndef " + ARGV[0].upcase + "_HPP_"
puts "#define " + ARGV[0].upcase + "_HPP_"
puts "\n"

traverse(".")

puts "\n"
puts "#endif //" + ARGV[0].upcase + "_HPP_"
