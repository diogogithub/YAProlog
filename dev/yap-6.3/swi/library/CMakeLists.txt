set (LIBRARY_PL
  aggregate.pl
  base64.pl
  broadcast.pl
  ctypes.pl
  date.pl
  debug.pl
  edit.pl
  main.pl
  menu.pl
  nb_set.pl
  occurs.yap
  operators.pl
  option.pl
  pairs.pl
  predicate_options.pl
  predopts.pl
  prolog_clause.pl
  prolog_colour.pl
  prolog_source.pl
  prolog_xref.pl
  pure_input.pl
  quasi_quotations.pl
  quintus.pl
  record.pl
  settings.pl
  shlib.pl
  thread_pool.pl
  url.pl
  utf8.pl
  win_menu.pl
  www_browser.pl
  dcg/basics.pl
  )



install(FILES ${LIBRARY_PL}
  DESTINATION ${libpl}
  )




