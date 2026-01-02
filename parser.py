#!/usr/bin/env python

import csv

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

def abc():
    with open('cta.csv') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            station_id = get_enum(row['STATION_DESCRIPTIVE_NAME'])
            stop_id = get_enum(row['STOP_NAME'])
            location = row['Location']
            location = location.replace('(', '').replace(')', '')
            coords = location.split(',')
            lat = coords[0].strip()
            lon = coords[1].strip()
            line_info = [row['RED'], row['BLUE'],row['G'],row['BRN'],row['P'],row['Y'],row['Pnk'], row['O']]
            line = get_line(line_info)
            print(f'[INDEX({stop_id})] = <.id = {stop_id}, .line = {line}, .station = < .id = {station_id}, .location = <{lat}, {lon}>>>,')

with open('cta.csv') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        station_id = get_enum(row['STATION_DESCRIPTIVE_NAME'])
        stop_id = get_enum(row['STOP_NAME'])
        location = row['Location']
        location = location.replace('(', '').replace(')', '')
        coords = location.split(',')
        lat = coords[0].strip()
        lon = coords[1].strip()
        line_info = [row['RED'], row['BLUE'],row['G'],row['BRN'],row['P'],row['Y'],row['Pnk'], row['O']]
        lines = get_line(line_info)
        if len(lines) > 1:
            print(f'{stop_id} shared between lines ', end='')
            for line in lines:
                print(line, end=',')
            print('\n')
