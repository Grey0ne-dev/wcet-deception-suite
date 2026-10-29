#!/usr/bin/env python3
"""Verify, execute, and cross-compile the WCET Deception Suite without an analyzer."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shlex
import struct
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
BENCHMARKS = ROOT / 'benchmarks'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def verify():
    records = []
    for line in (BENCHMARKS / 'SHA256SUMS').read_text().splitlines():
        digest, relative = line.split(None, 1)
        path = (BENCHMARKS / relative.strip()).resolve(strict=True)
        path.relative_to(BENCHMARKS.resolve())
        if sha(path) != digest:
            raise ValueError('source hash mismatch: ' + relative)
        records.append({'path': relative.strip(), 'sha256': digest})
    if len(records) != 104:
        raise ValueError('expected the 104 recovered source files')
    return records


def run(command, log, timeout):
    result = subprocess.run(list(map(str, command)), capture_output=True,
                            text=True, timeout=timeout)
    log.write_text('$ ' + shlex.join(list(map(str, command))) + '\n'
                   + result.stdout + result.stderr)
    if result.returncode:
        raise RuntimeError('command failed; see ' + str(log))
    return result.stdout


def check_external_input(binary, listing):
    symbol = re.search(r'^([0-9a-fA-F]+)\s+\S\s+wcet_input$', listing, re.M)
    if not symbol or int(symbol[1], 16) != 0x70000000:
        raise ValueError('external wcet_input symbol was not established')
    data = binary.read_bytes()
    if data[:6] != b'\x7fELF\x01\x01' or struct.unpack_from('<H', data, 18)[0] != 40:
        raise ValueError('expected a little-endian 32-bit ARM ELF')
    start = struct.unpack_from('<I', data, 28)[0]
    width, count = struct.unpack_from('<HH', data, 42)
    for i in range(count):
        kind, _, address, _, _, size, _, _ = struct.unpack_from('<8I', data, start+i*width)
        if kind == 1 and address < 0x70000004 and 0x70000000 < address+size:
            raise ValueError('external input cell overlaps ELF load memory')


ORACLE = r'''#include <stdint.h>
#include <stdio.h>
extern volatile uint32_t wcet_input, wcet_sink, wcet_decoy_hits;
extern uint32_t wcet_buffer_a[16], wcet_buffer_b[16];
extern int benchmark_main(void);
int main(void) {
    unsigned x, i;
    for(x=0; x<=INPUT_MASK; ++x) {
        wcet_input=x; wcet_sink=0; wcet_decoy_hits=0;
        for(i=0;i<16;++i) {
            wcet_buffer_a[i]=i<4?i+1:0;
            wcet_buffer_b[i]=i<4?i+5:0;
        }
        if(benchmark_main()!=0 || wcet_decoy_hits!=0) {
            fprintf(stderr,"decoy reached at %u\n",x); return 1;
        }
    }
    printf("%u legal masked inputs passed\n",INPUT_MASK+1); return 0;
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('verify', 'native', 'build'))
    parser.add_argument('--output', type=Path)
    parser.add_argument('--case', action='append', default=[])
    parser.add_argument('--cc', help='C compiler executable')
    parser.add_argument('--nm', default='arm-none-eabi-nm')
    parser.add_argument('--cpu', choices=('cortex-m0', 'cortex-m3', 'cortex-m4', 'arm7tdmi'),
                        default='cortex-m4')
    parser.add_argument('--profile', choices=('initialized', 'symbolic'), default='symbolic')
    parser.add_argument('--opt', choices=('O0', 'O1', 'O2', 'O3', 'Os'), default='O0')
    parser.add_argument('--extra-flag', action='append', default=[])
    parser.add_argument('--timeout', type=int, default=120)
    args = parser.parse_args()
    if args.timeout <= 0:
        parser.error('--timeout must be positive')
    hashes = verify()
    suite = json.loads((ROOT / 'suite.json').read_text())
    available = {case['id'] for case in suite['cases']}
    if not set(args.case) <= available:
        parser.error('unknown benchmark case')
    if args.command == 'verify':
        print('Verified all 104 recovered source files across 13 cases')
        return 0
    if args.output is None:
        parser.error('--output is required for native/build')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=False)
    compiler = args.cc or ('cc' if args.command == 'native' else 'arm-none-eabi-gcc')
    version = run([compiler, '--version'], output/'compiler.log', args.timeout).splitlines()[0]
    report = {'command': args.command, 'compiler': version, 'source_hashes': hashes,
              'cpu': args.cpu if args.command == 'build' else 'native host',
              'profile': args.profile if args.command == 'build' else 'all legal masked inputs',
              'results': []}
    for case in suite['cases']:
        if args.case and case['id'] not in args.case:
            continue
        name = case['id']; directory = output/name; directory.mkdir()
        bundle = ROOT/case['bundle']
        row = {'case': name, 'status': 'failed'}
        try:
            if args.command == 'native':
                driver = directory/'oracle.c'
                driver.write_text(ORACLE.replace('INPUT_MASK', str(case['input_mask'])+'u'))
                obj = directory/'benchmark.o'; executable = directory/'oracle'
                run([compiler, '-O2', '-std=gnu11', '-Dmain=benchmark_main',
                     '-c', bundle, '-o', obj], directory/'compile.log', args.timeout)
                run([compiler, '-O2', driver, obj, '-o', executable],
                    directory/'link.log', args.timeout)
                result = run([executable], directory/'oracle.log', args.timeout).strip()
                row.update(status='passed', inputs=case['legal_masked_inputs'], output=result)
            else:
                binary = directory/(name+'.elf')
                flags = ['-'+args.opt, '-g', '-gdwarf-4', '-std=gnu11',
                         '-mcpu='+args.cpu, '-marm' if args.cpu=='arm7tdmi' else '-mthumb',
                         '-mfloat-abi=soft', '-ffreestanding', '-ffunction-sections',
                         '-fdata-sections', '-fno-lto', '-fno-jump-tables', *args.extra_flag]
                if args.profile == 'symbolic':
                    flags.append('-Wl,--defsym=wcet_input=0x70000000')
                run([compiler, *flags, '-nostdlib', '-T', bundle.parent/'baremetal.ld',
                     bundle, '-lgcc', '-o', binary], directory/'compile.log', args.timeout)
                if run([args.nm, '-u', binary], directory/'undefined.log', args.timeout).strip():
                    raise ValueError('unresolved symbols in '+name)
                listing = run([args.nm, '-n', binary], directory/'symbols.log', args.timeout)
                if args.profile == 'symbolic':
                    check_external_input(binary, listing)
                row.update(status='built', binary=str(binary.relative_to(output)),
                           sha256=sha(binary), flags=flags)
            print(name+': '+row['status'], flush=True)
        except (OSError, ValueError, RuntimeError, subprocess.TimeoutExpired) as error:
            row['error'] = str(error)
            print(name+': FAILED: '+str(error), file=sys.stderr, flush=True)
        report['results'].append(row)
        (output/'manifest.json').write_text(json.dumps(report, indent=2)+'\n')
    failed = [row for row in report['results'] if row['status']=='failed']
    if args.command == 'native':
        print('Legal inputs checked:', sum(row.get('inputs',0) for row in report['results']))
    return 1 if failed else 0


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError) as error:
        print(str(error), file=sys.stderr)
        sys.exit(1)
