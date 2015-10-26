#!/bin/env ruby


require "erb"

firstport = 5200

system = [
      {"rank" => 0, "tier" =>0, "rpc"=>3, "name" => "BoardReader",  "conf" => ["boardreader.erb.fcl",     {"src" => [0,1], "tgt" => [2,3], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
      {"rank" => 1, "tier" =>0, "rpc"=>3, "name" => "BoardReader",  "conf" => ["boardreader.erb.fcl",     {"src" => [1,2], "tgt" => [2,3], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
  
      {"rank" => 2, "tier" =>1, "rpc"=>4, "name" => "EventBuilder", "conf" => ["eventbuilder_t1.erb.fcl", {"src" => [0,1], "tgt" => [4,5], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
      {"rank" => 3, "tier" =>1, "rpc"=>4, "name" => "EventBuilder", "conf" => ["eventbuilder_t1.erb.fcl", {"src" => [0,1], "tgt" => [4,5], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
  
      {"rank" => 4, "tier" =>2, "rpc"=>4, "name" => "EventBuilder", "conf" => ["eventbuilder_t2.erb.fcl", {"src" => [2,3], "tgt" => [6,7], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
      {"rank" => 5, "tier" =>2, "rpc"=>4, "name" => "EventBuilder", "conf" => ["eventbuilder_t2.erb.fcl", {"src" => [2,3], "tgt" => [6,7], "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },

      {"rank" => 6, "tier" =>3, "rpc"=>5, "name" => "Aggregator",   "conf" => ["aggregator.erb.fcl",      {"src" => [4,5], "tgt" => []    , "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", },
      {"rank" => 7, "tier" =>3, "rpc"=>5, "name" => "Aggregator",   "conf" => ["aggregator.erb.fcl",      {"src" => [4,5], "tgt" => []    , "fpe" =>1 , "epb" =>1  }],"host"=> "localhost", }
  ]

build_xmlrpc_client_list = lambda{ |s, p| 
  list = "" 
  s.each {|c| list+=(";http://%s:%s/RPC2,%s" % [ c["host"], (p + c["rank"]).to_s, c["rpc"].to_s])} 
  return "\"" + list + "\"" 
}

class ConfigWriter
  def initialize config, xmlrpclist, firstport
    @@firstport = firstport
    @rank = config["rank"]
    @tier = config["tier"]        
    @infile = config["conf"][0]
    @outfile = "%s_%s_%s.fcl.out" % [config["name"] , config["host"], (@@firstport + config["rank"]).to_s ]
    #@xmlrpc_clients = xmlrpc_client_list_from	
    @conf = config["conf"][1]
    @xmlrpclist = xmlrpclist
  end

  def write
    result = ERB.new(File.read(@infile)).result( binding )
    outputFile = File.new(@outfile,File::CREAT|File::TRUNC|File::RDWR)        
    outputFile.write(result)
    outputFile.close
  end
end

#main program
system.each {|cfg| ConfigWriter.new(cfg,build_xmlrpc_client_list.call(system,firstport),firstport).write }    

