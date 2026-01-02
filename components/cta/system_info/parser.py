#!/usr/bin/env python

import csv

line_names = [
    'RED_LINE',
    'BLUE_LINE',
    #'GREEN_LINE',
    #'BROWN_LINE',
    #'PURPLE_LINE',
    #'YELLOW_LINE',
    #'PINK_LINE',
    #'ORANGE_LINE',
]

class led:
    def __init__(self):
        self.type = ''
        self.next_station = ''
        self.prev_station = ''
        self.station = ''
        self.index = 0
        self.line = ''

class Station:
    def __init__(self):
        self.name = ''
        self.id = ''
        self.connections = list[str]()

class Line:
    def __init__(self):
        self.name = ''
        self.stations = list[Station]()
        self.directions = list[list[str]]()

def parse_leds() -> list[led]:
    with open('system_info/leds.txt') as f:
        lines = f.readlines()
    i = 0
    leds = list[led]()
    stations_seen = set[str]()
    cta_line = ''
    for line in lines:
        [count_str, station] = line.split(',', 1)
        station = station.strip()
        count = int(count_str)
        if count < 0:
            stations_seen = set[str]()
            cta_line = station
        else:
            for x in range(count):
                leds.append(led())
                leds[-1].type = 'rail'
                leds[-1].index = i
                leds[-1].line = cta_line
                i += 1
            if station not in stations_seen:
                leds.append(led())
                leds[-1].type = 'station'
                leds[-1].index = i
                leds[-1].line = cta_line
                leds[-1].station = station
                stations_seen.add(station)
                i += 1
    return leds

def parse_line(filename : str, name: str) -> Line:
    line = Line()
    line.name = name
    with open(filename) as f:
        stations = f.readlines()
    hints = stations.pop(0).strip()
    directions = hints.split('(')
    directions.pop(0)
    for d in directions:
        d = d.replace(')', '').replace('\"', '')
        line.directions.append(d.split(','))
    for station in stations:
        [name, rest] = station.split('->')
        line.stations.append(Station())
        name = name.strip().replace('\"', '')
        line.stations[-1].name = name
        rest_stations = rest.split('\"')
        for i in range(len(rest_stations)):
            if i % 2 == 1:
                line.stations[-1].connections.append(rest_stations[i])
    return line


def get_line(line_info : list[str]) -> list[str]:
    lines = list()
    for i in range(len(line_names)):
        if line_info[i] == 'true':
            lines.append(line_names[i])
    return lines

def get_enum(original : str) -> str:
    name = original.upper()
    name = name.replace('(', '')  \
               .replace(')', '')  \
               .replace(' ', '_') \
               .replace('-', '_') \
               .replace('\'', '') \
               .replace('/', '_') \
               .replace('.', '') \
               .replace(',', '') \
               .replace('_&', '') \
               .replace('__', '_') \
               .replace('__', '_') \
               .replace('_LINES', '') \
               .replace('_LINE', '') 
    if name[0] >= '0' and name[0] <= '9':
        name = '_' + name
    return name

lines = list[Line]()
for line in line_names:
    lines.append(parse_line(f'system_info/lines/{line}.txt', line))
leds = parse_leds()

station_indices = dict()
i = 0
with open('system_info/cta.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        station_name = row['STATION_DESCRIPTIVE_NAME']
        if station_name in station_indices:
            continue
        station_indices[station_name] = i
        i += 1
        station_id = get_enum(station_name)
        location = row['Location']
        location = location.replace('(', '').replace(')', '')
        coords = location.split(',')
        lat = coords[0].strip()
        lon = coords[1].strip()
        # Find LEDs with this station
        led_map = dict()
        for l in leds:
            if l.type == 'station' and l.station == station_name:
                led_map[l.line] = l.index
        print(f'{{ .id = {station_id}, .location = {{ {lat}f, {lon}f }}, .led_index = {{ ', end='')
        for line_index, index in led_map.items():
            print(f'[{line_index}] = {index}', end=', ')
        print('}}')

with open('system_info/cta.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        stop_id = get_enum(row['STOP_NAME'])
        station = row['STATION_DESCRIPTIVE_NAME']
        station_idx = station_indices[station]

        line_info = [row['RED'], row['BLUE'],row['G'],row['BRN'],row['P'],row['Y'],row['Pnk'], row['O']]
        lines_info = get_line(line_info)
        line_flag = ''
        for i, line in enumerate(lines_info):
            if i > 0:
                line_flag += ' | '
            line_flag += 'FLAG_' + line
        print(f'[INDEX({stop_id})] = {{ .id = {stop_id}, .line = {line_flag}, .station = {station_idx},')
