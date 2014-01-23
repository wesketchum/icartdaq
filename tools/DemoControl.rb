#!/usr/bin/env ruby

require 'date'
require "optparse"
require "ostruct"
require "xmlrpc/client"

# PLEASE NOTE: If/when there comes a time that we want to add more board
# types to this script, we should go back to the ds50MasterControl script
# and use it as an example since it may not be obvious how to add boards
# from what currently exists in this script.  KAB, 28-Dec-2013

# [SCF] It'd be nice if we were using a newer version of ruby (1.9.3) that had
# better string format substitution.  For the time being we'll setup our
# string constants using the newer convention and do the ugly substitutions.

# 17-Sep-2013, KAB - provide a way to fetch the online monitoring
# configuration from a separate file
if ENV['ARTDAQ_CONFIG_PATH']
  @cfgPathArray = ENV['ARTDAQ_CONFIG_PATH'].split(':')
  @cfgPathArray = @cfgPathArray.reverse
  (@cfgPathArray).each do |path|
    $LOAD_PATH.unshift path
  end
end
begin
  require 'onmon_config'
rescue Exception => msg
end

# Please NOTE that this assignment for the ONMON_CFG will only
# be used if it wasn't included from the external file already
if (defined?(PHYS_ANAL_ONMON_CFG)).nil? || (PHYS_ANAL_ONMON_CFG).nil?
  PHYS_ANAL_ONMON_CFG = "\
    app: {
      module_type: RootApplication
      force_new: true
    }
    wf: {
      module_type: WFViewer
      fragments_per_board: %{fragments_per_board}
      fragment_receiver_count: %{total_frs}
      fragment_ids: %{fragment_ids}
      fragment_type_labels: %{fragment_type_labels} " \
      + File.read(ENV['FCL_DIR'] + "/WFViewer.fcl") \
      + "    }"
end


# And, this assignment for the prescale will only
# be used if it wasn't included from the external file already
if (defined?(ONMON_EVENT_PRESCALE)).nil? || (ONMON_EVENT_PRESCALE).nil?
  ONMON_EVENT_PRESCALE = 1
end
# ditto, the online monitoring modules that are run
if (defined?(ONMON_MODULES)).nil? || (ONMON_MODULES).nil?
  ONMON_MODULES = "[ app, wf ]"
end

EB_CONFIG = "\
BEGIN_PROLOG
  huff:
  {
    module_type: Compression
    raw_label: daq
    want_bins: false
    perf_print: false
    use_diffs: true
    record_compression: false
  }

  huffdiff_1720: @local::huff
  huffdiff_1720.instance_name: V1720
  huffdiff_1720.table_file: \"table_daqV1720_huff_diff.txt\"
  huffdiff_1724: @local::huff
  huffdiff_1724.instance_name: V1724
  huffdiff_1724.table_file: \"table_daqV1724_huff_diff.txt\"
END_PROLOG
services: {
  scheduler: {
    fileMode: NOMERGE
  }
  user: {
    NetMonTransportServiceInterface: {
      service_provider: NetMonTransportService
      first_data_receiver_rank: %{ag_rank}
      mpi_buffer_count: %{netmonout_buffer_count}
      max_fragment_size_words: %{size_words}
      data_receiver_count: 1 # %{ag_count}
      #broadcast_sends: true
    }
  }
  Timing: { summaryOnly: true }
  #SimpleMemoryCheck: { }
}
daq: {
  max_fragment_size_words: %{size_words}
  event_builder: {
    mpi_buffer_count: %{buffer_count}
    first_fragment_receiver_rank: 0
    fragment_receiver_count: %{total_frs}
    expected_fragments_per_event: %{total_fragments}
    use_art: true
    print_event_store_stats: true
    verbose: %{verbose}
  }
}
outputs: {
  %{netmon_output}netMonOutput: {
  %{netmon_output}  module_type: NetMonOutput
  %{netmon_output}  %{drop_uncompressed}outputCommands: [ \"keep *\", \"drop artdaq::Fragments_daq_V1720_*\", \"drop artdaq::Fragments_daq_V1724_*\" ]
  %{netmon_output}}
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
    # compress the raw
    %{enable_1720_compression}huffdiffV1720: @local::huffdiff_1720  
    %{enable_1724_compression}huffdiffV1724: @local::huffdiff_1724 
    %{enable_172x_compression}huffdiffV1720: @local::huffdiff_1720  
    %{enable_172x_compression}huffdiffV1724: @local::huffdiff_1724 
  }
  %{enable_1720_compression}p1: [ huffdiffV1720 ]
  %{enable_1724_compression}p1: [ huffdiffV1724 ]
  %{enable_172x_compression}p1: [ huffdiffV1720, huffdiffV1724 ]

  %{enable_onmon}a1: [ app, wf ]

  %{netmon_output}my_output_modules: [ netMonOutput ]
  %{root_output}my_output_modules: [ normalOutput ]
}
source: {
  module_type: RawInput
  waiting_time: 900
  resume_after_timeout: true
  fragment_type_map: [[1, \"missed\"], [3, \"V1720\"], [4, \"V1724\"], [6, \"TOY1\"], [7, \"TOY2\"]]
}
process_name: DAQ"

AG_CONFIG = "\
BEGIN_PROLOG
  huff:
  {
    module_type: Compression
    raw_label: daq
    want_bins: false
    perf_print: false
    use_diffs: true
    record_compression: false
  }

  huffdiff_1720: @local::huff
  huffdiff_1720.instance_name: V1720
  huffdiff_1720.table_file: \"table_daqV1720_huff_diff.txt\"
  huffdiff_1724: @local::huff
  huffdiff_1724.instance_name: V1724
  huffdiff_1724.table_file: \"table_daqV1724_huff_diff.txt\"

  unhuff:
  {
    module_type: Decompression
  }

  dunhuff_1720: @local::unhuff
  dunhuff_1720.instance_name: V1720
  dunhuff_1720.compressed_label: huffdiffV1720
  dunhuff_1720.table_file: \"table_daqV1720_huff_diff.txt\"
  dunhuff_1724: @local::unhuff
  dunhuff_1724.instance_name: V1724
  dunhuff_1724.compressed_label: huffdiffV1724
  dunhuff_1724.table_file: \"table_daqV1724_huff_diff.txt\"
END_PROLOG
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
daq: {
  max_fragment_size_words: %{size_words}
  aggregator: {
    mpi_buffer_count: %{buffer_count}
    first_event_builder_rank: %{total_frs}
    event_builder_count: %{total_ebs}
    expected_events_per_bunch: %{bunch_size}
    print_event_store_stats: true
    event_queue_depth: %{queue_depth}
    event_queue_wait_time: %{queue_timeout}
    onmon_event_prescale: %{onmon_event_prescale}
    xmlrpc_client_list: \"%{xmlrpc_client_list}\"
    file_size_MB: %{file_size}
    file_duration: %{file_duration}
    file_event_count: %{file_event_count}
  }
}
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
    # compress the raw
    %{enable_1720_compression}huffdiffV1720: @local::huffdiff_1720  
    %{enable_1724_compression}huffdiffV1724: @local::huffdiff_1724 
    %{enable_172x_compression}huffdiffV1720: @local::huffdiff_1720  
    %{enable_172x_compression}huffdiffV1724: @local::huffdiff_1724 

    ## uncompress the raw
    #dunhuffV1720: @local::dunhuff_1720
    #dunhuffV1724: @local::dunhuff_1724
  }
  %{enable_1720_compression}p1: [ huffdiffV1720 ]
  %{enable_1724_compression}p1: [ huffdiffV1724 ]
  %{enable_172x_compression}p1: [ huffdiffV1720, huffdiffV1724 ]

  %{enable_onmon}a1: %{onmon_modules}

  %{root_output}my_output_modules: [ normalOutput ]
}
process_name: DAQAG"

