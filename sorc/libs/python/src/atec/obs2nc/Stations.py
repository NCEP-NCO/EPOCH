'''
Created on Jan 16, 2015

@author: hsoh
'''

import sys
import math
import traceback

from atec.db_config.DbConfig import DbConfig

MODULE_NAME = 'Stations'

class Station(object):
    def __init__(self, row):
        '''
        Constructor
        '''
        self.row = row

    def get_range_code(self):
        range_code = None
        if self.row:
            range_code = self.row[0]
            tmp_array = range_code.split('_')
            if (1 < len(tmp_array)) and (2 == len(tmp_array[1])):
                if tmp_array[1][0].isalpha() and tmp_array[1][1].isdigit():
                    range_code = '%s0%s' % (range_code[:-1], range_code[-1])
        return range_code
    
    def get_range_code_org(self):
        range_code = None
        if self.row:    range_code = self.row[0]
        return range_code
    
    def get_unique_site_id(self):
        range_code = None
        if self.row: range_code = self.row[1]
        return range_code
    
    def get_lat(self):
        lat = None
        if self.row: lat = self.row[2]
        return lat
    
    def get_lon(self):
        lon = None
        if self.row: lon = self.row[3]
        return lon
    
    def get_elevation(self):
        elev = None
        if self.row: elev = self.row[4]
        return elev
    
    def get_elevation_km(self):
        elev = self.get_elevation()
        if elev: elev = elev/1000
        return elev
    
    
    def get_count(self):
        count = 0
        if self.row: count = self.row[5]
        return count
    
    def to_string(self):
        return '  station: %s, %f, %f, %f' % (
                self.get_range_code(), self.get_lat(),
                self.get_lon(), self.get_elevation())
          
class Stations(object):
    '''
    classdocs
    '''
    MIR_TABLE_NAME = 'station_information'
    ARMADA_PROFILER_TABLE_NAME = 'wind-profiler_site-metadata'
    
    #SQL_SELECT_TEMPLATE = '%s %s %s' % (
    #    'SELECT range_code, unique_site_id, latitude, longitude, elevation, count(*)' 
    #    'FROM %s S JOIN %s O ON S.unique_site_id=O.unique_site_id'
    #    'WHERE %s GROUP BY range_code, latitude, longitude, elevation ORDER BY range_code'
    #    )
    SQL_GROUP_BY_TEMPLATE = 'GROUP BY %s, latitude, longitude, %s'
    SQL_ORDER_BY_TEMPLATE = 'ORDER BY %s'
    
    #SQL_SELECT_TEMPLATE = 'SELECT S.%s, S.%s, latitude, longitude, %s, count(*) FROM %s %s'
    SQL_SELECT_TEMPLATE = 'SELECT S.%s, S.%s, latitude, longitude, %s FROM %s %s'
    SQL_TEMPLATE_JOIN_MIR = '`%s` S JOIN `%s` O ON S.unique_site_id=O.unique_site_id'
    SQL_TEMPLATE_JOIN_ARMADA = '`%s` S JOIN `%s` O ON S.site_id=O.site_id AND S.unit_id=O.unit_id AND date_time_stop > NOW()'
    
    EPSILON = 0.01
    
    #SQL_SELECT_TEMPLATE_MIR = ''' 
    #    SELECT range_code, S.unique_site_id, latitude, longitude, elevation, count(*)
    #    FROM `%s` S JOIN `%s` O ON S.unique_site_id=O.unique_site_id WHERE %s'''
    
    #SQL_SELECT_TEMPLATE_ARMADA_PROFILER = ''' 
    #    SELECT S.site_id, S.unit_id, latitude, longitude, surface_altitude, count(*)
    #    FROM `%s` S JOIN `%s` O ON S.site_id=O.site_id AND S.unit_id=O.unit_id AND date_time_stop > NOW() WHERE %s'''

    
    def __init__(self, cursor):
        '''
        Constructor
        '''
        self.cursor = cursor
        self.station_list = []
    
    @staticmethod
    def get_epsilon():
        return Stations.EPSILON
    @staticmethod
    def set_epsilon(epsilon):
        Stations.EPSILON = epsilon
    
    #def add_stations(self, station_array):
    #  debug = False
    #  debug = not debug
    #  if station_array is n ot None and 0 < len(station_array):
    #    for station in station_array:
    #      self.station_list.append(station)

    def add_stations(self, station_table_name, id_pattern=None, station_ids=None, 
            common_col_name=None, unique_col_name=None,
            elevation_col_name=None, is_exclusive_list=True, l_debug=False):
        debug = False
        #debug = not debug
        debug = debug or l_debug
        method_name = '%s.%s()' % (MODULE_NAME,'add_stations')

        if station_table_name is None:
            station_table_name = Stations.MIR_TABLE_NAME
        if debug:
            print('%s table_name: %s' % (method_name, station_table_name))
            if station_ids is not None:
                print('%s is exclusive list: %r' % (method_name, is_exclusive_list))
                print('%s station_ids: %r' % (method_name, station_ids))
        
        (common_col_name, unique_col_name, elevation_column_name, sql_range_code) = self.set_default_names(
                station_table_name, id_pattern,
                common_col_name, unique_col_name, elevation_col_name)
                
        if debug:   print('%s common_col: %s, unique_col: %s, elevation_column: %s' % (
                method_name, common_col_name, unique_col_name, elevation_column_name))
        
        sql_order_by = Stations.SQL_ORDER_BY_TEMPLATE % (common_col_name)
        sql_where = sql_range_code
        if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
            sql_where = '%s AND date_time_stop>NOW()' % sql_where
            
        if 0 < len(sql_where):  sql_where = 'WHERE %s' % (sql_where)
            
        station_table_name_A = '`%s` S' % (station_table_name)
        sql_where_and_more   = "%s %s" % (sql_where, sql_order_by)
        sql_select = Stations.SQL_SELECT_TEMPLATE % (common_col_name,
                unique_col_name, elevation_column_name,
                station_table_name_A, sql_where_and_more)
        #if debug :  print ('sql_select: %s' % (sql_select))
        return self.add_stations_by_sql(sql_select, id_pattern, station_ids,
                is_exclusive_list)
    
    def add_stations_with_real_data(self, station_table_name, id_pattern=None,
            station_ids=None, obs_table_name=None, interval_days=2, 
            common_col_name=None, unique_col_name=None,
            elevation_col_name=None, obs_time_column=None,
            is_exclusive_list=True, l_debug=False):
        debug = False
        #debug = not debug
        debug = debug or l_debug
        method_name = '%s.%s()' % (MODULE_NAME,'add_stations_with_real_data')

        if station_table_name is None:
            station_table_name = Stations.MIR_TABLE_NAME
        if debug:   print('%s table_name: %s' % (method_name, station_table_name))
        
        (common_col_name, unique_col_name, elevation_column_name, sql_range_code) = self.set_default_names(
                station_table_name, id_pattern,
                common_col_name, unique_col_name, elevation_col_name)
                
        if debug:   print('%s common_col: %s, unique_col: %s, elevation_column: %s' % (
                method_name, common_col_name, unique_col_name, elevation_column_name))
        
        sql_where = ''
        sql_group_by = Stations.SQL_GROUP_BY_TEMPLATE % (
                common_col_name, elevation_column_name)
        sql_order_by = Stations.SQL_ORDER_BY_TEMPLATE % (common_col_name)
        if obs_table_name is None:
            sql_join_table = '`%s` S' % (station_table_name)
            sql_where = sql_range_code
        else:
            if obs_time_column is None:
                if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
                    obs_time_column = 'date_time'
                else:
                    obs_time_column = 'obs_time'
        
            sql_min_date = 'DATE_ADD(NOW(), INTERVAL -%d DAY)' % (interval_days)
            if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
                sql_join_template = Stations.SQL_TEMPLATE_JOIN_ARMADA
            else:
                sql_join_template = Stations.SQL_TEMPLATE_JOIN_MIR
                
            #if id_pattern is None:
            #    sql_range_code = 'S.%s IS NOT NULL' % (target_column_name)
            #else:
            #    sql_range_code = "S.%s LIKE '%s%%'" % (target_column_name, id_pattern)
            sql_join_table = sql_join_template % (station_table_name, obs_table_name)
            sql_where = "%s AND O.%s>%s" % (sql_range_code, obs_time_column, sql_min_date)
            
        if 0 < len(sql_where):  sql_where = 'WHERE %s' % (sql_where)
            
        sql_where_and_more = "%s %s %s" % (sql_where, sql_group_by, sql_order_by)
        sql_select = Stations.SQL_SELECT_TEMPLATE % (common_col_name,
                unique_col_name, elevation_column_name,
                sql_join_table, sql_where_and_more)
        #if debug :  print ('sql_select: %s' % (sql_select))
        return self.add_stations_by_sql(sql_select, id_pattern, station_ids, is_exclusive_list)
    
    def add_stations_by_sql(self, sql_select, id_pattern=None,
            station_ids=None, is_exclusive_list=True):
        debug = True
        debug = not debug
        method_name = '%s.%s()' % (MODULE_NAME,'add_stations_by_sql')

        if debug :  print ('sql_select: %s' % (sql_select))
        try:
            self.cursor.execute( sql_select )
            rows = self.cursor.fetchall()
            new_range_codes = []
            range_codes = self.get_range_codes()
            if rows is not None:
                for row in rows:
                    station = Station(row)
                    #print (station.get_range_code(), station.get_lat(), station.get_lon(), station.get_elevation())
                    range_code = station.get_range_code()
                    #include_me = False
                    include_me = True
                    if station_ids is not None:
                        if range_code in range_codes or range_code in new_range_codes:
                            include_me = False
                        else:
                            is_exist_in_list = self.exist_in_list(range_code,
                                    station_ids, id_pattern)
                            include_me = not (is_exist_in_list == is_exclusive_list)
                            #if debug:   print('%s is_exist_in_list: %r ==> include_me: %r' % (
                            #        method_name, is_exist_in_list, include_me))

                            #if (not self.exist_in_list(range_code, station_ids, id_pattern)):
                            #    if range_code not in range_codes and range_code not in new_range_codes:
                            #        self.station_list.append(station)
                            #        new_range_codes.append(range_code)
                    #else:
                    #    include_me = True
                    #if debug:
                    #    print('%s include_me: %r' % (method_name, include_me))
                    if include_me:
                        self.station_list.append(station)
                        new_range_codes.append(range_code)
        except:
            ex = sys.exc_info()[0]
            print('=====Error at %s sql: %r \n' % (method_name, sql_select))
            print('%s except: %r' % (method_name, ex))
            traceback.print_exc()
        
        if 0 == len(self.station_list):
            print('%s Empty stations, sql: %s' % (method_name, sql_select))
        return self.station_list
    
    def exist_in_list(self, range_code, station_ids, id_pattern):
        exist = False
        #method_name = '     %s.%s' % (MODULE_NAME, 'exist_in_list()')
        #print (' check range_code: %s, id_list[0]: %s' % (range_code, id_list[0]))
        if station_ids is None or 0 == len(station_ids) or id_pattern is None:
            exist = True
        else:
            range_code_l = range_code.lower()
            for station_id in station_ids:
                #print ('%s check range_code: %s, id: %s' % (method_name, range_code, station_id))
                if station_id.lower() == range_code_l:
                    exist = True
                    break
                else:
                    tmp_id = '%s%s' % (id_pattern, station_id)
                    if tmp_id == range_code:
                        exist = True
                        break
        
        #print ('%s check range_code: %s, exist: %r' % (method_name, range_code, exist))
        return exist
      
    def get_elevations(self):
        elevations = []
        #for index, station in enumerate(self.station_list):
        for station in self.station_list:
            elevations.append(station.get_elevation())
        return elevations
    
    def get_elevations_km(self):
        elevations = []
        #for index, station in enumerate(self.station_list):
        for station in self.station_list:
            elevations.append(station.get_elevation_km())
        return elevations
    
    def get_latitudes(self):
        latitudes = []
        #for index, station in enumerate(self.station_list):
        for station in self.station_list:
            latitudes.append(station.get_lat())
        return latitudes
    
    def get_longitudes(self):
        longitudes = []
        #for index, station in enumerate(self.station_list):
        for station in self.station_list:
            longitudes.append(station.get_lon())
        return longitudes
    
    def get_range_code(self, unique_site_id):
        range_code = None
        station = self.get_station_by_unique_site_id(unique_site_id)
        if station is not None:
            range_code = station.get_range_code()
            
        return range_code
    
    def get_range_codes(self):
        range_codes = []
        #for index, station in enumerate(self.station_list):
        for station in self.station_list:
            range_codes.append(station.get_range_code())
        return range_codes
    
    def get_station_by_range_code(self, range_code):
        station = None
        station_index = self.get_station_index(range_code)
        if 0 <= station_index: station = self.station_list[station_index]
        return station
    
    def get_station_by_unique_site_id(self, unique_site_id):
        station = None
        station_index = self.get_station_index_by_unique_site_id(unique_site_id)
        if 0 <= station_index: station = self.station_list[station_index]
        return station
    
    def get_station_count(self):
        return len(self.station_list)
    
    # id_list = ('dpg_s01', 'dpg_s02', 'dpg_s03')
    # obs_table_name='sams_surface_data'
    # id_pattern = 'dpg_s'
    def get_station_list(self):
        return self.station_list
    
    def get_station_index(self, range_code):
        debug = False
        #debug = True
        #
        #method_name = '     %s.%s()' % (MODULE_NAME, 'get_station_index')
        method_name = '     get_station_index()'
        station_index = -1
        if self.station_list:
            for index, station in enumerate(self.station_list):
                if debug: print ('%s range_code: %s' % (method_name, station.get_range_code()))
                if station.get_range_code() == range_code or station.get_range_code_org() == range_code:
                    station_index = index
                    break
                index = index + 1
        
        return station_index
    
    def get_station_index_by_unique_site_id(self, unique_site_id):
        debug = False
        #debug = True
        #
        #method_name = '     %s.%s()' % (MODULE_NAME, 'get_station_index')
        method_name = '     get_station_index()'
        station_index = -1
        if self.station_list:
            for index, station in enumerate(self.station_list):
                if debug: print ('%s range_code: %s' % (method_name, station.get_range_code()))
                if station.get_unique_site_id() == unique_site_id:
                    station_index = index
                    break
                index = index + 1
        
        return station_index
    
    def get_unique_site_id(self, range_code):
        unique_site_id = None
        #method_name = '     %s.%s()' % (MODULE_NAME, 'get_unique_site_id')
        station = self.get_station_by_range_code(range_code)
        if station is not None:
            unique_site_id = station.get_unique_site_id()
        return unique_site_id
    
    def get_unique_site_id_by_lat_lon(self, lat, lon):
        unique_site_id = None
        epsilon = Stations.get_epsilon()
        for station in self.station_list:
            lat_diff = math.fabs(station.get_lat() - lat) 
            lon_diff = math.fabs(station.get_lon() - lon) 
            if lat_diff < epsilon and lon_diff < epsilon:
                unique_site_id = station.get_unique_site_id()
                break
        return unique_site_id
    
    def get_unique_site_id_by_unit_id(self, range_name, unit_id):
        unique_site_id = None
        station = self.get_station_by_range_code(range_name)
        if station is not None:
            unique_site_id = station.get_unique_site_id()
        return unique_site_id
    
    def set_default_names(self, station_table_name, id_pattern,
            common_col_name, unique_col_name, elevation_col_name):
        if common_col_name is None:
            if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
                common_col_name = 'site_id'
            else:
                common_col_name = 'range_code'
                
        if unique_col_name is None:
            if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
                unique_col_name = 'unit_id'
            else:
                unique_col_name = 'unique_site_id'
        if elevation_col_name is None:
            if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
                elevation_column_name = 'surface_altitude'
            else:
                elevation_column_name = 'elevation'
        #if debug:   print('%s common_col: %s, unique_col: %s, elevation_column: %s' % (method_name, common_col_name, unique_col_name, elevation_column_name))
        
        if id_pattern is None:
            sql_range_code = 'S.%s IS NOT NULL' % (common_col_name)
        else:
            sql_range_code = "S.%s LIKE '%s%%'" % (common_col_name, id_pattern)
        
        if Stations.ARMADA_PROFILER_TABLE_NAME == station_table_name:
            sql_range_code = "%s AND date_time_stop > NOW()" % (sql_range_code)
        return (common_col_name, unique_col_name, elevation_column_name, sql_range_code)

    
def main():
    db_config = DbConfig()
    db_conn = db_config.get_db_connection('mir')
    cursor = db_config.get_db_cursor(db_conn)
    
    
    #id_list =('01', '02', '03')
    id_list =('dpg_p01', 'dpg_p02', 'dpg_p03', 'dpg_p05', 'dpg_p08', 'dpg_p10')
    siteInfo = Stations(cursor)
    stations = siteInfo.get_station_ids(id_list, 'pwids_data', "dpg_p")
    
    if stations:
        for station in stations:
            print ('station: %s' % (station.to_string()))
        print ('station count: %d' % (siteInfo.get_station_count()))
    
if __name__ == '__main__':
    main()
    print (' Done')