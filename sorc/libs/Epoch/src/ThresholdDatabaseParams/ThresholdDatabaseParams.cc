/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
/* ** Copyright UCAR                                                         */
/* ** University Corporation for Atmospheric Research (UCAR)                 */
/* ** National Center for Atmospheric Research (NCAR)                        */
/* ** Boulder, Colorado, USA                                                 */
/* ** BSD licence applies - redistribution and use in source and binary      */
/* ** forms, with or without modification, are permitted provided that       */
/* ** the following conditions are met:                                      */
/* ** 1) If the software is modified to produce derivative works,            */
/* ** such modified software should be clearly marked, so as not             */
/* ** to confuse it with the version available from UCAR.                    */
/* ** 2) Redistributions of source code must retain the above copyright      */
/* ** notice, this list of conditions and the following disclaimer.          */
/* ** 3) Redistributions in binary form must reproduce the above copyright   */
/* ** notice, this list of conditions and the following disclaimer in the    */
/* ** documentation and/or other materials provided with the distribution.   */
/* ** 4) Neither the name of UCAR nor the names of its contributors,         */
/* ** if any, may be used to endorse or promote products derived from        */
/* ** this software without specific prior written permission.               */
/* ** DISCLAIMER: THIS SOFTWARE IS PROVIDED 'AS IS' AND WITHOUT ANY EXPRESS  */
/* ** OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/* ** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.    */
/* *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=* */
////////////////////////////////////////////
// ThresholdDatabaseParams.cc
//
// TDRP C++ code file for class 'ThresholdDatabaseParams'.
//
// This file has been automatically
// generated by TDRP, do not modify.
//
/////////////////////////////////////////////

/**
 *
 * @file Params.cc
 *
 * @class Params
 *
 * This class is automatically generated by the Table
 * Driven Runtime Parameters (TDRP) system
 *
 * @note Source is automatically generated from
 *       paramdef file at compile time, do not modify
 *       since modifications will be overwritten.
 *
 *
 * @author Automatically generated
 *
 */
