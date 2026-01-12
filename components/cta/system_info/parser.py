#!/usr/bin/env python

import csv

from pathlib import Path

script_path = Path(__file__).resolve()
script_dir = script_path.parent

line_names = [
    'RED_LINE',
    'BLUE_LINE',
    'GREEN_LINE',
    'BROWN_LINE',
    'PURPLE_LINE',
    'YELLOW_LINE',
    'PINK_LINE',
    'ORANGE_LINE',
]

class led:
    def __init__(self):
        self.type = ''
        self.next_station = ''
        self.prev_station = ''
        self.station = ''
        self.index = 0
        self.line = ''

class Stop:
    def __init__(self):
        self.next = ''
        self.prev = ''

class Line:
    def __init__(self):
        self.name = ''
        self.stations = dict[str, Stop]()

def parse_leds() -> dict[str, list[led]]:
    with open(script_dir.joinpath('leds.txt')) as f:
        lines = f.readlines()
    i = 0
    leds = dict()
    stations_seen = set[str]()
    cta_line = ''
    prev_station = ''
    for line in lines:
        [count_str, station] = line.split(',', 1)
        station = station.strip()
        count = int(count_str)
        if count < 0:
            stations_seen = set[str]()
            cta_line = station
            leds[cta_line] = list[led]()
        else:
            for _ in range(count):
                leds[cta_line].append(led())
                leds[cta_line][-1].type = 'rail'
                leds[cta_line][-1].index = i
                leds[cta_line][-1].line = cta_line
                leds[cta_line][-1].prev_station = prev_station
                leds[cta_line][-1].next_station = station
                i += 1
            if station not in stations_seen:
                leds[cta_line].append(led())
                leds[cta_line][-1].type = 'station'
                leds[cta_line][-1].index = i
                leds[cta_line][-1].line = cta_line
                leds[cta_line][-1].station = station
                stations_seen.add(station)
                i += 1
        prev_station = station
    return leds

def parse_line(filename : str, name: str) -> Line:
    line = Line()
    line.name = name
    with open(script_dir.joinpath(filename)) as f:
        stops = f.readlines()
    for i, stop in enumerate(stops):
        stop = stop.strip()
        line.stations[stop] = Stop()
        line.stations[stop].prev = stops[i - 1].strip()
        line.stations[stop].next = stops[(i + 1) % len(stops)].strip()
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

lines = dict[str, Line]()
for line in line_names:
    lines[line] = (parse_line(f'lines/{line}.txt', line))
leds = parse_leds()

station_indices = dict()
i = 0
station_led_map = dict[str, dict[str, int]]()
for line in line_names:
    station_led_map[line] = dict[str, int]()
with open(script_dir.joinpath('cta.csv')) as csvfile:
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
        for line, led_list in leds.items():
            for l in led_list:
                if l.type == 'station' and l.station == station_name:
                    led_map[l.line] = l.index
                    station_led_map[l.line][station_name] = l.index
        station_name_fixed = station_name[:station_name.index(' (')]
        print(f'{{ .id = {station_id}, .name = \"{station_name_fixed}\" .location = {{ {lat}f, {lon}f }}, .led_index = {{ ', end='')
        for line_index, index in led_map.items():
            print(f'[{line_index}] = {index}', end=', ')
        print('}},')

stop_station_map = dict[str, dict]()
with open(script_dir.joinpath('cta.csv')) as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        stop_station_map[row['STOP_NAME']] = row['STATION_DESCRIPTIVE_NAME']

stop_led_map = dict[str, dict[str, list[int]]]()
for line in line_names:
    stop_led_map[line] = dict[str, list[int]]()
with open(script_dir.joinpath('cta.csv')) as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        stop_name = row['STOP_NAME']
        stop_id = get_enum(stop_name)
        station = row['STATION_DESCRIPTIVE_NAME']
        station_idx = station_indices[station]

        line_info = [row['RED'], row['BLUE'],row['G'],row['BRN'],row['P'],row['Y'],row['Pnk'], row['O']]
        lines_info = get_line(line_info)
        line_flag = ''
        led_starts = dict()
        led_counts = dict()
        for i, line in enumerate(lines_info):
            if i > 0:
                line_flag += ' | '
            line_flag += 'FLAG_' + line
            led_counts[line] = 0
            led_starts[line] = -1
            try:
                prev = lines[line].stations[stop_name].prev
            except:
                continue
            prev_station = stop_station_map[prev]
            for l in leds[line]:
                if l.type == 'station':
                    continue
                if l.next_station == station and l.prev_station == prev_station:
                    if led_starts[line] == -1:
                        led_starts[line] = l.index
                    led_counts[line] += 1
                elif l.prev_station == station and l.next_station == prev_station:
                    if led_starts[line] < l.index:
                        led_starts[line] = l.index
                    led_counts[line] -= 1
            stop_led_map[line][stop_name] = [ led_starts[line], led_counts[line] ]
        print(f'[INDEX({stop_id})] = {{ .line = {line_flag}, .station = {station_idx}, .led = {{ ', end='')
        for line in lines_info:
            print(f'[{line}] = {{ .start = {led_starts[line]}, .count = {led_counts[line]} }}, ', end='')
        print('}},')

for line in line_names:
    print(f'[{line}] = {{')
    count = 0
    for stop in lines[line].stations:
        positive = stop_led_map[line][stop][1] > 0
        for i in range(abs(stop_led_map[line][stop][1])):
            if positive:
                index = stop_led_map[line][stop][0] + i
            else:
                index = stop_led_map[line][stop][0] - i
            count += 1
            if count % 16 == 0:
                end = '\n    '
            else:
                end = ''
            print(f'{index}, ', end=end)
        try:
            count += 1
            if count % 16 == 0:
                end = '\n    '
            else:
                end = ''
            print(f'{station_led_map[line][stop_station_map[stop]]}, ', end=end)
        except:
            count += 1
            if count % 16 == 0:
                end = '\n    '
            else:
                end = ''
            print(f'/* FIX ({stop}) */', end=end)
    print('},')

