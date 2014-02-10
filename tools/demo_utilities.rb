
# 27-Jan_2014, JCF - will be loading in FHiCL code from files located
# in directories referred to by the FHICL_FILE_PATH variable; supply
# only the basename of the file (e.g., read_fcl("WFViewer.fcl"), not
# read_fcl("/my/full/path/WFViewer.fcl")

def read_fcl( filename )
  paths = ENV['FHICL_FILE_PATH'].split(':')

  paths.each do |path|                                                                            

    fullname = path + "/" + filename

    if File.file?( fullname )
      return File.read( fullname )
    end
  end

  raise Exception.new("Unable to locate desired file " + 
                      filename + 
                      " in any of the directories referred to by the " +
                      "FHICL_FILE_PATH environment variable")
end

