# This function will generate the FHiCL code used to control the
# AggregatorMain application by configuring its
# artdaq::AggregatorCore object

require File.join( File.dirname(__FILE__), 'generateAggregator' )
require File.join( File.dirname(__FILE__), 'generateCompression' )

def generateAggregatorMain(dataDir, runNumber, totalFRs, totalEBs, bunchSize,
                           compressionLevel, totalv1720s, totalv1724s, onmonEnable,
                           diskWritingEnable, agIndex, totalAGs, fragSizeWords,
                           xmlrpcClientList, fileSizeThreshold, fileDuration,
                           fileEventCount, fclWFViewer, onmonEventPrescale)

agConfig = String.new( "\
services: {
  scheduler: {
    fileMode: NOMERGE
  }
  user: {
    NetMonTransportServiceInterface: {
      service_provider: NetMonTransportService
      max_fragment_size_words: %{size_words}
    }
  }
  Timing: { summaryOnly: true }
  #SimpleMemoryCheck: { }
}

%{aggregator_code}

source: {
  module_type: NetMonInput
}
outputs: {
  %{root_output}normalOutput: {
  %{root_output}  module_type: RootOutput
  %{root_output}  fileName: \"%{output_file}\"
  %{root_output}  compressionLevel: 0
  %{root_output}  %{drop_uncompressed}outputCommands: [ \"keep *\", \"drop artdaq::Fragments_daq_V1720_*\", \"drop artdaq::Fragments_daq_V1724_*\" ]
  %{root_output}}
}
physics: {
  analyzers: {
%{phys_anal_onmon_cfg}
  }

  producers: {

     %{huffdiffV1720}

     %{huffdiffV1724}

     # Decompression functionality can also be added...
  }

  p1: [ %{compressionModules} ] 

  %{enable_onmon}a1: %{onmon_modules}

  %{root_output}my_output_modules: [ normalOutput ]
}
process_name: DAQAG"
)

  queueDepth, queueTimeout = -999, -999

  if agIndex == 0
    if totalAGs > 1
      onmonEnable = 0
    end
    queueDepth = 20
    queueTimeout = 5
  else
    diskWritingEnable = 0
    queueDepth = 2
    queueTimeout = 1
  end

  aggregator_code = generateAggregator( totalFRs, totalEBs, bunchSize, fragSizeWords,
                                        xmlrpcClientList, fileSizeThreshold, fileDuration,
                                        fileEventCount, queueDepth, queueTimeout, onmonEventPrescale )
  agConfig.gsub!(/\%\{aggregator_code\}/, aggregator_code)

  puts "Initial aggregator " + String(agIndex) + " disk writing setting = " +
  String(diskWritingEnable)
  # Do the substitutions in the aggregator configuration given the options
  # that were passed in from the command line.  Assure that files written out
  # by each AG are unique by including a timestamp in the file name.
  currentTime = Time.now
  fileName = "artdaqdemo_"
  fileName += "r%06r_sr%02s_%to"
  fileName += ".root"
  outputFile = File.join(dataDir, fileName)

  agConfig.gsub!(/\%\{output_file\}/, outputFile)
  agConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
  agConfig.gsub!(/\%\{size_words\}/, String(fragSizeWords))

  agConfig.gsub!(/\%\{onmon_modules\}/, String(ONMON_MODULES))

  compressionModules = []

  if Integer(compressionLevel) > 0 && Integer(compressionLevel) < 3
    if Integer(totalv1720s) > 0
      agConfig.gsub!(/\%\{huffdiffV1720\}/, "huffdiffV1720: { " + generateCompression("V1720") + "}" )
      compressionModules << "huffdiffV1720"
    else
      agConfig.gsub!(/\%\{huffdiffV1720\}/, "")
    end

    if Integer(totalv1724s) > 0
      agConfig.gsub!(/\%\{huffdiffV1724\}/, "huffdiffV1724: { " + generateCompression("V1724") + "}" )
      compressionModules << "huffdiffV1724"
    else
      agConfig.gsub!(/\%\{huffdiffV1724\}/, "")
    end

  else
    agConfig.gsub!(/\%\{huffdiffV1720\}/, "")
    agConfig.gsub!(/\%\{huffdiffV1724\}/, "")
  end

  agConfig.gsub!(/\%\{compressionModules\}/, compressionModules.join(","))

  if Integer(compressionLevel) > 1
    agConfig.gsub!(/\%\{drop_uncompressed\}/, "")
  else
    agConfig.gsub!(/\%\{drop_uncompressed\}/, "#")
  end

  puts "agIndex = %d, totalAGs = %d, onmonEnable = %d" % [agIndex, totalAGs, onmonEnable]

  puts "Final aggregator " + String(agIndex) + " disk writing setting = " +
  String(diskWritingEnable)
  if Integer(diskWritingEnable) != 0
    agConfig.gsub!(/\%\{root_output\}/, "")
  else
    agConfig.gsub!(/\%\{root_output\}/, "#")
  end
  if Integer(onmonEnable) != 0
    agConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, fclWFViewer )
    agConfig.gsub!(/\%\{enable_onmon\}/, "")
  else
    agConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, "")
    agConfig.gsub!(/\%\{enable_onmon\}/, "#")
  end

  return agConfig  
end
