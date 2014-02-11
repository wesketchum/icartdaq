
require File.join( File.dirname(__FILE__), 'demo_utilities' )
  
def generateToy(startingFragmentId, boardId, fragmentsPerBoard, fragmentType)

  toyConfig = String.new( "\
    generator: ToySimulator
    fragment_type: %{fragment_type}
    fragments_per_board: %{fragments_per_board}
    fragment_id: %{starting_fragment_id}
    board_id: %{board_id}
    random_seed: %{random_seed}
    sleep_on_stop_us: 500000 " \
    + read_fcl("ToySimulator.fcl") )
  
  toyConfig.gsub!(/\%\{starting_fragment_id\}/, String(startingFragmentId))
  toyConfig.gsub!(/\%\{fragments_per_board\}/, String(fragmentsPerBoard))
  toyConfig.gsub!(/\%\{board_id\}/, String(boardId))
  toyConfig.gsub!(/\%\{random_seed\}/, String(rand(10000))) 
  toyConfig.gsub!(/\%\{fragment_type\}/, fragmentType) 
  return toyConfig

end
