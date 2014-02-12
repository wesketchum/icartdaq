
require File.join( File.dirname(__FILE__), 'demo_utilities' )

def generateWFViewer(totalFRs, fragmentsPerBoard, fragmentIDList, fragmentTypeList)

  phys_anal_onmon_cfg = "\
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
      + read_fcl("WFViewer.fcl") \
      + "    }"


    # John F., 1/21/14 -- before sending FHiCL configurations to the
    # EventBuilderMain and AggregatorMain processes, construct the
    # strings listing fragment ids and fragment types which will be
    # used by the WFViewer

    fragmentIDListString, fragmentTypeListString = "[ ", "[ "

    fragmentIDList.each { |id| fragmentIDListString += " %d," % [ id ] }
    fragmentTypeList.each { |type| fragmentTypeListString += "%s," % [type ] }

    fragmentIDListString[-1], fragmentTypeListString[-1] = "]", "]" 

  wfViewerConfig = String.new(phys_anal_onmon_cfg)
  wfViewerConfig.gsub!(/\%\{total_frs\}/, String(totalFRs))
  wfViewerConfig.gsub!(/\%\{fragments_per_board\}/, String(fragmentsPerBoard))
  wfViewerConfig.gsub!(/\%\{fragment_ids\}/, String(fragmentIDListString))
  wfViewerConfig.gsub!(/\%\{fragment_type_labels\}/, String(fragmentTypeListString))
  return wfViewerConfig
end

