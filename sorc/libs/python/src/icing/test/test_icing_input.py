#!/usr/bin/env python

import os
import sys
import pytest
import icing_input

from datetime import datetime

test_ymdhm = "201902010608"
test_ymdh = "2019020106"

test_unix = int(datetime.strptime(test_ymdhm, '%Y%m%d%H%M').strftime('%s'))
test_unix_ymdh = int(datetime.strptime(test_ymdh, '%Y%m%d%H').strftime('%s'))

envs_to_test = {'FIP_MODEL_NAME': "%Y%m%d_i%G_f0%F_WRF-RR-NCO.grb2",
                'FIP_MODEL_DIR': '/home/mccabe/WRF-RR-NCO-130-bgrb/%Y%m%d',
                'CURRENT_DIR': os.getcwd(),
}

@pytest.mark.parametrize(
    'run_time, archive_time', [
        (test_ymdh, test_unix_ymdh),
        (test_ymdhm, test_unix),
        ]
)
def test_get_archive_time(run_time, archive_time):
    icing_input.icing_message.debug_mode = True
    assert(icing_input.get_archive_time(run_time) == archive_time)

@pytest.mark.parametrize(
    'inputs, template', [
        ({'template':envs_to_test['FIP_MODEL_DIR'], 'run_time':test_unix,},
             '/home/mccabe/WRF-RR-NCO-130-bgrb/20190201'),
        ({'template':envs_to_test['FIP_MODEL_NAME'], 'run_time':test_unix, 'gen_hour':9, 'forecast':3},
             "20190201_i09_f003_WRF-RR-NCO.grb2"),
        ({'template':envs_to_test['FIP_MODEL_NAME'], 'run_time':test_unix, 'gen_hour':12, 'forecast':12},
             "20190201_i12_f012_WRF-RR-NCO.grb2"),
        ]
)
def test_fill_template(inputs, template):
    icing_input.icing_message.debug_mode = True
    assert(icing_input.fill_template(**inputs) == template)

@pytest.mark.parametrize(
    'run_time, offset, time_str', [
(test_unix, 0, "2019 02 01 06 00 00"),
(test_unix, 1, "2019 02 01 05 59 00"),
(test_unix, 60, "2019 02 01 05 59 00"),
(test_unix, 3600, "2019 02 01 05 00 00"),
(test_unix, 25200, "2019 01 31 23 00 00"),
         ]
)
def test_build_app_time_str(run_time, offset, time_str):
    icing_input.icing_message.debug_mode = True
    assert(icing_input.build_app_time_str(run_time, offset) == time_str)


@pytest.mark.parametrize(
    'run_time, offset, time_str', [
(test_unix, 0, "2019 02 01 06 08 00"),
(test_unix, 1, "2019 02 01 06 07 00"),
(test_unix, 60, "2019 02 01 06 07 00"),
(test_unix, 3600, "2019 02 01 05 08 00"),
(test_unix, 25200, "2019 01 31 23 08 00"),
         ]
)
def test_build_app_time_str_no_truncation(run_time, offset, time_str):
    icing_input.icing_message.debug_mode = True
    assert(icing_input.build_app_time_str_no_truncation(run_time, offset) == time_str)


# To implement test for icing_input.check_for_file, we would need to run LdataWriter to generate an
# ldata info file so it exists within the max valid age, then check the value of the relative path
#def test_check_for_file():
#    assert(icing_input.check_for_file(test_dir) == test_ldata_rel_path)



# To test check_environ_dir with a run_time, tile_num, or yesterdayOK argument, we will need to
#  create test directories to check
# uses:
#  icing_input.check_environ_dir('FIP_MODEL_DIR', p.opt['run_time'])
#  icing_input.check_environ_dir('FIP_DATA_DIR')
#  icing_input.check_environ_dir('FIP_LDM_DIR')

@pytest.mark.parametrize(
    'inputs, result', [
({'env_var':'CURRENT_DIR',},True),
#({'env_var':'FIP_MODEL_DIR', 'run_time':test_unix,}True),
         ]
)
def test_check_environ_dir(inputs, result):
    env_var = inputs['env_var']
    os.environ[env_var] = envs_to_test[env_var]
    assert(icing_input.check_environ_dir(**inputs) == result)


#icing_input.get_gen_path(p.opt['run_time'], mdv_fip_pressure_dir, \
#icing_input.get_gen_path(p.opt['run_time'], grib_fip_dir, '%Y%m%d_%H0000.grb', fcast)
#icing_input.get_gen_path(p.opt['run_time'], grib2_fip_dir, '%Y%m%d/%Y%m%d_%H0000.grb2', fcast)
# get_gen_path(run_time, base_dir, fn_tmplate, fcast=0, add_date=False):

@pytest.mark.parametrize(
    'inputs, gen_path', [
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'', 'fcast':0, 'add_date':False},''),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'', 'fcast':0, 'add_date':False},'/base/dir/path/'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'', 'fcast':3, 'add_date':False},'/base/dir/path/f03/'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'', 'fcast':0, 'add_date':True},'/base/dir/path/20190201/'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'', 'fcast':3, 'add_date':True},'/base/dir/path/f03/20190201/'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'%H%M%S.ext', 'fcast':0, 'add_date':False},'/base/dir/path/060800.ext'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'%H%M%S.ext', 'fcast':3, 'add_date':False},'/base/dir/path/f03/060800.ext'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'%H%M%S.ext', 'fcast':0, 'add_date':True},'/base/dir/path/20190201/060800.ext'),
({'run_time':test_unix, 'base_dir':'/base/dir/path', 'fn_tmplate':'%H%M%S.ext', 'fcast':3, 'add_date':True},'/base/dir/path/f03/20190201/060800.ext'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'', 'fcast':3, 'add_date':False},'f03/'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'', 'fcast':0, 'add_date':True},'20190201/'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'', 'fcast':3, 'add_date':True},'f03/20190201/'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'%H%M%S.ext', 'fcast':0, 'add_date':False},'060800.ext'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'%H%M%S.ext', 'fcast':3, 'add_date':False},'f03/060800.ext'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'%H%M%S.ext', 'fcast':0, 'add_date':True},'20190201/060800.ext'),
({'run_time':test_unix, 'base_dir':'', 'fn_tmplate':'%H%M%S.ext', 'fcast':3, 'add_date':True},'f03/20190201/060800.ext'),
        ]
)
def test_get_gen_path(inputs, gen_path):
    assert(icing_input.get_gen_path(**inputs) == gen_path)

# To test get_file success, we will have to write or touch a file before calling the function
# uses:
#  icing_input.get_file(pathname, 5):
@pytest.mark.parametrize(
    'inputs, result', [
({'file':'file-that-does-not-exist', 'timeout':1}, 0),
        ]
)
def test_get_file(inputs, result):
    assert(icing_input.get_file(**inputs) == result)

# To test get_wrf_rr_model_file, we will have to create a file that matches the format of the env vars
#  before running the function
# uses:
#  icing_input.get_wrf_rr_model_file('FIP_MODEL_DIR', 'FIP_MODEL_NAME', \
#  icing_input.get_wrf_rr_model_file('FIP_MODEL_DIR', 'FIP_MODEL_NAME', \


# template for values to pass into pytests to call it multiple times
#@pytest.mark.parametrize(
#    'key, value', [
#('',''),
#         ]
#)   
