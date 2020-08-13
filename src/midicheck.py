import re
import os
import subprocess

globalsTimestamp = 0
map = {}

effectXlate = {
    'Convol': 'Convolotron',
    'Opticaltrem': 'OpticalTrem',
    'Har': 'Harmonizer',
    'Rev': 'Reverb',
    'Distorsion': 'Distortion',
    'WhaWha': 'WahWah',
}

COMMENT_RX = re.compile(r'\s*// (.*)')
SEP_RX = re.compile(r'/|,\s*')
PARM_RX = re.compile(r'\s*(\S+)\s*=\s*(\d+)\s*,')
GLOBALS_FILE = '/home/mmuller/w2/rakarrack-plus/src/global.h'
CHANGEPAR_RX = re.compile(r'([\-\+ ])(\s*)efx_(\S+)->changepar\((\S+),(.*)')
PRESERVE_RX = re.compile(r'\+\s+preserve_parm\(FX_([^\s,]+), ([^\s,]+)\);')

def parseGlobal():
    inEnum = False
    effects = []
    gotParm = False
    for line in open(GLOBALS_FILE):
        if inEnum:
            m = COMMENT_RX.match(line)
            if m:
                if gotParm:
                    effects = SEP_RX.split(m.group(1).strip())
                else:
                    effects += SEP_RX.split(m.group(1).strip())
                gotParm = False
                continue

            m = PARM_RX.match(line)
            if m:
                parm = m.group(1)
                val = m.group(2)
                for effect in effects:
                    map[effect, val] = parm
                gotParm = True
                continue

            if line == '};\n':
                break

        else:
            if line == 'enum ParmIndex {\n':
                inEnum = True

def run():
    parseGlobal()

    # Initialize to the first values we use.
    effect = 'Main'
    parmEnum = 'Parm_Volume'

    lineNum = 1
    proc = subprocess.Popen(['git', 'diff', 'rkrMIDI.C'],
                            stdout=subprocess.PIPE,
                            bufsize=1)
    lines = iter(proc.stdout.readline, b'')
    for line in lines:
        line = line.decode()
        print(repr(line))
        m = CHANGEPAR_RX.match(line)
        if m:
            effectVar = m.group(3)
            effect = effectXlate.get(effectVar, effectVar)
            parm = m.group(4)

            if m.group(1) == '+':
                parmEnum = map[effect, numericParm]
                if parmEnum != parm:
                    print(f'expected enum of {parmEnum}, got {parm}')
                    raise Exception(f'failed at {lineNum}: {line}')
            else:
                numericParm = parm
            continue

        m = PRESERVE_RX.match(line)
        if m:
            if m.group(1) != effect and m.group(1) != 'Main':
                raise Exception(f'error {lineNum}: bad effect: {line}')
            if m.group(2) != parmEnum and \
               m.group(1) not in ('Main', 'EQ1', 'EQ2'):
                print(parmEnum)
                raise Exception(f'error {lineNum}: bad param: {line}')
        lineNum += 1

run()
