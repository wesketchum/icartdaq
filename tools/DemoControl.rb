#!/usr/bin/env ruby

require "optparse"
require "ostruct"
require "xmlrpc/client"

# It'd be nice if we were using a newer version of ruby (1.9.3) that had
# better string format substitution.  For the time being we'll setup our
# string constants using the newer convention and do the substitutions.
EB_DISK_CONFIG = "\
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
}
daq: {
  max_fragment_size_words: 524288
  event_builder: {
    mpi_buffer_count: %{buffer_count}
    first_fragment_receiver_rank: 0
    fragment_receiver_count: %{total_frs}
    expected_fragments_per_event: %{total_frs}
    use_art: true
    print_event_store_stats: true
  }
}
outputs: {
  normalOutput: {
    module_type: RootOutput
    fileName: \"%{output_file}\"
    compressionLevel: 0
    %{drop_uncompressed}outputCommands: [ \"keep *\", \"drop artdaq::Fragments_daq_V1720_*\", \"drop artdaq::Fragments_daq_V1724_*\" ]
  }
}
source: {
  module_type: RawInput
  waiting_time: 20
  resume_after_timeout: true
  fragment_type_map: [[1, \"missed\"], [3, \"V1720\"], [4, \"V1724\"]]
}
physics: {
  producers: {
    # compress the raw
    huffdiffV1720: @local::huffdiff_1720  
    huffdiffV1724: @local::huffdiff_1724 

    # uncompress the raw
    dunhuffV1720: @local::dunhuff_1720
    dunhuffV1724: @local::dunhuff_1724
  }
  %{enable_1720_compression}p1: [ huffdiffV1720 ]
  %{enable_1724_compression}p1: [ huffdiffV1724 ]
  %{enable_172x_compression}p1: [ huffdiffV1720, huffdiffV1724 ]
  %{enable_compression}trigger_paths: [ p1 ]
  my_output_modules: [ normalOutput ]
  end_paths: [ my_output_modules ]
}
process_name: DAQ"

V1720_SIM_CONFIG = "\
daq: {
  max_fragment_size_words: 524288 
  fragment_receiver: {
    mpi_buffer_count: %{buffer_count}
    mpi_sync_interval: 50
    first_event_builder_rank: %{total_frs}
    event_builder_count: %{total_ebs}
    generator: V172xSimulator 
    freqs_file: \"V1720_sample_freqs.dat\" 
    fragments_per_event: 1 
    nChannels: 3000 
    starting_fragment_id: %{fragment_id}
    board_id: %{board_id}
    fragment_id: %{fragment_id}
    rt_priority: 2
  }
}"

class ConfigGen
  def generateEventBuilder(ebIndex, totalFRs, dataDir, runNumber,
                           compressionLevel, totalv1720s, totalv1724s)
    # Do the substitutions in the event builder configuration given the options
    # that were passed in from the command line.  
    ebConfig = String.new(EB_DISK_CONFIG)
    ebConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
    ebConfig.gsub!(/\%\{buffer_count\}/, String(totalFRs*10))
    if Integer(compressionLevel) > 0
      if Integer(totalv1720s) > 0 and Integer(totalv1724s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "")
        ebConfig.gsub!(/\%\{enable_compression\}/, "")
      elsif Integer(totalv1720s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_compression\}/, "")
      elsif Integer(totalv1724s) > 0
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_compression\}/, "")
      else
        ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
        ebConfig.gsub!(/\%\{enable_compression\}/, "#")
      end
    else
      ebConfig.gsub!(/\%\{enable_1720_compression\}/, "#")
      ebConfig.gsub!(/\%\{enable_1724_compression\}/, "#")
      ebConfig.gsub!(/\%\{enable_172x_compression\}/, "#")
      ebConfig.gsub!(/\%\{enable_compression\}/, "#")
    end
    if Integer(compressionLevel) > 1
      ebConfig.gsub!(/\%\{drop_uncompressed\}/, "")
    else
      ebConfig.gsub!(/\%\{drop_uncompressed\}/, "#")
    end
    currentTime = Time.now
    #fileName = "artdaqdemo_run%06d_eb%02d_" % [Integer(runNumber), ebIndex]
    fileName = "artdaqdemo_eb%02d_" % [ebIndex]
    #fileName += "%d%02d%02d-%02d%02d%02d" % [currentTime.year, currentTime.month,
    #                                         currentTime.day, currentTime.hour,
    #                                         currentTime.min, currentTime.sec]
    fileName += "r%06r_sr%02s_%to"
    fileName += ".root"
    outputFile = File.join(dataDir, fileName)
    ebConfig.gsub!(/\%\{output_file\}/, outputFile)
    return ebConfig
  end

  def generateV1720(fragmentID, totalEBs, totalFRs, v1720Index)
    # Do the substitutions in the V1720 configuration given the options that 
    # were passed in from the command line.
    v1720Config = String.new(V1720_SIM_CONFIG)
    
    v1720Config.gsub!(/\%\{total_ebs\}/, String(totalEBs))
    v1720Config.gsub!(/\%\{total_frs\}/, String(totalFRs))
    v1720Config.gsub!(/\%\{fragment_id\}/, String(fragmentID))
    v1720Config.gsub!(/\%\{buffer_count\}/, String(totalEBs*10))
    v1720Config.gsub!(/\%\{board_id\}/, String(v1720Index))
    return v1720Config
  end