COMPOSITE_GENERATOR_CONFIG = "\
%{prolog}
daq: {
  max_fragment_size_words: %{size_words}
  fragment_receiver: {
    mpi_buffer_count: %{buffer_count}
    first_event_builder_rank: %{total_frs}
    event_builder_count: %{total_ebs}
    generator: CompositeDriver
    fragment_id: 999
    board_id: 999
    generator_config_list:
    [
      # the format of this list is {daq:<paramSet},{daq:<paramSet>},...
      %{generator_list}
    ]
  }
}"

TOY_SIM_CONFIG = "\
daq: {
  max_fragment_size_words: %{size_words}
  fragment_receiver: {
    mpi_buffer_count: %{buffer_count}
    mpi_sync_interval: 50
    first_event_builder_rank: %{total_frs}
    event_builder_count: %{total_ebs}
    generator: ToySimulator
    fragment_type: %{fragment_type}
    fragments_per_board: %{fragments_per_board}
    starting_fragment_id: %{starting_fragment_id}
    fragment_id: %{starting_fragment_id}
    board_id: %{board_id}
    random_seed: %{random_seed}
    sleep_on_stop_us: 500000 " \
    + File.read(ENV['FCL_DIR'] + "/ToySimulator.fcl") \
    + " }\
    }"

V1720_SIM_CONFIG = "\
daq: {
  max_fragment_size_words: %{size_words}
  fragment_receiver: {
    mpi_buffer_count: %{buffer_count}
    mpi_sync_interval: 50
    first_event_builder_rank: %{total_frs}
    event_builder_count: %{total_ebs}
    generator: V172xSimulator
    fragment_type: %{fragment_type}
    freqs_file: \"V1720_sample_freqs.dat\"
    fragments_per_board: %{fragments_per_board}
    starting_fragment_id: %{starting_fragment_id}
    fragment_id: %{starting_fragment_id}
    board_id: %{board_id}
    random_seed: %{random_seed}
    sleep_on_stop_us: 500000 "  \
    + File.read(ENV['FCL_DIR'] + "/V172xSimulator.fcl") \
    + " } \
    }"