#include "ThresholdDatabaseParams.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  ThresholdDatabaseParams::ThresholdDatabaseParams()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "ThresholdDatabaseParams";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  ThresholdDatabaseParams::ThresholdDatabaseParams(const ThresholdDatabaseParams& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "ThresholdDatabaseParams";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  ThresholdDatabaseParams::~ThresholdDatabaseParams()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void ThresholdDatabaseParams::operator=(const ThresholdDatabaseParams& other)

  {

    // sync the other object

    other.sync();

    // free up any existing memory

    freeAll();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // copy table

    tdrpCopyTable((TDRPtable *) other._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = other._exitDeferred;

  }

  ////////////////////////////////////////////
  // loadFromArgs()
  //
  // Loads up TDRP using the command line args.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   char **params_path_p:
  //     If this is non-NULL, it is set to point to the path
  //     of the params file used.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int ThresholdDatabaseParams::loadFromArgs(int argc, char **argv,
                           char **override_list,
                           char **params_path_p,
                           bool defer_exit)
  {
    int exit_deferred;
    if (_tdrpLoadFromArgs(argc, argv,
                          _table, &_start_,
                          override_list, params_path_p,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadApplyArgs()
  //
  // Loads up TDRP using the params path passed in, and applies
  // the command line args for printing and checking.
  //
  // Check usage() for command line actions associated with
  // this function.
  //
  //   const char *param_file_path: the parameter file to be read in
  //
  //   argc, argv: command line args
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   bool defer_exit: normally, if the command args contain a 
  //      print or check request, this function will call exit().
  //      If defer_exit is set, such an exit is deferred and the
  //      private member _exitDeferred is set.
  //      Use exidDeferred() to test this flag.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int ThresholdDatabaseParams::loadApplyArgs(const char *params_path,
                            int argc, char **argv,
                            char **override_list,
                            bool defer_exit)
  {
    int exit_deferred;
    if (tdrpLoadApplyArgs(params_path, argc, argv,
                          _table, &_start_,
                          override_list,
                          _className,
                          defer_exit, &exit_deferred)) {
      return (-1);
    } else {
      if (exit_deferred) {
        _exitDeferred = true;
      }
      return (0);
    }
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  //

  bool ThresholdDatabaseParams::isArgValid(const char *arg)
  {
    return (tdrpIsArgValid(arg));
  }

  ////////////////////////////////////////////
  // load()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to load
  // up more than one class for a single application. It is a
  // lower-level routine than loadFromArgs, and hence more
  // flexible, but the programmer must do more work.
  //
  //   const char *param_file_path: the parameter file to be read in.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int ThresholdDatabaseParams::load(const char *param_file_path,
                   char **override_list,
                   int expand_env, int debug)
  {
    if (tdrpLoad(param_file_path,
                 _table, &_start_,
                 override_list,
                 expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadFromBuf()
  //
  // Loads up TDRP for a given class.
  //
  // This version of load gives the programmer the option to
  // load up more than one module for a single application,
  // using buffers which have been read from a specified source.
  //
  //   const char *param_source_str: a string which describes the
  //     source of the parameter information. It is used for
  //     error reporting only.
  //
  //   char **override_list: A null-terminated list of overrides
  //     to the parameter file.
  //     An override string has exactly the format of an entry
  //     in the parameter file itself.
  //
  //   const char *inbuf: the input buffer
  //
  //   int inlen: length of the input buffer
  //
  //   int start_line_num: the line number in the source which
  //     corresponds to the start of the buffer.
  //
  //   expand_env: flag to control environment variable
  //               expansion during tokenization.
  //               If TRUE, environment expansion is set on.
  //               If FALSE, environment expansion is set off.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int ThresholdDatabaseParams::loadFromBuf(const char *param_source_str,
                          char **override_list,
                          const char *inbuf, int inlen,
                          int start_line_num,
                          int expand_env, int debug)
  {
    if (tdrpLoadFromBuf(param_source_str,
                        _table, &_start_,
                        override_list,
                        inbuf, inlen, start_line_num,
                        expand_env, debug)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // loadDefaults()
  //
  // Loads up default params for a given class.
  //
  // See load() for more detailed info.
  //
  //  Returns 0 on success, -1 on failure.
  //

  int ThresholdDatabaseParams::loadDefaults(int expand_env)
  {
    if (tdrpLoad(NULL,
                 _table, &_start_,
                 NULL, expand_env, FALSE)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // sync()
  //
  // Syncs the user struct data back into the parameter table,
  // in preparation for printing.
  //
  // This function alters the table in a consistent manner.
  // Therefore it can be regarded as const.
  //

  void ThresholdDatabaseParams::sync(void) const
  {
    tdrpUser2Table(_table, (char *) &_start_);
  }

  ////////////////////////////////////////////
  // print()
  // 
  // Print params file
  //
  // The modes supported are:
  //
  //   PRINT_SHORT:   main comments only, no help or descriptions
  //                  structs and arrays on a single line
  //   PRINT_NORM:    short + descriptions and help
  //   PRINT_LONG:    norm  + arrays and structs expanded
  //   PRINT_VERBOSE: long  + private params included
  //

  void ThresholdDatabaseParams::print(FILE *out, tdrp_print_mode_t mode)
  {
    tdrpPrint(out, _table, _className, mode);
  }

  ////////////////////////////////////////////
  // checkAllSet()
  //
  // Return TRUE if all set, FALSE if not.
  //
  // If out is non-NULL, prints out warning messages for those
  // parameters which are not set.
  //

  int ThresholdDatabaseParams::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int ThresholdDatabaseParams::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void ThresholdDatabaseParams::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void ThresholdDatabaseParams::usage(ostream &out)
  {
    out << "TDRP args: [options as below]\n"
        << "   [ -params/--params path ] specify params file path\n"
        << "   [ -check_params/--check_params] check which params are not set\n"
        << "   [ -print_params/--print_params [mode]] print parameters\n"
        << "     using following modes, default mode is 'norm'\n"
        << "       short:   main comments only, no help or descr\n"
        << "                structs and arrays on a single line\n"
        << "       norm:    short + descriptions and help\n"
        << "       long:    norm  + arrays and structs expanded\n"
        << "       verbose: long  + private params included\n"
        << "       short_expand:   short with env vars expanded\n"
        << "       norm_expand:    norm with env vars expanded\n"
        << "       long_expand:    long with env vars expanded\n"
        << "       verbose_expand: verbose with env vars expanded\n"
        << "   [ -tdrp_debug] debugging prints for tdrp\n"
        << "   [ -tdrp_usage] print this usage\n";
  }

  ////////////////////////////////////////////
  // arrayRealloc()
  //
  // Realloc 1D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int ThresholdDatabaseParams::arrayRealloc(const char *param_name, int new_array_n)
  {
    if (tdrpArrayRealloc(_table, &_start_,
                         param_name, new_array_n)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // array2DRealloc()
  //
  // Realloc 2D array.
  //
  // If size is increased, the values from the last array 
  // entry is copied into the new space.
  //
  // Returns 0 on success, -1 on error.
  //

  int ThresholdDatabaseParams::array2DRealloc(const char *param_name,
                             int new_array_n1,
                             int new_array_n2)
  {
    if (tdrpArray2DRealloc(_table, &_start_, param_name,
                           new_array_n1, new_array_n2)) {
      return (-1);
    } else {
      return (0);
    }
  }

  ////////////////////////////////////////////
  // _init()
  //
  // Class table initialization function.
  //
  //

  void ThresholdDatabaseParams::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'Comment 0'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 0");
    tt->comment_hdr = tdrpStrDup("Thresholds Database Parms");
    tt->comment_text = tdrpStrDup("--------Params for a thresholds database ---- ");
    tt++;
    
    // Parameter 'fromDatabase'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("fromDatabase");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("TRUE if the thresholds come from a database, FALSE if this is data with a fixed threshold");
    tt->val_offset = (char *) &fromDatabase - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'fieldName'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("fieldName");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Database and Model data field name");
    tt->val_offset = (char *) &fieldName - &_start_;
    tt->single_val.s = tdrpStrDup("APCP3Hr");
    tt++;
    
    // Parameter 'compare'
    // ctype is '_Compare_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("compare");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Type of comparison to see if data passes test");
    tt->val_offset = (char *) &compare - &_start_;
    tt->enum_def.name = tdrpStrDup("Compare_t");
    tt->enum_def.nfields = 3;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("LE");
      tt->enum_def.fields[0].val = LE;
      tt->enum_def.fields[1].name = tdrpStrDup("EQ");
      tt->enum_def.fields[1].val = EQ;
      tt->enum_def.fields[2].name = tdrpStrDup("GE");
      tt->enum_def.fields[2].val = GE;
    tt->single_val.e = GE;
    tt++;
    
    // Parameter 'coldstartThresh'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("coldstartThresh");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("threshold to use when none in database (fromDatabase=true) or the fixed threshold (fromDatabase=false)");
    tt->val_offset = (char *) &coldstartThresh - &_start_;
    tt->single_val.d = 8.98847e+307;
    tt++;
    
    // Parameter 'doOutputThresholdsGrid'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("doOutputThresholdsGrid");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("TRUE to output the thresholds grids along with probabilities for this field");
    tt->val_offset = (char *) &doOutputThresholdsGrid - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'Comment 1'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 1");
    tt->comment_hdr = tdrpStrDup("Params for when fromDatabase=TRUE");
    tt->comment_text = tdrpStrDup("--------The remaining params are used only when fromDatabase=TRUE-----------");
    tt++;
    
    // Parameter 'databaseUrl'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("databaseUrl");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("location of thresholds database");
    tt->val_offset = (char *) &databaseUrl - &_start_;
    tt->single_val.s = tdrpStrDup("not_set");
    tt++;
    
    // Parameter 'isOutputObar'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("isOutputObar");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("TRUE if this database is the one from which obar thresholds are used to create probabiliites (only one field can have this set to TRUE).  Output probability fields get created for each obar threshold specified here.");
    tt->val_offset = (char *) &isOutputObar - &_start_;
    tt->single_val.b = pTRUE;
    tt++;
    
    // Parameter 'numFieldsWithOutsideThresh'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("numFieldsWithOutsideThresh");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Number of fields total that have an outside threshold.  The outside thresholds are used ONLY when isOutputObar is true, ie for only one field. At most 4 fields are allowed in the design");
    tt->val_offset = (char *) &numFieldsWithOutsideThresh - &_start_;
    tt->single_val.i = 3;
    tt++;
    
    // Parameter 'obsSpec'
    // ctype is '_ObsThresh_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRUCT_TYPE;
    tt->param_name = tdrpStrDup("obsSpec");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("The specs for each obar threshold expected in inputs\nobsThresh = the obs threshold expected in the database\ngriddedFieldName = name of output probabiity field (used only when isOutputObar is true)\nfield1 = first field with an outside threshold\noutsideThresh1 = the outside threshold for the first field\nfield2 = second field with an outside threshold\noutsideThresh2 = the outside threshold for the second field\nfield3 = third field with an outside threshold\noutsideThresh3 = the outside threshold for the third field\nfield4 = fourth field with an outside threshold\noutsideThresh4 = the outside threshold for the fourth field\nNOTE: Expect the list of fields to be complete (all fields for which thresholds are used)");
    tt->array_offset = (char *) &_obsSpec - &_start_;
    tt->array_n_offset = (char *) &obsSpec_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(ObsThresh_t);
    tt->array_n = 1;
    tt->struct_def.name = tdrpStrDup("ObsThresh_t");
    tt->struct_def.nfields = 10;
    tt->struct_def.fields = (struct_field_t *)
        tdrpMalloc(tt->struct_def.nfields * sizeof(struct_field_t));
      tt->struct_def.fields[0].ftype = tdrpStrDup("double");
      tt->struct_def.fields[0].fname = tdrpStrDup("obsThresh");
      tt->struct_def.fields[0].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[0].rel_offset = 
        (char *) &_obsSpec->obsThresh - (char *) _obsSpec;
      tt->struct_def.fields[1].ftype = tdrpStrDup("string");
      tt->struct_def.fields[1].fname = tdrpStrDup("griddedFieldName");
      tt->struct_def.fields[1].ptype = STRING_TYPE;
      tt->struct_def.fields[1].rel_offset = 
        (char *) &_obsSpec->griddedFieldName - (char *) _obsSpec;
      tt->struct_def.fields[2].ftype = tdrpStrDup("string");
      tt->struct_def.fields[2].fname = tdrpStrDup("field1");
      tt->struct_def.fields[2].ptype = STRING_TYPE;
      tt->struct_def.fields[2].rel_offset = 
        (char *) &_obsSpec->field1 - (char *) _obsSpec;
      tt->struct_def.fields[3].ftype = tdrpStrDup("double");
      tt->struct_def.fields[3].fname = tdrpStrDup("outsideThresh1");
      tt->struct_def.fields[3].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[3].rel_offset = 
        (char *) &_obsSpec->outsideThresh1 - (char *) _obsSpec;
      tt->struct_def.fields[4].ftype = tdrpStrDup("string");
      tt->struct_def.fields[4].fname = tdrpStrDup("field2");
      tt->struct_def.fields[4].ptype = STRING_TYPE;
      tt->struct_def.fields[4].rel_offset = 
        (char *) &_obsSpec->field2 - (char *) _obsSpec;
      tt->struct_def.fields[5].ftype = tdrpStrDup("double");
      tt->struct_def.fields[5].fname = tdrpStrDup("outsideThresh2");
      tt->struct_def.fields[5].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[5].rel_offset = 
        (char *) &_obsSpec->outsideThresh2 - (char *) _obsSpec;
      tt->struct_def.fields[6].ftype = tdrpStrDup("string");
      tt->struct_def.fields[6].fname = tdrpStrDup("field3");
      tt->struct_def.fields[6].ptype = STRING_TYPE;
      tt->struct_def.fields[6].rel_offset = 
        (char *) &_obsSpec->field3 - (char *) _obsSpec;
      tt->struct_def.fields[7].ftype = tdrpStrDup("double");
      tt->struct_def.fields[7].fname = tdrpStrDup("outsideThresh3");
      tt->struct_def.fields[7].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[7].rel_offset = 
        (char *) &_obsSpec->outsideThresh3 - (char *) _obsSpec;
      tt->struct_def.fields[8].ftype = tdrpStrDup("string");
      tt->struct_def.fields[8].fname = tdrpStrDup("field4");
      tt->struct_def.fields[8].ptype = STRING_TYPE;
      tt->struct_def.fields[8].rel_offset = 
        (char *) &_obsSpec->field4 - (char *) _obsSpec;
      tt->struct_def.fields[9].ftype = tdrpStrDup("double");
      tt->struct_def.fields[9].fname = tdrpStrDup("outsideThresh4");
      tt->struct_def.fields[9].ptype = DOUBLE_TYPE;
      tt->struct_def.fields[9].rel_offset = 
        (char *) &_obsSpec->outsideThresh4 - (char *) _obsSpec;
    tt->n_struct_vals = 10;
    tt->struct_vals = (tdrpVal_t *)
        tdrpMalloc(tt->n_struct_vals * sizeof(tdrpVal_t));
      tt->struct_vals[0].d = 4;
      tt->struct_vals[1].s = tdrpStrDup("ProbConvWx");
      tt->struct_vals[2].s = tdrpStrDup("CAPE");
      tt->struct_vals[3].d = 200;
      tt->struct_vals[4].s = tdrpStrDup("APCP3Hr");
      tt->struct_vals[5].d = 4;
      tt->struct_vals[6].s = tdrpStrDup("ULWRF3hr");
      tt->struct_vals[7].d = 200;
      tt->struct_vals[8].s = tdrpStrDup("NotUsed");
      tt->struct_vals[9].d = 0;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }
