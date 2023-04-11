#!/usr/bin/env python

import sys
import pytest
import icing_app


@pytest.mark.parametrize(
    'key, value', [
        ({'app_name':"test", 'instance':"inst", 'check_outdir':"True", 'start_time':"", 'stop_time':"", 'in_file':"", 'opt_in_file':"", 'out_dir':"", 'diag_dir':"", 'rt':"", 'additional_args':"", 'time_center':"", 'time_radius':""}, ""),
        ({'app_name':"test", 'instance':"inst", 'check_outdir':"True", 'start_time':"", 'stop_time':"", 'in_file':"", 'opt_in_file':"", 'out_dir':"/out/dir", 'diag_dir':"", 'rt':"", 'additional_args':"", 'time_center':"5", 'time_radius':"10"}, "run_test.inst.sh 5 10 /out/dir"),
        ({'app_name':"test", 'instance':"inst", 'check_outdir':"True", 'start_time':"", 'stop_time':"", 'in_file':"/input/file/name", 'opt_in_file':"", 'out_dir':"/out/dir", 'diag_dir':"", 'rt':"", 'additional_args':"", 'time_center':"", 'time_radius':""}, "run_test.inst.sh /input/file/name /out/dir inst"),
        ]
)
def test_get_app_cmd(key, value):
    assert(icing_app.get_app_cmd(**key) == value)
