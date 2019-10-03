{
  'targets': [
    {
      'target_name': 'profiler',
      'win_delay_load_hook': 'true',
      'sources': [
        'src/profiler.cc',
        'src/cpu_profiler.cc',
        'src/cpu_profile.cc',
        'src/cpu_profile_node.cc',
      ],
      'include_dirs' : [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