class ConfigGen

   def generateWFViewer(totalFRs, fragmentsPerBoard, fragmentIDList, fragmentTypeList)
     wfViewerConfig = String.new(PHYS_ANAL_ONMON_CFG)
     wfViewerConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
     wfViewerConfig.gsub!(/\%\{fragments_per_board\}/, String(fragmentsPerBoard))
     wfViewerConfig.gsub!(/\%\{fragment_ids\}/, String(fragmentIDList))
     wfViewerConfig.gsub!(/\%\{fragment_type_labels\}/, String(fragmentTypeList))
     return wfViewerConfig
   end


  def generateEventBuilder(ebIndex, totalFRs, totalEBs, totalAGs, compressionLevel, 
                           totalv1720s, totalv1724s, dataDir, onmonEnable,
                           diskWritingEnable, fragSizeWords, totalFragments, fragmentsPerBoard,
                           fragmentIDList, fragmentTypeList)
    # Do the substitutions in the event builder configuration given the options
    # that were passed in from the command line.  
    ebConfig = String.new(EB_CONFIG)
    ebConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
    ebConfig.gsub!(/\%\{total_fragments\}/, String(totalFragments))
    ebConfig.gsub!(/\%\{ag_rank\}/, String(totalFRs + totalEBs))
    ebConfig.gsub!(/\%\{ag_count\}/, String(totalAGs))
    ebConfig.gsub!(/\%\{buffer_count\}/, String(totalFRs*8))
    ebConfig.gsub!(/\%\{size_words\}/, String(fragSizeWords))
    ebConfig.gsub!(/\%\{netmonout_buffer_count\}/, String(totalAGs*4))
    if Integer(compressionLevel) > 0 && Integer(compressionLevel) < 3
      if Integer(totalv1720s) > 0 and Integer(totalv1724s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "")
      elsif Integer(totalv1720s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      elsif Integer(totalv1724s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      else
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      end
    else
      ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
      ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
      ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
    end
    if Integer(compressionLevel) > 1
      ebConfig.gsub!(/\%\{drop_uncompressed\}/, "")
    else
      ebConfig.gsub!(/\%\{drop_uncompressed\}/, "#")
    end
    if Integer(totalAGs) >= 1
      ebConfig.gsub!(/\%\{netmon_output\}/, "")
      ebConfig.gsub!(/\%\{root_output\}/, "#")
      ebConfig.gsub!(/\%\{enable_onmon\}/, "#")
      ebConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, "")
      ebConfig.gsub!(/\%\{verbose\}/, "false")
    else
      ebConfig.gsub!(/\%\{netmon_output\}/, "#")
      if Integer(diskWritingEnable) != 0
        ebConfig.gsub!(/\%\{root_output\}/, "")
      else
        ebConfig.gsub!(/\%\{root_output\}/, "#")
      end
      if Integer(onmonEnable) != 0
        ebConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, generateWFViewer(totalFRs, fragmentsPerBoard, fragmentIDList, fragmentTypeList))
        ebConfig.gsub!(/\%\{enable_onmon\}/, "")
      else
        ebConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, "")
        ebConfig.gsub!(/\%\{enable_onmon\}/, "#")
      end
      ebConfig.gsub!(/\%\{verbose\}/, "true")
    end

    currentTime = Time.now
    fileName = "artdaqdemo_eb%02d_" % ebIndex
    fileName += "r%06r_sr%02s_%to"
    fileName += ".root"
    outputFile = File.join(dataDir, fileName)
    ebConfig.gsub!(/\%\{output_file\}/, outputFile)

    return ebConfig
  end

  def generateAggregator(dataDir, runNumber, totalFRs, totalEBs, bunchSize,
                         compressionLevel, totalv1720s, totalv1724s, onmonEnable,
                         diskWritingEnable, agIndex, totalAGs, fragSizeWords,
                         xmlrpcClientList, fileSizeThreshold, fileDuration,
                         fileEventCount, fragmentsPerBoard,
                         fragmentIDList, fragmentTypeList)
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
    agConfig = String.new(AG_CONFIG)
    agConfig.gsub!(/\%\{output_file\}/, outputFile)
    agConfig.gsub!(/\%\{total_ebs\}/, String(totalEBs))
    agConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
    agConfig.gsub!(/\%\{buffer_count\}/, String(totalEBs*4))
    agConfig.gsub!(/\%\{bunch_size\}/, String(bunchSize))
    agConfig.gsub!(/\%\{onmon_event_prescale\}/, String(ONMON_EVENT_PRESCALE))
    agConfig.gsub!(/\%\{onmon_modules\}/, String(ONMON_MODULES))
    agConfig.gsub!(/\%\{size_words\}/, String(fragSizeWords))
    agConfig.gsub!(/\%\{xmlrpc_client_list\}/, String(xmlrpcClientList))
    agConfig.gsub!(/\%\{file_size\}/, String(fileSizeThreshold))
    agConfig.gsub!(/\%\{file_duration\}/, String(fileDuration))
    agConfig.gsub!(/\%\{file_event_count\}/, String(fileEventCount))
    if Integer(compressionLevel) > 0 && Integer(compressionLevel) < 3
      if Integer(totalv1720s) > 0 and Integer(totalv1724s) > 0
        agConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_172x_compression\}/, "")
      elsif Integer(totalv1720s) > 0
        agConfig.gsub!(/\%\{enable_1720_compression\}/, "")
        agConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      elsif Integer(totalv1724s) > 0
        agConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_1724_compression\}/, "")
        agConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      else
        agConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        agConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      end
    else
      agConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
      agConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
      agConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
    end
    if Integer(compressionLevel) > 1
      agConfig.gsub!(/\%\{drop_uncompressed\}/, "")
    else
      agConfig.gsub!(/\%\{drop_uncompressed\}/, "#")
    end

    if agIndex == 0
      if totalAGs > 1
        onmonEnable = 0
      end
      agConfig.gsub!(/\%\{queue_depth\}/, "20")
      agConfig.gsub!(/\%\{queue_timeout\}/, "5.0")
    else
      diskWritingEnable = 0
      agConfig.gsub!(/\%\{queue_depth\}/, "2")
      agConfig.gsub!(/\%\{queue_timeout\}/, "1.0")
    end

    puts "Final aggregator " + String(agIndex) + " disk writing setting = " +
      String(diskWritingEnable)
    if Integer(diskWritingEnable) != 0
      agConfig.gsub!(/\%\{root_output\}/, "")
    else
      agConfig.gsub!(/\%\{root_output\}/, "#")
    end
    if Integer(onmonEnable) != 0
      agConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, generateWFViewer(totalFRs, fragmentsPerBoard, fragmentIDList, fragmentTypeList))
      agConfig.gsub!(/\%\{enable_onmon\}/, "")
    else
      agConfig.gsub!(/\%\{phys_anal_onmon_cfg\}/, "")
      agConfig.gsub!(/\%\{enable_onmon\}/, "#")
    end

    return agConfig  
  end
  
  def generateToy(startingFragmentId, totalEBs, totalFRs,
                    fragSizeWords, boardId, fragmentsPerBoard, fragmentType)
    toyConfig = String.new(TOY_SIM_CONFIG)
    
    toyConfig.gsub!(/\%\{total_ebs\}/, String(totalEBs))
    toyConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
    toyConfig.gsub!(/\%\{starting_fragment_id\}/, String(startingFragmentId))
    toyConfig.gsub!(/\%\{fragments_per_board\}/, String(fragmentsPerBoard))
    toyConfig.gsub!(/\%\{board_id\}/, String(boardId))
    toyConfig.gsub!(/\%\{buffer_count\}/, String(totalEBs*8))
    toyConfig.gsub!(/\%\{size_words\}/, String(fragSizeWords))
    toyConfig.gsub!(/\%\{random_seed\}/, String(rand(10000))) 
    toyConfig.gsub!(/\%\{fragment_type\}/, fragmentType) 
    return toyConfig
  end

  def generateV1720(startingFragmentId, totalEBs, totalFRs,
                    fragSizeWords, boardId, fragmentsPerBoard, fragmentType)
    v1720Config = String.new(V1720_SIM_CONFIG)
    
    v1720Config.gsub!(/\%\{total_ebs\}/, String(totalEBs))
    v1720Config.gsub!(/\%\{total_frs\}/, String(totalFRs))
    v1720Config.gsub!(/\%\{starting_fragment_id\}/, String(startingFragmentId))
    v1720Config.gsub!(/\%\{fragments_per_board\}/, String(fragmentsPerBoard))
    v1720Config.gsub!(/\%\{board_id\}/, String(boardId))
    v1720Config.gsub!(/\%\{buffer_count\}/, String(totalEBs*8))
    v1720Config.gsub!(/\%\{size_words\}/, String(fragSizeWords))
    v1720Config.gsub!(/\%\{random_seed\}/, String(rand(10000))) 
    v1720Config.gsub!(/\%\{fragment_type\}/, fragmentType) 
    return v1720Config
  end

  def generateComposite(totalEBs, totalFRs, configStringArray)
    compositeConfig = String.new(COMPOSITE_GENERATOR_CONFIG)
    compositeConfig.gsub!(/\%\{total_ebs\}/, String(totalEBs))
    compositeConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
    compositeConfig.gsub!(/\%\{buffer_count\}/, String(totalEBs*8))

    # The complications here are A) determining the largest buffer size that
    # has been requested by the child configurations and using that for the
    # composite buffers size and B) moving any PROLOG declarations from the
    # individual configuration strings to the front of the full CFG string.
    prologList = []
    fragSizeWords = 0
    configString = ""
    first = true
    configStringArray.each do |cfg|
      my_match = /(.*)BEGIN_PROLOG(.*)END_PROLOG(.*)/im.match(cfg)
      if my_match
        thisProlog = my_match[2]
        cfg = my_match[1] + my_match[3]

        found = false
        prologList.each do |savedProlog|
          if thisProlog == savedProlog
            found = true
            break
          end
        end
        if ! found
          prologList << thisProlog
        end
      end

      if first
        first = false
      else
        configString += ", "
      end
      configString += "{" + cfg + "}"

      my_match = /max_fragment_size_words\s*\:\s*(\d+)/.match(cfg)
      if my_match
        begin
          sizeWords = Integer(my_match[1])
          if sizeWords > fragSizeWords
            fragSizeWords = sizeWords
          end
        rescue Exception => msg
          puts "Warning: exception parsing size_words in composite child configuration: " + my_match[1] + " " + msg
        end
      end
    end
    compositeConfig.gsub!(/\%\{size_words\}/, String(fragSizeWords))
    compositeConfig.gsub!(/\%\{generator_list\}/, configString)

    prologString = ""
    if prologList.length > 0
      prologList.each do |savedProlog|
        prologString += "\n"
        prologString += savedProlog
      end
      prologString = "BEGIN_PROLOG" + prologString + "\nEND_PROLOG"
    end
    compositeConfig.gsub!(/\%\{prolog\}/, prologString)

    return compositeConfig
  end

  def generateXmlRpcClientList(cmdLineOptions)
    xmlrpcClients = ""
#    boardreaders = Array.new + @options.v1720s + @options.toys
#    boardreaders.each do |proc|
    (cmdLineOptions.v1720s + cmdLineOptions.toys).each do |proc|
      br = cmdLineOptions.boardReaders[proc.board_reader_index]
      if br.hasBeenIncludedInXMLRPCList
        next
      else
        br.hasBeenIncludedInXMLRPCList = true
        xmlrpcClients += ";http://" + proc.host + ":" +
          String(proc.port) + "/RPC2"
        xmlrpcClients += ",3"  # group number
      end
    end
    (cmdLineOptions.eventBuilders).each do |proc|
      xmlrpcClients += ";http://" + proc.host + ":" +
        String(proc.port) + "/RPC2"
      xmlrpcClients += ",4"  # group number
    end
    (cmdLineOptions.aggregators).each do |proc|
      xmlrpcClients += ";http://" + proc.host + ":" +
        String(proc.port) + "/RPC2"
      xmlrpcClients += ",5"  # group number
    end
    return xmlrpcClients
  end
end

class CommandLineParser
  def initialize(configGen)
    @configGen = configGen
    @options = OpenStruct.new
    @options.aggregators = []
    @options.eventBuilders = []
    @options.v1720s = []
    @options.toys = []
    @options.boardReaders = []
    @options.dataDir = nil
    @options.command = nil
    @options.summary = false
    @options.runNumber = "0101"
    @options.serialize = false
    @options.runOnmon = 0
    @options.writeData = 1
    @options.runDurationSeconds = -1
    @options.eventsInRun = -1
    @options.fileSizeThreshold = 0
    @options.fileDurationSeconds = 0
    @options.eventsInFile = 0
    @options.fragmentsPerBoard = 1

    @optParser = OptionParser.new do |opts|
      opts.banner = "Usage: DemoControl.rb [options]"
      opts.separator ""
      opts.separator "Specific options:"

      opts.on("--eb [host,port,compression_level]", Array,
              "Add an event builder that runs on the",
              "specified host and port and optionally",
              "compresses ADC data.") do |eb|
        if eb.length != 3
          puts "You must specifiy a host, port, and compression level."
          exit
        end
        ebConfig = OpenStruct.new
        ebConfig.host = eb[0]
        ebConfig.port = Integer(eb[1])
        ebConfig.compression_level = Integer(eb[2])
        ebConfig.kind = "eb"
        ebConfig.index = @options.eventBuilders.length
        @options.eventBuilders << ebConfig
      end

      opts.on("--ag [host,port,bunch_size,compression_level]", Array,
              "Add an aggregator that runs on the",
              "specified host and port.  Also specify the",
              "number of events to pass to art per bunch,",
              "and the compression level.") do |ag|
        if ag.length != 4
          puts "You must specifiy a host, port, bunch size, and"
          puts "compression level."
          exit
        end
        agConfig = OpenStruct.new
        agConfig.host = ag[0]
        agConfig.port = Integer(ag[1])
        agConfig.kind = "ag"
        agConfig.bunch_size = Integer(ag[2])
        agConfig.compression_level = Integer(ag[3])
        agConfig.index = @options.aggregators.length
        @options.aggregators << agConfig
      end
    
      opts.on("--v1720 [host,port,board_id]", Array, 
              "Add a V1720 fragment receiver that runs on the specified host and port, ",
              "and has the specified board ID.") do |v1720|
        if v1720.length != 3
          puts "You must specifiy a host, port, and board ID."
          exit
        end
        v1720Config = OpenStruct.new
        v1720Config.host = v1720[0]
        v1720Config.port = Integer(v1720[1])
        v1720Config.board_id = Integer(v1720[2])
        v1720Config.kind = "V1720"
        v1720Config.index = (@options.v1720s + @options.toys).length
        v1720Config.board_reader_index = addToBoardReaderList(v1720Config.host, v1720Config.port,
                                                              v1720Config.kind, v1720Config.index)
        @options.v1720s << v1720Config
      end

      opts.on("--v1724 [host,port,board_id]", Array, 
              "Add a V1724 fragment receiver that runs on the specified host, port, ",
              "and board ID.") do |v1724|
        if v1724.length != 3
          puts "You must specifiy a host, port, and board ID."
          exit
        end
        v1724Config = OpenStruct.new
        v1724Config.host = v1724[0]
        v1724Config.port = Integer(v1724[1])
        v1724Config.board_id = Integer(v1724[2])
        v1724Config.kind = "V1724"
        v1724Config.index = (@options.v1720s + @options.toys).length
        v1724Config.board_reader_index = addToBoardReaderList(v1724Config.host, v1724Config.port,
                                                              v1724Config.kind, v1724Config.index)
        # NOTE that we're simply adding this to the 1720 list...
        @options.v1720s << v1724Config
      end

      opts.on("--toy1 [host,port,board_id]", Array, 
              "Add a TOY1 fragment receiver that runs on the specified host, port, ",
              "and board ID.") do |toy1|
        if toy1.length != 3
          puts "You must specifiy a host, port, and board ID."
          exit
        end
        toy1Config = OpenStruct.new
        toy1Config.host = toy1[0]
        toy1Config.port = Integer(toy1[1])
        toy1Config.board_id = Integer(toy1[2])
        toy1Config.kind = "TOY1"
        toy1Config.index = (@options.v1720s + @options.toys).length
        toy1Config.board_reader_index = addToBoardReaderList(toy1Config.host, toy1Config.port,
                                                              toy1Config.kind, toy1Config.index)
        @options.toys << toy1Config
      end


      opts.on("--toy2 [host,port,board_id]", Array, 
              "Add a TOY2 fragment receiver that runs on the specified host, port, ",
              "and board ID.") do |toy2|
        if toy2.length != 3
          puts "You must specifiy a host, port, and board ID."
          exit
        end
        toy2Config = OpenStruct.new
        toy2Config.host = toy2[0]
        toy2Config.port = Integer(toy2[1])
        toy2Config.board_id = Integer(toy2[2])
        toy2Config.kind = "TOY2"
        toy2Config.index = (@options.v1720s + @options.toys).length
        toy2Config.board_reader_index = addToBoardReaderList(toy2Config.host, toy2Config.port,
                                                              toy2Config.kind, toy2Config.index)

        @options.toys << toy2Config
      end


      opts.on("-d", "--data-dir [data dir]", 
              "Directory that the event builders will", "write data to.") do |dataDir|
        @options.dataDir = dataDir
      end

      opts.on("-m", "--online-monitoring [enable flag (0 or 1)]", 
              "Whether to run the online monitoring modules.") do |runOnmon|
        @options.runOnmon = runOnmon
      end

      opts.on("-w", "--write-data [enable flag (0 or 1)]", 
              "Whether to write data to disk.") do |writeData|
        @options.writeData = writeData
      end

      opts.on("-c", "--command [command]", 
              "Execute a command: start, stop, init, shutdown, pause, resume, status, get-legal-commands.") do |command|
        @options.command = command
      end

      opts.on("-r", "--run-number [number]", "Specify the run number.") do |run|
        @options.runNumber = run
      end

      opts.on("-t", "--run-duration [minutes]",
              "Stop the run after the specified amount of time (minutes).") do |timeInMinutes|
        @options.runDurationSeconds = Integer(timeInMinutes) * 60
      end

      opts.on("-n", "--run-event-count [number]",
              "Stop the run after the specified number of events have been collected.") do |eventCount|
        @options.eventsInRun = Integer(eventCount)
      end

      opts.on("-f", "--file-size [number of MB]",
              "Close each data file when the specified size is reached (MB).") do |fileSize|
        @options.fileSizeThreshold = Float(fileSize)
      end

      opts.on("--file-duration [minutes]",
              "Closes each file after the specified amount of time (minutes).") do |timeInMinutes|
        @options.fileDurationSeconds = Integer(timeInMinutes) * 60
      end

      opts.on("--file-event-count [number]",
              "Close each file after the specified number of events have been written.") do |eventCount|
        @options.eventsInFile = Integer(eventCount)
      end

      opts.on("-s", "--summary", "Summarize the configuration.") do
        @options.summary = true
      end

      opts.on("-e", "--serialize", "Serialize the config for System Control.") do
        @options.serialize = true
      end

      opts.on("--fragments-per-board [N]", "Fragments each event will send to a single readout board") do |fragsPerBoard|
        @options.fragmentsPerBoard = Integer(fragsPerBoard)
      end

#      wfViewerConfig = OpenStruct.new

      opts.on_tail("-h", "--help", "Show this message.") do
        puts opts
        exit
      end
    end
  end

  def parse()
    if ARGV.length == 0
      puts @optParser
      exit
    end

    @optParser.parse(ARGV)
    self.validate()
  end

  def validate()
    # In sim mode make sure there is an eb and a 1720
    return nil
  end

  def addToBoardReaderList(host, port, kind, boardIndex)
    # check for an existing boardReader with the same host and port
    brIndex = 0
    @options.boardReaders.each do |br|
      if host == br.host && port == br.port
        br.kindList << kind
        br.boardIndexList << boardIndex
        br.cfgList << ""
        br.boardCount += 1
        return brIndex
      end
      brIndex += 1
    end

    # if needed, create a new boardReader
    br = OpenStruct.new
    br.host = host
    br.port = port
    br.kindList = [kind]
    br.boardIndexList = [boardIndex]
    br.cfgList = [""]
    br.boardCount = 1
    br.commandHasBeenSent = false
    br.hasBeenIncludedInXMLRPCList = false
    br.kind = "multi-board"

    brIndex = @options.boardReaders.length
    @options.boardReaders << br
    return brIndex
  end

  def summarize()
    # Print out a summary of the configuration that was passed in from the
    # the command line.  Everything will be printed in terms of what process
    # is running on which host.
    puts "Configuration Summary:"
    hostMap = {}
    (@options.eventBuilders + @options.v1720s + @options.toys + @options.aggregators).each do |proc|
      if not hostMap.keys.include?(proc.host)
        hostMap[proc.host] = []
      end
      hostMap[proc.host] << proc
    end

    hostMap.each_key { |host|
      puts "  %s:" % [host]
      hostMap[host].sort! { |x,y|
        x.port <=> y.port
      }

      totalEBs = @options.eventBuilders.length
      totalFRs = @options.boardReaders.length
      hostMap[host].each do |item|
        case item.kind
        when "eb"
          puts "    EventBuilder, port %d, rank %d" % [item.port, totalFRs + item.index]
        when "ag"
          puts "    Aggregator, port %d, rank %d" % [item.port, totalEBs + totalFRs + item.index]
        when "V1720", "V1724", "TOY1", "TOY2"
          puts "    FragmentReceiver, Simulated %s, port %d, rank %d, board_id %d" % 
            [item.kind.upcase,
             item.port,
             item.index,
             item.board_id]
        end
      end
      puts ""
    }
    STDOUT.flush
    return nil
  end

  def getOptions()
    return @options
  end
