////////////////////////////////////////////
// Params.cc
//
// TDRP C++ code file for class 'Params'.
//
// Code for program PbarCompute
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
#include "Params.hh"
#include <cstring>

  ////////////////////////////////////////////
  // Default constructor
  //

  Params::Params()

  {

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // initialize table

    _init();

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Copy constructor
  //

  Params::Params(const Params& source)

  {

    // sync the source object

    source.sync();

    // zero out table

    memset(_table, 0, sizeof(_table));

    // zero out members

    memset(&_start_, 0, &_end_ - &_start_);

    // class name

    _className = "Params";

    // copy table

    tdrpCopyTable((TDRPtable *) source._table, _table);

    // set members

    tdrpTable2User(_table, &_start_);

    _exitDeferred = false;

  }

  ////////////////////////////////////////////
  // Destructor
  //

  Params::~Params()

  {

    // free up

    freeAll();

  }

  ////////////////////////////////////////////
  // Assignment
  //

  void Params::operator=(const Params& other)

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

  int Params::loadFromArgs(int argc, char **argv,
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

  int Params::loadApplyArgs(const char *params_path,
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

  bool Params::isArgValid(const char *arg)
  {
    return (tdrpIsArgValid(arg));
  }

  ////////////////////////////////////////////
  // isArgValid()
  // 
  // Check if a command line arg is a valid TDRP arg.
  // return number of args consumed.
  //

  int Params::isArgValidN(const char *arg)
  {
    return (tdrpIsArgValidN(arg));
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

  int Params::load(const char *param_file_path,
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

  int Params::loadFromBuf(const char *param_source_str,
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

  int Params::loadDefaults(int expand_env)
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

  void Params::sync(void) const
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

  void Params::print(FILE *out, tdrp_print_mode_t mode)
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

  int Params::checkAllSet(FILE *out)
  {
    return (tdrpCheckAllSet(out, _table, &_start_));
  }

  //////////////////////////////////////////////////////////////
  // checkIsSet()
  //
  // Return TRUE if parameter is set, FALSE if not.
  //
  //

  int Params::checkIsSet(const char *paramName)
  {
    return (tdrpCheckIsSet(paramName, _table, &_start_));
  }

  ////////////////////////////////////////////
  // freeAll()
  //
  // Frees up all TDRP dynamic memory.
  //

  void Params::freeAll(void)
  {
    tdrpFreeAll(_table, &_start_);
  }

  ////////////////////////////////////////////
  // usage()
  //
  // Prints out usage message for TDRP args as passed
  // in to loadFromArgs().
  //

  void Params::usage(ostream &out)
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

  int Params::arrayRealloc(const char *param_name, int new_array_n)
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

  int Params::array2DRealloc(const char *param_name,
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

  void Params::_init()

  {

    TDRPtable *tt = _table;

    // Parameter 'Comment 0'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = COMMENT_TYPE;
    tt->param_name = tdrpStrDup("Comment 0");
    tt->comment_hdr = tdrpStrDup("PbarCompute");
    tt->comment_text = tdrpStrDup("This program reads Ensemble model data and computes 'pbar' at a configured set of thresholds within a set of tiles, and writes results to SPDB.  Pbars are done for a precip field at precip thresholds, and also for a cloudtops field conditioned on both the precip field and the cloudtop field.  The precip field is field 1, the cloud top field is field 2.  A third field, typically CAPE, can optionally be used as well with a fixed threshold, to constrain the pbar values");
    tt++;
    
    // Parameter 'tilingParamFile'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("tilingParamFile");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Parameter file with tiling information");
    tt->val_offset = (char *) &tilingParamFile - &_start_;
    tt->single_val.s = tdrpStrDup("Tiling.params");
    tt++;
    
    // Parameter 'ensembleMembers'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("ensembleMembers");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("");
    tt->array_offset = (char *) &_ensembleMembers - &_start_;
    tt->array_n_offset = (char *) &ensembleMembers_n - &_start_;
    tt->is_array = TRUE;
    tt->array_len_fixed = FALSE;
    tt->array_elem_size = sizeof(char*);
    tt->array_n = 20;
    tt->array_vals = (tdrpVal_t *)
        tdrpMalloc(tt->array_n * sizeof(tdrpVal_t));
      tt->array_vals[0].s = tdrpStrDup("gep01");
      tt->array_vals[1].s = tdrpStrDup("gep02");
      tt->array_vals[2].s = tdrpStrDup("gep03");
      tt->array_vals[3].s = tdrpStrDup("gep04");
      tt->array_vals[4].s = tdrpStrDup("gep05");
      tt->array_vals[5].s = tdrpStrDup("gep06");
      tt->array_vals[6].s = tdrpStrDup("gep07");
      tt->array_vals[7].s = tdrpStrDup("gep08");
      tt->array_vals[8].s = tdrpStrDup("gep09");
      tt->array_vals[9].s = tdrpStrDup("gep10");
      tt->array_vals[10].s = tdrpStrDup("gep11");
      tt->array_vals[11].s = tdrpStrDup("gep12");
      tt->array_vals[12].s = tdrpStrDup("gep13");
      tt->array_vals[13].s = tdrpStrDup("gep14");
      tt->array_vals[14].s = tdrpStrDup("gep15");
      tt->array_vals[15].s = tdrpStrDup("gep16");
      tt->array_vals[16].s = tdrpStrDup("gep17");
      tt->array_vals[17].s = tdrpStrDup("gep18");
      tt->array_vals[18].s = tdrpStrDup("gep19");
      tt->array_vals[19].s = tdrpStrDup("gep20");
    tt++;
    
    // Parameter 'maxAgeHours'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("maxAgeHours");
    tt->descr = tdrpStrDup("maximum age hours");
    tt->help = tdrpStrDup("Maximum age in real time of data compared to current real time in order to use the data (hours)");
    tt->val_offset = (char *) &maxAgeHours - &_start_;
    tt->single_val.d = 24;
    tt++;
    
    // Parameter 'triggerFeedbackMinutes'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("triggerFeedbackMinutes");
    tt->descr = tdrpStrDup("triggering feedback minutes");
    tt->help = tdrpStrDup("Triggering mechanism will return from each URL after this number of minutes to tell the handler there has been no new input. Set this fairly small to speed up detection of timeout and disable situations.");
    tt->val_offset = (char *) &triggerFeedbackMinutes - &_start_;
    tt->single_val.d = 1;
    tt++;
    
    // Parameter 'urlTimeoutMinutes'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("urlTimeoutMinutes");
    tt->descr = tdrpStrDup("URL timout minutes");
    tt->help = tdrpStrDup("If a URL has produced data at a gen time, but no new lead times trigger, it has 'timed out'.  This parameter tells how long to wait until declaring a timeout.  It can be fairly big if this is a rare event");
    tt->val_offset = (char *) &urlTimeoutMinutes - &_start_;
    tt->single_val.d = 60;
    tt++;
    
    // Parameter 'urlDisableMinutes'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("urlDisableMinutes");
    tt->descr = tdrpStrDup("URL disable minutes");
    tt->help = tdrpStrDup("If a URL has not produced data at a gen time, but other URls have, the URL is declared 'disabled' if this many minutes have elapsed between the first data at the gen time from any URL, and the current real time");
    tt->val_offset = (char *) &urlDisableMinutes - &_start_;
    tt->single_val.d = 60;
    tt++;
    
    // Parameter 'pbarSpdb'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("pbarSpdb");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Pbar Spdb , output of this app");
    tt->val_offset = (char *) &pbarSpdb - &_start_;
    tt->single_val.s = tdrpStrDup("spdbp:://localhost::EpochOps/CMCE/pbar");
    tt++;
    
    // Parameter 'threshMin1'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshMin1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Minimum threshold to try, field1 (precip)");
    tt->val_offset = (char *) &threshMin1 - &_start_;
    tt->single_val.d = 1;
    tt++;
    
    // Parameter 'threshMax1'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshMax1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Maximum threshold to try, field1 (precip)");
    tt->val_offset = (char *) &threshMax1 - &_start_;
    tt->single_val.d = 6;
    tt++;
    
    // Parameter 'threshDelta1'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshDelta1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Step between thresholds when trying multiple, field1 (precip)");
    tt->val_offset = (char *) &threshDelta1 - &_start_;
    tt->single_val.d = 0.25;
    tt++;
    
    // Parameter 'thresholdedComparison1'
    // ctype is '_Comparison_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("thresholdedComparison1");
    tt->descr = tdrpStrDup("comparison choice");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &thresholdedComparison1 - &_start_;
    tt->enum_def.name = tdrpStrDup("Comparison_t");
    tt->enum_def.nfields = 2;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("GREATER_THAN_OR_EQUAL");
      tt->enum_def.fields[0].val = GREATER_THAN_OR_EQUAL;
      tt->enum_def.fields[1].name = tdrpStrDup("LESS_THAN_OR_EQUAL");
      tt->enum_def.fields[1].val = LESS_THAN_OR_EQUAL;
    tt->single_val.e = GREATER_THAN_OR_EQUAL;
    tt++;
    
    // Parameter 'inputThreshField1'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("inputThreshField1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input threshold data field name in ensemble model data, goes into SPDB, field1 (precip)");
    tt->val_offset = (char *) &inputThreshField1 - &_start_;
    tt->single_val.s = tdrpStrDup("APCP3Hr");
    tt++;
    
    // Parameter 'threshFieldColdstartThreshold1'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshFieldColdstartThreshold1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input thresholded field data default coldstart threshold field1 (precip)");
    tt->val_offset = (char *) &threshFieldColdstartThreshold1 - &_start_;
    tt->single_val.d = 2;
    tt++;
    
    // Parameter 'threshMin2'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshMin2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Minimum threshold to try, field2 (cloudtop)");
    tt->val_offset = (char *) &threshMin2 - &_start_;
    tt->single_val.d = 170;
    tt++;
    
    // Parameter 'threshMax2'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshMax2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Maximum threshold to try, field2 (cloudtop)");
    tt->val_offset = (char *) &threshMax2 - &_start_;
    tt->single_val.d = 350;
    tt++;
    
    // Parameter 'threshDelta2'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshDelta2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Step between thresholds when trying multiple, field2 (cloudtop)");
    tt->val_offset = (char *) &threshDelta2 - &_start_;
    tt->single_val.d = 10;
    tt++;
    
    // Parameter 'thresholdedComparison2'
    // ctype is '_Comparison_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("thresholdedComparison2");
    tt->descr = tdrpStrDup("comparison choice");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &thresholdedComparison2 - &_start_;
    tt->enum_def.name = tdrpStrDup("Comparison_t");
    tt->enum_def.nfields = 2;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("GREATER_THAN_OR_EQUAL");
      tt->enum_def.fields[0].val = GREATER_THAN_OR_EQUAL;
      tt->enum_def.fields[1].name = tdrpStrDup("LESS_THAN_OR_EQUAL");
      tt->enum_def.fields[1].val = LESS_THAN_OR_EQUAL;
    tt->single_val.e = LESS_THAN_OR_EQUAL;
    tt++;
    
    // Parameter 'inputThreshField2'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("inputThreshField2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input threshold data field name in ensemble model data, goes into SPDB, field2 (cloudtops)");
    tt->val_offset = (char *) &inputThreshField2 - &_start_;
    tt->single_val.s = tdrpStrDup("ULWRF3Hr");
    tt++;
    
    // Parameter 'threshFieldColdstartThreshold2'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("threshFieldColdstartThreshold2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input thresholded field data default coldstart threshold, field2 (cloudtops)");
    tt->val_offset = (char *) &threshFieldColdstartThreshold2 - &_start_;
    tt->single_val.d = 200;
    tt++;
    
    // Parameter 'hasFixedField1'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("hasFixedField1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Use an additional input along with a fixed threshold for field1 (precip)");
    tt->val_offset = (char *) &hasFixedField1 - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'inputFixedField1'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("inputFixedField1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input fixed field in model data, goes into SPDB as a fixed value, if hasFixedField1, ignored otherwise");
    tt->val_offset = (char *) &inputFixedField1 - &_start_;
    tt->single_val.s = tdrpStrDup("CAPE");
    tt++;
    
    // Parameter 'fixedFieldComparison1'
    // ctype is '_Comparison_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("fixedFieldComparison1");
    tt->descr = tdrpStrDup("comparison choice for fixed field, ignored if hasFixedField1 is FALSE");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &fixedFieldComparison1 - &_start_;
    tt->enum_def.name = tdrpStrDup("Comparison_t");
    tt->enum_def.nfields = 2;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("GREATER_THAN_OR_EQUAL");
      tt->enum_def.fields[0].val = GREATER_THAN_OR_EQUAL;
      tt->enum_def.fields[1].name = tdrpStrDup("LESS_THAN_OR_EQUAL");
      tt->enum_def.fields[1].val = LESS_THAN_OR_EQUAL;
    tt->single_val.e = GREATER_THAN_OR_EQUAL;
    tt++;
    
    // Parameter 'fixedThreshold1'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("fixedThreshold1");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("The fixed field threshold to use, if hasFixedField1 is true");
    tt->val_offset = (char *) &fixedThreshold1 - &_start_;
    tt->single_val.d = 200;
    tt++;
    
    // Parameter 'hasFixedField2'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("hasFixedField2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Use a input along with a fixed threshold for field2 (cloudtops)");
    tt->val_offset = (char *) &hasFixedField2 - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // Parameter 'inputFixedField2'
    // ctype is 'char*'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = STRING_TYPE;
    tt->param_name = tdrpStrDup("inputFixedField2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Input fixed field in model data, goes into SPDB as a fixed value, if hasFixedField2, ignored otherwise");
    tt->val_offset = (char *) &inputFixedField2 - &_start_;
    tt->single_val.s = tdrpStrDup("CAPE");
    tt++;
    
    // Parameter 'fixedFieldComparison2'
    // ctype is '_Comparison_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = ENUM_TYPE;
    tt->param_name = tdrpStrDup("fixedFieldComparison2");
    tt->descr = tdrpStrDup("comparison choice for fixed field, ignored if hasFixedField2 is FALSE");
    tt->help = tdrpStrDup("");
    tt->val_offset = (char *) &fixedFieldComparison2 - &_start_;
    tt->enum_def.name = tdrpStrDup("Comparison_t");
    tt->enum_def.nfields = 2;
    tt->enum_def.fields = (enum_field_t *)
        tdrpMalloc(tt->enum_def.nfields * sizeof(enum_field_t));
      tt->enum_def.fields[0].name = tdrpStrDup("GREATER_THAN_OR_EQUAL");
      tt->enum_def.fields[0].val = GREATER_THAN_OR_EQUAL;
      tt->enum_def.fields[1].name = tdrpStrDup("LESS_THAN_OR_EQUAL");
      tt->enum_def.fields[1].val = LESS_THAN_OR_EQUAL;
    tt->single_val.e = GREATER_THAN_OR_EQUAL;
    tt++;
    
    // Parameter 'fixedThreshold2'
    // ctype is 'double'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = DOUBLE_TYPE;
    tt->param_name = tdrpStrDup("fixedThreshold2");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("The fixed field threshold to use, if hasFixedField2 is true");
    tt->val_offset = (char *) &fixedThreshold2 - &_start_;
    tt->single_val.d = 200;
    tt++;
    
    // Parameter 'num_threads'
    // ctype is 'int'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = INT_TYPE;
    tt->param_name = tdrpStrDup("num_threads");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Number of threads on ensemble member, 0 or 1 for no threading");
    tt->val_offset = (char *) &num_threads - &_start_;
    tt->single_val.i = 0;
    tt++;
    
    // Parameter 'debug_state'
    // ctype is 'tdrp_bool_t'
    
    memset(tt, 0, sizeof(TDRPtable));
    tt->ptype = BOOL_TYPE;
    tt->param_name = tdrpStrDup("debug_state");
    tt->descr = tdrpStrDup("");
    tt->help = tdrpStrDup("Set true to see more debugging of internal state");
    tt->val_offset = (char *) &debug_state - &_start_;
    tt->single_val.b = pFALSE;
    tt++;
    
    // trailing entry has param_name set to NULL
    
    tt->param_name = NULL;
    
    return;
  
  }
