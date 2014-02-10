
require File.join( File.dirname(__FILE__), 'demo_utilities' )
  
def generateToy(startingFragmentId, totalEBs, totalFRs,
                fragSizeWords, boardId, fragmentsPerBoard, fragmentType)

  toy_sim_config = "\
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
    + read_fcl("ToySimulator.fcl") \
    + " }\
    }"
  
  toyConfig = String.new(toy_sim_config)
  
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