end

class SystemControl
  def initialize(cmdLineOptions, configGen)
    @options = cmdLineOptions
    @configGen = configGen
  end

  def init()
    ebIndex = 0
    agIndex = 0
    totalv1720s = 0
    totalv1724s = 0
    totaltoy1s = 0
    totaltoy2s = 0
    @options.v1720s.each do |proc|
      case proc.kind
      when "V1720"
        totalv1720s += 1
      when "V1724"
        totalv1724s += 1
      end
    end
    @options.toys.each do |proc|
      case proc.kind
      when "TOY1"
        totaltoy1s += 1
      when "TOY2"
        totaltoy2s += 1
      end
    end
    totalBoards = @options.v1720s.length + @options.toys.length
    totalFRs = @options.boardReaders.length
    totalEBs = @options.eventBuilders.length
    totalAGs = @options.aggregators.length
    inputBuffSizeWords = 2097152

    fragmentsPerBoard = @options.fragmentsPerBoard

    #if Integer(totalv1720s) > 0
    #  inputBuffSizeWords = 8192 * @options.v1720s[0].gate_width
    #end
    xmlrpcClients = @configGen.generateXmlRpcClientList(@options)

    # 02-Dec-2013, KAB - loop over the front-end boards and build the configurations
    # that we will send to them.  These configurations are stored in the associated
    # boardReaders list entries since there are system configurations in which
    # multiple boards are read out by a single BoardReader, and it seems simpler to
    # store the CFGs in the boardReader list for everything

    # John F., 1/21/14 -- added the toy fragment generators

    (@options.v1720s + @options.toys).each { |boardreaderOptions|
      br = @options.boardReaders[boardreaderOptions.board_reader_index]
      listIndex = 0
      br.kindList.each do |kind|
        if kind == boardreaderOptions.kind && br.boardIndexList[listIndex] == boardreaderOptions.index

          if kind == "V1720" || kind == "V1724"
            cfg = @configGen.generateV1720(boardreaderOptions.index*fragmentsPerBoard,
                                           totalEBs, totalFRs,
                                           Integer(inputBuffSizeWords/8),
                                           boardreaderOptions.board_id, fragmentsPerBoard, kind)
          elsif kind == "TOY1" || kind == "TOY2"
            cfg = @configGen.generateToy(boardreaderOptions.index*fragmentsPerBoard,
                                           totalEBs, totalFRs,
                                           Integer(inputBuffSizeWords/8),
                                           boardreaderOptions.board_id, fragmentsPerBoard, kind)
          end

          br.cfgList[listIndex] = cfg
          break
        end
        listIndex += 1
      end
    }




    threads = []

    (@options.v1720s + @options.toys).each { |proc|
      br = @options.boardReaders[proc.board_reader_index]
      if br.boardCount > 1
        if br.commandHasBeenSent
          next
        else
          br.commandHasBeenSent = true
          proc = br
          cfg = @configGen.generateComposite(totalEBs, totalFRs, br.cfgList)
        end
      else
        cfg = br.cfgList[0]
      end

      currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
      puts "%s: Sending the INIT command to %s:%d." %
        [currentTime, proc.host, proc.port]
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(proc.host, "/RPC2",
                                          proc.port)
        if @options.serialize
          fileName = "BoardReader_%s_%s_%d.fcl" % [proc.kind,proc.host, proc.port]
          puts "  writing %s..." % fileName
          handle = File.open(fileName, "w")
          handle.write(cfg)
          handle.close()
        end
        result = xmlrpcClient.call("daq.init", cfg)
        currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
        puts "%s: %s FragmentReceiver on %s:%d result: %s" %
          [currentTime, proc.kind, proc.host, proc.port, result]
        STDOUT.flush
      end
    }
    STDOUT.flush
    threads.each { |aThread|
      aThread.join()
    }


    # John F., 1/21/14 -- before sending FHiCL configurations to the
    # EventBuilderMain and AggregatorMain processes, construct the
    # strings listing fragment ids and fragment types which will be
    # used by the WFViewer

    # Notice that "board_id" is taken to be synonmyous with
    # "fragment_id" here; this will need to change if we go with more
    # than one fragment per event per BoardReaderMain

    fragmentIDList, fragmentTypeList = "[ ", "[ "

    (@options.v1720s + @options.toys).each { |proc|
      fragmentIDList += " %d," % [ proc.board_id ]
      fragmentTypeList += " %s," % [ proc.kind ]
    }
    
    # Get rid of the trailing ","s, and replace with a close-bracket

    fragmentIDList[-1], fragmentTypeList[-1] = "]", "]" 


    # 27-Jun-2013, KAB - send INIT to EBs and AG last
    threads = []
    @options.eventBuilders.each { |ebOptions|
      currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
      puts "%s: Sending the INIT command to %s:%d." %
        [currentTime, ebOptions.host, ebOptions.port]
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(ebOptions.host, "/RPC2", 
                                          ebOptions.port)

        cfg = @configGen.generateEventBuilder(ebIndex, totalFRs, totalEBs, totalAGs,
                                              ebOptions.compression_level,
                                              totalv1720s, totalv1724s,
                                              @options.dataDir, @options.runOnmon,
                                              @options.writeData, inputBuffSizeWords,
                                              totalBoards*fragmentsPerBoard, fragmentsPerBoard,
                                              fragmentIDList, fragmentTypeList)

        if @options.serialize
          fileName = "EventBuilder_%s_%d.fcl" % [ebOptions.host, ebOptions.port]
          puts "  writing %s..." % fileName
          handle = File.open(fileName, "w")
          handle.write(cfg)
          handle.close()
        end
        result = xmlrpcClient.call("daq.init", cfg)
        currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
        puts "%s: EventBuilder on %s:%d result: %s" %
          [currentTime, ebOptions.host, ebOptions.port, result]
        STDOUT.flush
      end
      ebIndex += 1
    }

    @options.aggregators.each { |agOptions|
      currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
      puts "%s: Sending the INIT command to %s:%d." %
        [currentTime, agOptions.host, agOptions.port]
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(agOptions.host, "/RPC2", 
                                          agOptions.port)
        cfg = @configGen.generateAggregator(@options.dataDir, @options.runNumber,
                                            totalFRs, totalEBs, agOptions.bunch_size,
                                            agOptions.compression_level,
                                            totalv1720s, totalv1724s,
                                            @options.runOnmon, @options.writeData,
                                            agIndex, totalAGs, inputBuffSizeWords,
                                            xmlrpcClients, @options.fileSizeThreshold,
                                            @options.fileDurationSeconds,
                                            @options.eventsInFile, fragmentsPerBoard,
                                            fragmentIDList, fragmentTypeList)
        if @options.serialize
          fileName = "Aggregator_%s_%d.fcl" % [agOptions.host, agOptions.port]
          puts "  writing %s..." % fileName
          handle = File.open(fileName, "w")
          handle.write(cfg)
          handle.close()
        end
        result = xmlrpcClient.call("daq.init", cfg)
        currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
        puts "%s: Aggregator on %s:%d result: %s" %
          [currentTime, agOptions.host, agOptions.port, result]
        STDOUT.flush
      end
      agIndex += 1
    }
    
    STDOUT.flush
    threads.each { |aThread|
      aThread.join()
    }
  end

  def start(runNumber)
    self.sendCommandSet("start", @options.aggregators, runNumber)
    self.sendCommandSet("start", @options.eventBuilders, runNumber)
    self.sendCommandSet("start", @options.v1720s, runNumber)
    self.sendCommandSet("start", @options.toys, runNumber)
  end

  def sendCommandSet(commandName, procs, commandArg = nil)
    threads = []
    procs.each do |proc|
      # 02-Dec-2013, KAB - use the boardReader instance instead of the
      # actual card when multiple cards are read out by a single BR
      if proc.board_reader_index != nil
        br = @options.boardReaders[proc.board_reader_index]
        if br.boardCount > 1
          if br.commandHasBeenSent
            next
          else
            br.commandHasBeenSent = true
            proc = br
          end
        end
      end

      currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
      puts "%s: Attempting to connect to %s:%d and %s a run." %
        [currentTime, proc.host, proc.port, commandName]
      STDOUT.flush
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(proc.host, "/RPC2", proc.port)
        xmlrpcClient.timeout = 60
        if commandName == "stop"
          if proc.kind == "ag"
            xmlrpcClient.timeout = 120
          elsif proc.kind == "eb" || proc.kind == "multi-board"
            xmlrpcClient.timeout = 45
          else
            xmlrpcClient.timeout = 30
          end
        end
        begin
          if commandArg != nil
            result = xmlrpcClient.call("daq.%s" % [commandName], commandArg)
          else
            result = xmlrpcClient.call("daq.%s" % [commandName])
          end
        rescue Exception => msg
          result = "Exception: " + msg
        end
        currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
        case proc.kind
        when "eb"
          puts "%s: EventBuilder on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "ag"
          puts "%s: Aggregator on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "V1720"
          puts "%s: V1720 FragmentReceiver on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "V1724"
          puts "%s: V1724 FragmentReceiver on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "TOY1"
          puts "%s: TOY1 FragmentReceiver on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "TOY2"
          puts "%s: TOY2 FragmentReceiver on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        when "multi-board"
          puts "%s: multi-board FragmentReceiver on %s:%d result: %s" %
            [currentTime, proc.host, proc.port, result]
        end
        STDOUT.flush
      end
    end
    threads.each { |aThread|
      aThread.join()
    }
  end

  def shutdown()
    self.sendCommandSet("shutdown", @options.v1720s)
    self.sendCommandSet("shutdown", @options.toys)
    self.sendCommandSet("shutdown", @options.eventBuilders)
    self.sendCommandSet("shutdown", @options.aggregators)
  end

  def pause()
    self.sendCommandSet("pause", @options.v1720s)
    self.sendCommandSet("pause", @options.toys)
    self.sendCommandSet("pause", @options.eventBuilders)
    self.sendCommandSet("pause", @options.aggregators)
  end

  def stop()
    totalAGs = @options.aggregators.length
    if @options.eventsInRun > 0
      if Integer(totalAGs) > 0
        if Integer(totalAGs) > 1
          puts "NOTE: more than one Aggregator is running (count=%d)." % [totalAGs]
          puts " -> The first Aggregator will be used to determine the number of events"
          puts " -> in the current run."
        end
        aggregatorEventCount = 0
        previousAGEventCount = 0
        sleepTime = 0
        while aggregatorEventCount >= 0 && aggregatorEventCount < @options.eventsInRun do
          sleep(sleepTime)
          currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
          puts "%s: Attempting to fetch the number of events from the Aggregator." %
            [currentTime]
          STDOUT.flush
          xmlrpcClient = XMLRPC::Client.new(@options.aggregators[0].host, "/RPC2",
                                            @options.aggregators[0].port)
          xmlrpcClient.timeout = 10
          exceptionOccurred = false
          begin
            result = xmlrpcClient.call("daq.report", "event_count")
            if result == "busy" || result == "-1"
              # support one retry
              sleep(10)
              result = xmlrpcClient.call("daq.report", "event_count")
            end
            aggregatorEventCount = Integer(result)
          rescue Exception => msg
            exceptionOccurred = true
            result = "Exception: " + msg
            aggregatorEventCount = previousAGEventCount
          end
          currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
          if exceptionOccurred
            puts "%s: There was a problem communicating with the Aggregator (%s)," %
              [currentTime, result]
            puts "  the fetch of the number of events will be retried."
          else
            puts "%s: The Aggregator reports the following number of events: %s." %
              [currentTime, result]
          end
          STDOUT.flush

          if aggregatorEventCount > 0 && previousAGEventCount > 0 && \
            aggregatorEventCount > previousAGEventCount && sleepTime > 0 then
            remainingEvents = @options.eventsInRun - aggregatorEventCount
            recentRate = (aggregatorEventCount - previousAGEventCount) / sleepTime
            if recentRate > 0
              sleepTime = (remainingEvents / 2) / recentRate
            else
              sleepTime = 10
            end
            if sleepTime < 10
              sleepTime = 10;
            end
            if sleepTime > 900
              sleepTime = 900;
            end
          else
            sleepTime = 10
          end
          previousAGEventCount = aggregatorEventCount
        end
      else
        puts "No Aggregator in use - Unable to determine the number of events in the current run."     
      end
    elsif @options.runDurationSeconds > 0
      if Integer(totalAGs) > 0
        if Integer(totalAGs) > 1
          puts "NOTE: more than one Aggregator is running (count=%d)." % [totalAGs]
          puts " -> The first Aggregator will be used to determine the run duration."
        end
        aggregatorRunDuration = 0
        sleepTime = 0
        while aggregatorRunDuration >= 0 && aggregatorRunDuration < @options.runDurationSeconds do
          sleep(sleepTime)
          currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
          puts "%s: Attempting to fetch the run duration from the Aggregator." %
            [currentTime]
          STDOUT.flush
          xmlrpcClient = XMLRPC::Client.new(@options.aggregators[0].host, "/RPC2",
                                            @options.aggregators[0].port)
          xmlrpcClient.timeout = 10
          exceptionOccurred = false
          begin
            result = xmlrpcClient.call("daq.report", "run_duration")
            if result == "busy" || result == "-1"
              # support one retry
              sleep(10)
              result = xmlrpcClient.call("daq.report", "run_duration")
            end
            aggregatorRunDuration = Float(result)
          rescue Exception => msg
            exceptionOccurred = true
            result = "Exception: " + msg
            aggregatorRunDuration = 0
          end
          currentTime = DateTime.now.strftime("%Y/%m/%d %H:%M:%S")
          if exceptionOccurred
            puts "%s: There was a problem communicating with the Aggregator (%s)," %
              [currentTime, result]
            puts "  the fetch of the run duration will be retried."
          else
            puts "%s: The Aggregator reports the following run duration: %s seconds." %
              [currentTime, result]
          end
          STDOUT.flush

          if aggregatorRunDuration > 0 then
            remainingTime = @options.runDurationSeconds - aggregatorRunDuration
            sleepTime = remainingTime / 2
            if sleepTime < 10
              sleepTime = 10;
            end
            if sleepTime > 900
              sleepTime = 900;
            end
          else
            sleepTime = 10
          end
        end
      else
        puts "No Aggregator in use - Unable to determine the duration of the current run."
      end
    end

    self.sendCommandSet("stop", @options.v1720s)
    self.sendCommandSet("stop", @options.toys)
    self.sendCommandSet("stop", @options.eventBuilders)
    @options.aggregators.each do |proc|
      tmpList = []
      tmpList << proc
      self.sendCommandSet("stop", tmpList)
    end
  end

  def resume()
    self.sendCommandSet("resume", @options.aggregators)
    self.sendCommandSet("resume", @options.eventBuilders)
    self.sendCommandSet("resume", @options.v1720s)
    self.sendCommandSet("resume", @options.toys)
  end

  def checkStatus()
    self.sendCommandSet("status", @options.aggregators)
    self.sendCommandSet("status", @options.eventBuilders)
    self.sendCommandSet("status", @options.v1720s)
    self.sendCommandSet("status", @options.toys)
  end

  def getLegalCommands()
    self.sendCommandSet("legal_commands", @options.aggregators)
    self.sendCommandSet("legal_commands", @options.eventBuilders)
    self.sendCommandSet("legal_commands", @options.v1720s)
    self.sendCommandSet("legal_commands", @options.toys)
  end
end

if __FILE__ == $0
  cfgGen = ConfigGen.new
  cmdLineParser = CommandLineParser.new(cfgGen)
  cmdLineParser.parse()
  options = cmdLineParser.getOptions()
  puts "DemoControl disk writing setting = " + options.writeData

  if options.summary
    cmdLineParser.summarize()
  end

  sysCtrl = SystemControl.new(options, cfgGen)

  if options.command == "init"
    sysCtrl.init()
  elsif options.command == "start"
    sysCtrl.start(options.runNumber)
  elsif options.command == "stop"
    sysCtrl.stop()
  elsif options.command == "shutdown"
    sysCtrl.shutdown()
  elsif options.command == "pause"
    sysCtrl.pause()
  elsif options.command == "resume"
    sysCtrl.resume()
  elsif options.command == "status"
    sysCtrl.checkStatus()
  elsif options.command == "get-legal-commands"
    sysCtrl.getLegalCommands()
  end
end