end

class CommandLineParser
  def initialize(configGen)
    @configGen = configGen
    @options = OpenStruct.new
    @options.eventBuilders = []
    @options.v1720s = []
    @options.dataDir = nil
    @options.command = nil
    @options.summary = false
    @options.runNumber = "0101"
    @options.serialize = false

    @optParser = OptionParser.new do |opts|
      opts.banner = "Usage: DemoControl.rb [options]"
      opts.separator ""
      opts.separator "Specific options:"

      opts.on("--eb [host,port,compression_level]", Array,
              "Add an event builder that runs on the",
              "specified host and port using the specified",
              "compression level. Compression levels:",
              "  0: no compression",
              "  1: compress ADC data and keep everything",
              "  2: compress ADC data and discard uncompressed data") do |eb|
        if eb.length != 3
          puts "You must specifiy a host, port, and compression level"
          puts "for the event builder."
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

      opts.on("--v1720 [host,port,fragment_id]", Array, 
              "Add a V1720 fragment receiver that runs on",
              "the specified host, port and fragment ID.") do |v1720|
        if v1720.length != 3
          puts "You must specifiy a host, port, and fragment ID."
          exit
        end
        v1720Config = OpenStruct.new
        v1720Config.host = v1720[0]
        v1720Config.port = Integer(v1720[1])
        v1720Config.fragment_id = Integer(v1720[2])
        v1720Config.kind = "v1720"
        v1720Config.index = @options.v1720s.length
        @options.v1720s << v1720Config
      end

      opts.on("--v1724 [host,port,fragment_id]", Array, 
              "Add a V1724 fragment receiver that runs on",
              "the specified host, port and fragment ID.") do |v1724|
        if v1724.length != 3
          puts "You must specifiy a host, port, and fragment ID."
          exit
        end
        v1724Config = OpenStruct.new
        v1724Config.host = v1724[0]
        v1724Config.port = Integer(v1724[1])
        v1724Config.fragment_id = Integer(v1724[2])
        v1724Config.kind = "v1724"
        # NOTE that we're simply adding this to the 1720 list...
        @options.v1720s << v1724Config
      end

      opts.on("-d", "--data-dir [data dir]", 
              "Directory that the event builders will", "write data to.") do |dataDir|
        @options.dataDir = dataDir
      end

      opts.on("-c", "--command [command]", 
              "Execute a command: start, stop, init, shutdown, pause, resume.") do |command|
        @options.command = command
      end

      opts.on("-r", "--run-number [number]", "Specify the run number.") do |run|
        @options.runNumber = run
      end

      opts.on("-s", "--summary", "Summarize the configuration.") do
        @options.summary = true
      end

      opts.on("-e", "--serialize", "Serialize the config for System Control.") do
        @options.serialize = true
      end

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

  def summarize()
    # Print out a summary of the configuration that was passed in from the
    # the command line.  Everything will be printed in terms of what process
    # is running on which host.
    puts "Configuration Summary:"
    hostMap = {}
    (@options.eventBuilders + @options.v1720s).each do |proc|
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

      totalFRs = @options.v1720s.length
      hostMap[host].each do |item|
        case item.kind
        when "eb"
          puts "    EventBuilder, port %d, rank %d" % [item.port, totalFRs + item.index]
        when "v1720", "v1724"
          puts "    FragmentReceiver, Simulated %s, port %d, rank %d" % [item.kind.upcase,
                                                                         item.port,
                                                                         item.index]
        end
      end
      puts ""
    }
    return nil
  end

  def serialize()
    # The current system control pulls the FHCL config to initialize DAQ
    # processes from a series of files on disk.  The files are named as follows:
    #  initorder#hostname#port#description#on.fcl
    # 
    # This method will generate configurations given the command line options
    # and write out files for System Control.
    puts "Serializing config:"
    procIndex = 1
    v1720Index = 0
    ebIndex = 0
    totalv1720s = 0
    totalv1724s = 0
    @options.v1720s.each do |proc|
      case proc.kind
      when "v1720"
        totalv1720s += 1
      when "v1724"
        totalv1724s += 1
      end
    end
    totalFRs = @options.v1720s.length
    totalEBs = @options.eventBuilders.length
    desc = "%dx%d" % [totalFRs, totalEBs]
    (@options.eventBuilders + @options.v1720s).each do |proc|
      fileName = "%d#%s#%d#%s#on.fcl" % [procIndex, proc.host, proc.port, desc]
      procIndex += 1
      puts "  writing %s..." % fileName
      handle = File.open(fileName, "w")
      case proc.kind
      when "eb"
        cfg = @configGen.generateEventBuilder(ebIndex, totalFRs, @options.dataDir,
                                              @options.runNumber,
                                              proc.compression_level,
                                              totalV1720s, totalv1724s)
        ebIndex += 1
      when "v1720", "v1724"
        cfg = @configGen.generateV1720(proc.fragment_id, totalEBs, totalFRs, v1720Index)
        v1720Index += 1
      end
      handle.write(cfg)
      handle.close()
    end
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
    # The DAQ components need to be initialized in parallel as this is the step
    # in which connections are made between the various components.  The XMLRPC
    # call will block until all connections have been made which means that the
    # configurations need to be posted to all processes at the same time.
    threads = []
    ebIndex = 0
    totalv1720s = 0
    totalv1724s = 0
    @options.v1720s.each do |proc|
      case proc.kind
      when "v1720"
        totalv1720s += 1
      when "v1724"
        totalv1724s += 1
      end
    end
    totalFRs = @options.v1720s.length
    totalEBs = @options.eventBuilders.length
    @options.eventBuilders.each { |ebOptions|
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(ebOptions.host, "/RPC2", 
                                          ebOptions.port)
        cfg = @configGen.generateEventBuilder(ebIndex, totalFRs, @options.dataDir,
                                              @options.runNumber,
                                              ebOptions.compression_level,
                                              totalv1720s, totalv1724s)
        result = xmlrpcClient.call("daq.init", cfg)
        puts "EventBuilder on %s:%d result: %s" % [ebOptions.host, ebOptions.port,
                                                   result]
      end
      ebIndex += 1
    }

    v1720Index = 0
    @options.v1720s.each { |v1720Options|
      threads << Thread.new() do
        xmlrpcClient = XMLRPC::Client.new(v1720Options.host, "/RPC2",
                                          v1720Options.port)
        cfg = @configGen.generateV1720(v1720Options.fragment_id,
                                       totalEBs, totalFRs, v1720Index)
        result = xmlrpcClient.call("daq.init", cfg)
        puts "V1720 FragmentReceiver on %s:%d result: %s" % [v1720Options.host, 
                                                             v1720Options.port,
                                                             result]
      end
      v1720Index += 1
    }
    
    threads.each { |aThread|
      aThread.join()
    }
  end

  def start(runNumber)
    # Starting runs is simple in that all processes are sent the same message
    # and that can be done serially.
    (@options.eventBuilders + @options.v1720s).each do |proc|
      puts "Attempting to connect to %s:%d and start a run." % [proc.host, 
                                                                proc.port]
      xmlrpcClient = XMLRPC::Client.new(proc.host, "/RPC2", proc.port)
      result = xmlrpcClient.call("daq.start", runNumber)
      case proc.kind
      when "eb"
        puts "EventBuilder on %s:%d result: %s" % [proc.host, proc.port, result]
      when "v1720", "v1724"
        puts "%s FragmentReceiver on %s:%d result: %s" % [proc.kind.upcase, proc.host,
                                                          proc.port, result]
      end
    end
  end

  def sendSerialCommand(commandName, procs)
    procs.each do |proc|
      puts "Attempting to connect to %s:%d and %s a run." % [proc.host, 
                                                             proc.port,
                                                             commandName]
      xmlrpcClient = XMLRPC::Client.new(proc.host, "/RPC2", proc.port)
      result = xmlrpcClient.call("daq.%s" % [commandName])
      case proc.kind
      when "eb"
        puts "EventBuilder on %s:%d result: %s" % [proc.host, proc.port, 
                                                   result]
      when "v1720"
        puts "V1720 FragmentReceiver on %s:%d result: %s" % [proc.host, proc.port,
                                                             result]
      when "v1170"
        puts "V1170 FragmentReceiver on %s:%d result: %s" % [proc.host, proc.port,
                                                             result]
      end
    end
  end

  def shutdown()
    self.sendSerialCommand("shutdown", @options.eventBuilders +
                           @options.v1720s)
  end

  def pause()
    self.sendSerialCommand("pause", @options.v1720s +
                           @options.eventBuilders)
  end

  def stop()
    self.sendSerialCommand("stop", @options.v1720s +
                           @options.eventBuilders)
  end

  def resume()
    self.sendSerialCommand("resume", @options.eventBuilders +
                           @options.v1720s)
  end
end

if __FILE__ == $0
  cfgGen = ConfigGen.new
  cmdLineParser = CommandLineParser.new(cfgGen)
  cmdLineParser.parse()
  options = cmdLineParser.getOptions()

  if options.summary
    cmdLineParser.summarize()
  end

  if options.serialize
    cmdLineParser.serialize()
    exit
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
  end
end
