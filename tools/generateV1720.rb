
require File.join( File.dirname(__FILE__), 'demo_utilities' )


def generateV1720(startingFragmentId, totalEBs, totalFRs,
                  fragSizeWords, boardId, fragmentsPerBoard, fragmentType)

v1720_sim_config = "\
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
    + read_fcl("V172xSimulator.fcl") \
    + " } \
    }"



  v1720Config = String.new(v1720_sim_config)
    
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
